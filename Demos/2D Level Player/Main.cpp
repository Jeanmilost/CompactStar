/****************************************************************************
 * ==> Main ----------------------------------------------------------------*
 ****************************************************************************
 * Description : Ground collision tool main form                            *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2018, this file is part of the CompactStar Engine.  *
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
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_hDC(NULL),
    m_hRC(NULL),
    m_FrameCount(0),
    m_PrevOrigin(0),
    m_Angle(0.0f),
    m_PosVelocity(0.0f),
    m_DirVelocity(0.0f),
    m_FPS(0.0),
    m_StartTime(0),
    m_PreviousTime(0),
    m_PrevDesignerTime(0),
    m_Initialized(false)
{
    // build the model dir from the application exe
    UnicodeString sceneDir = ::ExtractFilePath(Application->ExeName);
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExcludeTrailingPathDelimiter(sceneDir) + L"\\Levels";
                m_SceneDir =   AnsiString(sceneDir).c_str();

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

    // create the level manager
    m_pLevel.reset(new CSR_Level());
}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm()
{
    // close the level
    CloseLevel();

    // delete the scene completely
    DeleteScene();

    // shutdown OpenGL
    CSR_OpenGLHelper::DisableOpenGL(Handle, m_hDC, m_hRC);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject* pSender)
{
    // initialize the scene
    InitScene(ClientWidth, ClientHeight);

    // initialize the timer
    m_StartTime        = ::GetTickCount();
    m_PreviousTime     = ::GetTickCount();
    m_PrevDesignerTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject* pSender)
{
    if (!m_Initialized)
        return;

    // update the viewport
    m_pLevel->CreateViewport(ClientWidth, ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aeEventsMessage(tagMSG& msg, bool& handled)
{
    m_PosVelocity = 0.0f;
    m_DirVelocity = 0.0f;

    switch (msg.message)
    {
        case WM_KEYDOWN:
        {
            // get the scene
            CSR_Scene* pScene = m_pLevel->GetScene();

            // no scene?
            if (!pScene)
                break;

            // ignore any edit event
            if (Application->MainForm->ActiveControl &&
                Application->MainForm->ActiveControl->InheritsFrom(__classid(TEdit)))
                break;

            // search for the pressed key
            switch (msg.wParam)
            {
                case VK_LEFT:
                    m_PosVelocity = -1.0f;
                    handled       =  true;
                    break;

                case VK_RIGHT:
                    m_PosVelocity = 1.0f;
                    handled       = true;
                    break;
            }

            return;
        }
    }
}
//---------------------------------------------------------------------------
GLuint TMainForm::OnLoadCubemap(const CSR_Level::IFileNames& fileNames)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return M_CSR_Error_Code;

    return pMainForm->LoadCubemap(fileNames);
}
//------------------------------------------------------------------------------
GLuint TMainForm::OnLoadTexture(const std::string& fileName)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return M_CSR_Error_Code;

    return pMainForm->LoadTexture(fileName);
}
//------------------------------------------------------------------------------
CSR_Shader* TMainForm::OnGetShader(const void* pModel, CSR_EModelType type)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->m_pLevel->OnGetShader(pModel, type);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->m_pLevel->OnSceneBegin(pScene, pContext);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->m_pLevel->OnSceneEnd(pScene, pContext);
}
//------------------------------------------------------------------------------
bool TMainForm::OpenLevel(const std::string& fileName)
{
    // close any previously opened level
    CloseLevel();

    if (!m_pLevel.get())
        return false;

    try
    {
        // create the level serializer
        m_pLevelFile.reset(new CSR_LevelFile_XML("", false));
        m_pLevelFile->Set_OnLoadCubemap(OnLoadCubemap);
        m_pLevelFile->Set_OnLoadTexture(OnLoadTexture);

        // load the level
        if (!m_pLevelFile->Load(fileName.c_str(), *m_pLevel.get()))
        {
            CloseLevel();
            return false;
        }

        // get the scene
        CSR_Scene* pScene = m_pLevel->GetScene();

        // initialize the viewpoint
        InitializeViewPoint(&pScene->m_ViewMatrix);
    }
    __finally
    {
        m_pLevelFile.reset();
    }

    return true;
}
//------------------------------------------------------------------------------
void TMainForm::CloseLevel()
{
    if (!m_pLevel.get())
        return;

    // clear the level manager
    m_pLevel->Clear();
}
//---------------------------------------------------------------------------
GLuint TMainForm::LoadTexture(const std::string& fileName) const
{
    try
    {
        // load texture in a picture
        std::auto_ptr<TPicture> pPicture(new TPicture());

        // texture file exists on the disk?
        if (::FileExists(UnicodeString(AnsiString(fileName.c_str()))))
            pPicture->LoadFromFile(fileName.c_str());
        else
        {
            // level serializer exists?
            if (!m_pLevelFile.get())
                return M_CSR_Error_Code;

            // get the file to load
            const CSR_Buffer* pFile = m_pLevelFile->GetFile(fileName);

            // found it?
            if (!pFile)
                return M_CSR_Error_Code;

            // create a stream from the file
            std::auto_ptr<TMemoryStream> pStream(new TMemoryStream());
            pStream->Write(pFile->m_pData, pFile->m_Length);
            pStream->Position = 0;

            std::auto_ptr<TGraphic> pGraphic;

            // load the correct file format
            if (fileName.rfind(".png") == fileName.length() - 4)
            {
                pGraphic.reset(new TPngImage());
                pGraphic->LoadFromStream(pStream.get());
            }
            else
            if (fileName.rfind(".jpg")  == fileName.length() - 4 ||
                fileName.rfind(".jepg") == fileName.length() - 5)
            {
                pGraphic.reset(new TJPEGImage());
                pGraphic->LoadFromStream(pStream.get());
            }
            else
            if (fileName.rfind(".bmp")  == fileName.length() - 4)
            {
                pGraphic.reset(new TBitmap());
                pGraphic->LoadFromStream(pStream.get());
            }
            else
                return M_CSR_Error_Code;

            // Load the texture
            pPicture->Assign(pGraphic.get());
        }

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
            textureID = csrTextureFromPixelBuffer(pPixelBuffer);
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
//---------------------------------------------------------------------------
GLuint TMainForm::LoadCubemap(const CSR_Level::IFileNames fileNames) const
{
    try
    {
        GLuint textureID = M_CSR_Error_Code;

        // create new OpenGL texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        const std::size_t fileNameCount = fileNames.size();

        // iterate through the cubemap texture files to load
        for (std::size_t i = 0; i < fileNameCount; i++)
        {
            // load texture in a picture
            std::auto_ptr<TPicture> pPicture(new TPicture());

            // texture file exists on the disk?
            if (::FileExists(UnicodeString(AnsiString(fileNames[i].c_str()))))
                pPicture->LoadFromFile(fileNames[i].c_str());
            else
            {
                // level serializer exists?
                if (!m_pLevelFile.get())
                    return M_CSR_Error_Code;

                // get the file to load
                const CSR_Buffer* pFile = m_pLevelFile->GetFile(fileNames[i]);

                // found it?
                if (!pFile)
                    return M_CSR_Error_Code;

                // create a stream from the file
                std::auto_ptr<TMemoryStream> pStream(new TMemoryStream());
                pStream->Write(pFile->m_pData, pFile->m_Length);
                pStream->Position = 0;

                std::auto_ptr<TGraphic> pGraphic;

                // load the correct file format
                if (fileNames[i].rfind(".png") == fileNames[i].length() - 4)
                {
                    pGraphic.reset(new TPngImage());
                    pGraphic->LoadFromStream(pStream.get());
                }
                else
                if (fileNames[i].rfind(".jpg")  == fileNames[i].length() - 4 ||
                    fileNames[i].rfind(".jepg") == fileNames[i].length() - 5)
                {
                    pGraphic.reset(new TJPEGImage());
                    pGraphic->LoadFromStream(pStream.get());
                }
                else
                if (fileNames[i].rfind(".bmp")  == fileNames[i].length() - 4)
                {
                    pGraphic.reset(new TBitmap());
                    pGraphic->LoadFromStream(pStream.get());
                }
                else
                    return M_CSR_Error_Code;

                // Load the texture
                pPicture->Assign(pGraphic.get());
            }

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
                        // copy to pixel array
                        if (pixelSize == 3)
                        {
                            const std::size_t offset = yPos + (((pTexture->Width - 1) - x) * 3);

                            ((unsigned char*)pPixelBuffer->m_pData)[offset]     = pLineRGB[x].rgbtRed;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 1] = pLineRGB[x].rgbtGreen;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 2] = pLineRGB[x].rgbtBlue;
                        }
                        else
                        {
                            const std::size_t offset = yPos + (((pTexture->Width - 1) - x) * 4);

                            ((unsigned char*)pPixelBuffer->m_pData)[offset]     = pLineRGBA[x].rgbRed;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 1] = pLineRGBA[x].rgbGreen;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 2] = pLineRGBA[x].rgbBlue;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 3] = pLineRGBA[x].rgbReserved;
                        }
                    }
                }

                // load the texture on the GPU
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0,
                             GL_RGB,
                             pPixelBuffer->m_Width,
                             pPixelBuffer->m_Height,
                             0,
                             GL_RGB,
                             GL_UNSIGNED_BYTE,
                             pPixelBuffer->m_pData);
            }
            __finally
            {
                csrPixelBufferRelease(pPixelBuffer);
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,     GL_CLAMP_TO_EDGE);

        return textureID;
    }
    catch (...)
    {
        return M_CSR_Error_Code;
    }
}
//------------------------------------------------------------------------------
void TMainForm::InitializeViewPoint(const CSR_Matrix4* pMatrix)
{
    // do initialize the viewpoint from an existing matrix?
    if (pMatrix)
    {
        CSR_Vector3 translation;

        // extract the translation from the matrix
        csrMat4TranslationFrom(pMatrix, &translation.m_X, &translation.m_Y, &translation.m_Z);

        // set the viewpoint bounding sphere position
        m_ViewSphere.m_Center.m_X = -translation.m_X;
        m_ViewSphere.m_Center.m_Y =  translation.m_Y;
        m_ViewSphere.m_Center.m_Z = -translation.m_Z;
        m_ViewSphere.m_Radius     = 0.1f;

        return;
    }

    // reset the viewpoint bounding sphere to his default position
    m_ViewSphere.m_Center.m_X = 0.0f;
    m_ViewSphere.m_Center.m_Y = 0.0f;
    m_ViewSphere.m_Center.m_Z = 3.08f;
    m_ViewSphere.m_Radius     = 0.1f;
}
//------------------------------------------------------------------------------
void TMainForm::CreateScene()
{
    // create the level scene
    CSR_Scene* pScene = m_pLevel->CreateScene();

    if (!pScene)
        throw std::runtime_error("Internal error - could not create the scene");

    // initialize the viewpoint
    InitializeViewPoint(NULL);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // create the default scene
    CreateScene();

    // configure the scene context
    m_pLevel->m_SceneContext.m_fOnGetShader  = OnGetShader;
    m_pLevel->m_SceneContext.m_fOnSceneBegin = OnSceneBegin;
    m_pLevel->m_SceneContext.m_fOnSceneEnd   = OnSceneEnd;

    // set the viewpoint bounding sphere default position
    m_ViewSphere.m_Center.m_X = 0.0f;
    m_ViewSphere.m_Center.m_Y = 0.0f;
    m_ViewSphere.m_Center.m_Z = 3.08f;
    m_ViewSphere.m_Radius     = 0.1f;

    // build the main scene shader
    if (!m_pLevel->BuildSceneShader())
    {
        // show the error message to the user
        ::MessageDlg(L"Failed to load the shader.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // todo FIXME manage the various levels
    // load the first level
    if (!OpenLevel(m_SceneDir + "\\Demo.xml"))
    {
        // show the error message to the user
        ::MessageDlg(L"Failed to load the level.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    m_Initialized = false;

    // delete the level
    m_pLevel.reset();
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    // keep the old position to revert it in case of error
    const CSR_Vector3 oldPos = m_ViewSphere.m_Center;

    // is player rotating?
    if (m_DirVelocity)
    {
        // calculate the player direction
        m_Angle += m_DirVelocity * elapsedTime;

        // validate and apply it
        if (m_Angle > M_PI * 2.0f)
            m_Angle -= M_PI * 2.0f;
        else
        if (m_Angle < 0.0f)
            m_Angle += M_PI * 2.0f;
    }

    // is player moving?
    if (m_PosVelocity)
    {
        // calculate the next position
        m_ViewSphere.m_Center.m_X += m_PosVelocity * sinf(m_Angle + (M_PI * 0.5f)) * elapsedTime;
        m_ViewSphere.m_Center.m_Z -= m_PosVelocity * cosf(m_Angle + (M_PI * 0.5f)) * elapsedTime;
    }

    // enable the shader program
    m_pLevel->EnableShader();

    // calculate and update the view position on the ground
    m_pLevel->ApplyGroundCollision(&m_ViewSphere, m_Angle, oldPos);
}
//---------------------------------------------------------------------------
void TMainForm::OnDrawScene(bool resize)
{
    // is level created?
    if (!m_pLevel.get() || !m_pLevel->GetScene())
    {
        Invalidate();
        return;
    }

    // do draw the scene for a resize?
    if (resize)
    {
        if (!m_Initialized)
            return;

        // just process a minimal draw
        UpdateScene(0.0);
        m_pLevel->Draw();

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
    m_pLevel->Draw();

    ::SwapBuffers(m_hDC);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;
    OnDrawScene(false);
}
//---------------------------------------------------------------------------
