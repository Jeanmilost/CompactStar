object SoundSelection: TSoundSelection
  Left = 0
  Top = 0
  AutoSize = True
  BorderStyle = bsDialog
  Caption = 'Sound selection'
  ClientHeight = 113
  ClientWidth = 400
  Color = clBtnFace
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
  object blBottomLine: TBevel
    AlignWithMargins = True
    Left = 3
    Top = 76
    Width = 394
    Height = 1
    Align = alTop
    Shape = bsTopLine
    ExplicitTop = 299
  end
  object paSound: TPanel
    Left = 0
    Top = 0
    Width = 400
    Height = 73
    Align = alTop
    AutoSize = True
    BevelOuter = bvNone
    TabOrder = 0
    object laSound: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 3
      Width = 394
      Height = 24
      Align = alTop
      Caption = 'Select an ambient sound'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -20
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      ExplicitWidth = 220
    end
    object paSoundFile: TPanel
      AlignWithMargins = True
      Left = 3
      Top = 33
      Width = 394
      Height = 37
      Align = alTop
      AutoSize = True
      BevelOuter = bvNone
      TabOrder = 0
      object laSoundFileName: TLabel
        AlignWithMargins = True
        Left = 3
        Top = 0
        Width = 388
        Height = 13
        Margins.Top = 0
        Margins.Bottom = 0
        Align = alTop
        Caption = 'Select a file'
        ExplicitWidth = 55
      end
      object paSoundFileName: TPanel
        AlignWithMargins = True
        Left = 3
        Top = 16
        Width = 388
        Height = 21
        Margins.Bottom = 0
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object edSoundFileName: TEdit
          AlignWithMargins = True
          Left = 0
          Top = 0
          Width = 364
          Height = 21
          Margins.Left = 0
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alClient
          ReadOnly = True
          TabOrder = 0
        end
        object btSoundBrowse: TButton
          AlignWithMargins = True
          Left = 367
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
          OnClick = btSoundBrowseClick
        end
      end
    end
  end
  object paButtons: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 85
    Width = 394
    Height = 25
    Margins.Top = 5
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    object btOk: TButton
      Left = 319
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
  object odSound: TOpenDialog
    Filter = 'All sound files|*.wav|Wave sound|*.wav'
    Options = [ofHideReadOnly, ofFileMustExist, ofEnableSizing]
    Left = 361
    Top = 5
  end
end
