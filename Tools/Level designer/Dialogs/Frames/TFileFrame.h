/*****************************************************************************
 * ==> TFileFrame -----------------------------------------------------------*
 *****************************************************************************
 * Description : This module provides a frame to open a file                 *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#ifndef TFileFrameH
#define TFileFrameH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>

// std
#include <string>

/**
* Frame allowing to select a file
*@author Jean-Milost Reymond
*/
class TFileFrame : public TFrame
{
    __published:
        TEdit *edFileName;
        TButton *btBrowse;
        TOpenDialog *odOpen;

        void __fastcall btBrowseClick(TObject* pSender);

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
        *@aram pOwner - frame owner
        */
        __fastcall TFileFrame(TComponent* pOwner);

        /**
        * Set OnFileSelected callback function
        *@param fHandler - function handler
        */
        void Set_OnFileSelected(ITfOnFileSelected fHandler);

    private:
        std::wstring      m_DefaultDir;
        ITfOnFileSelected m_fOnFileSelected;
};
extern PACKAGE TFileFrame* FileFrame;
#endif
