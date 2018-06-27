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

#ifndef CSR_VCLHelperH
#define CSR_VCLHelperH

// vcl
#include <Vcl.Controls.hpp>
#include <Vcl.ComCtrls.hpp>

// std
#include <string>
#include <vector>

/**
* VCL control hook
*@author Jean-Milost Reymond
*/
class CSR_VCLControlHook
{
    public:
        /**
        * Called when a hooked control receives a Windows message
        *@param pControl - hooked control
        *@param message - Windows message
        *@param fCtrlOriginalProc - control original Windows procedure
        *@return true if the message was resolved and should no longer be handled, otherwise false
        */
        typedef bool (__closure *ITfOnMessage)(TControl*  pControl,
                                               TMessage&  message,
                                               TWndMethod fCtrlOriginalProc);

        /**
        * Constructor
        *@param pControl - control to hook
        *@param fOnMessage - callback function to call when a Windows message is received
        */
        CSR_VCLControlHook(TControl* pControl, ITfOnMessage fOnMessage);

        virtual ~CSR_VCLControlHook();

    private:
        TControl*    m_pControl;
        ITfOnMessage m_fOnMessage;
        TWndMethod   m_fCtrlOriginalProc;

        /**
        * Hooked control main procedure
        *@param message- Windows procedure message
        */
        void __fastcall ControlWndProc(TMessage& message);
};

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

        typedef std::vector<TControl*> IControls;

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

        /**
        * Applies a Full-Scene AntiAliasing (FSAA) on a bitmap
        *@param pSource - source bitmap on which the antialiasing should be applied
        *@param[in, out] pDest - destination bitmap containing the antialiased image
        *@param factor - antialiasing factor
        *@note The resulting destination image will be smaller than the source in a ratio defined by
        *      the factor parameter. For example, a source image of 400x400 pixels, on which an
        *      antialiasing factor of 4 is applied, will result to an image of 100x100 pixels
        */
        static void ApplyAntialiasing(TBitmap* pSource, TBitmap* pDest, std::size_t factor);

        /**
        * Distributes the controls horizontally, from first on the left to last on the right
        *@param controls - controls to distribute
        */
        static void DistributeCtrlsLTR(const IControls& controls);

        /**
        * Distributes the controls horizontally, from first on the right to last on the left
        *@param controls - controls to distribute
        */
        static void DistributeCtrlsRTL(const IControls& controls);

        /**
        * Distributes the controls vertically, from first on the top to last on the bottom
        *@param controls - controls to distribute
        */
        static void DistributeCtrlsTTB(const IControls& controls);

        /**
        * Converts a string to a wstring (using the VCL functions)
        *@param str - string to convert
        *@return converted string
        */
        static std::wstring StrToWStr(const std::string& str);

        /**
        * Converts a wstring to a string (using the VCL functions)
        *@param str - string to convert
        *@return converted string
        */
        static std::string WStrToStr(const std::wstring& str);
};

#endif
