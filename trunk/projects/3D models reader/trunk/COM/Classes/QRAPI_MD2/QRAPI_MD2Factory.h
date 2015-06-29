/******************************************************************************
 * ==> QRAPI_MD2Factory ------------------------------------------------------*
 ******************************************************************************
 * Description : MD2 model COM object factory                                 *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QRAPI_MD2FactoryH
#define QRAPI_MD2FactoryH

// qr engine
#include "QR_MD2.h"

// interface
#include "QRAPI_IMD2.h"

// {43109B91-37DC-4DE2-A348-7B4EF9AD9692}
static const CLSID CLSID_QRAPI_MD2Factory =
        {0x43109b91, 0x37dc, 0x4de2, {0xa3, 0x48, 0x7b, 0x4e, 0xf9, 0xad, 0x96, 0x92}};

/**
* MD2 models COM object factory
*@author Jean-Milost Reymond
*/
class QRAPI_MD2Factory : public TInterfacedObject, QRAPI_IMD2
{
    public:
        __fastcall QRAPI_MD2Factory();
        __fastcall ~QRAPI_MD2Factory();

        /**
        * Loads MD2 mesh from file
        *@param pFileName - file name
        *@param pVersion - file version, can be NULL (default version will be used in this case)
        *@return success or error message        */        virtual QRAPI_EResult __stdcall LoadMesh(const QRAPI_Char*  pFileName,
                                                 const QRAPI_Float* pVersion);

        /**
        * Loads normals table from file
        *@param pFileName - file name
        *@param pVersion - file version
        *@return success or error message        */        virtual QRAPI_EResult __stdcall LoadNormalsTable(const QRAPI_Char*  pFileName,
                                                         const QRAPI_Float* pVersion);

        /**
        * Set animation
        *@param start - animation start frame index
        *@param end - animation end frame index
        *@param pName - animation name
        *@return success or error message
        */
        virtual QRAPI_EResult __stdcall SetAnimation(QRAPI_SizeT       start,
                                                     QRAPI_SizeT       end,
                                                     const QRAPI_Char* pName);

        /**
        * Animate mesh
        *@param pElapsedTime - elapsed time since last frame was drawn
        *@param fps - number of frames per seconds
        *@return success or error message
        */
        virtual QRAPI_EResult __stdcall Animate(const QRAPI_Double* pElapsedTime,
                                                QRAPI_SizeT         fps);

        /**
        * Gets vertex buffer
        *@param hVBs - vertex buffer container handle to populate
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
                                                        bool                     convertRHLH) const;

        /**
        * Draws MD2 model using OpenGL (in direct mode)
        *@param vertexFormat - vertex format (i.e what data the vertex contains)
        *@param convertRHLH - if true, left hand system will be converted to right hand or vice-versa
        *@return success or error message
        */
        virtual QRAPI_EResult __stdcall Draw_To_OpenGL_1(QRAPI_EVertexFormat vertexFormat,
                                                         bool                convertRHLH) const;

        /**
        * Query interface
        *@param IID - interface identifier
        *@param[out] pObj - object matching with interface
        *@return error or success message
        */
        virtual HRESULT __stdcall QueryInterface(const GUID& IID, void** pObj);

        /**
        * Add interface reference
        *@return new reference count
        */
        virtual ULONG __stdcall AddRef();

        /**
        * Release interface
        *@return new reference count
        */
        virtual ULONG __stdcall Release();

    private:
        QR_MD2 m_MD2;
};
#endif
