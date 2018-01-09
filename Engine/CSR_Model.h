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
;

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
    #include "CSR_Model.c"
#endif

#endif
