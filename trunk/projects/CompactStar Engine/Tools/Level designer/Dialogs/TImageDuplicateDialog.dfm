object ImageDuplicateDialog: TImageDuplicateDialog
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'This image has a duplicate'
  ClientHeight = 249
  ClientWidth = 400
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 13
  object paLeft: TPanel
    Left = 0
    Top = 0
    Width = 111
    Height = 249
    Align = alLeft
    BevelOuter = bvNone
    TabOrder = 0
    object imCurrent: TImage
      AlignWithMargins = True
      Left = 3
      Top = 16
      Width = 105
      Height = 105
      Align = alTop
      Center = True
      Proportional = True
      Stretch = True
      ExplicitTop = 13
    end
    object imNew: TImage
      AlignWithMargins = True
      Left = 3
      Top = 140
      Width = 105
      Height = 105
      Align = alTop
      Center = True
      Proportional = True
      Stretch = True
      ExplicitLeft = 6
      ExplicitTop = 252
    end
    object laCurrent: TLabel
      Left = 0
      Top = 0
      Width = 111
      Height = 13
      Align = alTop
      Caption = 'Current image'
      ExplicitWidth = 68
    end
    object laNew: TLabel
      Left = 0
      Top = 124
      Width = 111
      Height = 13
      Align = alTop
      Caption = 'New image'
      ExplicitWidth = 52
    end
  end
  object paRight: TPanel
    Left = 111
    Top = 0
    Width = 289
    Height = 249
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 1
    object laCaption: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 3
      Width = 283
      Height = 13
      Align = alTop
      Caption = 'Another file with a similar name already exists:'
      WordWrap = True
      ExplicitWidth = 223
    end
    object laFileName: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 22
      Width = 283
      Height = 13
      Align = alTop
      Caption = '##########'
      ExplicitWidth = 80
    end
    object laDuplicatePrefix: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 159
      Width = 283
      Height = 13
      Align = alTop
      Caption = 'Duplicate prefix'
      Enabled = False
      ExplicitWidth = 75
    end
    object paButtons: TPanel
      Left = 0
      Top = 218
      Width = 289
      Height = 31
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 1
      object btOk: TButton
        AlignWithMargins = True
        Left = 211
        Top = 3
        Width = 75
        Height = 25
        Align = alRight
        Caption = 'Ok'
        TabOrder = 0
        OnClick = btOkClick
      end
    end
    object rgAction: TRadioGroup
      AlignWithMargins = True
      Left = 3
      Top = 48
      Width = 283
      Height = 105
      Margins.Top = 10
      Align = alTop
      Caption = 'Please select an action to apply'
      ItemIndex = 0
      Items.Strings = (
        'Keep the current image'
        'Keep the new image'
        'Generate a duplicate file')
      TabOrder = 0
      OnClick = rgActionClick
    end
    object edDuplicatePrefix: TEdit
      AlignWithMargins = True
      Left = 3
      Top = 178
      Width = 283
      Height = 21
      Align = alTop
      Enabled = False
      TabOrder = 2
    end
  end
end
