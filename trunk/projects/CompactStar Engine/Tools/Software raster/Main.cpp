#include <vcl.h>
#pragma hdrstop
#include "Main.h"

#include <Vcl.Graphics.hpp>

#include <stdint.h>
#include <memory>
#include <fstream>

//REM #include <chrono>

#include "cow.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

CSR_PixelBuffer* g_pModelTexture   = 0;

//---------------------------------------------------------------------------
TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{}
//------------------------------------------------------------------------------
void OnTextureRead(std::size_t index, const CSR_PixelBuffer* pPixelBuffer)
{
    // no pixel buffer?
    if (!pPixelBuffer)
        return;

    // release the previously existing texture, if any
    if (g_pModelTexture)
        csrPixelBufferRelease(g_pModelTexture);

    // copy the pixel buffer content (the source buffer will be released sooner)
    g_pModelTexture = (CSR_PixelBuffer*)malloc(sizeof(CSR_PixelBuffer));
    memcpy(g_pModelTexture, pPixelBuffer, sizeof(CSR_PixelBuffer));

    // copy the texture pixel data
    g_pModelTexture->m_pData = (unsigned char*)malloc(pPixelBuffer->m_DataLength);
    memcpy(g_pModelTexture->m_pData, pPixelBuffer->m_pData, pPixelBuffer->m_DataLength);
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
    x    = stX * g_pModelTexture->m_Width;
    y    = stY * g_pModelTexture->m_Height;
    line = g_pModelTexture->m_Width * g_pModelTexture->m_BytePerPixel;

    // calculate the pixel index to get
    const size_t index = (y * line) + (x * g_pModelTexture->m_BytePerPixel);

    // get the pixel color from texture
    pColor->m_R = (float)(((unsigned char*)(g_pModelTexture->m_pData))[index])     / 255.0f;
    pColor->m_G = (float)(((unsigned char*)(g_pModelTexture->m_pData))[index + 1]) / 255.0f;
    pColor->m_B = (float)(((unsigned char*)(g_pModelTexture->m_pData))[index + 2]) / 255.0f;
    pColor->m_A = 1.0f;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::bt1Click(TObject *Sender)
{
    const uint32_t imageWidth  = 640;
    const uint32_t imageHeight = 480;
    const float    zNear       = 1.0f;
    const float    zFar        = 1000.0f;

    CSR_Matrix4 projectionMatrix;
    float w = imageWidth;
    float h = imageHeight;

    // calculate matrix items
    const float fov    = 45.0f;
    const float aspect = w / h;

    csrMat4Perspective(fov, aspect, zNear, zFar, &projectionMatrix);

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
    /*
    worldToCamera.m_Table[0][0] =  0.707107f; worldToCamera.m_Table[0][1] = -0.331295f; worldToCamera.m_Table[0][2] =  0.624695f;  worldToCamera.m_Table[0][3] = 0.0f;
    worldToCamera.m_Table[1][0] =  0.0f;      worldToCamera.m_Table[1][1] =  0.883452f; worldToCamera.m_Table[1][2] =  0.468521f;  worldToCamera.m_Table[1][3] = 0.0f;
    worldToCamera.m_Table[2][0] = -0.707107f; worldToCamera.m_Table[2][1] = -0.331295f; worldToCamera.m_Table[2][2] =  0.624695f;  worldToCamera.m_Table[2][3] = 0.0f;
    worldToCamera.m_Table[3][0] = -1.63871f;  worldToCamera.m_Table[3][1] = -5.747777f; worldToCamera.m_Table[3][2] = -40.400412f; worldToCamera.m_Table[3][3] = 1.0f;
    */
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
    angle = M_PI * 0.25;

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
    csrMat4Multiply(&modelViewMatrix, &projectionMatrix, &worldToCamera);

    const uint32_t ntris = 3156;

    CSR_Pixel pixel;
    pixel.m_R = 0;
    pixel.m_G = 0;
    pixel.m_B = 0;
    pixel.m_A = 255;

    // create the frame buffer
    CSR_FrameBuffer* pFrameBuffer = csrFrameBufferCreate(imageWidth, imageHeight);
    csrFrameBufferClear(pFrameBuffer, &pixel);

    // create the depth buffer
    CSR_DepthBuffer* pDepthBuffer = csrDepthBufferCreate(imageWidth, imageHeight);
    csrDepthBufferClear(pDepthBuffer, zFar);

    CSR_VertexBuffer vb;

    CSR_Vector3 vertices[1732];
    CSR_Vector2 st[3056];

    for (std::size_t i = 0; i < 1732; ++i)
    {
        vertices[i].m_X = verticesRAW[i * 3];
        vertices[i].m_Y = verticesRAW[(i * 3) + 1];
        vertices[i].m_Z = verticesRAW[(i * 3) + 2];
    }

    for (std::size_t i = 0; i < 3056; ++i)
    {
        st[i].m_X = stRAW[i * 2];
        st[i].m_Y = stRAW[(i * 2) + 1];
    }

    vb.m_Format.m_Type              = CSR_VT_Triangles;
    vb.m_Format.m_HasNormal         = 0;
    vb.m_Format.m_HasTexCoords      = 1;
    vb.m_Format.m_HasPerVertexColor = 0;
    vb.m_Format.m_Stride            = 5;
    vb.m_Culling.m_Type             = CSR_CT_Back;
    vb.m_Culling.m_Face             = CSR_CF_CW;
    vb.m_Count                      = ntris * 3 * vb.m_Format.m_Stride;
    vb.m_pData                      = (float*)malloc(vb.m_Count * sizeof(float));

    for (std::size_t i = 0; i < ntris; ++i)
    {
        vb.m_pData[ i * 3 * 5]      = vertices[nvertices[i * 3]].m_X;
        vb.m_pData[(i * 3 * 5) + 1] = vertices[nvertices[i * 3]].m_Y;
        vb.m_pData[(i * 3 * 5) + 2] = vertices[nvertices[i * 3]].m_Z;
        vb.m_pData[(i * 3 * 5) + 3] = st[stindices[i * 3]].m_X;
        vb.m_pData[(i * 3 * 5) + 4] = st[stindices[i * 3]].m_Y;

        vb.m_pData[(i * 3 * 5) + 5] = vertices[nvertices[i * 3 + 1]].m_X;
        vb.m_pData[(i * 3 * 5) + 6] = vertices[nvertices[i * 3 + 1]].m_Y;
        vb.m_pData[(i * 3 * 5) + 7] = vertices[nvertices[i * 3 + 1]].m_Z;
        vb.m_pData[(i * 3 * 5) + 8] = st[stindices[i * 3 + 1]].m_X;
        vb.m_pData[(i * 3 * 5) + 9] = st[stindices[i * 3 + 1]].m_Y;

        vb.m_pData[(i * 3 * 5) + 10] = vertices[nvertices[i * 3 + 2]].m_X;
        vb.m_pData[(i * 3 * 5) + 11] = vertices[nvertices[i * 3 + 2]].m_Y;
        vb.m_pData[(i * 3 * 5) + 12] = vertices[nvertices[i * 3 + 2]].m_Z;
        vb.m_pData[(i * 3 * 5) + 13] = st[stindices[i * 3 + 2]].m_X;
        vb.m_pData[(i * 3 * 5) + 14] = st[stindices[i * 3 + 2]].m_Y;
    }

    // configure the MDL model vertex format
    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 0;

    CSR_VertexCulling vc;
    vc.m_Type = CSR_CT_Back;
    vc.m_Face = CSR_CF_CW;

    // load MDL model
    CSR_MDL* pMDL = csrMDLOpen("N:\\Jeanmilost\\Devel\\Projects\\CompactStar Engine\\Common\\Models\\wizard.mdl",
                               0,
                              &vf,
                              &vc,
                               0,
                               0,
                               OnTextureRead);

    CSR_Raster raster;
    csrRasterInit(&raster);

    csrRasterDraw(&worldToCamera,
                   zNear,
                   zFar,
                   pMDL->m_pModel[0].m_pMesh[0].m_pVB,// &vb,
                  &raster,
                   pFrameBuffer,
                   pDepthBuffer,
                   0,
                   OnApplyFragmentShader);

    std::auto_ptr<TBitmap> pBitmap(new TBitmap());
    pBitmap->PixelFormat = pf24bit;
    pBitmap->SetSize(imageWidth, imageHeight);

    for (std::size_t y = 0; y < imageHeight; ++y)
    {
        TRGBTriple* pLine = reinterpret_cast<TRGBTriple*>(pBitmap->ScanLine[y]);

        for (std::size_t x = 0; x < imageWidth; ++x)
        {
            pLine[x].rgbtRed   = pFrameBuffer->m_pPixel[y * imageWidth + x].m_R;
            pLine[x].rgbtGreen = pFrameBuffer->m_pPixel[y * imageWidth + x].m_G;
            pLine[x].rgbtBlue  = pFrameBuffer->m_pPixel[y * imageWidth + x].m_B;
        }
    }

    if (pMDL)
        csrMDLRelease(pMDL);

    free(vb.m_pData);

    // release the depth buffer
    csrDepthBufferRelease(pDepthBuffer);

    // release the frame buffer
    csrFrameBufferRelease(pFrameBuffer);

    //REM delete [] frameBuffer;
    //REM delete [] depthBuffer;

    //pBitmap->SaveToFile(L"test.bmp");
    //REM im1->Picture->Assign(pBitmap.get());
    Canvas->Draw(0, 0, pBitmap.get());
}
//---------------------------------------------------------------------------
