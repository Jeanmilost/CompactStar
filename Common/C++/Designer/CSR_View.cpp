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

#include "CSR_View.h"

//---------------------------------------------------------------------------
// CSR_View::IGridOptions
//---------------------------------------------------------------------------
CSR_View::IGridOptions::IGridOptions() :
    m_Offset(15)
{}
//---------------------------------------------------------------------------
CSR_View::IGridOptions::~IGridOptions()
{}
//---------------------------------------------------------------------------
// CSR_View
//---------------------------------------------------------------------------
CSR_View::CSR_View() :
    m_BgColor(clWhite),
    m_GridColor(clSilver),
    m_hBrush(NULL),
    m_hPen(NULL)
{
    // create the default brush and pen
    m_hBrush = ::CreateSolidBrush(Graphics::ColorToRGB(m_BgColor));
    m_hPen   = ::CreatePen(PS_DOT, 1, Graphics::ColorToRGB(m_GridColor));
}
//---------------------------------------------------------------------------
CSR_View::~CSR_View()
{
    if (m_hPen)
        ::DeleteObject(m_hPen);

    if (m_hBrush)
        ::DeleteObject(m_hBrush);
}
//---------------------------------------------------------------------------
TColor CSR_View::GetBgColor() const
{
    return m_BgColor;
}
//---------------------------------------------------------------------------
void CSR_View::SetBgColor(TColor color)
{
    if (m_BgColor == color)
        return;

    m_BgColor = color;

    if (m_hBrush)
        ::DeleteObject(m_hBrush);

    m_hBrush = ::CreateSolidBrush(Graphics::ColorToRGB(m_BgColor));
}
//---------------------------------------------------------------------------
TColor CSR_View::GetGridColor() const
{
    return m_GridColor;
}
//---------------------------------------------------------------------------
void CSR_View::SetGridColor(TColor color)
{
    if (m_GridColor == color)
        return;

    m_GridColor = color;

    if (m_hPen)
        ::DeleteObject(m_hPen);

    m_hPen = ::CreatePen(PS_DOT, 1, Graphics::ColorToRGB(m_GridColor));
}
//---------------------------------------------------------------------------
void CSR_View::DrawGrid(const TRect& rect, const IGridOptions& options, HDC hDC) const
{
    if (!hDC)
        return;

    if (!m_hBrush || !m_hPen)
        return;

    // select the brush and pen to use
    ::SelectObject(hDC, m_hBrush);
    ::SelectObject(hDC, m_hPen);

    // select the background mode to apply
    ::SetBkMode(hDC, OPAQUE);

    // draw document background
    ::FillRect(hDC, &rect, m_hBrush);

    // get the dot width
    const int dotWidth = 6;

    // get the view width and height
    const std::size_t width  = rect.Width();
    const std::size_t height = rect.Height();

    // calculate shifting to apply to grid
    const int shiftX = -(options.m_Origin.X % int(options.m_Offset));
    const int shiftY = -(options.m_Origin.Y % int(options.m_Offset));

    // calculate the value to apply to the lines to keep the grid coherent with the scrolling
    const int scrollX = options.m_Origin.X % dotWidth;
    const int scrollY = options.m_Origin.Y % dotWidth;

    ::SetBkMode(hDC, TRANSPARENT);

    // draw horizontal lines
    for (std::size_t i = 0; i < height + options.m_Offset; i += options.m_Offset)
    {
        ::MoveToEx(hDC, (rect.Left  - dotWidth) - scrollX, i + shiftY, NULL);
        ::LineTo  (hDC, (rect.Right + dotWidth) - scrollX, i + shiftY);
    }

    // draw vertical lines
    for (std::size_t i = 0; i < width + options.m_Offset; i += options.m_Offset)
    {
        ::MoveToEx(hDC, i + shiftX, (rect.Top    - dotWidth) - scrollY, NULL);
        ::LineTo  (hDC, i + shiftX, (rect.Bottom + dotWidth) - scrollY);
    }
}
//---------------------------------------------------------------------------
