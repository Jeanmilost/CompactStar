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
    const uint32_t imageWidth = 640;
    const uint32_t imageHeight = 480;
    Matrix44f worldToCamera( 0.707107f, -0.331295f,  0.624695f,  0.0f,
                             0.0f,       0.883452f,  0.468521f,  0.0f,
                            -0.707107f, -0.331295f,  0.624695f,  0.0f,
                            -1.63871f,  -5.747777f, -40.400412f, 1.0f);

    const uint32_t ntris = 3156;
    const float nearClippingPlane = 1;
    const float farClippingPlane = 1000;

    CSR_Pixel pixel;
    pixel.m_R = 255;
    pixel.m_G = 255;
    pixel.m_B = 255;
    pixel.m_A = 255;

    CSR_FrameBuffer* pFrameBuffer = CSR_Raster::csrFrameBufferCreate(imageWidth, imageHeight);
    CSR_Raster::csrFrameBufferClear(pFrameBuffer, &pixel);

    CSR_Raster::csrRasterRasterize(imageWidth,
                                   imageHeight,
                                   &worldToCamera,
                                   nearClippingPlane,
                                   farClippingPlane,
                                   verticesRAW,
                                   3156,//sizeof(verticesRAW) / sizeof(verticesRAW[0]),
                                   nvertices,
                                   sizeof(nvertices) / sizeof(nvertices[0]),
                                   stRAW,
                                   sizeof(stRAW) / sizeof(stRAW[0]),
                                   stindices,
                                   sizeof(stindices) / sizeof(stindices[0]),
                                   pFrameBuffer);

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

    //REM delete [] frameBuffer;
    //REM delete [] depthBuffer;

    pBitmap->SaveToFile(L"test.bmp");
    //REM im1->Picture->Assign(pBitmap.get());
}
//---------------------------------------------------------------------------
