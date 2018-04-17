#ifndef TAddItemDialogH
#define TAddItemDialogH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Buttons.hpp>

class TAddItemDialog : public TForm
{
    __published:
        TPageControl *pcWizard;
        TTabSheet *tsSelectItem;
        TTabSheet *tsAddSurface;
        TBevel *blSeparator;
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

        void __fastcall FormShow(TObject* pSender);
        void __fastcall btCancelClick(TObject* pSender);
        void __fastcall btOKClick(TObject* pSender);
        void __fastcall btSelectItemAddSurfaceClick(TObject* pSender);

    public:
        __fastcall TAddItemDialog(TComponent* pOwner);

    private:
};
extern PACKAGE TAddItemDialog* AddItemDialog;
#endif
