/****************************************************************************
 * ==> Software rasterizer main form ---------------------------------------*
 ****************************************************************************
 * Description : This is a tool showing how to load and draw a model using  *
 *               the software rasterizer                                    *
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
#include <Vcl.Graphics.hpp>

// std
#include <stdint.h>
#include <memory>
#include <fstream>

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TMainForm::IStats
//---------------------------------------------------------------------------
TMainForm::IStats::IStats() :
    m_FPS(0)
{}
//---------------------------------------------------------------------------
TMainForm::IStats::~IStats()
{}
//---------------------------------------------------------------------------
void TMainForm::IStats::Clear()
{}
//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_pModel(NULL),
    m_pModelTexture(NULL),
    m_pFrameBuffer(NULL),
    m_pDepthBuffer(NULL),
    m_zNear(1.0f),
    m_zFar(1000.0f),
    m_AngleX(M_PI / 2.0f),
    m_AngleY(0.0f),
    m_pTextureLastTime(0.0),
    m_pModelLastTime(0.0),
    m_pMeshLastTime(0.0),
    m_TextureIndex(0),
    m_ModelIndex(0),
    m_MeshIndex(0),
    m_FrameCount(0),
    m_StartTime(0),
    m_PreviousTime(0),
    m_Initialized(false)
{}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm()
{
    DeleteScene();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject* pSender)
{
    // initialize the scene
    InitScene(paView->ClientWidth, paView->ClientHeight);

    // initialize the timer
    m_StartTime    = ::GetTickCount();
    m_PreviousTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject* pSender)
{
    CreateViewport(paView->ClientWidth, paView->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormMouseWheel(TObject* pSender, TShiftState shift, int wheelDelta,
        TPoint& mousePos, bool& handled)
{
    tbModelDistance->Position = tbModelDistance->Position + ((wheelDelta > 0) ? 1 : -1);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btLoadModelClick(TObject* pSender)
{
    // set the default dir
    odOpen->InitialDir = GetModelsDir();

    // show the open dialog box
    if (!odOpen->Execute())
        return;

    // get the file name
    const std::string fileName = AnsiString(odOpen->FileName).c_str();

    // load the model
    if (!LoadModel(fileName))
        ::MessageDlg(("Failed to load the model.\r\n\r\n" + fileName).c_str(),
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
}
//------------------------------------------------------------------------------
UnicodeString TMainForm::GetModelsDir() const
{
    // build the model dir from the application exe
    UnicodeString modelDir = ::ExtractFilePath(Application->ExeName);
                  modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
                  modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
                  modelDir = ::ExcludeTrailingPathDelimiter(modelDir) + L"\\Common\\Models\\MDL";

    // dir exists?
    if (::DirectoryExists(modelDir))
        return modelDir;

    // build the model dir from the application exe, in his /Debug or /Release path
    modelDir = ::ExtractFilePath(Application->ExeName);
    modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
    modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
    modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
    modelDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(modelDir));
    modelDir = ::ExcludeTrailingPathDelimiter(modelDir) + L"\\Common\\Models\\MDL";

    // set the default dir in open dialog, if exists
    if (::DirectoryExists(modelDir))
        return modelDir;

    return L"";
}
//------------------------------------------------------------------------------
bool TMainForm::LoadModel(const std::string& fileName)
{
    // release the previous model
    if (m_pModel)
        csrMDLRelease(m_pModel);

    // configure the MDL model vertex format
    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 0;

    CSR_VertexCulling vc;
    vc.m_Type = CSR_CT_Back;
    vc.m_Face = CSR_CF_CCW;

    // load MDL model
    m_pModel = csrMDLOpen(fileName.c_str(),
                          0,
                          &vf,
                          &vc,
                          0,
                          0,
                          OnTextureReadCallback);

    if (!m_pModel)
        return false;

    // get the AABB tree
    if (m_pModel->m_ModelCount && m_pModel->m_pModel->m_MeshCount)
    {
        CSR_AABBNode* pTree = csrAABBTreeFromMesh(&m_pModel->m_pModel[0].m_pMesh[0]);

        if (pTree)
            m_PosY = -CalculateYPos(pTree, true);
    }

    // get the animation count
    const int animCount = m_pModel->m_AnimationCount ? m_pModel->m_AnimationCount - 1 : 0;

    // update the interface
    tbModelDistance->Position  = 100;
    tbAnimationNb->Max         = animCount;
    tbAnimationNb->Enabled     = animCount;
    tbAnimationSpeed->Position = 10;
    tbAnimationSpeed->Enabled  = tbAnimationNb->Enabled;

    return true;
}
//------------------------------------------------------------------------------
void TMainForm::CreateViewport(int w, int h)
{
    // release the previous frame buffer
    if (m_pFrameBuffer)
        csrFrameBufferRelease(m_pFrameBuffer);

    // release the previous depth buffer
    if (m_pDepthBuffer)
        csrDepthBufferRelease(m_pDepthBuffer);

    // create the frame and depth buffers
    m_pFrameBuffer = csrFrameBufferCreate(w, h);
    m_pDepthBuffer = csrDepthBufferCreate(w, h);

    // calculate matrix items
    const float fov    = 45.0f;
    const float aspect = float(w) / float(h);

    csrMat4Perspective(fov, aspect, m_zNear, m_zFar, &m_ProjectionMatrix);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // initialize the raster
    csrRasterInit(&m_Raster);

    CreateViewport(w, h);

    if (!LoadModel((AnsiString(GetModelsDir()) + "\\wizard.mdl").c_str()))
    {
        ::MessageDlg("An error occurred while the default model was opened.\n\nApplication will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Close();
        return;
    }

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    m_Initialized = false;

    // release the model texture
    if (m_pModelTexture)
        csrPixelBufferRelease(m_pModelTexture);

    // release the model
    if (m_pModel)
        csrMDLRelease(m_pModel);

    // release the depth buffer
    if (m_pDepthBuffer)
        csrDepthBufferRelease(m_pDepthBuffer);

    // release the frame buffer
    if (m_pFrameBuffer)
        csrFrameBufferRelease(m_pFrameBuffer);
}
//---------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    float       angle;
    CSR_Vector3 t;
    CSR_Vector3 axis;
    CSR_Vector3 factor;
    CSR_Matrix4 translateMatrix;
    CSR_Matrix4 rotateMatrixX;
    CSR_Matrix4 rotateMatrixY;
    CSR_Matrix4 scaleMatrix;
    CSR_Matrix4 combinedMatrixLevel1;
    CSR_Matrix4 combinedMatrixLevel2;
    CSR_Matrix4 modelViewMatrix;
    CSR_Matrix4 worldToCamera;

    // calculate the next angle
    m_AngleY += (elapsedTime * (float(tbRotationSpeed->Position) * 0.01f));

    // avoid the angle to exceeds the bounds even if a huge time has elapsed since last update
    while (m_AngleY > M_PI * 2.0f)
        m_AngleY -= M_PI * 2.0f;

    // set translation
    t.m_X =  0.0f;
    t.m_Y =  m_PosY;
    t.m_Z = -float(tbModelDistance->Position);

    csrMat4Translate(&t, &translateMatrix);

    // set rotation axis
    axis.m_X = 1.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 0.0f;

    // set rotation angle
    angle = m_AngleX;

    csrMat4Rotate(angle, &axis, &rotateMatrixX);

    // set rotation axis
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    // set rotation angle
    angle = m_AngleY;

    csrMat4Rotate(angle, &axis, &rotateMatrixY);

    // set scale factor
    factor.m_X = 0.5f;
    factor.m_Y = 0.5f;
    factor.m_Z = 0.5f;

    csrMat4Scale(&factor, &scaleMatrix);

    // calculate model view matrix
    csrMat4Multiply(&scaleMatrix,          &rotateMatrixX,   &combinedMatrixLevel1);
    csrMat4Multiply(&combinedMatrixLevel1, &rotateMatrixY,   &combinedMatrixLevel2);
    csrMat4Multiply(&combinedMatrixLevel2, &translateMatrix, &modelViewMatrix);

    // build the final matrix
    csrMat4Multiply(&modelViewMatrix, &m_ProjectionMatrix, &m_Matrix);

    // calculate the next indexes to use for the MDL model
    if (!ckPauseModelAnimation->Checked && m_pModel)
        csrMDLUpdateIndex(m_pModel,
                          tbAnimationSpeed->Position,
                          tbAnimationNb->Position,
                         &m_TextureIndex,
                         &m_ModelIndex,
                         &m_MeshIndex,
                         &m_pTextureLastTime,
                         &m_pModelLastTime,
                         &m_pMeshLastTime,
                          elapsedTime);
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    CSR_Pixel pixel;
    pixel.m_R = 0;
    pixel.m_G = 0;
    pixel.m_B = 0;
    pixel.m_A = 255;

    // clear the buffers
    csrFrameBufferClear(m_pFrameBuffer, &pixel);
    csrDepthBufferClear(m_pDepthBuffer, m_zFar);

    // get the current model mesh to draw
    const CSR_Mesh* pMesh = csrMDLGetMesh(m_pModel, m_ModelIndex, m_MeshIndex);

    // found it?
    if (!pMesh)
        return;

    // draw the model
    csrRasterDraw(&m_Matrix,
                   m_zNear,
                   m_zFar,
                   pMesh->m_pVB,
                  &m_Raster,
                   m_pFrameBuffer,
                   m_pDepthBuffer,
                   0,
                   OnApplyFragmentShaderCallback);

    std::auto_ptr<TBitmap> pBitmap(new TBitmap());
    pBitmap->PixelFormat = pf24bit;
    pBitmap->SetSize(paView->ClientWidth, paView->ClientHeight);

    for (std::size_t y = 0; y < paView->ClientHeight; ++y)
    {
        TRGBTriple* pLine = reinterpret_cast<TRGBTriple*>(pBitmap->ScanLine[y]);

        for (std::size_t x = 0; x < paView->ClientWidth; ++x)
        {
            pLine[x].rgbtRed   = m_pFrameBuffer->m_pPixel[y * paView->ClientWidth + x].m_R;
            pLine[x].rgbtGreen = m_pFrameBuffer->m_pPixel[y * paView->ClientWidth + x].m_G;
            pLine[x].rgbtBlue  = m_pFrameBuffer->m_pPixel[y * paView->ClientWidth + x].m_B;
        }
    }

    HDC hViewDC = NULL;

    try
    {
        hViewDC = ::GetDC(paView->Handle);

        if (hViewDC)
            //REM paView->Canvas->Draw(0, 0, pBitmap.get());
            ::BitBlt(hViewDC, 0, 0, pBitmap->Width, pBitmap->Height, pBitmap->Canvas->Handle, 0, 0, SRCCOPY);
    }
    __finally
    {
        if (hViewDC)
            ::ReleaseDC(paView->Handle, hViewDC);
    }
}
//---------------------------------------------------------------------------
void TMainForm::ShowStats() const
{
    unsigned    vertexCount = 0;
    unsigned    stride;
    std::size_t polyCount;

    if (m_pModel)
    {
        // get the current model mesh to draw
        const CSR_Mesh* pMesh = csrMDLGetMesh(m_pModel, m_ModelIndex, m_MeshIndex);

        // found it?
        if (!pMesh)
            return;

        // get the mesh stride
        stride = pMesh->m_pVB ? pMesh->m_pVB->m_Format.m_Stride : 0;

        // count all vertices contained in the mesh
        for (std::size_t i = 0; i < pMesh->m_Count; ++i)
            vertexCount += pMesh->m_pVB[i].m_Count;

        // calculate the polygons count
        polyCount = stride ? ((vertexCount / stride) / 3) : 0;
    }
    else
        return;

    // show the stats
    laPolygonCount->Caption = L"Polygons Count: " + ::IntToStr(int(polyCount));
    laFPS->Caption          = L"FPS:"             + ::IntToStr(int(m_Stats.m_FPS));
}
//---------------------------------------------------------------------------
float TMainForm::CalculateYPos(const CSR_AABBNode* pTree, bool rotated) const
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
//------------------------------------------------------------------------------
void TMainForm::OnTextureReadCallback(      std::size_t      index,
                                      const CSR_PixelBuffer* pPixelBuffer,
                                            int*             pNoGPU)
{
    // redirect the callback to the main form
    static_cast<TMainForm*>(Application->MainForm)->OnTextureRead(index, pPixelBuffer, pNoGPU);
}
//---------------------------------------------------------------------------
void TMainForm::OnApplyFragmentShaderCallback(const CSR_Matrix4*  pMatrix,
                                              const CSR_Polygon3* pPolygon,
                                              const CSR_Vector2*  pST,
                                              const CSR_Vector3*  pSampler,
                                                    float         z,
                                                    CSR_Color*    pColor)
{
    // redirect the callback to the main form
    static_cast<TMainForm*>(Application->MainForm)->OnApplyFragmentShader(pMatrix,
                                                                        pPolygon,
                                                                        pST,
                                                                        pSampler,
                                                                        z,
                                                                        pColor);
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

        return;
    }

    // calculate time interval
    const unsigned __int64 now            = ::GetTickCount();
    const double           elapsedTime    = (now - m_PreviousTime) / 1000.0;
                           m_PreviousTime =  now;

    if (!m_Initialized)
        return;

    // clear the tree stats
    m_Stats.Clear();

    ++m_FrameCount;

    // calculate the FPS
    if (m_FrameCount >= 100)
    {
        const double      smoothing = 0.1;
        const std::size_t fpsTime   = now > m_StartTime ? now - m_StartTime : 1;
        const std::size_t newFPS    = (m_FrameCount * 100) / fpsTime;
        m_StartTime                 = ::GetTickCount();
        m_FrameCount                = 0;
        m_Stats.m_FPS               = (newFPS * smoothing) + (m_Stats.m_FPS * (1.0 - smoothing));
    }

    // update and draw the scene
    UpdateScene(elapsedTime);
    DrawScene();

    // show the stats
    ShowStats();
}
//------------------------------------------------------------------------------
void TMainForm::OnTextureRead(std::size_t index, const CSR_PixelBuffer* pPixelBuffer, int* pNoGPU)
{
    // don't generate the texture on the GPU side (OpenGL isn't used here)
    if (pNoGPU)
        *pNoGPU = 1;

    // no pixel buffer?
    if (!pPixelBuffer)
        return;

    // release the previously existing texture, if any
    if (m_pModelTexture)
        csrPixelBufferRelease(m_pModelTexture);

    // copy the pixel buffer content (the source buffer will be released sooner)
    m_pModelTexture = (CSR_PixelBuffer*)malloc(sizeof(CSR_PixelBuffer));
    memcpy(m_pModelTexture, pPixelBuffer, sizeof(CSR_PixelBuffer));

    // copy the texture pixel data
    m_pModelTexture->m_pData = (unsigned char*)malloc(pPixelBuffer->m_DataLength);
    memcpy(m_pModelTexture->m_pData, pPixelBuffer->m_pData, pPixelBuffer->m_DataLength);
}
//---------------------------------------------------------------------------
void TMainForm::OnApplyFragmentShader(const CSR_Matrix4*  pMatrix,
                                      const CSR_Polygon3* pPolygon,
                                      const CSR_Vector2*  pST,
                                      const CSR_Vector3*  pSampler,
                                            float         z,
                                            CSR_Color*    pColor)
{
    float  stX;
    float  stY;
    size_t x;
    size_t y;
    size_t line;

    // limit the texture coordinate between 0 and 1 (equivalent to OpenGL clamp mode)
    csrMathClamp(pST->m_X, 0.0f, 1.0f, &stX);
    csrMathClamp(pST->m_Y, 0.0f, 1.0f, &stY);

    // calculate the x and y coordinate to pick in the texture, and the line length in pixels
    x    = stX * m_pModelTexture->m_Width;
    y    = stY * m_pModelTexture->m_Height;
    line = m_pModelTexture->m_Width * m_pModelTexture->m_BytePerPixel;

    // calculate the pixel index to get
    const size_t index = (y * line) + (x * m_pModelTexture->m_BytePerPixel);

    // get the pixel color from texture
    pColor->m_R = (float)(((unsigned char*)(m_pModelTexture->m_pData))[index])     / 255.0f;
    pColor->m_G = (float)(((unsigned char*)(m_pModelTexture->m_pData))[index + 1]) / 255.0f;
    pColor->m_B = (float)(((unsigned char*)(m_pModelTexture->m_pData))[index + 2]) / 255.0f;
    pColor->m_A = 1.0f;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;
    OnDrawScene(false);
}
//---------------------------------------------------------------------------
