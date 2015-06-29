/******************************************************************************
 * ==> QR_StringTools --------------------------------------------------------*
 ******************************************************************************
 * Description : Tools for string manipulation                                *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QR_StringTools.h"

// qr engine
#include "QR_Exception.h"

// libraries
#include "ConvertUTF.h"

//------------------------------------------------------------------------------
// QR_StringTools - c++ cross-platform
//------------------------------------------------------------------------------
std::wstring QR_StringTools::StrToWStr(const std::string& str)
{
    std::wstring wstr(str.length(), L' ');
    std::copy(str.begin(), str.end(), wstr.begin());
    return wstr;
}
//------------------------------------------------------------------------------
std::string QR_StringTools::WStrToStr(const std::wstring& wstr)
{
    std::string str(wstr.length(), ' ');
    std::copy(wstr.begin(), wstr.end(), str.begin());
    return str;
}
//------------------------------------------------------------------------------
std::wstring QR_StringTools::UTF8ToUTF16(const std::string& str)
{
    // source code from:
    // http://www.codeproject.com/Articles/17573/Convert-Between-std-string-and-std-wstring-UTF-8-a

    size_t wideSize = str.length();

    #if defined(__BORLANDC__) || defined(__CODEGEARC__)
        #pragma warn -8008
        #pragma warn -8066
    #endif

    // search for system char length (can be different on Windows, Linux, OSX or other systems)
    if (sizeof(QR_WChar) == 2)
    {
        // calculate utf16 size, prepare utf16 container and calculate source and dest offsets
        QR_WChar*   pWideStringNative = new QR_WChar[wideSize + 1];
        const UTF8* pSourceStart      = reinterpret_cast<const UTF8*>(str.c_str());
        const UTF8* pSourceEnd        = pSourceStart + wideSize;
        UTF16*      pTargetStart      = reinterpret_cast<UTF16*>(pWideStringNative);
        UTF16*      pTargetEnd        = pTargetStart + wideSize + 1;

        // try to convert to utf16
        ConversionResult res = ConvertUTF8toUTF16(&pSourceStart,
                                                  pSourceEnd,
                                                  &pTargetStart,
                                                  pTargetEnd,
                                                  lenientConversion);

        // conversion failed?
        if (res != conversionOK)
        {
            delete[] pWideStringNative;
            M_THROW_EXCEPTION("Conversion failed - wchar_t length - 2");
        }

        // get and return utf16 converted string
        *pTargetStart = 0;
        std::wstring resultString(pWideStringNative);
        delete[] pWideStringNative;
        return resultString;
    }
    else
    if (sizeof(QR_WChar) == 4)
    {
        // calculate utf16 size, prepare utf16 container and calculate source and dest offsets
        QR_WChar*   pWideStringNative = new QR_WChar[wideSize + 1];
        const UTF8* pSourceStart      = reinterpret_cast<const UTF8*>(str.c_str());
        const UTF8* pSourceEnd        = pSourceStart + wideSize;
        UTF32*      pTargetStart      = reinterpret_cast<UTF32*>(pWideStringNative);
        UTF32*      pTargetEnd        = pTargetStart + wideSize;

        // try to convert to utf16
        ConversionResult res = ConvertUTF8toUTF32(&pSourceStart,
                                                  pSourceEnd,
                                                  &pTargetStart,
                                                  pTargetEnd,
                                                  lenientConversion);

        // conversion failed?
        if (res != conversionOK)
        {
            delete[] pWideStringNative;
            M_THROW_EXCEPTION("Conversion failed - wchar_t length - 4");
        }

        // get and return utf16 converted string
        *pTargetStart = 0;
        std::wstring resultString(pWideStringNative);
        delete[] pWideStringNative;
        return resultString;
    }
    else
        M_THROW_EXCEPTION("Conversion failed - unknown wchar_t length");

    #if defined(__BORLANDC__) || defined(__CODEGEARC__)
        #pragma warn .8066
        #pragma warn .8008
    #endif
}
//------------------------------------------------------------------------------
std::string QR_StringTools::UTF16ToUTF8(const std::wstring& wstr)
{
    // source code from:
    // http://www.codeproject.com/Articles/17573/Convert-Between-std-string-and-std-wstring-UTF-8-a

    size_t wideSize = wstr.length();

    #if defined(__BORLANDC__) || defined(__CODEGEARC__)
        #pragma warn -8008
        #pragma warn -8066
    #endif

    // search for system char length (can be different on Windows, Linux, OSX or other systems)
    if (sizeof(QR_WChar) == 2)
    {
        // calculate utf8 size, prepare utf8 container and calculate source and dest offsets
        QR_SizeT     utf8Size          = 2 * wideSize + 1;
        QR_Char*     pUtf8StringNative = new QR_Char[utf8Size];
        const UTF16* pSourceStart      = reinterpret_cast<const UTF16*>(wstr.c_str());
        const UTF16* pSourceEnd        = pSourceStart + wideSize;
        UTF8*        pTargetStart      = reinterpret_cast<UTF8*>(pUtf8StringNative);
        UTF8*        pTargetEnd        = pTargetStart + utf8Size;

        // try to convert to utf8
        ConversionResult res = ConvertUTF16toUTF8(&pSourceStart,
                                                  pSourceEnd,
                                                  &pTargetStart,
                                                  pTargetEnd,
                                                  lenientConversion);

        // conversion failed?
        if (res != conversionOK)
        {
            delete [] pUtf8StringNative;
            M_THROW_EXCEPTION("Conversion failed - wchar_t length - 2");
        }

        // get and return utf8 converted string
        *pTargetStart = 0;
        std::string resultString(pUtf8StringNative);
        delete [] pUtf8StringNative;
        return resultString;
    }
    else
    if (sizeof(QR_WChar) == 4)
    {
        // calculate utf8 size, prepare utf8 container and calculate source and dest offsets
        QR_SizeT     utf8Size          = 4 * wideSize + 1;
        QR_Char*     pUtf8StringNative = new QR_Char[utf8Size];
        const UTF32* pSourceStart      = reinterpret_cast<const UTF32*>(wstr.c_str());
        const UTF32* pSourceEnd        = pSourceStart + wideSize;
        UTF8*        pTargetStart      = reinterpret_cast<UTF8*>(pUtf8StringNative);
        UTF8*        pTargetEnd        = pTargetStart + utf8Size;

        // try to convert to utf8
        ConversionResult res = ConvertUTF32toUTF8(&pSourceStart,
                                                  pSourceEnd,
                                                  &pTargetStart,
                                                  pTargetEnd,
                                                  lenientConversion);

        // conversion failed?
        if (res != conversionOK)
        {
            delete [] pUtf8StringNative;
            M_THROW_EXCEPTION("Conversion failed - wchar_t length - 4");
        }

        // get and return utf8 converted string
        *pTargetStart = 0;
        std::string resultString(pUtf8StringNative);
        delete [] pUtf8StringNative;
        return resultString;
    }
    else
        M_THROW_EXCEPTION("Conversion failed - unknown wchar_t length");

    #if defined(__BORLANDC__) || defined(__CODEGEARC__)
        #pragma warn .8066
        #pragma warn .8008
    #endif
}
//------------------------------------------------------------------------------
std::string QR_StringTools::Fill(const std::string& text, QR_Char fillChar, QR_SizeT lineLength)
{
    if (text.length() >= lineLength)
        return text;

    std::string rawLine(lineLength, fillChar);

    QR_SizeT startPos = ((lineLength - text.length()) / 2);

    rawLine.replace(startPos, text.length(), text);
    return rawLine;
}
//------------------------------------------------------------------------------
std::wstring QR_StringTools::Fill(const std::wstring& text, QR_WChar fillChar, QR_SizeT lineLength)
{
    if (text.length() + 2 >= lineLength)
        return text;

    std::wstring rawLine(lineLength, fillChar);

    QR_SizeT startPos = ((lineLength - text.length()) / 2);

    rawLine.replace(startPos, text.length(), text);
    return rawLine;
}
//------------------------------------------------------------------------------
std::string QR_StringTools::Trim(const std::string& value)
{
    // value is empty?
    if (!value.length())
        return value;

    QR_SizeT startPos = 0;

    // search for first non space char
    for (QR_SizeT i = 0; i < value.length(); ++i)
        if (value[i] != ' ')
        {
            startPos = i;
            break;
        }

    QR_SizeT endPos = value.length();

    // search for last non space char
    for (QR_Int32 i = value.length() - 1; i >= 0; --i)
        if (value[i] != ' ')
        {
            endPos = i + 1;
            break;
        }

    // is start and end position identical or invalid?
    if (startPos >= endPos - 1)
        return "";

    // return trimmed string
    return value.substr(startPos, endPos - startPos);
}
//------------------------------------------------------------------------------
std::wstring QR_StringTools::Trim(const std::wstring& value)
{
    // value is empty?
    if (!value.length())
        return value;

    QR_SizeT startPos = 0;

    // search for first non space char
    for (QR_SizeT i = 0; i < value.length(); ++i)
        if (value[i] != ' ')
        {
            startPos = i;
            break;
        }

    QR_SizeT endPos = value.length();

    // search for last non space char
    for (QR_Int32 i = value.length() - 1; i >= 0; --i)
        if (value[i] != ' ')
        {
            endPos = i + 1;
            break;
        }

    // is start and end position identical or invalid?
    if (startPos >= endPos - 1)
        return L"";

    // return trimmed string
    return value.substr(startPos, endPos - startPos);
}
//------------------------------------------------------------------------------
std::string QR_StringTools::ToLowerCase(const std::string& text)
{
    // is text empty?
    if (text.empty())
        return "";

    std::string result;

    // iterate through text chars
    for (QR_SizeT i = 0; i < text.length(); ++i)
        // convert to lower case if needed
        result += std::tolower(text[i]);

    return result;
}
//------------------------------------------------------------------------------
std::wstring QR_StringTools::ToLowerCase(const std::wstring& text)
{
    // is text empty?
    if (text.empty())
        return L"";

    std::wstring result;

    // iterate through text chars
    for (QR_SizeT i = 0; i < text.length(); ++i)
        // convert to lower case if needed
        result += std::tolower(text[i]);

    return result;
}
//------------------------------------------------------------------------------
std::string QR_StringTools::ToUpperCase(const std::string& text)
{
    // is text empty?
    if (text.empty())
        return "";

    std::string result;

    // iterate through text chars
    for (QR_SizeT i = 0; i < text.length(); ++i)
        // convert to lower case if needed
        result += std::toupper(text[i]);

    return result;
}
//------------------------------------------------------------------------------
std::wstring QR_StringTools::ToUpperCase(const std::wstring& text)
{
    // is text empty?
    if (text.empty())
        return L"";

    std::wstring result;

    // iterate through text chars
    for (QR_SizeT i = 0; i < text.length(); ++i)
        // convert to lower case if needed
        result += std::toupper(text[i]);

    return result;
}
//------------------------------------------------------------------------------
std::string  QR_StringTools::SearchAndReplace(const std::string& str, const std::string& search,
        const std::string& replace, bool all)
{
    std::string result = str;

    // begin search on first character
    QR_SizeT position = 0;

    // found search occurrence
    while ((position = result.find(search, position)) != result.npos)
    {
        // replace occurrence
        result.replace(position, search.length(), replace);

        // do replace all occurrences?
        if (!all)
            return result;

        // calculate next search position
        position += replace.length();
    }

    return result;
}
//------------------------------------------------------------------------------
std::wstring QR_StringTools::SearchAndReplace(const std::wstring& str, const std::wstring& search,
        const std::wstring& replace, bool all)
{
    std::wstring result = str;

    // begin search on first character
    QR_SizeT position = 0;

    // found search occurrence
    while ((position = result.find(search, position)) != result.npos)
    {
        // replace occurrence
        result.replace(position, search.length(), replace);

        // do replace all occurrences?
        if (!all)
            return result;

        // calculate next search position
        position += replace.length();
    }

    return result;
}
//------------------------------------------------------------------------------
std::string QR_StringTools::BoolToStr(bool value, bool numeric)
{
    if (numeric)
        return value ? "1" : "0";

    return value ? "true" : "false";
}
//------------------------------------------------------------------------------
std::wstring QR_StringTools::BoolToWStr(bool value, bool numeric)
{
    if (numeric)
        return value ? L"1" : L"0";

    return value ? L"true" : L"false";
}
//------------------------------------------------------------------------------
bool QR_StringTools::StrToBool(const std::string& str)
{
    return (str == "1" || ToLowerCase(str) == "true");
}
//------------------------------------------------------------------------------
bool QR_StringTools::StrToBool(const std::wstring& str)
{
    return (str == L"1" || ToLowerCase(str) == L"true");
}
//------------------------------------------------------------------------------

