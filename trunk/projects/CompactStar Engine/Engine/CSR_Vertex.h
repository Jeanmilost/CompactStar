/****************************************************************************
 * ==> CSR_Vertex ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the vertex functions and types        *
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

#ifndef CSR_VertexH
#define CSR_VertexH

// compactStar engine
#include "CSR_Geometry.h"
#include "CSR_Texture.h"

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

// todo FIXME -cFeature -oJean: add the cull mode and face

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
    CSR_Texture       m_Texture;
    CSR_VertexBuffer* m_pVB;
    size_t            m_Count;
} CSR_Mesh;

/**
* Indexed polygon
*/
typedef struct
{
    const CSR_VertexBuffer* m_pVB;
          size_t            m_pIndex[3];
} CSR_IndexedPolygon;

/**
* Indexed polygon buffer
*/
typedef struct
{
    CSR_IndexedPolygon* m_pIndexedPolygon;
    size_t              m_Count;
} CSR_IndexedPolygonBuffer;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Vertex format functions
        //-------------------------------------------------------------------

        /**
        * Calculates the vertex stride
        *@param[in, out] pVertexFormat - vertex format for which the stride should be calculated
        */
        void csrVertexFormatCalculateStride(CSR_VertexFormat* pVertexFormat);

        //-------------------------------------------------------------------
        // Vertex buffer functions
        //-------------------------------------------------------------------

        /**
        * Creates a vertex buffer
        *@return newly created vertex buffer, 0 on error
        *@note The vertex buffer must be released when no longer used, see csrVertexBufferRelease()
        */
        CSR_VertexBuffer* csrVertexBufferCreate(void);

        /**
        * Releases a vertex buffer
        *@param pVB - vertex buffer to release
        */
        void csrVertexBufferRelease(CSR_VertexBuffer* pVB);

        /**
        * Adds a vertex to a vertex buffer
        *@param pVertex - vertex
        *@param pNormal - normal
        *@param pUV - texture coordinate
        *@param color - color
        *@param[in, out] pVB - vertex buffer to add to
        *@return 1 on success, otherwise 0
        */
        int csrVertexBufferAdd(CSR_Vector3*      pVertex,
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
        // Indexed polygon functions
        //-------------------------------------------------------------------

        /**
        * Gets a polygon from an indexed polygon
        *@param pIndexedPolygon - indexed polygon to get from
        *@param[out] pPolygon - polygon
        *@return 1 on success, otherwise 0
        */
        int csrIndexedPolygonToPolygon(const CSR_IndexedPolygon* pIndexedPolygon,
                                             CSR_Polygon3*       pPolygon);

        //-------------------------------------------------------------------
        // Indexed polygon buffer functions
        //-------------------------------------------------------------------

        /**
        * Creates an indexed polygon buffer
        *@return newly created indexed polygon buffer, 0 on error
        *@note The indexed polygon buffer must be released when no longer used, see
               csrIndexedPolygonBufferRelease()
        */
        CSR_IndexedPolygonBuffer* csrIndexedPolygonBufferCreate(void);

        /**
        * Releases an indexed polygon buffer
        *@param pIPB - indexed polygon buffer to release
        */
        void csrIndexedPolygonBufferRelease(CSR_IndexedPolygonBuffer* pIPB);

        /**
        * Adds an indexed polygon to an indexed polygon buffer
        *@param pIndexedPolygon - indexed polygon to add to the indexed polygon buffer
        *@param[in, out] pIPB - indexed polygon buffer to add to
        *@return 1 on success, otherwise 0
        */
        int csrIndexedPolygonBufferAdd(const CSR_IndexedPolygon*       pIndexedPolygon,
                                             CSR_IndexedPolygonBuffer* pIPB);

        /**
        * Gets an indexed polygon buffer from a mesh
        *@param pMesh - mesh
        *@return indexed polygon buffer, 0 on error
        *@note The indexed polygon buffer must be released when no longer used, see
        *      csrIndexedPolygonBufferRelease()
        *@note BE CAREFUL, the indexed polygon buffer is valid as long as his source mesh is valid.
        *      If the mesh is released, the indexed polygon buffer should be released together.
        *      However the indexed polygon buffer may be released after the mesh
        */
        CSR_IndexedPolygonBuffer* csrIndexedPolygonBufferFromMesh(const CSR_Mesh* pMesh);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Vertex.c"
#endif

#endif
