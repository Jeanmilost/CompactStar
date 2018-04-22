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
    btNext->Enabled = ModelFileExists();
}
//--------------------------------------------------------------------------------------------------
void TAddItemDialog::DrawModelToBitmap(const std::string& fileName, int width, int height, TBitmap* pBitmap)
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

    CSR_Shader* pShader = NULL;
    CSR_MDL*    pModel  = NULL;

    try
    {
        // configure OpenGL
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_TEXTURE_2D);

        const std::string vsTextured = CSR_DesignerHelper::GetVertexShader(CSR_DesignerHelper::IE_ST_Texture);
        const std::string fsTextured = CSR_DesignerHelper::GetFragmentShader(CSR_DesignerHelper::IE_ST_Texture);

        // enable view context
        wglMakeCurrent(hDC, hRC);

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

        CSR_VertexFormat vf;
        vf.m_HasNormal         = 0;
        vf.m_HasTexCoords      = 1;
        vf.m_HasPerVertexColor = 1;

        CSR_Material sm;
        sm.m_Color       = 0xFFFF;
        sm.m_Transparent = 0;
        sm.m_Wireframe   = 0;

        pModel = csrMDLOpen(fileName.c_str(), 0, &vf, NULL, &sm, NULL, NULL);

        CSR_Matrix4 matrix;

        // create a viewport
        CSR_OpenGLHelper::CreateViewport(drawWidth, drawHeight, pShader, matrix);

        // create the view matrix
        csrMat4Identity(&matrix);

        // connect view matrix to shader
        const GLint viewUniform = glGetUniformLocation(pShader->m_ProgramID, "csr_uView");
        glUniformMatrix4fv(viewUniform, 1, 0, &matrix.m_Table[0][0]);

        float       angle;
        CSR_Vector3 t;
        CSR_Vector3 r;
        CSR_Vector3 factor;
        CSR_Matrix4 translateMatrix;
        CSR_Matrix4 rotateMatrixX;
        CSR_Matrix4 rotateMatrixY;
        CSR_Matrix4 rotateMatrixZ;
        CSR_Matrix4 scaleMatrix;
        CSR_Matrix4 combinedMatrix1;
        CSR_Matrix4 combinedMatrix2;
        CSR_Matrix4 combinedMatrix3;

        // set translation
        t.m_X =  0.0f;
        t.m_Y =  0.0f;
        t.m_Z = -2.0f;

        csrMat4Translate(&t, &translateMatrix);

        // set rotation axis
        r.m_X = 1.0f;
        r.m_Y = 0.0f;
        r.m_Z = 0.0f;

        // set rotation angle
        angle = -M_PI * 0.5f;

        csrMat4Rotate(angle, &r, &rotateMatrixX);

        // set rotation axis
        r.m_X = 0.0f;
        r.m_Y = 1.0f;
        r.m_Z = 0.0f;

        // set rotation angle
        angle = 0.0f;

        csrMat4Rotate(angle, &r, &rotateMatrixY);

        // set rotation axis
        r.m_X = 0.0f;
        r.m_Y = 0.0f;
        r.m_Z = 1.0f;

        // set rotation angle
        angle = 0.0f;

        csrMat4Rotate(angle, &r, &rotateMatrixZ);

        // set scale factor
        factor.m_X = 0.05f;
        factor.m_Y = 0.05f;
        factor.m_Z = 0.05f;

        csrMat4Scale(&factor, &scaleMatrix);

        // calculate model view matrix
        csrMat4Multiply(&scaleMatrix,     &rotateMatrixX,   &combinedMatrix1);
        csrMat4Multiply(&combinedMatrix1, &rotateMatrixY,   &combinedMatrix2);
        csrMat4Multiply(&combinedMatrix2, &rotateMatrixZ,   &combinedMatrix3);
        csrMat4Multiply(&combinedMatrix3, &translateMatrix, &matrix);

        // connect the model view matrix to shader
        GLint modelMatrixSlot = glGetUniformLocation(pShader->m_ProgramID, "csr_uModel");
        glUniformMatrix4fv(modelMatrixSlot, 1, 0, &matrix.m_Table[0][0]);

        // clear scene
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw the model
        csrDrawMDL(pModel, pShader, 0, 0, 0, 0);

        if (pOverlayForm->Visible)
            ::SwapBuffers(hDC);
        else
        {
            glFlush();

            // create image overlay
            std::auto_ptr<TBitmap> pOverlay(new TBitmap());
            GetBitmapFromOpenGL(pOverlay.get());

            // create antialiased final image overlay
            std::auto_ptr<TBitmap> pAntialiasedOverlay(new TBitmap());

            // apply 4x4 antialiasing on the rendered image
            ApplyAntialiasing(pOverlay.get(), pAntialiasedOverlay.get(), 4);

            // copy final image
            pBitmap->Assign(pAntialiasedOverlay.get());
        }
    }
    __finally
    {
        csrMDLRelease(pModel);

        csrShaderRelease(pShader);

        // shutdown OpenGL
        CSR_OpenGLHelper::DisableOpenGL(pOverlayForm->Handle, hDC, hRC);
    }
}
//--------------------------------------------------------------------------------------------------
void TAddItemDialog::GetBitmapFromOpenGL(TBitmap* pBitmap)
{
    // no bitmap?
    if (!pBitmap)
        return;

    GLint dimensions[4];

    // get viewport dimensions
    glGetIntegerv(GL_VIEWPORT, dimensions);

    TRGBQuad* pPixels = NULL;

    try
    {
        // create bits to contain bitmap
        pPixels = new TRGBQuad[dimensions[2] * dimensions[3] * 4];

        // flush OpenGL
        glFinish();
        glPixelStorei(GL_PACK_ALIGNMENT,   4);
        glPixelStorei(GL_PACK_ROW_LENGTH,  0);
        glPixelStorei(GL_PACK_SKIP_ROWS,   0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

        // get pixels from last OpenGL rendering
        glReadPixels(0, 0, dimensions[2], dimensions[3], GL_RGBA, GL_UNSIGNED_BYTE, pPixels);

        // configure destination bitmap
        pBitmap->PixelFormat = pf32bit;
        pBitmap->SetSize(dimensions[2], dimensions[3]);

        // iterate through lines to copy
        for (GLint y = 0; y < dimensions[3]; ++y)
        {
            // get next line to copy and calculate y position (origin is on the left bottom on the
            // source, but on the left top on the destination)
            TRGBQuad*         pLine = static_cast<TRGBQuad*>(pBitmap->ScanLine[y]);
            const std::size_t yPos  = ((dimensions[3] - 1) - y) * dimensions[2];

            // iterate through pixels to copy
            for (GLint x = 0; x < dimensions[2]; ++x)
            {
                // take the opportunity to swap the pixel RGB values
                pLine[x].rgbRed      = pPixels[yPos + x].rgbBlue;
                pLine[x].rgbGreen    = pPixels[yPos + x].rgbGreen;
                pLine[x].rgbBlue     = pPixels[yPos + x].rgbRed;
                pLine[x].rgbReserved = pPixels[yPos + x].rgbReserved;
            }
        }
    }
    __finally
    {
        if (pPixels)
            delete[] pPixels;
    }
}
//--------------------------------------------------------------------------------------------------
void TAddItemDialog::ApplyAntialiasing(TBitmap* pSource, TBitmap* pDest, std::size_t factor)
{
    // no source bitmap?
    if (!pSource)
        return;

    // no destination bitmap?
    if (!pDest)
        return;

    // configure destination bitmap
    pDest->PixelFormat = pSource->PixelFormat;
    pDest->AlphaFormat = pSource->AlphaFormat;
    pDest->SetSize(pSource->Width / factor, pSource->Height / factor);

    // set stretch mode to half tones (thus resizing will be smooth)
    int prevMode = ::SetStretchBltMode(pDest->Canvas->Handle, HALFTONE);

    try
    {
        // apply antialiasing on the destination image
        ::StretchBlt(pDest->Canvas->Handle,
                     0,
                     0,
                     pDest->Width,
                     pDest->Height,
                     pSource->Canvas->Handle,
                     0,
                     0,
                     pSource->Width,
                     pSource->Height,
                     SRCCOPY);
    }
    __finally
    {
        // restore previous stretch blit mode
        ::SetStretchBltMode(pDest->Canvas->Handle, prevMode);
    }
}
//---------------------------------------------------------------------------
float TAddItemDialog::CalculateYPos(const CSR_AABBNode* pTree, bool rotated) const
{
    // no tree or box?
    if (!pTree || !pTree->m_pBox)
        return 0.0f;

    // is model rotated?
    if (rotated)
        // calculate the y position from the z axis
        return (pTree->m_pBox->m_Max.m_Z + pTree->m_pBox->m_Min.m_Z) / 2.0f;

    // calculate the y position from the z axis
    return (pTree->m_pBox->m_Max.m_Y + pTree->m_pBox->m_Min.m_Y) / 2.0f;
}
//---------------------------------------------------------------------------
