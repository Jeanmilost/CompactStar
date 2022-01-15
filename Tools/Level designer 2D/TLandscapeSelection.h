/****************************************************************************
 * ==> TLandscapeSelection -------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to select the files      *
 *               required to generate a landscape                           *
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

#ifndef TLandscapeSelectionH
#define TLandscapeSelectionH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>

// std
#include <string>

/**
* Landscape selection files dialog box
*@author Jean-Milost Reymond
*/
class TLandscapeSelection : public TForm
{
    __published:
        TPanel *paSource;
        TPanel *paModelTexture;
        TBevel *blSource;
        TLabel *laSource;
        TPanel *paBitmap;
        TLabel *laTexture;
        TRadioButton *rbSourceBitmap;
        TRadioButton *rbSourceModel;
        TPanel *paModel;
        TImage *imBitmap;
        TPanel *paBitmapFile;
        TPanel *paBitmapFileName;
        TEdit *edBitmapFileName;
        TButton *btBitmapBrowse;
        TLabel *laBitmapFileTitle;
        TPanel *paModelFileName;
        TEdit *edModelFileName;
        TButton *btModelBrowse;
        TLabel *laModelFileNameTitle;
        TPanel *paTextureFile;
        TLabel *laTextureFileName;
        TPanel *paTextureScreenshot;
        TEdit *edTextureFileName;
        TButton *btTextureBrowse;
        TPanel *paTexture;
        TImage *imTexture;
        TPanel *paButtons;
        TButton *btOk;
        TButton *btCancel;
        TOpenDialog *odModel;
        TOpenPictureDialog *opdPicture;
        TRadioButton *rbSourceDontModify;
        TBevel *blBottomLine;

        void __fastcall btBitmapBrowseClick(TObject* pSender);
        void __fastcall btModelBrowseClick(TObject* pSender);
        void __fastcall btTextureBrowseClick(TObject* pSender);
        void __fastcall btCancelClick(TObject* pSender);
        void __fastcall btOkClick(TObject* pSender);
        void __fastcall rbSourceBitmapClick(TObject* pSender);
        void __fastcall rbSourceModelClick(TObject* pSender);
        void __fastcall rbSourceDontModifyClick(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - form owner
        *@param defaultDir - application default dir
        */
        __fastcall TLandscapeSelection(TComponent* pOwner, const std::wstring& defaultDir);
};
extern PACKAGE TLandscapeSelection* LandscapeSelection;
#endif
