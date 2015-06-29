/******************************************************************************
 * ==> QR_FileBuffer ---------------------------------------------------------*
 ******************************************************************************
 * Description : File buffer interface                                        *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_FileBufferH
#define QR_FileBufferH

// std
#include <string>

// qr engine
#include "QR_Buffer.h"

/**
* File buffer interface
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_FileBuffer : public QR_Buffer
{
    public:
        /**
        * Constructor
        */
        QR_FileBuffer();

        /**
        * Destructor
        */
        virtual ~QR_FileBuffer();

        /**
        * Buffer read/write mode
        */
        enum IEMode
        {
            IE_M_Unknown = 0,
            IE_M_Read,
            IE_M_Write,
            IE_M_RW,
        };

        /**
        * Opens file in specified mode
        *@param fileName - file name
        *@param mode - opening mode
        *@return true on success, otherwise false
        */
        virtual bool Open(const std::string&  fileName, IEMode mode);
        virtual bool Open(const std::wstring& fileName, IEMode mode);

    protected:
        IEMode m_Mode;

        /**
        * Closes file
        */
        virtual void Close() = 0;
};

#endif // QR_FileBufferH
