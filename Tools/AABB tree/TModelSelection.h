/****************************************************************************
 * ==> AABB Tree tool model selection form ---------------------------------*
 ****************************************************************************
 * Description : Aligned-Axis Bounding Box tool selection form              *
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

#ifndef TModelSelectionH
#define TModelSelectionH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>

/**
* AABB Tree tool model selection form
*@author Jean-Milost Reymond
*/
class TModelSelection : public TForm
{
    __published:
        TGroupBox *gbShape;
        TPanel *paMDLModel;
        TButton *btOpenFile;
        TButton *btSelect;
        TLabel *laSelectMDLModel;
        TEdit *edMDLFilelName;
        TOpenDialog *odOpen;
        TRadioButton *rbSphere;
        TRadioButton *rbCylinder;
        TRadioButton *rbDisk;
        TRadioButton *rbRing;
        TRadioButton *rbSpiral;
        TRadioButton *rbSurface;
        TRadioButton *rbBox;
        TPanel *paShapeConfig;
        TLabel *laSlices;
        TEdit *edSlices;
        TLabel *laStacks;
        TEdit *edStacks;
        TUpDown *udSlices;
        TUpDown *udStacks;

        void __fastcall btSelectClick(TObject* pSender);
        void __fastcall btOpenFileClick(TObject* pSender);
        void __fastcall OnClick(TObject* pSender);
        void __fastcall edSlicesAndStacksChange(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - form owner
        */
        __fastcall TModelSelection(TComponent* pOwner);

        /**
        * Gets the model file name
        *@return model file name
        */
        std::wstring GetModelFileName() const;

    private:
        std::wstring m_ModelFileName;
};
extern PACKAGE TModelSelection* pModelSelection;
#endif
