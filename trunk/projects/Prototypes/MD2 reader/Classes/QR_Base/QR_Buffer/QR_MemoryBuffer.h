/******************************************************************************
 * ==> QR_MemoryBuffer -------------------------------------------------------*
 ******************************************************************************
 * Description : Buffer keeping data in memory                                *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_MemoryBufferH
#define QR_MemoryBufferH

// std
#include <vector>

// qr engine
#include "QR_Buffer.h"

/**
* Buffer keeping data in memory
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_MemoryBuffer : public QR_Buffer
{
    public:
        /**
        * Constructor
        */
        QR_MemoryBuffer();

        /**
        * Destructor
        */
        virtual ~QR_MemoryBuffer();

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
        * Gets memory pointer where data begins
        *@return pointer, NULL if data are empty
        */
        virtual const IDataType* GetPtr() const;

        /**
        * Gets current offset position, in bytes (depends on IDataType)
        *@return current offset position, in bytes
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

    protected:
        std::vector<IDataType> m_Data;
        ISizeType              m_Offset;
};

#endif // QR_MemoryBufferH
