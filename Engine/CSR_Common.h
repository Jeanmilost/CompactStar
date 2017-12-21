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
        // Buffer functions
        //-------------------------------------------------------------------

        /**
        * Creates a new buffer
        *@return newly created buffer, NULL on error
        *@note The ReleaseBuffer() function must be called to release the buffer when it becomes useless
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
        *@return a buffer containing the file content, NULL on error
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
