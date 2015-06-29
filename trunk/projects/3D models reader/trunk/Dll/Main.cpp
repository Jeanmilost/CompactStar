/******************************************************************************
 * ==> Main ------------------------------------------------------------------*
 ******************************************************************************
 * Description : DLL main entry point                                         *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#pragma hdrstop
#pragma argsused

// qr engine
#ifdef _DEBUG
    #include "QR_Types.h"
#endif

// interface
#include "QRAPI_MD2.h"

//------------------------------------------------------------------------------
// DLL main entry point
//------------------------------------------------------------------------------
extern "C" int _libmain(unsigned long reason)
{
    #ifdef _DEBUG
        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn -8008
            #pragma warn -8066
        #endif

        if (sizeof(M_Precision) != sizeof(M_QRAPI_Precision))
            throw "Precision used in QR Engine did not match with precision used in API";

        #if defined(__BORLANDC__) || defined(__CODEGEARC__)
            #pragma warn .8066
            #pragma warn .8008
        #endif
    #endif

    return 1;
}
//------------------------------------------------------------------------------

