/*****************************************************************************
 * ==> TGrayscaleMapSelectionFrame ------------------------------------------*
 *****************************************************************************
 * Description : This module provides a frame to allow the user to load a    *
 *               grayscale bitmap which will be used to generate a landscape *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#ifndef TGrayscaleMapSelectionFrameH
#define TGrayscaleMapSelectionFrameH

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
#include <memory>
#include <string>

/**
* Load a texture frame
*@author Jean-Milost Reymond
*/
class TGrayscaleMapSelectionFrame : public TFrame
{
    __published:
        TPanel *paBackground;
        TImage *imBitmap;
        TPanel *paLeft;
        TLabel *laBitmapFile;
        TPanel *paBitmapFile;
        TButton *btBitmapFileOpen;
        TEdit *edBitmapFile;
        TPanel *paButtons;
        TOpenPictureDialog *odOpen;
        TSpeedButton *btInfo;
        TSpeedButton *btConfig;
        TPopupMenu *pmConfig;
        TMenuItem *miCopyToLocalDir;
        TMenuItem *miDeleteBitmap;

        void __fastcall miDeleteBitmapClick(TObject* pSender);
        void __fastcall btBitmapFileOpenClick(TObject* pSender);
        void __fastcall btInfoClick(TObject* pSender);
        void __fastcall btConfigClick(TObject* pSender);

    public:
        /**
        * Called when a file was selected
        *@param pSender - event sender
        *@param fileName - newly selected file name
        */
        typedef void __fastcall (__closure *ITfOnFileSelected)(      TObject*      pSender,
                                                               const std::wstring& fileName);

        /**
        * Constructor
        *@param pOwner - frame owner
        */
        __fastcall TGrayscaleMapSelectionFrame(TComponent* pOwner);

        /**
        * Set OnFileSelected callback function
        *@param fHandler - function handler
        */
        void Set_OnFileSelected(ITfOnFileSelected fHandler);

    private:
        std::auto_ptr<TPicture> m_pDefaultPicture;
        std::wstring            m_DefaultDir;
        ITfOnFileSelected       m_fOnFileSelected;
};
extern PACKAGE TGrayscaleMapSelectionFrame* GrayscaleMapSelectionFrame;
#endif
