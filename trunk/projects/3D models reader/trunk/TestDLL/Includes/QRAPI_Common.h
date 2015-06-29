/******************************************************************************
 * ==> QRAPI_Common ----------------------------------------------------------*
 ******************************************************************************
 * Description : QR_Engine API common classes and declarations                *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QRAPI_CommonH
#define QRAPI_CommonH

// std
#include <stdint.h>
#include <vector>

// qr api
#include "QRAPI.h"

//------------------------------------------------------------------------------
// Global defines
//------------------------------------------------------------------------------
#define M_QRAPI_Precision QRAPI_Float // real numbers precision, can be e.g. float or double
//------------------------------------------------------------------------------

/**
* Common API functions
*@author Jean-Milost Reymond
*/
#ifdef __cplusplus
    extern "C"
    {
#endif
        // used cross-platform types
        typedef bool          QRAPI_Bool;
        typedef std::int8_t   QRAPI_Int8;
        typedef std::uint8_t  QRAPI_UInt8;
        typedef std::int16_t  QRAPI_Int16;
        typedef std::uint16_t QRAPI_UInt16;
        typedef std::int32_t  QRAPI_Int32;
        typedef std::uint32_t QRAPI_UInt32;
        typedef std::int64_t  QRAPI_Int64;
        typedef std::uint64_t QRAPI_UInt64;
        typedef float         QRAPI_Float;
        typedef double        QRAPI_Double;
        typedef std::size_t   QRAPI_SizeT;

        // used cross-platform types for texts
        typedef char    QRAPI_Char;
        typedef wchar_t QRAPI_WChar;

        // used cross-platform types for buffers
        typedef std::size_t QRAPI_BufferSizeType;
        typedef QRAPI_UInt8 QRAPI_BufferDataType;

        // used cross-platform types for GUID (when based on pointer system)
        typedef uintptr_t                   QRAPI_GUIDType;
        typedef std::vector<QRAPI_GUIDType> QRAPI_GUIDList;

        /**
        * Global API function result enumeration
        *@author Jean-Milost Reymond
        */
        enum QRAPI_EResult
        {
            QRAPI_R_Success               =  0,
            QRAPI_R_OK                    =  0,
            QRAPI_R_None                  =  0,
            QRAPI_R_Internal_Exception    = -1,
            QRAPI_R_Internal_Call_Failed  = -2,
            QRAPI_R_Param_Not_Initialized = -3,
            QRAPI_R_Get_Vertex_Buffer     = -4,
        };

        typedef M_QRAPI_Precision QRAPI_VertexDataType;
#ifdef __cplusplus
    }
#endif

#endif
