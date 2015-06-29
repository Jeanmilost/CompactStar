{******************************************************************************
 * ==> QRAPI_MD2 -------------------------------------------------------------*
 ******************************************************************************
 * Description : DLL interface for MD2 model                                  *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************}

unit QRAPI_MD2;

interface

uses
    QRAPI_Common, QRAPI_VertexBuffer;

type
    // MD2 model handler
    TMD2Handle = THandle;

    {**
    * Creates MD2 model instance
    *@return MD2 model handle, NULL on error
    *}
    function Create_MD2: TMD2Handle; stdcall;

    {**
    * Deletes MD2 model instance
    *@param handle - MD2 model handle to delete
    *@return success or error message
    *}
    function Delete_MD2(handle: TMD2Handle): TQRAPIResult; stdcall;

    {**
    * Loads MD2 mesh from file
    *@param handle - MD2 model handle
    *@param pFileName - file name
    *@param pVersion - file version, can be NULL (default version will be used in this case)
    *@return success or error message    *}    function LoadMesh_MD2(handle:          TMD2Handle;
                          const pFileName: PChar;
                          const pVersion:  PSingle): TQRAPIResult; stdcall;

    {**
    * Loads normals table from file
    *@param handle - MD2 model handle
    *@param pFileName - file name
    *@param pVersion - file version
    *@return success or error message    *}    function LoadNormalsTable_MD2(handle:          TMD2Handle;
                                  const pFileName: PChar;
                                  const pVersion:  PSingle): TQRAPIResult; stdcall;

    {**
    * Set animation
    *@param handle - MD2 model handle
    *@param start - animation start frame index
    *@param end - animation end frame index
    *@param pName - animation name
    *@return success or error message
    *}
    function SetAnimation_MD2(handle:               TMD2Handle;
                              startFrame, endFrame: Cardinal;
                              pName:                PChar): TQRAPIResult; stdcall;

    {**
    * Animate mesh
    *@param handle - MD2 model handle
    *@param pElapsedTime - elapsed time since last frame was drawn
    *@param fps - number of frames per seconds
    *@return success or error message
    *}
    function Animate_MD2(handle: TMD2Handle; pElapsedTime: PExtended; fps: Cardinal): TQRAPIResult; stdcall;

    {**
    * Gets vertex buffer
    *@param handle - MD2 model handle
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
    *}
    function GetVertexBuffer_MD2(handle:       TMD2Handle;
                                 hVBs:         TVertexBufferContainerHandle;
                                 vertexFormat: TVertexFormat;
                                 convertRHLH:  Boolean): TQRAPIResult; stdcall;

    {**
    * Draws MD2 model using OpenGL (in direct mode)
    *@param handle - MD2 model handle
    *@param vertexFormat - vertex format (i.e what data the vertex contains)
    *@param convertRHLH - if true, left hand system will be converted to right hand or vice-versa
    *@return success or error message
    *}
    function Draw_MD2_To_OpenGL_1(handle:       TMD2Handle;
                                  vertexFormat: TVertexFormat;
                                  convertRHLH:  Boolean): TQRAPIResult; stdcall;

implementation
    function Create_MD2;           external dllName;
    function Delete_MD2;           external dllName;
    function LoadMesh_MD2;         external dllName;
    function LoadNormalsTable_MD2; external dllName;
    function SetAnimation_MD2;     external dllName;
    function Animate_MD2;          external dllName;
    function GetVertexBuffer_MD2;  external dllName;
    function Draw_MD2_To_OpenGL_1; external dllName;
end.
