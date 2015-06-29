/******************************************************************************
 * ==> QR_Registry -----------------------------------------------------------*
 ******************************************************************************
 * Description : Tool functions to manipulate the Windows registry            *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_RegistryH
#define QR_RegistryH

// qr engine
#include "QR_Types.h"

// libraries
#include <Windows.h>

/**
* Tool functions to manipulate the Windows registry
*@author Jean-Milost Reymond
*/
class QR_Registry
{
    public:
        /**
        * Registry type enumeration
        */
        enum IRegistryType
        {
            IE_RT_Default = 0,
            IE_RT_32Bit,
            IE_RT_64Bit,
            IE_RT_Both,
        };

        /**
        * Writes key in Windows registry
        *@param hRootKey - main root key to write to
        *@param pSubKey - sub-key from main root key to write to
        *@param pName - key name
        *@param dwType - key type to write
        *@param pvData - key data
        *@param dwDataSize - key data size
        *@return true on success, otherwise false
        */
        static bool WriteKey(HKEY           hRootKey,
                             const QR_Char* pSubKey,
                             LPCTSTR        pName,
                             DWORD          dwType,
                             void*          pvData,
                             DWORD          dwDataSize);

        /**
        * Deletes key from Windows registry
        *@param hRootKey - main root key to write to
        *@param pSubKey - sub-key from main root key to write to
        *@param registryType - registry type
        *@return true on success, otherwise false
        *@note A deleted key is not removed until the last handle to it is closed
        *@note The subkey to be deleted must not have subkeys. To delete a key and all its subkeys,
        *      you need to enumerate the subkeys and delete them individually
        */
        static bool DeleteKey(HKEY hRootKey, const QR_Char* pSubKey, IRegistryType registryType);
};
#endif
