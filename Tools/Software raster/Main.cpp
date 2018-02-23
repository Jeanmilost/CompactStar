//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "Main.h"

#include <Vcl.Graphics.hpp>

#include "CSR_SoftwareRaster.h"

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

        /*REM
        const CSR_Vector3& v0 = vertices[pIndices[i * 3]];
        const CSR_Vector3& v1 = vertices[pIndices[i * 3 + 1]];
        const CSR_Vector3& v2 = vertices[pIndices[i * 3 + 2]];

        // Prepare vertex attributes. Divde them by their vertex z-coordinate (though we use a multiplication here because v.z = 1 / v.z)
        CSR_Vector2 st0 = st[pUVIndices[i * 3]];
        CSR_Vector2 st1 = st[pUVIndices[i * 3 + 1]];
        CSR_Vector2 st2 = st[pUVIndices[i * 3 + 2]];

        vb.m_pData
        */
    }

    CSR_Raster raster;
    csrRasterInit(&raster);

    csrRasterDraw(&worldToCamera,
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
