/****************************************************************************
 * ==> TSoundSelection -----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to select an ambient     *
 *               sound                                                      *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2019, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#ifndef TSoundSelectionH
#define TSoundSelectionH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>

// std
#include <string>

/**
* Ambient sound selection
*@author Jean-Milost Reymond
*/
class TSoundSelection : public TForm
{
    __published:
        TPanel *paSound;
        TLabel *laSound;
        TPanel *paSoundFile;
        TLabel *laSoundFileName;
        TPanel *paSoundFileName;
        TEdit *edSoundFileName;
        TButton *btSoundBrowse;
        TPanel *paButtons;
        TButton *btOk;
        TButton *btCancel;
        TBevel *blBottomLine;
        TOpenDialog *odSound;

        void __fastcall btSoundBrowseClick(TObject* pSender);
        void __fastcall btCancelClick(TObject* pSender);
        void __fastcall btOkClick(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - form owner
        *@param defaultDir - application default dir
        */
        __fastcall TSoundSelection(TComponent* pOwner, const std::wstring& initialDir);
};
extern PACKAGE TSoundSelection* SoundSelection;
#endif
