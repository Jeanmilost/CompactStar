object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'Spaceship'
  ClientHeight = 390
  ClientWidth = 390
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PrintScale = poNone
  Scaled = False
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object aeEvents: TApplicationEvents
    OnMessage = aeEventsMessage
    Left = 760
    Top = 8
  end
end
