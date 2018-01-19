/****************************************************************************
 * ==> CSR_Texture ---------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the texture functions and types       *
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

#ifndef CSR_TextureH
#define CSR_TextureH

// compactStar engine
#include "CSR_Common.h"


// Structures
//---------------------------------------------------------------------------

/**
* Pixel type
*/
typedef enum
{
    CSR_PT_RGB,
    CSR_PT_BGR,
    CSR_PT_RGBA,
    CSR_PT_BGRA,
    CSR_PT_ARGB,
    CSR_PT_ABGR,
} CSR_EPixelType;

/**
* Pixel buffer
*/
typedef struct
{
    CSR_EPixelType m_PixelType;
    unsigned       m_Width;
    unsigned       m_Height;
    unsigned       m_Stride;
    unsigned       m_BytePerPixel;
    size_t         m_DataLength;
    void*          m_pData;
} CSR_PixelBuffer;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Pixel buffer functions
        //-------------------------------------------------------------------

        /**
        * Creates a pixel buffer
        *@return newly created pixel buffer, 0 on error
        *@note The pixel buffer must be released when no longer used, see csrPixelBufferRelease()
        */
        CSR_PixelBuffer* csrPixelBufferCreate(void);

        /**
        * Releases a pixel buffer
        *@param pPB - pixel buffer to release
        */
        void csrPixelBufferRelease(CSR_PixelBuffer* pPB);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Texture.c"
#endif

#endif