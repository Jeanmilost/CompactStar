/******************************************************************************
 * ==> QRAPI -----------------------------------------------------------------*
 ******************************************************************************
 * Description : Basic QR_Engine API declarations                             *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QRAPIH
#define QRAPIH

// std
#include <_null.h>

//------------------------------------------------------------------------------
// API declaration
//------------------------------------------------------------------------------
#ifdef _WIN32
    // Win32 DLL import/export mode
    #if defined(QR_ENGINE_API_EXPORT)
        // inside DLL
        #define QR_ENGINE_API __declspec(dllexport)
    #else
        // outside DLL
        #define QR_ENGINE_API __declspec(dllimport)
    #endif
#endif
//------------------------------------------------------------------------------

#endif
