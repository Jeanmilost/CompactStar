/****************************************************************************
 * ==> TModelSelection -----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a dialog box to configure a model on  *
 *               the landscape                                              *
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

#ifndef TModelSelectionH
#define TModelSelectionH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "TVector3Frame.h"
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ExtDlgs.hpp>
#include <Vcl.Imaging.pngimage.hpp>

// interface
#include "TVector3Frame.h"

/**
* Model selection dialog box
*@author Jean-Milost Reymond
*/
class TModelSelection : public TForm
{
    __published:
        TLabel *laTransform;
        TLabel *laPosition;
        TLabel *laRotation;
        TLabel *laScaling;
        TBevel *blBottomLine;
        TBevel *blMiddleLine;
        TBevel *blTopLine;
        TVector3Frame *vfPosition;
        TVector3Frame *vfRotation;
        TVector3Frame *vfScaling;
        TPanel *paModelTexture;
        TLabel *laTexture;
        TPanel *paTexture;
        TImage *imTexture;
        TPanel *paTextureFile;
        TLabel *laTextureFileName;
        TPanel *paTextureScreenshot;
        TEdit *edTextureFileName;
        TButton *btTextureBrowse;
        TPanel *paButtons;
        TButton *btOk;
        TButton *btCancel;
        TOpenPictureDialog *opdPicture;
        TPanel *paModel;
        TLabel *laModelFileNameTitle;
        TPanel *paModelFileName;
        TEdit *edModelFileName;
        TButton *btModelBrowse;
        TLabel *laModelFileName;
        TOpenDialog *odModel;

        void __fastcall btModelBrowseClick(TObject* pSender);
        void __fastcall btTextureBrowseClick(TObject* pSender);
        void __fastcall btCancelClick(TObject* pSender);
        void __fastcall btOkClick(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - form owner
        *@param defaultDir - application default dir
        */
        __fastcall TModelSelection(TComponent* pOwner, const std::wstring& initialDir);

        /**
        * Builds a model matrix from the configured values on the interface
        *@param[in, out] pMatrix - matrix to populate with the values, populated matrix on function ends
        */
        void BuildMatrix(CSR_Matrix4* pMatrix) const;
};
extern PACKAGE TModelSelection* ModelSelection;
#endif
