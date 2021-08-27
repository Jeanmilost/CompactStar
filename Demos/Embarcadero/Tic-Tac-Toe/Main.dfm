object MainForm: TMainForm
  Left = 0
  Top = 0
  BorderStyle = bsSingle
  Caption = 'Tic-Tac-Toe - [0]->[9] place pawns, [Space] restart'
  ClientHeight = 400
  ClientWidth = 400
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
