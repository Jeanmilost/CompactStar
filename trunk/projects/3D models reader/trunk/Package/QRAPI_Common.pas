{******************************************************************************
 * ==> QRAPI_Common ----------------------------------------------------------*
 ******************************************************************************
 * Description : QR_Engine API common classes and declarations                *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************}

unit QRAPI_Common;

interface

const
    dllName = '3DModelsReaderEngine.dll';

type
    TQRAPIPrecision = Single;

    {**
    * Global API function result enumeration
    *@author Jean-Milost Reymond
    *}
    TQRAPIResult =
    (
        qrSuccess             =  0,
        qrOK                  =  0,
        qrNone                =  0,
        qrInternalException   = -1,
        qrInternalCallFailed  = -2,
        qrParamNotInitialized = -3,
        qrGetVertexBuffer     = -4
    );

implementation

end.
