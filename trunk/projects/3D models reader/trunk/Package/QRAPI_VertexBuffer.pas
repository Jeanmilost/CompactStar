{******************************************************************************
 * ==> QRAPI_VertexBuffer ----------------------------------------------------*
 ******************************************************************************
 * Description : QR_Engine API vertex buffer functions                        *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************}

unit QRAPI_VertexBuffer;

interface

uses
    System.Generics.Collections, QRAPI_Common;

type
    {**
    * Vertex format enumeration
    *@note Flags can be combined
    }
    TVertexFormat =
    (
        tvfNone      = $00,
        tvfNormals   = $01,
        tvfTexCoords = $02,
        tvfColors    = $04
    );

    {**
    * Vertex buffer type enumeration
    *}
    TVertexType =
    (
        tvtUnknown = 0,
        tvtTriangles,
        tvtTriangleStrip,
        tvtTriangleFan,
        tvtQuads,
        tvtQuadStrip
    );
    PVertexType = ^TVertexType;

    TVertexData = TList<TQRAPIPrecision>;

    {**
    * Vertex buffer for the DLL import/export
    *@author Jean-Milost Reymond
    *}
    TVertexBuffer = record
        m_Data: TVertexData;
        m_Type: TVertexType;
    end;

    TVertexBufferHandle    = THandle;
    TVertexBufferContainer = TList<TVertexBuffer>;

    {**
    * Vertex buffer container handle, that can contain list of vertex buffers
    *@note When accessing an STL object created in one DLL or EXE through a pointer or reference in
    *      a different DLL or EXE, you may experience an access violation or other serious program
    *      errors including the appearance of data corruption or data loss. Most classes in the
    *      Standard C++ Libraries use static data members directly or indirectly. Since these classes
    *      are generated through template instantiation, each executable image (usually with DLL or
    *      EXE file name extensions) will contain its own copy of the static data member for a given
    *      class. When a method of the class that requires the static data member is executed, it
    *      uses the static data member in the executable image in which the method code resides.
    *      Since the static data members in the executable images are not in sync, this action could
    *      result in an access violation or data may appear to be lost or corrupted.
    *}
    TVertexBufferContainerHandle = ^TVertexBufferContainer;

    {**
    * Creates vertex buffer container instance
    *@returns vertex buffer container handle, NULL on error
    *}
    function Create_VertexBufferContainer: TVertexBufferContainerHandle; stdcall;

    {**
    * Deletes vertex buffer container instance
    *@param handle - vertex buffer container handle to delete
    *@return success or error message
    *}
    function Delete_VertexBufferContainer(handle: TVertexBufferContainerHandle): TQRAPIResult; stdcall;

    {**
    * Gets vertex buffer count
    *@param handle - vertex buffer container handle to count
    *@param pResult - resulting value
    *@return success or error message
    *}
    function Get_VertexBuffer_Count(handle:  TVertexBufferContainerHandle;
                                    pResult: PCardinal): TQRAPIResult; stdcall;

    {**
    * Gets vertex buffer
    *@param handle - vertex buffer container handle to extract from
    *@param index - vertex buffer index
    *@return vertex buffer, NULL on error or if not found
    *}
    function Get_VertexBuffer(handle: TVertexBufferContainerHandle;
                              index:  Cardinal): TQRAPIResult; stdcall;

    {**
    * Deletes vertex buffer
    *@param handle - vertex buffer container handle to extract from
    *@return success or error message
    *}
    function Delete_VertexBuffer(handle: TVertexBufferHandle): TQRAPIResult; stdcall;

    {**
    * Gets vertex buffer type
    *@param handle - vertex buffer handle to get from
    *@param pResult - resulting value
    *@return success or error message
    *}
    function Get_VertexBuffer_Type(handle:  TVertexBufferHandle;
                                   pResult: PVertexType): TQRAPIResult; stdcall;

    {**
    * Gets vertex buffer size
    *@param handle - vertex buffer handle to get from
    *@param pResult - resulting value
    *@return success or error message
    *}
    function Get_VertexBuffer_Size(handle:  TVertexBufferHandle;
                                   pResult: PCardinal): TQRAPIResult; stdcall;

    {**
    * Gets vertex buffer data
    *@param handle - vertex buffer handle to get from
    *@param pData - vertex buffer data
    *@return success or error message
    *@note pData should be initialized first, and should contain as much space as the value returned
    *      by Get_VertexBuffer_Size()
    *}
    function Get_VertexBuffer_Data(handle: TVertexBufferHandle;
                                   pData:  Pointer): TQRAPIResult; stdcall;

implementation
    function Create_VertexBufferContainer; external dllName;
    function Delete_VertexBufferContainer; external dllName;
    function Get_VertexBuffer_Count;       external dllName;
    function Get_VertexBuffer;             external dllName;
    function Delete_VertexBuffer;          external dllName;
    function Get_VertexBuffer_Type;        external dllName;
    function Get_VertexBuffer_Size;        external dllName;
    function Get_VertexBuffer_Data;        external dllName;
end.
