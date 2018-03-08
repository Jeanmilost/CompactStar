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

#pragma hdrstop
#include "CSR_DesignerViewHelper.h"

//---------------------------------------------------------------------------
// CSR_DesignerViewHelper::IGridOptions
//---------------------------------------------------------------------------
CSR_DesignerViewHelper::IGridOptions::IGridOptions() :
    m_BgColor(clWhite),
    m_GridColor(clSilver),
    m_Offset(15)
{}
//---------------------------------------------------------------------------
CSR_DesignerViewHelper::IGridOptions::~IGridOptions()
{}
//---------------------------------------------------------------------------
// CSR_DesignerViewHelper
//---------------------------------------------------------------------------
void CSR_DesignerViewHelper::DrawGrid(const TRect& rect, const IGridOptions& options, HDC hDC)
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

        const std::size_t width  = rect.Width();
        const std::size_t height = rect.Height();

        ::SetBkMode(hDC, TRANSPARENT);

        // draw horizontal lines
        for (std::size_t i = 0; i < height; i += options.m_Offset)
        {
            ::MoveToEx(hDC, rect.Left,  i, NULL);
            ::LineTo  (hDC, rect.Right, i);
        }

        // draw vertical lines
        for (std::size_t i = 0; i < width; i += options.m_Offset)
        {
            ::MoveToEx(hDC, i, rect.Top, NULL);
            ::LineTo  (hDC, i, rect.Bottom);
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
