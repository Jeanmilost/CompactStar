/****************************************************************************
 * ==> CSR_Serializer ------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the functions required for the        *
 *               serialization of models and scenes                         *
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

#ifndef CSR_SerializerH
#define CSR_SerializerH

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_Renderer.h"

//---------------------------------------------------------------------------
// Enumerators
//---------------------------------------------------------------------------

/**
* File header type
*/
typedef enum
{
    CSR_HT_VertexFormat = 0,
    CSR_HT_VertexCulling,
    CSR_HT_VertexMaterial
} CSR_EHeaderType;

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

/**
* Scene file header
*/
typedef struct
{
    CSR_EHeaderType m_HeaderType;
    unsigned        m_Length;
    unsigned        m_Version;
    unsigned        m_DataCount;
    unsigned        m_DataLength;
} CSR_SceneFileHeader;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Write functions
        //-------------------------------------------------------------------

        /**
        * Writes a vertex buffer inside a buffer
        *@param pVB - vertex buffer to write
        *@param[in, out] pBuffer - buffer to write in
        *@return 1 on success, otherwise 0
        */
        int csrSerializerWriteVB(const CSR_VertexBuffer* pVB, CSR_Buffer* pBuffer);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Serializer.c"
#endif

#endif