object FileFrame: TFileFrame
  Left = 0
  Top = 0
  Width = 436
  Height = 25
  TabOrder = 0
  object edFileName: TEdit
    AlignWithMargins = True
    Left = 3
    Top = 2
    Width = 397
    Height = 21
    Margins.Top = 2
    Margins.Right = 0
    Margins.Bottom = 2
    Align = alClient
    ReadOnly = True
    TabOrder = 0
  end
  object btBrowse: TButton
    AlignWithMargins = True
    Left = 403
    Top = 0
    Width = 30
    Height = 25
    Margins.Top = 0
    Margins.Bottom = 0
    Align = alRight
    Caption = '...'
    TabOrder = 1
    OnClick = btBrowseClick
  end
  object odOpen: TOpenDialog
    Filter = 
      'Quake I Models|*.mdl|WaveFront Models|*.obj|All Model Files|*.md' +
      'l; *.obj'
    FilterIndex = 3
    Left = 368
  end
end
