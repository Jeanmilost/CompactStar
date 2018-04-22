object VertexColorFrame: TVertexColorFrame
  Left = 0
  Top = 0
  Width = 442
  Height = 31
  TabOrder = 0
  object paMain: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 436
    Height = 25
    Margins.Bottom = 0
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    ExplicitTop = 6
    object laCaption: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 3
      Width = 85
      Height = 19
      Align = alLeft
      Caption = 'Vertex color'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Layout = tlCenter
    end
    object paColor: TPanel
      Left = 411
      Top = 0
      Width = 25
      Height = 25
      Cursor = crHandPoint
      Align = alRight
      BevelOuter = bvNone
      BorderStyle = bsSingle
      Color = clWhite
      Ctl3D = False
      ParentBackground = False
      ParentCtl3D = False
      TabOrder = 1
      TabStop = True
      OnClick = paColorClick
    end
    object tbOpacity: TTrackBar
      AlignWithMargins = True
      Left = 251
      Top = 0
      Width = 150
      Height = 25
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 10
      Margins.Bottom = 0
      Align = alRight
      Max = 100
      Position = 100
      SelEnd = 100
      ShowSelRange = False
      TabOrder = 0
      TickStyle = tsNone
    end
  end
  object cdColor: TColorDialog
    Left = 218
  end
end
