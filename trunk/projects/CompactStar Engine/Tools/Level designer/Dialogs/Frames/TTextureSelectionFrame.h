/*****************************************************************************
 * ==> TTextureSelectionFrame -----------------------------------------------*
 *****************************************************************************
 * Description : This module provides a frame to allow the user to load a    *
 *               texture for a model                                         *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#ifndef TTextureSelectionFrameH
#define TTextureSelectionFrameH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Menus.hpp>

// std
#include <string>

/**
* Load a texture frame
*@author Jean-Milost Reymond
*/
class TTextureSelectionFrame : public TFrame
{
    __published:
        TPanel *paBackground;
        TImage *imTexture;
        TPanel *paLeft;
        TLabel *laTextureFile;
        TPanel *paTextureFile;
        TButton *btTextureFileOpen;
        TEdit *edTextureFile;
        TPanel *paButtons;
        TOpenPictureDialog *odOpen;
        TSpeedButton *btInfo;
        TSpeedButton *btConfig;
        TPopupMenu *pmConfig;
        TMenuItem *miCopyToLocalDir;

        void __fastcall btTextureFileOpenClick(TObject* pSender);
        void __fastcall btInfoClick(TObject* pSender);
        void __fastcall btConfigClick(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - frame owner
        */
        __fastcall TTextureSelectionFrame(TComponent* pOwner);

    private:
        std::wstring m_DefaultDir;
};
extern PACKAGE TTextureSelectionFrame* TextureSelectionFrame;
#endif
