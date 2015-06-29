/******************************************************************************
 * ==> QR_StdFileBuffer ------------------------------------------------------*
 ******************************************************************************
 * Description : File buffer based on std library                             *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_StdFileBufferH
#define QR_StdFileBufferH

// std
#include <cstdio>

// qr engine
#include "QR_Types.h"
#include "QR_FileBuffer.h"

/**
* File buffer based on std library
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_StdFileBuffer : public QR_FileBuffer
{
    public:
        /**
        * Constructor
        */
        QR_StdFileBuffer();

        /**
        * Destructor
        */
        virtual ~QR_StdFileBuffer();

        /**
        * Opens file in specified mode
        *@param fileName - file name
        *@param mode - opening mode
        *@return true on success, otherwise false
        */
        virtual bool Open(const std::string&  fileName, IEMode mode);
        virtual bool Open(const std::wstring& fileName, IEMode mode);

        /**
        * Clears buffer completely
        */
        virtual void Clear();

        /**
        * Checks if buffer is empty
        *@return true if buffer is empty, otherwise false
        */
        virtual bool Empty();

        /**
        * Gets current offset position, in bytes (depends on IDataType)
        *@return current offset position, in bytes, -1L on error
        */
        virtual ISizeType GetOffset() const;

        /**
        * Gets data size, in bytes (depends on IDataType)
        *@return data size, in bytes
        */
        virtual ISizeType GetSize() const;

        /**
        * Seeks offset
        *@param start - absolute start offset to seek from
        *@param delta - number of bytes to seek from start offset, must be positive
        *@return new offset position
        */
        virtual ISizeType Seek(const ISizeType& start, const ISizeType& delta);

        /**
        * Reads data from buffer
        *@param pBuffer - destination buffer that will receive the read data
        *@param length - length to read in source buffer
        *@return read data length
        *@note Data will be read from current offset
        */
        virtual ISizeType Read(void* pBuffer, const ISizeType& length);

        /**
        * Writes data into buffer
        *@param pBuffer - source buffer to write from
        *@param length - length to write from source buffer
        *@return written data length
        *@note Data will be written from current offset
        */
        virtual ISizeType Write(void* pBuffer, const ISizeType& length);

        /**
        * Reads buffer content as string
        *@return buffer content as string
        */
        virtual std::string ToStr();

    protected:
        std::FILE* m_FileBuffer;

        /**
        * Closes file
        */
        virtual void Close();
};

#endif // QR_StdFileBufferH
