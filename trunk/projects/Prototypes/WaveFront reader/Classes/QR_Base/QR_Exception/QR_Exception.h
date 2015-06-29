/*****************************************************************************
 * ==> QR_Exception ---------------------------------------------------------*
 * ***************************************************************************
 * Description : Engine exception class                                      *
 * Developer   : Jean-Milost Reymond                                         *
 *****************************************************************************/

#ifndef QR_ExceptionH
#define QR_ExceptionH

// std
#include <exception>
#include <string>

// qr engine
#include "QR_Types.h"

//------------------------------------------------------------------------------
// Global defines
//------------------------------------------------------------------------------
#define M_THROW_EXCEPTION(message) throw QR_Exception(message, __FILE__, __FUNCTION__, __LINE__)
#define M_FORMAT_EXCEPTION(type, message) QR_ExceptionFormatter::Format(type, message, __FILE__,\
        __FUNCTION__, __LINE__)
#define M_FORMAT_EXCEPTION_HTML(type, message) QR_ExceptionFormatter::Format_HTML(type, message,\
        __FILE__, __FUNCTION__, __LINE__)
#define M_FORMAT_EXCEPTION_HTML_PRIVATE(type, message) QR_ExceptionFormatter::Format_HTML_Private(type, message,\
        __FILE__, __FUNCTION__, __LINE__)
#define M_FORMAT_MESSAGE(message) QR_ExceptionFormatter::Message(message, __FUNCTION__, __LINE__)
#define M_FORMAT_MESSAGE_HTML(message) QR_ExceptionFormatter::Message_HTML(message, __FUNCTION__, __LINE__)
#define M_ASSERT(condition) {if (!condition) M_THROW_EXCEPTION("Assertion failed");}
//------------------------------------------------------------------------------

/**
* Tool to format exceptions
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_ExceptionFormatter
{
    public:
        /**
        * Formats exception message for logs
        *@param type - type of exception (STD, QR_Exception, ...)
        *@param message - message to format
        *@param file - the name of the file where the exception was raised
        *@param function - the name of the function where the exception was raised
        *@param line - the line number where the excption was raised
        *@return formatted message
        */
        static std::string Format(const std::string& type,
                                  const std::string& message,
                                  const std::string& file,
                                  const std::string& function,
                                  QR_SizeT           line);

        /**
        * Formats exception message for HTML logs
        *@param type - type of exception (STD, QR_Exception, ...)
        *@param message - message to format
        *@param file - the name of the file where the exception was raised
        *@param function - the name of the function where the exception was raised
        *@param line - the line number where the excption was raised
        *@return formatted message
        */
        static std::string Format_HTML(const std::string& type,
                                       const std::string& message,
                                       const std::string& file,
                                       const std::string& function,
                                       QR_SizeT           line);

        /**
        * Formats exception message for HTML logs, hiding sensitive data
        *@param type - type of exception (STD, QR_Exception, ...)
        *@param message - message to format
        *@param file - the name of the file where the exception was raised
        *@param function - the name of the function where the exception was raised
        *@param line - the line number where the excption was raised
        *@return formatted message
        */
        static std::string Format_HTML_Private(const std::string& type,
                                               const std::string& message,
                                               const std::string& file,
                                               const std::string& function,
                                               QR_SizeT           line);

        /**
        * Formats a message to display to user
        *@param message - message to format
        *@param function - the name of the function where the exception was raised
        *@param line - the line number where the excption was raised
        *@return formatted message
        */
        static std::string Message(const std::string& message,
                                   const std::string& function,
                                   QR_SizeT           line);

        /**
        * Formats a message to display to user for HTML
        *@param message - message to format
        *@param function - the name of the function where the exception was raised
        *@param line - the line number where the excption was raised
        *@return formatted message
        */
        static std::string Message_HTML(const std::string& message,
                                        const std::string& function,
                                        QR_SizeT           line);
};

/**
* Exception class
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_Exception : public std::exception
{
    public:
        /**
        * Constructor
        */
        QR_Exception() throw();

        /**
        * Constructor
        *@param message - exception message
        */
        QR_Exception(const std::string& message,
                     const std::string& file,
                     const std::string& function,
                     QR_SizeT           line) throw();

        /**
        * Destructor
        */
        virtual ~QR_Exception() throw();

        /**
        * Gets default exception error message
        *@return default exception error message
        */
        virtual const QR_Char* what() const throw();

        /**
        * Formats exception text
        *@return formatted text
        */
        virtual const QR_Char* Format() const;

        /**
        * Formats exception text for HTML
        *@return formatted text
        */
        virtual std::string Format_HTML() const;

        /**
        * Formats exception text for HTML, hiding sensitive data
        *@return formatted text
        */
        virtual std::string Format_HTML_Private() const;

        /**
        * Gets exception message
        *@return exception message
        */
        virtual const std::string& Message() const;

    private:
        std::string m_FormattedMessage;
        std::string m_Message;
        std::string m_File;
        std::string m_Function;
        QR_SizeT    m_Line;
};
#endif
