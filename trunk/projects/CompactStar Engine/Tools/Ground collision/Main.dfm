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
    Cursor = crArrow
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
    object laAltitude: TLabel
      Left = 0
      Top = 40
      Width = 185
      Height = 13
      Align = alTop
      Caption = 'Altitude:'
      ExplicitWidth = 41
    end
    object laFPS: TLabel
      Left = 0
      Top = 53
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
    object blOptions: TBevel
      Left = 0
      Top = 108
      Width = 185
      Height = 2
      Align = alTop
      Shape = bsTopLine
      ExplicitTop = 217
    end
    object laOptions: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 86
      Width = 179
      Height = 19
      Margins.Top = 20
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
    object btLoadModel: TButton
      AlignWithMargins = True
      Left = 3
      Top = 572
      Width = 179
      Height = 25
      Align = alBottom
      Caption = 'Open Model...'
      TabOrder = 5
      OnClick = btLoadModelClick
    end
    object ckAntialiasing: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 113
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Antialiasing'
      Checked = True
      State = cbChecked
      TabOrder = 0
    end
    object ckDisableSound: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 182
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Disable the sound'
      TabOrder = 3
      ExplicitTop = 159
    end
    object btResetViewport: TButton
      AlignWithMargins = True
      Left = 3
      Top = 205
      Width = 179
      Height = 25
      Align = alTop
      Caption = 'Reset the viewport'
      TabOrder = 4
      OnClick = btResetViewportClick
      ExplicitTop = 182
    end
    object ckShowBall: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 136
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show a ball on the camera front'
      TabOrder = 1
    end
    object ckSlipAgainstSlopes: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 159
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Slip against the slopes'
      TabOrder = 2
      ExplicitLeft = -5
      ExplicitTop = 182
    end
  end
  object aeEvents: TApplicationEvents
    OnMessage = aeEventsMessage
    Left = 16
    Top = 8
  end
end
