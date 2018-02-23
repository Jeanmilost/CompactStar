/****************************************************************************
 * ==> CSR_SoftwareRaster --------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a small software rasterizer, that may *
 *               be used to replace OpenGL for simple rendering             *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2018, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#ifndef CSR_SoftwareRasterH
#define CSR_SoftwareRasterH

// std
#include <stddef.h>

// compactStart engine
#include "CSR_Geometry.h"
#include "CSR_Vertex.h"

//---------------------------------------------------------------------------
// Enumerators
//---------------------------------------------------------------------------

typedef enum
{
    CSR_RT_Fill = 0,
    CSR_RT_Overscan
} CSR_ERasterType;

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

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

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        //
        //-------------------------------------------------------------------

        CSR_FrameBuffer* csrFrameBufferCreate(size_t width, size_t height);
        int csrFrameBufferInit(size_t width, size_t height, CSR_FrameBuffer* pFB);
        void csrFrameBufferRelease(CSR_FrameBuffer* pFB);
        void csrFrameBufferClear(CSR_FrameBuffer* pFB, CSR_Pixel* pPixel);

        CSR_DepthBuffer* csrDepthBufferCreate(size_t width, size_t height);
        int csrDepthBufferInit(size_t width, size_t height, CSR_DepthBuffer* pDB);
        void csrDepthBufferRelease(CSR_DepthBuffer* pDB);
        void csrDepthBufferClear(CSR_DepthBuffer* pDB, float farClippingPlane);

        void csrRasterInit(CSR_Raster* pRaster);
        float csrRasterFindMin(float a, float b, float c);
        float csrRasterFindMax(float a, float b, float c);
        float csrRasterFindEdge(const CSR_Vector3* pV1,
                                       const CSR_Vector3* pV2,
                                       const CSR_Vector3* pV3);

        //Compute screen coordinates based on a physically-based camera model
        // http://www.scratchapixel.com/lessons/3d-basic-rendering/3d-viewing-pinhole-camera
        void csrRasterGetScreenCoordinates(const CSR_Raster* pRaster,
                                                        float       imageWidth,
                                                        float       imageHeight,
                                                        float       zNear,
                                                        CSR_Rect*   pScreenRect);

        // Compute vertex raster screen coordinates. Vertices are defined in world space.
        // They are then converted to camera space, then to NDC space (in the range [-1,1]) and then to raster space.
        // The z-coordinates of the vertex in raster space is set with the z-coordinate of the vertex in camera space.
        void csrRasterRasterizeVertex(const CSR_Vector3* pInVertex,
                                             const CSR_Matrix4* pMatrix,
                                             const CSR_Rect*    pScreenRect,
                                                   float        zNear,
                                                   float        imageWidth,
                                                   float        imageHeight,
                                                   CSR_Vector3* pOutVertex);

        int csrRasterGetPolygon(      size_t            v1Index,
                                      size_t            v2Index,
                                      size_t            v3Index,
                                const CSR_VertexBuffer* pVB,
                                      CSR_Polygon3*     pPolygon,
                                      CSR_Vector3*      pNormal,
                                      CSR_Vector2*      pST,
                                      unsigned*         pColor);

        int csrRasterDrawPolygon(const CSR_Polygon3*     pPolygon,
                                        const CSR_Vector3*      pNormal,
                                        const CSR_Vector2*      pST,
                                        const unsigned*         pColor,
                                        const CSR_Matrix4*      pMatrix,
                                              float             zNear,
                                        const CSR_Rect*         pScreenRect,
                                              CSR_FrameBuffer*  pFB,
                                              CSR_DepthBuffer*  pDB);

        int csrRasterDraw(const CSR_Matrix4*      pMatrix,
                                       float             zNear,
                                       float             zFar,
                                 const CSR_VertexBuffer* pVB,
                                 const CSR_Raster*       pRaster,
                                       CSR_FrameBuffer*  pFB,
                                       CSR_DepthBuffer*  pDB);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_SoftwareRaster.c"
#endif

#endif