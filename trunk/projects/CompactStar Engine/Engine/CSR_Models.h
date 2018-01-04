/****************************************************************************
 * ==> CSR_Models ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the basic models functions and types  *
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

#ifndef CSR_ModelsH
#define CSR_ModelsH

// compactStar engine
#include "CSR_Geometry.h"

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

/**
* Vertex type
*/
typedef enum
{
    CSR_VT_Triangles,
    CSR_VT_TriangleStrip,
    CSR_VT_TriangleFan,
    CSR_VT_Quads,
    CSR_VT_QuadStrip
} CSR_EVertexType;

/**
* Vertex format
*/
typedef struct
{
    CSR_EVertexType m_Type;
    int             m_UseNormals;
    int             m_UseTextures;
    int             m_UseColors;
    unsigned        m_Stride;
} CSR_VertexFormat;

/**
* Vertex buffer
*/
typedef struct
{
    CSR_VertexFormat m_Format;
    float*           m_pData;
    size_t           m_Count;
} CSR_VertexBuffer;

/**
* Mesh
*/
typedef struct
{
    CSR_VertexBuffer* m_pVertices;
    size_t            m_Count;
} CSR_Mesh;

/**
* Polygon index, used to get the polygon vertices from a vertex buffer
*/
typedef struct
{
    const CSR_VertexBuffer* m_pVB;
          size_t            m_pIndex[3];
} CSR_PolygonIndex;

/**
* Polygon buffer
*/
typedef struct
{
    CSR_PolygonIndex* m_pPolygons;
    size_t            m_Count;
} CSR_PolygonBuffer;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Vertex functions
        //-------------------------------------------------------------------

        /**
        * Creates a vertex buffer
        *@return newly created vertex buffer, 0 on error
        *@note The vertex buffer must be released when no longer used, see csrVertexBufferRelease()
        */
        CSR_VertexBuffer* csrVertexBufferCreate(void);

        /**
        * Releases a vertex buffer
        *@param pV - vertex buffer to release
        */
        void csrVertexBufferRelease(CSR_VertexBuffer* pV);

        /**
        * Calculates the vertex stride
        *@param[in, out] pVertexFormat - vertex format for which the stride should be calculated
        */
        void csrVertexCalculateStride(CSR_VertexFormat* pVertexFormat);

        /**
        * Adds a vertex to a vertex buffer
        *@param pVertex - vertex
        *@param pNormal - normal
        *@param pUV - texture coordinate
        *@param color - color
        *@param[in, out] pVB - vertex buffer to add to
        */
        void csrVertexAdd(CSR_Vector3*      pVertex,
                          CSR_Vector3*      pNormal,
                          CSR_Vector2*      pUV,
                          unsigned          color,
                          CSR_VertexBuffer* pVB);

        //-------------------------------------------------------------------
        // Mesh functions
        //-------------------------------------------------------------------

        /**
        * Creates a mesh
        *@return newly created mesh, 0 on error
        *@note The mesh must be released when no longer used, see csrMeshRelease()
        */
        CSR_Mesh* csrMeshCreate(void);

        /**
        * Releases a mesh
        *@param pMesh - mesh to release
        */
        void csrMeshRelease(CSR_Mesh* pMesh);

        //-------------------------------------------------------------------
        // Polygon functions
        //-------------------------------------------------------------------

        /**
        * Creates a polygon buffer
        *@return newly created polygon buffer, 0 on error
        *@note The polygon buffer must be released when no longer used, see csrPolygonBufferRelease()
        */
        CSR_PolygonBuffer* csrPolygonBufferCreate(void);

        /**
        * Releases a polygon buffer
        *@param pPB - polygon buffer to release
        */
        void csrPolygonBufferRelease(CSR_PolygonBuffer* pPB);

        /**
        * Gets a polygon from a polygon index
        *@param pPolygonIndex - polygon index to get from
        *@param[out] pPolygon - polygon to populate
        */
        void csrPolygonFromIndex(const CSR_PolygonIndex* pPolygonIndex, CSR_Polygon3* pPolygon);

        /**
        * Adds a polygon index to a polygon buffer
        *@param pPI - polygon index to add to the polygon buffer
        *@param[in, out] pPB - polygon buffer to add to
        */
        void csrPolygonIndexAdd(const CSR_PolygonIndex* pPI, CSR_PolygonBuffer* pPB);

        /**
        * Gets a polygon buffer from a mesh
        *@param pMesh - mesh
        *@return polygon buffer, 0 on error
        *@note The polygon buffer must be released when no longer used, see csrPolygonBufferRelease()
        *@note BE CAREFUL, the polygon buffer is valid as long as his source mesh is valid. If the
        *      mesh is released, the polygon buffer should be released together. However the polygon
        *      buffer may be released after the mesh
        */
        CSR_PolygonBuffer* csrPolygonBufferFromMesh(const CSR_Mesh* pMesh);

        //-------------------------------------------------------------------
        // Shape functions
        //-------------------------------------------------------------------

        /**
        * Creates a surface
        *@param width - surface width
        *@param height - surface height
        *@param color - color in ARGB format
        *@return mesh containing the surface, 0 on error
        *@note The mesh must be released when no longer used, see csrMeshRelease()
        */
        CSR_Mesh* csrShapeCreateSurface(const CSR_VertexFormat* pVertexFormat,
                                              float             width,
                                              float             height,
                                              unsigned          color);

