/****************************************************************************
 * ==> CSR_Model -----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the model functions and types         *
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

#ifndef CSR_ModelH
#define CSR_ModelH

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Vertex.h"

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------

#define M_MDL_Mesh_File_Version 6
#define M_MDL_ID                (('O' << 24) + ('P' << 16) + ('D' << 8) + 'I')

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

/**
* MDL header
*/
typedef struct
{
    unsigned m_ID;
    unsigned m_Version;
    float    m_Scale[3];
    float    m_Translate[3];
    float    m_BoundingRadius;
    float    m_EyePosition[3];
    unsigned m_SkinCount;
    unsigned m_SkinWidth;
    unsigned m_SkinHeight;
    unsigned m_VertexCount;
    unsigned m_PolygonCount;
    unsigned m_FrameCount;
    unsigned m_SyncType;
    unsigned m_Flags;
    float    m_Size;
} CSR_MDLHeader;

/**
* MDL skin
*/
typedef struct
{
    unsigned       m_Group;
    unsigned       m_Count;
    unsigned       m_TexLen;
    float*         m_pTime;
    unsigned char* m_pData;
} CSR_MDLSkin;

/**
* MDL texture coordinate
*/
typedef struct
{
    unsigned m_OnSeam;
    unsigned m_U;
    unsigned m_V;
} CSR_MDLTextureCoord;

/**
* MDL polygon
*/
typedef struct
{
    unsigned m_FacesFront;
    unsigned m_VertexIndex[3];
} CSR_MDLPolygon;

/**
* MDL vertex
*/
typedef struct
{
    unsigned char m_Vertex[3];
    unsigned char m_NormalIndex;
} CSR_MDLVertex;

/**
* MDL frame
*/
typedef struct
{
    CSR_MDLVertex  m_BoundingBoxMin;
    CSR_MDLVertex  m_BoundingBoxMax;
    char           m_Name[16];
    CSR_MDLVertex* m_pVertex;
} CSR_MDLFrame;

/**
* MDL frame group
*/
typedef struct
{
    unsigned char m_Type;
    unsigned char m_Count;
    CSR_MDLVertex m_BoundingBoxMin;
    CSR_MDLVertex m_BoundingBoxMax;
    float*        m_pTime;
    CSR_MDLFrame* m_pFrame;
} CSR_MDLFrameGroup;

/**
* Model, it's a collection of meshes, each of them represent a frame. The model may be animated, by
* showing each frame, one after the other
*/
typedef struct
{
    CSR_Mesh* m_pMesh;
    size_t    m_MeshCount;
    double    m_Time;
} CSR_Model;

/**
* Model animation (based on frames)
*/
typedef struct
{
    char   m_Name[16];
    size_t m_Start;
    size_t m_End;
} CSR_ModelAnimation;

/**
* Model texture
*/
typedef struct
{
    GLuint m_TextureID;
    double m_Time;
} CSR_ModelTexture;

