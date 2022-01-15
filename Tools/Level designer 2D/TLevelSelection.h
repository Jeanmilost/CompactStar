/****************************************************************************
 * ==> TLevelSelection -----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to save the landscape    *
 *               level into a file                                          *
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

#ifndef TLevelSelectionH
#define TLevelSelectionH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>

// std
#include <string>

/**
* Dialog box to save the landscape level into a file
*@author Jean-Milost Reymond
*/
class TLevelSelection : public TForm
{
    __published:
        TLabel *laFileName;
        TPanel *paButtons;
        TButton *btOk;
        TButton *btCancel;
        TBevel *blBottomLine;
        TPanel *paLevel;
        TLabel *laFileNameTitle;
        TPanel *paFileName;
        TEdit *edFileName;
        TButton *btBrowse;
        TLabel *laOptions;
        TBevel *blTopLine;
        TCheckBox *ckSaveFileContent;
        TSaveDialog *sdSave;

        void __fastcall btBrowseClick(TObject* pSender);
        void __fastcall btCancelClick(TObject* pSender);
        void __fastcall btOkClick(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - form owner
        *@param defaultDir - application default dir
        */
        __fastcall TLevelSelection(TComponent* pOwner, const std::wstring& initialDir);
};
extern PACKAGE TLevelSelection* LevelSelection;
#endif
