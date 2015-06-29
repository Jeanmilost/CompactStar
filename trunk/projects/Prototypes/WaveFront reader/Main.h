#ifndef MainH
#define MainH

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <gl\gl.h>
#include <gl\glu.h>

//REM #include "IQRMD2.h"

class TMainForm : public TForm
{
    __published:

        void __fastcall FormResize(TObject *Sender);
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
              TShiftState Shift);

    public:
        __fastcall TMainForm(TComponent* pOwner);
        virtual __fastcall ~TMainForm();

        void __fastcall IdleLoop(TObject*, bool&);
        void __fastcall RenderGLScene();
        void __fastcall SetPixelFormatDescriptor();
        void __fastcall DrawObjects();
        void __fastcall CreateInpriseCorporateLogo();
        void __fastcall DrawGoldCube();
        void __fastcall DrawRedPyramid();
        void __fastcall DrawBlueSphere();
        void __fastcall DrawRing();
        void __fastcall SetupLighting();
        void __fastcall SetupTextures();

    private:
        //REM IQRMD2*     m_pMD2;
        std::time_t m_PreviousTime;

        HDC hdc;
        HGLRC hrc;
        int PixelFormat;
        GLuint startoflist;
        GLfloat x, y, xstep, ystep, w, h;
        GLsizei size;
        GLuint texture1, texture2, texture3;
        GLUquadricObj* GoldCube, *GoldCubeTop, *GoldCubeBottom,
                        *RedPyramid, *RedPyramidBottom, *BlueSphere, *Ring;
        Graphics::TBitmap* bitmap;
};
extern PACKAGE TMainForm* MainForm;
#endif
