/******************************************************************************
 * ==> QR_MD2Common ----------------------------------------------------------*
 ******************************************************************************
 * Description : Common classes, enums and values for MD2 model               *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_MD2CommonH
#define QR_MD2CommonH

//------------------------------------------------------------------------------
// Global defines
//------------------------------------------------------------------------------
#define M_MD2_Invalid_Index (QR_SizeT)-1
//------------------------------------------------------------------------------

/**
* Common classes, enums and values for MD2 model
*@author Jean-Milost Reymond
*/
class QR_MD2Common
{
    public:
        /**
        * Error, success and warning codes
        */
        enum IECode
        {
            IE_C_Not3DCoords          = -8,
            IE_C_IncompatibleFrames   = -7,
            IE_C_IncompatibleVertices = -6,
            IE_C_VertexNotFound       = -5,
            IE_C_FrameNotFound        = -4,
            IE_C_NormalsTableEmpty    = -3,
            IE_C_IndexOutOfBounds     = -2,
            IE_C_InvalidFileVersion   = -1,
            IE_C_Success              =  0,
            IE_C_EmptyBuffer          =  1,
        };
};

#endif // QR_MD2CommonH
