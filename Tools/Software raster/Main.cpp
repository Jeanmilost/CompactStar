#include <vcl.h>
#pragma hdrstop
#include "Main.h"

#include <Vcl.Graphics.hpp>

#include <stdint.h>
#include <memory>
#include <fstream>

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_pModel(NULL),
    m_pModelTexture(NULL),
    m_pFrameBuffer(NULL),
    m_pDepthBuffer(NULL),
    m_zNear(1.0f),
    m_zFar(1000.0f),
    m_pTextureLastTime(0.0),
    m_pModelLastTime(0.0),
    m_pMeshLastTime(0.0),
    m_TextureIndex(0),
    m_ModelIndex(0),
    m_MeshIndex(0),
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
void __fastcall TMainForm::FormShow(TObject *Sender)
{
    // initialize the scene
    InitScene(ClientWidth, ClientHeight);

    // initialize the timer
    m_StartTime    = ::GetTickCount();
    m_PreviousTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//------------------------------------------------------------------------------
void TMainForm::TextureRead(std::size_t index, const CSR_PixelBuffer* pPixelBuffer)
{
    static_cast<TMainForm*>(Application->MainForm)->OnTextureRead(index, pPixelBuffer);
}
//---------------------------------------------------------------------------
void TMainForm::ApplyFragmentShader(const CSR_Matrix4*  pMatrix,
                                    const CSR_Polygon3* pPolygon,
                                    const CSR_Vector2*  pST,
                                    const CSR_Vector3*  pSampler,
                                          float         z,
                                          CSR_Color*    pColor)
{
    static_cast<TMainForm*>(Application->MainForm)->OnApplyFragmentShader(pMatrix,
                                                                        pPolygon,
                                                                        pST,
                                                                        pSampler,
                                                                        z,
                                                                        pColor);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // initialize the raster
    csrRasterInit(&m_Raster);

    // calculate matrix items
    const float fov    = 45.0f;
    const float aspect = float(w) / float(h);

    csrMat4Perspective(fov, aspect, m_zNear, m_zFar, &m_ProjectionMatrix);

    // create the frame and depth buffers
    m_pFrameBuffer = csrFrameBufferCreate(w, h);
    m_pDepthBuffer = csrDepthBufferCreate(w, h);

    // configure the MDL model vertex format
    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 0;

    CSR_VertexCulling vc;
    vc.m_Type = CSR_CT_Back;
    vc.m_Face = CSR_CF_CCW;

    // load MDL model
    m_pModel = csrMDLOpen("N:\\Jeanmilost\\Devel\\Projects\\CompactStar Engine\\Common\\Models\\wizard.mdl",
                          0,
                          &vf,
                          &vc,
                          0,
                          0,
                          TextureRead);

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

    // set translation
    t.m_X =  0.0f;
    t.m_Y =  0.0f;
    t.m_Z = -150.0f;

    csrMat4Translate(&t, &translateMatrix);

    // set rotation axis
    axis.m_X = 1.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 0.0f;

    // set rotation angle
    angle = M_PI * 0.5;

    csrMat4Rotate(angle, &axis, &rotateMatrixX);

    // set rotation axis
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    // set rotation angle
    angle = -M_PI * 0.25;

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
    csrMDLUpdateIndex(m_pModel,
                      10,
                      0,
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
                   ApplyFragmentShader);

    std::auto_ptr<TBitmap> pBitmap(new TBitmap());
    pBitmap->PixelFormat = pf24bit;
    pBitmap->SetSize(ClientWidth, ClientHeight);

    for (std::size_t y = 0; y < ClientHeight; ++y)
    {
        TRGBTriple* pLine = reinterpret_cast<TRGBTriple*>(pBitmap->ScanLine[y]);

        for (std::size_t x = 0; x < ClientWidth; ++x)
        {
            pLine[x].rgbtRed   = m_pFrameBuffer->m_pPixel[y * ClientWidth + x].m_R;
            pLine[x].rgbtGreen = m_pFrameBuffer->m_pPixel[y * ClientWidth + x].m_G;
            pLine[x].rgbtBlue  = m_pFrameBuffer->m_pPixel[y * ClientWidth + x].m_B;
        }
    }

    Canvas->Draw(0, 0, pBitmap.get());
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

    // update and draw the scene
    UpdateScene(elapsedTime);
    DrawScene();
}
//------------------------------------------------------------------------------
void TMainForm::OnTextureRead(std::size_t index, const CSR_PixelBuffer* pPixelBuffer)
{
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
