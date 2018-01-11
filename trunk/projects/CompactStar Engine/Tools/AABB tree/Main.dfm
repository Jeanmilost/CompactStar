object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'Aligned-Axis Bounding Box Tool'
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
    TabOrder = 1
    object laOptionsCaption: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 108
      Width = 179
      Height = 19
      Align = alTop
      Caption = 'Options'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      ExplicitTop = 30
      ExplicitWidth = 62
    end
    object laTransparency: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 252
      Width = 179
      Height = 13
      Margins.Top = 5
      Align = alTop
      Caption = 'Transparency'
      ExplicitTop = 174
      ExplicitWidth = 66
    end
    object blOptionsSeparator: TBevel
      Left = 0
      Top = 130
      Width = 185
      Height = 2
      Align = alTop
      Shape = bsTopLine
      ExplicitTop = 19
    end
    object blFilesSeparator: TBevel
      Left = 0
      Top = 322
      Width = 185
      Height = 2
      Align = alTop
      Shape = bsTopLine
      ExplicitLeft = 3
      ExplicitTop = 449
    end
    object laFilesCaption: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 300
      Width = 179
      Height = 19
      Margins.Top = 10
      Align = alTop
      Caption = 'Files'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      ExplicitTop = 222
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
      ExplicitLeft = 3
      ExplicitTop = 59
    end
    object laHitPolygons: TLabel
      Left = 0
      Top = 66
      Width = 185
      Height = 13
      Align = alTop
      Caption = 'Hit Polygons:'
      ExplicitLeft = 3
      ExplicitTop = 33
    end
    object laPolygonsToCheck: TLabel
      Left = 0
      Top = 40
      Width = 185
      Height = 13
      Align = alTop
      Caption = 'Polygons To Check:'
      ExplicitTop = 27
      ExplicitWidth = 94
    end
    object laMaxPolyToCheck: TLabel
      Left = 0
      Top = 53
      Width = 185
      Height = 13
      Align = alTop
      Caption = 'Max Polygons To Check:'
      ExplicitTop = 40
      ExplicitWidth = 117
    end
    object laFPS: TLabel
      Left = 0
      Top = 92
      Width = 185
      Height = 13
      Align = alTop
      Caption = 'FPS:'
      ExplicitTop = 79
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
    object ckWireFrame: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 227
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'WireFrame'
      Checked = True
      State = cbChecked
      TabOrder = 0
      ExplicitTop = 149
    end
    object ckShowLeafOnly: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 204
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show Leaf Only'
      TabOrder = 1
      ExplicitTop = 126
    end
    object tbTransparency: TTrackBar
      AlignWithMargins = True
      Left = 3
      Top = 268
      Width = 179
      Height = 19
      Margins.Top = 0
      Align = alTop
      Max = 100
      Position = 80
      ShowSelRange = False
      TabOrder = 2
      TickStyle = tsNone
      ExplicitTop = 190
    end
    object btLoadModel: TButton
      AlignWithMargins = True
      Left = 3
      Top = 327
      Width = 179
      Height = 25
      Align = alTop
      Caption = 'Load Model...'
      TabOrder = 3
      ExplicitTop = 249
    end
    object btSaveTree: TButton
      AlignWithMargins = True
      Left = 3
      Top = 358
      Width = 179
      Height = 25
      Align = alTop
      Caption = 'Save Tree...'
      TabOrder = 4
      ExplicitTop = 280
    end
    object ckShowCollidingBoxesOnly: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 181
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show Colliding Boxes Only'
      TabOrder = 5
      ExplicitTop = 103
    end
    object ckShowBoxes: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 158
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show Boxes'
      Checked = True
      State = cbChecked
      TabOrder = 6
      ExplicitTop = 80
    end
    object ckShowCollidingPolygons: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 135
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show Colliding Polygons'
      Checked = True
      State = cbChecked
      TabOrder = 7
      ExplicitTop = 57
    end
  end
end
