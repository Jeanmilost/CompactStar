/****************************************************************************
 * ==> CSR_DesignerHelper --------------------------------------------------*
 ****************************************************************************
 * Description : This module provides an helper class for the designer      *
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

#ifndef CSR_DesignerHelperH
#define CSR_DesignerHelperH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Forms.hpp>

// std
#include <string>

/**
* Designer helper
*@author Jean-Milost Reymond
*/
class CSR_DesignerHelper
{
    public:
        /**
        * Shader type
        */
        enum IEShaderType
        {
            IE_ST_Color,
            IE_ST_Texture
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

        /**
        * Gets the current application dir
        *@return the current application dir
        */
        static std::wstring GetApplicationDir();

        /**
        * Gets the default scene dir
        *@return the default scene dir
        */
        static std::wstring GetSceneDir();
};

#endif
