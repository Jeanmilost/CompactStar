/****************************************************************************
 * ==> CSR_Base64 ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a base 64 encoder and decoder         *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2022, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#ifndef CSR_Base64H
#define CSR_Base64H

// std
#include <stdio.h>
#include <stdlib.h>
#include <string>

/**
* Base64 encoder and decoder
*@author Jean-Milost Reymond
*/
class CSR_Base64
{
    public:
        /**
        * Encodes a binary data in a base64 string
        *@param pData - binary data to encode
        *@param length - binary data length
        *@return binary data as base64 encoded string
        */
        static std::string Encode(const unsigned char* pData, std::size_t length);

        /**
        * Decodes a base64 string and get resulting data
        *@param str - base64 string containing data to decode
        *@param[out] pData - decoded data, the array should be deleted by user when useless
        *@param[out] length - data length
        *@note The caller is responsible to delete the data when it become useless
        */
        static void Decode(const std::string& str, unsigned char*& pData, std::size_t& length);
};

#endif
