/****************************************************************************
 * ==> TBoxSelection -------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to configure a box on    *
 *               the landscape                                              *
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

#ifndef TBoxSelectionH
#define TBoxSelectionH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TVector3Frame.h"
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>

/**
* Box selection dialog box
*@author Jean-Milost Reymond
*/
class TBoxSelection : public TForm
{
    __published:
        TLabel *laBoxTransform;
        TLabel *laBoxPosition;
        TLabel *laBoxRotation;
        TLabel *laBoxScaling;
        TVector3Frame *vfBoxPosition;
        TVector3Frame *vfBoxRotation;
        TVector3Frame *vfBoxScaling;
        TPanel *paBoxTexture;
        TLabel *laTexture;
        TPanel *paTexture;
        TImage *imTexture;
        TPanel *paTextureFile;
        TLabel *laTextureFileName;
        TPanel *paTextureScreenshot;
        TEdit *edTextureFileName;
        TButton *btTextureBrowse;
        TLabel *laBoxOptions;
        TCheckBox *ckRepeatTextureOnEachFace;
        TPanel *paButtons;
        TButton *btOk;
        TButton *btCancel;
        TBevel *blBottomLine;
        TBevel *blMiddleLine;
        TBevel *blTopLine;
        TOpenPictureDialog *opdPicture;

        void __fastcall btTextureBrowseClick(TObject* pSender);
        void __fastcall btCancelClick(TObject* pSender);
        void __fastcall btOkClick(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - form owner
        *@param defaultDir - application default dir
        */
        __fastcall TBoxSelection(TComponent* pOwner, const std::wstring& initialDir);
};
extern PACKAGE TBoxSelection* BoxSelection;
#endif
