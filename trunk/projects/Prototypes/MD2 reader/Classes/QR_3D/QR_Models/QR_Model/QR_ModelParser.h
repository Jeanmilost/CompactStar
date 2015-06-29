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
        *@return 0 on success, otherwise warning (positive value) or error (negative value) code
        */
        virtual QR_Int32 Load(const std::string&  fileName) = 0;
        virtual QR_Int32 Load(const std::wstring& fileName) = 0;

        /**
        * Loads model from buffer
        *@param buffer - buffer
        *@param length - length to read in buffer, in bytes
        *@return 0 on success, otherwise warning (positive value) or error (negative value) code
        *@note Read will begin from current offset
        */
        virtual QR_Int32 Load(      QR_Buffer&            buffer,
                              const QR_Buffer::ISizeType& length) = 0;
};

#endif // QR_ModelParserH

