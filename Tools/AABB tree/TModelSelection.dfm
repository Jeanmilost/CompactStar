object ModelSelection: TModelSelection
  Left = 0
  Top = 0
  AutoSize = True
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Select a model'
  ClientHeight = 246
  ClientWidth = 388
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  PrintScale = poNone
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 13
  object paShape: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 382
    Height = 178
    Align = alTop
    AutoSize = True
    BevelOuter = bvNone
    ParentColor = True
    TabOrder = 0
    object rgShapes: TRadioGroup
      Left = 0
      Top = 0
      Width = 382
      Height = 97
      Align = alTop
      Caption = 'Shapes'
      Columns = 2
      Items.Strings = (
        'Surface'
        'Box'
        'Sphere'
        'Cylinder'
        'Disk'
        'Ring'
        'Spiral'
        'Model')
      TabOrder = 0
      OnClick = rgShapesClick
      ExplicitTop = 3
    end
    object paSlices: TPanel
      AlignWithMargins = True
      Left = 3
      Top = 100
      Width = 376
      Height = 21
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 1
      ExplicitTop = 103
      object laSlices: TLabel
        AlignWithMargins = True
        Left = 0
        Top = 0
        Width = 26
        Height = 20
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 1
        Align = alLeft
        Caption = 'Slices'
        Transparent = True
        Layout = tlCenter
        ExplicitHeight = 13
      end
      object edSlices: TEdit
        AlignWithMargins = True
        Left = 98
        Top = 0
        Width = 262
        Height = 21
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alRight
        NumbersOnly = True
        TabOrder = 0
        Text = '10'
        OnChange = edSlicesAndStacksChange
      end
      object udSlices: TUpDown
        Left = 360
        Top = 0
        Width = 16
        Height = 21
        Associate = edSlices
        Min = 1
        Max = 1000
        Position = 10
        TabOrder = 1
      end
    end
    object paStacks: TPanel
      AlignWithMargins = True
      Left = 3
      Top = 127
      Width = 376
      Height = 21
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 2
      ExplicitTop = 130
      object laStacks: TLabel
        AlignWithMargins = True
        Left = 0
        Top = 0
        Width = 31
        Height = 20
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 1
        Align = alLeft
        Caption = 'Stacks'
        Transparent = True
        Layout = tlCenter
        ExplicitHeight = 13
      end
      object edStacks: TEdit
        AlignWithMargins = True
        Left = 98
        Top = 0
        Width = 262
        Height = 21
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alRight
        NumbersOnly = True
        TabOrder = 0
        Text = '10'
        OnChange = edSlicesAndStacksChange
      end
      object udStacks: TUpDown
        Left = 360
        Top = 0
        Width = 16
        Height = 21
        Associate = edStacks
        Min = 1
        Max = 1000
        Position = 10
        TabOrder = 1
      end
    end
    object paFaces: TPanel
      AlignWithMargins = True
      Left = 3
      Top = 154
      Width = 376
      Height = 21
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 3
      ExplicitTop = 167
      object laFaces: TLabel
        AlignWithMargins = True
        Left = 0
        Top = 0
        Width = 28
        Height = 20
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 1
        Align = alLeft
        Caption = 'Faces'
        Transparent = True
        Layout = tlCenter
        ExplicitHeight = 13
      end
      object edFaces: TEdit
        AlignWithMargins = True
        Left = 98
        Top = 0
        Width = 262
        Height = 21
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alRight
        NumbersOnly = True
        TabOrder = 0
        Text = '10'
        OnChange = edSlicesAndStacksChange
      end
      object udFaces: TUpDown
        Left = 360
        Top = 0
        Width = 16
        Height = 21
        Associate = edFaces
        Min = 1
        Max = 1000
        Position = 10
        TabOrder = 1
      end
    end
  end
  object btSelect: TButton
    AlignWithMargins = True
    Left = 3
    Top = 218
    Width = 382
    Height = 25
    Align = alTop
    Caption = 'Select'
    TabOrder = 1
    OnClick = btSelectClick
    ExplicitTop = 164
  end
  object paMDLModel: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 187
    Width = 382
    Height = 25
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 2
    ExplicitTop = 133
    object laSelectMDLModel: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 2
      Width = 92
      Height = 20
      Margins.Top = 2
      Align = alLeft
      Caption = 'Select a MDL model'
      Layout = tlCenter
      ExplicitHeight = 13
    end
    object btOpenFile: TButton
      Left = 357
      Top = 0
      Width = 25
      Height = 25
      Margins.Top = 2
      Margins.Bottom = 2
      Align = alRight
      Caption = '...'
      TabOrder = 0
      OnClick = btOpenFileClick
      ExplicitLeft = 352
    end
    object edMDLFilelName: TEdit
      AlignWithMargins = True
      Left = 101
      Top = 2
      Width = 253
      Height = 21
      Margins.Top = 2
      Margins.Bottom = 2
      Align = alClient
      ReadOnly = True
      TabOrder = 1
      ExplicitWidth = 233
    end
  end
  object odOpen: TOpenDialog
    DefaultExt = '.mdl'
    Filter = 'Quake I model|*.mdl'
    Left = 344
    Top = 11
  end
end
