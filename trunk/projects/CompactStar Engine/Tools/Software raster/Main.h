#ifndef MainH
#define MainH

#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>

// compactStart engine
#include "CSR_Geometry.h"
#include "CSR_Vertex.h"
#include "CSR_Model.h"
#include "CSR_SoftwareRaster.h"

class TMainForm : public TForm
{
    __published:
        TButton *bt1;

        void __fastcall bt1Click(TObject *Sender);

    public:
        __fastcall TMainForm(TComponent* Owner);

    private:
        static void OnApplyFragmentShader(const CSR_Matrix4*  pMatrix,
                                          const CSR_Polygon3* pPolygon,
                                          const CSR_Vector2*  pST,
                                          const CSR_Vector3*  pSampler,
                                                float         z,
                                                CSR_Color*    pColor);
};
extern PACKAGE TMainForm *MainForm;
#endif
