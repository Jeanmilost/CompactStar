/****************************************************************************
 * ==> CSR_VCLHelper -------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides an helper class for the VCL           *
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
#include <Vcl.ComCtrls.hpp>

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
