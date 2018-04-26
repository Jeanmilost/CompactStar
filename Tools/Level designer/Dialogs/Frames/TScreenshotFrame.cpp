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
#include "CSR_DesignerHelper.h"

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
    m_pMDL(NULL),
    m_pModel(NULL),
    m_pAABBTree(NULL),
    m_AntialiasingFactor(4)
{}
//---------------------------------------------------------------------------
__fastcall TScreenshotFrame::~TScreenshotFrame()
{
    ReleaseScene();
}
//---------------------------------------------------------------------------
bool TScreenshotFrame::LoadModel(const std::string& fileName)
{
    // create a scene to draw the model screenshot
    if (!CreateScene(fileName.c_str(), imScreenshot->Width, imScreenshot->Height))
        return false;

    std::auto_ptr<TBitmap> pBitmap(new TBitmap());

    // draw the model in a bitmap
    if (!DrawScene(pBitmap.get()))
        return false;

    // show the loaded model
    imScreenshot->Picture->Assign(pBitmap.get());

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
void __fastcall TScreenshotFrame::paColorValueClick(TObject *Sender)
{
    cdColor->Color = paColorValue->Color;

    // notify user about selecting a color
    if (!cdColor->Execute())
        return;

    paColorValue->Color = cdColor->Color;

    // update the scene color
    csrRGBAToColor(csrColorBGRToRGBA(::ColorToRGB(paColorValue->Color)), &m_SceneColor);

    std::auto_ptr<TBitmap> pBitmap(new TBitmap());

    // redraw the model screenshot
    if (DrawScene(pBitmap.get()))
        imScreenshot->Picture->Assign(pBitmap.get());
}
//---------------------------------------------------------------------------
bool TScreenshotFrame::CreateScene(const std::string& fileName, int width, int height)
{
    // file exists?
    if (fileName.empty() || !::FileExists(UnicodeString(AnsiString(fileName.c_str())), false))
        return false;

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

    // initialize the view matrix to identity
    csrMat4Identity(&m_ViewMatrix);

    // configure the scene color
    m_SceneColor.m_R = 0.0f;
    m_SceneColor.m_G = 0.0f;
    m_SceneColor.m_B = 0.0f;
    m_SceneColor.m_A = 1.0f;

    // configure OpenGL
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    bool isWaveFront;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    CSR_Material sm;
    sm.m_Color       = 0xFFFFFFFF;
    sm.m_Transparent = 0;
    sm.m_Wireframe   = 0;

    // load the model
    if (fileName.rfind("mdl") == fileName.length() - 3)
    {
        m_pMDL = csrMDLOpen(fileName.c_str(), 0, &vf, NULL, &sm, NULL, NULL);

        if (m_pMDL && m_pMDL->m_ModelCount && m_pMDL->m_pModel->m_MeshCount)
            m_pAABBTree = csrAABBTreeFromMesh(&m_pMDL->m_pModel[0].m_pMesh[0]);

        isWaveFront = false;
    }
    else
    if (fileName.rfind("obj") == fileName.length() - 3)
    {
        m_pModel = csrWaveFrontOpen(fileName.c_str(), &vf, NULL, &sm, NULL, NULL);

        if (m_pModel && m_pModel->m_MeshCount)
            m_pAABBTree = csrAABBTreeFromMesh(&m_pModel->m_pMesh[0]);

        isWaveFront = true;
    }

    // get the best model matrix to show the model centered in the scene
    matrix = CSR_OpenGLHelper::GetBestModelMatrix( m_pAABBTree ? m_pAABBTree->m_pBox : NULL,
                                                  -5.0f,
                                                  !isWaveFront);

    // connect the model view matrix to shader
    const GLint modelMatrixSlot = glGetUniformLocation(m_pSceneShader->m_ProgramID, "csr_uModel");
    glUniformMatrix4fv(modelMatrixSlot, 1, 0, &matrix.m_Table[0][0]);

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

    // connect view matrix to shader
    const GLint viewUniform = glGetUniformLocation(m_pSceneShader->m_ProgramID, "csr_uView");
    glUniformMatrix4fv(viewUniform, 1, 0, &m_ViewMatrix.m_Table[0][0]);

    // draw the model
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
