/****************************************************************************
 * ==> TSkyboxSelection ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to select the files      *
 *               required to generate a skybox                              *
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

#ifndef TSkyboxSelectionH
#define TSkyboxSelectionH

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
#include <vector>

/**
* Skybox selection files dialog box
*@author Jean-Milost Reymond
*/
class TSkyboxSelection : public TForm
{
    __published:
        TPanel *paButtons;
        TButton *btOk;
        TButton *btCancel;
        TPanel *paImagesTop;
        TPanel *paImagesMiddle;
        TPanel *paImagesBottom;
        TImage *imLeft;
        TImage *imBack;
        TImage *imRight;
        TImage *imFront;
        TImage *imTop;
        TImage *imBottom;
        TOpenPictureDialog *opdPicture;

        void __fastcall OnImageClick(TObject* pSender);
        void __fastcall btCancelClick(TObject* pSender);
        void __fastcall btOkClick(TObject* pSender);

    public:
        typedef std::vector<std::wstring> IFileNames;

        /**
        * Constructor
        *@param pOwner - form owner
        *@param defaultDir - application default dir
        */
        __fastcall TSkyboxSelection(TComponent* pOwner, const std::wstring& initialDir);

        /**
        * Gets the skybox file names
        *@param[in, out] fileNames - file list to populate with the skybox file names
        */
        void GetFileNames(IFileNames& fileNames) const;

    private:
        std::wstring m_RightFileName;
        std::wstring m_LeftFileName;
        std::wstring m_TopFileName;
        std::wstring m_BottomFileName;
        std::wstring m_FrontFileName;
        std::wstring m_BackFileName;
};
extern PACKAGE TSkyboxSelection* SkyboxSelection;
#endif
