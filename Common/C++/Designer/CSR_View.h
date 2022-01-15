/****************************************************************************
 * ==> CSR_View ------------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a basic view                          *
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

#ifndef CSR_ViewH
#define CSR_ViewH

// vcl
#include <Vcl.Graphics.hpp>

/**
* Basic view
*@author Jean-Milost Reymond
*/
class CSR_View
{
    public:
        /**
        * Grid options
        */
        struct IGridOptions
        {
            TPoint      m_Origin;
            std::size_t m_Offset;

            IGridOptions();
            virtual ~IGridOptions();
        };

        CSR_View();
        virtual ~CSR_View();

        /**
        * Gets the background color
        *@return the background color
        */
        TColor GetBgColor() const;

        /**
        * Sets the background color
        *@param color - the background color
        */
        void SetBgColor(TColor color);

        /**
        * Gets the grid color
        *@return the grid color
        */
        TColor GetGridColor() const;

        /**
        * Sets the grid color
        *@param color - the grid color
        */
        void SetGridColor(TColor color);

    protected:
        /**
        * Draws a grid on a device context
        *@param rect - rect surrounding the grid area to fill
        *@param options - grid options
        *@param hDC - device context to paint on
        */
        virtual void DrawGrid(const TRect& rect, const IGridOptions& options, HDC hDC) const;

    private:
        TColor m_BgColor;
        TColor m_GridColor;
        HBRUSH m_hBrush;
        HPEN   m_hPen;
};

#endif
