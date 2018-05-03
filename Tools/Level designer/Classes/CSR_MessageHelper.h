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
