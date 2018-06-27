/****************************************************************************
 * ==> CSR_VCLHelper -------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides helper classes to deal with the VCL   *
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

#include "CSR_VCLHelper.h"

// vcl
#include <Vcl.Imaging.Jpeg.hpp>
#include <Vcl.Imaging.PngImage.hpp>
#include <Vcl.ComCtrls.hpp>

//---------------------------------------------------------------------------
// CSR_VCLControlHook
//---------------------------------------------------------------------------
CSR_VCLControlHook::CSR_VCLControlHook(TControl* pControl, ITfOnMessage fOnMessage) :
    m_pControl(pControl),
    m_fOnMessage(fOnMessage),
    m_fCtrlOriginalProc(NULL)
{
    // hook the control
    m_fCtrlOriginalProc    = m_pControl->WindowProc;
    m_pControl->WindowProc = ControlWndProc;
}
//---------------------------------------------------------------------------
CSR_VCLControlHook::~CSR_VCLControlHook()
{
    m_fOnMessage = NULL;

    if (m_fCtrlOriginalProc)
        m_pControl->WindowProc = m_fCtrlOriginalProc;
}
//---------------------------------------------------------------------------
void __fastcall CSR_VCLControlHook::ControlWndProc(TMessage& message)
{
    if (m_fOnMessage && m_fOnMessage(m_pControl, message, m_fCtrlOriginalProc))
        return;

    m_fCtrlOriginalProc(message);
}
//---------------------------------------------------------------------------
// CSR_VCLHelper
//---------------------------------------------------------------------------
bool CSR_VCLHelper::IsVisible(TControl* pControl)
{
    // is control valid and visible?
    if (!pControl || !pControl->Visible)
        return false;

    // if control is a tab sheet, it will be visible only if it is an active tabsheet
    if (pControl->InheritsFrom(__classid(TTabSheet)))
    {
        // get parent page control
        TPageControl* pPageControl = dynamic_cast<TPageControl*>(pControl->Parent);

        // check if tab sheet is active
        if (pPageControl && pPageControl->ActivePage != pControl)
            return false;
    }

    // check if parents are visible too
    return (!pControl->Parent || CSR_VCLHelper::IsVisible(pControl->Parent));
}
//---------------------------------------------------------------------------
void CSR_VCLHelper::ChangeTabsVisibility(TPageControl* pPageControl, TTabSheet* pActivePage, bool show)
{
    if (!pPageControl)
        return;

    // change the visibility of all tabs
    for (int i = 0; i < pPageControl->PageCount; ++i)
        pPageControl->Pages[i]->TabVisible = show;

    // select the default page to show
    pPageControl->ActivePage = pActivePage;
}
//---------------------------------------------------------------------------
CSR_VCLHelper::IEImageType CSR_VCLHelper::GetImageType(TPicture* pPicture)
{
    if (dynamic_cast<TBitmap*>(pPicture->Graphic))
        return IE_IT_Bitmap;

    if (dynamic_cast<TJPEGImage*>(pPicture->Graphic))
        return IE_IT_JPEG;

    if (dynamic_cast<TPngImage*>(pPicture->Graphic))
        return IE_IT_PNG;

    return IE_IT_Unknown;
}
//---------------------------------------------------------------------------
std::wstring CSR_VCLHelper::ImageTypeToStr(IEImageType type)
{
    switch (type)
    {
        case IE_IT_Bitmap: return L"Bitmap";
        case IE_IT_JPEG:   return L"JPEG";
        case IE_IT_PNG:    return L"PNG";
        default:           return L"Unknown";
    }
}
//---------------------------------------------------------------------------
unsigned CSR_VCLHelper::GetBitPerPixel(TPicture* pPicture)
{
    // no picture?
    if (!pPicture)
        return 0;

    // picture contains a bitmap?
    if (dynamic_cast<TBitmap*>(pPicture->Graphic))
        switch (pPicture->Bitmap->PixelFormat)
        {
            case pf1bit:   return 1;
            case pf4bit:   return 4;
            case pf8bit:   return 8;
            case pf15bit:  return 15;
            case pf16bit:  return 16;
            case pf24bit:  return 24;
            case pf32bit:  return 32;
            default:       return 0;
        }

    // picture contains a JPG?
    if (dynamic_cast<TJPEGImage*>(pPicture->Graphic))
        return 24;

    // picture contains a PNG?
    if (dynamic_cast<TPngImage*>(pPicture->Graphic))
    {
        TPngImage* pPng = static_cast<TPngImage*>(pPicture->Graphic);

        switch (pPng->Header->ColorType)
        {
            case COLOR_GRAYSCALEALPHA: return pPng->Header->BitDepth * 2;
            case COLOR_RGB:            return pPng->Header->BitDepth * 3;
            case COLOR_RGBALPHA:       return pPng->Header->BitDepth * 4;
            case COLOR_GRAYSCALE:
            case COLOR_PALETTE:        return pPng->Header->BitDepth;
            default:                   return 0;
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
void CSR_VCLHelper::ApplyAntialiasing(TBitmap* pSource, TBitmap* pDest, std::size_t factor)
{
    // no source bitmap?
    if (!pSource)
        return;

    // no destination bitmap?
    if (!pDest)
        return;

    // configure destination bitmap
    pDest->PixelFormat = pSource->PixelFormat;
    pDest->AlphaFormat = pSource->AlphaFormat;
    pDest->SetSize(pSource->Width / factor, pSource->Height / factor);

    // set stretch mode to half tones (thus resizing will be smooth)
    const int prevMode = ::SetStretchBltMode(pDest->Canvas->Handle, HALFTONE);

    try
    {
        // apply antialiasing on the destination image
        ::StretchBlt(pDest->Canvas->Handle,
                     0,
                     0,
                     pDest->Width,
                     pDest->Height,
                     pSource->Canvas->Handle,
                     0,
                     0,
                     pSource->Width,
                     pSource->Height,
                     SRCCOPY);
    }
    __finally
    {
        // restore previous stretch blit mode
        ::SetStretchBltMode(pDest->Canvas->Handle, prevMode);
    }
}
//---------------------------------------------------------------------------
void CSR_VCLHelper::DistributeCtrlsLTR(const IControls& controls)
{
    int pos = 0;

    // iterate through controls to distribute
    for (std::size_t i = 0; i < controls.size(); ++i)
    {
        // is control visible?
        if (!controls[i]->Visible)
            continue;

        // place the control on the left
        controls[i]->Left = pos;

        // calculate the next position
        pos += controls[i]->Margins->ControlWidth;
    }
}
//---------------------------------------------------------------------------
void CSR_VCLHelper::DistributeCtrlsRTL(const IControls& controls)
{
    // iterate through controls to distribute
    for (std::size_t i = 0; i < controls.size(); ++i)
    {
        // is control visible?
        if (!controls[i]->Visible)
            continue;

        // place the control on the right
        controls[i]->Left = 0;
    }
}
//---------------------------------------------------------------------------
void CSR_VCLHelper::DistributeCtrlsTTB(const IControls& controls)
{
    int pos = 0;

    // iterate through controls to distribute
    for (std::size_t i = 0; i < controls.size(); ++i)
    {
        // is control visible?
        if (!controls[i]->Visible)
            continue;

        // place the control on the left
        controls[i]->Top = pos;

        // calculate the next position
        pos += controls[i]->Margins->ControlHeight;
    }
}
//---------------------------------------------------------------------------
std::wstring CSR_VCLHelper::StrToWStr(const std::string& str)
{
    return UnicodeString(AnsiString(str.c_str())).c_str();
}
//---------------------------------------------------------------------------
std::string CSR_VCLHelper::WStrToStr(const std::wstring& str)
{
    return AnsiString(UnicodeString(str.c_str())).c_str();
}
//---------------------------------------------------------------------------
