/*****************************************************************************
 * ==> Wild soccer game demo ------------------------------------------------*
 *****************************************************************************
 * Description : Wild soccer game demo view controller                       *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#import "GameViewController.h"

// std
#include <memory>

// openGL
#import <OpenGLES/ES2/glext.h>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Collision.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_Renderer.h"
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Physics.h"

// classes
#import "CSR_ShaderHelper.h"
#import "CSR_ObjectiveCHelper.h"
#import "GameLogic.h"

// the compactStar engine should be compiled with an OpenGL renderer to run this demo
#ifndef CSR_USE_OPENGL
    #warning "OpenGL isn't the currently enabled library. Please enable the CSR_USE_OPENGL conditional define in CSR_Renderer.h, and use CSR_Renderer_OpenGL as renderer."
#endif

//----------------------------------------------------------------------------
// CSR_CallbackController
//----------------------------------------------------------------------------
void* CSR_CallbackController::m_pOwner = NULL;
//----------------------------------------------------------------------------
CSR_CallbackController::CSR_CallbackController()
{}
//----------------------------------------------------------------------------
CSR_CallbackController::CSR_CallbackController(void* pOwner)
{
    // set the global static owner to share between all the functions
    m_pOwner = pOwner;

    // configure the scene context
    csrSceneContextInit(&m_SceneContext);
    m_SceneContext.m_fOnGetShader     = OnGetShader;
    m_SceneContext.m_fOnGetID         = OnGetID;
    m_SceneContext.m_fOnDeleteTexture = OnDeleteTexture;
}
//----------------------------------------------------------------------------
CSR_CallbackController::~CSR_CallbackController()
{}
//----------------------------------------------------------------------------
void CSR_CallbackController::ReleaseModel(CSR_Model* pModel) const
{
    csrModelRelease(pModel, OnDeleteTexture);
}
//----------------------------------------------------------------------------
void CSR_CallbackController::ReleaseScene(CSR_Scene* pScene) const
{
    csrSceneRelease(pScene, OnDeleteTexture);
}
//----------------------------------------------------------------------------
void CSR_CallbackController::DrawScene(CSR_Scene* pScene) const
{
    // draw the scene
    csrSceneDraw(pScene, &m_SceneContext);
}
//----------------------------------------------------------------------------
void* CSR_CallbackController::OnGetShader(const void* pModel, CSR_EModelType type)
{
    return [(__bridge id)m_pOwner OnGetShader :pModel :type];
}
//----------------------------------------------------------------------------
void* CSR_CallbackController::OnGetID(const void* pKey)
{
    return [(__bridge id)m_pOwner OnGetID :pKey];
}
//----------------------------------------------------------------------------
void CSR_CallbackController::OnDeleteTexture(const CSR_Texture* pTexture)
{
    [(__bridge id)m_pOwner OnDeleteTexture :pTexture];
}
//----------------------------------------------------------------------------
// GameViewController
//----------------------------------------------------------------------------
@interface GameViewController()
{
    CSR_GameLogic*          m_pGameLogic;
    CSR_CallbackController* m_pCallbackController;
    CSR_Scene*              m_pScene;
    CSR_OpenGLShader*       m_pShader;
    CSR_OpenGLShader*       m_pSkyboxShader;
    void*                   m_pLandscapeKey;
    int                     m_AlternateStep;
    float                   m_Angle;
    float                   m_StepTime;
    float                   m_StepInterval;
    float                   m_PosVelocity;
    float                   m_DirVelocity;
    float                   m_ControlRadius;
    CSR_Matrix4             m_LandscapeMatrix;
    CSR_OpenGLID            m_ID[5];
    CFTimeInterval          m_PreviousTime;
}

@property (strong, nonatomic) EAGLContext* pContext;

/**
* Enables OpenGL
*/
- (void) EnableOpenGL;

/**
* Disables OpenGL
*/
- (void) DisableOpenGL;

/**
* Loads a landscape from a bitmap file
*@param pFileName - bitmap file name
*@return true on success, optherwise false
*/
- (bool) LoadLandscapeFromBitmap :(const char*)pFileName;

/**
* Creates the viewport
*@param w - viewport width
*@param h - viewport height
*/
- (void) CreateViewport :(float)w  :(float)h;

