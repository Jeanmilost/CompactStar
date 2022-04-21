/****************************************************************************
 * ==> Renderer ------------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a Metal renderer                      *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2022, this file is part of the CompactStar Engine.  *
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
    m_SceneContext.m_fOnGetShader = OnGetShader;
}
//----------------------------------------------------------------------------
CSR_CallbackController::~CSR_CallbackController()
{}
//----------------------------------------------------------------------------
void CSR_CallbackController::ReleaseModel(CSR_Model* _Nullable pModel) const
{
    csrModelRelease(pModel, nullptr);
}
//----------------------------------------------------------------------------
void CSR_CallbackController::ReleaseScene(CSR_Scene* _Nullable pScene) const
{
    csrSceneRelease(pScene, nullptr);
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
//---------------------------------------------------------------------------
// CSR_MetalRenderer
//---------------------------------------------------------------------------
@implementation CSR_MetalRenderer
{
    CSR_CallbackController* m_pCallbackController;
    id<MTLCommandQueue>     m_pCommandQueue;
    CSR_MetalShader*        m_pShader;
    double                  m_AnimTime;
    std::size_t             m_Frame;
    std::size_t             m_MaxAnimFrame;
    CFTimeInterval          m_PreviousTime;

    // -- DEMO CODE -------------------------
    CSR_Matrix4             m_ModelMatrix;
    float                   m_Angle;
    // -- DEMO CODE END ---------------------

    // TODO add your values here
}
//---------------------------------------------------------------------------
@synthesize m_pScene;
//---------------------------------------------------------------------------
- (nonnull instancetype) initWithMetalKitView :(nonnull MTKView*)pView;
{
    self = [super initWithMetalKitView :pView];

    if (self)
    {
        // build and configure the basic game values
        m_pScene              = nil;
        m_pCallbackController = new CSR_CallbackController((__bridge void*)self);
        m_pCommandQueue       = nil;
        m_pShader             = nil;
        m_Angle               = 0.0f;
        m_AnimTime            = 0.0;
        m_Frame               = 0;
        m_MaxAnimFrame        = 101;
        m_PreviousTime        = CACurrentMediaTime();

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
        // change the scene background color
        pRenderPassDescriptor.colorAttachments[0].clearColor =
                MTLClearColorMake(double(m_pScene->m_Color.m_R),
                                  double(m_pScene->m_Color.m_G),
                                  double(m_pScene->m_Color.m_B),
                                  double(m_pScene->m_Color.m_A));

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
    [m_pRenderEncoder setRenderPipelineState:[self GetRenderPipeline:@"default"]];
}
//----------------------------------------------------------------------------
- (void* _Nullable) OnGetShader :(const void* _Nullable)pModel :(CSR_EModelType)type
{
    return (__bridge void*)m_pShader;
}

//----------------------------------------------------------------------------
- (void) CreateViewport :(float)w :(float)h
{
    // calculate matrix items
    const float zNear  = 0.01f;
    const float zFar   = 1000.0f;
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
    m_pShader = [[CSR_MetalShader alloc]init :pLibrary
                                             :@"csrVertexShader"
                                             :@"csrFragmentShader"];

    // create the render pipeline
    [self CreateRenderPipeline :pView :@"default" :m_pShader];

    // initialize the scene
    m_pScene = csrSceneCreate();

    // configure the scene background color
    m_pScene->m_Color.m_R = 0.08f;
    m_pScene->m_Color.m_G = 0.12f;
    m_pScene->m_Color.m_B = 0.17f;
    m_pScene->m_Color.m_A = 1.0f;

    // configure the scene view matrix
    csrMat4Identity(&m_pScene->m_ViewMatrix);

    // -- DEMO CODE -------------------------
    csrMat4Identity(&m_ModelMatrix);

    char* pFileName = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 0;
    vf.m_HasPerVertexColor = 1;

    CSR_VertexCulling vc;
    vc.m_Type = CSR_CT_None;
    vc.m_Face = CSR_CF_CCW;

    // get the resource file path
    [CSR_ObjectiveCHelper ResourceToFileName :@"Hello World" :@"obj" :&pFileName];

    // load the "Hello World!" model
    CSR_Model* pModel = csrWaveFrontOpen(pFileName,
                                         &vf,
                                         &vc,
                                         0,
                                         0,
                                         0,
                                         0);

    free(pFileName);

    [self CreateBufferFromModel :pModel :true];

    // add the model to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 0);
    csrSceneAddModelMatrix(m_pScene, pModel, &m_ModelMatrix);

    // place the model in the 3d world (update the matrix directly)
    m_ModelMatrix.m_Table[3][0] = -0.01f;
    m_ModelMatrix.m_Table[3][1] = -0.025f;
    // -- DEMO CODE END ---------------------

    // TODO add your initialization code here

    // create an uniform for this scene item
    if (pSceneItem->m_pMatrixArray && pSceneItem->m_pMatrixArray->m_Count)
        [self CreateUniform :pSceneItem->m_pMatrixArray->m_pItem[0].m_pData];
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
    // -- DEMO CODE -------------------------
    // create a point of view from an arcball
    CSR_ArcBall arcball;
    arcball.m_AngleX = 0.0f;
    arcball.m_AngleY = m_Angle;
    arcball.m_Radius = 0.35f;
    csrSceneArcBallToMatrix(&arcball, &m_pScene->m_ViewMatrix);

    // rotate the view around the model
    m_Angle = fmod((float)(m_Angle + elapsedTime), (float)(M_PI * 2.0f));
    // -- DEMO CODE END ---------------------

    // TODO add your scene animation code here
}
//----------------------------------------------------------------------------
- (void) DrawScene;
{
    // draw the scene
    m_pCallbackController->DrawScene(m_pScene);
}
//---------------------------------------------------------------------------
@end
//---------------------------------------------------------------------------
