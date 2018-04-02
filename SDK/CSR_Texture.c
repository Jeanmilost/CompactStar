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

#include "CSR_Texture.h"

// std
#include <stdlib.h>

//---------------------------------------------------------------------------
// Pixel buffer functions
//---------------------------------------------------------------------------
CSR_PixelBuffer* csrPixelBufferCreate(void)
{
    // create a new pixel buffer
    CSR_PixelBuffer* pPB = (CSR_PixelBuffer*)malloc(sizeof(CSR_PixelBuffer));

    // succeeded?
    if (!pPB)
        return 0;

    // initialize the pixel buffer content
    csrPixelBufferInit(pPB);

    return pPB;
}
//---------------------------------------------------------------------------
void csrPixelBufferRelease(CSR_PixelBuffer* pPB)
{
    // no pixel buffer to release?
    if (!pPB)
        return;

    // free the pixel buffer content
    if (pPB->m_pData)
        free(pPB->m_pData);

    // free the pixel buffer
    free(pPB);
}
//---------------------------------------------------------------------------
void csrPixelBufferInit(CSR_PixelBuffer* pPB)
{
    // no pixel buffer to initialize?
    if (!pPB)
        return;

    // initialize the pixel buffer content
    pPB->m_PixelType    = CSR_PT_RGBA;
    pPB->m_Width        = 0;
    pPB->m_Height       = 0;
    pPB->m_Stride       = 0;
    pPB->m_BytePerPixel = 0;
    pPB->m_DataLength   = 0;
    pPB->m_pData        = 0;
}
//---------------------------------------------------------------------------
// Texture shader functions
//---------------------------------------------------------------------------
void csrTextureShaderInit(CSR_TextureShader* pTextureShader)
{
    // no texture shader to initialize?
    if (!pTextureShader)
        return;

    // initialize the texture shader content
    pTextureShader->m_TextureID = M_CSR_Error_Code;
    pTextureShader->m_BumpMapID = M_CSR_Error_Code;
}
//---------------------------------------------------------------------------
// Texture item functions
//---------------------------------------------------------------------------
CSR_TextureItem* csrTextureItemCreate(void)
{
    // create a new texture item
    CSR_TextureItem* pTI = (CSR_TextureItem*)malloc(sizeof(CSR_TextureItem));

    // succeeded?
    if (!pTI)
        return 0;

    // initialize the texture item content
    csrTextureItemInit(pTI);

    return pTI;
}
//---------------------------------------------------------------------------
void csrTextureItemRelease(CSR_TextureItem* pTI)
{
    // no texture item to release?
    if (!pTI)
        return;

    // do release the file name?
    if (pTI->m_pFileName)
        free(pTI->m_pFileName);

    // do release the texture?
    if (pTI->m_pTexture)
        csrPixelBufferRelease(pTI->m_pTexture);

    // do release the texture loaded on the GPU?
    if (pTI->m_TextureID != M_CSR_Error_Code)
        glDeleteTextures(1, &pTI->m_TextureID);
}
//---------------------------------------------------------------------------
void csrTextureItemInit(CSR_TextureItem* pTI)
{
    // no texture item to initialize?
    if (!pTI)
        return;

    // initialize the texture item content
    pTI->m_pFileName = 0;
    pTI->m_pTexture  = 0;
    pTI->m_TextureID = M_CSR_Error_Code;
}
//---------------------------------------------------------------------------
// Texture list functions
//---------------------------------------------------------------------------
CSR_TextureList* csrTextureListCreate(void)
{
    // create a new texture list
    CSR_TextureList* pTL = (CSR_TextureList*)malloc(sizeof(CSR_TextureList));

    // succeeded?
    if (!pTL)
        return 0;

    // initialize the texture list content
    csrTextureListInit(pTL);

    return pTL;
}
//---------------------------------------------------------------------------
void csrTextureListRelease(CSR_TextureList* pTL)
{
    size_t i;

    // no texture list to release?
    if (!pTL)
        return;

    // do free the texture items?
    if (pTL->m_pItem)
    {
        // iterate through texture items to free
        for (i = 0; i < pTL->m_Count; ++i)
            csrTextureItemRelease(&pTL->m_pItem[i]);

        // free the texture items
        free(pTL->m_pItem);
    }

    // Free the texture list
    free(pTL);
}
//---------------------------------------------------------------------------
void csrTextureListInit(CSR_TextureList* pTL)
{
    // no texture list to initialize?
    if (!pTL)
        return;

    // initialize the texture list content
    pTL->m_pItem = 0;
    pTL->m_Count = 0;
}
//---------------------------------------------------------------------------