/**
* Initializes the scene
*/
- (void) InitScene;

/**
* Deletes the scene
*/
- (void) DeleteScene;

/**
* Updates the scene
*@param elapsedTime - elapsed time since last update, in milliseconds
*/
- (void) UpdateScene :(float)elapsedTime;

/**
* Draws the scene
*/
- (void) DrawScene;

/**
* Called when screen is long pressed
*@param pSender - recognizer that raised the event
*/
- (void) OnLongPress :(UIGestureRecognizer*)pSender;

@end
//----------------------------------------------------------------------------
@implementation GameViewController
//----------------------------------------------------------------------------
- (void) viewDidLoad
{
    [super viewDidLoad];

    m_pGameLogic          =  [[CSR_GameLogic alloc]init];
    m_pCallbackController =  new CSR_CallbackController((__bridge void*)self);
    m_pScene              =  nil;
    m_pShader             =  nil;
    m_pSkyboxShader       =  nil;
    m_pLandscapeKey       =  nil;
    m_AlternateStep       =  0;
    m_Angle               =  M_PI / -4.0f;
    m_StepTime            =  0.0f;
    m_StepInterval        =  300.0f;
    m_PosVelocity         = -10.0f;
    m_DirVelocity         =  15.0f;
    m_ControlRadius       =  40.0f;
    m_PreviousTime        =  CACurrentMediaTime();

    // add a long press gesture to the view
    UILongPressGestureRecognizer* pGestureRecognizer =
    [[UILongPressGestureRecognizer alloc]initWithTarget:self
                                                 action:@selector(OnLongPress:)];
    
    pGestureRecognizer.minimumPressDuration = 0;
    
    // add gesture recognizer to view
    [self.view addGestureRecognizer: pGestureRecognizer];

    [self EnableOpenGL];
    [self InitScene];
}
//----------------------------------------------------------------------------
- (void) dealloc
{
    [self DeleteScene];
    [self DisableOpenGL];
    
    if (m_pCallbackController)
        delete m_pCallbackController;

    if ([EAGLContext currentContext] == self.pContext)
        [EAGLContext setCurrentContext:nil];
}
//----------------------------------------------------------------------------
- (void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    
    if ([self isViewLoaded] && ([[self view]window] == nil))
    {
        self.view = nil;
        
        [self DeleteScene];
        [self DisableOpenGL];
        
        if ([EAGLContext currentContext] == self.pContext)
            [EAGLContext setCurrentContext:nil];
        
        self.pContext = nil;
    }
}
//----------------------------------------------------------------------------
- (BOOL) prefersStatusBarHidden
{
    return YES;
}
//----------------------------------------------------------------------------
- (void)glkView :(GLKView*)view drawInRect:(CGRect)rect
{
    // calculate time interval
    const CFTimeInterval now            =  CACurrentMediaTime();
    const double         elapsedTime    = (now - m_PreviousTime);
                         m_PreviousTime =  now;
    
    [self UpdateScene :elapsedTime];
    [self DrawScene];
}
//----------------------------------------------------------------------------
- (void) EnableOpenGL
{
    self.pContext = [[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!self.pContext)
        NSLog(@"Failed to create ES context");
    
    GLKView* pView            = (GLKView*)self.view;
    pView.context             = self.pContext;
    pView.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [EAGLContext setCurrentContext:self.pContext];
}
//----------------------------------------------------------------------------
- (void) DisableOpenGL
{
    [EAGLContext setCurrentContext:self.pContext];
}
//----------------------------------------------------------------------------
- (void*) OnGetShader :(const void*)pModel :(CSR_EModelType)type
{
    if (pModel == m_pScene->m_pSkybox)
        return m_pSkyboxShader;
    
    return m_pShader;
}
//----------------------------------------------------------------------------
- (void*) OnGetID :(const void*)pKey
{
    // iterate through resource ids
    for (std::size_t i = 0; i < 5; ++i)
        // found the texture to get?
        if (pKey == m_ID[i].m_pKey)
            return &m_ID[i];

    return 0;
}
//----------------------------------------------------------------------------
- (void) OnDeleteTexture :(const CSR_Texture*)pTexture
{
    // iterate through resource ids
    for (std::size_t i = 0; i < 3; ++i)
        // found the texture to delete?
        if (pTexture == m_ID[i].m_pKey)
        {
            // unuse the texture
            if (m_ID[i].m_UseCount)
                --m_ID[i].m_UseCount;
            
            // is texture no longer used?
            if (m_ID[i].m_UseCount)
                return;
            
            // delete the texture from the GPU
            if (m_ID[i].m_ID != M_CSR_Error_Code)
            {
                glDeleteTextures(1, (GLuint*)(&m_ID[i].m_ID));
                m_ID[i].m_ID = M_CSR_Error_Code;
            }
            
            return;
        }
}
//----------------------------------------------------------------------------
- (bool) LoadLandscapeFromBitmap :(const char*)pFileName
{
    CSR_Material      material;
    CSR_VertexCulling vc;
    CSR_VertexFormat  vf;
    CSR_Model*        pModel;
    CSR_PixelBuffer*  pBitmap;
    CSR_SceneItem*    pSceneItem;
    
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    
    vc.m_Type = CSR_CT_None;
    vc.m_Face = CSR_CF_CW;
    
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;
    
    // create a model to contain the landscape
    pModel = csrModelCreate();
    
    // succeeded?
    if (!pModel)
        return false;
    
    // load a default grayscale bitmap from which a landscape will be generated
    pBitmap = csrPixelBufferFromBitmapFile(pFileName);
    
    // succeeded?
    if (!pBitmap)
    {
        m_pCallbackController->ReleaseModel(pModel);
        return false;
    }
    
    // load the landscape mesh from the grayscale bitmap
    pModel->m_pMesh     = csrLandscapeCreate(pBitmap, 3.0f, 0.2f, &vf, &vc, &material, 0);
    pModel->m_MeshCount = 1;
    
    csrPixelBufferRelease(pBitmap);
    
    csrMat4Identity(&m_LandscapeMatrix);
    
    // add the model to the scene
    pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, &m_LandscapeMatrix);
    
    // succeeded?
    if (pSceneItem)
        pSceneItem->m_CollisionType = CSR_CO_Ground;
    
    // keep the key
    m_pLandscapeKey = pModel;
    
    return true;
}
//----------------------------------------------------------------------------
- (void) CreateViewport :(float)w :(float)h
{
    // calculate matrix items
    const float zNear  = 0.01f;
    const float zFar   = 100.0f;
    const float fov    = 45.0f;
    const float aspect = w / h;
    
    csrMat4Perspective(fov, aspect, zNear, zFar, &m_pScene->m_ProjectionMatrix);
    
    csrShaderEnable(m_pShader);
    
    // connect projection matrix to shader
    GLint projectionUniform = glGetUniformLocation(m_pShader->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(projectionUniform, 1, 0, &m_pScene->m_ProjectionMatrix.m_Table[0][0]);
}
//----------------------------------------------------------------------------
- (void) InitScene
{
    CSR_VertexFormat vertexFormat;
    CSR_Material     material;
    CSR_PixelBuffer* pPixelBuffer = 0;
    CSR_Mesh*        pMesh;
    CSR_SceneItem*   pSceneItem;
    
    // initialize the scene
    m_pScene = csrSceneCreate();
    
    // configure the scene background color
    m_pScene->m_Color.m_R = 0.45f;
    m_pScene->m_Color.m_G = 0.8f;
    m_pScene->m_Color.m_B = 1.0f;
    m_pScene->m_Color.m_A = 1.0f;
    
    // configure the scene ground direction
    m_pScene->m_GroundDir.m_X =  0.0f;
    m_pScene->m_GroundDir.m_Y = -1.0f;
    m_pScene->m_GroundDir.m_Z =  0.0f;
    
    // configure the scene view matrix
    csrMat4Identity(&m_pScene->m_ViewMatrix);

    std::string vertexShader   = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Texture);
    std::string fragmentShader = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Texture);
    
    // compile, link and use shader
    m_pShader = csrOpenGLShaderLoadFromStr(vertexShader.c_str(),
                                           vertexShader.length(),
                                           fragmentShader.c_str(),
                                           fragmentShader.length(),
                                           0,
                                           0);

    // succeeded?
    if (!m_pShader)
        @throw @"Failed to load the scene shader";
    
    csrShaderEnable(m_pShader);
    
    // get shader attributes
    m_pShader->m_VertexSlot   = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aVertices");
    m_pShader->m_ColorSlot    = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aColor");
    m_pShader->m_TexCoordSlot = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aTexCoord");
    m_pShader->m_TextureSlot  = glGetAttribLocation(m_pShader->m_ProgramID, "csr_sColorMap");
    
    // get the screen rect
    CGRect screenRect = [[UIScreen mainScreen]bounds];
    
    // create the viewport
    [self CreateViewport :screenRect.size.width :screenRect.size.height];

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);
    
    char* pFileName = 0;

    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"level" :@"bmp" :&pFileName];

    // load the landscape
    if (![self LoadLandscapeFromBitmap :pFileName])
        @throw @"The landscape could not be loaded";
    
    free(pFileName);

    // get back the scene item containing the model
    pSceneItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);
    
    // found it?
    if (!pSceneItem)
        @throw @"The landscape was not found in the scene";
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"soccer_grass" :@"bmp" :&pFileName];

    // load landscape texture
    pPixelBuffer       = csrPixelBufferFromBitmapFile(pFileName);
    m_ID[0].m_pKey     = &((CSR_Model*)(pSceneItem->m_pModel))->m_pMesh[0].m_Skin.m_Texture;
    m_ID[0].m_ID       = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);
    m_ID[0].m_UseCount = 1;
    
    free(pFileName);

    // landscape texture will no longer be used
    csrPixelBufferRelease(pPixelBuffer);
    
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasPerVertexColor = 1;
    vertexFormat.m_HasTexCoords      = 1;
    
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    
    // create the ball
    pMesh = csrShapeCreateSphere(m_pGameLogic.m_pBall->m_Geometry.m_Radius,
                                 20,
                                 20,
                                 &vertexFormat,
                                 0,
                                 &material,
                                 0);
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"soccer_ball" :@"bmp" :&pFileName];

    // load ball texture
    pPixelBuffer       = csrPixelBufferFromBitmapFile(pFileName);
    m_ID[1].m_pKey     = &pMesh->m_Skin.m_Texture;
    m_ID[1].m_ID       = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);
    m_ID[1].m_UseCount = 1;
    
    free(pFileName);

    // ball texture will no longer be used
    csrPixelBufferRelease(pPixelBuffer);
    
    // add the mesh to the scene
    pSceneItem = csrSceneAddMesh(m_pScene, pMesh, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pMesh, &m_pGameLogic.m_pBall->m_Matrix);
    
    // configure the ball particle
    m_pGameLogic.m_pBall->m_pKey        = pSceneItem->m_pModel;
    m_pGameLogic.m_pBall->m_Body.m_Mass = 0.3f;
    
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasPerVertexColor = 1;
    vertexFormat.m_HasTexCoords      = 1;
    
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"soccer_goal" :@"obj" :&pFileName];

    // create the goal
    CSR_Model* pModel = csrWaveFrontOpen(pFileName,
                                         &vertexFormat,
                                         0,
                                         &material,
                                         0,
                                         0,
                                         0);
    
    free(pFileName);

    CSR_Vector3 translation;
    translation.m_X =  0.0f;
    translation.m_Y =  1.375f;
    translation.m_Z = -1.75f;
    
    CSR_Matrix4 translationMatrix;
    
    // apply translation to goal
    csrMat4Translate(&translation, &translationMatrix);
    
    CSR_Vector3 axis;
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;
    
    CSR_Matrix4 ryMatrix;
    
    // apply rotation on y axis to goal
    csrMat4Rotate(M_PI, &axis, &ryMatrix);
    
    CSR_Vector3 factor;
    factor.m_X = 0.0025f;
    factor.m_Y = 0.0025f;
    factor.m_Z = 0.0025f;
    
    CSR_Matrix4 scaleMatrix;
    
    // apply scaling to goal
    csrMat4Scale(&factor, &scaleMatrix);
    
    CSR_Matrix4 buildMatrix;
    
    // build the goal model matrix
    csrMat4Multiply(&ryMatrix, &scaleMatrix, &buildMatrix);
    csrMat4Multiply(&buildMatrix, &translationMatrix, &m_pGameLogic.m_pGoal->m_Matrix);
    
    // add the model to the scene
    pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, &m_pGameLogic.m_pGoal->m_Matrix);
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"soccer_goal" :@"bmp" :&pFileName];

    // load goal texture
    pPixelBuffer       = csrPixelBufferFromBitmapFile(pFileName);
    m_ID[2].m_pKey     = &pMesh->m_Skin.m_Texture;
    m_ID[2].m_ID       = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);
    m_ID[2].m_UseCount = 1;
    
    free(pFileName);

    // goal texture will no longer be used
    csrPixelBufferRelease(pPixelBuffer);
    
    CSR_Box goalBox;
    
    // transform the goal bounding box in his local system coordinates
    csrMat4ApplyToVector(&m_pGameLogic.m_pGoal->m_Matrix, &pSceneItem->m_pAABBTree[0].m_pBox->m_Min, &goalBox.m_Min);
    csrMat4ApplyToVector(&m_pGameLogic.m_pGoal->m_Matrix, &pSceneItem->m_pAABBTree[0].m_pBox->m_Max, &goalBox.m_Max);
    
    // configure the goal
    m_pGameLogic.m_pGoal->m_pKey = pSceneItem->m_pModel;
    csrMathMin(goalBox.m_Min.m_X, goalBox.m_Max.m_X, &m_pGameLogic.m_pGoal->m_Bounds.m_Min.m_X);
    csrMathMin(goalBox.m_Min.m_Z, goalBox.m_Max.m_Z, &m_pGameLogic.m_pGoal->m_Bounds.m_Min.m_Y);
    csrMathMax(goalBox.m_Min.m_X, goalBox.m_Max.m_X, &m_pGameLogic.m_pGoal->m_Bounds.m_Max.m_X);
    csrMathMax(goalBox.m_Min.m_Z, goalBox.m_Max.m_Z, &m_pGameLogic.m_pGoal->m_Bounds.m_Max.m_Y);
    
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasPerVertexColor = 1;
    vertexFormat.m_HasTexCoords      = 1;
    
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    
    // create the You Won surface
    pMesh = csrShapeCreateSurface(0.6f, 0.2f, &vertexFormat, 0, &material, 0);
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"you_won" :@"bmp" :&pFileName];

    // load the You Won texture
    pPixelBuffer       = csrPixelBufferFromBitmapFile(pFileName);
    m_ID[3].m_pKey     = &pMesh->m_Skin.m_Texture;
    m_ID[3].m_ID       = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);
    m_ID[3].m_UseCount = 1;
    
    free(pFileName);

    // goal texture will no longer be used
    csrPixelBufferRelease(pPixelBuffer);
        
    // add the mesh to the scene
    pSceneItem = csrSceneAddMesh(m_pScene, pMesh, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pMesh, m_pGameLogic.m_pYouWonMatrix);
    
    vertexShader   = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Skybox);
    fragmentShader = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Skybox);
    
    // load the skybox shader
    m_pSkyboxShader = csrOpenGLShaderLoadFromStr(vertexShader.c_str(),
                                                 vertexShader.length(),
                                                 fragmentShader.c_str(),
                                                 fragmentShader.length(),
                                                 0,
                                                 0);

    // succeeded?
    if (!m_pSkyboxShader)
        @throw @"Failed to load the skybox shader";
    
    // enable the shader program
    csrShaderEnable(m_pSkyboxShader);
    
    // get shader attributes
    m_pSkyboxShader->m_VertexSlot  = glGetAttribLocation (m_pSkyboxShader->m_ProgramID, "csr_aVertices");
    m_pSkyboxShader->m_CubemapSlot = glGetUniformLocation(m_pSkyboxShader->m_ProgramID, "csr_sCubemap");
    
    // create the skybox
    m_pScene->m_pSkybox = csrSkyboxCreate(1.0f, 1.0f, 1.0f);
    
    // succeeded?
    if (!m_pScene->m_pSkybox)
    {
        // show the error message to the user
        printf("Failed to create the skybox.\n");
        return;
    }
    
    char* pCubemapFileNames[6];

    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_right_small"  :@"bmp" :&pCubemapFileNames[0]];
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_left_small"   :@"bmp" :&pCubemapFileNames[1]];
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_top_small"    :@"bmp" :&pCubemapFileNames[2]];
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_bottom_small" :@"bmp" :&pCubemapFileNames[3]];
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_front_small"  :@"bmp" :&pCubemapFileNames[4]];
    [CSR_ObjectiveCHelper ResourceToFileName :@"skybox_back_small"   :@"bmp" :&pCubemapFileNames[5]];

    // load the cubemap texture
    m_ID[4].m_pKey     = &m_pScene->m_pSkybox->m_Skin.m_CubeMap;
    m_ID[4].m_ID       = csrOpenGLCubemapLoad((const char**)pCubemapFileNames);
    m_ID[4].m_UseCount = 1;
    
    free(pCubemapFileNames[0]);
    free(pCubemapFileNames[1]);
    free(pCubemapFileNames[2]);
    free(pCubemapFileNames[3]);
    free(pCubemapFileNames[4]);
    free(pCubemapFileNames[5]);
}
//----------------------------------------------------------------------------
- (void) DeleteScene
{
    // delete the scene
    m_pCallbackController->ReleaseScene(m_pScene);
    m_pScene = nil;
    
    // delete scene shader
    csrOpenGLShaderRelease(m_pShader);
    m_pShader = nil;
    
    // delete skybox shader
    csrOpenGLShaderRelease(m_pSkyboxShader);
    m_pSkyboxShader = nil;
}
//----------------------------------------------------------------------------
- (void) UpdateScene :(float)elapsedTime
{
    float      angle;
    CSR_Sphere prevSphere;
    CSR_Plane  groundPlane;
    
    [m_pGameLogic ApplyPhysics :m_pScene :elapsedTime];
    
    // if screen isn't touched, do nothing
    if (!m_pGameLogic.m_pTouchOrigin->m_X || !m_pGameLogic.m_pTouchOrigin->m_Y)
        return;
    
    prevSphere = *m_pGameLogic.m_pViewSphere;
    
    // calculate the angle formed by the touch gesture x and y distances
    if ((m_pGameLogic.m_pTouchPosition->m_X < m_pGameLogic.m_pTouchOrigin->m_X || m_pGameLogic.m_pTouchPosition->m_Y < m_pGameLogic.m_pTouchOrigin->m_Y) &&
        !(m_pGameLogic.m_pTouchPosition->m_X < m_pGameLogic.m_pTouchOrigin->m_X && m_pGameLogic.m_pTouchPosition->m_Y < m_pGameLogic.m_pTouchOrigin->m_Y))
        angle = -atanf((m_pGameLogic.m_pTouchPosition->m_Y - m_pGameLogic.m_pTouchOrigin->m_Y) / (m_pGameLogic.m_pTouchPosition->m_X - m_pGameLogic.m_pTouchOrigin->m_X));
    else
        angle =  atanf((m_pGameLogic.m_pTouchPosition->m_Y - m_pGameLogic.m_pTouchOrigin->m_Y) / (m_pGameLogic.m_pTouchPosition->m_X - m_pGameLogic.m_pTouchOrigin->m_X));
    
    // calculate the possible min and max values for each axis
    float minX = m_pGameLogic.m_pTouchOrigin->m_X - (cosf(angle) * m_ControlRadius);
    float maxX = m_pGameLogic.m_pTouchOrigin->m_X + (cosf(angle) * m_ControlRadius);
    float minY = m_pGameLogic.m_pTouchOrigin->m_Y - (sinf(angle) * m_ControlRadius);
    float maxY = m_pGameLogic.m_pTouchOrigin->m_Y + (sinf(angle) * m_ControlRadius);
    
    // limit the touch gesture in a radius distance
    if (m_pGameLogic.m_pTouchPosition->m_X > maxX)
        m_pGameLogic.m_pTouchPosition->m_X = maxX;
    else
    if (m_pGameLogic.m_pTouchPosition->m_X < minX)
        m_pGameLogic.m_pTouchPosition->m_X = minX;

    if (m_pGameLogic.m_pTouchPosition->m_Y > maxY)
        m_pGameLogic.m_pTouchPosition->m_Y = maxY;
    else
    if (m_pGameLogic.m_pTouchPosition->m_Y < minY)
        m_pGameLogic.m_pTouchPosition->m_Y = minY;

    // calculate the final pos and dir velocity
    const float posVelocity = (m_PosVelocity * ((m_pGameLogic.m_pTouchPosition->m_Y - m_pGameLogic.m_pTouchOrigin->m_Y) / m_pGameLogic.m_pTouchOrigin->m_Y));
    const float dirVelocity = (m_DirVelocity * ((m_pGameLogic.m_pTouchPosition->m_X - m_pGameLogic.m_pTouchOrigin->m_X) / m_pGameLogic.m_pTouchOrigin->m_X));
    
    // calculate the next player direction
    m_Angle += dirVelocity * elapsedTime;
    
    // validate it
    if (m_Angle > M_PI * 2.0f)
        m_Angle -= M_PI * 2.0f;
    else
    if (m_Angle < 0.0f)
        m_Angle += M_PI * 2.0f;
    
    // calculate the next player position
    m_pGameLogic.m_pViewSphere->m_Center.m_X += posVelocity * cosf(m_Angle + (M_PI * 0.5f)) * elapsedTime;
    m_pGameLogic.m_pViewSphere->m_Center.m_Z += posVelocity * sinf(m_Angle + (M_PI * 0.5f)) * elapsedTime;
    
    // calculate the ground position and check if next position is valid
    if (![m_pGameLogic ApplyGroundCollision :m_pScene :m_pGameLogic.m_pViewSphere :m_Angle :&m_pScene->m_ViewMatrix :&groundPlane])
    {
        // invalid next position, get the scene item (just one for this scene)
        const CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);
        
        // found it?
        if (pItem)
        {
            // check if the x position is out of bounds, and correct it if yes
            if (m_pGameLogic.m_pViewSphere->m_Center.m_X <= pItem->m_pAABBTree->m_pBox->m_Min.m_X ||
                m_pGameLogic.m_pViewSphere->m_Center.m_X >= pItem->m_pAABBTree->m_pBox->m_Max.m_X)
                m_pGameLogic.m_pViewSphere->m_Center.m_X = prevSphere.m_Center.m_X;
            
            // do the same thing with the z position. Doing that separately for each axis will make
            // the point of view to slide against the landscape border (this is possible because the
            // landscape is axis-aligned)
            if (m_pGameLogic.m_pViewSphere->m_Center.m_Z <= pItem->m_pAABBTree->m_pBox->m_Min.m_Z ||
                m_pGameLogic.m_pViewSphere->m_Center.m_Z >= pItem->m_pAABBTree->m_pBox->m_Max.m_Z)
                m_pGameLogic.m_pViewSphere->m_Center.m_Z = prevSphere.m_Center.m_Z;
        }
        else
            // failed to get the scene item, just revert the position
            m_pGameLogic.m_pViewSphere->m_Center = prevSphere.m_Center;
        
        // recalculate the ground value (this time the collision result isn't tested, because the
        // previous position is always considered as valid)
        [m_pGameLogic ApplyGroundCollision :m_pScene :m_pGameLogic.m_pViewSphere :m_Angle :&m_pScene->m_ViewMatrix :&groundPlane];
    }
    else
    {
        float       groundAngle;
        CSR_Vector3 slopeDir;
        
        // get the slope direction
        slopeDir.m_X = groundPlane.m_A;
        slopeDir.m_Y = groundPlane.m_B;
        slopeDir.m_Z = groundPlane.m_C;
        
        // calculate the slope angle
        csrVec3Dot(&m_pScene->m_GroundDir, &slopeDir, &groundAngle);
        
        // is the slope too inclined to allow the player to walk on it?
        if (fabs(groundAngle) < 0.5f)
            // revert the position
            m_pGameLogic.m_pViewSphere->m_Center = prevSphere.m_Center;
        else
        if (m_pGameLogic.m_pViewSphere->m_Center.m_X >= m_pGameLogic.m_pGoal->m_Bounds.m_Min.m_X && m_pGameLogic.m_pViewSphere->m_Center.m_X <= m_pGameLogic.m_pGoal->m_Bounds.m_Max.m_X &&
            m_pGameLogic.m_pViewSphere->m_Center.m_Z >= m_pGameLogic.m_pGoal->m_Bounds.m_Min.m_Y &&
            m_pGameLogic.m_pViewSphere->m_Center.m_Z <= m_pGameLogic.m_pGoal->m_Bounds.m_Max.m_Y)
        {
            // find on which edge the player is hitting the goal
            switch ([m_pGameLogic GetGoalHitEdge :&prevSphere.m_Center])
            {
                case IE_E_Top:
                case IE_E_Bottom: m_pGameLogic.m_pViewSphere->m_Center.m_Z = prevSphere.m_Center.m_Z; break;

                case IE_E_Left:
                case IE_E_Right:  m_pGameLogic.m_pViewSphere->m_Center.m_X = prevSphere.m_Center.m_X; break;

                default:
                    m_pGameLogic.m_pViewSphere->m_Center.m_X = prevSphere.m_Center.m_X;
                    m_pGameLogic.m_pViewSphere->m_Center.m_Z = prevSphere.m_Center.m_Z;
                    break;
            }

            // recalculate the ground value (this time the collision result isn't tested, because
            // the previous position is always considered as valid)
            [m_pGameLogic ApplyGroundCollision :m_pScene :m_pGameLogic.m_pViewSphere :m_Angle :&m_pScene->m_ViewMatrix :&groundPlane];
        }
    }
    
    // calculate next time where the step sound should be played
    m_StepTime += (elapsedTime * 1000.0f);
    
    // count frames
    while (m_StepTime > m_StepInterval)
    {
        // do play the left or right footstep sound?
        if (!(m_AlternateStep % 2))
        {
            csrSoundStop(m_pGameLogic.m_pFootStepLeftSound);
            csrSoundPlay(m_pGameLogic.m_pFootStepLeftSound);
        }
        else
        {
            csrSoundStop(m_pGameLogic.m_pFootStepRightSound);
            csrSoundPlay(m_pGameLogic.m_pFootStepRightSound);
        }

        m_StepTime = 0.0f;
        
        // next time the other footstep sound will be played
        m_AlternateStep = (m_AlternateStep + 1) & 1;
    }
}
//----------------------------------------------------------------------------
- (void) DrawScene;
{
    CSR_Plane groundPlane;
    
    // finalize the view matrix
    [m_pGameLogic ApplyGroundCollision :m_pScene :m_pGameLogic.m_pViewSphere :m_Angle :&m_pScene->m_ViewMatrix :&groundPlane];
    
    // draw the scene
    m_pCallbackController->DrawScene(m_pScene);
}
//----------------------------------------------------------------------------
- (void)OnLongPress :(UIGestureRecognizer*)pSender
{
    const CGPoint touchPos = [pSender locationInView :nil];
    
    switch (pSender.state)
    {
        case UIGestureRecognizerStateBegan:
            // initialize the position. NOTE inverted because the screen is in landscape mode
            m_pGameLogic.m_pTouchOrigin->m_X   = touchPos.y;
            m_pGameLogic.m_pTouchOrigin->m_Y   = touchPos.x;
            m_pGameLogic.m_pTouchPosition->m_X = touchPos.y;
            m_pGameLogic.m_pTouchPosition->m_Y = touchPos.x;
            break;
            
        case UIGestureRecognizerStateChanged:
            // get the next position. NOTE inverted because the screen is in landscape mode
            m_pGameLogic.m_pTouchPosition->m_X = touchPos.y;
            m_pGameLogic.m_pTouchPosition->m_Y = touchPos.x;
            break;
            
        case UIGestureRecognizerStateEnded:
            if (m_pGameLogic.m_pTouchPosition->m_X == m_pGameLogic.m_pTouchOrigin->m_X && m_pGameLogic.m_pTouchPosition->m_Y == m_pGameLogic.m_pTouchOrigin->m_Y)
                [m_pGameLogic Shoot];

            // reset the position
            m_pGameLogic.m_pTouchOrigin->m_X   = 0;
            m_pGameLogic.m_pTouchOrigin->m_Y   = 0;
            m_pGameLogic.m_pTouchPosition->m_X = 0;
            m_pGameLogic.m_pTouchPosition->m_Y = 0;
            break;
            
        default:
            break;
    }
}
//----------------------------------------------------------------------------
@end
//----------------------------------------------------------------------------
