#pragma hdrstop
#include "CSR_Raster.h"

// std
#include <stdlib>
#include <math.h>

/*REM
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
*/

//REM #include <Vcl.Graphics.hpp>

//---------------------------------------------------------------------------
CSR_FrameBuffer* CSR_Raster::csrFrameBufferCreate(size_t width, size_t height)
{
    // create a frame buffer
    CSR_FrameBuffer* pFB = (CSR_FrameBuffer*)malloc(sizeof(CSR_FrameBuffer));

    // succeeded?
    if (!pFB)
        return 0;

    // initialize the frame buffer content
    if (!csrFrameBufferInit(width, height, pFB))
    {
        csrFrameBufferRelease(pFB);
        return 0;
    }

    return pFB;
}
//---------------------------------------------------------------------------
int CSR_Raster::csrFrameBufferInit(size_t width, size_t height, CSR_FrameBuffer* pFB)
{
    // calculate the buffer size to create
    const std::size_t size = width * height;

    // validate the input
    if (!size || !pFB)
        return 0;

    // create the pixel array
    pFB->m_pPixel = (CSR_Pixel*)malloc(size * sizeof(CSR_Pixel));

    // succeeded?
    if (!pFB->m_pPixel)
    {
        pFB->m_Width  = 0;
        pFB->m_Height = 0;
        pFB->m_Size   = 0;

        return 0;
    }

    // populate the frame buffer
    pFB->m_Width  = width;
    pFB->m_Height = height;
    pFB->m_Size   = size;

    return 1;
}
//---------------------------------------------------------------------------
void CSR_Raster::csrFrameBufferRelease(CSR_FrameBuffer* pFB)
{
    // nothing to release?
    if (!pFB)
        return;

    // release the pixel buffer
    if (pFB->m_pPixel)
        free(pFB->m_pPixel);

    // release the frame buffer
    free(pFB);
}
//---------------------------------------------------------------------------
void CSR_Raster::csrFrameBufferClear(CSR_FrameBuffer* pFB, CSR_Pixel* pPixel)
{
    size_t i;

    // validate the input
    if (!pFB || !pFB->m_pPixel)
        return;

    // fill the buffer with the pixel color value
    for (i = 0; i < pFB->m_Size; ++i)
        memcpy(&pFB->m_pPixel[i], pPixel, sizeof(CSR_Pixel));
}
//---------------------------------------------------------------------------
CSR_DepthBuffer* CSR_Raster::csrDepthBufferCreate(size_t width, size_t height)
{
    // create a depth buffer
    CSR_DepthBuffer* pDB = (CSR_DepthBuffer*)malloc(sizeof(CSR_DepthBuffer));

    // succeeded?
    if (!pDB)
        return 0;

    // initialize the depth buffer content
    if (!csrDepthBufferInit(width, height, pDB))
    {
        csrDepthBufferRelease(pDB);
        return 0;
    }

    return pDB;
}
//---------------------------------------------------------------------------
int CSR_Raster::csrDepthBufferInit(size_t width, size_t height, CSR_DepthBuffer* pDB)
{
    // calculate the buffer size to create
    const std::size_t size = width * height;

    // validate the input
    if (!size || !pDB)
        return 0;

    // create the depth data
    pDB->m_pData = (float*)malloc(size * sizeof(float));

    // succeeded?
    if (!pDB->m_pData)
    {
        pDB->m_Width  = 0;
        pDB->m_Height = 0;
        pDB->m_Size   = 0;

        return 0;
    }

    // populate the frame buffer
    pDB->m_Width  = width;
    pDB->m_Height = height;
    pDB->m_Size   = size;

    return 1;
}
//---------------------------------------------------------------------------
void CSR_Raster::csrDepthBufferRelease(CSR_DepthBuffer* pDB)
{
    // nothing to release?
    if (!pDB)
        return;

    // release the data
    if (pDB->m_pData)
        free(pDB->m_pData);

    // release the depth buffer
    free(pDB);
}
//---------------------------------------------------------------------------
void CSR_Raster::csrDepthBufferClear(CSR_DepthBuffer* pDB, float farClippingPlane)
{
    size_t i;

    // validate the input
    if (!pDB || !pDB->m_pData)
        return;

    // fill the buffer with the far clipping plane value
    for (i = 0; i < pDB->m_Size; ++i)
        memcpy(&pDB->m_pData[i], &farClippingPlane, sizeof(float));
}
//---------------------------------------------------------------------------
float CSR_Raster::csrRasterFindMin(float a, float b, float c)
{
    if (a < b && a < c)
        return a;

    if (b < c)
        return b;

    return c;
}
//---------------------------------------------------------------------------
float CSR_Raster::csrRasterFindMax(float a, float b, float c)
{
    if (a > b && a > c)
        return a;

    if (b > c)
        return b;

    return c;
}
//---------------------------------------------------------------------------
float CSR_Raster::csrRasterFindEdge(const Vec3f& a, const Vec3f& b, const Vec3f& c)
{
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}
//---------------------------------------------------------------------------
void CSR_Raster::csrRasterGetScreenCoordinates(float                  filmApertureWidth,
                                               float                  filmApertureHeight,
                                               size_t                 imageWidth,
                                               size_t                 imageHeight,
                                               CSR_EFitResolutionGate fitFilm,
                                               float                  nearClippingPLane,
                                               float                  focalLength,
                                               float&                 top,
                                               float&                 bottom,
                                               float&                 left,
                                               float&                 right)
{
          float xScale;
          float yScale;
    const float filmAspectRatio   = filmApertureWidth / filmApertureHeight;
    const float deviceAspectRatio = (float)imageWidth / (float)imageHeight;
    const float inchToMm          = 25.4f;

    top   = ((filmApertureHeight * inchToMm / 2.0f) / focalLength) * nearClippingPLane;
    right = ((filmApertureWidth  * inchToMm / 2.0f) / focalLength) * nearClippingPLane;

    // field of view (horizontal)
    //REM float fov = 2 * 180 / M_PI * atan((filmApertureWidth * inchToMm / 2) / focalLength);
    //REM std::cerr << "Field of view " << fov << std::endl;

    xScale = 1;
    yScale = 1;

    switch (fitFilm)
    {
        case CSR_RG_Overscan:
            if (filmAspectRatio > deviceAspectRatio)
                yScale = filmAspectRatio / deviceAspectRatio;
            else
                xScale = deviceAspectRatio / filmAspectRatio;

            break;

        case CSR_RG_Fill:
        default:
            if (filmAspectRatio > deviceAspectRatio)
                xScale = deviceAspectRatio / filmAspectRatio;
            else
                yScale = filmAspectRatio / deviceAspectRatio;

            break;
    }

    right *= xScale;
    top   *= yScale;

    bottom = -top;
    left   = -right;
}
//---------------------------------------------------------------------------
void CSR_Raster::csrRasterConvertTo(const Vec3f&     vertexWorld,
                                    const Matrix44f& worldToCamera,
                                          float      l,
                                          float      r,
                                          float      t,
                                          float      b,
                                          float      zNear,
                                          unsigned   imageWidth,
                                          unsigned   imageHeight,
                                          Vec3f&     vertexRaster)
{
    Vec3f vertexCamera;
    Vec2f vertexScreen;
    Vec2f vertexNDC;

    worldToCamera.multVecMatrix(vertexWorld, vertexCamera);

    // convert to screen space
    vertexScreen.x = zNear * vertexCamera.x / -vertexCamera.z;
    vertexScreen.y = zNear * vertexCamera.y / -vertexCamera.z;

    // now convert point from screen space to NDC space (in range [-1,1])
    vertexNDC.x = 2.0f * vertexScreen.x / (r - l) - (r + l) / (r - l);
    vertexNDC.y = 2.0f * vertexScreen.y / (t - b) - (t + b) / (t - b);

    // convert to raster space
    vertexRaster.x = (vertexNDC.x + 1) / 2 * imageWidth;
    // in raster space y is down so invert direction
    vertexRaster.y = (1 - vertexNDC.y) / 2 * imageHeight;
    vertexRaster.z = -vertexCamera.z;
}
//---------------------------------------------------------------------------
int CSR_Raster::csrRasterRasterize(size_t           imageWidth,
                                   size_t           imageHeight,
                                   const Matrix44f* pWorldToCamera,
                                   float            nearClippingPlane,
                                   float            farClippingPlane,
                                   const float*     pVertices,
                                   size_t           vertCount,
                                   const unsigned*  pIndices,
                                   size_t           indiceCount,
                                   const float*     pUV,
                                   size_t           uvCount,
                                   const unsigned*  pUVIndices,
                                   size_t           uvIndiceCount,
                                   CSR_FrameBuffer* pFB)
{
    CSR_DepthBuffer* pDB;

    // 35mm Full Aperture in inches
    const float filmApertureWidth  = 0.980;
    const float filmApertureHeight = 0.735;
    const float focalLength        = 20.0f; // in mm

    const Matrix44f cameraToWorld = pWorldToCamera->inverse();

    // compute screen coordinates
    float t, b, l, r;

    csrRasterGetScreenCoordinates(filmApertureWidth,
                                  filmApertureHeight,
                                  imageWidth,
                                  imageHeight,
                                  CSR_RG_Overscan,
                                  nearClippingPlane,
                                  focalLength,
                                  t, b, l, r);

    pDB = csrDepthBufferCreate(imageWidth, imageHeight);

    if (!pDB)
        return 0;

        csrDepthBufferClear(pDB, farClippingPlane);

    // define the frame-buffer and the depth-buffer. Initialize depth buffer
    // to far clipping plane.
    //REM Vec3<unsigned char> *frameBuffer = new Vec3<unsigned char>[imageWidth * imageHeight];
    //REM for (uint32_t i = 0; i < imageWidth * imageHeight; ++i) frameBuffer[i] = Vec3<unsigned char>(255);
    //REM float *depthBuffer = new float[imageWidth * imageHeight];
    //REM for (uint32_t i = 0; i < imageWidth * imageHeight; ++i) depthBuffer[i] = farClippingPLane;

    //REM auto t_start = std::chrono::high_resolution_clock::now();

    Vec3f vertices[1732];
    Vec2f st[3056];

    for (std::size_t i = 0; i < 1732; ++i)
        vertices[i] = Vec3f(pVertices[i * 3], pVertices[(i * 3) + 1], pVertices[(i * 3) + 2]);

    for (std::size_t i = 0; i < 3056; ++i)
        st[i] = Vec2f(pUV[i * 2], pUV[(i * 2) + 1]);

    // Outer loop
    for (uint32_t i = 0; i < vertCount; ++i)
    {
        const Vec3f &v0 = vertices[pIndices[i * 3]];
        const Vec3f &v1 = vertices[pIndices[i * 3 + 1]];
        const Vec3f &v2 = vertices[pIndices[i * 3 + 2]];

        // Convert the vertices of the triangle to raster space
        Vec3f v0Raster, v1Raster, v2Raster;
        csrRasterConvertTo(v0, *pWorldToCamera, l, r, t, b, nearClippingPlane, imageWidth, imageHeight, v0Raster);
        csrRasterConvertTo(v1, *pWorldToCamera, l, r, t, b, nearClippingPlane, imageWidth, imageHeight, v1Raster);
        csrRasterConvertTo(v2, *pWorldToCamera, l, r, t, b, nearClippingPlane, imageWidth, imageHeight, v2Raster);

        //Precompute reciprocal of vertex z-coordinate
        v0Raster.z = 1 / v0Raster.z,
        v1Raster.z = 1 / v1Raster.z,
        v2Raster.z = 1 / v2Raster.z;

        // Prepare vertex attributes. Divde them by their vertex z-coordinate (though we use a multiplication here because v.z = 1 / v.z)
        Vec2f st0 = st[pUVIndices[i * 3]];
        Vec2f st1 = st[pUVIndices[i * 3 + 1]];
        Vec2f st2 = st[pUVIndices[i * 3 + 2]];

        st0 *= v0Raster.z, st1 *= v1Raster.z, st2 *= v2Raster.z;

        float xmin = csrRasterFindMin(v0Raster.x, v1Raster.x, v2Raster.x);
        float ymin = csrRasterFindMin(v0Raster.y, v1Raster.y, v2Raster.y);
        float xmax = csrRasterFindMax(v0Raster.x, v1Raster.x, v2Raster.x);
        float ymax = csrRasterFindMax(v0Raster.y, v1Raster.y, v2Raster.y);

        // the triangle is out of screen
        if (xmin > imageWidth - 1 || xmax < 0 || ymin > imageHeight - 1 || ymax < 0)
            continue;

        // be careful xmin/xmax/ymin/ymax can be negative. Don't cast to uint32_t
        uint32_t x0 = std::max(int32_t(0), (int32_t)(std::floor(xmin)));
        uint32_t x1 = std::min(int32_t(imageWidth) - 1, (int32_t)(std::floor(xmax)));
        uint32_t y0 = std::max(int32_t(0), (int32_t)(std::floor(ymin)));
        uint32_t y1 = std::min(int32_t(imageHeight) - 1, (int32_t)(std::floor(ymax)));

        float area = csrRasterFindEdge(v0Raster, v1Raster, v2Raster);

        // Inner loop
        for (uint32_t y = y0; y <= y1; ++y) {
            for (uint32_t x = x0; x <= x1; ++x)
            {
                Vec3f pixelSample(x + 0.5, y + 0.5, 0);
                float w0 = csrRasterFindEdge(v1Raster, v2Raster, pixelSample);
                float w1 = csrRasterFindEdge(v2Raster, v0Raster, pixelSample);
                float w2 = csrRasterFindEdge(v0Raster, v1Raster, pixelSample);
                if (w0 >= 0 && w1 >= 0 && w2 >= 0)
                {
                    w0 /= area;
                    w1 /= area;
                    w2 /= area;
                    float oneOverZ = v0Raster.z * w0 + v1Raster.z * w1 + v2Raster.z * w2;
                    float z = 1 / oneOverZ;

                    //Depth-buffer test
                    if (z < pDB->m_pData[y * imageWidth + x])
                    {
                        pDB->m_pData[y * imageWidth + x] = z;

                        Vec2f st = st0 * w0 + st1 * w1 + st2 * w2;

                        st *= z;

                        //If you need to compute the actual position of the shaded point in camera space. Proceed like with the other vertex attribute. Divide the point coordinates by the vertex z-coordinate then interpolate using barycentric coordinates and finally multiply by sample depth.
                        Vec3f v0Cam, v1Cam, v2Cam;
                        pWorldToCamera->multVecMatrix(v0, v0Cam);
                        pWorldToCamera->multVecMatrix(v1, v1Cam);
                        pWorldToCamera->multVecMatrix(v2, v2Cam);

                        float px = (v0Cam.x/-v0Cam.z) * w0 + (v1Cam.x/-v1Cam.z) * w1 + (v2Cam.x/-v2Cam.z) * w2;
                        float py = (v0Cam.y/-v0Cam.z) * w0 + (v1Cam.y/-v1Cam.z) * w1 + (v2Cam.y/-v2Cam.z) * w2;

                        Vec3f pt(px * z, py * z, -z); // pt is in camera space

                        //Compute the face normal which is used for a simple facing ratio. Keep in mind that we are doing all calculation in camera space. Thus the view direction can be computed as the point on the object in camera space minus Vec3f(0), the position of the camera in camera space.
                        Vec3f n = (v1Cam - v0Cam).crossProduct(v2Cam - v0Cam);
                        n.normalize();
                        Vec3f viewDirection = -pt;
                        viewDirection.normalize();

                        float nDotView =  std::max(0.f, n.dotProduct(viewDirection));

                        // The final color is the reuslt of the faction ration multiplied by the checkerboard pattern.
                        const int M = 10;
                        float checker = (fmod(st.x * M, 1.0f) > 0.5f) ^ (fmod(st.y * M, 1.0f) < 0.5f);
                        float c = 0.3 * (1 - checker) + 0.7 * checker;
                        nDotView *= c;
                        pFB->m_pPixel[y * imageWidth + x].m_R = nDotView * 255;
                        pFB->m_pPixel[y * imageWidth + x].m_G = nDotView * 255;
                        pFB->m_pPixel[y * imageWidth + x].m_B = nDotView * 255;
                    }
                }
            }
        }
    }

    /*REM
    auto t_end = std::chrono::high_resolution_clock::now();
    auto passedTime = std::chrono::duration<double, std::milli>(t_end - t_start).count();
    std::cerr << "Wall passed time:  " << passedTime << " ms" << std::endl;

    // Store the result of the framebuffer to a PPM file (Photoshop reads PPM files).
    std::ofstream ofs;
    ofs.open("./output.ppm");
    ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
    ofs.write((char*)frameBuffer, imageWidth * imageWidth * 3);
    ofs.close();
    */

    return 0;
}
