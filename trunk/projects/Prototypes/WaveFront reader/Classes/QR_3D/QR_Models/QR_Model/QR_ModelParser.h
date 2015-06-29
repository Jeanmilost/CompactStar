/******************************************************************************
 * ==> QR_ModelParser --------------------------------------------------------*
 ******************************************************************************
 * Description : Basic model parser interface                                 *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_ModelParserH
#define QR_ModelParserH

// qr engine
#include "QR_Types.h"
#include "QR_Buffer.h"

/**
* Basic model parser interface
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_ModelParser
{
    public:
        QR_ModelParser();
        virtual ~QR_ModelParser();

        /**
        * Loads model from file
        *@param fileName - file name
        *@return true on success, otherwise false
        */
        virtual bool Load(const std::string&  fileName) = 0;
        virtual bool Load(const std::wstring& fileName) = 0;

        /**
        * Loads model from buffer
        *@param buffer - buffer
        *@param length - length to read in buffer
        *@return true on success, otherwise false
        *@note Read will begin from current offset
        */
        virtual bool Load(      QR_Buffer&            buffer,
                          const QR_Buffer::ISizeType& length) = 0;
};
#endif
