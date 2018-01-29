/****************************************************************************
 * ==> CSR_Shader ----------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the shader functions and types        *
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

#ifndef CSR_ShaderH
#define CSR_ShaderH

// compactStar engine
#include "CSR_Common.h"

//---------------------------------------------------------------------------
// Structures
//---------------------------------------------------------------------------

/**
* Shader
*/
typedef struct
{
    GLuint m_ProgramID;
    GLuint m_VertexID;
    GLuint m_FragmentID;
    GLint  m_VertexSlot;
    GLint  m_NormalSlot;
    GLint  m_TexCoordSlot;
    GLint  m_TextureSlot;
    GLint  m_BumpMapSlot;
    GLint  m_ColorSlot;
    GLint  m_ModelSlot;
} CSR_Shader;

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Shader functions
        //-------------------------------------------------------------------

        /**
        * Creates a shader
        *@return newly created shader, 0 on error
        *@note The shader must be released when no longer used, see csrShaderRelease()
        */
        CSR_Shader* csrShaderCreate(void);

        /**
        * Releases a shader
        *@param pShader - shader to release
        */
        void csrShaderRelease(CSR_Shader* pShader);

        /**
        * Initializes a shader structure
        *@param pShader - shader to initialize
        */
        void csrShaderInit(CSR_Shader* pShader);

        /**
        * Loads, compiles and links a shader from vertex and fragment files
        *@param pVertex - vertex shader program file name
        *@param pFragment - fragment shader program file name
        *@return newly created shader, 0 on error
        *@note The shader must be released when no longer used, see csrShaderRelease()
        */
        CSR_Shader* csrShaderLoadFromFile(const char* pVertex, const char* pFragment);

        /**
        * Loads, compiles and links a shader from strings containing the vertex and fragment programs
        *@param pVertex - string containing the vertex shader program to load
        *@param vertexLength - vertex shader program string length
        *@param pFragment - string containing the fragment shader program to load
        *@param fragmentLength - fragment shader program string length
        *@return newly created shader, 0 on error
        *@note The shader must be released when no longer used, see csrShaderRelease()
        */
        CSR_Shader* csrShaderLoadFromStr(const char* pVertex,
                                         size_t      vertexLength,
                                         const char* pFragment,
                                         size_t      fragmentLength);

        /**
        * Loads, compiles and links a shader from vertex and fragment buffers
        *@param pVertex - buffer containing the vertex shader program to load
        *@param pFragment - buffer containing the fragment shader program to load
        *@return newly created shader, 0 on error
        *@note The shader must be released when no longer used, see csrShaderRelease()
        */
        CSR_Shader* csrShaderLoadFromBuffer(const CSR_Buffer* pVertex, const CSR_Buffer* pFragment);

        /**
        * Compiles a shader program
        *@param pSource - source shader program to compile
        *@param shaderType - shader type, parameters are:
        *                    -> GL_VERTEX_SHADER for vertex shader
        *                    -> GL_FRAGMENT_SHADER for fragment shader
        *@param[in, out] pShader - shader that will contain the compiled program
        *@return 1 on success, otherwise 0
        */
        int csrShaderCompile(const CSR_Buffer* pSource, GLenum shaderType, CSR_Shader* pShader);

        /**
        * Links the shader
        *@param[in, out] pShader - shader to link, linked shader if function ends with success
        *@return 1 on success, otherwise 0
        */
        int csrShaderLink(CSR_Shader* pShader);

        /**
        * Enables a shader (i.e. notify that from now this shader will be used)
        *@param pShader - shader to enable, disable any previously enabled shader if 0
        */
        void csrShaderEnable(CSR_Shader* pShader);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Shader.c"
#endif

#endif
