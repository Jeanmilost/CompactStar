/****************************************************************************
 * ==> CSR_DesignerViewHelper ----------------------------------------------*
 ****************************************************************************
 * Description : This module provides an helper class for the designer      *
 *               views                                                      *
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

#ifndef CSR_DesignerViewHelperH
#define CSR_DesignerViewHelperH

// vcl
#include <Vcl.Graphics.hpp>

/**
* Designer view helper
*@author Jean-Milost Reymond
*/
class CSR_DesignerViewHelper
{
    public:
        struct IGridOptions
        {
            TColor      m_BgColor;
            TColor      m_GridColor;
            std::size_t m_Offset;

            IGridOptions();
            virtual ~IGridOptions();
        };

        static void DrawGrid(const TRect& rect, const IGridOptions& options, HDC hDC);
};

#endif
