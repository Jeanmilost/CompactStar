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
#include <vector>

// compactStar engine
#include "CSR_Common.h"

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
        * Grid options
        */
        struct IGridOptions
        {
            TColor      m_BgColor;
            TColor      m_GridColor;
            std::size_t m_Offset;

            IGridOptions();
            virtual ~IGridOptions();
        };

        /**
        * Draws a grid on a device context
        *@param rect - rect surrounding the grid area to fill
        *@param options - grid options
        *@param hDC - device context to paint on
        */
        static void DrawGrid(const TRect& rect, const IGridOptions& options, HDC hDC);

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
