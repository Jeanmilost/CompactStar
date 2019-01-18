object LevelSelection: TLevelSelection
  Left = 0
  Top = 0
  AutoSize = True
  BorderStyle = bsDialog
  Caption = 'Save the level'
  ClientHeight = 167
  ClientWidth = 426
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  PrintScale = poNone
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 13
  object laFileName: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 420
    Height = 25
    Align = alTop
    Caption = 'Model file name'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ExplicitWidth = 149
  end
  object blBottomLine: TBevel
    AlignWithMargins = True
    Left = 3
    Top = 132
    Width = 420
    Height = 1
    Margins.Top = 6
    Align = alTop
    Shape = bsTopLine
    ExplicitLeft = -2
    ExplicitTop = 125
    ExplicitWidth = 458
  end
  object laOptions: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 81
    Width = 420
    Height = 25
    Align = alTop
    Caption = 'Options'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ExplicitWidth = 71
  end
  object blTopLine: TBevel
    AlignWithMargins = True
    Left = 3
    Top = 74
    Width = 420
    Height = 1
    Margins.Top = 6
    Align = alTop
    Shape = bsTopLine
    ExplicitLeft = 27
    ExplicitWidth = 458
  end
  object paButtons: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 139
    Width = 420
    Height = 25
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object btOk: TButton
      Left = 345
      Top = 0
      Width = 75
      Height = 25
      Align = alRight
      Caption = 'Ok'
      TabOrder = 1
      OnClick = btOkClick
    end
    object btCancel: TButton
      Left = 0
      Top = 0
      Width = 75
      Height = 25
      Align = alLeft
      Caption = 'Cancel'
      TabOrder = 0
      OnClick = btCancelClick
    end
  end
  object paLevel: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 31
    Width = 420
    Height = 37
    Margins.Top = 0
    Margins.Bottom = 0
    Align = alTop
    AutoSize = True
    BevelOuter = bvNone
    TabOrder = 1
    object laFileNameTitle: TLabel
      AlignWithMargins = True
      Left = 0
      Top = 0
      Width = 55
      Height = 13
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alTop
      Caption = 'Select a file'
    end
    object paFileName: TPanel
      AlignWithMargins = True
      Left = 0
      Top = 16
      Width = 420
      Height = 21
      Margins.Left = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 0
      object edFileName: TEdit
        AlignWithMargins = True
        Left = 0
        Top = 0
        Width = 399
        Height = 21
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alClient
        Enabled = False
        ReadOnly = True
        TabOrder = 0
      end
      object btBrowse: TButton
        AlignWithMargins = True
        Left = 399
        Top = 0
        Width = 21
        Height = 21
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alRight
        Caption = '...'
        TabOrder = 1
        OnClick = btBrowseClick
      end
    end
  end
  object ckSaveFileContent: TCheckBox
    Left = 0
    Top = 109
    Width = 426
    Height = 17
    Align = alTop
    Caption = 'Save the file content'
    TabOrder = 2
  end
  object sdSave: TSaveDialog
    DefaultExt = '.xml'
    Filter = 'All level files|*.xml|XML file|*.xml'
    Options = [ofOverwritePrompt, ofEnableSizing]
    Left = 387
    Top = 3
  end
end
