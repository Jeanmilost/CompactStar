/*****************************************************************************
 * ==> Main -----------------------------------------------------------------*
 *****************************************************************************
 * Description : Application main entry point                                *
 * Developer   : Jean-Milost Reymond                                         *
 *****************************************************************************/

// qr engine
#include "QR_Exception.h"
#include "QR_Application.h"

//------------------------------------------------------------------------------
// WinMain
//------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
        int iCmdShow)
{
    std::string appTitle = "Keline";

    try
    {
        return QR_Application::Execute(hInstance, appTitle);
    }
    catch (QR_Exception& e)
    {
        ::MessageBox(NULL, e.Format(), appTitle.c_str(), MB_OK | MB_ICONSTOP);
        return EXIT_FAILURE;
    }
    catch (std::exception& e)
    {
        ::MessageBox(NULL, e.what(), appTitle.c_str(), MB_OK | MB_ICONSTOP);
        return EXIT_FAILURE;
    }
    catch (...)
    {
        ::MessageBox(NULL, "Unknown exception", appTitle.c_str(), MB_OK | MB_ICONSTOP);
        return EXIT_FAILURE;
    }
}
//------------------------------------------------------------------------------
