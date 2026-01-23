/****************************************************************************
 * ==> Tic-tac-toe game ----------------------------------------------------*
 ****************************************************************************
 * Description : A complete tic-tac-toe game using a mimimax algorithm      *
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

#include <vcl.h>
#pragma hdrstop
#include "Main.h"

// vcl
#include <Vcl.Imaging.jpeg.hpp>
#include <Vcl.Imaging.pngimage.hpp>

// std
#include <memory>
#include <sstream>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Scene.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_ShaderHelper.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif
#pragma link "openAL32E.lib"
#pragma resource "*.dfm"

// texture files
#define PLAYFIELD_TEXTURE "\\playfield.bmp"
#define CROSS_TEXTURE     "\\cross.bmp"
#define ROUND_TEXTURE     "\\round.bmp"
#define YOUWON_TEXTURE    "\\youwon.bmp"
#define YOULOOSE_TEXTURE  "\\youloose.bmp"
#define EQUALITY_TEXTURE  "\\equality.bmp"

// player pawn
#define M_Player_Pawn   IE_PA_Cross
#define M_Computer_Pawn IE_PA_Round

// configure which player will begin the game
#define M_Starting_Player IE_PL_Player

//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_hDC(NULL),
    m_hRC(NULL),
    m_pOpenALDevice(NULL),
    m_pOpenALContext(NULL),
    m_pScene(NULL),
    m_pShader(NULL),
    m_pPlayfield(0),
    m_pCross(0),
    m_pRound(0),
    m_pYouWonMsg(0),
    m_pYouLooseMsg(0),
    m_pEqualityMsg(0),
    m_IsPlaying(M_Starting_Player),
    m_UseOpening(false),
    m_GameOver(false),
    m_pPawnMatrix(0),
    m_FrameCount(0),
    m_FPS(0.0),
    m_StartTime(0),
    m_PreviousTime(0),
    m_Initialized(false)
{
    // build the model dir from the application exe
    UnicodeString sceneDir = ::ExtractFilePath(Application->ExeName);
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExcludeTrailingPathDelimiter(sceneDir) + L"\\Resources";
                m_SceneDir =   AnsiString(sceneDir).c_str();

    // initialize OpenAL
    csrSoundInitializeOpenAL(&m_pOpenALDevice, &m_pOpenALContext);

    // enable OpenGL
    CSR_OpenGLHelper::EnableOpenGL(Handle, &m_hDC, &m_hRC);

    // stop GLEW crashing on OSX :-/
    glewExperimental = GL_TRUE;

    // initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        // shutdown OpenGL
        CSR_OpenGLHelper::DisableOpenGL(Handle, m_hDC, m_hRC);

        // close the app
        Application->Terminate();
    }
}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm()
{
    // delete the scene completely
    DeleteScene();

    // shutdown OpenGL
    CSR_OpenGLHelper::DisableOpenGL(Handle, m_hDC, m_hRC);

    // release OpenAL interface
    csrSoundReleaseOpenAL(m_pOpenALDevice, m_pOpenALContext);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject* pSender)
{
    // initialize the scene
    InitScene(ClientWidth, ClientHeight);

    // initialize the timer
    m_StartTime    = ::GetTickCount();
    m_PreviousTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject* pSender)
{
    if (!m_Initialized)
        return;

    // update the viewport
    CSR_OpenGLHelper::CreateViewport(ClientWidth,
                                     ClientHeight,
                                     0.01f,
                                     100.0f,
                                     m_pShader,
                                     m_pScene->m_ProjectionMatrix);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aeEventsMessage(tagMSG& msg, bool& handled)
{
    switch (msg.message)
    {
        case WM_KEYDOWN:
            switch (msg.wParam)
            {
                case VK_SPACE: ClearPlayfield();   return;
                case '7':
                case VK_NUMPAD7: SetPlayerPawn(0); return;
                case '8':
                case VK_NUMPAD8: SetPlayerPawn(1); return;
                case '9':
                case VK_NUMPAD9: SetPlayerPawn(2); return;
                case '4':
                case VK_NUMPAD4: SetPlayerPawn(3); return;
                case '5':
                case VK_NUMPAD5: SetPlayerPawn(4); return;
                case '6':
                case VK_NUMPAD6: SetPlayerPawn(5); return;
                case '1':
                case VK_NUMPAD1: SetPlayerPawn(6); return;
                case '2':
                case VK_NUMPAD2: SetPlayerPawn(7); return;
                case '3':
                case VK_NUMPAD3: SetPlayerPawn(8); return;
            }
    }
}
//------------------------------------------------------------------------------
void* TMainForm::OnGetShaderCallback(const void* pModel, CSR_EModelType type)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->OnGetShader(pModel, type);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneBeginCallback(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnSceneBegin(pScene, pContext);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneEndCallback(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnSceneEnd(pScene, pContext);
}
//---------------------------------------------------------------------------
void* TMainForm::OnGetIDCallback(const void* pKey)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return NULL;

    return pMainForm->OnGetID(pKey);
}
//---------------------------------------------------------------------------
void TMainForm::OnDeleteTextureCallback(const CSR_Texture* pTexture)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnDeleteTexture(pTexture);
}
//---------------------------------------------------------------------------
GLuint TMainForm::LoadTexture(const std::string& fileName) const
{
    try
    {
        // load texture in a picture
        std::auto_ptr<TPicture> pPicture(new TPicture());
        pPicture->LoadFromFile(fileName.c_str());

        // convert it to a bitmap
        std::auto_ptr<TBitmap> pTexture(new TBitmap());
        pTexture->Assign(pPicture->Graphic);

        int pixelSize;

        // search for bitmap pixel format
        switch (pTexture->PixelFormat)
        {
            case pf24bit: pixelSize = 3; break;
            case pf32bit: pixelSize = 4; break;
            default:                     return M_CSR_Error_Code;
        }

        GLuint           textureID    = M_CSR_Error_Code;
        CSR_PixelBuffer* pPixelBuffer = csrPixelBufferCreate();

        try
        {
            // configure the pixel buffer
            pPixelBuffer->m_PixelType    = CSR_PT_BGR;
            pPixelBuffer->m_ImageType    = CSR_IT_Raw;
            pPixelBuffer->m_Width        = pTexture->Width;
            pPixelBuffer->m_Height       = pTexture->Height;
            pPixelBuffer->m_BytePerPixel = pixelSize;
            pPixelBuffer->m_DataLength   = pTexture->Width * pTexture->Height * pixelSize;

            // reserve memory for the pixel array. NOTE use malloc and not new here to be conform
            // with the c standards (otherwise CodeGuard will not be happy)
            pPixelBuffer->m_pData = malloc(pPixelBuffer->m_DataLength);

            TRGBTriple* pLineRGB;
            TRGBQuad*   pLineRGBA;

            // iterate through lines to copy
            for (int y = 0; y < pTexture->Height; ++y)
            {
                // get the next pixel line from bitmap
                if (pixelSize == 3)
                    pLineRGB  = static_cast<TRGBTriple*>(pTexture->ScanLine[y]);
                else
                    pLineRGBA = static_cast<TRGBQuad*>(pTexture->ScanLine[y]);

                // calculate the start y position
                const int yPos = y * pTexture->Width * pixelSize;

                // iterate through pixels to copy
                for (int x = 0; x < pTexture->Width; ++x)
                {
                    // copy to pixel array and take the opportunity to swap the pixel RGB values
                    if (pixelSize == 3)
                    {
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 3)]     = pLineRGB[x].rgbtRed;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 3) + 1] = pLineRGB[x].rgbtGreen;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 3) + 2] = pLineRGB[x].rgbtBlue;
                    }
                    else
                    {
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4)]     = pLineRGBA[x].rgbRed;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4) + 1] = pLineRGBA[x].rgbGreen;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4) + 2] = pLineRGBA[x].rgbBlue;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4) + 3] = pLineRGBA[x].rgbReserved;
                    }
                }
            }

            // load the texture on the GPU
            textureID = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);
        }
        __finally
        {
            csrPixelBufferRelease(pPixelBuffer);
        }

        return textureID;
    }
    catch (...)
    {
        return M_CSR_Error_Code;
    }
}
//------------------------------------------------------------------------------
bool TMainForm::IsOnLeftDiagonal(const ICell* pCell) const
{
    return (pCell->m_X == pCell->m_Y);
}
//------------------------------------------------------------------------------
bool TMainForm::IsOnRightDiagonal(const ICell* pCell) const
{
    return (pCell->m_X + pCell->m_Y == 2);
}
//------------------------------------------------------------------------------
bool TMainForm::IsOnCompleteLine(const ICell* pCell, IEPawn pawn) const
{
    unsigned horiz     = 0;
    unsigned vert      = 0;
    unsigned lDiagonal = 0;
    unsigned rDiagonal = 0;

    // iterate through cells
    for (unsigned j = 0; j < 3; ++j)
        for (unsigned i = 0; i < 3; ++i)
        {
            // get cell to check
            const ICell* pCellToCheck = &m_Cells[i][j];

            // check if cell is on same line as reference cell and if cell contains pawn
            if (pCellToCheck->m_X == pCell->m_X && pCellToCheck->m_Pawn == pawn)
                ++horiz;

            // check if cell is on same column as reference cell and if cell contains pawn
            if (pCellToCheck->m_Y == pCell->m_Y && pCellToCheck->m_Pawn == pawn)
                ++vert;

            // check if cell and reference cell are on left diagonal and if cell contains pawn
            if (IsOnLeftDiagonal(pCell) && IsOnLeftDiagonal(pCellToCheck) && pCellToCheck->m_Pawn == pawn)
                ++lDiagonal;

            // check if cell and reference cell are on right diagonal and if cell contains pawn
            if (IsOnRightDiagonal(pCell) && IsOnRightDiagonal(pCellToCheck) && pCellToCheck->m_Pawn == pawn)
                ++rDiagonal;
        }

    // a line is complete if all cells of line, column or diagonal contains same pawn
    return (horiz == 3 || vert == 3 || rDiagonal == 3 || lDiagonal == 3);
}
//------------------------------------------------------------------------------
bool TMainForm::IsGameOver() const
{
    unsigned occupiedCell = 0;

    // iterate through cells
    for (unsigned j = 0; j < 3; ++j)
        for (unsigned i = 0; i < 3; ++i)
        {
            // get cell
            const ICell* pCell = &m_Cells[i][j];

            // is cell occupied by pawn?
            if (pCell->m_Pawn != IE_PA_None)
                ++occupiedCell;
        }

    // are all cells occupied?
    return (occupiedCell >= 9);
}
//------------------------------------------------------------------------------
TMainForm::IEPlayer TMainForm::HasWin() const
{
    // iterate through cells
    for (unsigned j = 0; j < 3; ++j)
        for (unsigned i = 0; i < 3; ++i)
        {
            // get cell
            const ICell* pCell = &m_Cells[i][j];

            // check if player 1 has win (i.e. if he has completed a line)
            if (IsOnCompleteLine(pCell, M_Player_Pawn))
                return IE_PL_Player;

            // check if player 2 has win (i.e. if he has completed a line)
            if (IsOnCompleteLine(pCell, M_Computer_Pawn))
                return IE_PL_Computer;
        }

    return IE_PL_None;
}
//---------------------------------------------------------------------------
int TMainForm::MiniMax(IEPlayer player, ICell*& pBestCell, int deep)
{
    // calculate min and max sum
    const int sum = (1 << 20);
          int max = -sum;
          int min =  sum;

    // any player has win?
    const IEPlayer winner = HasWin();

    // is game over?
    if (winner == IE_PL_Player)
        return IE_PL_Player;
    else
    if (winner == IE_PL_Computer)
        return IE_PL_Computer;
    else
    if (IsGameOver())
        return 0;

    // iterate through cells
    for (unsigned j = 0; j < 3; ++j)
        for (unsigned i = 0; i < 3; ++i)
        {
            ICell* pCell = &m_Cells[i][j];

            // is cell already occupied?
            if (pCell->m_Pawn != IE_PA_None)
                continue;

            // search for player to check
            switch (player)
            {
                case IE_PL_Player:
                {
                    // simulate player 1 move
                    pCell->m_Pawn = M_Player_Pawn;

                    // check next opposite player move
                    const int result = MiniMax(IE_PL_Computer, pBestCell, deep + 1);

                    // revert cell to original value
                    pCell->m_Pawn = IE_PA_None;

                    // do update max?
                    if (result > max)
                    {
                        max = result;

                        // first level?
                        if (!deep)
                            // get best cell
                            pBestCell = pCell;
                    }

                    continue;
                }

                case IE_PL_Computer:
                {
                    // simulate player 2 move
                    pCell->m_Pawn = M_Computer_Pawn;

                    // check next opposite player move
                    int result = MiniMax(IE_PL_Player, pBestCell, deep + 1);

                    // revert cell to original value
                    pCell->m_Pawn = IE_PA_None;

                    // do update min?
                    if (result < min)
                    {
                        min = result;

                        // first level?
                        if (!deep)
                            // get best cell
                            pBestCell = pCell;
                    }

                    continue;
                }

                default:
                    continue;
            }
        }

    // is player?
    if (player == IE_PL_Player)
        return max;

    return min;
}
//---------------------------------------------------------------------------
void TMainForm::ClearPlayfield()
{
    // reset current player
    m_IsPlaying = M_Starting_Player;

    // reset game over
    m_GameOver = 0;

    // hide messages
    m_YouWonMatrix.m_Table[3][0]   = 9999.0f;
    m_YouWonMatrix.m_Table[3][1]   = 9999.0f;
    m_YouLooseMatrix.m_Table[3][0] = 9999.0f;
    m_YouLooseMatrix.m_Table[3][1] = 9999.0f;
    m_EqualityMatrix.m_Table[3][0] = 9999.0f;
    m_EqualityMatrix.m_Table[3][1] = 9999.0f;

    // iterate through cells
    for (unsigned j = 0; j < 3; ++j)
        for (unsigned i = 0; i < 3; ++i)
        {
            // clear cells
            m_Cells[i][j].m_Pawn = IE_PA_None;

            m_Cells[i][j].m_Matrix_Cross.m_Table[3][0] = 9999.0f;
            m_Cells[i][j].m_Matrix_Cross.m_Table[3][1] = 9999.0f;

            m_Cells[i][j].m_Matrix_Round.m_Table[3][0] = 9999.0f;
            m_Cells[i][j].m_Matrix_Round.m_Table[3][1] = 9999.0f;
        }
}
//---------------------------------------------------------------------------
void TMainForm::SetPlayerPawn(unsigned index)
{
    // isn't the player turn?
    if (m_IsPlaying != IE_PL_Player)
        return;

    // game over?
    if (m_GameOver)
        return;

    // search for index
    switch (index)
    {
        case 0:
            if (!SetPawn(M_Player_Pawn, 0, 0))
                return;

            break;

        case 1:
            if (!SetPawn(M_Player_Pawn, 1, 0))
                return;

            break;

        case 2:
            if (!SetPawn(M_Player_Pawn, 2, 0))
                return;

            break;

        case 3:
            if (!SetPawn(M_Player_Pawn, 0, 1))
                return;

            break;

        case 4:
            if (!SetPawn(M_Player_Pawn, 1, 1))
                return;

            break;

        case 5:
            if (!SetPawn(M_Player_Pawn, 2, 1))
                return;

            break;

        case 6:
            if (!SetPawn(M_Player_Pawn, 0, 2))
                return;

            break;

        case 7:
            if (!SetPawn(M_Player_Pawn, 1, 2))
                return;

            break;

        case 8:
            if (!SetPawn(M_Player_Pawn, 2, 2))
                return;

            break;
    }

    // check if game ended
    m_GameOver = IsGameOver() || (HasWin() != IE_PL_None);

    // since now computer is playing
    m_IsPlaying = IE_PL_SkipRendering;
}
//---------------------------------------------------------------------------
bool TMainForm::SetPawn(IEPawn pawn, unsigned x, unsigned y)
{
    // indices out of bounds?
    if (x > 2 || y > 2)
        return false;

    // slot already occupied?
    if (m_Cells[x][y].m_Pawn != IE_PA_None)
        return false;

    // set pawn type
    m_Cells[x][y].m_Pawn = pawn;

    // set pawn on playfield
    switch (pawn)
    {
        case IE_PA_Cross:
            m_Cells[x][y].m_Matrix_Cross.m_Table[3][0] = m_Cells[x][y].m_Pos.m_X;
            m_Cells[x][y].m_Matrix_Cross.m_Table[3][1] = m_Cells[x][y].m_Pos.m_Y;
            break;

        case IE_PA_Round:
            m_Cells[x][y].m_Matrix_Round.m_Table[3][0] = m_Cells[x][y].m_Pos.m_X;
            m_Cells[x][y].m_Matrix_Round.m_Table[3][1] = m_Cells[x][y].m_Pos.m_Y;
            break;
    }

    return true;
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // create the default scene
    m_pScene = csrSceneCreate();

    // create the scene context
    csrSceneContextInit(&m_SceneContext);
    m_SceneContext.m_fOnSceneBegin = OnSceneBeginCallback;
    m_SceneContext.m_fOnSceneEnd   = OnSceneEndCallback;
    m_SceneContext.m_fOnGetShader  = OnGetShaderCallback;
    m_SceneContext.m_fOnGetID      = OnGetIDCallback;

    // configure the scene color background
    m_pScene->m_Color.m_R = 1.0f;
    m_pScene->m_Color.m_G = 1.0f;
    m_pScene->m_Color.m_B = 1.0f;
    m_pScene->m_Color.m_A = 1.0f;

    // initialize the matrices
    csrMat4Identity(&m_pScene->m_ProjectionMatrix);
    csrMat4Identity(&m_pScene->m_ViewMatrix);
    csrMat4Identity(&m_PlayfieldMatrix);
    csrMat4Identity(&m_YouWonMatrix);
    csrMat4Identity(&m_YouLooseMatrix);
    csrMat4Identity(&m_EqualityMatrix);

    const std::string vsTextured = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Texture);
    const std::string fsTextured = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Texture);

    // compile, link and use shader
    m_pShader = csrOpenGLShaderLoadFromStr(vsTextured.c_str(),
                                           vsTextured.length(),
                                           fsTextured.c_str(),
                                           fsTextured.length(),
                                           0,
                                           0);
    csrShaderEnable(m_pShader);

    // configure the shader slots
    m_pShader->m_VertexSlot   = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aVertices");
    m_pShader->m_ColorSlot    = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aColor");
    m_pShader->m_TexCoordSlot = glGetAttribLocation(m_pShader->m_ProgramID, "csr_aTexCoord");
    m_pShader->m_TextureSlot  = glGetAttribLocation(m_pShader->m_ProgramID, "csr_sTexture");

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    CSR_VertexFormat  vf;
    CSR_VertexCulling vc;
    CSR_Material      material;

    // configure the vertex format
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // configure the vertex culling
    vc.m_Type = CSR_CT_Back;
    vc.m_Face = CSR_CF_CCW;

    // configure the material
    material.m_Color       = 0xffffffff;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;
    material.m_uScale      = 1.0f;
    material.m_vScale      = 1.0f;

    // create the surfaces
    m_pPlayfield   = csrShapeCreateSurface(0.8f, 0.8f, &vf, &vc, &material, 0);
    m_pCross       = csrShapeCreateSurface(0.2f, 0.2f, &vf, &vc, &material, 0);
    m_pRound       = csrShapeCreateSurface(0.2f, 0.2f, &vf, &vc, &material, 0);
    m_pYouWonMsg   = csrShapeCreateSurface(0.2f, 0.1f, &vf, &vc, &material, 0);
    m_pYouLooseMsg = csrShapeCreateSurface(0.2f, 0.1f, &vf, &vc, &material, 0);
    m_pEqualityMsg = csrShapeCreateSurface(0.2f, 0.1f, &vf, &vc, &material, 0);

    m_PlayfieldMatrix.m_Table[3][2] = 0.001f;

    // add playfield to the scene
    csrSceneAddMesh(m_pScene, m_pPlayfield, 0, 0);
    csrSceneAddModelMatrix(m_pScene, m_pPlayfield, &m_PlayfieldMatrix);

    // add pawns to the scene
    csrSceneAddMesh(m_pScene, m_pCross, 0, 0);
    csrSceneAddMesh(m_pScene, m_pRound, 0, 0);

    // add messages to the scene
    csrSceneAddMesh(m_pScene, m_pYouWonMsg,   0, 0);
    csrSceneAddMesh(m_pScene, m_pYouLooseMsg, 0, 0);
    csrSceneAddMesh(m_pScene, m_pEqualityMsg, 0, 0);
    csrSceneAddModelMatrix(m_pScene, m_pYouWonMsg,   &m_YouWonMatrix);
    csrSceneAddModelMatrix(m_pScene, m_pYouLooseMsg, &m_YouLooseMatrix);
    csrSceneAddModelMatrix(m_pScene, m_pEqualityMsg, &m_EqualityMatrix);

    const float step = 0.28f;

    // calculate and keep the possible pawn positions on the playfield grid
    for (std::size_t j = 0; j < 3; ++j)
        for (std::size_t i = 0; i < 3; ++i)
        {
            m_Cells[i][j].m_X = i;
            m_Cells[i][j].m_Y = j;

            m_Cells[i][j].m_Pos.m_X = step - (i * step);
            m_Cells[i][j].m_Pos.m_Y = step - (j * step);

            csrMat4Identity(&m_Cells[i][j].m_Matrix_Cross);
            csrMat4Identity(&m_Cells[i][j].m_Matrix_Round);

            csrSceneAddModelMatrix(m_pScene, m_pCross, &m_Cells[i][j].m_Matrix_Cross);
            csrSceneAddModelMatrix(m_pScene, m_pRound, &m_Cells[i][j].m_Matrix_Round);
        }

    // load the playfield texture
    CSR_OpenGLHelper::AddTexture(&m_pPlayfield->m_Skin,
                                  LoadTexture(m_SceneDir + PLAYFIELD_TEXTURE),
                                  m_OpenGLResources);

    // load the cross pawn texture
    CSR_OpenGLHelper::AddTexture(&m_pCross->m_Skin,
                                  LoadTexture(m_SceneDir + CROSS_TEXTURE),
                                  m_OpenGLResources);

    // load the round pawn texture
    CSR_OpenGLHelper::AddTexture(&m_pRound->m_Skin,
                                  LoadTexture(m_SceneDir + ROUND_TEXTURE),
                                  m_OpenGLResources);

    // load the "you loose" texture
    CSR_OpenGLHelper::AddTexture(&m_pYouLooseMsg->m_Skin,
                                  LoadTexture(m_SceneDir + YOULOOSE_TEXTURE),
                                  m_OpenGLResources);

    // load the "you won" texture
    CSR_OpenGLHelper::AddTexture(&m_pYouWonMsg->m_Skin,
                                  LoadTexture(m_SceneDir + YOUWON_TEXTURE),
                                  m_OpenGLResources);

    // load the "equality!" texture
    CSR_OpenGLHelper::AddTexture(&m_pEqualityMsg->m_Skin,
                                  LoadTexture(m_SceneDir + EQUALITY_TEXTURE),
                                  m_OpenGLResources);

    // clear the playfield
    ClearPlayfield();

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    m_Initialized = false;

    // release the scene
    csrSceneRelease(m_pScene, OnDeleteTextureCallback);

    // release the shader
    csrOpenGLShaderRelease(m_pShader);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    // create a point of view from an arcball
    CSR_ArcBall arcball;
    arcball.m_AngleX = 0.0f;
    arcball.m_AngleY = M_PI;
    arcball.m_Radius = 1.0f;
    csrSceneArcBallToMatrix(&arcball, &m_pScene->m_ViewMatrix);

    // do skip for the next rendering?
    if (m_IsPlaying == IE_PL_SkipRendering)
    {
        m_IsPlaying = IE_PL_Computer;
        return;
    }

    // game over?
    if (m_GameOver)
        // search which player won the game and show matching message
        switch (HasWin())
        {
            case IE_PL_Player:
                m_YouWonMatrix.m_Table[3][0] = 0.0f;
                m_YouWonMatrix.m_Table[3][1] = 0.0f;
                return;

            case IE_PL_Computer:
                m_YouLooseMatrix.m_Table[3][0] = 0.0f;
                m_YouLooseMatrix.m_Table[3][1] = 0.0f;
                return;

            default:
                m_EqualityMatrix.m_Table[3][0] = 0.0f;
                m_EqualityMatrix.m_Table[3][1] = 0.0f;
                return;
        }

    // is computer playing?
    if (m_IsPlaying == IE_PL_Computer)
    {
        unsigned i;
        unsigned j;
        unsigned count = 0;

        // use random opening?
        if (m_UseOpening)
            // iterate through cells and count occupied ones
            for (j = 0; j < 3; ++j)
                for (i = 0; i < 3; ++i)
                {
                    const ICell* pCell = &m_Cells[i][j];

                    // is cell already occupied?
                    if (pCell->m_Pawn == IE_PA_None)
                        continue;

                    ++count;
                }

        // put the next pawn on grid
        if (m_UseOpening && count < 2)
            while (!SetPawn(M_Computer_Pawn, rand() % 3, rand() % 3));
        else
        {
            ICell* pBestCell = 0;

            // search for best cell
            MiniMax(IE_PL_Computer, pBestCell, 0);

            if (pBestCell)
                SetPawn(M_Computer_Pawn, pBestCell->m_X, pBestCell->m_Y);
        }

        // check if game ended
        m_GameOver  = IsGameOver() || (HasWin() != IE_PL_None);

        // since now player is playing
        m_IsPlaying = IE_PL_Player;
    }
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    // draw the scene
    csrSceneDraw(m_pScene, &m_SceneContext);
}
//---------------------------------------------------------------------------
void TMainForm::OnDrawScene(bool resize)
{
    // do draw the scene for a resize?
    if (resize)
    {
        if (!m_Initialized)
            return;

        // just process a minimal draw
        UpdateScene(0.0);
        DrawScene();

        ::SwapBuffers(m_hDC);
        return;
    }

    // calculate time interval
    const unsigned __int64 now            = ::GetTickCount();
    const double           elapsedTime    = (now - m_PreviousTime) / 1000.0;
                           m_PreviousTime =  now;

    if (!m_Initialized)
        return;

    ++m_FrameCount;

    // calculate the FPS
    if (m_FrameCount >= 100)
    {
        const double      smoothing = 0.1;
        const std::size_t fpsTime   = now > m_StartTime ? now - m_StartTime : 1;
        const std::size_t newFPS    = (m_FrameCount * 100) / fpsTime;
        m_StartTime                 = ::GetTickCount();
        m_FrameCount                = 0;
        m_FPS                       = (newFPS * smoothing) + (m_FPS * (1.0 - smoothing));
    }

    // update and draw the scene
    UpdateScene(elapsedTime);
    DrawScene();

    ::SwapBuffers(m_hDC);
}
//---------------------------------------------------------------------------
void* TMainForm::OnGetShader(const void* pModel, CSR_EModelType type)
{
    return m_pShader;
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    csrDrawBegin(&pScene->m_Color);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    csrDrawEnd();
}
//---------------------------------------------------------------------------
void* TMainForm::OnGetID(const void* pKey)
{
    return CSR_OpenGLHelper::GetTextureID(pKey, m_OpenGLResources);
}
//---------------------------------------------------------------------------
void TMainForm::OnDeleteTexture(const CSR_Texture* pTexture)
{
    return CSR_OpenGLHelper::DeleteTexture(pTexture, m_OpenGLResources);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;
    OnDrawScene(false);
}
//---------------------------------------------------------------------------
