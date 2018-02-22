//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Main.h"

#include <Vcl.Graphics.hpp>

#include "CSR_Raster.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
#include <stdint.h>
#include <memory>
#include <fstream>

//REM #include <chrono>

#include "cow.h"
void __fastcall TForm1::bt1Click(TObject *Sender)
{
    const uint32_t imageWidth  = 640;
    const uint32_t imageHeight = 480;
    const float    zNear       = 1.0f;
    const float    zFar        = 1000.0f;

    CSR_Matrix4 worldToCamera;
    worldToCamera.m_Table[0][0] =  0.707107f; worldToCamera.m_Table[0][1] = -0.331295f; worldToCamera.m_Table[0][2] =  0.624695f;  worldToCamera.m_Table[0][3] = 0.0f;
    worldToCamera.m_Table[1][0] =  0.0f;      worldToCamera.m_Table[1][1] =  0.883452f; worldToCamera.m_Table[1][2] =  0.468521f;  worldToCamera.m_Table[1][3] = 0.0f;
    worldToCamera.m_Table[2][0] = -0.707107f; worldToCamera.m_Table[2][1] = -0.331295f; worldToCamera.m_Table[2][2] =  0.624695f;  worldToCamera.m_Table[2][3] = 0.0f;
    worldToCamera.m_Table[3][0] = -1.63871f;  worldToCamera.m_Table[3][1] = -5.747777f; worldToCamera.m_Table[3][2] = -40.400412f; worldToCamera.m_Table[3][3] = 1.0f;

    const uint32_t ntris = 3156;

    CSR_Pixel pixel;
    pixel.m_R = 255;
    pixel.m_G = 255;
    pixel.m_B = 255;
    pixel.m_A = 255;

    // create the frame buffer
    CSR_FrameBuffer* pFrameBuffer = CSR_SoftwareRaster::csrFrameBufferCreate(imageWidth, imageHeight);
    CSR_SoftwareRaster::csrFrameBufferClear(pFrameBuffer, &pixel);

    // create the depth buffer
    CSR_DepthBuffer* pDepthBuffer = CSR_SoftwareRaster::csrDepthBufferCreate(imageWidth, imageHeight);
    CSR_SoftwareRaster::csrDepthBufferClear(pDepthBuffer, zFar);

    CSR_VertexBuffer vb;
    /*REM
    verticesRAW,
    3156,//sizeof(verticesRAW) / sizeof(verticesRAW[0]),
    nvertices,
    sizeof(nvertices) / sizeof(nvertices[0]),
    stRAW,
    sizeof(stRAW) / sizeof(stRAW[0]),
    stindices,
    sizeof(stindices) / sizeof(stindices[0]),
    */
    /*REM
    CSR_Vector3 vertices[1732];
    CSR_Vector2 st[3056];

    for (std::size_t i = 0; i < 1732; ++i)
        vertices[i] = CSR_Vector3(pVertices[i * 3], pVertices[(i * 3) + 1], pVertices[(i * 3) + 2]);

    for (std::size_t i = 0; i < 3056; ++i)
        st[i] = CSR_Vector2(pUV[i * 2], pUV[(i * 2) + 1]);
    */
    /*REM
    const CSR_Vector3& v0 = vertices[pIndices[i * 3]];
    const CSR_Vector3& v1 = vertices[pIndices[i * 3 + 1]];
    const CSR_Vector3& v2 = vertices[pIndices[i * 3 + 2]];
    */
    /*REM
    // Prepare vertex attributes. Divde them by their vertex z-coordinate (though we use a multiplication here because v.z = 1 / v.z)
    CSR_Vector2 st0 = st[pUVIndices[i * 3]];
    CSR_Vector2 st1 = st[pUVIndices[i * 3 + 1]];
    CSR_Vector2 st2 = st[pUVIndices[i * 3 + 2]];
    */

    CSR_Raster raster;
    CSR_SoftwareRaster::csrRasterInit(&raster);

    CSR_SoftwareRaster::csrRasterDraw(&worldToCamera,
                                       zNear,
                                       zFar,
                                      &vb,
                                      &raster,
                                       pFrameBuffer,
                                       pDepthBuffer);

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

    // release the depth buffer
    CSR_SoftwareRaster::csrDepthBufferRelease(pDepthBuffer);

    // release the frame buffer
    CSR_SoftwareRaster::csrFrameBufferRelease(pFrameBuffer);

    //REM delete [] frameBuffer;
    //REM delete [] depthBuffer;

    pBitmap->SaveToFile(L"test.bmp");
    //REM im1->Picture->Assign(pBitmap.get());
}
//---------------------------------------------------------------------------
