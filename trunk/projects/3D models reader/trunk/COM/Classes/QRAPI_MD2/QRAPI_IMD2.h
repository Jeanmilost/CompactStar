/******************************************************************************
 * ==> QRAPI_IMD2 ------------------------------------------------------------*
 ******************************************************************************
 * Description : MD2 model COM interface                                      *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QRAPI_IMD2H
#define QRAPI_IMD2H

// embarcadero
#include <System.hpp>

// interface
#include "QRAPI_Common.h"
#include "QRAPI_IVertexBuffer.h"

// {286E2EBA-3969-42F8-8950-748DF09DF95A}
static const IID IID_QRAPI_IMD2 =
        {0x286e2eba, 0x3969, 0x42f8, {0x89, 0x50, 0x74, 0x8d, 0xf0, 0x9d, 0xf9, 0x5a}};

/**
* COM interface for MD2 models
*@author Jean-Milost Reymond
*/
struct __declspec(uuid("{286E2EBA-3969-42F8-8950-748DF09DF95A}")) QRAPI_IMD2 : public System::IInterface
{
    /**
    * Loads MD2 mesh from file
    *@param pFileName - file name
    *@param pVersion - file version, can be NULL (default version will be used in this case)
    *@return success or error message    */    virtual QRAPI_EResult __stdcall LoadMesh(const QRAPI_Char*  pFileName,
                                             const QRAPI_Float* pVersion) = 0;

    /**
    * Loads normals table from file
    *@param pFileName - file name
    *@param pVersion - file version
    *@return success or error message    */    virtual QRAPI_EResult __stdcall LoadNormalsTable(const QRAPI_Char*  pFileName,
                                                     const QRAPI_Float* pVersion) = 0;

    /**
    * Set animation
    *@param start - animation start frame index
    *@param end - animation end frame index
    *@param pName - animation name
    *@return success or error message
    */
    virtual QRAPI_EResult __stdcall SetAnimation(QRAPI_SizeT       start,
                                                 QRAPI_SizeT       end,
                                                 const QRAPI_Char* pName) = 0;

    /**
    * Animate mesh
    *@param pElapsedTime - elapsed time since last frame was drawn
    *@param fps - number of frames per seconds
    *@return success or error message
    */
    virtual QRAPI_EResult __stdcall Animate(const QRAPI_Double* pElapsedTime,
                                            QRAPI_SizeT         fps) = 0;

    /**
    * Gets vertex buffer
    *@param pVBs - vertex buffer list to populate
    *@param vertexFormat - vertex format (i.e what data the vertex contains)
    *@param convertRHLH - if true, left hand system will be converted to right hand or vice-versa
    *@return success or error message
    *@note vertex buffer content is organized as follow:
    *      [1]x [2]y [3]z [4]nx [5]ny [6]nz [7]tu [8]tv [9]r [10]g [11]b [12]a
    *      where:
    *      x/y/z    - vertex coordinates
    *      nx/ny/nz - vertex normal (if includeNormal is activated)
    *      tu/tv    - vertex texture coordinates(if includeTexture is activated)
    *      r/g/b/a  - vertex color(if includeColor is activated)
    *@note Vertex buffers created internally in container must be deleted when useless. WARNING,
    *      the container can contain data to delete even if function failed
    */
    virtual QRAPI_EResult __stdcall GetVertexBuffer(_di_QRAPI_IVertexBuffers pVBs,
                                                    QRAPI_EVertexFormat      vertexFormat,
                                                    bool                     convertRHLH) const = 0;

    /**
    * Draws MD2 model using OpenGL (in direct mode)
    *@param vertexFormat - vertex format (i.e what data the vertex contains)
    *@param convertRHLH - if true, left hand system will be converted to right hand or vice-versa
    *@return success or error message
    */
    virtual QRAPI_EResult __stdcall Draw_To_OpenGL_1(QRAPI_EVertexFormat vertexFormat,
                                                     bool                convertRHLH) const = 0;
};

/**
* MD2 model Delphi interface
*/
typedef System::DelphiInterface<QRAPI_IMD2> _di_QRAPI_IMD2;

/**
* Creates MD2 instance
*@return MD2 instance
*/
EXTERN_C _di_QRAPI_IMD2 QR_ENGINE_API QRAPI_CreateMD2();
#endif
