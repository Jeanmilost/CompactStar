/******************************************************************************
 * ==> QR_Types --------------------------------------------------------------*
 ******************************************************************************
 * Description : QR engine basic types                                        *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_TypesH
#define QR_TypesH

// std
#include <cstddef>
#include <stdint.h>
#include <vector>

//------------------------------------------------------------------------------
// Global macros
//------------------------------------------------------------------------------
#define M_Precision QR_Float // real numbers precision, can be e.g. float or double
#define M_Epsilon   1.0E-3   // epsilon value used for tolerance
//------------------------------------------------------------------------------

// used cross-platform types
typedef bool        QR_Bool;
typedef float       QR_Float;
typedef double      QR_Double;
typedef std::size_t QR_SizeT;

// c++98/c++11 dependent types
#ifdef QRENGINE_USE_CPP11
    // c++11 version
    typedef std::int8_t   QR_Int8;
    typedef std::uint8_t  QR_UInt8;
    typedef std::int16_t  QR_Int16;
    typedef std::uint16_t QR_UInt16;
    typedef std::int32_t  QR_Int32;
    typedef std::uint32_t QR_UInt32;
    typedef std::int64_t  QR_Int64;
    typedef std::uint64_t QR_UInt64;
#else
    // c++98 version
    typedef char               QR_Int8;
    typedef unsigned char      QR_UInt8;
    typedef short              QR_Int16;
    typedef unsigned short     QR_UInt16;
    typedef int                QR_Int32;
    typedef unsigned           QR_UInt32;
    typedef long long          QR_Int64;
    typedef unsigned long long QR_UInt64;
#endif

// used cross-platform types for texts
typedef char    QR_Char;
typedef wchar_t QR_WChar;

// used cross-platform types for buffers
typedef std::size_t QR_BufferSizeType;
typedef QR_UInt8    QR_BufferDataType;

// used cross-platform types for GUID (when based on pointer system)
typedef uintptr_t                QR_GUIDType;
typedef std::vector<QR_GUIDType> QR_GUIDList;

#endif // QR_TypesH
