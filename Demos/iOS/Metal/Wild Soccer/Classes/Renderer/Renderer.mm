/****************************************************************************
 * ==> Renderer ------------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a Metal renderer                      *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2019, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#import "Renderer.h"

// std
#include <string>

// metal
#import <ModelIO/ModelIO.h>
#import "CSR_Renderer_Metal_Types.h"

// classes
#import "CSR_ObjectiveCHelper.h"

//----------------------------------------------------------------------------
// CSR_CallbackController
//----------------------------------------------------------------------------
void* CSR_CallbackController::m_pOwner = NULL;
//----------------------------------------------------------------------------
CSR_CallbackController::CSR_CallbackController()
{}
//----------------------------------------------------------------------------
CSR_CallbackController::CSR_CallbackController(void* _Nonnull pOwner)
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
void CSR_CallbackController::ReleaseModel(CSR_Model* _Nullable pModel) const
{
    csrModelRelease(pModel, OnDeleteTexture);
}
//----------------------------------------------------------------------------
void CSR_CallbackController::ReleaseScene(CSR_Scene* _Nullable pScene) const
{
    csrSceneRelease(pScene, OnDeleteTexture);
}
//----------------------------------------------------------------------------
void CSR_CallbackController::DrawScene(CSR_Scene* _Nullable pScene) const
{
    // draw the scene
    csrSceneDraw(pScene, &m_SceneContext);
}
//----------------------------------------------------------------------------
void* _Nullable CSR_CallbackController::OnGetShader(const void* _Nullable pModel, CSR_EModelType type)
{
    return [(__bridge id)m_pOwner OnGetShader :pModel :type];
}
//----------------------------------------------------------------------------
void* _Nullable CSR_CallbackController::OnGetID(const void* _Nullable pKey)
{
    return [(__bridge id)m_pOwner OnGetID :pKey];
}
//----------------------------------------------------------------------------
void CSR_CallbackController::OnDeleteTexture(const CSR_Texture* _Nullable pTexture)
{
    [(__bridge id)m_pOwner OnDeleteTexture :pTexture];
}
//---------------------------------------------------------------------------
// CSR_MetalRenderer
//---------------------------------------------------------------------------
@implementation CSR_MetalRenderer
{
    CSR_CallbackController*    m_pCallbackController;
    CSR_GameLogic*             m_pGameLogic;
    id<MTLCommandQueue>        m_pCommandQueue;
    CSR_MetalShader*           m_pShader;
    CSR_MetalShader*           m_pSkyboxShader;
    void*                      m_pLandscapeKey;
    int                        m_AlternateStep;
    float                      m_Angle;
    float                      m_Rotation;
    float                      m_StepTime;
    float                      m_StepInterval;
    float                      m_PosVelocity;
    float                      m_DirVelocity;
    float                      m_ControlRadius;
    CSR_Matrix4                m_LandscapeMatrix;
    CFTimeInterval             m_PreviousTime;
}
//---------------------------------------------------------------------------
@synthesize m_pScene;
//---------------------------------------------------------------------------
- (nonnull instancetype) initWithMetalKitView :(nonnull MTKView*)pView
                                              :(nonnull CSR_GameLogic*)pGameLogic;
{
    self = [super initWithMetalKitView :pView];

    if (self)
    {
        // build and configure the basic game values
        m_pScene              =  nil;
        m_pCallbackController =  new CSR_CallbackController((__bridge void*)self);
        m_pGameLogic          =  pGameLogic;
        m_pCommandQueue       =  nil;
        m_pShader             =  nil;
        m_pSkyboxShader       =  nil;
        m_pLandscapeKey       =  nil;
        m_AlternateStep       =  0;
        m_Angle               =  M_PI / -4.0f;
        m_Rotation            =  0.0f;
        m_StepTime            =  0.0f;
        m_StepInterval        =  300.0f;
        m_PosVelocity         = -10.0f;
        m_DirVelocity         =  15.0f;
        m_ControlRadius       =  40.0f;
        m_PreviousTime        =  CACurrentMediaTime();

        [self InitScene :pView];
    }
    
    return self;
}
//----------------------------------------------------------------------------
- (void) dealloc
{
    [self DeleteScene];
    
    if (m_pCallbackController)
        delete m_pCallbackController;

    // useless because using ARC
    //[super dealloc];
}
//---------------------------------------------------------------------------
- (void) drawInMTKView :(nonnull MTKView*)pView
{
    dispatch_semaphore_wait(m_Semaphore, DISPATCH_TIME_FOREVER);
    
    m_UniformBufferIndex = (m_UniformBufferIndex + 1) % g_ParallelBufferCount;
    
    id <MTLCommandBuffer> pCommandBuffer = [m_pCommandQueue commandBuffer];
    pCommandBuffer.label                 = @"CSR_DrawCommand";

    __block dispatch_semaphore_t block_sema = m_Semaphore;
    
    [pCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
     {
         dispatch_semaphore_signal(block_sema);
     }];
    
    // calculate time interval
    const CFTimeInterval now            =  CACurrentMediaTime();
    const double         elapsedTime    = (now - m_PreviousTime);
                         m_PreviousTime =  now;

    // update the scene content
    [self UpdateScene :elapsedTime];

    MTLRenderPassDescriptor* pRenderPassDescriptor = pView.currentRenderPassDescriptor;
    
    if (pRenderPassDescriptor != nil)
    {
        m_pRenderEncoder       = [pCommandBuffer renderCommandEncoderWithDescriptor:pRenderPassDescriptor];
        m_pRenderEncoder.label = @"CSR_RenderEncoder";

        [self DrawScene];
        
        [m_pRenderEncoder endEncoding];
        [pCommandBuffer presentDrawable:pView.currentDrawable];
        
        m_pRenderEncoder = nil;
    }
    
    [pCommandBuffer commit];
}
//---------------------------------------------------------------------------
- (void) mtkView :(nonnull MTKView*)pView drawableSizeWillChange:(CGSize)size
{
    // drawable size or orientation changed, rebuild the viewport
    [self CreateViewport :size.width :size.height];
}
//---------------------------------------------------------------------------
- (void) csrMetalShaderEnable :(const void* _Nullable)pShader
{
    // set the render pipeline state to use
    if (pShader == (__bridge const void * _Nullable)(m_pSkyboxShader))
    {
        [m_pRenderEncoder setRenderPipelineState:[self GetRenderPipeline:@"skybox"]];
        return;
    }

    [m_pRenderEncoder setRenderPipelineState:[self GetRenderPipeline:@"default"]];
}
//----------------------------------------------------------------------------
- (void* _Nullable) OnGetShader :(const void* _Nullable)pModel :(CSR_EModelType)type
{
    if (pModel == m_pScene->m_pSkybox)
        return (__bridge void*)m_pSkyboxShader;
    
    return (__bridge void*)m_pShader;
}
//----------------------------------------------------------------------------
- (void* _Nullable) OnGetID :(const void* _Nullable)pKey
{
    return (__bridge void*)[self GetTexture :pKey];
}
//----------------------------------------------------------------------------
- (void) OnDeleteTexture :(const CSR_Texture* _Nullable)pTexture
{}
//----------------------------------------------------------------------------
- (bool) LoadLandscapeFromBitmap :(const char* _Nullable)pFileName
{
    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    
    CSR_VertexCulling vc;
    vc.m_Type = CSR_CT_None;
    vc.m_Face = CSR_CF_CW;
    
    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;
    
    // create a model to contain the landscape
    CSR_Model* pModel = csrModelCreate();
    
    // succeeded?
    if (!pModel)
        return false;
    
    // load a default grayscale bitmap from which a landscape will be generated
    CSR_PixelBuffer* pBitmap = csrPixelBufferFromBitmapFile(pFileName);
    
    // succeeded?
    if (!pBitmap)
    {
        m_pCallbackController->ReleaseModel(pModel);
        return false;
    }
    
    // load the landscape mesh from the grayscale bitmap
    pModel->m_pMesh     = csrLandscapeCreate(pBitmap, 3.0f, 0.2f, &vf, &vc, &material, 0);
    pModel->m_MeshCount = 1;
    
    [self CreateBufferFromModel :pModel :false];

    csrPixelBufferRelease(pBitmap);
    
    csrMat4Identity(&m_LandscapeMatrix);
    
    // add the model to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, &m_LandscapeMatrix);
    
    // create an uniform for this scene item
    if (pSceneItem->m_pMatrixArray && pSceneItem->m_pMatrixArray->m_Count)
        [self CreateUniform :pSceneItem->m_pMatrixArray->m_pItem[0].m_pData];
    
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
}
//----------------------------------------------------------------------------
- (void) InitScene :(nonnull MTKView*)pView
{
    // configure the view
    pView.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
    pView.colorPixelFormat        = MTLPixelFormatBGRA8Unorm_sRGB;
    pView.sampleCount             = 1;

    // get the Metal library
    id<MTLLibrary> pLibrary = [m_pDevice newDefaultLibrary];

    // create the Metal main threaded command queue and the thread semaphore to use
    m_pCommandQueue = [m_pDevice newCommandQueue];

    // create, build and link the game shader
    m_pShader   = [[CSR_MetalShader alloc]init :pLibrary
                                               :@"csrVertexShader"
                                               :@"csrFragmentShader"];

    m_pSkyboxShader = [[CSR_MetalShader alloc]init :pLibrary
                                                   :@"csrSkyboxVertexShader"
                                                   :@"csrSkyboxFragmentShader"];

    // create the render pipelines
    [self CreateRenderPipeline :pView :@"default" :m_pShader];
    [self CreateRenderPipeline :pView :@"skybox"  :m_pSkyboxShader];

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
    
    char* pFileName = 0;
    
    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"level" :@"bmp" :&pFileName];
    
    // load the landscape
    if (![self LoadLandscapeFromBitmap :pFileName])
        @throw @"The landscape could not be loaded";
    
    free(pFileName);
    
    // get back the scene item containing the model
    CSR_SceneItem* pSceneItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);
    
    // found it?
    if (!pSceneItem)
        @throw @"The landscape was not found in the scene";
    
    NSURL* pUrl;
    
    // get the resources texture path
    pUrl = [[NSBundle mainBundle]URLForResource: @"soccer_grass" withExtension:@"bmp"];
    
    // load the landscape model texture
    if (![self CreateTexture :&((CSR_Model*)(pSceneItem->m_pModel))->m_pMesh[0].m_Skin.m_Texture :pUrl])
        @throw @"The landscape texture could not be loaded";
    
    CSR_VertexFormat vertexFormat;
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 1;
    vertexFormat.m_HasPerVertexColor = 1;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    
    CSR_Mesh* pMesh;

    // create the ball
    pMesh = csrShapeCreateSphere(m_pGameLogic.m_pBall->m_Geometry.m_Radius,
                                 20,
                                 20,
                                 &vertexFormat,
                                 0,
                                 &material,
                                 0);

    [self CreateBufferFromMesh :pMesh :false];

    // get the resource texture path
    pUrl = [[NSBundle mainBundle]URLForResource: @"soccer_ball" withExtension:@"bmp"];
    
    // load the ball model texture
    if (![self CreateTexture :&pMesh->m_Skin.m_Texture :pUrl])
        @throw @"The ball texture could not be loaded";
 
    // add the mesh to the scene
    pSceneItem = csrSceneAddMesh(m_pScene, pMesh, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pMesh, &m_pGameLogic.m_pBall->m_Matrix);
    
    // create an uniform for this scene item
    if (pSceneItem->m_pMatrixArray && pSceneItem->m_pMatrixArray->m_Count)
        [self CreateUniform :pSceneItem->m_pMatrixArray->m_pItem[0].m_pData];

    // configure the ball body
    m_pGameLogic.m_pBall->m_pKey        = pSceneItem->m_pModel;
    m_pGameLogic.m_pBall->m_Body.m_Mass = 0.3f;
    
    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasTexCoords      = 1;
    vertexFormat.m_HasPerVertexColor = 1;

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
    
    [self CreateBufferFromModel :pModel :false];

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

    // create an uniform for this scene item
    if (pSceneItem->m_pMatrixArray && pSceneItem->m_pMatrixArray->m_Count)
        [self CreateUniform :pSceneItem->m_pMatrixArray->m_pItem[0].m_pData];

    pUrl = [[NSBundle mainBundle]URLForResource: @"soccer_goal" withExtension:@"bmp"];
    
    if (![self CreateTexture :&pModel->m_pMesh->m_Skin.m_Texture :pUrl])
        @throw @"The goal texture could not be loaded";

    CSR_Box goalBox;
    
    // transform the goal bounding box in his local system coordinates
    csrMat4ApplyToVector(&m_pGameLogic.m_pGoal->m_Matrix, &pSceneItem->m_pAABBTree[0].m_pBox->m_Min, &goalBox.m_Min);
    csrMat4ApplyToVector(&m_pGameLogic.m_pGoal->m_Matrix, &pSceneItem->m_pAABBTree[0].m_pBox->m_Max, &goalBox.m_Max);
    
    // configure the goal
    m_pGameLogic.m_pGoal->m_pKey = pSceneItem->m_pModel;
    csrMathMin(goalBox.m_Min.m_X, goalBox.m_Max.m_X, &m_pGameLogic.m_pGoal->m_Bounds.m_Min.m_X);
    csrMathMin(goalBox.m_Min.m_Z, goalBox.m_Max.m_Z, &m_pGameLogic.m_pGoal->m_Bounds.m_Min.m_Y);
    csrMathMax(goalBox.m_Min.m_X, goalBox.m_Max.m_X, &m_pGameLogic.m_pGoal->m_Bounds.m_Max.m_X);
    csrMathMax(goalBox.m_Min.m_Z, goalBox.m_Max.m_Z, &m_pGameLogic.m_pGoal->m_Bounds.m_Max.m_Y );

    vertexFormat.m_HasNormal         = 0;
    vertexFormat.m_HasPerVertexColor = 1;
    vertexFormat.m_HasTexCoords      = 1;
    
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    
    // create the You Won surface
    pMesh = csrShapeCreateSurface(0.6f, 0.2f, &vertexFormat, 0, &material, 0);
    
    [self CreateBufferFromMesh :pMesh :false];
    
    // add the mesh to the scene
    pSceneItem = csrSceneAddMesh(m_pScene, pMesh, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pMesh, m_pGameLogic.m_pYouWonMatrix);
    
    // create an uniform for this scene item
    if (pSceneItem->m_pMatrixArray && pSceneItem->m_pMatrixArray->m_Count)
        [self CreateUniform :pSceneItem->m_pMatrixArray->m_pItem[0].m_pData];

    // get the resource texture path
    pUrl = [[NSBundle mainBundle]URLForResource: @"you_won" withExtension:@"bmp"];
    
    // load the you won model texture
    if (![self CreateTexture :&pMesh->m_Skin.m_Texture :pUrl])
        @throw @"The you won texture could not be loaded";
    
    // create the skybox
    m_pScene->m_pSkybox = csrSkyboxCreate(1.0f, 1.0f, 1.0f);
    
    // succeeded?
    if (!m_pScene->m_pSkybox)
        @throw @"Failed to create the skybox";
    
    [self CreateBufferFromMesh :m_pScene->m_pSkybox :false];

    // create an uniform for the skybox
    [self CreateSkyboxUniform];

    // get the resource texture path
    NSArray* pImageNames =
    @[[[NSBundle mainBundle]pathForResource:@"skybox_right_small"  ofType:@"bmp"],
      [[NSBundle mainBundle]pathForResource:@"skybox_left_small"   ofType:@"bmp"],
      [[NSBundle mainBundle]pathForResource:@"skybox_top_small"    ofType:@"bmp"],
      [[NSBundle mainBundle]pathForResource:@"skybox_bottom_small" ofType:@"bmp"],
      [[NSBundle mainBundle]pathForResource:@"skybox_front_small"  ofType:@"bmp"],
      [[NSBundle mainBundle]pathForResource:@"skybox_back_small"   ofType:@"bmp"]];

    // load the cubemap texture
    if (![self CreateCubemapTexture :&m_pScene->m_pSkybox->m_Skin.m_CubeMap :pImageNames])
        @throw @"The skybox texture could not be loaded";
}
//----------------------------------------------------------------------------
- (void) DeleteScene
{
    // delete the scene
    m_pCallbackController->ReleaseScene(m_pScene);
    m_pScene = nil;
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
//---------------------------------------------------------------------------
@end
//---------------------------------------------------------------------------
