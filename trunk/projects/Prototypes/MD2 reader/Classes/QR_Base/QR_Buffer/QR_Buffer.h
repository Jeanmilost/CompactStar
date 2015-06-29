/******************************************************************************
 * ==> QR_Buffer -------------------------------------------------------------*
 ******************************************************************************
 * Description : Buffer interface. Buffers can contain binary data, text, ... *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_BufferH
#define QR_BufferH

// qr engine
#include "QR_Types.h"

/**
* Buffer interface. Buffers can contain binary data, text, ...
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_Buffer
{
    public:
        typedef QR_SizeT ISizeType;
        typedef QR_Char  IDataType;

        /**
        * Constructor
        */
        QR_Buffer();

        /**
        * Copy constructor
        *@param other - other buffer to copy from
        *@note Internal offset will be seek to 0 after copy
        */
        QR_Buffer(QR_Buffer& other);

        /**
        * Destructor
        */
        virtual ~QR_Buffer();

        /**
        * Copy operator
        *@param other - other buffer to copy from
        *@return this buffer
        *@note Current buffer content will be completely replaced by new
        *@note Internal offset will be seek to 0 after copy
        */
        virtual QR_Buffer& operator =(QR_Buffer& other);

        /**
        * Copies buffer content from another buffer
        *@param other - other buffer to copy from
        *@note Current buffer content will be completely replaced by new
        *@note Internal offset will be seek to 0 after copy
        */
        virtual void Copy(QR_Buffer& other);

        /**
        * Copies buffer content from another buffer
        *@param pOther - other buffer to copy from
        *@note Current buffer content will be completely replaced by new
        *@note Internal offset will be seek to 0 after copy
        */
        virtual void Copy(QR_Buffer* pOther);

        /**
        * Clears buffer completely
        */
        virtual void Clear() = 0;

        /**
        * Checks if buffer is empty
        *@return true if buffer is empty, otherwise false
        */
        virtual bool Empty() = 0;

        /**
        * Gets current offset position, in bytes (depends on IDataType)
        *@return current offset position, in bytes
        */
        virtual ISizeType GetOffset() const = 0;

        /**
        * Gets data size, in bytes (depends on IDataType)
        *@return data size, in bytes
        */
        virtual ISizeType GetSize() const = 0;

        /**
        * Seeks offset
        *@param start - absolute start offset to seek from
        *@param delta - number of bytes to seek from start offset, must be positive
        *@return new offset position
        */
        virtual ISizeType Seek(const ISizeType& start, const ISizeType& delta) = 0;

        /**
        * Reads data from buffer
        *@param pBuffer - destination buffer that will receive the read data
        *@param length - length to read in source buffer
        *@return read data length
        *@note Data will be read from current offset
        */
        virtual ISizeType Read(void* pBuffer, const ISizeType& length) = 0;

        /**
        * Writes data into buffer
        *@param pBuffer - source buffer to write from
        *@param length - length to write from source buffer
        *@return written data length
        *@note Data will be written from current offset
        */
        virtual ISizeType Write(void* pBuffer, const ISizeType& length) = 0;

        /**
        * Reads buffer content as string
        *@return buffer content as string
        */
        virtual std::string ToStr() = 0;
};

#endif // QR_BUFFER_H
