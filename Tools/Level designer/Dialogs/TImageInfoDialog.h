/*****************************************************************************
 * ==> TImageInfoDialog -----------------------------------------------------*
 *****************************************************************************
 * Description : This module contains a dialog to show image info            *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#ifndef TImageInfoDialogH
#define TImageInfoDialogH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>

// std
#include <string>

/**
* Image info dialog
*@author Jean-Milost Reymond
*/
class TImageInfoDialog : public TForm
{
    __published:
        TImage *imImage;
        TPanel *paInfo;
        TLabel *laTitle;
        TBevel *blSeparator;
        TPanel *paWidth;
        TLabel *laWidthCaption;
        TLabel *laWidthValue;
        TPanel *paHeight;
        TLabel *laHeightCaption;
        TLabel *laHeightValue;
        TPanel *paBPP;
        TLabel *laBPPCaption;
        TLabel *laBPPValue;
        TPanel *paButtons;
        TButton *btClose;
        TPanel *paFileName;
        TLabel *laFileNameCaption;
        TLabel *laFileNameValue;
        TPanel *paImageType;
        TLabel *laImageTypeCaption;
        TLabel *laImageTypeValue;

        void __fastcall btCloseClick(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - form owner
        *@param pPicture - picture containing the image for which the info should be get
        *@param fileName - image file name
        */
        __fastcall TImageInfoDialog(      TComponent*   pOwner,
                                          TPicture*     pPicture,
                                    const std::wstring& fileName);
};
extern PACKAGE TImageInfoDialog *ImageInfoDialog;
#endif
