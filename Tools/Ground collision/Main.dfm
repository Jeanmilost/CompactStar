object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'Ground collision'
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
  Position = poScreenCenter
  PrintScale = poNone
  Scaled = False
  OnCreate = FormCreate
  OnMouseWheel = FormMouseWheel
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
    Cursor = crCross
    Align = alClient
    BevelOuter = bvNone
    DoubleBuffered = False
    ParentBackground = False
    ParentDoubleBuffered = False
    TabOrder = 0
  end
  object paControls: TPanel
    Left = 615
    Top = 0
    Width = 185
    Height = 600
    Align = alRight
    BevelOuter = bvNone
    Constraints.MinWidth = 185
    ParentBackground = False
    TabOrder = 1
    object laOptionsCaption: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 115
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
    object laTransparency: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 305
      Width = 179
      Height = 13
      Margins.Top = 5
      Align = alTop
      Caption = 'AABB Tree Box Transparency'
      ExplicitWidth = 141
    end
    object blOptionsSeparator: TBevel
      Left = 0
      Top = 137
      Width = 185
      Height = 2
      Align = alTop
      Shape = bsTopLine
      ExplicitTop = 19
    end
    object blFilesSeparator: TBevel
      Left = 0
      Top = 567
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
      Top = 545
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
    object laHitBoxes: TLabel
      Left = 0
      Top = 79
      Width = 185
      Height = 13
      Align = alTop
      Caption = 'Hit Boxes:'
      ExplicitWidth = 49
    end
    object laHitPolygons: TLabel
      Left = 0
      Top = 66
      Width = 185
      Height = 13
      Align = alTop
      Caption = 'Hit Polygons:'
      ExplicitWidth = 63
    end
    object laPolygonsToCheck: TLabel
      Left = 0
      Top = 40
      Width = 185
      Height = 13
      Align = alTop
      Caption = 'Polygons To Check:'
      ExplicitWidth = 94
    end
    object laMaxPolyToCheck: TLabel
      Left = 0
      Top = 53
      Width = 185
      Height = 13
      Align = alTop
      Caption = 'Max Polygons To Check:'
      ExplicitWidth = 117
    end
    object laFPS: TLabel
      Left = 0
      Top = 92
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
      Top = 391
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
      Top = 477
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
      Top = 434
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
      Top = 348
      Width = 179
      Height = 13
      Margins.Top = 5
      Align = alTop
      Caption = 'Model Distance'
      ExplicitWidth = 72
    end
    object ckWireFrame: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 234
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show Boxes In WireFrame'
      Checked = True
      State = cbChecked
      TabOrder = 4
    end
    object ckShowLeafOnly: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 211
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show Leaf Boxes Only'
      TabOrder = 3
    end
    object tbTransparency: TTrackBar
      AlignWithMargins = True
      Left = 3
      Top = 321
      Width = 179
      Height = 19
      Margins.Top = 0
      Align = alTop
      Max = 100
      Position = 80
      ShowSelRange = False
      TabOrder = 8
      TickStyle = tsNone
    end
    object btLoadModel: TButton
      AlignWithMargins = True
      Left = 3
      Top = 572
      Width = 179
      Height = 25
      Align = alBottom
      Caption = 'Open Model...'
      TabOrder = 12
      OnClick = btLoadModelClick
    end
    object ckShowCollidingBoxesOnly: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 188
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show Colliding Boxes Only'
      TabOrder = 2
    end
    object ckShowBoxes: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 165
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show Boxes'
      Checked = True
      State = cbChecked
      TabOrder = 1
    end
    object ckShowCollidingPolygons: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 142
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show Colliding Polygons'
      Checked = True
      State = cbChecked
      TabOrder = 0
    end
    object tbRotationSpeed: TTrackBar
      AlignWithMargins = True
      Left = 3
      Top = 407
      Width = 179
      Height = 19
      Margins.Top = 0
      Align = alTop
      Max = 200
      Position = 50
      ShowSelRange = False
      TabOrder = 9
      TickStyle = tsNone
    end
    object tbAnimationSpeed: TTrackBar
      AlignWithMargins = True
      Left = 3
      Top = 493
      Width = 179
      Height = 19
      Margins.Top = 0
      Align = alTop
      Enabled = False
      Max = 50
      Min = 1
      Position = 10
      ShowSelRange = False
      TabOrder = 11
      TickStyle = tsNone
    end
    object ckPauseModelAnimation: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 257
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Pause The Model Animation'
      Checked = True
      State = cbChecked
      TabOrder = 5
    end
    object tbAnimationNb: TTrackBar
      AlignWithMargins = True
      Left = 3
      Top = 450
      Width = 179
      Height = 19
      Margins.Top = 0
      Align = alTop
      Enabled = False
      Max = 50
      ShowSelRange = False
      TabOrder = 10
      TickStyle = tsNone
    end
    object tbModelDistance: TTrackBar
      AlignWithMargins = True
      Left = 3
      Top = 364
      Width = 179
      Height = 19
      Margins.Top = 0
      Align = alTop
      Max = 150
      Min = 1
      Position = 2
      ShowSelRange = False
      TabOrder = 7
      TickStyle = tsNone
    end
    object ckAntialiasing: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 280
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Antialiasing'
      Checked = True
      State = cbChecked
      TabOrder = 6
    end
  end
  object aeEvents: TApplicationEvents
    OnMessage = aeEventsMessage
    Left = 16
    Top = 8
  end
end
