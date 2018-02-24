#include <vcl.h>
#pragma hdrstop
#include "Main.h"

#include <Vcl.Graphics.hpp>

#pragma package(smart_init)
#pragma resource "*.dfm"

TMainForm *MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{}
//---------------------------------------------------------------------------
void TMainForm::OnApplyFragmentShader(const CSR_Matrix4*  pMatrix,
                                      const CSR_Polygon3* pPolygon,
                                      const CSR_Vector2*  pST,
                                      const CSR_Vector3*  pSampler,
                                            float         z,
                                            CSR_Color*    pColor)
{
    /*REM
    pColor->m_R = 1.0f;
    pColor->m_G = 1.0f;
    pColor->m_B = 0.0f;
    pColor->m_A = 1.0f;
    */

    /**/
    const int M = 10;

    // FIXME map a real texture here, and use the vertex buffer normal instead

    CSR_Polygon3 cameraPoly;

    // If you need to compute the actual position of the shaded point in camera space.
    // Proceed like with the other vertex attribute. Divide the point coordinates by
    // the vertex z-coordinate then interpolate using barycentric coordinates and
    // finally multiply by sample depth.
    csrMat4Transform(pMatrix, &pPolygon->m_Vertex[0], &cameraPoly.m_Vertex[0]);
    csrMat4Transform(pMatrix, &pPolygon->m_Vertex[1], &cameraPoly.m_Vertex[1]);
    csrMat4Transform(pMatrix, &pPolygon->m_Vertex[2], &cameraPoly.m_Vertex[2]);

    float pixelX;
    float pixelY;

    pixelX = (cameraPoly.m_Vertex[0].m_X / -cameraPoly.m_Vertex[0].m_Z) * pSampler->m_X +
             (cameraPoly.m_Vertex[1].m_X / -cameraPoly.m_Vertex[1].m_Z) * pSampler->m_Y +
             (cameraPoly.m_Vertex[2].m_X / -cameraPoly.m_Vertex[2].m_Z) * pSampler->m_Z;
    pixelY = (cameraPoly.m_Vertex[0].m_Y / -cameraPoly.m_Vertex[0].m_Z) * pSampler->m_X +
             (cameraPoly.m_Vertex[1].m_Y / -cameraPoly.m_Vertex[1].m_Z) * pSampler->m_Y +
             (cameraPoly.m_Vertex[2].m_Y / -cameraPoly.m_Vertex[2].m_Z) * pSampler->m_Z;

    CSR_Vector3 point;

    // calculate the pixel in the camera space
    point.m_X =  pixelX * z;
    point.m_Y =  pixelY * z;
    point.m_Z = -z;

    CSR_Vector3 v1v0Cam;
    CSR_Vector3 v1v0CamXv2v0Cam;
    CSR_Vector3 v2v0Cam;
    CSR_Vector3 normal;
    CSR_Vector3 viewDirection;
    CSR_Vector3 nViewDir;
    float       nDotView;

    // Compute the face normal which is used for a simple facing ratio. Keep in mind
    // that we are doing all calculation in camera space. Thus the view direction can
    // be computed as the point on the object in camera space minus CSR_Vector3(0),
    // the position of the camera in camera space.
    csrVec3Sub(&cameraPoly.m_Vertex[1], &cameraPoly.m_Vertex[0], &v1v0Cam);
    csrVec3Sub(&cameraPoly.m_Vertex[2], &cameraPoly.m_Vertex[0], &v2v0Cam);
    csrVec3Cross(&v1v0Cam, &v2v0Cam, &v1v0CamXv2v0Cam);
    csrVec3Normalize(&v1v0CamXv2v0Cam, &normal);

    viewDirection.m_X = -point.m_X;
    viewDirection.m_Y = -point.m_Y;
    viewDirection.m_Z = -point.m_Z;
    csrVec3Normalize(&viewDirection, &nViewDir);

    csrVec3Dot(&normal, &nViewDir, &nDotView);
    csrMathMax(0.0f, nDotView, &nDotView);

    float checker;
    float c;

    // FIXME map a real texture here
    // The final color is the reuslt of the faction ratio multiplied by the checkerboard pattern.
    checker   = (fmod(pST->m_X * M, 1.0f) > 0.5f) ^ (fmod(pST->m_Y * M, 1.0f) < 0.5f);
    c         = 0.3 * (1 - checker) + 0.7 * checker;
    nDotView *= c;
    /**/

    pColor->m_R = nDotView;
    pColor->m_G = nDotView;
    pColor->m_B = nDotView;
}
//---------------------------------------------------------------------------
#include <stdint.h>
#include <memory>
#include <fstream>

//REM #include <chrono>

#include "cow.h"
void __fastcall TMainForm::bt1Click(TObject *Sender)
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

    // load MDL model
    CSR_MDL* pMDL = csrMDLOpen("N:\\Jeanmilost\\Devel\\Projects\\CompactStar Engine\\Common\\Models\\shadow.mdl",
                               0,
                              &vf,
                               0,
                               0,
                               0);

    pMDL->m_pModel[0].m_pMesh[0].m_pVB->m_Culling.m_Type = CSR_CT_Back;
    pMDL->m_pModel[0].m_pMesh[0].m_pVB->m_Culling.m_Face = CSR_CF_CCW;

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
