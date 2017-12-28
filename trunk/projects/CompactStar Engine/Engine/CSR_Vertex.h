/****************************************************************************
 * ==> CSR_Vertex ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the basic vertex functions and types  *
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

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

/**
* Vertex buffer type enumerator
*/
typedef enum
{
    CSR_E_Triangles,
    CSR_E_TriangleStrip,
    CSR_E_TriangleFan,
    CSR_E_Quads,
    CSR_E_QuadStrip
} CSR_EVertexBufferType;

/**
* Vertex format
*/
typedef struct
{
    CSR_EVertexBufferType m_Type;
    int                   m_UseNormals;
    int                   m_UseTextures;
    int                   m_UseColors;
    unsigned              m_Stride;
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
* Indexed polygon, keep the index of each vertex composing a polygon inside a vertex buffer
*/
typedef struct
{
    size_t m_Index[3];
} CSR_IndexedPolygon;

/**
* Indexed polygon table
*/
typedef struct
{
    CSR_IndexedPolygon* m_pData;
    size_t              m_Count;
} CSR_IndexedPolygonTable;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Vertex functions
        //-------------------------------------------------------------------

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
        // Vertex buffer functions
        //-------------------------------------------------------------------

        /**
        * Creates a vertex buffer
        *@return newly created vertex buffer, 0 on error
        *@note The buffer must be released when no longer used, see csrVertexBufferRelease()
        */
        CSR_VertexBuffer* csrVertexBufferCreate(void);

        /**
        * Releases a vertex buffer
        *@param pVB - vertex buffer to release
        */
        void csrVertexBufferRelease(CSR_VertexBuffer* pVB);

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
        *@param pVB - vertex buffer containing the vertices referred by the indexed polygon
        *@param pIP - indexed polygon
        *@param[out] pP - polygon
        *@return 1 on success, otherwise 0
        */
        int csrIndexedToPolygon(const CSR_VertexBuffer*   pVB,
                                const CSR_IndexedPolygon* pIP,
                                      CSR_Polygon3*       pP);

        /**
        * Adds a new indexed polygon inside an indexed polygon table
        *@param i1 - polygon first vertex index
        *@param i2 - polygon second vertex index
        *@param i3 - polygon third vertex index
        *@param[in, out] pIT - indexed polygon table in which the new indexed polygon should be added
        */
        void csrIndexedAdd(size_t i1, size_t i2, size_t i3, CSR_IndexedPolygonTable* pIT);

        //-------------------------------------------------------------------
        // Indexed polygon table functions
        //-------------------------------------------------------------------

        /**
        * Creates an index polygon table
        *@return newly created indexed polygon table, 0 on error
        *@note The table must be released when no longer used, see csrIndexedPolygonTableRelease()
        */
        CSR_IndexedPolygonTable* csrIndexedPolygonTableCreate(void);

        /**
        * Extracts indexed polygons from a vertex buffer
        *@param pVB - source vertex buffer from which the indexed polygons should be extracted
        *@return newly created indexed polygon table, 0 on error
        *@note The table must be released when no longer used, see csrIndexedPolygonTableRelease()
        */
        CSR_IndexedPolygonTable* csrIndexedPolygonTableFromVB(const CSR_VertexBuffer* pVB);

        /**
        * Releases an index polygon table
        *@param pIT - indexed polygon table to release
        */
        void csrIndexedPolygonTableRelease(CSR_IndexedPolygonTable* pIT);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Common.c"
#endif

#endif
