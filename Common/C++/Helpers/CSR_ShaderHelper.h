/****************************************************************************
 * ==> CSR_ShaderHelper ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides an helper class for the shader        *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2022, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#ifndef CSR_ShaderHelperH
#define CSR_ShaderHelperH

// std
#include <string>
#include <vector>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Scene.h"

/**
* Shader helper
*@author Jean-Milost Reymond
*/
class CSR_ShaderHelper
{
    public:
        /**
        * Shader type
        */
        enum IEShaderType
        {
            IE_ST_Color,
            IE_ST_Texture,
            IE_ST_Skybox
        };

        /**
        * Gets a vertex shader
        *@param type - shader type to get
        *@return the vertex shader
        */
        static std::string GetVertexShader(IEShaderType type);

        /**
        * Gets a fragment shader
        *@param type - shader type to get
        *@return the fragment shader
        */
        static std::string GetFragmentShader(IEShaderType type);
};

#endif
