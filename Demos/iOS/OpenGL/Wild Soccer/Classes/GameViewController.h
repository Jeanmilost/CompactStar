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

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

// compactStar engine
#include "CSR_Texture.h"
#include "CSR_Scene.h"

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
        CSR_CallbackController(void* pOwner);

        virtual ~CSR_CallbackController();

        /**
        * Releases a model
        *@param pModel - model to release
        */
        void ReleaseModel(CSR_Model* pModel) const;

        /**
        * Releases a scene
        *@param pScene - scene to release
        */
        void ReleaseScene(CSR_Scene* pScene) const;
    
        /**
        * Draws a scene
        *@param pScene - scene to draw
        */
        void DrawScene(CSR_Scene* pScene) const;

        /**
        * Constructor
        *@param elapsedTime - elapsed time since last task was processed
        *@param pItems - task items
        */
        //virtual void ExecuteTasks(float elapsedTime, MINI_BotItem* pItems);

    private:
        CSR_SceneContext m_SceneContext;
        static void*     m_pOwner;

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
        static void* OnGetShader(const void* pModel, CSR_EModelType type);

        /**
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        static void* OnGetID(const void* pKey);

        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        static void OnDeleteTexture(const CSR_Texture* pTexture);
};

/**
* Game view controller, contains the application main view and his associated events
*@author Jean-Milost Reymond
*/
@interface GameViewController : GLKViewController
{}

/**
* Called when a shader should be get for a model
*@param pModel - model for which the shader should be get
*@param type - model type
*@return shader to use to draw the model, 0 if no shader
*@note The model will not be drawn if no shader is returned
*/
- (void*) OnGetShader :(const void*)pModel :(CSR_EModelType)type;

/**
* Called when a resource identifier should be get from a key
*@param pKey - key for which the resource identifier should be get
*@return identifier, 0 on error or if not found
*/
- (void*) OnGetID :(const void*)pKey;

/**
* Called when a texture should be deleted
*@param pTexture - texture to delete
*/
- (void) OnDeleteTexture :(const CSR_Texture*)pTexture;

@end
