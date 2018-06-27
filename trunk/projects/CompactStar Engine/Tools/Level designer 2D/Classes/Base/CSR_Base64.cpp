/****************************************************************************
 * ==> CSR_Base64 ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a base 64 encoder and decoder         *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2018, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#include "CSR_Base64.h"

//---------------------------------------------------------------------------
// Global variables
//---------------------------------------------------------------------------
/**
* Table to encode the base64 data
*/
const static char* g_EncodeTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" ;
//---------------------------------------------------------------------------
/**
* Table to decode the base64 data, this will map A => 0, B => 1, ...
*/
const static unsigned char g_DecodeTable[] =
{
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  62, 0,  0,  0,  63, 52, 53,
    54, 55, 56, 57, 58, 59, 60, 61, 0,  0,
    0,  0,  0,  0,  0,  0,  1,  2,  3,  4,
    5,  6,  7,  8,  9,  10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 0,  0,  0,  0,  0,  0,  26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,
};
//---------------------------------------------------------------------------
// CSR_Base64
//---------------------------------------------------------------------------
std::string CSR_Base64::Encode(const unsigned char* pData, std::size_t length)
{
    std::string result;

    // calculate the padding to apply. 2 will become 1 and 1 will become 2 but 0 remains 0
    const std::size_t modLen  = length % 3;
    const std::size_t padding = ((modLen & 1) << 1) + ((modLen & 2) >> 1);

    // reserve memory for the string
    result.resize(4 * (length + padding) / 3);

    std::size_t offset = 0;
    std::size_t i;

    // iterate through data to encode
    for (i = 0; i <= length - 3; i += 3)
    {
        // get 3 next byte to encode
        const unsigned char byte0 = pData[i];
        const unsigned char byte1 = pData[i + 1];
        const unsigned char byte2 = pData[i + 2];

        // encode byte
        result[offset]  = g_EncodeTable[         byte0  >> 2];                 ++offset;
        result[offset]  = g_EncodeTable[((0x03 & byte0) << 4) + (byte1 >> 4)]; ++offset;
        result[offset]  = g_EncodeTable[((0x0f & byte1) << 2) + (byte2 >> 6)]; ++offset;
        result[offset]  = g_EncodeTable[  0x3f & byte2];                       ++offset;
    }

    // encode the final part, if required
    switch (padding)
    {
        case 2:
        {
            result[offset] = g_EncodeTable[        pData[i]  >> 2]; ++offset;
            result[offset] = g_EncodeTable[(0x03 & pData[i]) << 4]; ++offset;
            result[offset] = '=';                                   ++offset;
            result[offset] = '=';
            break;
        }

        case 1:
        {
            result[offset] = g_EncodeTable[         pData[i]      >> 2];                        ++offset;
            result[offset] = g_EncodeTable[((0x03 & pData[i])     << 4) + (pData[i + 1] >> 4)]; ++offset;
            result[offset] = g_EncodeTable[ (0x0f & pData[i + 1]) << 2];                        ++offset;
            result[offset] = '=';
            break;
        }
    }

    return result;
}
//---------------------------------------------------------------------------
void CSR_Base64::Decode(const std::string& str, unsigned char*& pData, std::size_t& length)
{
    pData  = NULL;
    length = 0;

    // is source string valid?
    if (str.length() < 2)
        return;

    std::size_t padding = 0;

    // check if last char is padding
    if (str[length - 1] == '=')
        ++padding;

    // check if before last char is padding
    if (str[length - 2] == '=')
        ++padding;

    // calculate data length and allocate memory for data
    length = (3 * (str.length() / 4)) - padding;
    pData  = new unsigned char[length];

    // data was assigned correctly?
    if (!pData)
    {
        length = 0;
        return;
    }

    std::size_t i;
    std::size_t offset = 0;
    std::size_t count  = str.length() - 4 - padding;

    // iterate through chars to decode
    for (i = 0; i <= count; i += 4)
    {
        // get bytes to decode from table
        const std::size_t a = g_DecodeTable[str[i]];
        const std::size_t b = g_DecodeTable[str[i + 1]];
        const std::size_t c = g_DecodeTable[str[i + 2]];
        const std::size_t d = g_DecodeTable[str[i + 3]];

        // decode bytes
        pData[offset] = (a << 2) | (b >> 4); ++offset;
        pData[offset] = (b << 4) | (c >> 2); ++offset;
        pData[offset] = (c << 6) | (d);      ++offset;
    }

    // decode the final part, if required
    switch (padding)
    {
        case 1:
        {
            const std::size_t a = g_DecodeTable[str[i]];
            const std::size_t b = g_DecodeTable[str[i + 1]];
            const std::size_t c = g_DecodeTable[str[i + 2]];

            pData[offset] = (a << 2) | (b >> 4); ++offset;
            pData[offset] = (b << 4) | (c >> 2);
            break;
        }

        case 2:
        {
            const std::size_t a = g_DecodeTable[str[i]];
            const std::size_t b = g_DecodeTable[str[i + 1]];

            pData[offset] = (a << 2) | (b >> 4);
            break;
        }
    }
}
//---------------------------------------------------------------------------
