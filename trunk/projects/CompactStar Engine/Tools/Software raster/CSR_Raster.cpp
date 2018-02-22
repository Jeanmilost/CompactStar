#pragma hdrstop
#include "CSR_Raster.h"

// std
#include <stdlib>
#include <math.h>

//---------------------------------------------------------------------------
CSR_FrameBuffer* CSR_SoftwareRaster::csrFrameBufferCreate(size_t width, size_t height)
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
int CSR_SoftwareRaster::csrFrameBufferInit(size_t width, size_t height, CSR_FrameBuffer* pFB)
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
void CSR_SoftwareRaster::csrFrameBufferRelease(CSR_FrameBuffer* pFB)
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
void CSR_SoftwareRaster::csrFrameBufferClear(CSR_FrameBuffer* pFB, CSR_Pixel* pPixel)
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
CSR_DepthBuffer* CSR_SoftwareRaster::csrDepthBufferCreate(size_t width, size_t height)
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
int CSR_SoftwareRaster::csrDepthBufferInit(size_t width, size_t height, CSR_DepthBuffer* pDB)
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
void CSR_SoftwareRaster::csrDepthBufferRelease(CSR_DepthBuffer* pDB)
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
void CSR_SoftwareRaster::csrDepthBufferClear(CSR_DepthBuffer* pDB, float farClippingPlane)
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
void csrRasterInit(CSR_Raster* pRaster)
{
    // no raster to initialize?
    if (!pRaster)
        return;

    pRaster->m_ApertureWidth  = 0.980; // 35mm full aperture in inches
    pRaster->m_ApertureHeight = 0.735; // 35mm full aperture in inches
    pRaster->m_FocalLength    = 20.0f; // focal length in mm
    pRaster->m_Type           = CSR_RT_Overscan;
}
//---------------------------------------------------------------------------
float CSR_SoftwareRaster::csrRasterFindMin(float a, float b, float c)
{
    if (a < b && a < c)
        return a;

    if (b < c)
        return b;

    return c;
}
//---------------------------------------------------------------------------
float CSR_SoftwareRaster::csrRasterFindMax(float a, float b, float c)
{
    if (a > b && a > c)
        return a;

    if (b > c)
        return b;

    return c;
}
//---------------------------------------------------------------------------
float CSR_SoftwareRaster::csrRasterFindEdge(const CSR_Vector3* pV1,
                                    const CSR_Vector3* pV2,
                                    const CSR_Vector3* pV3)
{
    return ((pV3->m_X - pV1->m_X) * (pV2->m_Y - pV1->m_Y)) -
           ((pV3->m_Y - pV1->m_Y) * (pV2->m_X - pV1->m_X));
}
//---------------------------------------------------------------------------
void csrRasterGetScreenCoordinates(const CSR_Raster* pRaster,
                                         float       imageWidth,
                                         float       imageHeight,
                                         float       zNear,
                                         CSR_Rect*   pScreenRect)
{
          float xScale;
          float yScale;
    const float filmAspectRatio   = pRaster->m_ApertureWidth / pRaster->m_ApertureHeight;
    const float deviceAspectRatio = imageWidth               / imageHeight;
    const float inchToMm          = 25.4f;

    // validate the input
    if (!pRaster || !pScreenRect)
        return;

    // calculate the right and top edges
    pScreenRect->m_Min.m_Y = ((pRaster->m_ApertureHeight * inchToMm / 2.0f) / pRaster->m_FocalLength) * zNear;
    pScreenRect->m_Max.m_X = ((pRaster->m_ApertureWidth  * inchToMm / 2.0f) / pRaster->m_FocalLength) * zNear;

    xScale = 1;
    yScale = 1;

    // calculate the aspect ratio to apply
    switch (pRaster->m_Type)
    {
        case CSR_RT_Overscan:
            if (filmAspectRatio > deviceAspectRatio)
                yScale = filmAspectRatio / deviceAspectRatio;
            else
                xScale = deviceAspectRatio / filmAspectRatio;

            break;

        case CSR_RT_Fill:
        default:
            if (filmAspectRatio > deviceAspectRatio)
                xScale = deviceAspectRatio / filmAspectRatio;
            else
                yScale = filmAspectRatio / deviceAspectRatio;

            break;
    }

    // apply the ratio to the right and top edges
    pScreenRect->m_Max.m_X *= xScale;
    pScreenRect->m_Min.m_Y *= yScale;

    // calculate the left and bottom edges
    pScreenRect->m_Max.m_Y = -pScreenRect->m_Min.m_Y;
    pScreenRect->m_Min.m_X = -pScreenRect->m_Max.m_X;
}
//---------------------------------------------------------------------------
void CSR_SoftwareRaster::csrRasterRasterizeVertex(const CSR_Vector3* pInVertex,
                                                  const CSR_Matrix4* pMatrix,
                                                  const CSR_Rect*    pScreenRect,
                                                        float        zNear,
                                                        float        imageWidth,
                                                        float        imageHeight,
                                                        CSR_Vector3* pOutVertex)
{
    CSR_Vector3 vertexCamera;
    CSR_Vector2 vertexScreen;
    CSR_Vector2 vertexNDC;
    float       subRightLeft;
    float       addRightLeft;
    float       subTopBottom;
    float       addTopBottom;

    // validate the input
    if (!pInVertex || !pMatrix || !pScreenRect || !pOutVertex)
        return;

    // transform the input vertex into the camera space
    csrMat4Transform(pMatrix, pInVertex, &vertexCamera);

    // transfrom the camera vertex to a point in the screen space
    vertexScreen.m_X = (zNear * vertexCamera.m_X) / -vertexCamera.m_Z;
    vertexScreen.m_Y = (zNear * vertexCamera.m_Y) / -vertexCamera.m_Z;

    subRightLeft = pScreenRect->m_Max.m_X - pScreenRect->m_Min.m_X;
    addRightLeft = pScreenRect->m_Max.m_X + pScreenRect->m_Min.m_X;
    subTopBottom = pScreenRect->m_Min.m_Y - pScreenRect->m_Max.m_X;
    addTopBottom = pScreenRect->m_Min.m_Y + pScreenRect->m_Max.m_X;

    // convert point from screen space to NDC space (in range [-1, 1])
    vertexNDC.m_X = ((2.0f * vertexScreen.m_X) / subRightLeft) - (addRightLeft / subRightLeft);
    vertexNDC.m_Y = ((2.0f * vertexScreen.m_Y) / subTopBottom) - (addTopBottom / subTopBottom);

    // convert to raster space. NOTE in raster space y is down, so the direction is inverted
    pOutVertex->m_X = (vertexNDC.m_X + 1.0f) / 2.0f * imageWidth;
    pOutVertex->m_Y = (1.0f - vertexNDC.m_Y) / 2.0f * imageHeight;
    pOutVertex->m_Z = -vertexCamera.m_Z;
}
//---------------------------------------------------------------------------
int CSR_SoftwareRaster::csrRasterGetPolygon(      size_t            v1Index,
                                                  size_t            v2Index,
                                                  size_t            v3Index,
                                            const CSR_VertexBuffer* pVB,
                                                  CSR_Polygon3*     pPolygon,
                                                  CSR_Vector3*      pNormal,
                                                  CSR_Vector2*      pST,
                                                  unsigned*         pColor)
{
    size_t offset;

    // validate the input
    if (!pVB || !pPolygon || !pNormal || !pST || !pColor)
        return 0;

    // extract the polygon from source vertex buffer
    pPolygon->m_Vertex[0].m_X = pVB->m_pData[v1Index];
    pPolygon->m_Vertex[0].m_Y = pVB->m_pData[v1Index + 1];
    pPolygon->m_Vertex[0].m_Z = pVB->m_pData[v1Index + 2];
    pPolygon->m_Vertex[1].m_X = pVB->m_pData[v2Index];
    pPolygon->m_Vertex[1].m_Y = pVB->m_pData[v2Index + 1];
    pPolygon->m_Vertex[1].m_Z = pVB->m_pData[v2Index + 2];
    pPolygon->m_Vertex[2].m_X = pVB->m_pData[v3Index];
    pPolygon->m_Vertex[2].m_Y = pVB->m_pData[v3Index + 1];
    pPolygon->m_Vertex[2].m_Z = pVB->m_pData[v3Index + 2];

    offset += 3;

    // extract the normal from source vertex buffer
    if (pVB->m_Format.m_HasNormal)
    {
        pNormal[0].m_X = pVB->m_pData[v1Index + offset];
        pNormal[0].m_Y = pVB->m_pData[v1Index + offset + 1];
        pNormal[0].m_Z = pVB->m_pData[v1Index + offset + 2];
        pNormal[1].m_X = pVB->m_pData[v2Index + offset];
        pNormal[1].m_Y = pVB->m_pData[v2Index + offset + 1];
        pNormal[1].m_Z = pVB->m_pData[v2Index + offset + 2];
        pNormal[2].m_X = pVB->m_pData[v3Index + offset];
        pNormal[2].m_Y = pVB->m_pData[v3Index + offset + 1];
        pNormal[2].m_Z = pVB->m_pData[v3Index + offset + 2];

        offset += 3;
    }
    else
    {
        pNormal[0].m_X = 0.0f;
        pNormal[0].m_Y = 0.0f;
        pNormal[0].m_Z = 0.0f;
        pNormal[1].m_X = 0.0f;
        pNormal[1].m_Y = 0.0f;
        pNormal[1].m_Z = 0.0f;
        pNormal[2].m_X = 0.0f;
        pNormal[2].m_Y = 0.0f;
        pNormal[2].m_Z = 0.0f;
    }

    // extract the texture coordinates from source vertex buffer
    if (pVB->m_Format.m_HasTexCoords)
    {
        pST[0].m_X = pVB->m_pData[v1Index + offset];
        pST[0].m_Y = pVB->m_pData[v1Index + offset + 1];
        pST[1].m_X = pVB->m_pData[v2Index + offset];
        pST[1].m_Y = pVB->m_pData[v2Index + offset + 1];
        pST[2].m_X = pVB->m_pData[v3Index + offset];
        pST[2].m_Y = pVB->m_pData[v3Index + offset + 1];

        offset += 2;
    }
    else
    {
        pST[0].m_X = 0.0f;
        pST[0].m_Y = 0.0f;
        pST[1].m_X = 0.0f;
        pST[1].m_Y = 0.0f;
        pST[2].m_X = 0.0f;
        pST[2].m_Y = 0.0f;
    }

    // extract the color from source vertex buffer
    if (pVB->m_Format.m_HasPerVertexColor)
    {
        pColor[0] = ((((unsigned char)floor(255.0f * pVB->m_pData[v1Index + offset])     & 0xFF) << 24) |
                     (((unsigned char)floor(255.0f * pVB->m_pData[v1Index + offset + 1]) & 0xFF) << 16) |
                     (((unsigned char)floor(255.0f * pVB->m_pData[v1Index + offset + 2]) & 0xFF) << 8)  |
                      ((unsigned char)floor(255.0f * pVB->m_pData[v1Index + offset + 3]) & 0xFF));
        pColor[1] = ((((unsigned char)floor(255.0f * pVB->m_pData[v2Index + offset])     & 0xFF) << 24) |
                     (((unsigned char)floor(255.0f * pVB->m_pData[v2Index + offset + 1]) & 0xFF) << 16) |
                     (((unsigned char)floor(255.0f * pVB->m_pData[v2Index + offset + 2]) & 0xFF) << 8)  |
                      ((unsigned char)floor(255.0f * pVB->m_pData[v2Index + offset + 3]) & 0xFF));
        pColor[2] = ((((unsigned char)floor(255.0f * pVB->m_pData[v3Index + offset])     & 0xFF) << 24) |
                     (((unsigned char)floor(255.0f * pVB->m_pData[v3Index + offset + 1]) & 0xFF) << 16) |
                     (((unsigned char)floor(255.0f * pVB->m_pData[v3Index + offset + 2]) & 0xFF) << 8)  |
                      ((unsigned char)floor(255.0f * pVB->m_pData[v3Index + offset + 3]) & 0xFF));
    }
    else
    {
        pColor[0] = 0;
        pColor[1] = 0;
        pColor[2] = 0;
    }

    return 1;
}
//---------------------------------------------------------------------------
int CSR_SoftwareRaster::csrRasterDrawPolygon(const CSR_Polygon3*    pPolygon,
                                             const CSR_Vector3*     pNormal,
                                             const CSR_Vector2*     pST,
                                             const unsigned*        pColor,
                                             const CSR_Matrix4*     pMatrix,
                                                   float            zNear,
                                             const CSR_Rect*        pScreenRect,
                                                   CSR_FrameBuffer* pFB,
                                                   CSR_DepthBuffer* pDB)
{
    float        xMin;
    float        yMin;
    float        xMax;
    float        yMax;
    float        xStart;
    float        yStart;
    float        xEnd;
    float        yEnd;
    float        area;
    float        w0;
    float        w1;
    float        w2;
    float        invZ;
    float        z;
    float        pixelX;
    float        pixelY;
    float        nDotView;
    float        checker;
    float        c;
    size_t       x;
    size_t       y;
    size_t       x0;
    size_t       x1;
    size_t       y0;
    size_t       y1;
    CSR_Polygon3 rasterPoly;
    CSR_Polygon3 cameraPoly;
    CSR_Vector2  st[3];
    CSR_Vector2  stCoord;
    CSR_Vector3  pixelSample;
    CSR_Vector3  point;
    CSR_Vector3  v1v0Cam;
    CSR_Vector3  v1v0CamXv2v0Cam;
    CSR_Vector3  v2v0Cam;
    CSR_Vector3  normal;
    CSR_Vector3  viewDirection;
    CSR_Vector3  nViewDir;

    //FIXME const int M = 10;

    // validate the input
    if (!pPolygon || !pNormal || !pST || !pColor || !pMatrix || !pScreenRect || !pFB || !pDB)
        return 0;

    // rasterize the polygon
    csrRasterRasterizeVertex(&pPolygon->m_Vertex[0], pMatrix, pScreenRect, zNear, pFB->m_Width, pFB->m_Height, &rasterPoly.m_Vertex[0]);
    csrRasterRasterizeVertex(&pPolygon->m_Vertex[1], pMatrix, pScreenRect, zNear, pFB->m_Width, pFB->m_Height, &rasterPoly.m_Vertex[1]);
    csrRasterRasterizeVertex(&pPolygon->m_Vertex[2], pMatrix, pScreenRect, zNear, pFB->m_Width, pFB->m_Height, &rasterPoly.m_Vertex[2]);

    // invert the vertex z-coordinate (to allow multiplication later instead of division)
    rasterPoly.m_Vertex[0].m_Z = 1.0f / rasterPoly.m_Vertex[0].m_Z,
    rasterPoly.m_Vertex[1].m_Z = 1.0f / rasterPoly.m_Vertex[1].m_Z,
    rasterPoly.m_Vertex[2].m_Z = 1.0f / rasterPoly.m_Vertex[2].m_Z;

    // calculate the texture coordinates, divde them by their vertex z-coordinate
    st[0].m_X = pST[0].m_X * rasterPoly.m_Vertex[0].m_Z;
    st[0].m_Y = pST[0].m_Y * rasterPoly.m_Vertex[0].m_Z;
    st[1].m_X = pST[1].m_X * rasterPoly.m_Vertex[1].m_Z;
    st[1].m_Y = pST[1].m_Y * rasterPoly.m_Vertex[1].m_Z;
    st[2].m_X = pST[2].m_X * rasterPoly.m_Vertex[2].m_Z;
    st[2].m_Y = pST[2].m_Y * rasterPoly.m_Vertex[2].m_Z;

    // calculate the polygon bounding rect
    xMin = csrRasterFindMin(rasterPoly.m_Vertex[0].m_X, rasterPoly.m_Vertex[1].m_X, rasterPoly.m_Vertex[2].m_X);
    yMin = csrRasterFindMin(rasterPoly.m_Vertex[0].m_Y, rasterPoly.m_Vertex[1].m_Y, rasterPoly.m_Vertex[2].m_Y);
    xMax = csrRasterFindMax(rasterPoly.m_Vertex[0].m_X, rasterPoly.m_Vertex[1].m_X, rasterPoly.m_Vertex[2].m_X);
    yMax = csrRasterFindMax(rasterPoly.m_Vertex[0].m_Y, rasterPoly.m_Vertex[1].m_Y, rasterPoly.m_Vertex[2].m_Y);

    // is the polygon out of screen?
    if (xMin > (float)(pFB->m_Width  - 1) || xMax < 0.0f ||
        yMin > (float)(pFB->m_Height - 1) || yMax < 0.0f)
        return 1;

    // calculate the area to draw
    csrMathMax(0.0f,                       xMin, &xStart);
    csrMathMin((float)(pFB->m_Width  - 1), xMax, &xEnd);
    csrMathMax(0.0f,                       yMin, &yStart);
    csrMathMin((float)(pFB->m_Height - 1), yMax, &yEnd);

    x0 = floor(xStart);
    x1 = floor(xEnd);
    y0 = floor(yStart);
    y1 = floor(yEnd);

    area = csrRasterFindEdge(&rasterPoly.m_Vertex[0], &rasterPoly.m_Vertex[1], &rasterPoly.m_Vertex[2]);

    // iterate through pixels to draw
    for (y = y0; y <= y1; ++y)
        for (x = x0; x <= x1; ++x)
        {
            pixelSample.m_X = x + 0.5f;
            pixelSample.m_Y = y + 0.5f;
            pixelSample.m_Z =     0.0f;

            // calculate the pixel position
            w0 = csrRasterFindEdge(&rasterPoly.m_Vertex[1], &rasterPoly.m_Vertex[2], &pixelSample);
            w1 = csrRasterFindEdge(&rasterPoly.m_Vertex[2], &rasterPoly.m_Vertex[0], &pixelSample);
            w2 = csrRasterFindEdge(&rasterPoly.m_Vertex[0], &rasterPoly.m_Vertex[1], &pixelSample);

            // FIXME here the culling is tested. If culling is inverted, also invert the wx value below (e.g. with -area)
            // is pixel visible?
            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                w0 /= area;
                w1 /= area;
                w2 /= area;

                // calculate the pixel z weight
                invZ = (rasterPoly.m_Vertex[0].m_Z * w0) +
                       (rasterPoly.m_Vertex[1].m_Z * w1) +
                       (rasterPoly.m_Vertex[2].m_Z * w2);
                z    = 1.0f / invZ;

                // test the pixel against the depth buffer
                if (z < pDB->m_pData[y * pFB->m_Width + x])
                {
                    // test passed, update the depth buffer
                    pDB->m_pData[y * pFB->m_Width + x] = z;

                    /* FIXME map a real texture here, and use the vertex buffer normal instead
                    // calculate the texture coordinate
                    stCoord.m_X = ((st[0].m_X * w0) + (st[1].m_X * w1) + (st[2].m_X * w2)) * z;
                    stCoord.m_Y = ((st[0].m_Y * w0) + (st[1].m_Y * w1) + (st[2].m_Y * w2)) * z;

                    // If you need to compute the actual position of the shaded point in camera space.
                    // Proceed like with the other vertex attribute. Divide the point coordinates by
                    // the vertex z-coordinate then interpolate using barycentric coordinates and
                    // finally multiply by sample depth.
                    csrMat4Transform(pMatrix, &pPolygon->m_Vertex[0], &cameraPoly.m_Vertex[0]);
                    csrMat4Transform(pMatrix, &pPolygon->m_Vertex[1], &cameraPoly.m_Vertex[1]);
                    csrMat4Transform(pMatrix, &pPolygon->m_Vertex[2], &cameraPoly.m_Vertex[2]);

                    pixelX = (cameraPoly.m_Vertex[0].m_X / -cameraPoly.m_Vertex[0].m_Z) * w0 +
                             (cameraPoly.m_Vertex[1].m_X / -cameraPoly.m_Vertex[1].m_Z) * w1 +
                             (cameraPoly.m_Vertex[2].m_X / -cameraPoly.m_Vertex[2].m_Z) * w2;
                    pixelY = (cameraPoly.m_Vertex[0].m_Y / -cameraPoly.m_Vertex[0].m_Z) * w0 +
                             (cameraPoly.m_Vertex[1].m_Y / -cameraPoly.m_Vertex[1].m_Z) * w1 +
                             (cameraPoly.m_Vertex[2].m_Y / -cameraPoly.m_Vertex[2].m_Z) * w2;

                    // calculate the pixel in the camera space
                    point.m_X =  pixelX * z;
                    point.m_Y =  pixelY * z;
                    point.m_Z = -z;

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

                    // FIXME map a real texture here
                    // The final color is the reuslt of the faction ratio multiplied by the checkerboard pattern.
                    checker   = (fmod(stCoord.m_X * M, 1.0f) > 0.5f) ^ (fmod(stCoord.m_Y * M, 1.0f) < 0.5f);
                    c         = 0.3 * (1 - checker) + 0.7 * checker;
                    nDotView *= c;
                    */

                    // fixme use the result of the pixel shader here
                    // write the pixel inside the frame buffer
                    pFB->m_pPixel[y * pFB->m_Width + x].m_R = 255;//FIXME nDotView * 255;
                    pFB->m_pPixel[y * pFB->m_Width + x].m_G = 0;//FIXME nDotView * 255;
                    pFB->m_pPixel[y * pFB->m_Width + x].m_B = 0;//FIXME nDotView * 255;
                }
            }
        }

    return 1;
}
//---------------------------------------------------------------------------
int CSR_SoftwareRaster::csrRasterDraw(const CSR_Matrix4*      pMatrix,
                                            float             zNear,
                                            float             zFar,
                                      const CSR_VertexBuffer* pVB,
                                      const CSR_Raster*       pRaster,
                                            CSR_FrameBuffer*  pFB,
                                            CSR_DepthBuffer*  pDB)
{
    size_t       vertexCount;
    size_t       i;
    size_t       index;
    float        determinant;
    CSR_Rect     screenRect;
    CSR_Matrix4  invMat;
    CSR_Polygon3 polygon;
    CSR_Vector3  normal[3];
    CSR_Vector2  st[3];
    unsigned     color[3];

    // validate the input
    if (!pMatrix || !pVB || !pVB->m_Format.m_Stride || !pRaster || !pFB || !pDB)
        return 0;

    csrMat4Inverse(pMatrix, &invMat, &determinant);

    // get the raster screen coordinates
    csrRasterGetScreenCoordinates(pRaster,
                                  pFB->m_Width,
                                  pFB->m_Height,
                                  zNear,
                                 &screenRect);

    // search for vertex type
    switch (pVB->m_Format.m_Type)
    {
        case CSR_VT_Triangles:
        {
            // calculate iteration step
            const unsigned step = (pVB->m_Format.m_Stride * 3);

            // iterate through source vertices
            for (i = 0; i < pVB->m_Count; i += step)
            {
                // get the next polygon to draw
                if (!csrRasterGetPolygon(i,
                                         i +  pVB->m_Format.m_Stride,
                                         i + (pVB->m_Format.m_Stride * 2),
                                         pVB,
                                        &polygon,
                                         normal,
                                         st,
                                         color))
                    return 0;

                // draw the polygon
                if (!csrRasterDrawPolygon(&polygon,
                                           normal,
                                           st,
                                           color,
                                           pMatrix,
                                           zNear,
                                          &screenRect,
                                           pFB,
                                           pDB))
                    return 0;
            }

            return 1;
        }

        case CSR_VT_TriangleStrip:
        {
            // calculate length to read in triangle strip buffer
            const unsigned stripLength = (pVB->m_Count - (pVB->m_Format.m_Stride * 2));

            index = 0;

            // iterate through source vertices
            for (i = 0; i < stripLength; i += pVB->m_Format.m_Stride)
            {
                // extract polygon from source buffer, revert odd polygons
                if (!index || !(index % 2))
                {
                    // get the next polygon to draw
                    if (!csrRasterGetPolygon(i,
                                             i +  pVB->m_Format.m_Stride,
                                             i + (pVB->m_Format.m_Stride * 2),
                                             pVB,
                                            &polygon,
                                             normal,
                                             st,
                                             color))
                        return 0;
                }
                else
                {
                    // get the next polygon to draw
                    if (!csrRasterGetPolygon(i +  pVB->m_Format.m_Stride,
                                             i,
                                             i + (pVB->m_Format.m_Stride * 2),
                                             pVB,
                                            &polygon,
                                             normal,
                                             st,
                                             color))
                        return 0;
                }

                // draw the polygon
                if (!csrRasterDrawPolygon(&polygon,
                                           normal,
                                           st,
                                           color,
                                           pMatrix,
                                           zNear,
                                          &screenRect,
                                           pFB,
                                           pDB))
                    return 0;

                ++index;
            }

            return 1;
        }

        case CSR_VT_TriangleFan:
        {
            // calculate length to read in triangle fan buffer
            const unsigned fanLength = (pVB->m_Count - pVB->m_Format.m_Stride);

            // iterate through source vertices
            for (i = pVB->m_Format.m_Stride; i < fanLength; i += pVB->m_Format.m_Stride)
            {
                // get the next polygon to draw
                if (!csrRasterGetPolygon(0,
                                         i,
                                         i + pVB->m_Format.m_Stride,
                                         pVB,
                                        &polygon,
                                         normal,
                                         st,
                                         color))
                    return 0;

                // draw the polygon
                if (!csrRasterDrawPolygon(&polygon,
                                           normal,
                                           st,
                                           color,
                                           pMatrix,
                                           zNear,
                                          &screenRect,
                                           pFB,
                                           pDB))
                    return 0;
            }

            return 1;
        }

        case CSR_VT_Quads:
        {
            // calculate iteration step
            const unsigned step = (pVB->m_Format.m_Stride * 4);

            // iterate through source vertices
            for (i = 0; i < pVB->m_Count; i += step)
            {
                // calculate vertices position
                const unsigned v1 = i;
                const unsigned v2 = i +  pVB->m_Format.m_Stride;
                const unsigned v3 = i + (pVB->m_Format.m_Stride * 2);
                const unsigned v4 = i + (pVB->m_Format.m_Stride * 3);

                // get the next polygon to draw
                if (!csrRasterGetPolygon(v1,
                                         v2,
                                         v3,
                                         pVB,
                                        &polygon,
                                         normal,
                                         st,
                                         color))
                    return 0;

                // draw the polygon
                if (!csrRasterDrawPolygon(&polygon,
                                           normal,
                                           st,
                                           color,
                                           pMatrix,
                                           zNear,
                                          &screenRect,
                                           pFB,
                                           pDB))
                    return 0;

                // get the next polygon to draw
                if (!csrRasterGetPolygon(v3,
                                         v2,
                                         v4,
                                         pVB,
                                        &polygon,
                                         normal,
                                         st,
                                         color))
                    return 0;

                // draw the polygon
                if (!csrRasterDrawPolygon(&polygon,
                                           normal,
                                           st,
                                           color,
                                           pMatrix,
                                           zNear,
                                          &screenRect,
                                           pFB,
                                           pDB))
                    return 0;
            }

            return 1;
        }

        case CSR_VT_QuadStrip:
        {
            // calculate iteration step
            const unsigned step = (pVB->m_Format.m_Stride * 2);

            // calculate length to read in triangle strip buffer
            const unsigned stripLength = (pVB->m_Count - (pVB->m_Format.m_Stride * 2));

            // iterate through source vertices
            for (i = 0; i < stripLength; i += step)
            {
                // calculate vertices position
                const unsigned v1 = i;
                const unsigned v2 = i +  pVB->m_Format.m_Stride;
                const unsigned v3 = i + (pVB->m_Format.m_Stride * 2);
                const unsigned v4 = i + (pVB->m_Format.m_Stride * 3);

                // get the next polygon to draw
                if (!csrRasterGetPolygon(v1,
                                         v2,
                                         v3,
                                         pVB,
                                        &polygon,
                                         normal,
                                         st,
                                         color))
                    return 0;

                // draw the polygon
                if (!csrRasterDrawPolygon(&polygon,
                                           normal,
                                           st,
                                           color,
                                           pMatrix,
                                           zNear,
                                          &screenRect,
                                           pFB,
                                           pDB))
                    return 0;

                // get the next polygon to draw
                if (!csrRasterGetPolygon(v3,
                                         v2,
                                         v4,
                                         pVB,
                                        &polygon,
                                         normal,
                                         st,
                                         color))
                    return 0;

                // draw the polygon
                if (!csrRasterDrawPolygon(&polygon,
                                           normal,
                                           st,
                                           color,
                                           pMatrix,
                                           zNear,
                                          &screenRect,
                                           pFB,
                                           pDB))
                    return 0;
            }

            return 1;
        }

        default:
            return 0;
    }
}
//---------------------------------------------------------------------------
