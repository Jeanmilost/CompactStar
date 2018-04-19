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

#ifndef CSR_VCLHelperH
#define CSR_VCLHelperH

// vcl
#include <Vcl.Controls.hpp>
#include <Vcl.ComCtrls.hpp>

// std
#include <string>

/**
* Helper class for the VCL
*@author Jean-Milost Reymond
*/
class CSR_VCLHelper
{
    public:
        /**
        * Image types
        */
        enum IEImageType
        {
            IE_IT_Unknown = 0,
            IE_IT_Bitmap,
            IE_IT_JPEG,
            IE_IT_PNG
        };

        /**
        * Checks if a control is visible, considering also all his parents
        *@param pControl - control to check
        *@return true if the control is visible, otherwise false
        */
        static bool IsVisible(TControl* pControl);

        /**
        * Changes the visibility of all page control tabs, and select the default tab
        *@param pPageControl - page control on which the tabs visibility should be changed
        *@param pActivePage - active page to show, if NULL no page will be selected
        *@param show - if true all the tabs will be shown, hidden otherwise
        */
        static void ChangeTabsVisibility(TPageControl* pPageControl, TTabSheet* pActivePage, bool show);

        /**
        * Gets the type of an image contained in a picture
        *@param pPicture - picture containing the image for which the type should be get
        *@return picture image type
        */
        static IEImageType GetImageType(TPicture* pPicture);

        /**
        * Converts an image type to a human readable type
        *@param type - image ty to convert
        *@return image type as string
        */
        static std::wstring ImageTypeToStr(IEImageType type);

        /**
        * Gets the bit per pixels of an image contained in a picture
        *@param pPicture - picture containing the image for which the byte per pixels should be get
        *@return the byte per pixels, 0 if not found or on error
        */
        static unsigned GetBitPerPixel(TPicture* pPicture);
};

#endif
