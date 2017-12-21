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

#include "CSR_Common.h"

// std
#include <stdlib.h>
#include <stdio.h>

//---------------------------------------------------------------------------
// Buffer functions
//---------------------------------------------------------------------------
CSR_Buffer* csrBufferCreate(void)
{
    CSR_Buffer* pBuffer = NULL;

    // create a new buffer
    pBuffer = (CSR_Buffer*)malloc(sizeof(CSR_Buffer));

    // succeeded?
    if (!pBuffer)
        return NULL;

    // initialize the buffer content
    pBuffer->m_pData  = NULL;
    pBuffer->m_Length = 0;

    return pBuffer;
}
//---------------------------------------------------------------------------
void csrBufferRelease(CSR_Buffer* pBuffer)
{
    // no buffer to free?
    if (!pBuffer)
        return;

    // free the buffer content
    if (pBuffer->m_pData)
        free(pBuffer->m_pData);

    // free the buffer
    free(pBuffer);
}
//---------------------------------------------------------------------------
// File functions
//---------------------------------------------------------------------------
size_t csrFileSize(const char* pFileName)
{
    FILE*  pFile;
    size_t fileSize;

    // open the file
    pFile = fopen(pFileName, "rb");

    // succeeded?
    if (!pFile)
        return 0;

    // measure the file size
    fseek(pFile, 0, SEEK_END);
    fileSize = ftell(pFile);
    fclose(pFile);

    return fileSize;
}
//---------------------------------------------------------------------------
CSR_Buffer* csrFileOpen(const char* pFileName)
{
    FILE*       pFile;
    CSR_Buffer* pBuffer;
    size_t      bytesRead;

    // create a new buffer
    pBuffer = csrBufferCreate();

    // succeeded?
    if (!pBuffer)
        return NULL;

    // open the file
    pFile = fopen(pFileName, "rb");

    // succeeded?
    if (!pFile)
        return pBuffer;

    // measure the file size
    fseek(pFile, 0, SEEK_END);
    pBuffer->m_Length = ftell(pFile);

    // is file empty?
    if (!pBuffer->m_Length)
    {
        fclose(pFile);
        return pBuffer;
    }

    // allocate memory for the file content
    pBuffer->m_pData = (unsigned char*)malloc(pBuffer->m_Length + 1);

    // succeeded?
    if (!pBuffer->m_pData)
    {
        fclose(pFile);
        pBuffer->m_Length = 0;
        return pBuffer;
    }

    // certify that the buffer content is well terminated (useful in case the data is a text)
    pBuffer->m_pData[pBuffer->m_Length] = '\0';

    // reset cursor position on the file start
    fseek(pFile, 0, SEEK_SET);

    // read the whole file content
    bytesRead = fread(pBuffer->m_pData, 1, pBuffer->m_Length, pFile);

    // close the file
    fclose(pFile);

    // file was read successfully?
    if (bytesRead != pBuffer->m_Length)
    {
        // clear the buffer
        free(pBuffer->m_pData);
        pBuffer->m_pData  = NULL;
        pBuffer->m_Length = 0;

        return pBuffer;
    }

    return pBuffer;
}
//---------------------------------------------------------------------------
