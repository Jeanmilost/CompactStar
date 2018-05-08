/*****************************************************************************
 * ==> CSR_MessageHelper ----------------------------------------------------*
 *****************************************************************************
 * Description : This module provides a helper class for the messages owned  *
 *               by the application                                          *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#ifndef CSR_MessageHelperH
#define CSR_MessageHelperH

// std
#include <string>
#include <memory>

// helper macros to catching and showing error messages to user
#define M_CSR_Try try
#define M_CSR_CatchShow                                                                                                        \
    catch (const System::Sysutils::Exception& e)                                                                               \
    {                                                                                                                          \
        ::MessageDlg((CSR_MessageHelper::Get()->GetError_Unexpected() + e.Message.c_str()).c_str(),                            \
                     mtError,                                                                                                  \
                     TMsgDlgButtons() << mbOK,                                                                                 \
                     0);                                                                                                       \
    }                                                                                                                          \
    catch (const std::exception& ex)                                                                                           \
    {                                                                                                                          \
        ::MessageDlg((CSR_MessageHelper::Get()->GetError_Unexpected() + UnicodeString(AnsiString(ex.what())).c_str()).c_str(), \
                     mtError,                                                                                                  \
                     TMsgDlgButtons() << mbOK,                                                                                 \
                     0);                                                                                                       \
    }                                                                                                                          \
    catch (...)                                                                                                                \
    {                                                                                                                          \
        ::MessageDlg((CSR_MessageHelper::Get()->GetError_Unexpected() + L"Unknown error").c_str(),                             \
                     mtError,                                                                                                  \
                     TMsgDlgButtons() << mbOK,                                                                                 \
                     0);                                                                                                       \
    }

/**
* Helper containing all the application messages
*@author Jean-Milost Reymond
*/
class CSR_MessageHelper
{
    public:
        /**
        * Get message helper instance
        *@returns instance
        */
        static CSR_MessageHelper* Get();

        /**
        * Delete message helper instance
        */
        static void Delete();

        /**
        * Gets the scene dir
        *@return the scene dir
        */
        std::wstring GetSceneDir() const;

        /**
        * Gets the textures dir
        *@return the textures dir
        */
        std::wstring GetTexturesDir() const;

        /**
        * Gets the models dir
        *@return the models dir
        */
        std::wstring GetModelsDir() const;

        /**
        * Gets the loading model textures warning message
        *@return the loading model textures warning message
        */
        std::wstring GetWarn_ModelTextures() const;

        /**
        * Gets the unsupported textures count warning message
        *@return the unsupported textures count warning message
        */
        std::wstring GetWarn_UnsupportedTextureCount() const;

        /**
        * Gets the copy texture error message
        *@return the copy texture error message
        */
        std::wstring GetError_CopyTexture() const;

        /**
        * Gets the opening model error message
        *@return the opening model error message
        */
        std::wstring GetError_OpeningModel() const;

        /**
        * Gets the loading model error message
        *@return the loading model error message
        */
        std::wstring GetError_LoadingModel() const;

        /**
        * Gets the unexpected error message
        *@return the unexpected error message
        */
        std::wstring GetError_Unexpected() const;

    private:
        /**
        * Instance class, needed to allow auto_ptr usage despite of singleton privacity and without
        * declare auto_ptr friend
        */
        struct IInstance
        {
            CSR_MessageHelper* m_pInstance;

            IInstance();
            virtual ~IInstance();
        };

        static std::auto_ptr<IInstance> m_pMessageHelper;

        /**
        * Constructor
        *@note This constructor cannot be accessed externally to prevent unwanted object instances
        */
        CSR_MessageHelper();

        /**
        * Copy constructor
        *@param other - other object to copy from
        *@note This constructor cannot be accessed externally to prevent unwanted copies
        */
        CSR_MessageHelper(const CSR_MessageHelper& other);

        /**
        * Copy operator
        *@param other - other object to copy from
        *@note This operator cannot be accessed externally to prevent unwanted copies
        */
        CSR_MessageHelper& operator = (const CSR_MessageHelper& other);

        /**
        * Destructor
        *@note This destructor cannot be accessed externally to prevent unwanted destructions
        */
        ~CSR_MessageHelper();
};

#endif
