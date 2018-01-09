object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'AABB Tool'
  ClientHeight = 600
  ClientWidth = 800
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object spMainView: TSplitter
    Left = 612
    Top = 0
    Height = 600
    Align = alRight
    ResizeStyle = rsUpdate
    OnMoved = spMainViewMoved
    ExplicitLeft = 797
    ExplicitTop = -8
  end
  object paView: TPanel
    Left = 0
    Top = 0
    Width = 612
    Height = 600
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    ExplicitWidth = 792
    ExplicitHeight = 572
  end
  object paControls: TPanel
    Left = 615
    Top = 0
    Width = 185
    Height = 600
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 1
    object laTreeControl: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 3
      Width = 179
      Height = 13
      Align = alTop
      Caption = 'Tree control'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 58
    end
    object laTransparency: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 70
      Width = 179
      Height = 13
      Margins.Top = 5
      Align = alTop
      Caption = 'Transparency'
      ExplicitWidth = 66
    end
    object ckWireFrame: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 22
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show WireFrame boxes'
      Checked = True
      State = cbChecked
      TabOrder = 0
      ExplicitLeft = 80
      ExplicitTop = 96
      ExplicitWidth = 97
    end
    object ckShowLeafOnly: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 45
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show Leaf Only'
      TabOrder = 1
      ExplicitLeft = 48
      ExplicitTop = 96
      ExplicitWidth = 97
    end
    object tbTransparency: TTrackBar
      AlignWithMargins = True
      Left = 3
      Top = 86
      Width = 179
      Height = 45
      Margins.Top = 0
      Align = alTop
      Max = 100
      Position = 80
      ShowSelRange = False
      TabOrder = 2
      TickStyle = tsNone
      ExplicitTop = 87
    end
  end
end
