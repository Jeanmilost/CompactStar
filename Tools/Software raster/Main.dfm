object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'Software rasterizer'
  ClientHeight = 636
  ClientWidth = 742
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
  object spMainView: TSplitter
    Left = 554
    Top = 0
    Height = 636
    Align = alRight
    ResizeStyle = rsUpdate
    ExplicitLeft = 797
    ExplicitTop = -8
    ExplicitHeight = 600
  end
  object paView: TPanel
    Left = 0
    Top = 0
    Width = 554
    Height = 636
    Cursor = crCross
    Align = alClient
    BevelOuter = bvNone
    DoubleBuffered = False
    ParentBackground = False
    ParentDoubleBuffered = False
    TabOrder = 0
  end
  object paControls: TPanel
    Left = 557
    Top = 0
    Width = 185
    Height = 636
    Align = alRight
    BevelOuter = bvNone
    Constraints.MinWidth = 185
    ParentBackground = False
    TabOrder = 1
    object laOptionsCaption: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 63
      Width = 179
      Height = 19
      Margins.Top = 10
      Align = alTop
      Caption = 'Options'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      ExplicitWidth = 62
    end
    object blOptionsSeparator: TBevel
      Left = 0
      Top = 85
      Width = 185
      Height = 2
      Align = alTop
      Shape = bsTopLine
      ExplicitTop = 19
    end
    object blFilesSeparator: TBevel
      Left = 0
      Top = 603
      Width = 185
      Height = 2
      Align = alBottom
      Shape = bsTopLine
      ExplicitLeft = 3
      ExplicitTop = 449
    end
    object laFilesCaption: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 581
      Width = 179
      Height = 19
      Margins.Top = 10
      Align = alBottom
      Caption = 'Files'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      ExplicitWidth = 37
    end
    object blStatsSeparator: TBevel
      Left = 0
      Top = 25
      Width = 185
      Height = 2
      Align = alTop
      Shape = bsTopLine
      ExplicitLeft = 6
      ExplicitTop = 17
    end
    object laStatsCaption: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 3
      Width = 179
      Height = 19
      Align = alTop
      Caption = 'Statistics'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      ExplicitWidth = 75
    end
    object laFPS: TLabel
      Left = 0
      Top = 40
      Width = 185
      Height = 13
      Align = alTop
      Caption = 'FPS:'
      ExplicitWidth = 22
    end
    object laPolygonCount: TLabel
      Left = 0
      Top = 27
      Width = 185
      Height = 13
      Align = alTop
      Caption = 'Polygon Count:'
      ExplicitWidth = 74
    end
    object laRotationSpeedCaption: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 158
      Width = 179
      Height = 13
      Margins.Top = 5
      Align = alTop
      Caption = 'Model Rotation Speed'
      ExplicitWidth = 105
    end
    object laAnimationSpeedCaption: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 244
      Width = 179
      Height = 13
      Margins.Top = 5
      Align = alTop
      Caption = 'Model Animation Speed'
      ExplicitWidth = 111
    end
    object laAnimationNbCaption: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 201
      Width = 179
      Height = 13
      Margins.Top = 5
      Align = alTop
      Caption = 'Model Animation Nb.'
      ExplicitWidth = 98
    end
    object laModelDistance: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 115
      Width = 179
      Height = 13
      Margins.Top = 5
      Align = alTop
      Caption = 'Model Distance'
      ExplicitWidth = 72
    end
    object btLoadModel: TButton
      AlignWithMargins = True
      Left = 3
      Top = 608
      Width = 179
      Height = 25
      Align = alBottom
      Caption = 'Open Model...'
      TabOrder = 5
      OnClick = btLoadModelClick
      ExplicitTop = 606
    end
    object tbRotationSpeed: TTrackBar
      AlignWithMargins = True
      Left = 3
      Top = 174
      Width = 179
      Height = 19
      Margins.Top = 0
      Align = alTop
      Max = 200
      Position = 50
      ShowSelRange = False
      TabOrder = 2
      TickStyle = tsNone
    end
    object tbAnimationSpeed: TTrackBar
      AlignWithMargins = True
      Left = 3
      Top = 260
      Width = 179
      Height = 19
      Margins.Top = 0
      Align = alTop
      Enabled = False
      Max = 50
      Min = 1
      Position = 10
      ShowSelRange = False
      TabOrder = 4
      TickStyle = tsNone
    end
    object ckPauseModelAnimation: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 90
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Pause The Model Animation'
      TabOrder = 0
    end
    object tbAnimationNb: TTrackBar
      AlignWithMargins = True
      Left = 3
      Top = 217
      Width = 179
      Height = 19
      Margins.Top = 0
      Align = alTop
      Enabled = False
      Max = 50
      ShowSelRange = False
      TabOrder = 3
      TickStyle = tsNone
    end
    object tbModelDistance: TTrackBar
      AlignWithMargins = True
      Left = 3
      Top = 131
      Width = 179
      Height = 19
      Margins.Top = 0
      Align = alTop
      Max = 150
      Min = 1
      Position = 2
      ShowSelRange = False
      TabOrder = 1
      TickStyle = tsNone
    end
  end
  object odOpen: TOpenDialog
    DefaultExt = 'mdl'
    Filter = 'Quake I model (*.mdl)|*.mdl'
    Left = 13
    Top = 16
  end
end
