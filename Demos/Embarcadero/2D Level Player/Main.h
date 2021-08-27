/****************************************************************************
 * ==> 2D level player -----------------------------------------------------*
 ****************************************************************************
 * Description : Player for 2D levels                                       *
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

// classes
#include "CSR_Level.h"
#include "CSR_LevelFile_XML.h"
#include "CSR_VCLHelper.h"

/**
* 2D level player main form
*@author Jean-Milost Reymond
*/
class TMainForm : public TForm
{
    __published:
        TApplicationEvents *aeEvents;

        void __fastcall FormShow(TObject* pSender);
        void __fastcall FormResize(TObject* pSender);
        void __fastcall aeEventsMessage(tagMSG& msg, bool& handled);

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
        * Called when a cubemap texture should be loaded
        *@param fileNames - textures file names to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        static GLuint OnLoadCubemap(const CSR_Level::IFileNames& fileNames);

        /**
        * Called when a texture should be loaded
        *@param fileName - textures file name to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        static GLuint OnLoadTexture(const std::string& fileName);

        /**
        * Called when a skin should be applied to a model
        *@param index - skin index (in case the model contains several skins)
        *@param pSkin - skin
        *@param[in, out] pCanRelease - if 1, the skin content may be released after the skin is applied
        */
        static void OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease);

        /**
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader shoudl be get
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
        * Called when scene begins
        *@param pScene - scene to begin
        *@param pContext - scene context
        */
        static void OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when scene ends
        *@param pScene - scene to end
        *@param pContext - scene context
        */
        static void OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext);

        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        static void OnDeleteTexture(const CSR_Texture* pTexture);

    private:
        std::auto_ptr<CSR_Level>         m_pLevel;
        std::auto_ptr<CSR_LevelFile_XML> m_pLevelFile;
        HDC                              m_hDC;
        HGLRC                            m_hRC;
        CSR_Sphere                       m_ViewSphere;
        std::string                      m_SceneDir;
        std::size_t                      m_FrameCount;
        int                              m_PrevOrigin;
        float                            m_Angle;
        float                            m_PosVelocity;
        float                            m_DirVelocity;
        double                           m_FPS;
        unsigned __int64                 m_StartTime;
        unsigned __int64                 m_PreviousTime;
        unsigned __int64                 m_PrevDesignerTime;
        bool                             m_Initialized;

        /**
        * Opens a level
        *@param fileName - level file name to open
        *@return true on success, otherwise false
        */
        bool OpenLevel(const std::string& fileName);

        /**
        * Closes a level
        */
        void CloseLevel();

        /**
        * Loads a texture
        *@param fileName - texture file name to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        GLuint LoadTexture(const std::string& fileName) const;

        /**
        * Loads a cubemap texture
        *@param fileNames - textures file names to load
        *@return texture identifier on the GPU, M_CSR_Error_Code on error
        */
        GLuint LoadCubemap(const CSR_Level::IFileNames fileNames) const;

        /**
        * Loads a sound
        *@param fileName - sound file name to load
        *@return loaded sound on success, otherwise NULL
        *@note The returned sound should be released once useless
        */
        CSR_Sound* LoadSound(const std::string& fileName) const;

        /**
        * Initializes the viewpoint
        *@param pMatrix - matrix from which the viewpoint should be initialized
        */
        void InitializeViewPoint(const CSR_Matrix4* pMatrix);

        /**
        * Creates a default scene
        */
        void CreateScene();

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
        * Called when the scene should be drawn
        *@param resize - if true, the scene should be repainted during a resize
        */
        void OnDrawScene(bool resize);

        /**
        * Called while application is idle
        *@param pSender - event sender
        *@param[in, out] done - if true, event is done and will no longer be called
        */
        void __fastcall OnIdle(TObject* pSender, bool& done);
};
extern PACKAGE TMainForm* MainForm;
#endif