/**
* Quake I (.mdl) model
*/
typedef struct
{
    CSR_Model*          m_pModel;
    size_t              m_ModelCount;
    CSR_ModelAnimation* m_pAnimation;
    size_t              m_AnimationCount;
    CSR_ModelTexture*   m_pTexture;
    size_t              m_TextureCount;
} CSR_MDL;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Shape functions
        //-------------------------------------------------------------------

        /**
        * Creates a surface
        *@param pVertexFormat - vertex format
        *@param width - surface width
        *@param height - surface height
        *@param color - color in RGBA format
        *@return mesh containing the surface, 0 on error
        *@note The mesh must be released when no longer used, see csrMeshRelease()
        */
        CSR_Mesh* csrShapeCreateSurface(const CSR_VertexFormat* pVertexFormat,
                                              float             width,
                                              float             height,
                                              unsigned          color);

        /**
        * Creates a box
        *@param pVertexFormat - vertex format
        *@param width - box width
        *@param height - box height
        *@param depth - box depth
        *@param color - color in RGBA format
        *@param repeatTexOnEachFace - if 1 the texture will be repeated on each face
        *@return mesh containing the surface, 0 on error
        *@note The mesh must be released when no longer used, see csrMeshRelease()
        */
        CSR_Mesh* csrShapeCreateBox(const CSR_VertexFormat* pVertexFormat,
                                          float             width,
                                          float             height,
                                          float             depth,
                                          unsigned          color,
                                          int               repeatTexOnEachFace);

        /**
        * Creates a sphere
        *@param pVertexFormat - vertex format
        *@param radius - sphere radius
        *@param slices - slices (longitude) count
        *@param stacks - stacks (latitude) count
        *@param color - color in RGBA format
        *@return mesh containing the sphere, 0 on error
        *@note The mesh must be released when no longer used, see csrMeshRelease()
        */
        CSR_Mesh* csrShapeCreateSphere(const CSR_VertexFormat* pVertexFormat,
                                             float             radius,
                                             int               slices,
                                             int               stacks,
                                             unsigned          color);

        /**
        * Creates a cylinder
        *@param pVertexFormat - vertex format to use
        *@param radius - cylinder radius
        *@param height - cylinder height
        *@param faces - number of faces composing the cylinder
        *@param color - color in RGBA format
        *@return mesh containing the cylinder, 0 on error
        *@note The mesh must be released when no longer used, see csrMeshRelease()
        */
        CSR_Mesh* csrShapeCreateCylinder(const CSR_VertexFormat* pVertexFormat,
                                               float             radius,
                                               float             height,
                                               int               faces,
                                               unsigned          color);

        /**
        * Creates a disk
        *@param pVertexFormat - vertex format to use
        *@param centerX - the disk center on the x axis
        *@param centerY - the disk center on the y axis
        *@param radius - disk radius
        *@param sliceCount - disk slice count
        *@param color - disk color
        *@return mesh containing the disk, 0 on error
        *@note The mesh must be released when no longer used, see csrMeshRelease()
        */
        CSR_Mesh* csrShapeCreateDisk(const CSR_VertexFormat* pVertexFormat,
                                           float             centerX,
                                           float             centerY,
                                           float             radius,
                                           unsigned          sliceCount,
                                           unsigned          color);

        /**
        * Creates a ring
        *@param pVertexFormat - vertex format to use
        *@param centerX - the ring center on the x axis
        *@param centerY - the ring center on the y axis
        *@param minRadius - internal radius
        *@param maxRadius - external radius
        *@param sliceCount - slice count
        *@param minColor - internal edge color
        *@param maxColor - external edge color
        *@return mesh containing the ring, 0 on error
        *@note The mesh must be released when no longer used, see csrMeshRelease()
        */
        CSR_Mesh* csrShapeCreateRing(const CSR_VertexFormat* pVertexFormat,
                                           float             centerX,
                                           float             centerY,
                                           float             minRadius,
                                           float             maxRadius,
                                           unsigned          sliceCount,
                                           unsigned          minColor,
                                           unsigned          maxColor);

        /**
        * Creates a spiral
        *@param pVertexFormat - vertex format to use
        *@param centerX - the spiral center on the x axis
        *@param centerY - the spiral center on the y axis
        *@param minRadius - internal radius
        *@param maxRadius - external radius
        *@param deltaMin - delta to apply to the min radius
        *@param deltaMax - delta to apply to the max radius
        *@param deltaZ - delta to apply to the z radius
        *@param stackCount - stack count
        *@param sliceCount - slice count
        *@param minColor - internal edge color
        *@param maxColor - external edge color
        *@return mesh containing the spiral, 0 on error
        *@note The mesh must be released when no longer used, see csrMeshRelease()
        */
        CSR_Mesh* csrShapeCreateSpiral(const CSR_VertexFormat* pVertexFormat,
                                             float             centerX,
                                             float             centerY,
                                             float             minRadius,
                                             float             maxRadius,
                                             float             deltaMin,
                                             float             deltaMax,
                                             float             deltaZ,
                                             unsigned          stackCount,
                                             unsigned          sliceCount,
                                             unsigned          minColor,
                                             unsigned          maxColor);

        //-------------------------------------------------------------------
        // Model functions
        //-------------------------------------------------------------------

        /**
        * Creates a model
        *@return newly created model, 0 on error
        *@note The model must be released when no longer used, see csrModelRelease()
        */
        CSR_Model* csrModelCreate(void);

        /**
        * Releases a model
        *@param pModel - model to release
        */
        void csrModelRelease(CSR_Model* pModel);

        //-------------------------------------------------------------------
        // MDL model functions
        //-------------------------------------------------------------------

        /**
        * Reads MDL header
        *@param pBuffer - buffer containing the MDL data
        *@param[in, out] pOffset - offset to read from, new offset position after function ends
        *@param[out] pHeader - MDL header structure to populate
        *@return 1 on success, otherwise 0
        */
        int csrMDLReadHeader(const CSR_Buffer* pBuffer, size_t* pOffset, CSR_MDLHeader* pHeader);

        /**
        * Reads MDL skin
        *@param pBuffer - buffer containing the MDL data
        *@param[in, out] pOffset - offset to read from, new offset position after function ends
        *@param pHeader - MDL header structure
        *@param[out] pSkin - MDL skin structure to populate
        *@return 1 on success, otherwise 0
        */
        int csrMDLReadSkin(const CSR_Buffer*    pBuffer,
                                 size_t*        pOffset,
                           const CSR_MDLHeader* pHeader,
                                 CSR_MDLSkin*   pSkin);

        /**
        * Reads MDL texture coordinates
        *@param pBuffer - buffer containing the MDL data
        *@param[in, out] pOffset - offset to read from, new offset position after function ends
        *@param[out] pTexCoord - MDL texture coordinates structure to populate
        *@return 1 on success, otherwise 0
        */
        int csrMDLReadTextureCoord(const CSR_Buffer*          pBuffer,
                                         size_t*              pOffset,
                                         CSR_MDLTextureCoord* pTexCoord);

        /**
        * Reads MDL polygon
        *@param pBuffer - buffer containing the MDL data
        *@param[in, out] pOffset - offset to read from, new offset position after function ends
        *@param[out] pPolygon - MDL polygon structure to populate
        *@return 1 on success, otherwise 0
        */
        int csrMDLReadPolygon(const CSR_Buffer* pBuffer, size_t* pOffset, CSR_MDLPolygon* pPolygon);

        /**
        * Reads MDL vertex
        *@param pBuffer - buffer containing the MDL data
        *@param[in, out] pOffset - offset to read from, new offset position after function ends
        *@param[out] pVertex - MDL vertex structure to populate
        *@return 1 on success, otherwise 0
        */
        int csrMDLReadVertex(const CSR_Buffer* pBuffer, size_t* pOffset, CSR_MDLVertex* pVertex);

        /**
        * Reads MDL frame
        *@param pBuffer - buffer containing the MDL data
        *@param[in, out] pOffset - offset to read from, new offset position after function ends
        *@param pHeader - MDL header
        *@param[out] pFrame - MDL frame structure to populate
        *@return 1 on success, otherwise 0
        */
        int csrMDLReadFrame(const CSR_Buffer*    pBuffer,
                                  size_t*        pOffset,
                            const CSR_MDLHeader* pHeader,
                                  CSR_MDLFrame*  pFrame);

        /**
        * Reads MDL frame group
        *@param pBuffer - buffer containing the MDL data
        *@param[in, out] pOffset - offset to read from, new offset position after function ends
        *@param pHeader - MDL header
        *@param[out] pFrameGroup - MDL frame group structure to populate
        *@return 1 on success, otherwise 0
        */
        int csrMDLReadFrameGroup(const CSR_Buffer*        pBuffer,
                                       size_t*            pOffset,
                                 const CSR_MDLHeader*     pHeader,
                                       CSR_MDLFrameGroup* pFrameGroup);

        /**
        * Uncompresses the MDL texture
        *@param pSkin - model skin
        *@param pPalette - palette to use to uncompress the texture, if 0 the default palette will be used
        *@param width - texture width
        *@param height - texture height
        *@param index - texture index
        *@return a pixel buffer containing the texture, 0 on error
        *@note The pixel buffer must be released when no longer used, see csrPixelBufferRelease()
        */
        CSR_PixelBuffer* csrMDLUncompressTexture(const CSR_MDLSkin*   pSkin,
                                                 const CSR_Buffer*    pPalette,
                                                       size_t         width,
                                                       size_t         height,
                                                       size_t         index);

        /**
        * Uncompresses a MDL vertex
        *@param pHeader - MDL header
        *@param pVertex - MDL vertex
        *@param[out] pResult - resulting vector
        */
        void csrMDLUncompressVertex(const CSR_MDLHeader* pHeader,
                                    const CSR_MDLVertex* pVertex,
                                          CSR_Vector3*   pResult);

        /**
        * Populates a model from MDL data
        *@param pHeader - MDL header
        *@param pFrameGroup - MDL frame group
        *@param pPolygon - MDL polygon
        *@param pTexCoord - MDL texture coordinates
        *@param pVertexFormat - vertex format to use
        *@param color - color in RGBA format
        *@param textureID - texture identifier
        *@param[out] pModel - model to populate (a blank model should be propvided)
        */
        void csrMDLPopulateModel(const CSR_MDLHeader*       pHeader,
                                 const CSR_MDLFrameGroup*   pFrameGroup,
                                 const CSR_MDLPolygon*      pPolygon,
                                 const CSR_MDLTextureCoord* pTexCoord,
                                 const CSR_VertexFormat*    pVertexFormat,
                                       unsigned             color,
                                       CSR_Model*           pModel);

        /**
        * Creates a MDL model from a buffer
        *@param pBuffer - buffer containing the MDL data to read
        *@param pPalette - palette to use to generate the model texture, if 0 a default palette will be used
        *@param pVertexFormat - vertex format to use
        *@param color - color in RGBA format
        *@return the newly created MDL model, 0 on error
        *@note The MDL model must be released when no longer used, see csrMDLModelRelease()
        */
        CSR_MDL* csrMDLCreate(const CSR_Buffer*       pBuffer,
                              const CSR_Buffer*       pPalette,
                                    CSR_VertexFormat* pVertexFormat,
                                    unsigned          color);

        /**
        * Opens a MDL model from a file
        *@param pFileName - MDL model file name
        *@param pPalette - palette to use to generate the model texture, if 0 a default palette will be used
        *@param pVertexFormat - vertex format to use
        *@param color - color in RGBA format
        *@return the newly created MDL model, 0 on error
        *@note The MDL model must be released when no longer used, see csrMDLModelRelease()
        */
        CSR_MDL* csrMDLOpen(const char*             pFileName,
                            const CSR_Buffer*       pPalette,
                                  CSR_VertexFormat* pVertexFormat,
                                  unsigned          color);

        /**
        * Releases the MDL objects used during the opening
        *@param pHeader - MDL header
        *@param pFrameGroup - MDL frame group
        *@param pSkin - MDL skin
        *@param pTexCoord - MDL texture coordinates
        *@param pPolygon - MDL polygon
        */
        void csrMDLReleaseObjects(CSR_MDLHeader*       pHeader,
                                  CSR_MDLFrameGroup*   pFrameGroup,
                                  CSR_MDLSkin*         pSkin,
                                  CSR_MDLTextureCoord* pTexCoord,
                                  CSR_MDLPolygon*      pPolygon);

        /**
        * Updates the MDL model indexes (e.g. before getting the next mesh to show)
        *@param pMDL - MDL model
        *@param fps - frame per seconds to apply
        *@aram animationIndex - animation index
        *@param[in, out] pTextureIndex - texture index, new texture index on function ends
        *@param[in, out] pModelIndex - model index, new model index on function ends
        *@param[in, out] pMeshIndex - mesh index, new mesh index on function ends
        *@param[in, out] pTextureLastTime - texture last known time
        *@param[in, out] pModelLastTime - model last known time
        *@param[in, out] pMeshLastTime - mesh last known time
        *@param elapsedTime - elapsed time since last calculation
        */
        void csrMDLUpdateIndex(const CSR_MDL* pMDL,
                                     size_t   fps,
                                     size_t   animationIndex,
                                     size_t*  pTextureIndex,
                                     size_t*  pModelIndex,
                                     size_t*  pMeshIndex,
                                     double*  pTextureLastTime,
                                     double*  pModelLastTime,
                                     double*  pMeshLastTime,
                                     double   elapsedTime);

        /**
        * Get the current mesh from a MDL model (e.g. to draw it)
        *@param pMDL - MDL model to get from
        *@param modelIndex - model index, 0 if unknown (csrMDLUpdateIndex() may be called first)
        *@param meshIndex - mesh index, 0 if unknown (csrMDLUpdateIndex() may be called first)
        *@return current mesh from MDL model, 0 on error or if not found
        *@note The returned mesh will be valid as long as his owner model is
        */
        CSR_Mesh* csrMDLGetMesh(const CSR_MDL* pMDL, size_t modelIndex, size_t meshIndex);

        /**
        * Releases a MDL model
        *@param pMDL - MDL model to release
        */
        void csrMDLRelease(CSR_MDL* pMDL);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Model.c"
#endif

#endif