//        /**
//        * Creates a sphere
//        *@param pRadius - sphere radius
//        *@param slices - slices (longitude) number
//        *@param stacks - stacks (latitude) number
//        *@param color - color in RGBA format
//        *@param [in, out] pVertexFormat - vertex format to use
//        *@param[out] pVertices - resulting vertex buffer
//        *@param[out] pVertexCount - vertex count in buffer
//        *@param[out] pIndexes - index list containing offset and length of each vertex buffer composing sphere
//        *@param[out] pIndexCount - resulting index count
//        */
//        void miniCreateSphere(const float*             pRadius,
//                                    int                slices,
//                                    int                stacks,
//                                    unsigned           color,
//                                    MINI_VertexFormat* pVertexFormat,
//                                    float**            pVertices,
//                                    unsigned*          pVertexCount,
//                                    MINI_Index**       pIndexes,
//                                    unsigned*          pIndexCount);
//
//        /**
//        * Creates a cylinder
//        *@param pRadius - cylinder radius
//        *@param pHeight - cylinder height
//        *@param faces - number of faces composing the cylinder
//        *@param color - color in RGBA format
//        *@param[in, out] pVertexFormat - vertex format to use
//        *@param[out] pVertices - resulting vertex buffer
//        *@param[out] pVertexCount - vertex count in buffer
//        */
//        void miniCreateCylinder(const float*             pRadius,
//                                const float*             pHeight,
//                                      int                faces,
//                                      unsigned           color,
//                                      MINI_VertexFormat* pVertexFormat,
//                                      float**            pVertices,
//                                      unsigned*          pVertexCount);
//
//        /**
//        * Creates a disk
//        *@param centerX - the disk center on the x axis
//        *@param centerY - the disk center on the y axis
//        *@param radius - disk radius
//        *@param sliceCount - disk slice count
//        *@param color - disk color
//        *@param pVertexFormat - vertex format to apply
//        *@param[out] pVertices - resulting vertex buffer
//        *@param[out] pVertexCount - vertex count in buffer
//        */
//        void miniCreateDisk(float              centerX,
//                            float              centerY,
//                            float              radius,
//                            unsigned           sliceCount,
//                            unsigned           color,
//                            MINI_VertexFormat* pVertexFormat,
//                            float**            pVertices,
//                            unsigned*          pVertexCount);
//
//        /**
//        * Creates a ring
//        *@param centerX - the ring center on the x axis
//        *@param centerY - the ring center on the y axis
//        *@param minRadius - internal radius
//        *@param maxRadius - external radius
//        *@param sliceCount - slice count
//        *@param minColor - internal edge color
//        *@param maxColor - external edge color
//        *@param pVertexFormat - vertex format to apply
//        *@param[out] pVertices - resulting vertex buffer
//        *@param[out] pVertexCount - vertex count in buffer
//        */
//        void miniCreateRing(float              centerX,
//                            float              centerY,
//                            float              minRadius,
//                            float              maxRadius,
//                            unsigned           sliceCount,
//                            unsigned           minColor,
//                            unsigned           maxColor,
//                            MINI_VertexFormat* pVertexFormat,
//                            float**            pVertices,
//                            unsigned*          pVertexCount);
//
//        /**
//        * Creates a spiral
//        *@param centerX - the spiral center on the x axis
//        *@param centerY - the spiral center on the y axis
//        *@param minRadius - internal radius
//        *@param maxRadius - external radius
//        *@param deltaMin - delta to apply to the min radius
//        *@param deltaMax - delta to apply to the max radius
//        *@param deltaZ - delta to apply to the z radius
//        *@param stackCount - stack count
//        *@param sliceCount - slice count
//        *@param minColor - internal edge color
//        *@param maxColor - external edge color
//        *@param pVertexFormat - vertex format to apply
//        *@param[out] pVertices - resulting vertex buffer
//        *@param[out] pVertexCount - vertex count in buffer
//        *@param[out] pIndexes - resulting index buffer
//        *@param[out] pIndexCount - index count in buffer
//        */
//        void miniCreateSpiral(float              centerX,
//                              float              centerY,
//                              float              minRadius,
//                              float              maxRadius,
//                              float              deltaMin,
//                              float              deltaMax,
//                              float              deltaZ,
//                              unsigned           stackCount,
//                              unsigned           sliceCount,
//                              unsigned           minColor,
//                              unsigned           maxColor,
//                              MINI_VertexFormat* pVertexFormat,
//                              float**            pVertices,
//                              unsigned*          pVertexCount,
//                              MINI_Index**       pIndexes,
//                              unsigned*          pIndexCount);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Models.c"
#endif

#endif
