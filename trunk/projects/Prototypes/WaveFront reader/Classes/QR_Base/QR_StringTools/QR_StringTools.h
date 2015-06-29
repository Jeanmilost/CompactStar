/******************************************************************************
 * ==> QR_StringTools --------------------------------------------------------*
 ******************************************************************************
 * Description : Tools for string manipulation                                *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_StringToolsH
#define QR_StringToolsH

// std
#include <string>

// qr engine
#include "QR_Types.h"

/**
* Tools for string manipulation
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_StringTools
{
    public:
        /**
        * Converts string to wstring
        *@param wstr - string to convert
        *@return converted string
        */
        static std::wstring StrToWStr(const std::string& str);

        /**
        * Converts wstring to string
        *@param wstr - string to convert
        *@return converted string
        */
        static std::string WStrToStr(const std::wstring& wstr);

        /**
        * Converts utf8 to utf16
        *@param str - string to convert
        *@return converted string
        */
        static std::wstring UTF8ToUTF16(const std::string& str);

        /**
        * Converts utf16 to utf8
        *@param wstr - string to convert
        *@return converted string
        */
        static std::string UTF16ToUTF8(const std::wstring& wstr);

        /**
        * Fills line
        *@param text - text
        *@param fillChar - fill char
        *@param lineLength - line length
        *@return filled line
        */
        static std::string  Fill(const std::string&  text, QR_Char  fillChar, QR_SizeT lineLength);
        static std::wstring Fill(const std::wstring& text, QR_WChar fillChar, QR_SizeT lineLength);

        /**
        * Trims text (i.e. remove spaces before and after text)
        *@param value - value to trim
        *@return trimmed value
        */
        static std::string  Trim(const std::string&  value);
        static std::wstring Trim(const std::wstring& value);

        /**
        * Converts text to lower case
        *@return lowered case text
        */
        static std::string  ToLowerCase(const std::string&  text);
        static std::wstring ToLowerCase(const std::wstring& text);

        /**
        * Converts text to upper case
        *@return uppered case text
        */
        static std::string  ToUpperCase(const std::string&  text);
        static std::wstring ToUpperCase(const std::wstring& text);

        /**
        * Searches for a string occurrence in another string, and replaces by a replacement string
        *@param str - string to scan
        *@param search - string to search
        *@param replace - string to replace by
        *@param all - if true, all string occurrence will be replaced, only first occurrence if false
        *@return processed string
        */
        static std::string  SearchAndReplace(const std::string&  str, const std::string&  search,
                const std::string&  replace, bool all);
        static std::wstring SearchAndReplace(const std::wstring& str, const std::wstring& search,
                const std::wstring& replace, bool all);

        /**
        * Converts boolean value to string
        *@param value - value to convert
        *@aram numeric - if true, result will contains numeric value, otherwise litteral value
        *@return converted string
        */
        static std::string  BoolToStr(bool value,  bool numeric);
        static std::wstring BoolToWStr(bool value, bool numeric);

        /**
        * Converts string to boolean value
        *@param str - string to convert
        *@return converted value
        */
        static bool StrToBool(const std::string&  str);
        static bool StrToBool(const std::wstring& str);
};
#endif
