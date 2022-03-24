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

#import <MetalKit/MetalKit.h>

// compactStar engine
#include "CSR_Scene.h"
#include "CSR_Renderer_Metal.h"

// classes
#import "GameLogic.h"

// simd
#import <simd/simd.h>

/**
* A callback controller class, used to map the callbacks between the Objective-C
* and the C languages
*@author Jean-Milost Reymond
*/
class CSR_CallbackController
{
    public:
        /**
        * Constructor
        *@param pOwner - owner for this manager
        */
        CSR_CallbackController(void* _Nonnull pOwner);
    
        virtual ~CSR_CallbackController();
    
        /**
        * Releases a model
        *@param pModel - model to release
        */
        void ReleaseModel(CSR_Model* _Nullable pModel) const;
    
        /**
        * Releases a scene
        *@param pScene - scene to release
        */
        void ReleaseScene(CSR_Scene* _Nullable pScene) const;
    
        /**
        * Draws a scene
        *@param pScene - scene to draw
        */
        void DrawScene(CSR_Scene* _Nullable pScene) const;
    
    private:
        CSR_SceneContext      m_SceneContext;
        static void* _Nonnull m_pOwner;
    
        /**
        * Constructor
        *@note This constructor is prohibited, as the controller cannot exist without an owner
        */
        CSR_CallbackController();
    
        /**
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader should be get
        *@param type - model type
        *@return shader to use to draw the model, 0 if no shader
        *@note The model will not be drawn if no shader is returned
        */
        static void* _Nullable OnGetShader(const void* _Nullable pModel, CSR_EModelType type);
    
        /**
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        static void* _Nullable OnGetID(const void* _Nullable pKey);
    
        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        static void OnDeleteTexture(const CSR_Texture* _Nullable pTexture);
};

/**
* Platform iondependent Metal renderer
*@author Jean-Milost Reymond
*/
@interface CSR_MetalRenderer : CSR_MetalBasicRenderer
{
    CSR_Scene* m_pScene;
}

@property (nonatomic, assign) CSR_Scene* _Nullable m_pScene;

/**
* Initializes a Metal view
*@param pView - view above with the Metal library will draw
*@return metal view instance
*/
- (nonnull instancetype) initWithMetalKitView :(nonnull MTKView*)pView
                                              :(nonnull CSR_GameLogic*)pGameLogic;

/**
* Releases the class
*/
- (void) dealloc;

/**
* Called when a per frame drawing is required
*@param pView - view above with the Metal library will draw
*/
- (void) drawInMTKView :(nonnull MTKView*)pView;

/**
* Called when the view orientation or size is about to change
*@param pView - view for which orientation or size is about to change
*@param size - new size
*/
- (void) mtkView :(nonnull MTKView*)pView drawableSizeWillChange:(CGSize)size;

/**
* Enables a shader (i.e. notify that from now this shader will be used)
*@param pShader - shader to enable, disable any previously enabled shader if 0
*/
- (void) csrMetalShaderEnable :(const void* _Nullable)pShader;

/**
* Called when a shader should be get for a model
*@param pModel - model for which the shader should be get
*@param type - model type
*@return shader to use to draw the model, 0 if no shader
*@note The model will not be drawn if no shader is returned
*/
- (void* _Nullable) OnGetShader :(const void* _Nullable)pModel :(CSR_EModelType)type;

/**
* Called when a resource identifier should be get from a key
*@param pKey - key for which the resource identifier should be get
*@return identifier, 0 on error or if not found
*/
- (void* _Nullable) OnGetID :(const void* _Nullable)pKey;

/**
* Called when a texture should be deleted
*@param pTexture - texture to delete
*/
- (void) OnDeleteTexture :(const CSR_Texture* _Nullable)pTexture;

/**
* Loads a landscape from a bitmap file
*@param pFileName - bitmap file name
*@return true on success, optherwise false
*/
- (bool) LoadLandscapeFromBitmap :(const char* _Nullable)pFileName;

/**
* Creates the viewport
*@param w - viewport width
*@param h - viewport height
*/
- (void) CreateViewport :(float)w :(float)h;

/**
* Initializes the scene
*“param pView - the Metal view on which the scene will be drawn
*/
- (void) InitScene :(nonnull MTKView*)pView;

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

@end
