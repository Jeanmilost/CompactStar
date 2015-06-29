/******************************************************************************
 * ==> Main ------------------------------------------------------------------*
 ******************************************************************************
 * Description : DLL main entry point                                         *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#pragma hdrstop
#pragma argsused

// embarcadero
#include <ComServ.hpp>

// qr engine
#ifdef _DEBUG
    #include "QR_Types.h"
#endif
#include "QR_Registry.h"

// interface
#include "QRAPI.h"
#include "QRAPI_Common.h"
#include "QRAPI_IVertexBuffer.h"
#include "QRAPI_VertexBufferFactory.h"
#include "QRAPI_IMD2.h"
#include "QRAPI_MD2Factory.h"

//---------------------------------------------------------------------------
// Structs
//---------------------------------------------------------------------------
/**
* Structure used to register/unregister key/value pair inside Windows registry
*@author Jean-Milost Reymond
*/
struct QRAPI_Key
{
    CLSID       m_ClassID;
    std::string m_Name;
    std::string m_Description;

    /**
    * Constructor
    *@param classID - class identifier
    *@param name - class name
    *@param description - class description
    */
    QRAPI_Key(const CLSID& classID, const std::string& name, const std::string& description) :
        m_ClassID(classID),
        m_Name(name),
        m_Description(description)
    {}
};
//---------------------------------------------------------------------------
// Typedefs
//---------------------------------------------------------------------------
typedef std::vector<QRAPI_Key> QRAPI_Keys;
//---------------------------------------------------------------------------
// Global variables
//---------------------------------------------------------------------------
HMODULE g_hModule = NULL;
//---------------------------------------------------------------------------
// Create functions implementation (needed here because all classes implementation are needed)
//---------------------------------------------------------------------------
_di_QRAPI_IVertexBuffer QR_ENGINE_API QRAPI_CreateVertexBuffer()
{
    return reinterpret_cast<QRAPI_IVertexBuffer*>(new QRAPI_VertexBufferFactory());
}
//---------------------------------------------------------------------------
_di_QRAPI_IVertexBuffers QR_ENGINE_API QRAPI_CreateVertexBuffers()
{
    return (QRAPI_IVertexBuffers*)(new QRAPI_VertexBuffersFactory());
}
//---------------------------------------------------------------------------
_di_QRAPI_IMD2 QR_ENGINE_API QRAPI_CreateMD2()
{
    return (QRAPI_IMD2*)(new QRAPI_MD2Factory());
}
//------------------------------------------------------------------------------
// Common functions
//------------------------------------------------------------------------------
void PopulateRegistryKeys(QRAPI_Keys& keys)
{
    keys.push_back(QRAPI_Key(CLSID_QRAPI_VertexBufferFactory,  "QRAPI.VertexBufferFactory",  "Vertex buffer COM object factory"));
    keys.push_back(QRAPI_Key(CLSID_QRAPI_VertexBuffersFactory, "QRAPI.VertexBuffersFactory", "Vertex buffer list COM object factory"));
    keys.push_back(QRAPI_Key(CLSID_QRAPI_MD2Factory,           "QRAPI.MD2Factory",           "MD2 model COM object factory"));
}
//------------------------------------------------------------------------------
// DLL main entry points
//------------------------------------------------------------------------------
/**
* Server entry point, invoked by Windows when processes or threads are initialized or terminated
*@author Jean-Milost Reymond
*/
int APIENTRY DllMain(HANDLE hModule, DWORD dwReason, void* lpReserved)
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

    if (dwReason == DLL_PROCESS_ATTACH)
        g_hModule = (HMODULE)hModule;

    return 1;
}
//------------------------------------------------------------------------------
/**
* Server entry point invoked to inquire whether the DLL is no longer in use and should be unloaded
*@author Jean-Milost Reymond
*/
EXTERN_C HRESULT QR_ENGINE_API __stdcall DllCanUnloadNow()
{
    if (!g_nComObjsInUse)
        return S_OK;

    return S_FALSE;
}
//------------------------------------------------------------------------------
/**
* Server entry point allowing OLE to retrieve a class object
*@author Jean-Milost Reymond
*/
EXTERN_C HRESULT QR_ENGINE_API __stdcall DllGetClassObject(const CLSID& clsid, const IID& iid,
        void** ppv)
{
    // search for requested object
    if (clsid == CLSID_QRAPI_VertexBufferFactory)
    {
        // create new vertex buffer oject
        QRAPI_VertexBufferFactory* pVB = new QRAPI_VertexBufferFactory();

        // succeeded?
        if (!pVB)
            return E_OUTOFMEMORY;

        HRESULT result = pVB->QueryInterface(iid, ppv);

        // failed, and no ref was added?
        if (result != S_OK && !pVB->RefCount)
            // in this case, delete the object to avoid memory leak
            delete pVB;

        return result;
    }
    else
    if (clsid == CLSID_QRAPI_VertexBuffersFactory)
    {
        // create new vertex buffers oject
        QRAPI_VertexBuffersFactory* pVBs = new QRAPI_VertexBuffersFactory();

        // succeeded?
        if (!pVBs)
            return E_OUTOFMEMORY;

        HRESULT result = pVBs->QueryInterface(iid, ppv);

        // failed, and no ref was added?
        if (result != S_OK && !pVBs->RefCount)
            // in this case, delete the object to avoid memory leak
            delete pVBs;

        return result;
    }
    else
    if (clsid == CLSID_QRAPI_MD2Factory)
    {
        // create new MD2 oject
        QRAPI_MD2Factory* pMD2 = new QRAPI_MD2Factory();

        // succeeded?
        if (!pMD2)
            return E_OUTOFMEMORY;

        HRESULT result = pMD2->QueryInterface(iid, ppv);

        // failed, and no ref was added?
        if (result != S_OK && !pMD2->RefCount)
            // in this case, delete the object to avoid memory leak
            delete pMD2;

        return result;
    }

    return CLASS_E_CLASSNOTAVAILABLE;
}
//------------------------------------------------------------------------------
/**
* Server entry point invoked to instruct the server to create registry entries for all classes
* supported by the module
*@author Jean-Milost Reymond
*/
EXTERN_C HRESULT QR_ENGINE_API __stdcall DllRegisterServer()
{
    char szInprocServer32Buff[MAX_PATH] = "";

    // get the "InprocServer32" module file name
    ::GetModuleFileName(g_hModule, szInprocServer32Buff, sizeof(szInprocServer32Buff));

    QRAPI_Keys keys;

    // populate keys to register
    PopulateRegistryKeys(keys);

    // get key count
    const QRAPI_SizeT count = keys.size();

    // iterate through keys to register
    for (QRAPI_SizeT i = 0; i < count; ++i)
    {
        WCHAR* lpwszClsid;
        char   szBuff[MAX_PATH]      = "";
        char   szClsid[MAX_PATH]     = "";
        char   szInproc[MAX_PATH]    = "";
        char   szDescriptionVal[256] = "";
        char   szProgId[MAX_PATH];

        // get class ID to register as string
        ::StringFromCLSID(keys[i].m_ClassID, &lpwszClsid);

        // create registry keys to use
        wsprintf(szClsid,  "%S",         lpwszClsid);
        wsprintf(szInproc, "%s\\%s\\%s", "clsid", szClsid, "InprocServer32");
        wsprintf(szProgId, "%s\\%s\\%s", "clsid", szClsid, "ProgId");

        // write default value
        wsprintf(szBuff,           "%s",     keys[i].m_Description.c_str());
        wsprintf(szDescriptionVal, "%s\\%s", "clsid", szClsid);

        // write class ID key in the registry
        QR_Registry::WriteKey(HKEY_CLASSES_ROOT,
                              szDescriptionVal,
                              NULL, // write to the "default" value
                              REG_SZ,
                              (void*)szBuff,
                              lstrlen(szBuff));

        // get the "InprocServer32" module file name
        lstrcpy(szBuff, szInprocServer32Buff);

        // write the "InprocServer32" key data
        QR_Registry::WriteKey(HKEY_CLASSES_ROOT,
                              szInproc,
                              NULL, // write to the "default" value
                              REG_SZ,
                              (void*)szBuff,
                              lstrlen(szBuff));

        lstrcpy(szBuff, keys[i].m_Name.c_str());

        // write the "ProgId" key data under HKCR\clsid\{---}\ProgId
        QR_Registry::WriteKey(HKEY_CLASSES_ROOT,
                              szProgId,
                              NULL,
                              REG_SZ,
                              (void*)szBuff,
                              lstrlen(szBuff));

        wsprintf(szBuff, "%s", keys[i].m_Description.c_str());

        // write the "ProgId" data under HKCR\CodeGuru.FastAddition
        QR_Registry::WriteKey(HKEY_CLASSES_ROOT,
                              keys[i].m_Name.c_str(),
                              NULL,
                              REG_SZ,
                              (void*)szBuff,
                              lstrlen(szBuff));

        wsprintf(szProgId, "%s\\%s", keys[i].m_Name.c_str(), "CLSID");

        QR_Registry::WriteKey(HKEY_CLASSES_ROOT,
                              szProgId,
                              NULL,
                              REG_SZ,
                              (void*)szClsid,
                              lstrlen(szClsid));
    }

    return 1;
}
//------------------------------------------------------------------------------
/**
* Server entry point invoked to instruct the server to remove all registry entries created through
* DllRegisterServer
*@author Jean-Milost Reymond
*/
EXTERN_C HRESULT QR_ENGINE_API __stdcall DllUnregisterServer()
{
    QRAPI_Keys keys;

    // populate keys to unregister
    PopulateRegistryKeys(keys);

    // get key count
    const QRAPI_SizeT count = keys.size();

    // iterate through keys to unregister
    for (QRAPI_SizeT i = 0; i < count; ++i)
    {
        char   szKeyName[256] = "";
        char   szClsid[256]   = "";
        WCHAR* lpwszClsid;

        // delete the ProgId entry
        wsprintf(szKeyName, "%s\\%s", keys[i].m_Name.c_str(), "CLSID");
        QR_Registry::DeleteKey(HKEY_CLASSES_ROOT, szKeyName,              QR_Registry::IE_RT_Both);
        QR_Registry::DeleteKey(HKEY_CLASSES_ROOT, keys[i].m_Name.c_str(), QR_Registry::IE_RT_Both);

        // get class ID to register as string
        ::StringFromCLSID(keys[i].m_ClassID, &lpwszClsid);

        //delete the CLSID entry for this COM object
        wsprintf(szClsid,   "%S",                  lpwszClsid);
        wsprintf(szKeyName, "%s\\%s\\%s", "CLSID", szClsid, "InprocServer32");
        QR_Registry::DeleteKey(HKEY_CLASSES_ROOT, szKeyName, QR_Registry::IE_RT_Both);

        wsprintf(szKeyName, "%s\\%s\\%s", "CLSID", szClsid, "ProgId");
        QR_Registry::DeleteKey(HKEY_CLASSES_ROOT, szKeyName, QR_Registry::IE_RT_Both);

        wsprintf(szKeyName,"%s\\%s", "CLSID", szClsid);
        QR_Registry::DeleteKey(HKEY_CLASSES_ROOT, szKeyName, QR_Registry::IE_RT_Both);
    }

    return 1;
}
// ------------------------------------------------------------------------------
/**
* Server installation/setup entry point. Used for 'PerUser' registration. Invoked via call to
* "regsvr32 /n /i:user [/u] axlibrary.dll"
*@author Jean-Milost Reymond
*/
EXTERN_C HRESULT QR_ENGINE_API __stdcall DllInstall(BOOL bInstall, LPCWSTR pszCmdLine)
{
    /* TODO FIXME
    Comserv::TComServer* comserver = Comserv::GetComServer();
    if (comserver)
    {
        bool savePerUser = comserver->PerUserRegistration;
        if (pszCmdLine && !Sysutils::StrIComp(pszCmdLine, L"user"))
            comserver->PerUserRegistration = true;
        else
            comserver->PerUserRegistration = false;
        HRESULT result = E_FAIL;
        if (bInstall)
        {
            result = DllRegisterServer();
            if (result == E_FAIL)
                DllUnregisterServer();
        }
        else
            result = DllUnregisterServer();
        comserver->PerUserRegistration = savePerUser;
        return result;
    }
    */

    return E_FAIL;
}
//------------------------------------------------------------------------------

