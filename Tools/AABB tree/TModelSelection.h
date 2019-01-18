/****************************************************************************
 * ==> AABB Tree tool model selection form ---------------------------------*
 ****************************************************************************
 * Description : Aligned-Axis Bounding Box tool selection form              *
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
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>

// std
#include <vector>
#include <string>

/**
* AABB Tree tool model selection form
*@author Jean-Milost Reymond
*/
class TModelSelection : public TForm
{
    __published:
        TPanel *paMDLModel;
        TButton *btOpenFile;
        TButton *btSelect;
        TLabel *laSelectMDLModel;
        TEdit *edMDLFilelName;
        TOpenDialog *odOpen;
        TPanel *paShape;
        TRadioGroup *rgShapes;
        TPanel *paSlices;
        TLabel *laSlices;
        TEdit *edSlices;
        TUpDown *udSlices;
        TPanel *paStacks;
        TLabel *laStacks;
        TEdit *edStacks;
        TUpDown *udStacks;
        TPanel *paFaces;
        TLabel *laFaces;
        TEdit *edFaces;
        TUpDown *udFaces;
        TColorDialog *cdColors;
        TPanel *paColors;
        TLabel *laColors;
        TPanel *paSelectedColor;
        TButton *btSelectColor;

        void __fastcall rgShapesClick(TObject* pSender);
        void __fastcall paSelectedColorClick(TObject* pSender);
        void __fastcall edSlicesAndStacksChange(TObject* pSender);
        void __fastcall btOpenFileClick(TObject* pSender);
        void __fastcall btSelectColorClick(TObject* pSender);
        void __fastcall btSelectClick(TObject* pSender);

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

        /**
        * Gets the model file name
        *@return model file name
        */
        void SetModelFileName(const std::wstring& fileName);

    private:
        typedef std::vector<TControl*> IControls;

        std::wstring m_ModelFileName;

        /**
        * Selects the model color
        */
        void SelectModelColor();

        /**
        * Distribute the VCL controls from top to bottom
        *@param controls - controls to distribute
        */
        void DistributeCtrlsTopToBottom(IControls& controls);
};
extern PACKAGE TModelSelection* pModelSelection;
#endif
