#ifndef CSR_RasterH
#define CSR_RasterH

// std
#include <stddef.h>

// compactStart engine
#include "CSR_Geometry.h"

//REM
#include "geometry.h"

typedef enum
{
    CSR_RG_Fill = 0,
    CSR_RG_Overscan
} CSR_EFitResolutionGate;

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

class CSR_Raster
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

        static float csrRasterFindMin(float a, float b, float c);
        static float csrRasterFindMax(float a, float b, float c);
        static float csrRasterFindEdge(const Vec3f &a, const Vec3f &b, const Vec3f &c);

        //Compute screen coordinates based on a physically-based camera model
        // http://www.scratchapixel.com/lessons/3d-basic-rendering/3d-viewing-pinhole-camera
        static void csrRasterGetScreenCoordinates(float                  filmApertureWidth,
                                                  float                  filmApertureHeight,
                                                  size_t                 imageWidth,
                                                  size_t                 imageHeight,
                                                  CSR_EFitResolutionGate fitFilm,
                                                  float                  nearClippingPLane,
                                                  float                  focalLength,
                                                  float&                 top,
                                                  float&                 bottom,
                                                  float&                 left,
                                                  float&                 right);

        // Compute vertex raster screen coordinates. Vertices are defined in world space.
        // They are then converted to camera space, then to NDC space (in the range [-1,1]) and then to raster space.
        // The z-coordinates of the vertex in raster space is set with the z-coordinate of the vertex in camera space.
        static void csrRasterConvertTo(const Vec3f&     vertexWorld,
                                       const Matrix44f& worldToCamera,
                                             float      l,
                                             float      r,
                                             float      t,
                                             float      b,
                                             float      zNear,
                                             unsigned   imageWidth,
                                             unsigned   imageHeight,
                                             Vec3f&     vertexRaster);

        static int csrRasterRasterize(size_t           imageWidth,
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
                                      CSR_FrameBuffer* pFB);
;
};

#endif
