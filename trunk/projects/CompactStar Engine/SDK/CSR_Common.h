/****************************************************************************
 * ==> CSR_Common ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the common functions and types        *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2018, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#ifndef CSR_CommonH
#define CSR_CommonH

// std
#include <stddef.h>

// openGL
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include <gles2.h>
    #include <gles2ext.h>
#elif defined(__APPLE__)
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif defined(__CODEGEARC__) || defined(__GNUC__)
    #include <Windows.h>
    #define GLEW_STATIC
    #include <gl/glew.h>
    #include <gl/gl.h>

    // missing in RAD studio OpenGL header
    #define GL_CLAMP_TO_EDGE 0x812F
#endif

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------
#define M_COMPACTSTAR_VERSION 0.1
#define M_CSR_Epsilon         1.0E-3 // epsilon value used for tolerance

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

/**
* Endianness type
*/
typedef enum
{
    CSR_E_LittleEndian,
    CSR_E_BigEndian,
} CSR_EEndianness;

/**
* Memory buffer
*/
typedef struct
{
    unsigned char* m_pData;
    size_t         m_Length;
} CSR_Buffer;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Memory functions
        //-------------------------------------------------------------------

        /**
        * Allocates or reallocates a new block of memory
        *@param pMemory - previous memory block to reallocate, if 0 a new block will be allocated
        *@param size - size of a single item in the memory block, in bytes
        *@param count - number of items the memory block will contain
        *@return newly allocated or reallocated memory block, 0 on error
        *@note If the newly allocated size is smaller than the existing one, the remaining extra
        *      memory will be freed internally
        *@note The memory pointed by pMemory is preserved in case the reallocation failed
        *@note The new memory block should be freed with the free() function when becoming useless
        */
        void* csrMemoryAlloc(void* pMemory, size_t size, size_t count);

        /**
        * Detects if the target system endianness is big or little
        *@return the target system endianness
        */
        CSR_EEndianness csrMemoryEndianness(void);

        /**
        * Swaps the content of a memory from big endian to little endian, or vice-versa
        *@param[in, out] pMemory - memory to swap, swapped memory on function ends
        *@param size - size of the memory to swap
        */
        void csrMemorySwap(void* pMemory, size_t size);

        //-------------------------------------------------------------------
        // Math functions
        //-------------------------------------------------------------------

        /**
        * Gets the minimum value between 2 values
        *@param a - first value to compare
        *@param b - second value to compare with
        *@param[out] pR - the found minimum value
        */
        void csrMathMin(float a, float b, float* pR);

        /**
        * Gets the maximum value between 2 values
        *@param a - first value to compare
        *@param b - second value to compare with
        *@param[out] pR - the found maximum value
        */
        void csrMathMax(float a, float b, float* pR);

        /**
        * Clamps a value between a range
        *@param value - value to clamp
        *@param minVal - min range to clamp
        *@param maxVal - max range to clamp
        *@param[out] pR - the found maximum value
        */
        void csrMathClamp(float value, float minVal, float maxVal, float* pR);

        /**
        * Checks if a value is between a range
        *@param value - value to check
        *@param rangeStart - range start value
        *@param rangeEnd - range end value
        *@param tolerance - tolerance for calculation
        *@return 1 if value is between the range, otherwise 0
        */
        int csrMathBetween(float value, float rangeStart, float rangeEnd, float tolerance);

        /**
        * Rounds a value to the closest integer
        *@param value - value to round
        *@param[out] pR - rounded value
        */
        void csrMathRound(float value, float* pR);

        /**
        * Rounds a value to his next exponent
        *@param value - value to round
        *@param exp - exponent to which the value will be rounded
        *@param[out] pR - rounded value
        */
        void csrMathRoundToExp(float value, unsigned exp, float* pR);

        //-------------------------------------------------------------------
        // Buffer functions
        //-------------------------------------------------------------------

        /**
        * Creates a new buffer
        *@return newly created buffer, 0 on error
        *@note The buffer must be released when no longer used, see csrReleaseBuffer()
        */
        CSR_Buffer* csrBufferCreate(void);

        /**
        * Releases a buffer and frees his memory
        *@param pBuffer - buffer to release
        */
        void csrBufferRelease(CSR_Buffer* pBuffer);

        /**
        * Reads a data from a buffer
        *@param pBuffer - buffer to read from
        *@param[in, out] pOffset - offset to read from, new offset position after function ends
        *@param length - length of one data to read, in bytes
        *@param count - number of data to read in the buffer, in bytes
        *@param[out] pData - read data
        *@return 1 on success, otherwise 0
        */
        int csrBufferRead(const CSR_Buffer* pBuffer,
                                size_t*     pOffset,
                                size_t      length,
                                size_t      count,
                                void*       pData);

        //-------------------------------------------------------------------
        // File functions
        //-------------------------------------------------------------------

        /**
        * Gets the size of a file
        *@param pFileName - file name for which the size should be get
        *@return file size in bytes, 0 on error
        */
        size_t csrFileSize(const char* pFileName);

        /**
        * Opens a file
        *@param pFileName - file name
        *@return a buffer containing the file content, 0 on error
        *@note The buffer must be released when no longer used, see csrReleaseBuffer()
        */
        CSR_Buffer* csrFileOpen(const char* pFileName);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Common.c"
#endif

#endif