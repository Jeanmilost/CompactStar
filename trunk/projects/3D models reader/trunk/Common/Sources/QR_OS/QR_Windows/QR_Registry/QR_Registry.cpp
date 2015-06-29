/******************************************************************************
 * ==> QR_Registry -----------------------------------------------------------*
 ******************************************************************************
 * Description : Tool functions to manipulate the Windows registry            *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QR_Registry.h"

// qr engine
#include "QR_Exception.h"

//---------------------------------------------------------------------------
// QR_Registry - c++ Windows dependent
//---------------------------------------------------------------------------
bool QR_Registry::WriteKey(HKEY           hRootKey,
                           const QR_Char* pSubKey,
                           LPCTSTR        pName,
                           DWORD          dwType,
                           void*          pvData,
                           DWORD          dwDataSize)
{
    HKEY hk;

    // open root key, create complete path if needed
    if (::RegCreateKey(hRootKey, pSubKey, &hk) != ERROR_SUCCESS)
        return false;

    // write key and data
    bool result = (::RegSetValueEx(hk, pName, 0, dwType,(CONST BYTE*)pvData, dwDataSize) == ERROR_SUCCESS);

    // close opened key
    if (::RegCloseKey(hk) != ERROR_SUCCESS)
        return false;

    return result;
}
//---------------------------------------------------------------------------
bool QR_Registry::DeleteKey(HKEY hRootKey, const QR_Char* pSubKey, IRegistryType registryType)
{
    // search for registry type to delete from
    switch (registryType)
    {
        case IE_RT_Default:
        case IE_RT_32Bit:
            // delete key from 32 bit register
            return (::RegDeleteKeyEx(hRootKey, pSubKey, KEY_WOW64_32KEY, 0) == ERROR_SUCCESS);

        case IE_RT_64Bit:
            // delete key from 64 bit register
            return (::RegDeleteKeyEx(hRootKey, pSubKey, KEY_WOW64_64KEY, 0) == ERROR_SUCCESS);

        case IE_RT_Both:
        {
            bool success = true;

            // delete key from both 32 and 64 bit registers
            success &= (::RegDeleteKeyEx(hRootKey, pSubKey, KEY_WOW64_32KEY, 0) == ERROR_SUCCESS);
            success &= (::RegDeleteKeyEx(hRootKey, pSubKey, KEY_WOW64_64KEY, 0) == ERROR_SUCCESS);

            return success;
        }

        default:
            M_THROW_EXCEPTION("Unknown registry type");
    }
}
//---------------------------------------------------------------------------

