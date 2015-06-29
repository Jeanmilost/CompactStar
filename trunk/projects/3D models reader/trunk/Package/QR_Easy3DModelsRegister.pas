{******************************************************************************
 * ==> QR_Easy3DModelsRegister -----------------------------------------------*
 ******************************************************************************
 * Description : Easy 3D models package registration                          *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************}

unit QR_Easy3DModelsRegister;

interface

uses
    System.Classes, QR_Easy3DModelOpenGLView;

procedure Register;

implementation

procedure Register;
begin
    // register all package components in RAD studio toolbar
    RegisterComponents('Easy3DModels', [TEasy3DModelOpenGLView]);
end;

end.
