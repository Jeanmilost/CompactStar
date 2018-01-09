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

#include "CSR_Common.h"

// std
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//---------------------------------------------------------------------------
// Memory functions
//---------------------------------------------------------------------------
void* csrMemoryAlloc(void* pMemory, size_t size, size_t count)
{
    // do reallocate a previously existing memory?
    if (!pMemory)
        // no, just allocate the new memory
        return malloc(size * count);

    // yes, reallocate the existing memory to include the new size
    return realloc(pMemory, size * count);
}
//---------------------------------------------------------------------------
// Math functions
//---------------------------------------------------------------------------
void csrMathMin(float a, float b, float* pR)
{
    if (a < b)
    {
        *pR = a;
        return;
    }

    *pR = b;
}
//---------------------------------------------------------------------------
void csrMathMax(float a, float b, float* pR)
{
    if (a > b)
    {
        *pR = a;
        return;
    }

    *pR = b;
}
//---------------------------------------------------------------------------
int csrMathBetween(float value, float rangeStart, float rangeEnd, float tolerance)
{
    float minVal;
    float maxVal;

    csrMathMin(rangeStart, rangeEnd, &minVal);
    csrMathMax(rangeStart, rangeEnd, &maxVal);

    // check if each value is between start and end limits considering tolerance
    if (value >= (minVal - tolerance) && value <= (maxVal + tolerance))
        return 1;

    return 0;
}
//----------------------------------------------------------------------------
void csrMathRound(float value, float* pR)
{
    if (value < 0.0f)
        *pR = ceil(value - 0.5f);
    else
        *pR = floor(value + 0.5f);
}
//----------------------------------------------------------------------------
void csrMathRoundToExp(float value, unsigned exp, float* pR)
{
    const float power = value * pow(10.0f, exp);

    csrMathRound(power, pR);

    *pR *= pow(0.1f, exp);
}
//---------------------------------------------------------------------------
// Buffer functions
//---------------------------------------------------------------------------
CSR_Buffer* csrBufferCreate(void)
{
    // create a new buffer
    CSR_Buffer* pBuffer = (CSR_Buffer*)malloc(sizeof(CSR_Buffer));

    // succeeded?
    if (!pBuffer)
        return 0;

    // initialize the buffer content
    pBuffer->m_pData  = 0;
    pBuffer->m_Length = 0;

    return pBuffer;
}
//---------------------------------------------------------------------------
void csrBufferRelease(CSR_Buffer* pBuffer)
{
    // no buffer to release?
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

    if (!pFileName)
        return 0;

    // create a new buffer
    pBuffer = csrBufferCreate();

    // succeeded?
    if (!pBuffer)
        return 0;

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
        pBuffer->m_pData  = 0;
        pBuffer->m_Length = 0;

        return pBuffer;
    }

    return pBuffer;
}
//---------------------------------------------------------------------------
