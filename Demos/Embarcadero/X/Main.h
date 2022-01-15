/****************************************************************************
 * ==> DirectX model demo --------------------------------------------------*
 ****************************************************************************
 * Description : DirectX (.x) model with bone animation demo                *
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

#ifndef MainH
#define MainH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.AppEvnts.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>

// std
#include <string>
#include <vector>
#include <map>

// compactStar engine
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Scene.h"

// classes
#include "CSR_VCLHelper.h"
#include "CSR_OpenGLHelper.h"

/**
* DirectX model demo main form
*@author Jean-Milost Reymond
*/
class TMainForm : public TForm
{
    __published:

        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - form owner
        */
        __fastcall TMainForm(TComponent* pOwner);

        /**
        * Destructor
        */
        virtual __fastcall ~TMainForm();

        /**
        * Called when a texture should be loaded
        *@param pTextureName - texture name to load
        *@return pixel buffer containing the loaded texture
        */
        static CSR_PixelBuffer* OnLoadTextureCallback(const char* pTextureName);

        /**
        * Called when a skin should be applied to a model
        *@param index - skin index (in case the model contains several skins)
        *@param pSkin - skin
        *@param[in, out] pCanRelease - if 1, the skin content may be released after the skin is applied
        */
        static void OnApplySkinCallback(size_t index, const CSR_Skin* pSkin, int* pCanRelease);

        /**
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader shoudl be get
        *@param type - model type
        *@return shader to use to draw the model, 0 if no shader
        *@note The model will not be drawn if no shader is returned
        */
        static void* OnGetShaderCallback(const void* pModel, CSR_EModelType type);

        /**
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        static void* OnGetIDCallback(const void* pKey);

        /**
        * Called when the X model indexes should be get
        *@param pX - X model for which the indexes should be get
        *@param[in, out] pAnimSetIndex - animation set index
        *@param[in, out] pFrameIndex - frame index
        */
        static void OnGetXIndexCallback(const CSR_X* pX, size_t* pAnimSetIndex, size_t* pFrameIndex);

        /**
        * Called when scene begins
        *@param pScene - scene to begin
        *@param pContext - scene context
        */
        static void OnSceneBeginCallback(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when scene ends
        *@param pScene - scene to end
        *@param pContext - scene context
        */
        static void OnSceneEndCallback(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        static void OnDeleteTextureCallback(const CSR_Texture* pTexture);

    private:
        HDC                          m_hDC;
        HGLRC                        m_hRC;
        CSR_Scene*                   m_pScene;
        CSR_SceneContext             m_SceneContext;
        CSR_OpenGLShader*            m_pShader;
        CSR_OpenGLHelper::IResources m_OpenGLResources;
        CSR_Matrix4                  m_ModelMatrix;
        std::string                  m_SceneDir;
        std::size_t                  m_FrameCount;
        int                          m_PrevOrigin;
        float                        m_Angle;
        double                       m_FPS;
        unsigned __int64             m_StartTime;
        unsigned __int64             m_PreviousTime;
        bool                         m_Initialized;

        /**
        * Loads a texture
        *@param fileName - texture file name to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        GLuint LoadTexture(const std::string& fileName) const;

        /**
        * Initializes the scene
        *@param w - scene width
        *@param h - scene height
        */
        void InitScene(int w, int h);

        /**
        * Deletes the scene
        */
        void DeleteScene();

        /**
        * Updates the scene
        *@param elapsedTime - elapsed time since last update, in milliseconds
        */
        void UpdateScene(float elapsedTime);

        /**
        * Draws the scene
        */
        void DrawScene();

        /**
        * Called when the scene should be drawn
        *@param resize - if true, the scene should be repainted during a resize
        */
        void OnDrawScene(bool resize);

        /**
        * Called when a texture should be loaded
        *@param pTextureName - texture name to load
        *@return pixel buffer containing the loaded texture
        */
        CSR_PixelBuffer* OnLoadTexture(const char* pTextureName);

        /**
        * Called when a skin should be applied to a model
        *@param index - skin index (in case the model contains several skins)
        *@param pSkin - skin
        *@param[in, out] pCanRelease - if 1, the skin content may be released after the skin is applied
        */
        void OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease);

        /**
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader shoudl be get
        *@param type - model type
        *@return shader to use to draw the model, 0 if no shader
        *@note The model will not be drawn if no shader is returned
        */
        void* OnGetShader(const void* pModel, CSR_EModelType type);

        /**
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        void* OnGetID(const void* pKey);

        /**
        * Called when the X model indexes should be get
        *@param pX - X model for which the indexes should be get
        *@param[in, out] pAnimSetIndex - animation set index
        *@param[in, out] pFrameIndex - frame index
        */
        void OnGetXIndex(const CSR_X* pX, size_t* pAnimSetIndex, size_t* pFrameIndex);

        /**
        * Called when scene begins
        *@param pScene - scene to begin
        *@param pContext - scene context
        */
        void OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when scene ends
        *@param pScene - scene to end
        *@param pContext - scene context
        */
        void OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        void OnDeleteTexture(const CSR_Texture* pTexture);

        /**
        * Called while application is idle
        *@param pSender - event sender
        *@param[in, out] done - if true, event is done and will no longer be called
        */
        void __fastcall OnIdle(TObject* pSender, bool& done);
};
extern PACKAGE TMainForm* MainForm;
#endif
