/*****************************************************************************
 * ==> TAddItemDialog -------------------------------------------------------*
 *****************************************************************************
 * Description : This module contains a toolbox dialog to add a scene item   *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#ifndef TAddItemDialogH
#define TAddItemDialogH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include "TTextureSelectionFrame.h"

/**
* Add an item dialog box
*@author Jean-Milost Reymond
*/
class TAddItemDialog : public TForm
{
    __published:
        TPageControl *pcWizard;
        TTabSheet *tsSelectItem;
        TTabSheet *tsAddSurface;
        TBevel *blSelectItemSeparator;
        TPanel *paNavigation;
        TButton *btOK;
        TButton *btCancel;
        TLabel *laSelectItemTitle;
        TSpeedButton *btSelectItemAddSurface;
        TSpeedButton *btSelectItemAddBox;
        TSpeedButton *btSelectItemAddSphere;
        TSpeedButton *btSelectItemAddCylinder;
        TSpeedButton *btSelectItemAddDisk;
        TSpeedButton *btSelectItemAddSpiral;
        TSpeedButton *btSelectItemAddRing;
        TSpeedButton *btSelectItemAddModel;
        TPanel *paMain;
        TLabel *laAddSurfaceTitle;
        TBevel *blAddSurfaceSeparator;
        TButton *btBack;
        TTextureSelectionFrame *tsfAddSurfaceTexture;
        TLabel *laAddSurfaceTextureTitle;
        TLabel *laAddSurfaceBumpmapTitle;
        TTextureSelectionFrame *tsfAddSurfaceBumpmap;

        void __fastcall FormShow(TObject* pSender);
        void __fastcall btCancelClick(TObject* pSender);
        void __fastcall btOKClick(TObject* pSender);
        void __fastcall btSelectItemAddSurfaceClick(TObject* pSender);
        void __fastcall btBackClick(TObject* pSender);

    public:
        __fastcall TAddItemDialog(TComponent* pOwner);

    private:
};
extern PACKAGE TAddItemDialog* AddItemDialog;
#endif
