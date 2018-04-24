/*****************************************************************************
 * ==> TAddItemDialog -------------------------------------------------------*
 *****************************************************************************
 * Description : This module contains a toolbox dialog to add a scene item   *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#include <vcl.h>
#pragma hdrstop
#include "TAddItemDialog.h"

// std
#include <memory>

// compactStar engine
#include "CSR_Renderer.h"

// classes
#include "CSR_VCLHelper.h"
#include "CSR_OpenGLHelper.h"
#include "CSR_DesignerHelper.h"

#pragma package(smart_init)
#pragma link "TTextureSelectionFrame"
#pragma link "TVertexColorFrame"
#pragma link "TFileFrame"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TAddItemDialog
//---------------------------------------------------------------------------
TAddItemDialog* AddItemDialog;
//---------------------------------------------------------------------------
__fastcall TAddItemDialog::TAddItemDialog(TComponent* pOwner) :
    TForm(pOwner),
    m_ModelType(IE_MT_Unknown)
{
    CSR_VCLHelper::ChangeTabsVisibility(pcWizard, tsSelectItem, false);

    ffModelFile->Set_OnFileSelected(OnFileSelected);
}
//---------------------------------------------------------------------------
__fastcall TAddItemDialog::~TAddItemDialog()
{}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::FormShow(TObject* pSender)
{
    pcWizard->ClientHeight = btSelectItemAddModel->Top             +
                             btSelectItemAddModel->Height          +
                             btSelectItemAddModel->Margins->Bottom +
                             8;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btCancelClick(TObject* pSender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btBackClick(TObject* pSender)
{
    btOK->Enabled = false;

    if (pcWizard->ActivePage == tsConfig)
    {
        if (m_ModelType == IE_MT_Model)
        {
            btNext->Enabled      = false;
            pcWizard->ActivePage = tsModel;
            return;
        }

        m_ModelType          = IE_MT_Unknown;
        btBack->Enabled      = false;
        btNext->Enabled      = true;
        pcWizard->ActivePage = tsSelectItem;
    }
    else
    if (pcWizard->ActivePage == tsModel)
    {
        m_ModelType          = IE_MT_Unknown;
        btBack->Enabled      = false;
        btNext->Enabled      = true;
        pcWizard->ActivePage = tsSelectItem;
    }
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnNextClick(TObject* pSender)
{
    // search for selected page
    if (pcWizard->ActivePage == tsSelectItem)
    {
        btBack->Enabled = true;
        btOK->Enabled   = false;

        // search for selected item
        if (btSelectItemAddSurface->Down)
        {
            m_ModelType          = IE_MT_Surface;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddBox->Down)
        {
            m_ModelType          = IE_MT_Box;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddSphere->Down)
        {
            m_ModelType          = IE_MT_Sphere;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddCylinder->Down)
        {
            m_ModelType          = IE_MT_Cylinder;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddDisk->Down)
        {
            m_ModelType          = IE_MT_Disk;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddRing->Down)
        {
            m_ModelType          = IE_MT_Ring;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddSpiral->Down)
        {
            m_ModelType          = IE_MT_Spiral;
            pcWizard->ActivePage = tsConfig;
        }
        else
        if (btSelectItemAddModel->Down)
        {
            m_ModelType          = IE_MT_Model;
            btNext->Enabled      = ModelFileExists();
            pcWizard->ActivePage = tsModel;
        }
    }
    else
    if (pcWizard->ActivePage == tsModel)
        pcWizard->ActivePage = tsConfig;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::btOKClick(TObject* pSender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnSelectItemButtonClick(TObject* pSender)
{
    btNext->Enabled =
            (pcWizard->ActivePage == tsSelectItem || (pcWizard->ActivePage == tsModel && ModelFileExists()));
}
//---------------------------------------------------------------------------
TAddItemDialog::IEModelType TAddItemDialog::GetModelType() const
{
    return m_ModelType;
}
//---------------------------------------------------------------------------
bool TAddItemDialog::ModelFileExists() const
{
    return (!ffModelFile->edFileName->Text.IsEmpty() &&
           ::FileExists(ffModelFile->edFileName->Text.c_str(), false));
}
//---------------------------------------------------------------------------
void __fastcall TAddItemDialog::OnFileSelected(TObject* pSender, const std::wstring& fileName)
{
    const bool modelExists = ModelFileExists();

    btNext->Enabled = modelExists;

    if (modelExists)
    {
        std::unique_ptr<TBitmap> pBitmap(new TBitmap());

        CSR_Matrix4 viewMatrix;

        // create the view matrix
        csrMat4Identity(&viewMatrix);

        // draw the model in a bitmap
        DrawModelToBitmap(AnsiString(ffModelFile->edFileName->Text).c_str(),
                          imScreenshot->Width,
                          imScreenshot->Height,
                          viewMatrix,
                          pBitmap.get());

        // show the loaded model
        imScreenshot->Picture->Assign(pBitmap.get());
    }
}
//--------------------------------------------------------------------------------------------------
void TAddItemDialog::DrawModelToBitmap(const std::string& fileName,
                                             int          width,
                                             int          height,
                                             CSR_Matrix4  viewMatrix,
                                             TBitmap*     pBitmap)
{
    // file exists?
    if (fileName.empty() || !::FileExists(UnicodeString(AnsiString(fileName.c_str())), false))
        return;

    // no bitmap to draw to?
    if (!pBitmap)
        return;

    // calculate rendering surface width and height (4x higher to allow 4x4 antialiasing to be
    // applied later)
    const int drawWidth  = width  * 4;
    const int drawHeight = height * 4;

    // create overlay render surface (cannot use a bitmap directly, unfortunately)
    std::unique_ptr<TForm> pOverlayForm(new TForm((TComponent*)NULL));
    pOverlayForm->ClientWidth  = drawWidth;
    pOverlayForm->ClientHeight = drawHeight;
    pOverlayForm->Visible      = false;

    HDC   hDC;
    HGLRC hRC;

    // initialize OpenGL
    CSR_OpenGLHelper::EnableOpenGL(pOverlayForm->Handle, &hDC, &hRC);

    CSR_Shader*   pShader   = NULL;
    CSR_MDL*      pMDL      = NULL;
    CSR_Model*    pModel    = NULL;
    CSR_AABBNode* pAABBTree = NULL;

    try
    {
        // configure OpenGL
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LEQUAL);
        glDepthRangef(0.0f, 1.0f);

        const std::string vsTextured = CSR_DesignerHelper::GetVertexShader(CSR_DesignerHelper::IE_ST_Texture);
        const std::string fsTextured = CSR_DesignerHelper::GetFragmentShader(CSR_DesignerHelper::IE_ST_Texture);

        // enable view context
        wglMakeCurrent(hDC, hRC);

        // load the shader
        pShader = csrShaderLoadFromStr(vsTextured.c_str(),
                                       vsTextured.length(),
                                       fsTextured.c_str(),
                                       fsTextured.length(),
                                       NULL,
                                       NULL);

        if (!pShader)
            return;

        csrShaderEnable(pShader);

        // get shader attributes
        pShader->m_VertexSlot   = glGetAttribLocation(pShader->m_ProgramID, "csr_aVertices");
        pShader->m_ColorSlot    = glGetAttribLocation(pShader->m_ProgramID, "csr_aColor");
        pShader->m_TexCoordSlot = glGetAttribLocation(pShader->m_ProgramID, "csr_aTexCoord");

        bool rotated = false;

        CSR_VertexFormat vf;
        vf.m_HasNormal         = 0;
        vf.m_HasTexCoords      = 1;
        vf.m_HasPerVertexColor = 1;

        CSR_Material sm;
        sm.m_Color       = 0xFFFFFFFF;
        sm.m_Transparent = 0;
        sm.m_Wireframe   = 0;

        // do load the model?
        if (!pModel && !pMDL)
            if (fileName.rfind("mdl") == fileName.length() - 3)
            {
                pMDL = csrMDLOpen(fileName.c_str(), 0, &vf, NULL, &sm, NULL, NULL);

                if (pMDL && pMDL->m_ModelCount && pMDL->m_pModel->m_MeshCount)
                    pAABBTree = csrAABBTreeFromMesh(&pMDL->m_pModel[0].m_pMesh[0]);

                rotated = true;
            }
            else
            if (fileName.rfind("obj") == fileName.length() - 3)
            {
                pModel = csrWaveFrontOpen(fileName.c_str(), &vf, NULL, &sm, NULL, NULL);

                if (pModel && pModel->m_MeshCount)
                    pAABBTree = csrAABBTreeFromMesh(&pModel->m_pMesh[0]);

                rotated = false;
            }

        CSR_Matrix4 matrix;

        // create a viewport
        CSR_OpenGLHelper::CreateViewport(drawWidth, drawHeight, pShader, matrix);

        // connect view matrix to shader
        const GLint viewUniform = glGetUniformLocation(pShader->m_ProgramID, "csr_uView");
        glUniformMatrix4fv(viewUniform, 1, 0, &viewMatrix.m_Table[0][0]);

        // get the best model matrix to show the model centered in the scene
        matrix = CSR_OpenGLHelper::GetBestModelMatrix( pAABBTree ? pAABBTree->m_pBox : NULL,
                                                      -5.0f,
                                                       rotated);

        // connect the model view matrix to shader
        GLint modelMatrixSlot = glGetUniformLocation(pShader->m_ProgramID, "csr_uModel");
        glUniformMatrix4fv(modelMatrixSlot, 1, 0, &matrix.m_Table[0][0]);

        // clear scene
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw the model
        if (pMDL)
            csrDrawMDL(pMDL, pShader, 0, 0, 0, 0);
        else
        if (pModel)
            csrDrawModel(pModel, 0, pShader, 0);

        glFlush();

        // create image overlay
        std::auto_ptr<TBitmap> pOverlay(new TBitmap());
        CSR_OpenGLHelper::GetBitmapFromOpenGL(pOverlay.get());

        // create antialiased final image overlay
        std::auto_ptr<TBitmap> pAntialiasedOverlay(new TBitmap());

        // apply 4x4 antialiasing on the rendered image
        CSR_OpenGLHelper::ApplyAntialiasing(pOverlay.get(), pAntialiasedOverlay.get(), 4);

        // copy final image
        pBitmap->Assign(pAntialiasedOverlay.get());
    }
    __finally
    {
        csrAABBTreeNodeRelease(pAABBTree);
        csrModelRelease(pModel);
        csrMDLRelease(pMDL);
        csrShaderRelease(pShader);

        // shutdown OpenGL
        CSR_OpenGLHelper::DisableOpenGL(pOverlayForm->Handle, hDC, hRC);
    }
}
//---------------------------------------------------------------------------
