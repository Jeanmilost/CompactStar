/*****************************************************************************
 * ==> TScreenshotFrame -----------------------------------------------------*
 *****************************************************************************
 * Description : This module contains a frame that allows the user to create *
 *               a screenshot from a model                                   *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#include <vcl.h>
#pragma hdrstop
#include "TScreenshotFrame.h"

// compactStar engine
#include "CSR_Renderer.h"

// classes
#include "CSR_VCLHelper.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TScreenshotFrame
//---------------------------------------------------------------------------
TScreenshotFrame* ScreenshotFrame;
//---------------------------------------------------------------------------
__fastcall TScreenshotFrame::TScreenshotFrame(TComponent* pOwner) :
    TFrame(pOwner),
    m_pSceneOverlayForm(NULL),
    m_pSceneShader(NULL),
    m_pMesh(NULL),
    m_pModel(NULL),
    m_pMDL(NULL),
    m_pAABBTree(NULL),
    m_AntialiasingFactor(4),
    m_Offset(0.0f),
    m_ArcBallOffset(0.1f),
    m_Reseting(false)
{}
//---------------------------------------------------------------------------
__fastcall TScreenshotFrame::~TScreenshotFrame()
{
    ReleaseScene();
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::FrameResize(TObject* pSender)
{
    paCamera->Margins->Left    = (paRight->Width - (btCameraBack->Left + btCameraBack->Width)) >> 1;
    paCamera->Margins->Top     = 0;
    paCamera->Margins->Right   = 0;
    paCamera->Margins->Bottom  = 0;
    paCamera->AlignWithMargins = true;
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::btCameraLeftClick(TObject* pSender)
{
    // move the camera
    if (rbArcball->Checked)
        m_ArcBall.m_AngleY = std::fmod(m_ArcBall.m_AngleY - m_ArcBallOffset, M_PI * 2.0f);
    else
        m_Camera.m_Position.m_X -= m_Offset;

    // redraw the scene
    DrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::btCameraRightClick(TObject* pSender)
{
    // move the camera
    if (rbArcball->Checked)
        m_ArcBall.m_AngleY = std::fmod(m_ArcBall.m_AngleY + m_ArcBallOffset, M_PI * 2.0f);
    else
        m_Camera.m_Position.m_X += m_Offset;

    // redraw the scene
    DrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::btCameraUpClick(TObject* pSender)
{
    // move the camera
    if (rbArcball->Checked)
        m_ArcBall.m_AngleX = std::fmod(m_ArcBall.m_AngleX + m_ArcBallOffset, M_PI * 2.0f);
    else
        m_Camera.m_Position.m_Y += m_Offset;

    // redraw the scene
    DrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::btCameraDownClick(TObject* pSender)
{
    // move the camera
    if (rbArcball->Checked)
        m_ArcBall.m_AngleX = std::fmod(m_ArcBall.m_AngleX - m_ArcBallOffset, M_PI * 2.0f);
    else
        m_Camera.m_Position.m_Y -= m_Offset;

    // redraw the scene
    DrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::btCameraBackClick(TObject* pSender)
{
    // move the camera
    if (rbArcball->Checked)
        m_ArcBall.m_Radius += m_Offset;
    else
        m_Camera.m_Position.m_Z -= m_Offset;

    // redraw the scene
    DrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::btCameraFrontClick(TObject* pSender)
{
    // move the camera
    if (rbArcball->Checked)
        m_ArcBall.m_Radius -= m_Offset;
    else
        m_Camera.m_Position.m_Z += m_Offset;

    // redraw the scene
    DrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::btConfigClick(TObject* pSender)
{
    // calculate the point where the popup menu sould be shown
    TPoint popupPos(0, btConfig->Height);

    // convert to screen coordinate system
    popupPos = btConfig->ClientToScreen(popupPos);

    // show the config popup menu
    pmConfig->Popup(popupPos.X, popupPos.Y);
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::miResetSceneClick(TObject* pSender)
{
    // if the scene was not initialized, do nothing
    if (!m_SceneContext.m_hDC || !m_SceneContext.m_hRC || !m_pSceneShader)
    {
        imScreenshot->Picture->Assign(NULL);
        return;
    }

    // enable view context and shader
    wglMakeCurrent(m_SceneContext.m_hDC, m_SceneContext.m_hRC);
    csrShaderEnable(m_pSceneShader);

    // reset the scene to his default state
    ResetScene();

    // redraw the scene
    DrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::rbArcballClick(TObject* pSender)
{
    if (m_Reseting)
        return;

    // if the scene was not initialized, do nothing
    if (!m_SceneContext.m_hDC || !m_SceneContext.m_hRC || !m_pSceneShader)
    {
        imScreenshot->Picture->Assign(NULL);
        return;
    }

    // for the arcball mode, update the arcball radius but let the model on the scene center
    m_ArcBall.m_Radius          = -m_ModelMatrix.m_Table[3][2] - m_Camera.m_Position.m_Z;
    m_ModelMatrix.m_Table[3][2] =  0.0f;

    // enable view context and shader
    wglMakeCurrent(m_SceneContext.m_hDC, m_SceneContext.m_hRC);
    csrShaderEnable(m_pSceneShader);

    // connect the model view matrix to shader
    const GLint modelMatrixSlot = glGetUniformLocation(m_pSceneShader->m_ProgramID, "csr_uModel");
    glUniformMatrix4fv(modelMatrixSlot, 1, 0, &m_ModelMatrix.m_Table[0][0]);

    // redraw the scene
    DrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::rbFirstViewPersonClick(TObject* pSender)
{
    if (m_Reseting)
        return;

    // if the scene was not initialized, do nothing
    if (!m_SceneContext.m_hDC || !m_SceneContext.m_hRC || !m_pSceneShader)
    {
        imScreenshot->Picture->Assign(NULL);
        return;
    }

    // for the first view mode, update the model z position and replace the camera on the center
    m_ModelMatrix.m_Table[3][2] = -m_ArcBall.m_Radius;
    m_Camera.m_Position.m_Z     =  0.0f;

    // enable view context and shader
    wglMakeCurrent(m_SceneContext.m_hDC, m_SceneContext.m_hRC);
    csrShaderEnable(m_pSceneShader);

    // connect the model view matrix to shader
    const GLint modelMatrixSlot = glGetUniformLocation(m_pSceneShader->m_ProgramID, "csr_uModel");
    glUniformMatrix4fv(modelMatrixSlot, 1, 0, &m_ModelMatrix.m_Table[0][0]);

    // redraw the scene
    DrawScene();
}
//---------------------------------------------------------------------------
void __fastcall TScreenshotFrame::paColorValueClick(TObject* pSender)
{
    cdColor->Color = paColorValue->Color;

    // notify user about selecting a color
    if (!cdColor->Execute())
        return;

    paColorValue->Color = cdColor->Color;

    // update the scene color
    csrRGBAToColor(csrColorBGRToRGBA(::ColorToRGB(paColorValue->Color)), &m_SceneColor);

    // redraw the scene
    DrawScene();
}
//---------------------------------------------------------------------------
void TScreenshotFrame::Enable(bool value)
{
    paColorValue->Enabled      = value;
    btCameraLeft->Enabled      = value;
    btCameraUp->Enabled        = value;
    btCameraBack->Enabled      = value;
    btCameraRight->Enabled     = value;
    btCameraDown->Enabled      = value;
    btCameraFront->Enabled     = value;
    miResetScene->Enabled      = value;
    rbArcball->Enabled         = value;
    rbFirstViewPerson->Enabled = value;
}
//---------------------------------------------------------------------------
bool TScreenshotFrame::LoadModel(      CSR_DesignerHelper::IEModelType type,
                                 const std::wstring&                   fileName,
                                 const std::wstring&                   textureFileName,
                                 const std::wstring&                   bumpMapFileName,
                                       unsigned                        color)
{
    // select the kind of model to load
    switch (type)
    {
        case CSR_DesignerHelper::IE_MT_Surface:
        case CSR_DesignerHelper::IE_MT_Box:
        case CSR_DesignerHelper::IE_MT_Sphere:
        case CSR_DesignerHelper::IE_MT_Cylinder:
        case CSR_DesignerHelper::IE_MT_Disk:
        case CSR_DesignerHelper::IE_MT_Ring:
        case CSR_DesignerHelper::IE_MT_Spiral:
            // create a scene to draw the model screenshot
            if (!CreateScene(type, imScreenshot->Width, imScreenshot->Height, color))
                return false;

            break;

        case CSR_DesignerHelper::IE_MT_Model:
            // create a scene to draw the model screenshot
            if (!CreateScene(AnsiString(UnicodeString(fileName.c_str())).c_str(),
                             imScreenshot->Width,
                             imScreenshot->Height,
                             color))
                return false;

            break;

        default:
            // unknown model type
            return false;
    }

    // do use an alternative texture?
    if (!textureFileName.empty())
    {
        // load the texture image
        std::auto_ptr<TPicture> pPicture(new TPicture());
        pPicture->LoadFromFile(textureFileName.c_str());

        // convert it to bitmap
        std::auto_ptr<TBitmap> pBitmap(new TBitmap());
        pBitmap->Assign(pPicture->Graphic);

        int   pixelSize;
        GLint pixelType;

        // search for bitmap pixel format
        switch (pBitmap->PixelFormat)
        {
            case pf24bit: pixelSize = 3; pixelType = GL_RGB;  break;
            case pf32bit: pixelSize = 4; pixelType = GL_RGBA; break;
            default:      return false;
        }

        unsigned char* pPixels = NULL;
        bool           result  = false;

        try
        {
            // reserve memory for the pixel array
            pPixels = new unsigned char[pBitmap->Width * pBitmap->Height * pixelSize];

            TRGBTriple* pLineRGB;
            TRGBQuad*   pLineRGBA;

            // iterate through lines to copy
            for (int y = 0; y < pBitmap->Height; ++y)
            {
                // get the next pixel line from bitmap
                if (pixelSize == 3)
                    pLineRGB  = static_cast<TRGBTriple*>(pBitmap->ScanLine[y]);
                else
                    pLineRGBA = static_cast<TRGBQuad*>(pBitmap->ScanLine[y]);

                int yPos;

                // calculate the start y position
                if (type == CSR_DesignerHelper::IE_MT_Model)
                    yPos = y * pBitmap->Width * pixelSize;
                else
                    yPos = ((pBitmap->Height - 1) - y) * pBitmap->Width * pixelSize;

                // iterate through pixels to copy
                for (int x = 0; x < pBitmap->Width; ++x)
                {
                    // copy to pixel array and take the opportunity to swap the pixel RGB values
                    if (pixelSize == 3)
                    {
                        pPixels[yPos + (x * 3)]     = pLineRGB[x].rgbtRed;
                        pPixels[yPos + (x * 3) + 1] = pLineRGB[x].rgbtGreen;
                        pPixels[yPos + (x * 3) + 2] = pLineRGB[x].rgbtBlue;
                    }
                    else
                    {
                        pPixels[yPos + (x * 4)]     = pLineRGBA[x].rgbRed;
                        pPixels[yPos + (x * 4) + 1] = pLineRGBA[x].rgbGreen;
                        pPixels[yPos + (x * 4) + 2] = pLineRGBA[x].rgbBlue;
                        pPixels[yPos + (x * 4) + 3] = pLineRGBA[x].rgbReserved;
                    }
                }
            }

            // set the texture
            result = SetTexture(pBitmap->Width, pBitmap->Height, pixelType, false, pPixels);
        }
        __finally
        {
            if (pPixels)
                delete[] pPixels;
        }

        // succeeded?
        if (!result)
            return false;
    }
    else
    if (!m_pMDL || !m_pMDL->m_TextureCount || m_pMDL->m_pTexture[0].m_TextureID == M_CSR_Error_Code)
    {
        const int pixelWidth  = 1;
        const int pixelHeight = 1;

        // create a pixel array of just 1 pixel, to create a colored texture (NOTE done this way to
        // avoid to use 2 different shaders to draw colored and textured models, or to use a too
        // complex shader)
        unsigned pixels[1];
        pixels[0] = color;

        // set the texture
        if (!SetTexture(pixelWidth, pixelHeight, GL_RGB, false, (unsigned char*)&pixels))
            return false;
    }

    // do use a bump map?
    if (!bumpMapFileName.empty())
    {
        // load the texture image
        std::auto_ptr<TPicture> pPicture(new TPicture());
        pPicture->LoadFromFile(bumpMapFileName.c_str());

        // convert it to bitmap
        std::auto_ptr<TBitmap> pBitmap(new TBitmap());
        pBitmap->Assign(pPicture->Graphic);

        int   pixelSize;
        GLint pixelType;

        // search for bitmap pixel format
        switch (pBitmap->PixelFormat)
        {
            case pf24bit: pixelSize = 3; pixelType = GL_RGB;  break;
            case pf32bit: pixelSize = 4; pixelType = GL_RGBA; break;
            default:      return false;
        }

        unsigned char* pPixels = NULL;
        bool           result  = false;

        try
        {
            // reserve memory for the pixel array
            pPixels = new unsigned char[pBitmap->Width * pBitmap->Height * pixelSize];

            TRGBTriple* pLineRGB;
            TRGBQuad*   pLineRGBA;

            // iterate through lines to copy
            for (int y = 0; y < pBitmap->Height; ++y)
            {
                // get the next pixel line from bitmap
                if (pixelSize == 3)
                    pLineRGB  = static_cast<TRGBTriple*>(pBitmap->ScanLine[y]);
                else
                    pLineRGBA = static_cast<TRGBQuad*>(pBitmap->ScanLine[y]);

                int yPos;

                // calculate the start y position
                if (type == CSR_DesignerHelper::IE_MT_Model)
                    yPos = y * pBitmap->Width * pixelSize;
                else
                    yPos = ((pBitmap->Height - 1) - y) * pBitmap->Width * pixelSize;

                // iterate through pixels to copy
                for (int x = 0; x < pBitmap->Width; ++x)
                {
                    // copy to pixel array and take the opportunity to swap the pixel RGB values
                    if (pixelSize == 3)
                    {
                        pPixels[yPos + (x * 3)]     = pLineRGB[x].rgbtRed;
                        pPixels[yPos + (x * 3) + 1] = pLineRGB[x].rgbtGreen;
                        pPixels[yPos + (x * 3) + 2] = pLineRGB[x].rgbtBlue;
                    }
                    else
                    {
                        pPixels[yPos + (x * 4)]     = pLineRGBA[x].rgbRed;
                        pPixels[yPos + (x * 4) + 1] = pLineRGBA[x].rgbGreen;
                        pPixels[yPos + (x * 4) + 2] = pLineRGBA[x].rgbBlue;
                        pPixels[yPos + (x * 4) + 3] = pLineRGBA[x].rgbReserved;
                    }
                }
            }

            // set the bump map
            result = SetTexture(pBitmap->Width, pBitmap->Height, pixelType, true, pPixels);
        }
        __finally
        {
            if (pPixels)
                delete[] pPixels;
        }

        // succeeded?
        if (!result)
            return false;
    }

    // draw the scene
    DrawScene();

    return true;
}
//---------------------------------------------------------------------------
bool TScreenshotFrame::GetScreenshot(TBitmap* pBitmap) const
{
    // no bitmap to export to?
    if (!pBitmap)
        return false;

    // configure the bitmap for the screenshot
    pBitmap->PixelFormat = pf24bit;
    pBitmap->SetSize(imScreenshot->Width, imScreenshot->Height);

    // export the screenshot
    return DrawScene(pBitmap);
}
//---------------------------------------------------------------------------
bool TScreenshotFrame::CreateScene(CSR_DesignerHelper::IEModelType type,
                                   int                             width,
                                   int                             height,
                                   unsigned                        color)
{
    // initialize OpenGL to draw the screenshot scene
    if (!InitializeScene(width, height))
        return false;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    CSR_Material sm;
    sm.m_Color       = color;
    sm.m_Transparent = 0;
    sm.m_Wireframe   = 0;

    // select the kind of model to load
    switch (type)
    {
        case CSR_DesignerHelper::IE_MT_Surface:  m_pMesh = csrShapeCreateSurface(1.0f, 1.0f, &vf, 0, &sm, 0);                                        break;
        case CSR_DesignerHelper::IE_MT_Box:      m_pMesh = csrShapeCreateBox(1.0f, 1.0f, 1.0f, 0, &vf, 0, &sm, 0);                                   break;
        case CSR_DesignerHelper::IE_MT_Sphere:   m_pMesh = csrShapeCreateSphere(1.0f, 20, 20, &vf, 0, &sm, 0);                                       break;
        case CSR_DesignerHelper::IE_MT_Cylinder: m_pMesh = csrShapeCreateCylinder(1.0f, 2.0f, 20, &vf, 0, &sm, 0);                                   break;
        case CSR_DesignerHelper::IE_MT_Disk:     m_pMesh = csrShapeCreateDisk(0.0f, 0.0f, 1.0f, 20, &vf, 0, &sm, 0);                                 break;
        case CSR_DesignerHelper::IE_MT_Ring:     m_pMesh = csrShapeCreateRing(0.0f, 0.0f, 0.5f, 1.0f, 20, &vf, 0, &sm, 0);                           break;
        case CSR_DesignerHelper::IE_MT_Spiral:   m_pMesh = csrShapeCreateSpiral(0.0f, 0.0f, 10.0f, 20.0f, 0.0f, 0.0f, 1.0f, 20, 10, &vf, 0, &sm, 0); break;

        default:
            // other models type cannot be opened this way
            return false;
    }

    if (!m_pMesh)
        return false;

    // create the aligned-axis bounding box tree for the model
    m_pAABBTree = csrAABBTreeFromMesh(m_pMesh);

    // set the scene to his default state
    ResetScene();

    return true;
}
//---------------------------------------------------------------------------
bool TScreenshotFrame::CreateScene(const std::string& fileName,
                                         int          width,
                                         int          height,
                                         unsigned     color)
{
    // file exists?
    if (fileName.empty() || !::FileExists(UnicodeString(AnsiString(fileName.c_str())), false))
        return false;

    // initialize OpenGL to draw the screenshot scene
    if (!InitializeScene(width, height))
        return false;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    CSR_Material sm;
    sm.m_Color       = color;
    sm.m_Transparent = 0;
    sm.m_Wireframe   = 0;

    // load the model
    if (fileName.rfind("mdl") == fileName.length() - 3)
    {
        // open the model
        m_pMDL = csrMDLOpen(fileName.c_str(), 0, &vf, NULL, &sm, NULL, NULL);

        // succeeded?
        if (!m_pMDL || !m_pMDL->m_ModelCount || !m_pMDL->m_pModel->m_MeshCount)
            return false;

        // create the aligned-axis bounding box tree for the model
        m_pAABBTree = csrAABBTreeFromMesh(&m_pMDL->m_pModel[0].m_pMesh[0]);
    }
    else
    if (fileName.rfind("obj") == fileName.length() - 3)
    {
        // open the model
        m_pModel = csrWaveFrontOpen(fileName.c_str(), &vf, NULL, &sm, NULL, NULL);

        // succeeded?
        if (!m_pModel || !m_pModel->m_MeshCount)
            return false;

        // create the aligned-axis bounding box tree for the model
        m_pAABBTree = csrAABBTreeFromMesh(&m_pModel->m_pMesh[0]);
    }
    else
        // unknown or unsupported model type
        return false;

    // set the scene to his default state
    ResetScene();

    return true;
}
//---------------------------------------------------------------------------
void TScreenshotFrame::ResetScene()
{
    try
    {
        m_Reseting = true;

        // initialize the arcball default values
        m_ArcBall.m_AngleX = 0.0f;
        m_ArcBall.m_AngleY = 0.0f;
        m_ArcBall.m_Radius = 1.0f;

        // initialize the camera default values
        m_Camera.m_Position.m_X = 0.0f;
        m_Camera.m_Position.m_Y = 0.0f;
        m_Camera.m_Position.m_Z = 0.0f;
        m_Camera.m_xAngle       = 0.0f;
        m_Camera.m_yAngle       = 0.0f;
        m_Camera.m_zAngle       = 0.0f;
        m_Camera.m_Factor.m_X   = 1.0f;
        m_Camera.m_Factor.m_Y   = 1.0f;
        m_Camera.m_Factor.m_Z   = 1.0f;
        m_Camera.m_MatCombType  =  IE_CT_Scale_Rotate_Translate;

        // update the interface
        imScreenshot->Picture->Assign(NULL);
        paColorValue->Color = clSilver;
        rbArcball->Checked  = true;

        // configure the scene color
        csrRGBAToColor(csrColorBGRToRGBA(::ColorToRGB(paColorValue->Color)), &m_SceneColor);

        // get a model matrix which make the model to fit the viewport. NOTE the field of view is 45°
        if (m_pAABBTree)
            m_ModelMatrix = CSR_OpenGLHelper::FitModelInView(m_pAABBTree->m_pBox, M_PI / 8.0f, m_pMDL);
        else
            csrMat4Identity(&m_ModelMatrix);

        // get the offset to use to position the model (1/10 if the camera distance)
        m_Offset = std::fabs(m_ModelMatrix.m_Table[3][2]) / 10.0f;

        // for the arcball mode, replace the model position by the arcball radius
        m_ArcBall.m_Radius          = -m_ModelMatrix.m_Table[3][2];
        m_ModelMatrix.m_Table[3][2] =  0.0f;

        // connect the model view matrix to shader
        const GLint modelMatrixSlot = glGetUniformLocation(m_pSceneShader->m_ProgramID, "csr_uModel");
        glUniformMatrix4fv(modelMatrixSlot, 1, 0, &m_ModelMatrix.m_Table[0][0]);
    }
    __finally
    {
        m_Reseting = false;
    }
}
//---------------------------------------------------------------------------
bool TScreenshotFrame::InitializeScene(int width, int height)
{
    // release the previous scene, if exists
    ReleaseScene();

    // calculate rendering surface size (higher than the wished one to support the antialiasing)
    const int drawWidth  = width  * m_AntialiasingFactor;
    const int drawHeight = height * m_AntialiasingFactor;

    // create overlay render surface (cannot use a bitmap directly, unfortunately, unless limiting
    // the rendering to OpenGL 1.1)
    m_pSceneOverlayForm               = new TForm((TComponent*)NULL);
    m_pSceneOverlayForm->ClientWidth  = drawWidth;
    m_pSceneOverlayForm->ClientHeight = drawHeight;
    m_pSceneOverlayForm->Visible      = false;

    // initialize OpenGL
    CSR_OpenGLHelper::EnableOpenGL(m_pSceneOverlayForm->Handle,
                                  &m_SceneContext.m_hDC,
                                  &m_SceneContext.m_hRC);

    const std::string vsTextured = CSR_DesignerHelper::GetVertexShader(CSR_DesignerHelper::IE_ST_Texture);
    const std::string fsTextured = CSR_DesignerHelper::GetFragmentShader(CSR_DesignerHelper::IE_ST_Texture);

    // load the shader
    m_pSceneShader = csrShaderLoadFromStr(vsTextured.c_str(),
                                          vsTextured.length(),
                                          fsTextured.c_str(),
                                          fsTextured.length(),
                                          NULL,
                                          NULL);

    if (!m_pSceneShader)
        return false;

    csrShaderEnable(m_pSceneShader);

    // get shader attributes
    m_pSceneShader->m_VertexSlot   = glGetAttribLocation(m_pSceneShader->m_ProgramID, "csr_aVertices");
    m_pSceneShader->m_ColorSlot    = glGetAttribLocation(m_pSceneShader->m_ProgramID, "csr_aColor");
    m_pSceneShader->m_TexCoordSlot = glGetAttribLocation(m_pSceneShader->m_ProgramID, "csr_aTexCoord");

    CSR_Matrix4 matrix;

    // create a viewport
    CSR_OpenGLHelper::CreateViewport(drawWidth, drawHeight, m_pSceneShader, matrix);

    // configure OpenGL
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    return true;
}
//---------------------------------------------------------------------------
void TScreenshotFrame::ReleaseScene()
{
    // enable screenshot context (required to release only the objects belonging to it)
    wglMakeCurrent(m_SceneContext.m_hDC, m_SceneContext.m_hRC);

    // release the currently opened model tree
    csrAABBTreeNodeRelease(m_pAABBTree);
    m_pAABBTree = NULL;

    // release the currently opened mesh
    csrMeshRelease(m_pMesh);
    m_pMesh = NULL;

    // release the currently opened model
    csrModelRelease(m_pModel);
    m_pModel = NULL;

    // release the currently opened MDL model
    csrMDLRelease(m_pMDL);
    m_pMDL = NULL;

    // delete the scene shader
    csrShaderRelease(m_pSceneShader);
    m_pSceneShader = NULL;

    // delete the scene
    if (m_pSceneOverlayForm)
    {
        // shutdown OpenGL
        CSR_OpenGLHelper::DisableOpenGL(m_pSceneOverlayForm->Handle,
                                        m_SceneContext.m_hDC,
                                        m_SceneContext.m_hRC);

        m_SceneContext.m_hDC = NULL;
        m_SceneContext.m_hRC = NULL;

        // delete the scene overlay
        delete m_pSceneOverlayForm;
        m_pSceneOverlayForm = NULL;
    }
}
//---------------------------------------------------------------------------
bool TScreenshotFrame::DrawScene(TBitmap* pBitmap) const
{
    // no bitmap to draw to?
    if (!pBitmap)
        return false;

    // no scene shader?
    if (!m_pSceneShader)
        return false;

    // scene context was initialized?
    if (!m_pSceneOverlayForm || !m_SceneContext.m_hDC || !m_SceneContext.m_hRC)
        return false;

    // enable view context
    wglMakeCurrent(m_SceneContext.m_hDC, m_SceneContext.m_hRC);

    // clear scene
    glClearColor(m_SceneColor.m_R, m_SceneColor.m_G, m_SceneColor.m_B, m_SceneColor.m_A);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    CSR_Matrix4 viewMatrix;

    // initialize the view matrix to identity
    csrMat4Identity(&viewMatrix);

    // get the view matrix to use
    if (rbArcball->Checked)
        csrSceneArcBallToMatrix(&m_ArcBall, &viewMatrix);
    else
        csrSceneCameraToMatrix(&m_Camera, &viewMatrix);

    // connect view matrix to shader
    const GLint viewUniform = glGetUniformLocation(m_pSceneShader->m_ProgramID, "csr_uView");
    glUniformMatrix4fv(viewUniform, 1, 0, &viewMatrix.m_Table[0][0]);

    // draw the model
    if (m_pMesh)
        csrDrawMesh(m_pMesh, m_pSceneShader, 0);
    else
    if (m_pMDL)
        csrDrawMDL(m_pMDL, m_pSceneShader, 0, 0, 0, 0);
    else
    if (m_pModel)
        csrDrawModel(m_pModel, 0, m_pSceneShader, 0);
    else
        return false;

    glFlush();

    // create image overlay
    std::auto_ptr<TBitmap> pOverlay(new TBitmap());
    CSR_OpenGLHelper::GetBitmapFromOpenGL(pOverlay.get());

    // create antialiased final image overlay
    std::auto_ptr<TBitmap> pAntialiasedOverlay(new TBitmap());

    // apply 4x4 antialiasing on the rendered image
    CSR_VCLHelper::ApplyAntialiasing(pOverlay.get(), pAntialiasedOverlay.get(), m_AntialiasingFactor);

    // copy final image
    pBitmap->Assign(pAntialiasedOverlay.get());

    return true;
}
//---------------------------------------------------------------------------
bool TScreenshotFrame::DrawScene() const
{
    std::auto_ptr<TBitmap> pBitmap(new TBitmap());

    // draw the model in a bitmap
    if (!DrawScene(pBitmap.get()))
    {
        imScreenshot->Picture->Assign(NULL);
        return false;
    }

    // show the model in the preview image
    imScreenshot->Picture->Assign(pBitmap.get());
    return true;
}
//---------------------------------------------------------------------------
bool TScreenshotFrame::SetTexture(      int            width,
                                        int            height,
                                        int            pixelType,
                                        bool           bumpMap,
                                  const unsigned char* pPixels) const
{
    // create new OpenGL texture
    if (bumpMap)
    {
        if (m_pMesh)
        {
            glGenTextures(1, &m_pMesh->m_Shader.m_BumpMapID);
            glBindTexture(GL_TEXTURE_2D, m_pMesh->m_Shader.m_BumpMapID);
        }
        else
        if (m_pModel)
        {
            glGenTextures(1, &m_pModel->m_pMesh[0].m_Shader.m_BumpMapID);
            glBindTexture(GL_TEXTURE_2D, m_pModel->m_pMesh[0].m_Shader.m_BumpMapID);
        }
        else
        if (m_pMDL)
        {
            // MDL model contains a texture list?
            if (!m_pMDL->m_TextureCount)
            {
                // no, creates one
                m_pMDL->m_pTexture = (CSR_ModelTexture*)malloc(sizeof(CSR_ModelTexture));

                // succeeded?
                if (!m_pMDL->m_pTexture)
                    return false;

                // configure the newly created texture list
                m_pMDL->m_pTexture[0].m_TextureID = M_CSR_Error_Code;
                m_pMDL->m_pTexture[0].m_BumpMapID = M_CSR_Error_Code;
                m_pMDL->m_TextureCount            = 1;
            }

            glGenTextures(1, &m_pMDL->m_pTexture[0].m_BumpMapID);
            glBindTexture(GL_TEXTURE_2D, m_pMDL->m_pTexture[0].m_BumpMapID);
        }
    }
    else
    {
        if (m_pMesh)
        {
            glGenTextures(1, &m_pMesh->m_Shader.m_TextureID);
            glBindTexture(GL_TEXTURE_2D, m_pMesh->m_Shader.m_TextureID);
        }
        else
        if (m_pModel)
        {
            glGenTextures(1, &m_pModel->m_pMesh[0].m_Shader.m_TextureID);
            glBindTexture(GL_TEXTURE_2D, m_pModel->m_pMesh[0].m_Shader.m_TextureID);
        }
        else
        if (m_pMDL)
        {
            // MDL model contains a texture list?
            if (!m_pMDL->m_TextureCount)
            {
                // no, creates one
                m_pMDL->m_pTexture = (CSR_ModelTexture*)malloc(sizeof(CSR_ModelTexture));

                // succeeded?
                if (!m_pMDL->m_pTexture)
                    return false;

                // configure the newly created texture list
                m_pMDL->m_pTexture[0].m_TextureID = M_CSR_Error_Code;
                m_pMDL->m_pTexture[0].m_BumpMapID = M_CSR_Error_Code;
                m_pMDL->m_TextureCount            = 1;
            }

            glGenTextures(1, &m_pMDL->m_pTexture[0].m_TextureID);
            glBindTexture(GL_TEXTURE_2D, m_pMDL->m_pTexture[0].m_TextureID);
        }
    }

    // set texture filtering
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // set texture wrapping mode
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // generate texture from pixel array
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 pixelType,
                 width,
                 height,
                 0,
                 pixelType,
                 GL_UNSIGNED_BYTE,
                 pPixels);

    return true;
}
//---------------------------------------------------------------------------
