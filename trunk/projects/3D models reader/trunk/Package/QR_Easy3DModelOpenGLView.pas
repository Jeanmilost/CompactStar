{******************************************************************************
 * ==> QR_Easy3DModelOpenGLView ----------------------------------------------*
 ******************************************************************************
 * Description : Easy 3D models OpenGL view                                   *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************}

unit QR_Easy3DModelOpenGLView;

interface

uses
    Winapi.Windows, System.SysUtils, System.Classes, Vcl.Controls, Winapi.OpenGL, QRAPI_VertexBuffer,
    QRAPI_MD2;

type
    TEasy3DModelOpenGLView = class(TGraphicControl)
        published
        public
            constructor Create(pOwner: TControl); virtual;
            procedure PaintTo2(handle: TMD2Handle; hDC: HDC); virtual;
        protected
        private
    end;

implementation
constructor TEasy3DModelOpenGLView.Create(pOwner: TControl);
begin
    inherited Create(pOwner);
end;

procedure TEasy3DModelOpenGLView.PaintTo2(handle: TMD2Handle; hDC: HDC);
begin
    glPushMatrix();
    glTranslatef(0.0, -7.0, -30.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    Draw_MD2_To_OpenGL_1(handle, tvfNone, False);
    glFlush();
    glPopMatrix();
end;

end.

