/******************************************************************************
 * ==> 3DModelsReaderEngine_PCH ----------------------------------------------*
 ******************************************************************************
 * Description : 3D models reader engine pre-compiled header                  *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

 #ifdef _WIN32
    #include <tchar.h>

    // in RAD studio XE7 and higher, some c++11 functions are available
    #define QRENGINE_USE_CPP11

    // set QR Engine API to export DLL mode
    #define QR_ENGINE_API_EXPORT
#endif

