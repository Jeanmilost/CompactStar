object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 
    'Weather effects - press 1 for rain, 2 for snow and 3 for both ra' +
    'in and snow'
  ClientHeight = 623
  ClientWidth = 800
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