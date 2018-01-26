object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'MainForm'
  ClientHeight = 672
  ClientWidth = 1102
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = mmMenu
  OldCreateOrder = False
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object spInspector: TSplitter
    Left = 914
    Top = 20
    Height = 652
    Align = alRight
    ResizeStyle = rsUpdate
    ExplicitLeft = 1099
    ExplicitTop = 0
  end
  object paToolbox: TPanel
    Left = 0
    Top = 0
    Width = 1102
    Height = 20
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
  end
  object paInspector: TPanel
    Left = 917
    Top = 20
    Width = 185
    Height = 652
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 1
  end
  object paViews: TPanel
    Left = 0
    Top = 20
    Width = 914
    Height = 652
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 2
    object pcViews: TPageControl
      Left = 0
      Top = 0
      Width = 914
      Height = 652
      ActivePage = ts3DView
      Align = alClient
      TabOrder = 0
      object ts3DView: TTabSheet
        Caption = '3D view'
        object pa3DView: TPanel
          Left = 0
          Top = 0
          Width = 906
          Height = 624
          Align = alClient
          BevelOuter = bvNone
          Color = clBlack
          ParentBackground = False
          TabOrder = 0
        end
      end
    end
  end
  object mmMenu: TMainMenu
    Left = 756
    Top = 5
    object miViews: TMenuItem
      Caption = 'Views'
      object mi3Dview: TMenuItem
        Action = ac3DView
      end
    end
  end
  object alActions: TActionList
    Left = 720
    Top = 5
    object ac3DView: TAction
      Category = 'Views'
      Caption = '3D View'
    end
  end
  object ilToolbox: TImageList
    ColorDepth = cd32Bit
    DrawingStyle = dsTransparent
    Masked = False
    ShareImages = True
    Left = 684
    Top = 5
    Bitmap = {
      494C010101000500040010001000FFFFFFFF2110FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000001000000001002000000000000010
      00000000000000000000000000000000000000000000413DDAFFCFCFDAFF0000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000CFCFDAFF413DDAFF3A38D9FF0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000433FD9FF0F0AD9FF3B39D8FFCFCF
      DAFF000000000000000000000000000000000000000000000000000000000000
      0000CFCFDAFF3B39D8FF0F0AD9FF433FD9FF0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000D1D0D9FF403CD9FF0F0AD9FF3B39
      D8FFCFCFDAFF000000000000000000000000000000000000000000000000D1D0
      D9FF403DD8FF0F0AD9FF3B39D8FFCFCFDAFF0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000D1D0D9FF403CD9FF0F0A
      D9FF3B39D8FFCFCFDAFF00000000000000000000000000000000D1D0D9FF403D
      D8FF0F0AD9FF3B39D8FFCFCFDAFF000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000D1D0D9FF403C
      D9FF0F0AD9FF3B39D8FFCFCFDAFF0000000000000000D1D0D9FF403DD8FF0F0A
      D9FF3B39D8FFCFCFDAFF00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000D1D0
      D9FF403CD9FF0F0AD9FF3B39D8FFCFCFDAFFD1D0D9FF403DD8FF0F0AD9FF3B39
      D8FFCFCFDAFF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000D1D0D9FF403CD9FF0F0AD9FF3936D9FF3E3BD9FF0F0AD9FF3B39D8FFCFCF
      DAFF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000D1D0D9FF3D39D8FF0F0AD9FF0F0AD9FF3837D9FFCFCFDAFF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000D1D0D9FF3E39D8FF0F0AD9FF0F0AD9FF3835D9FFCFCFDAFF0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000D1D0D9FF403DD8FF0F0AD9FF3936D9FF3D39D9FF0F0AD9FF3B39D8FFCFCF
      DAFF000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000D1D0
      D9FF403DD8FF0F0AD9FF3B39D8FFCFCFDAFFD1D0D9FF403CD9FF0F0AD9FF3B39
      D8FFCFCFDAFF0000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000D1D0D9FF403D
      D8FF0F0AD9FF3B39D8FFCFCFDAFF0000000000000000D1D0D9FF403CD9FF0F0A
      D9FF3B39D8FFCFCFDAFF00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000D1D0D9FF403DD8FF0F0A
      D9FF3B39D8FFCFCFDAFF00000000000000000000000000000000D1D0D9FF403C
      D9FF0F0AD9FF3B39D8FFCFCFDAFF000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000D1D0D9FF403DD8FF0F0AD9FF3B39
      D8FFCFCFDAFF000000000000000000000000000000000000000000000000D1D0
      D9FF403CD9FF0F0AD9FF3B39D8FFCFCFDAFF0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000433FD9FF0F0AD9FF3B39D8FFCFCF
      DAFF000000000000000000000000000000000000000000000000000000000000
      0000CFCFDAFF3B39D8FF0F0AD9FF433FD9FF0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000003A38D9FF413DDAFFCFCFDAFF0000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000CFCFDAFF413DDAFF3A38D9FF0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000100000000100010000000000800000000000000000000000
      000000000000000000000000FFFFFF0000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000}
  end
end