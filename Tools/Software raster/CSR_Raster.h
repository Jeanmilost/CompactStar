#ifndef CSR_RasterH
#define CSR_RasterH

// std
#include <stddef.h>

// compactStart engine
#include "CSR_Geometry.h"
#include "CSR_Vertex.h"

typedef enum
{
    CSR_RT_Fill = 0,
    CSR_RT_Overscan
} CSR_ERasterType;

typedef struct
{
    unsigned char m_R;
    unsigned char m_G;
    unsigned char m_B;
    unsigned char m_A;
} CSR_Pixel;

typedef struct
{
    CSR_Pixel* m_pPixel;
    size_t     m_Width;
    size_t     m_Height;
    size_t     m_Size;
} CSR_FrameBuffer;

typedef struct
{
    float* m_pData;
    size_t m_Width;
    size_t m_Height;
    size_t m_Size;
} CSR_DepthBuffer;

typedef struct
{
    float           m_ApertureWidth;  // in inches
    float           m_ApertureHeight; // in inches
    float           m_FocalLength;    // in mm
    CSR_ERasterType m_Type;
} CSR_Raster;

class CSR_SoftwareRaster
{
    public:
        static CSR_FrameBuffer* csrFrameBufferCreate(size_t width, size_t height);
        static int csrFrameBufferInit(size_t width, size_t height, CSR_FrameBuffer* pFB);
        static void csrFrameBufferRelease(CSR_FrameBuffer* pFB);
        static void csrFrameBufferClear(CSR_FrameBuffer* pFB, CSR_Pixel* pPixel);

        static CSR_DepthBuffer* csrDepthBufferCreate(size_t width, size_t height);
        static int csrDepthBufferInit(size_t width, size_t height, CSR_DepthBuffer* pDB);
        static void csrDepthBufferRelease(CSR_DepthBuffer* pDB);
        static void csrDepthBufferClear(CSR_DepthBuffer* pDB, float farClippingPlane);

        static void csrRasterInit(CSR_Raster* pRaster);
        static float csrRasterFindMin(float a, float b, float c);
        static float csrRasterFindMax(float a, float b, float c);
        static float csrRasterFindEdge(const CSR_Vector3* pV1,
                                       const CSR_Vector3* pV2,
                                       const CSR_Vector3* pV3);

        //Compute screen coordinates based on a physically-based camera model
        // http://www.scratchapixel.com/lessons/3d-basic-rendering/3d-viewing-pinhole-camera
        static void csrRasterGetScreenCoordinates(const CSR_Raster* pRaster,
                                                        float       imageWidth,
                                                        float       imageHeight,
                                                        float       zNear,
                                                        CSR_Rect*   pScreenRect);

        // Compute vertex raster screen coordinates. Vertices are defined in world space.
        // They are then converted to camera space, then to NDC space (in the range [-1,1]) and then to raster space.
        // The z-coordinates of the vertex in raster space is set with the z-coordinate of the vertex in camera space.
        static void csrRasterRasterizeVertex(const CSR_Vector3* pInVertex,
                                             const CSR_Matrix4* pMatrix,
                                             const CSR_Rect*    pScreenRect,
                                                   float        zNear,
                                                   float        imageWidth,
                                                   float        imageHeight,
                                                   CSR_Vector3* pOutVertex);

        static int CSR_SoftwareRaster::csrRasterGetPolygon(      size_t            v1Index,
                                                                 size_t            v2Index,
                                                                 size_t            v3Index,
                                                           const CSR_VertexBuffer* pVB,
                                                                 CSR_Polygon3*     pPolygon,
                                                                 CSR_Vector3*      pNormal,
                                                                 CSR_Vector2*      pST,
                                                                 unsigned*         pColor);

        static int csrRasterDrawPolygon(const CSR_Polygon3*     pPolygon,
                                        const CSR_Vector3*      pNormal,
                                        const CSR_Vector2*      pST,
                                        const unsigned*         pColor,
                                        const CSR_Matrix4*      pMatrix,
                                              float             zNear,
                                        const CSR_Rect*         pScreenRect,
                                              CSR_FrameBuffer*  pFB,
                                              CSR_DepthBuffer*  pDB);

        static int csrRasterDraw(const CSR_Matrix4*      pMatrix,
                                       float             zNear,
                                       float             zFar,
                                 const CSR_VertexBuffer* pVB,
                                 const CSR_Raster*       pRaster,
                                       CSR_FrameBuffer*  pFB,
                                       CSR_DepthBuffer*  pDB);
};

#endif
