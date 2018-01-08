object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'MainForm'
  ClientHeight = 600
  ClientWidth = 800
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pcMain: TPageControl
    Left = 0
    Top = 0
    Width = 800
    Height = 600
    ActivePage = tsView
    Align = alClient
    TabOrder = 0
    object tsView: TTabSheet
      Caption = 'tsView'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object paView: TPanel
        Left = 0
        Top = 0
        Width = 792
        Height = 572
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 0
      end
    end
  end
end
