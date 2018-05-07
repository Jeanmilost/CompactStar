/*****************************************************************************
 * ==> TImageDuplicateDialog ------------------------------------------------*
 *****************************************************************************
 * Description : This module contains a dialog that allows the user to       *
 *               select between 2 image duplicates                           *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#ifndef TImageDuplicateDialogH
#define TImageDuplicateDialogH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>

/**
* Dialog that allow the user to select between 2 image duplicates
*@author Jean-Milost Reymond
*/
class TImageDuplicateDialog : public TForm
{
    __published:
        TPanel *paLeft;
        TPanel *paRight;
        TImage *imCurrent;
        TPanel *paButtons;
        TButton *btOk;
        TLabel *laCaption;
        TImage *imNew;
        TLabel *laCurrent;
        TLabel *laNew;
        TRadioGroup *rgAction;
        TLabel *laFileName;
        TLabel *laDuplicatePrefix;
        TEdit *edDuplicatePrefix;

        void __fastcall rgActionClick(TObject* pSender);
        void __fastcall btOkClick(TObject* pSender);

    public:
        __fastcall TImageDuplicateDialog(TComponent* pOwner);
};
extern PACKAGE TImageDuplicateDialog* ImageDuplicateDialog;
#endif
