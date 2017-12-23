/****************************************************************************
 * ==> CSR_Common ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the common functions and declarations *
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

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------
#define M_COMPACTSTAR_VERSION 0.1
#define M_CSR_Epsilon         1.0E-3 // epsilon value used for tolerance

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

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
        // Math functions
        //-------------------------------------------------------------------

        /**
        * Gets the minimum value between 2 values
        *@param a - first value to compare
        *@param b - second value to compare with
        *@param[out] pR - the minimum value between the 2
        */
        void csrMathMin(float a, float b, float* pR);

        /**
        * Gets the maximum value between 2 values
        *@param a - first value to compare
        *@param b - second value to compare with
        *@param[out] pR - the maximum value between the 2
        */
        void csrMathMax(float a, float b, float* pR);

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
        *@note The buffer must be released when it becomes useless. The ReleaseBuffer() function
        *      must be called for this purpose
        */
        CSR_Buffer* csrBufferCreate(void);

        /**
        * Releases a buffer and frees his memory
        *@param pBuffer - buffer to release
        */
        void csrBufferRelease(CSR_Buffer* pBuffer);

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
        *@note The buffer must be released when it becomes useless. The ReleaseBuffer() function
        *      must be called for this purpose
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
