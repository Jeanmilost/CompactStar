/****************************************************************************
 * ==> Tic-tac-toe game ----------------------------------------------------*
 ****************************************************************************
 * Description : A complete tic-tac-toe game using a mimimax algorithm      *
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

// compactStar engine
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Scene.h"
#include "CSR_Sound.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_VCLHelper.h"

/**
* Tic-tac-toe game main form
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
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader shoudl be get
        *@param type - model type
        *@return shader to use to draw the model, 0 if no shader
        *@note The model will not be drawn if no shader is returned
        */
        static void* OnGetShaderCallback(const void* pModel, CSR_EModelType type);

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
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        static void* OnGetIDCallback(const void* pKey);

        /**
        * Called when a texture should be deleted
        *@param pTexture - texture to delete
        */
        static void OnDeleteTextureCallback(const CSR_Texture* pTexture);

    private:
        /**
        * Player or game phase type
        */
        enum IEPlayer
        {
            IE_PL_Computer      = -1,
            IE_PL_None          =  0,
            IE_PL_Player        =  1,
            IE_PL_SkipRendering =  2
        };

        /**
        * Pawn type a cell may receive
        */
        enum IEPawn
        {
            IE_PA_None,
            IE_PA_Round,
            IE_PA_Cross
        };

        /**
        * Playfield cell
        */
        struct ICell
        {
            unsigned    m_X;
            unsigned    m_Y;
            IEPawn      m_Pawn;
            CSR_Vector2 m_Pos;
            CSR_Matrix4 m_Matrix_Cross;
            CSR_Matrix4 m_Matrix_Round;
        };

        HDC                          m_hDC;
        HGLRC                        m_hRC;
        ALCdevice*                   m_pOpenALDevice;
        ALCcontext*                  m_pOpenALContext;
        CSR_Scene*                   m_pScene;
        CSR_SceneContext             m_SceneContext;
        CSR_OpenGLShader*            m_pShader;
        CSR_OpenGLHelper::IResources m_OpenGLResources;
        CSR_Mesh*                    m_pPlayfield;
        CSR_Mesh*                    m_pCross;
        CSR_Mesh*                    m_pRound;
        CSR_Mesh*                    m_pYouWonMsg;
        CSR_Mesh*                    m_pYouLooseMsg;
        CSR_Mesh*                    m_pEqualityMsg;
        IEPlayer                     m_IsPlaying;
        std::string                  m_SceneDir;
        bool                         m_UseOpening;
        bool                         m_GameOver;
        CSR_Matrix4*                 m_pPawnMatrix;
        CSR_Matrix4                  m_PlayfieldMatrix;
        CSR_Matrix4                  m_YouWonMatrix;
        CSR_Matrix4                  m_YouLooseMatrix;
        CSR_Matrix4                  m_EqualityMatrix;
        ICell                        m_Cells[3][3];
        std::size_t                  m_FrameCount;
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
        * Checks if cell is located on the left diagonal
        *@param pCell - cell to check
        *@return true if cell is located on left diagonal, otherwise false
        */
        bool IsOnLeftDiagonal(const ICell* pCell) const;

        /**
        * Checks if cell is located on the right diagonal
        *@param pCell - cell to check
        *@return true if cell is located on right diagonal, otherwise false
        */
        bool IsOnRightDiagonal(const ICell* pCell) const;

        /**
        * Checks if cell is located on a complete line of pawns
        *@param pCell - cell to check
        *@param pawn - pawn occupying the line
        *@return true if cell is located on a complete line of pawns, otherwise false
        */
        bool IsOnCompleteLine(const ICell* pCell, IEPawn pawn) const;

        /**
        * Checks if game is over
        *@return true if game is over, otherwise false
        */
        bool IsGameOver() const;

        /**
        * Checks if a player has win
        *@return winning player, or none
        */
        IEPlayer HasWin() const;

        /**
        * Calculates the next shot that the computer has to play
        *@param player - player played by computer
        *@param[out] pBestCell - best cell found for the next shot
        *@param deep - deep at which the minimax should stop
        *@return score
        */
        int MiniMax(IEPlayer player, ICell*& pBestCell, int deep);

        /**
        * Clears the playfield
        */
        void ClearPlayfield();

        /**
        * Sets the next player pawn
        *@param index - index
        *@note Indexes are distributed as follow:
        *      0|1|2
        *      3]4[5
        *      6|7|8
        */
        void SetPlayerPawn(unsigned index);

        /**
        * Sets the next pawn on the playfield
        *@param pawn - pawn to set
        *@param x - pawn x position
        *@param y - pawn y position
        *@return true if pawn was set successfully, otherwise false
        */
        bool SetPawn(IEPawn pawn, unsigned x, unsigned y);

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
        * Called when a shader should be get for a model
        *@param pModel - model for which the shader shoudl be get
        *@param type - model type
        *@return shader to use to draw the model, 0 if no shader
        *@note The model will not be drawn if no shader is returned
        */
        void* OnGetShader(const void* pModel, CSR_EModelType type);

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
        * Called when a resource identifier should be get from a key
        *@param pKey - key for which the resource identifier should be get
        *@return identifier, 0 on error or if not found
        */
        void* OnGetID(const void* pKey);

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
