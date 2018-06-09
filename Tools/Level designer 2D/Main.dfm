object MainForm: TMainForm
  Left = 0
  Top = 0
  Caption = 'Level designer'
  ClientHeight = 603
  ClientWidth = 800
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = mmMainMenu
  OldCreateOrder = False
  Position = poScreenCenter
  PrintScale = poNone
  Scaled = False
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object spMainView: TSplitter
    Left = 612
    Top = 0
    Height = 603
    Align = alRight
    ResizeStyle = rsUpdate
    OnMoved = spMainViewMoved
    ExplicitLeft = 797
    ExplicitTop = -8
    ExplicitHeight = 600
  end
  object paControls: TPanel
    Left = 615
    Top = 0
    Width = 185
    Height = 603
    Align = alRight
    BevelOuter = bvNone
    Constraints.MinWidth = 185
    ParentBackground = False
    TabOrder = 0
    object blOptions: TBevel
      Left = 0
      Top = 42
      Width = 185
      Height = 2
      Align = alTop
      Shape = bsTopLine
      ExplicitTop = 217
    end
    object laOptions: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 20
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
    object ckAntialiasing: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 47
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
      Top = 116
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Disable the sound'
      TabOrder = 3
      ExplicitTop = 139
    end
    object btResetViewport: TButton
      AlignWithMargins = True
      Left = 3
      Top = 139
      Width = 179
      Height = 25
      Align = alTop
      Caption = 'Reset the viewport'
      TabOrder = 4
      OnClick = btResetViewportClick
      ExplicitTop = 162
    end
    object ckShowBall: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 93
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Show a ball on the camera front'
      TabOrder = 2
    end
    object ckOilPainting: TCheckBox
      AlignWithMargins = True
      Left = 3
      Top = 70
      Width = 179
      Height = 17
      Align = alTop
      Caption = 'Oil painting'
      Checked = True
      State = cbChecked
      TabOrder = 1
    end
  end
  object paViews: TPanel
    Left = 0
    Top = 0
    Width = 612
    Height = 603
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object spViews: TSplitter
      Left = 0
      Top = 200
      Width = 612
      Height = 3
      Cursor = crVSplit
      Align = alTop
      ResizeStyle = rsUpdate
      OnMoved = spViewsMoved
      ExplicitTop = 41
      ExplicitWidth = 559
    end
    object paEngineView: TPanel
      Left = 0
      Top = 203
      Width = 612
      Height = 400
      Cursor = crArrow
      Align = alClient
      BevelOuter = bvNone
      DoubleBuffered = False
      ParentBackground = False
      ParentDoubleBuffered = False
      TabOrder = 0
    end
    object paDesignerView: TPanel
      Left = 0
      Top = 0
      Width = 612
      Height = 200
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 1
    end
  end
  object aeEvents: TApplicationEvents
    OnMessage = aeEventsMessage
    Left = 16
    Top = 8
  end
  object mmMainMenu: TMainMenu
    Left = 48
    Top = 8
    object miFile: TMenuItem
      Caption = 'File'
      object miFileNew: TMenuItem
        Caption = 'New...'
        OnClick = miFileNewClick
      end
    end
    object miLandscape: TMenuItem
      Caption = 'Landscape'
      object miAdd: TMenuItem
        Caption = 'Add'
        object miAddBox: TMenuItem
          Caption = 'Box'
          OnClick = miAddBoxClick
        end
      end
    end
    object miPostProcessing: TMenuItem
      Caption = 'Post-Processing'
      object miEffects: TMenuItem
        Caption = 'Effects...'
      end
    end
    object miSound: TMenuItem
      Caption = 'Sound'
    end
  end
end
