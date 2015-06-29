/*****************************************************************************
 * ==> QR_Exception ---------------------------------------------------------*
 * ***************************************************************************
 * Description : Engine exception class                                      *
 * Developer   : Jean-Milost Reymond                                         *
 *****************************************************************************/

#include "QR_Exception.h"

// std
#include <sstream>

//------------------------------------------------------------------------------
// QR_ExceptionFormatter - c++ cross-platform
//------------------------------------------------------------------------------
std::string QR_ExceptionFormatter::Format(const std::string& type,
                                          const std::string& message,
                                          const std::string& file,
                                          const std::string& function,
                                          QR_SizeT           line)
{
    std::ostringstream sstr;

    sstr << "[";

    if (type.empty())
        sstr << "Unknown";
    else
        sstr << type;

    sstr << " exception raised]" << std::endl << "[Message]  ";

    if (message.empty())
        sstr << "Unknown exception" << std::endl;
    else
        sstr << message << std::endl;

    sstr << "[File] "     << file     << std::endl
         << "[Function] " << function << std::endl
         << "[Line] "     << line     << std::endl;

    return sstr.str();
}
//------------------------------------------------------------------------------
std::string QR_ExceptionFormatter::Format_HTML(const std::string& type,
                                               const std::string& message,
                                               const std::string& file,
                                               const std::string& function,
                                               QR_SizeT           line)
{
    std::ostringstream sstr;

    sstr << "[";

    if (type.empty())
        sstr << "Unknown";
    else
        sstr << type;

    sstr << " exception raised]" << "<br>" << "[Message]  ";

    if (message.empty())
        sstr << "Unknown exception" << "<br>";
    else
        sstr << message << "<br>";

    sstr << "[File] "     << file     << "<br>"
         << "[Function] " << function << "<br>"
         << "[Line] "     << line     << "<br>";

    return sstr.str();
}
//------------------------------------------------------------------------------
std::string QR_ExceptionFormatter::Format_HTML_Private(const std::string& type,
                                                       const std::string& message,
                                                       const std::string& file,
                                                       const std::string& function,
                                                       QR_SizeT           line)
{
    std::ostringstream sstr;

    sstr << "[";

    if (type.empty())
        sstr << "Unknown";
    else
        sstr << type;

    sstr << " exception raised]" << "<br>" << "[Message]  ";

    if (message.empty())
        sstr << "Unknown exception" << "<br>";
    else
        sstr << message << "<br>";

    sstr << "[File] ######<br>"
         << "[Function] ######<br>"
         << "[Line] " << line << "<br>";

    return sstr.str();
}
//------------------------------------------------------------------------------
std::string QR_ExceptionFormatter::Message(const std::string& message,
                                           const std::string& function,
                                           QR_SizeT           line)
{
    std::ostringstream sstr;

    if (message.empty())
        sstr << "Unknown exception";
    else
        sstr << message;

    sstr << std::endl;

    sstr << "[Function] " << function << std::endl
         << "[Line] "     << line << std::endl;

    return sstr.str();
}
//------------------------------------------------------------------------------
std::string QR_ExceptionFormatter::Message_HTML(const std::string& message,
                                                const std::string& function,
                                                QR_SizeT           line)
{
    std::ostringstream sstr;

    if (message.empty())
        sstr << "Unknown exception";
    else
        sstr << message;

    sstr << "<br>";

    sstr << "[Function] " << function << "<br>"
         << "[Line] "     << line << "<br>";

    return sstr.str();
}
//------------------------------------------------------------------------------
// QR_Exception - c++ cross-platform
//------------------------------------------------------------------------------
QR_Exception::QR_Exception() throw()
{}
//------------------------------------------------------------------------------
QR_Exception::QR_Exception(const std::string& message,
                           const std::string& file,
                           const std::string& function,
                           QR_SizeT           line) throw()
{
    m_Message  = message;
    m_File     = file;
    m_Function = function;
    m_Line     = line;
}
//------------------------------------------------------------------------------
QR_Exception::~QR_Exception() throw()
{}
//------------------------------------------------------------------------------
const QR_Char* QR_Exception::what() const throw()
{
    return Format();
}
//------------------------------------------------------------------------------
const QR_Char* QR_Exception::Format() const
{
    std::ostringstream sstr;

    sstr << "[QR_Exception raised]" << std::endl << "[Message]  ";

    if (m_Message.empty())
        sstr << "Unknown exception" << std::endl;
    else
        sstr << m_Message << std::endl;

    sstr << "[File] "     << m_File << std::endl
         << "[Function] " << m_Function << std::endl
         << "[Line] "     << m_Line << std::endl;

    return sstr.str().c_str();
}
//------------------------------------------------------------------------------
std::string QR_Exception::Format_HTML() const
{
    std::ostringstream sstr;

    sstr << "[QR_Exception raised]" << "<br>" << "[Message]  ";

    if (m_Message.empty())
        sstr << "Unknown exception" << "<br>";
    else
        sstr << m_Message << "<br>";

    sstr << "[File] "     << m_File << "<br>"
         << "[Function] " << m_Function << "<br>"
         << "[Line] "     << m_Line << "<br>";

    return sstr.str().c_str();
}
//------------------------------------------------------------------------------
std::string QR_Exception::Format_HTML_Private() const
{
    std::ostringstream sstr;

    sstr << "[QR_Exception raised]" << "<br>" << "[Message]  ";

    if (m_Message.empty())
        sstr << "Unknown exception" << "<br>";
    else
        sstr << m_Message << "<br>";

    sstr << "[File] ######<br>"
         << "[Function] ######<br>"
         << "[Line] " << m_Line << "<br>";

    return sstr.str().c_str();
}
//------------------------------------------------------------------------------
const std::string& QR_Exception::Message() const
{
    return m_Message;
}
//------------------------------------------------------------------------------

