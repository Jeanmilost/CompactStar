object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'MainForm'
  ClientHeight = 300
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object bt1: TButton
    Left = 320
    Top = 248
    Width = 75
    Height = 25
    Caption = 'bt1'
    TabOrder = 0
    OnClick = bt1Click
  end
  object me1: TMemo
    Left = 256
    Top = 48
    Width = 185
    Height = 89
    Lines.Strings = (
      'me1')
    TabOrder = 1
  end
end
