/****************************************************************************
 * ==> CSR_View ------------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a basic view                          *
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

#include "CSR_View.h"

//---------------------------------------------------------------------------
// CSR_View::IGridOptions
//---------------------------------------------------------------------------
CSR_View::IGridOptions::IGridOptions() :
    m_BgColor(clWhite),
    m_GridColor(clSilver),
    m_Offset(15)
{}
//---------------------------------------------------------------------------
CSR_View::IGridOptions::~IGridOptions()
{}
//---------------------------------------------------------------------------
// CSR_View
//---------------------------------------------------------------------------
CSR_View::CSR_View()
{}
//---------------------------------------------------------------------------
CSR_View::~CSR_View()
{}
//---------------------------------------------------------------------------
void CSR_View::DrawGrid(const TRect& rect, const IGridOptions& options, HDC hDC) const
{
    if (!hDC)
        return;

    HBRUSH hBrush = NULL;
    HPEN   hPen   = NULL;

    try
    {
        // create the brush and pen to use
        hBrush = ::CreateSolidBrush(Graphics::ColorToRGB(options.m_BgColor));
        hPen   = ::CreatePen(PS_DOT, 1, Graphics::ColorToRGB(options.m_GridColor));

        if (!hBrush || !hPen)
            return;

        // select the brush and pen to use
        ::SelectObject(hDC, hBrush);
        ::SelectObject(hDC, hPen);

        ::SetBkMode(hDC, OPAQUE);

        // draw document background
        ::FillRect(hDC, &rect, hBrush);

        // get the view width and height
        const std::size_t width  = rect.Width();
        const std::size_t height = rect.Height();

        // calculate shifting to apply to grid
        const int shiftX = -(options.m_Origin.X % int(options.m_Offset));
        const int shiftY = -(options.m_Origin.Y % int(options.m_Offset));

        ::SetBkMode(hDC, TRANSPARENT);

        // draw horizontal lines
        for (std::size_t i = 0; i < height + options.m_Offset; i += options.m_Offset)
        {
            ::MoveToEx(hDC, rect.Left,  i + shiftY, NULL);
            ::LineTo  (hDC, rect.Right, i + shiftY);
        }

        // draw vertical lines
        for (std::size_t i = 0; i < width + options.m_Offset; i += options.m_Offset)
        {
            ::MoveToEx(hDC, i + shiftX, rect.Top, NULL);
            ::LineTo  (hDC, i + shiftX, rect.Bottom);
        }
    }
    __finally
    {
        if (hPen)
            ::DeleteObject(hPen);

        if (hBrush)
            ::DeleteObject(hBrush);
    }
}
//---------------------------------------------------------------------------
