object ShapeSelection: TShapeSelection
  Left = 0
  Top = 0
  AutoSize = True
  BorderStyle = bsDialog
  Caption = 'Add a %s'
  ClientHeight = 554
  ClientWidth = 315
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
  object laTransform: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 3
    Width = 309
    Height = 25
    Align = alTop
    Caption = 'Transform'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ExplicitWidth = 96
  end
  object laPosition: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 34
    Width = 309
    Height = 13
    Align = alTop
    Caption = 'Position'
    ExplicitWidth = 37
  end
  object laRotation: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 97
    Width = 309
    Height = 13
    Align = alTop
    Caption = 'Rotation'
    ExplicitWidth = 41
  end
  object laScaling: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 160
    Width = 309
    Height = 13
    Align = alTop
    Caption = 'Scaling'
    ExplicitWidth = 33
  end
  object laOptions: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 357
    Width = 309
    Height = 25
    Margins.Top = 0
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
  object blBottomLine: TBevel
    AlignWithMargins = True
    Left = 3
    Top = 519
    Width = 309
    Height = 1
    Align = alTop
    Shape = bsTopLine
    ExplicitLeft = -79
    ExplicitTop = 383
    ExplicitWidth = 394
  end
  object blMiddleLine: TBevel
    AlignWithMargins = True
    Left = 3
    Top = 353
    Width = 309
    Height = 1
    Margins.Top = 6
    Align = alTop
    Shape = bsTopLine
    ExplicitLeft = -2
    ExplicitTop = 350
  end
  object blTopLine: TBevel
    AlignWithMargins = True
    Left = 3
    Top = 223
    Width = 309
    Height = 1
    Align = alTop
    Shape = bsTopLine
    ExplicitLeft = -2
    ExplicitTop = 216
  end
  inline vfPosition: TVector3Frame
    Left = 0
    Top = 50
    Width = 315
    Height = 44
    Align = alTop
    DoubleBuffered = True
    Padding.Left = 3
    Padding.Top = 3
    Padding.Right = 3
    Padding.Bottom = 3
    ParentDoubleBuffered = False
    TabOrder = 0
    ExplicitTop = 50
    ExplicitWidth = 315
    inherited paLabels: TPanel
      Width = 309
      ExplicitWidth = 309
    end
    inherited paValues: TPanel
      Width = 309
      ExplicitWidth = 309
    end
  end
  inline vfRotation: TVector3Frame
    Left = 0
    Top = 113
    Width = 315
    Height = 44
    Align = alTop
    DoubleBuffered = True
    Padding.Left = 3
    Padding.Top = 3
    Padding.Right = 3
    Padding.Bottom = 3
    ParentDoubleBuffered = False
    TabOrder = 1
    ExplicitTop = 113
    ExplicitWidth = 315
    inherited paLabels: TPanel
      Width = 309
      ExplicitWidth = 309
    end
    inherited paValues: TPanel
      Width = 309
      ExplicitWidth = 309
    end
  end
  inline vfScaling: TVector3Frame
    Left = 0
    Top = 176
    Width = 315
    Height = 44
    Align = alTop
    DoubleBuffered = True
    Padding.Left = 3
    Padding.Top = 3
    Padding.Right = 3
    Padding.Bottom = 3
    ParentDoubleBuffered = False
    TabOrder = 2
    ExplicitTop = 176
    ExplicitWidth = 315
    inherited paLabels: TPanel
      Width = 309
      ExplicitWidth = 309
    end
    inherited paValues: TPanel
      Width = 309
      ExplicitWidth = 309
    end
  end
  object paShapeTexture: TPanel
    Left = 0
    Top = 227
    Width = 315
    Height = 120
    Align = alTop
    AutoSize = True
    BevelOuter = bvNone
    TabOrder = 3
    object laTexture: TLabel
      AlignWithMargins = True
      Left = 3
      Top = 0
      Width = 309
      Height = 24
      Margins.Top = 0
      Align = alTop
      Caption = 'Texture'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -20
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      ExplicitWidth = 69
    end
    object paTexture: TPanel
      AlignWithMargins = True
      Left = 20
      Top = 30
      Width = 292
      Height = 90
      Margins.Left = 20
      Margins.Bottom = 0
      Align = alTop
      BevelOuter = bvNone
      TabOrder = 0
      object imTexture: TImage
        AlignWithMargins = True
        Left = 0
        Top = 0
        Width = 90
        Height = 90
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alLeft
        Center = True
        Picture.Data = {
          0954506E67496D61676589504E470D0A1A0A0000000D49484452000000640000
          0064080600000070E295540000000473424954080808087C0864880000000970
          485973000038C9000038C901774E6AC20000001974455874536F667477617265
          007777772E696E6B73636170652E6F72679BEE3C1A000006DC4944415478DAED
          9D7D6C136518C09FF7D66D84F1B1C180100DFE81A28221E8305B0B98865E3FC6
          4624042511A3F8079A880A7E4645604685040D09060D242448A2F163212120E9
          AE1FD844B276E05043E4239818A3C198094360C176ED3D3ED75D471D15AE5BE1
          DEA4CF2FB9DCFB5EDFF7DEF79E5FEF7ABDBBBE159047837B717D5545EA7910D0
          4AD9BB69AA81C2A401C421BD523CD2150C5E048B383DFE0008DC45756FB35AA7
          00FFD0B4231ED15E2CA692530DBC0A80EB293976046D9F47C49712D1D01EAB15
          DC6EB723E9A836CA2FA3A9CA5CDC27004EE9800774CC6C3F128D9ECB95175783
          E55B0242ECA664ADE5EE09581B0F6BDBAC1677A9FED30830630401194457706E
          5728D46DA56CA3C73345118EB394544AD0745F753A591B8BC5D216B7F961DAE6
          7DD729D28B42AC4C843BF61B99AC1053C6DE623B8C025625C2DA2EABE59DAAFF
          18CDEE2F41501021332B11899CB452787E4B4B5D2699FE8392D5236D9802F657
          E77CE714686BD3AD946F52FD2AD509DFA05886A42C35A48805CDCD93D2FDFA19
          5A387E4821DA8DC4DFFF138F7E012294ECED79B9BBBBBBDFEAC6B8BCDE393A2A
          5BA9EE1DC30F89DE2750D9D119EDF8B8985A2E8F6F196DF41BB44DD68F00D76E
          F79F42C1B73A43A1434554124EAF7F3D202CA7E428733D461F260C29D79BC1F4
          5D828EEB6F93F60D792FF450E527E2514D336A0EBFF3CC7510F3BC8166FA3CDA
          4301AEBFBA14360A3A8C7C4FC939796503F1488766778FCB0192B248473C98CB
          D3A1ADDB107219CCB3A9ECF131A24D06DE336E1546FC8D33AC3A337FC958901F
          FC53743A79AFDDBD2C27869C79220BB1198AFF2918F8CE67C042EC868548060B
          910C162219147FE35BBC6A667F652136D3E8699EA9087D07256B1484570C2157
          28637EA5871F484829AE3531C34434797DDB048A17B21914AB8BBD46C49496EC
          D5DE795EEFDC0C629FD5ABA7CCCD438C7C154C29612192C14224838548060B91
          0C1622192C4432588864B0109B71BBDDA39295D5CF802E46830377B2109B71AA
          819D00F8B49146806FB2425C3EDFE4AA542A158BC52ED8DDC172C3A9FA4FD02C
          77853D2D9A3CFED78480CD94E9273B2B3A23DA5EBB3B594E14BA41653C2C6D3E
          802CBE8B473A1EB4BB93E504DF31940C1622192C4432588864B010C9602192C1
          4224838548060B910C1622192C4432588864B010C9A0F81B4F8BDE6366751662
          332E35F02902AE30B3475988CD34A8EAF82A70BC4E47AB316905B7E47E963B30
          AA8080CE78589B677727CB19E152FD4FD12EB29DD25784828F16396C045362B2
          F7D48D2184626EB76E754015E6E6C14F9D48060B910C1622192C4432588864B0
          10C9602192C142248385D8CC7C559D96818A0FC8C41801FA9B2CC4669C9EC07E
          10B8D8CC9E160D0D0D9555B5135B5088CB898816051E6FF19652E806D5679478
          6C200B1BE251ED1DBB3B594E14129284C131C9F1783C129A6D7727CB09BE852B
          192C4432588864B010C9602192C14224838548060B910C1622192C4432588864
          B010C960219241F13F4EB3FBCC6C8A85D88C4BF5BF4B02D699D97616623FC2E9
          F1193708475FAA1BB7CF10F2332D9C6EBE789084B4DADDC3724634F97C8D22A3
          6CA2745FA6425F732414FAC5EE4E9533FC908364B010C9602192C14224838548
          060B910C1622192C4432588864B0109B71F97C0FA12E3EA7640D4DCFB2109B71
          AA81C300981BCEA4472CF0FBA76674588B007DD5FDC9ADB158ECB2DD9D2C270A
          DDA04A50A2319B15624F3CDCB1D2EE4E961385846428A1980B4EC423DA2CBB3B
          594EF02D5CC970A9FED324608699CD0A313E336ACC05E748C824E09FB5DD2A72
          E395D599F94BC6826394B8FA1FEA08CDF1A8D661774FCB01DA3B5AE89DFF752E
          4FA7BCDDC2A506DA1070635EB91ED4F1C9C4A1902185F7949B833170DC229A7F
          4201AE1F5C8AB04134B817D757395267285B3BA4D279AA57E04FC23083885D58
          55B1BA2B18BC68B507D9DF63A3633B089C699CCE0D6F3B30499D6EA73DB80D8A
          78B30C7CF9CADEA69E3AFC18E205DAEE2D8968E8CB626A51E057E908AB851063
          F3D665C47AC290A2BD5001776603E3F4F896508C8C3F0353AC362410367546B5
          7556CB3BBD8176405C36FC80E4B50DD0DA19D10E5A6E5BF59F8511C91824E5A8
          546EFF3618ECB152D8B5B0793A2AFA19B8F115910C0AB13411EED83F58D094B2
          1BAEDD530A82023F4C84436B8A084A9066811204C5D835962722DA57960AB7B5
          29CEC371DADB617C299AD6D3CAB4AE58F0772B851B3D81D98AC01F6F50AC9764
          AC34641899FF98330E5FD58EFEE7A8DD561C18DCB7A6C00AE825D19D56F4A547
          43A1DFAC6E49D342FF0342812F2879D70802D24FD3BE8B75E31EFFA9BD3D65B9
          6D8F6F051D32B65172E208DAEEA3CFDACD8948E8BD622A39D5C0FB14328A298C
          CA5F17C5F0241D660E64F4F44747A2D173B917FE0531C73269550D0E0A000000
          0049454E44AE426082}
        Proportional = True
        Stretch = True
        ExplicitLeft = 8
        ExplicitHeight = 167
      end
      object paTextureFile: TPanel
        AlignWithMargins = True
        Left = 90
        Top = 0
        Width = 202
        Height = 90
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 0
        object laTextureFileName: TLabel
          AlignWithMargins = True
          Left = 3
          Top = 53
          Width = 196
          Height = 13
          Margins.Top = 0
          Margins.Bottom = 0
          Align = alBottom
          Caption = 'Select a file'
          ExplicitWidth = 55
        end
        object paTextureScreenshot: TPanel
          AlignWithMargins = True
          Left = 3
          Top = 69
          Width = 196
          Height = 21
          Margins.Bottom = 0
          Align = alBottom
          BevelOuter = bvNone
          TabOrder = 0
          object edTextureFileName: TEdit
            AlignWithMargins = True
            Left = 0
            Top = 0
            Width = 172
            Height = 21
            Margins.Left = 0
            Margins.Top = 0
            Margins.Bottom = 0
            Align = alClient
            ReadOnly = True
            TabOrder = 0
          end
          object btTextureBrowse: TButton
            AlignWithMargins = True
            Left = 175
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
            OnClick = btTextureBrowseClick
          end
        end
      end
    end
  end
  object ckRepeatTextureOnEachFace: TCheckBox
    AlignWithMargins = True
    Left = 3
    Top = 388
    Width = 309
    Height = 17
    Align = alTop
    Caption = 'Repeat texture on each face'
    TabOrder = 4
    ExplicitLeft = -2
  end
  object paButtons: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 526
    Width = 309
    Height = 25
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 5
    ExplicitTop = 418
    object btOk: TButton
      Left = 234
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
  object paSlicesAndStacks: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 411
    Width = 309
    Height = 21
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 6
    ExplicitLeft = 8
    object laStacks: TLabel
      AlignWithMargins = True
      Left = 198
      Top = 0
      Width = 60
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 5
      Margins.Bottom = 0
      Align = alRight
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Stacks'
      Layout = tlCenter
      ExplicitLeft = 207
    end
    object laSlices: TLabel
      AlignWithMargins = True
      Left = 0
      Top = 0
      Width = 60
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 5
      Margins.Bottom = 0
      Align = alLeft
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Slices'
      Layout = tlCenter
    end
    object edSlices: TEdit
      AlignWithMargins = True
      Left = 65
      Top = 0
      Width = 30
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alLeft
      NumbersOnly = True
      ReadOnly = True
      TabOrder = 0
      Text = '25'
    end
    object udSlices: TUpDown
      Left = 95
      Top = 0
      Width = 16
      Height = 21
      Associate = edSlices
      Min = 3
      Max = 50
      Position = 25
      TabOrder = 1
    end
    object edStacks: TEdit
      Left = 263
      Top = 0
      Width = 30
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 16
      Margins.Bottom = 0
      Align = alRight
      NumbersOnly = True
      ReadOnly = True
      TabOrder = 2
      Text = '25'
    end
    object udStacks: TUpDown
      Left = 293
      Top = 0
      Width = 16
      Height = 21
      Associate = edStacks
      Min = 2
      Max = 50
      Position = 25
      TabOrder = 3
    end
  end
  object paFaces: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 438
    Width = 309
    Height = 21
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 7
    ExplicitLeft = 6
    ExplicitTop = 419
    object laFaces: TLabel
      AlignWithMargins = True
      Left = 0
      Top = 0
      Width = 60
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 5
      Margins.Bottom = 0
      Align = alLeft
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Faces'
      Layout = tlCenter
    end
    object edFaces: TEdit
      AlignWithMargins = True
      Left = 65
      Top = 0
      Width = 30
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alLeft
      NumbersOnly = True
      ReadOnly = True
      TabOrder = 0
      Text = '25'
    end
    object udFaces: TUpDown
      Left = 95
      Top = 0
      Width = 16
      Height = 21
      Associate = edFaces
      Min = 3
      Max = 50
      Position = 25
      TabOrder = 1
    end
  end
  object paMinRadius: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 465
    Width = 309
    Height = 21
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 9
    ExplicitLeft = 35
    ExplicitTop = 452
    object laMinRadius: TLabel
      AlignWithMargins = True
      Left = 0
      Top = 0
      Width = 60
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 5
      Margins.Bottom = 0
      Align = alLeft
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Min. Radius'
      Layout = tlCenter
    end
    object laMinRadiusPercent: TLabel
      AlignWithMargins = True
      Left = 115
      Top = 0
      Width = 11
      Height = 21
      Margins.Left = 20
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alLeft
      Caption = '%'
      Layout = tlCenter
      ExplicitLeft = 151
      ExplicitHeight = 13
    end
    object edMinRadius: TEdit
      AlignWithMargins = True
      Left = 65
      Top = 0
      Width = 30
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alLeft
      NumbersOnly = True
      ReadOnly = True
      TabOrder = 0
      Text = '50'
      ExplicitTop = 3
    end
    object udMinRadius: TUpDown
      Left = 95
      Top = 0
      Width = 16
      Height = 21
      Associate = edMinRadius
      Min = 1
      Max = 99
      Position = 50
      TabOrder = 1
    end
  end
  object paDeltas: TPanel
    AlignWithMargins = True
    Left = 3
    Top = 492
    Width = 309
    Height = 21
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 8
    ExplicitLeft = -2
    ExplicitTop = 478
    object laDeltaMiax: TLabel
      AlignWithMargins = True
      Left = 111
      Top = 0
      Width = 55
      Height = 21
      Margins.Left = 16
      Margins.Top = 0
      Margins.Right = 5
      Margins.Bottom = 0
      Align = alLeft
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Delta max'
      Layout = tlCenter
      ExplicitLeft = 95
    end
    object laDeltaMin: TLabel
      AlignWithMargins = True
      Left = 0
      Top = 0
      Width = 60
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 5
      Margins.Bottom = 0
      Align = alLeft
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Delta min'
      Layout = tlCenter
      ExplicitLeft = -44
      ExplicitTop = -3
    end
    object laDeltaZ: TLabel
      AlignWithMargins = True
      Left = 203
      Top = 0
      Width = 55
      Height = 21
      Margins.Left = 16
      Margins.Top = 0
      Margins.Right = 5
      Margins.Bottom = 0
      Align = alRight
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Delta Z'
      Layout = tlCenter
      ExplicitLeft = 198
    end
    object edDeltaMin: TEdit
      AlignWithMargins = True
      Left = 65
      Top = 0
      Width = 30
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alLeft
      NumbersOnly = True
      ReadOnly = True
      TabOrder = 0
      Text = '0'
    end
    object udDeltaMin: TUpDown
      Left = 95
      Top = 0
      Width = 16
      Height = 21
      Associate = edDeltaMin
      Max = 1000
      TabOrder = 1
    end
    object edDeltaMax: TEdit
      Left = 171
      Top = 0
      Width = 30
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 16
      Margins.Bottom = 0
      Align = alLeft
      NumbersOnly = True
      ReadOnly = True
      TabOrder = 2
      Text = '0'
    end
    object udDeltaMax: TUpDown
      Left = 201
      Top = 0
      Width = 16
      Height = 21
      Associate = edDeltaMax
      Max = 1000
      TabOrder = 3
    end
    object edDeltaZ: TEdit
      AlignWithMargins = True
      Left = 263
      Top = 0
      Width = 30
      Height = 21
      Margins.Left = 0
      Margins.Top = 0
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alRight
      NumbersOnly = True
      ReadOnly = True
      TabOrder = 4
      Text = '1'
    end
    object udDeltaZ: TUpDown
      Left = 293
      Top = 0
      Width = 16
      Height = 21
      Associate = edDeltaZ
      Max = 1000
      Position = 1
      TabOrder = 5
    end
  end
  object opdPicture: TOpenPictureDialog
    Options = [ofHideReadOnly, ofFileMustExist, ofEnableSizing]
    Left = 279
    Top = 5
  end
end
