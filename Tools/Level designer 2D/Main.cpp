/****************************************************************************
 * ==> Level designer for 2D games -----------------------------------------*
 ****************************************************************************
 * Description : CompactStar Engine level designer for 2D games             *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2022, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#include <vcl.h>
#pragma hdrstop
#include "Main.h"

// vcl
#include <Vcl.Imaging.jpeg.hpp>
#include <Vcl.Imaging.pngimage.hpp>

// std
#include <memory>
#include <sstream>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Model.h"
#include "CSR_Mdl.h"
#include "CSR_X.h"
#include "CSR_Wavefront.h"
#include "CSR_Scene.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_ShaderHelper.h"
#include "CSR_LevelFile_XML.h"

// interface
#include "TLandscapeSelection.h"
#include "TShapeSelection.h"
#include "TModelSelection.h"
#include "TSoundSelection.h"
#include "TLevelSelection.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TMainForm::ICommand
//---------------------------------------------------------------------------
TMainForm::ICommand::ICommand() :
    m_Command(IE_C_None),
    m_pKey(NULL),
    m_Index(-1)
{}
//---------------------------------------------------------------------------
TMainForm::ICommand::~ICommand()
{}
//---------------------------------------------------------------------------
void TMainForm::ICommand::Clear()
{
    m_Command =  IE_C_None;
    m_pKey    =  NULL;
    m_Index   = -1;
}
//---------------------------------------------------------------------------
// TMainForm::IDesignerProperties
//---------------------------------------------------------------------------
TMainForm::IDesignerProperties::IDesignerProperties() :
    m_pTransformTranslate(NULL),
    m_pTransformRotate(NULL),
    m_pTransformScale(NULL)
{}
//---------------------------------------------------------------------------
TMainForm::IDesignerProperties::~IDesignerProperties()
{}
//---------------------------------------------------------------------------
void TMainForm::IDesignerProperties::Clear()
{
    m_pTransformTranslate = NULL;
    m_pTransformRotate    = NULL;
    m_pTransformScale     = NULL;
}
//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_hDC(NULL),
    m_hRC(NULL),
    m_FrameCount(0),
    m_PrevOrigin(0),
    m_Angle(0.0f),
    m_PosVelocity(0.0f),
    m_DirVelocity(0.0f),
    m_FPS(0.0),
    m_StartTime(0),
    m_PreviousTime(0),
    m_PrevDesignerTime(0),
    m_Initialized(false)
{
    // build the model dir from the application exe
    UnicodeString sceneDir = ::ExtractFilePath(Application->ExeName);
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExcludeTrailingPathDelimiter(sceneDir) + L"\\Scenes";
                m_SceneDir =   AnsiString(sceneDir).c_str();

    // set the open level dialog box initial dir
    odLevelOpen->InitialDir = sceneDir;

    // enable OpenGL
    CSR_OpenGLHelper::EnableOpenGL(paEngineView->Handle, &m_hDC, &m_hRC);

    // stop GLEW crashing on OSX :-/
    glewExperimental = GL_TRUE;

    // initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        // shutdown OpenGL
        CSR_OpenGLHelper::DisableOpenGL(Handle, m_hDC, m_hRC);

        // close the app
        Application->Terminate();
    }

    // create the level manager
    m_pLevel.reset(new CSR_Level());

    // create a designer view
    m_pDesignerView.reset(new CSR_DesignerView(paDesignerView));
}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm()
{
    // close the document
    CloseDocument();

    // required to reset the hooked procedures
    m_pDesignerView.reset();
    m_pEngineViewHook.reset();

    // delete the scene completely
    DeleteScene();

    // shutdown OpenGL
    CSR_OpenGLHelper::DisableOpenGL(paEngineView->Handle, m_hDC, m_hRC);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject* pSender)
{
    // initialize the scene
    InitScene(paEngineView->ClientWidth, paEngineView->ClientHeight);

    // hook the engine view procedure
    m_pEngineViewHook.reset (new CSR_VCLControlHook(paEngineView, OnEngineViewMessage));

    // initialize the timer
    m_StartTime        = ::GetTickCount();
    m_PreviousTime     = ::GetTickCount();
    m_PrevDesignerTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject* pSender)
{
    if (!m_Initialized)
        return;

    // update the viewport
    m_pLevel->CreateViewport(paEngineView->ClientWidth, paEngineView->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShortCut(TWMKey& msg, bool& handled)
{
    const TShiftState shiftState = ::KeyDataToShiftState(msg.KeyData);

    // search for pressed key
    switch (msg.CharCode)
    {
        case 'x':
        case 'X':
        {
            // ignore any edit event
            if (Application->MainForm->ActiveControl &&
                Application->MainForm->ActiveControl->InheritsFrom(__classid(TEdit)))
                return;

            // found a Ctrl+X shortcut?
            if (!shiftState.Contains(ssCtrl))
                return;

            // get the current selection
            const CSR_DesignerView::ISelection* pSelection = m_pDesignerView->GetSelection();

            // found it?
            if (!pSelection)
                return;

            // get the landscape model
            const CSR_SceneItem* pLandscape = m_pLevel->GetLandscape();

            // landscape model cannot be cut
            if (pSelection->m_pKey == pLandscape->m_pModel)
                return;

            // build the command to process
            ICommand command;
            command.m_Command = IE_C_Cut;
            command.m_pKey    = pSelection->m_pKey;
            command.m_Index   = pSelection->m_MatrixIndex;

            // process the command
            handled = ProcessCommand(command);
            return;
        }

        case 'c':
        case 'C':
        {
            // ignore any edit event
            if (Application->MainForm->ActiveControl &&
                Application->MainForm->ActiveControl->InheritsFrom(__classid(TEdit)))
                return;

            // get the current selection
            const CSR_DesignerView::ISelection* pSelection = m_pDesignerView->GetSelection();

            // found it?
            if (!pSelection)
                return;

            // get the landscape model
            const CSR_SceneItem* pLandscape = m_pLevel->GetLandscape();

            // landscape model cannot be copied
            if (pSelection->m_pKey == pLandscape->m_pModel)
                return;

            // build the command to process
            ICommand command;
            command.m_Command = IE_C_Copy;
            command.m_pKey    = pSelection->m_pKey;
            command.m_Index   = pSelection->m_MatrixIndex;

            // process the command
            handled = ProcessCommand(command);
            return;
        }

        case 'v':
        case 'V':
        {
            // ignore any edit event
            if (Application->MainForm->ActiveControl &&
                Application->MainForm->ActiveControl->InheritsFrom(__classid(TEdit)))
                return;

            // found a Ctrl+V shortcut?
            if (!shiftState.Contains(ssCtrl))
                return;

            // build the command to process
            ICommand command;
            command.m_Command = IE_C_Paste;

            // process the command
            handled = ProcessCommand(command);
            return;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miFileNewClick(TObject* pSender)
{
    // open a new landscape document
    if (!OpenDocument())
        CloseDocument();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miFileLoadClick(TObject* pSender)
{
    // prompt the user about the level file to open
    if (!odLevelOpen->Execute())
        return;

    // close the previously opened document
    CloseDocument();

    CSR_LevelFile_XML levelFile(m_SceneDir, false);
    levelFile.Set_OnLoadCubemap(OnLoadCubemap);
    levelFile.Set_OnLoadTexture(OnLoadTexture);
    levelFile.Set_OnApplySkin(OnApplySkin);
    levelFile.Set_OnSelectModel(OnSelectModel);
    levelFile.Set_OnUpdateDesigner(OnUpdateDesigner);

    // load the level
    if (!levelFile.Load(AnsiString(odLevelOpen->FileName).c_str(), *m_pLevel.get()))
    {
        // show the error message to the user
        ::MessageDlg(L"An error occurred while the level was loaded.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        CloseDocument();
        return;
    }

    // get the scene
    CSR_Scene* pScene = m_pLevel->GetScene();

    // link the scene to the designer view
    m_pDesignerView->SetScene(pScene);

    // initialize the viewpoint
    InitializeViewPoint(&pScene->m_ViewMatrix);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miFileSaveClick(TObject* pSender)
{
    // create a level selection dialog box
    std::auto_ptr<TLevelSelection> pDialog(new TLevelSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // show the dialog box to the user and check if action was canceled
    if (pDialog->ShowModal() != mrOk)
        return;

    // create and configure the level xml serializer
    CSR_LevelFile_XML levelFile(m_SceneDir, pDialog->ckSaveFileContent->Checked);
    levelFile.Set_OnLoadCubemap(OnLoadCubemap);
    levelFile.Set_OnLoadTexture(OnLoadTexture);
    levelFile.Set_OnApplySkin(OnApplySkin);
    levelFile.Set_OnSelectModel(OnSelectModel);
    levelFile.Set_OnUpdateDesigner(OnUpdateDesigner);

    // save the level
    if (!levelFile.Save(AnsiString(pDialog->edFileName->Text).c_str(), *m_pLevel.get()))
        // show the error message to the user
        ::MessageDlg(L"An error occurred while the level was saved.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miFileExitClick(TObject* pSender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miDesignerPlayModeClick(TObject* pSender)
{
    paDesignerView->Visible = !miDesignerPlayMode->Checked;
    paProperties->Visible   = !miDesignerPlayMode->Checked;
    spViews->Visible        = !miDesignerPlayMode->Checked;
    spMainView->Visible     = !miDesignerPlayMode->Checked;

    // realign the designer splitter
    if (paDesignerView->Visible)
        spViews->Top = paDesignerView->Top + paDesignerView->Height;

    // realign the properties splitter
    if (paProperties->Visible)
        spMainView->Top = paProperties->Top + paProperties->Height;

    // update the viewport
    m_pLevel->CreateViewport(paEngineView->ClientWidth, paEngineView->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddBoxClick(TObject* pSender)
{
    // get the current scene
    const CSR_Scene* pScene = m_pLevel->GetScene();

    // found it?
    if (!pScene)
    {
        // show the error message to the user
        ::MessageDlg(L"No model can be added to the scene, because no scene is currently opened.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
        return;
    }

    // create a shape selection dialog box
    std::auto_ptr<TShapeSelection> pShapeSelection
            (new TShapeSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // set the dialog box title
    pShapeSelection->Caption = L"Add a box";

    // hide the useless controls
    pShapeSelection->paSlicesAndStacks->Visible = false;
    pShapeSelection->paFaces->Visible           = false;
    pShapeSelection->paMinRadius->Visible       = false;
    pShapeSelection->paDeltas->Visible          = false;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = 0.5f;

    CSR_CollisionOutput collisionOutput;

    // find the ground position at which the box will be set
    csrSceneDetectCollision(pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(-m_pLevel->GetScene()->m_ViewMatrix.m_Table[3][0]).c_str());
    pShapeSelection->vfPosition->edY->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(collisionOutput.m_GroundPos).c_str());

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    // build the model matrix from the interface
    CSR_Matrix4 matrix;
    pShapeSelection->BuildMatrix(&matrix);

    // add the box to the level
    if (!m_pLevel->AddBox(matrix,
                          AnsiString(pShapeSelection->edTextureFileName->Text).c_str(),
                          pShapeSelection->ckRepeatTextureOnEachFace->Checked,
                          CSR_CO_None,
                          OnLoadTexture,
                          OnSelectModel))
        // show the error message to the user
        ::MessageDlg(L"Failed to create the box.", mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddCylinderClick(TObject* pSender)
{
    // get the current scene
    const CSR_Scene* pScene = m_pLevel->GetScene();

    // found it?
    if (!pScene)
    {
        // show the error message to the user
        ::MessageDlg(L"No model can be added to the scene, because no scene is currently opened.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
        return;
    }

    // create a shape selection dialog box
    std::auto_ptr<TShapeSelection> pShapeSelection
            (new TShapeSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // set the dialog box title
    pShapeSelection->Caption = L"Add a cylinder";

    // hide the useless controls
    pShapeSelection->ckRepeatTextureOnEachFace->Visible = false;
    pShapeSelection->paSlicesAndStacks->Visible         = false;
    pShapeSelection->paMinRadius->Visible               = false;
    pShapeSelection->paDeltas->Visible                  = false;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = 0.5f;

    CSR_CollisionOutput collisionOutput;

    // find the ground position at which the box will be set
    csrSceneDetectCollision(pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(-pScene->m_ViewMatrix.m_Table[3][0]).c_str());
    pShapeSelection->vfPosition->edY->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(collisionOutput.m_GroundPos).c_str());

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    // build the model matrix from the interface
    CSR_Matrix4 matrix;
    pShapeSelection->BuildMatrix(&matrix);

    // add the cylinder to the model
    if (!m_pLevel->AddCylinder(matrix,
                               AnsiString(pShapeSelection->edTextureFileName->Text).c_str(),
                             ::StrToInt(pShapeSelection->edFaces->Text),
                               CSR_CO_None,
                               OnLoadTexture,
                               OnSelectModel))
        // show the error message to the user
        ::MessageDlg(L"Failed to create the cylinder.", mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddDiskClick(TObject* pSender)
{
    // get the current scene
    const CSR_Scene* pScene = m_pLevel->GetScene();

    // found it?
    if (!pScene)
    {
        // show the error message to the user
        ::MessageDlg(L"No model can be added to the scene, because no scene is currently opened.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
        return;
    }

    // create a shape selection dialog box
    std::auto_ptr<TShapeSelection> pShapeSelection
            (new TShapeSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // set the dialog box title
    pShapeSelection->Caption = L"Add a disk";

    // hide the useless controls
    pShapeSelection->ckRepeatTextureOnEachFace->Visible = false;
    pShapeSelection->laStacks->Visible                  = false;
    pShapeSelection->edStacks->Visible                  = false;
    pShapeSelection->udStacks->Visible                  = false;
    pShapeSelection->paFaces->Visible                   = false;
    pShapeSelection->paMinRadius->Visible               = false;
    pShapeSelection->paDeltas->Visible                  = false;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = 0.5f;

    CSR_CollisionOutput collisionOutput;

    // find the ground position at which the box will be set
    csrSceneDetectCollision(pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(-pScene->m_ViewMatrix.m_Table[3][0]).c_str());
    pShapeSelection->vfPosition->edY->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(collisionOutput.m_GroundPos).c_str());

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    // build the model matrix from the interface
    CSR_Matrix4 matrix;
    pShapeSelection->BuildMatrix(&matrix);

    // add the disk to the level
    if (!m_pLevel->AddDisk(matrix,
                           AnsiString(pShapeSelection->edTextureFileName->Text).c_str(),
                         ::StrToInt(pShapeSelection->edSlices->Text),
                           CSR_CO_None,
                           OnLoadTexture,
                           OnSelectModel))
        // show the error message to the user
        ::MessageDlg(L"Failed to create the disk.", mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddRingClick(TObject* pSender)
{
    // get the current scene
    const CSR_Scene* pScene = m_pLevel->GetScene();

    // found it?
    if (!pScene)
    {
        // show the error message to the user
        ::MessageDlg(L"No model can be added to the scene, because no scene is currently opened.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
        return;
    }

    // create a shape selection dialog box
    std::auto_ptr<TShapeSelection> pShapeSelection
            (new TShapeSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // set the dialog box title
    pShapeSelection->Caption = L"Add a ring";

    // hide the useless controls
    pShapeSelection->ckRepeatTextureOnEachFace->Visible = false;
    pShapeSelection->laStacks->Visible                  = false;
    pShapeSelection->edStacks->Visible                  = false;
    pShapeSelection->udStacks->Visible                  = false;
    pShapeSelection->paFaces->Visible                   = false;
    pShapeSelection->paDeltas->Visible                  = false;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = 0.5f;

    CSR_CollisionOutput collisionOutput;

    // find the ground position at which the box will be set
    csrSceneDetectCollision(pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(-pScene->m_ViewMatrix.m_Table[3][0]).c_str());
    pShapeSelection->vfPosition->edY->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(collisionOutput.m_GroundPos).c_str());

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    // build the model matrix from the interface
    CSR_Matrix4 matrix;
    pShapeSelection->BuildMatrix(&matrix);

    // add the ring to the level
    if (!m_pLevel->AddRing(matrix,
                           AnsiString(pShapeSelection->edTextureFileName->Text).c_str(),
                         ::StrToInt(pShapeSelection->edSlices->Text),
                         ::StrToInt(pShapeSelection->edMinRadius->Text),
                           CSR_CO_None,
                           OnLoadTexture,
                           OnSelectModel))
        // show the error message to the user
        ::MessageDlg(L"Failed to create the ring.", mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddSphereClick(TObject* pSender)
{
    // get the current scene
    const CSR_Scene* pScene = m_pLevel->GetScene();

    // found it?
    if (!pScene)
    {
        // show the error message to the user
        ::MessageDlg(L"No model can be added to the scene, because no scene is currently opened.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
        return;
    }

    // create a shape selection dialog box
    std::auto_ptr<TShapeSelection> pShapeSelection
            (new TShapeSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // set the dialog box title
    pShapeSelection->Caption = L"Add a sphere";

    // hide the useless controls
    pShapeSelection->ckRepeatTextureOnEachFace->Visible = false;
    pShapeSelection->paFaces->Visible                   = false;
    pShapeSelection->paMinRadius->Visible               = false;
    pShapeSelection->paDeltas->Visible                  = false;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = 0.5f;

    CSR_CollisionOutput collisionOutput;

    // find the ground position at which the box will be set
    csrSceneDetectCollision(pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(-pScene->m_ViewMatrix.m_Table[3][0]).c_str());
    pShapeSelection->vfPosition->edY->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(collisionOutput.m_GroundPos).c_str());

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    // build the model matrix from the interface
    CSR_Matrix4 matrix;
    pShapeSelection->BuildMatrix(&matrix);

    // add the sphere to the level
    if (!m_pLevel->AddSphere(matrix,
                             AnsiString(pShapeSelection->edTextureFileName->Text).c_str(),
                           ::StrToInt(pShapeSelection->edSlices->Text),
                           ::StrToInt(pShapeSelection->edStacks->Text),
                             CSR_CO_None,
                             OnLoadTexture,
                             OnSelectModel))
        // show the error message to the user
        ::MessageDlg(L"Failed to create the sphere.", mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddSpiralClick(TObject* pSender)
{
    // get the current scene
    const CSR_Scene* pScene = m_pLevel->GetScene();

    // found it?
    if (!pScene)
    {
        // show the error message to the user
        ::MessageDlg(L"No model can be added to the scene, because no scene is currently opened.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
        return;
    }

    // create a shape selection dialog box
    std::auto_ptr<TShapeSelection> pShapeSelection
            (new TShapeSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // set the dialog box title
    pShapeSelection->Caption = L"Add a spiral";

    // hide the useless controls
    pShapeSelection->ckRepeatTextureOnEachFace->Visible = false;
    pShapeSelection->paFaces->Visible                   = false;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = 0.5f;

    CSR_CollisionOutput collisionOutput;

    // find the ground position at which the box will be set
    csrSceneDetectCollision(pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(-pScene->m_ViewMatrix.m_Table[3][0]).c_str());
    pShapeSelection->vfPosition->edY->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(collisionOutput.m_GroundPos).c_str());

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    // build the model matrix from the interface
    CSR_Matrix4 matrix;
    pShapeSelection->BuildMatrix(&matrix);

    // add the spiral to the level
    if (!m_pLevel->AddSpiral(matrix,
                             AnsiString(pShapeSelection->edTextureFileName->Text).c_str(),
                           ::StrToInt(pShapeSelection->edMinRadius->Text),
                           ::StrToInt(pShapeSelection->edDeltaMin->Text),
                           ::StrToInt(pShapeSelection->edDeltaMax->Text),
                           ::StrToInt(pShapeSelection->edDeltaZ->Text),
                           ::StrToInt(pShapeSelection->edSlices->Text),
                           ::StrToInt(pShapeSelection->edStacks->Text),
                             CSR_CO_None,
                             OnLoadTexture,
                             OnSelectModel))
        // show the error message to the user
        ::MessageDlg(L"Failed to create the spiral.", mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddSurfaceClick(TObject* pSender)
{
    // get the current scene
    const CSR_Scene* pScene = m_pLevel->GetScene();

    // found it?
    if (!pScene)
    {
        // show the error message to the user
        ::MessageDlg(L"No model can be added to the scene, because no scene is currently opened.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
        return;
    }

    // create a shape selection dialog box
    std::auto_ptr<TShapeSelection> pShapeSelection
            (new TShapeSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // set the dialog box title
    pShapeSelection->Caption = L"Add a surface";

    // hide the useless controls
    pShapeSelection->laOptions->Visible                 = false;
    pShapeSelection->ckRepeatTextureOnEachFace->Visible = false;
    pShapeSelection->paSlicesAndStacks->Visible         = false;
    pShapeSelection->paFaces->Visible                   = false;
    pShapeSelection->paMinRadius->Visible               = false;
    pShapeSelection->paDeltas->Visible                  = false;
    pShapeSelection->blBottomLine->Visible              = false;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = 0.5f;

    CSR_CollisionOutput collisionOutput;

    // find the ground position at which the box will be set
    csrSceneDetectCollision(pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(-pScene->m_ViewMatrix.m_Table[3][0]).c_str());
    pShapeSelection->vfPosition->edY->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(collisionOutput.m_GroundPos).c_str());

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    // build the model matrix from the interface
    CSR_Matrix4 matrix;
    pShapeSelection->BuildMatrix(&matrix);

    // add the surface to the level
    if (!m_pLevel->AddSurface(matrix,
                              AnsiString(pShapeSelection->edTextureFileName->Text).c_str(),
                              CSR_CO_None,
                              OnLoadTexture,
                              OnSelectModel))
        // show the error message to the user
        ::MessageDlg(L"Failed to create the spiral.", mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddWaveFrontClick(TObject* pSender)
{
    // get the current scene
    const CSR_Scene* pScene = m_pLevel->GetScene();

    // found it?
    if (!pScene)
    {
        // show the error message to the user
        ::MessageDlg(L"No model can be added to the scene, because no scene is currently opened.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
        return;
    }

    // create a model selection dialog box
    std::auto_ptr<TModelSelection> pModelSelection
            (new TModelSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = 0.5f;

    CSR_CollisionOutput collisionOutput;

    // find the ground position at which the box will be set
    csrSceneDetectCollision(pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pModelSelection->vfPosition->edX->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(-pScene->m_ViewMatrix.m_Table[3][0]).c_str());
    pModelSelection->vfPosition->edY->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(collisionOutput.m_GroundPos).c_str());

    // show the default scaling
    pModelSelection->vfScaling->edX->Text = L"1.0";
    pModelSelection->vfScaling->edY->Text = L"1.0";
    pModelSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pModelSelection->ShowModal() != mrOk)
        return;

    // build the model matrix from the interface
    CSR_Matrix4 matrix;
    pModelSelection->BuildMatrix(&matrix);

    // add the WaveFront model to the level
    if (!m_pLevel->AddWaveFront(matrix,
                                AnsiString(pModelSelection->edModelFileName->Text).c_str(),
                                AnsiString(pModelSelection->edTextureFileName->Text).c_str(),
                                NULL,
                                CSR_CO_None,
                                OnLoadTexture,
                                OnSelectModel))
        // show the error message to the user
        ::MessageDlg(L"Failed to add the WaveFront model.", mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddMDLModelClick(TObject* pSender)
{
    // get the current scene
    const CSR_Scene* pScene = m_pLevel->GetScene();

    // found it?
    if (!pScene)
    {
        // show the error message to the user
        ::MessageDlg(L"No model can be added to the scene, because no scene is currently opened.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
        return;
    }

    // create a model selection dialog box
    std::auto_ptr<TModelSelection> pModelSelection
            (new TModelSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // change the file filters
    pModelSelection->odModel->Filter = L"All model files|*.mdl|Quake I model|*.mdl";

    // hide the useless controls
    pModelSelection->paModelTexture->Visible = false;
    pModelSelection->blBottomLine->Visible   = false;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = 0.5f;

    CSR_CollisionOutput collisionOutput;

    // find the ground position at which the box will be set
    csrSceneDetectCollision(pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pModelSelection->vfPosition->edX->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(-pScene->m_ViewMatrix.m_Table[3][0]).c_str());
    pModelSelection->vfPosition->edY->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(collisionOutput.m_GroundPos).c_str());

    // show the default scaling
    pModelSelection->vfScaling->edX->Text = L"1.0";
    pModelSelection->vfScaling->edY->Text = L"1.0";
    pModelSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pModelSelection->ShowModal() != mrOk)
        return;

    // build the model matrix from the interface
    CSR_Matrix4 matrix;
    pModelSelection->BuildMatrix(&matrix);

    // add the Quake I model to the level
    if (!m_pLevel->AddMDL(matrix,
                          AnsiString(pModelSelection->edModelFileName->Text).c_str(),
                          NULL,
                          CSR_CO_None,
                          OnApplySkin,
                          OnSelectModel))
        // show the error message to the user
        ::MessageDlg(L"Failed to add the Quake I model.", mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miLandscapeResetViewportClick(TObject* pSender)
{
    // reset the viewpoint bounding sphere to his default position
    InitializeViewPoint(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miSkyboxAddClick(TObject* pSender)
{
    // is level opened?
    if (!m_pLevel.get() || !m_pLevel->GetScene())
    {
        // show the error message to the user
        ::MessageDlg(L"No skybox can be added to the scene, because no scene is currently opened.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
        return;
    }

    // create a model selection dialog box
    std::auto_ptr<TSkyboxSelection> pSkyboxSelection
            (new TSkyboxSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // show the dialog box to the user and check if action was canceled
    if (pSkyboxSelection->ShowModal() != mrOk)
        return;

    // get the skybox file names
    TSkyboxSelection::IFileNames fileNamesW;
    pSkyboxSelection->GetFileNames(fileNamesW);

    CSR_Level::IFileNames fileNames;

    const std::size_t fileNameCount = fileNamesW.size();

    // iterate through the cubemap texture files and convert them from unicode to ansi
    for (std::size_t i = 0; i < fileNameCount; i++)
        fileNames.push_back(CSR_VCLHelper::WStrToStr(fileNamesW[i]));

    // add the skybox to the model
    if (!m_pLevel->AddSkybox(fileNames, OnLoadCubemap))
        // show the error message to the user
        ::MessageDlg(L"Failed to add the skybox.", mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miPostProcessingNoneClick(TObject* pSender)
{
    m_pLevel->EnableMSAA(false);
    m_pLevel->EnableOilPainting(false);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miPostProcessingAntialiasingClick(TObject* pSender)
{
    m_pLevel->EnableMSAA(true);
    m_pLevel->EnableOilPainting(false);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miPostProcessingOilPaintingClick(TObject* pSender)
{
    m_pLevel->EnableMSAA(false);
    m_pLevel->EnableOilPainting(true);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miSoundOpenClick(TObject* pSender)
{
    // is level opened?
    if (!m_pLevel.get() || !m_pLevel->GetScene())
    {
        // show the error message to the user
        ::MessageDlg(L"No sound can be opened for the scene, because no scene is currently opened.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);
        return;
    }

    // create a sound selection dialog box
    std::auto_ptr<TSoundSelection> pSoundSelection
            (new TSoundSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // show it to user
    if (pSoundSelection->ShowModal() != mrOk)
        return;

    // open the level ambient sound
    if (!m_pLevel->OpenSound(AnsiString(pSoundSelection->edSoundFileName->Text).c_str(), NULL))
        // show the error message to the user
        ::MessageDlg(L"Failed to set the sound.", mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miSoundPauseClick(TObject* pSender)
{
    // pause or resume the sound
    m_pLevel->PauseSound(miSoundPause->Checked);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::spMainViewMoved(TObject* pSender)
{
    // is level created?
    if (!m_pLevel.get() || !m_pLevel->GetScene())
        return;

    // get back the scene item containing the model
    CSR_SceneItem* pItem = m_pLevel->GetLandscape();

    // found it?
    if (pItem)
        // calculate the ratio to use to draw the designer view
        if (pItem->m_AABBTreeCount)
            m_pDesignerView->SetRatio
                    (paDesignerView->ClientHeight / (pItem->m_pAABBTree[0].m_pBox->m_Max.m_Z -
                            pItem->m_pAABBTree[0].m_pBox->m_Min.m_Z));

    // update the viewport
    m_pLevel->CreateViewport(paEngineView->ClientWidth, paEngineView->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::spViewsMoved(TObject *Sender)
{
    // is level created?
    if (!m_pLevel.get() || !m_pLevel->GetScene())
        return;

    // get back the scene item containing the model
    CSR_SceneItem* pItem = m_pLevel->GetLandscape();

    // found it?
    if (pItem)
        // calculate the ratio to use to draw the designer view
        if (pItem->m_AABBTreeCount)
            m_pDesignerView->SetRatio
                    (paDesignerView->ClientHeight / (pItem->m_pAABBTree[0].m_pBox->m_Max.m_Z -
                            pItem->m_pAABBTree[0].m_pBox->m_Min.m_Z));

    // update the viewport
    m_pLevel->CreateViewport(paEngineView->ClientWidth, paEngineView->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aeEventsMessage(tagMSG& msg, bool& handled)
{
    m_PosVelocity = 0.0f;
    m_DirVelocity = 0.0f;

    switch (msg.message)
    {
        case WM_KEYDOWN:
        {
            // get the scene
            CSR_Scene* pScene = m_pLevel->GetScene();

            // no scene?
            if (!pScene)
                break;

            // ignore any edit event
            if (Application->MainForm->ActiveControl &&
                Application->MainForm->ActiveControl->InheritsFrom(__classid(TEdit)))
                break;

            // search for the pressed key
            switch (msg.wParam)
            {
                case VK_LEFT:
                    m_PosVelocity = -1.0f;
                    handled       =  true;
                    break;

                case VK_RIGHT:
                    m_PosVelocity = 1.0f;
                    handled       = true;
                    break;

                case VK_TAB:
                {
                    if (!m_pDesignerView.get())
                        return;

                    if (miDesignerPlayMode->Checked)
                        return;

                    // get pressed key and shift state
                    const TShiftState shiftState = ::KeyDataToShiftState(msg.lParam);

                    // select the prev or next model
                    if (shiftState.Contains(ssShift))
                        m_pDesignerView->SelectPrev();
                    else
                        m_pDesignerView->SelectNext();

                    // refresh the model properties
                    RefreshProperties();

                    handled = true;
                    break;
                }

                case VK_DELETE:
                {
                    // get the currently selected item
                    const CSR_DesignerView::ISelection* pSelection = m_pDesignerView->GetSelection();

                    // found it?
                    if (!pSelection)
                        break;

                    // get the landscape scene item
                    const CSR_SceneItem* pItem = m_pLevel->GetLandscape();

                    // found it?
                    if (!pItem)
                        break;

                    // is the main landscape model?
                    if (pSelection->m_pKey == pItem->m_pModel)
                        break;

                    // get the model key to delete from scene
                          void* pKeyToDel   = pSelection->m_pKey;
                    const int   matrixIndex = pSelection->m_MatrixIndex;

                    // get the scene item to modify
                    CSR_SceneItem* pSceneItem = csrSceneGetItem(pScene, pKeyToDel);

                    // scene item contains less than 2 matrices?
                    if (pSceneItem->m_pMatrixArray->m_Count < 2)
                    {
                        // select the next model
                        m_pDesignerView->SelectNext();

                        // delete the currently selected model
                        csrSceneDeleteFrom(pScene, pKeyToDel, OnDeleteTexture);

                        // delete the matching level item
                        m_pLevel->Delete(pKeyToDel);
                    }
                    else
                    {
                        // delete the matrix in the currently selected model
                        csrSceneDeleteFrom(pScene,
                                           pSceneItem->m_pMatrixArray->m_pItem[matrixIndex].m_pData,
                                           OnDeleteTexture);

                        // delete the matrix in the matching level item
                        m_pLevel->Delete(pKeyToDel, matrixIndex);

                        // select the next model
                        if (pSelection->m_MatrixIndex >= int(pSceneItem->m_pMatrixArray->m_Count))
                        {
                            CSR_DesignerView::ISelection selection;
                            selection.m_pKey        = pKeyToDel;
                            selection.m_MatrixIndex = pSceneItem->m_pMatrixArray->m_Count - 1;
                            m_pDesignerView->SetSelection(selection);
                        }
                    }

                    // invalidate the designer
                    paDesignerView->Invalidate();

                    // refresh the selected model properties
                    RefreshProperties();

                    break;
                }
            }

            return;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnViewClick(TObject* pSender)
{
    // get the view that sent the event as a Windows control
    TWinControl* pWinControl = dynamic_cast<TWinControl*>(pSender);

    // focus the view, if possible
    if (pWinControl && pWinControl->CanFocus())
        Application->MainForm->ActiveControl = pWinControl;
}
//------------------------------------------------------------------------------
GLuint TMainForm::OnLoadCubemap(const CSR_Level::IFileNames& fileNames)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return M_CSR_Error_Code;

    return pMainForm->LoadCubemap(fileNames);
}
//------------------------------------------------------------------------------
GLuint TMainForm::OnLoadTexture(const std::string& fileName)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return M_CSR_Error_Code;

    return pMainForm->LoadTexture(fileName);
}
//------------------------------------------------------------------------------
void TMainForm::OnSelectModel(void* pKey, int index)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    CSR_DesignerView::ISelection selection;
    selection.m_pKey        = pKey;
    selection.m_MatrixIndex = index;

    // select the model
    pMainForm->m_pDesignerView->SetSelection(selection);

    pMainForm->paDesignerView->Invalidate();

    // refresh the selected model properties
    pMainForm->RefreshProperties();
}
//------------------------------------------------------------------------------
void TMainForm::OnUpdateDesigner(void* pKey, int index, const CSR_Vector3& modelLength)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    // calculate and set the designer ratio
    pMainForm->m_pDesignerView->SetRatio
            (pMainForm->paDesignerView->ClientHeight / (modelLength.m_Z ? modelLength.m_Z : 1.0f));

    CSR_DesignerView::ISelection selection;
    selection.m_pKey        = pKey;
    selection.m_MatrixIndex = index;

    // select the model
    pMainForm->m_pDesignerView->SetSelection(selection);

    // refresh the selected model properties
    pMainForm->RefreshProperties();
}
//---------------------------------------------------------------------------
void TMainForm::OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    return pMainForm->m_pLevel->OnApplySkin(index, pSkin, pCanRelease);
}
//------------------------------------------------------------------------------
void* TMainForm::OnGetShader(const void* pModel, CSR_EModelType type)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->m_pLevel->OnGetShader(pModel, type);
}
//---------------------------------------------------------------------------
void* TMainForm::OnGetID(const void* pKey)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->m_pLevel->OnGetID(pKey);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->m_pLevel->OnSceneBegin(pScene, pContext);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->m_pLevel->OnSceneEnd(pScene, pContext);
}
//------------------------------------------------------------------------------
void TMainForm::OnDeleteTexture(const CSR_Texture* pTexture)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->m_pLevel->OnDeleteTexture(pTexture);
}
//---------------------------------------------------------------------------
bool TMainForm::OnEngineViewMessage(TControl* pControl, TMessage& message, TWndMethod fCtrlOriginalProc)
{
    switch (message.Msg)
    {
        case WM_ERASEBKGND:
            // do nothing, the background will be fully repainted later
            message.Result = 1L;
            return true;

        case WM_WINDOWPOSCHANGED:
        {
            if (!m_pLevel->GetScene())
                break;

            TPanel* pPanel = dynamic_cast<TPanel*>(pControl);

            if (!pPanel)
                return false;

            if (fCtrlOriginalProc)
                fCtrlOriginalProc(message);

            // redraw here, thus the view will be redrawn to the correct size in real time while the
            // size changes
            OnDrawScene(true);

            return true;
        }

        case WM_PAINT:
        {
            // is scene initialized?
            if (!m_pLevel->GetScene())
                return false;

            TPanel* pPanel = dynamic_cast<TPanel*>(pControl);

            if (!pPanel)
                return false;

            // draw the scene
            OnDrawScene(true);

            // validate entire client rect (it has just been completely redrawn)
            ::ValidateRect(pPanel->Handle, NULL);

            // notify main Windows procedure that the view was repainted
            message.Result = 0L;
            return true;
        }
    }

    return false;
}
//------------------------------------------------------------------------------
bool TMainForm::OpenDocument()
{
    // create a landscape selection dialog box
    std::auto_ptr<TLandscapeSelection> pLandscapeSelection
            (new TLandscapeSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // execute the dialog and check if user canceled it
    if (pLandscapeSelection->ShowModal() != mrOk)
        return true;

    const std::string textureFileName = AnsiString(pLandscapeSelection->edTextureFileName->Text).c_str();

    // do change the landscape model?
    if (pLandscapeSelection->rbSourceBitmap->Checked)
    {
        // recreate the scene
        CreateScene();

        // load the landscape from a grayscale image
        if (!m_pLevel->AddLandscapeFromBitmap(AnsiString(pLandscapeSelection->edBitmapFileName->Text).c_str(),
                                              textureFileName,
                                              NULL,
                                              OnUpdateDesigner))
        {
            // show the error message to the user
            ::MessageDlg(L"Failed to load the landscape from the grayscale image.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return false;
        }
    }
    else
    if (pLandscapeSelection->rbSourceModel->Checked)
    {
        // recreate the scene
        CreateScene();

        // load the landscape from a model file
        if (!m_pLevel->AddLandscape(AnsiString(pLandscapeSelection->edModelFileName->Text).c_str(),
                                    textureFileName,
                                    NULL,
                                    OnUpdateDesigner))
        {
            // show the error message to the user
            ::MessageDlg(L"Failed to load the landscape.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return false;
        }
    }

    // do change the texture?
    if (!textureFileName.empty())
    {
        // get back the landscape scene item
        const CSR_SceneItem* pItem = m_pLevel->GetLandscape();

        // found it?
        if (!pItem)
        {
            // show the error message to the user
            ::MessageDlg(L"Failed to get the landscape model.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return false;
        }

        // get the landscape model from the scene item
        const CSR_Model* pModel = static_cast<CSR_Model*>(pItem->m_pModel);

        // found it?
        if (!pModel || !pModel->m_MeshCount)
        {
            // show the error message to the user
            ::MessageDlg(L"The landscape model is empty.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return false;
        }

        // release the previous texture
        m_pLevel->DeleteTexture(&pModel->m_pMesh[0].m_Skin.m_Texture);

        // load the new one
        const GLuint textureID = LoadTexture(textureFileName);

        // failed?
        if (textureID == M_CSR_Error_Code)
        {
            // show the error message to the user
            ::MessageDlg(L"Failed to load the texture.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return false;
        }

        // add the new texture to the OpenGL resources
        m_pLevel->AddTexture(&pModel->m_pMesh[0].m_Skin.m_Texture, textureID);
    }

    paDesignerView->Invalidate();

    return true;
}
//------------------------------------------------------------------------------
void TMainForm::CloseDocument()
{
    // clear the properties view
    ClearProperties();

    // clear the designer view
    m_pDesignerView->Unselect();
    m_pDesignerView->SetScene(NULL);

    // clear the level manager
    m_pLevel->Clear();
}
//---------------------------------------------------------------------------
bool TMainForm::ProcessCommand(const ICommand& command)
{
    switch (command.m_Command)
    {
        case IE_C_Cut:
        case IE_C_Copy:
            m_ProcessingCommand = command;
            return true;

        case IE_C_Paste:
        {
            // get the current scene
            CSR_Scene* pScene = m_pLevel->GetScene();

            // found it?
            if (!pScene)
                return false;

            // get the scene item on which the command should be processed
            CSR_SceneItem* pSceneItem = csrSceneGetItem(pScene, m_ProcessingCommand.m_pKey);

            // found it?
            if (!pSceneItem)
                return false;

            // search for command to execute
            switch (m_ProcessingCommand.m_Command)
            {
                case IE_C_Cut:
                {
                    // get the level item
                    CSR_Level::IItem* pItem = m_pLevel->Get(m_ProcessingCommand.m_pKey);

                    // succeeded?
                    if (!pItem || !pItem->m_Matrices.size())
                        return false;

                    // check if the command index is out of bounds
                    if (m_ProcessingCommand.m_Index >= int(pItem->m_Matrices.size()))
                        return false;

                    // configure the new model matrix
                    *pItem->m_Matrices[m_ProcessingCommand.m_Index]                = *pItem->m_Matrices[m_ProcessingCommand.m_Index];
                     pItem->m_Matrices[m_ProcessingCommand.m_Index]->m_Table[3][0] = -pScene->m_ViewMatrix.m_Table[3][0];

                    // select the newly added model
                    OnSelectModel(m_ProcessingCommand.m_pKey, m_ProcessingCommand.m_Index);

                    // clear the processed command to avoid to cut repeatly
                    m_ProcessingCommand.Clear();
                    break;
                }

                case IE_C_Copy:
                {
                    // add a new matrix in the level item
                    CSR_Level::IItem* pItem = m_pLevel->Add(m_ProcessingCommand.m_pKey);

                    // succeeded?
                    if (!pItem || !pItem->m_Matrices.size())
                        return false;

                    // check if the command index is out of bounds
                    if (m_ProcessingCommand.m_Index >= int(pItem->m_Matrices.size()))
                        return false;

                    // get the newly added matrix index
                    const std::size_t matrixIndex = pItem->m_Matrices.size() - 1;

                    // configure the new model matrix
                    *pItem->m_Matrices[matrixIndex]                = *pItem->m_Matrices[m_ProcessingCommand.m_Index];
                     pItem->m_Matrices[matrixIndex]->m_Table[3][0] = -pScene->m_ViewMatrix.m_Table[3][0];

                    // link the newly added matrix with the scene
                    csrSceneAddModelMatrix(pScene, m_ProcessingCommand.m_pKey, pItem->m_Matrices[matrixIndex]);

                    // select the newly added model
                    OnSelectModel(m_ProcessingCommand.m_pKey, matrixIndex);
                    break;
                }

                default:
                    return false;
            }

            return true;
        }

        default:
            return false;
    }
}
//---------------------------------------------------------------------------
GLuint TMainForm::LoadTexture(const std::string& fileName) const
{
    try
    {
        // load texture in a picture
        std::auto_ptr<TPicture> pPicture(new TPicture());
        pPicture->LoadFromFile(fileName.c_str());

        // convert it to a bitmap
        std::auto_ptr<TBitmap> pTexture(new TBitmap());
        pTexture->Assign(pPicture->Graphic);

        int pixelSize;

        // search for bitmap pixel format
        switch (pTexture->PixelFormat)
        {
            case pf24bit: pixelSize = 3; break;
            case pf32bit: pixelSize = 4; break;
            default:                     return M_CSR_Error_Code;
        }

        GLuint           textureID    = M_CSR_Error_Code;
        CSR_PixelBuffer* pPixelBuffer = csrPixelBufferCreate();

        try
        {
            // configure the pixel buffer
            pPixelBuffer->m_PixelType    = CSR_PT_BGR;
            pPixelBuffer->m_ImageType    = CSR_IT_Raw;
            pPixelBuffer->m_Width        = pTexture->Width;
            pPixelBuffer->m_Height       = pTexture->Height;
            pPixelBuffer->m_BytePerPixel = pixelSize;
            pPixelBuffer->m_DataLength   = pTexture->Width * pTexture->Height * pixelSize;

            // reserve memory for the pixel array. NOTE use malloc and not new here to be conform
            // with the c standards (otherwise CodeGuard will not be happy)
            pPixelBuffer->m_pData = malloc(pPixelBuffer->m_DataLength);

            TRGBTriple* pLineRGB;
            TRGBQuad*   pLineRGBA;

            // iterate through lines to copy
            for (int y = 0; y < pTexture->Height; ++y)
            {
                // get the next pixel line from bitmap
                if (pixelSize == 3)
                    pLineRGB  = static_cast<TRGBTriple*>(pTexture->ScanLine[y]);
                else
                    pLineRGBA = static_cast<TRGBQuad*>(pTexture->ScanLine[y]);

                // calculate the start y position
                const int yPos = y * pTexture->Width * pixelSize;

                // iterate through pixels to copy
                for (int x = 0; x < pTexture->Width; ++x)
                {
                    // copy to pixel array and take the opportunity to swap the pixel RGB values
                    if (pixelSize == 3)
                    {
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 3)]     = pLineRGB[x].rgbtRed;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 3) + 1] = pLineRGB[x].rgbtGreen;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 3) + 2] = pLineRGB[x].rgbtBlue;
                    }
                    else
                    {
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4)]     = pLineRGBA[x].rgbRed;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4) + 1] = pLineRGBA[x].rgbGreen;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4) + 2] = pLineRGBA[x].rgbBlue;
                        ((unsigned char*)pPixelBuffer->m_pData)[yPos + (x * 4) + 3] = pLineRGBA[x].rgbReserved;
                    }
                }
            }

            // load the texture on the GPU
            textureID = csrOpenGLTextureFromPixelBuffer(pPixelBuffer);
        }
        __finally
        {
            csrPixelBufferRelease(pPixelBuffer);
        }

        return textureID;
    }
    catch (...)
    {
        return M_CSR_Error_Code;
    }
}
//---------------------------------------------------------------------------
GLuint TMainForm::LoadCubemap(const CSR_Level::IFileNames fileNames) const
{
    try
    {
        GLuint textureID = M_CSR_Error_Code;

        // create new OpenGL texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        const std::size_t fileNameCount = fileNames.size();

        // iterate through the cubemap texture files to load
        for (std::size_t i = 0; i < fileNameCount; i++)
        {
            // load texture in a picture
            std::auto_ptr<TPicture> pPicture(new TPicture());
            pPicture->LoadFromFile(fileNames[i].c_str());

            // convert it to a bitmap
            std::auto_ptr<TBitmap> pTexture(new TBitmap());
            pTexture->Assign(pPicture->Graphic);

            int pixelSize;

            // search for bitmap pixel format
            switch (pTexture->PixelFormat)
            {
                case pf24bit: pixelSize = 3; break;
                case pf32bit: pixelSize = 4; break;
                default:                     return M_CSR_Error_Code;
            }

            CSR_PixelBuffer* pPixelBuffer = csrPixelBufferCreate();

            try
            {
                // configure the pixel buffer
                pPixelBuffer->m_PixelType    = CSR_PT_BGR;
                pPixelBuffer->m_ImageType    = CSR_IT_Raw;
                pPixelBuffer->m_Width        = pTexture->Width;
                pPixelBuffer->m_Height       = pTexture->Height;
                pPixelBuffer->m_BytePerPixel = pixelSize;
                pPixelBuffer->m_DataLength   = pTexture->Width * pTexture->Height * pixelSize;

                // reserve memory for the pixel array. NOTE use malloc and not new here to be conform
                // with the c standards (otherwise CodeGuard will not be happy)
                pPixelBuffer->m_pData = malloc(pPixelBuffer->m_DataLength);

                TRGBTriple* pLineRGB;
                TRGBQuad*   pLineRGBA;

                // iterate through lines to copy
                for (int y = 0; y < pTexture->Height; ++y)
                {
                    // get the next pixel line from bitmap
                    if (pixelSize == 3)
                        pLineRGB  = static_cast<TRGBTriple*>(pTexture->ScanLine[y]);
                    else
                        pLineRGBA = static_cast<TRGBQuad*>(pTexture->ScanLine[y]);

                    // calculate the start y position
                    const int yPos = y * pTexture->Width * pixelSize;

                    // iterate through pixels to copy
                    for (int x = 0; x < pTexture->Width; ++x)
                    {
                        // copy to pixel array
                        if (pixelSize == 3)
                        {
                            const std::size_t offset = yPos + (((pTexture->Width - 1) - x) * 3);

                            ((unsigned char*)pPixelBuffer->m_pData)[offset]     = pLineRGB[x].rgbtRed;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 1] = pLineRGB[x].rgbtGreen;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 2] = pLineRGB[x].rgbtBlue;
                        }
                        else
                        {
                            const std::size_t offset = yPos + (((pTexture->Width - 1) - x) * 4);

                            ((unsigned char*)pPixelBuffer->m_pData)[offset]     = pLineRGBA[x].rgbRed;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 1] = pLineRGBA[x].rgbGreen;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 2] = pLineRGBA[x].rgbBlue;
                            ((unsigned char*)pPixelBuffer->m_pData)[offset + 3] = pLineRGBA[x].rgbReserved;
                        }
                    }
                }

                // load the texture on the GPU
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                             0,
                             GL_RGB,
                             pPixelBuffer->m_Width,
                             pPixelBuffer->m_Height,
                             0,
                             GL_RGB,
                             GL_UNSIGNED_BYTE,
                             pPixelBuffer->m_pData);
            }
            __finally
            {
                csrPixelBufferRelease(pPixelBuffer);
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,     GL_CLAMP_TO_EDGE);

        return textureID;
    }
    catch (...)
    {
        return M_CSR_Error_Code;
    }
}
//------------------------------------------------------------------------------
void TMainForm::ClearProperties()
{
    // clear the designer properties
    m_DesignerProperties.Clear();

    // delete the entire group. This is a annoying constraint of the properties group, because added
    // items can never be deleted dynamically. For a stupid reason I cannot figure out, the VCL
    // developers thought very intelligent to make the RemovePanel() function protected, preventing
    // thus any panel to be deleted dynamically!!!
    // (See: https://stackoverflow.com/questions/20809310/tcategorypanelgroup-delete-panel)
    if (cgProperties)
    {
        cgProperties->Parent = NULL;
        delete cgProperties;
        cgProperties = NULL;
    }

    // is component currently deleting?
    if (!paProperties || paProperties->ComponentState.Contains(csDestroying))
        // don't try to recreate properties group, otherwise an access violation may be raised under
        // some compiler versions, as e.g. in XE6
        return;

    // recreate properties panel
    std::auto_ptr<TCategoryPanelGroup> pProperties(new TCategoryPanelGroup(paProperties));
    pProperties->Name                    = L"cgProperties";
    pProperties->Align                   = alClient;
    pProperties->BevelInner              = bvNone;
    pProperties->BevelOuter              = bvNone;
    pProperties->VertScrollBar->Tracking = false;
    pProperties->Parent                  = paProperties;
    cgProperties                         = pProperties.release();
}
//------------------------------------------------------------------------------
void TMainForm::RefreshProperties()
{
    try
    {
        // workaround: lock the refresh while interface is updated, to avoid flickering when group
        // is deleted then recreated
        ::LockWindowUpdate(paProperties->Handle);

        ClearProperties();

        // create a category panel
        TCategoryPanel* pCategory = dynamic_cast<TCategoryPanel*>(cgProperties->CreatePanel(cgProperties));

        // created?
        if (!pCategory)
            return;

        // get the selection
        const CSR_DesignerView::ISelection* pSelection = m_pDesignerView->GetSelection();

        // found it?
        if (!pSelection)
            return;

        // get the selected scene item
        const CSR_SceneItem* pItem = m_pLevel->GetSceneItem(pSelection->m_pKey);

        // not found?
        if (!pItem || !pItem->m_pMatrixArray->m_Count)
            return;

        CSR_Matrix4 matrix =
                *static_cast<CSR_Matrix4*>
                        (pItem->m_pMatrixArray->m_pItem[pSelection->m_MatrixIndex].m_pData);

        std::auto_ptr<TLabel>        pLabel;
        std::auto_ptr<TVector3Frame> pVector3Frame;

        CSR_VCLHelper::IControls controls;

        // populate category panel
        pCategory->Name    =  "cpTransform";
        pCategory->Caption = L"Transform";

        // create the transform translate label
        pLabel.reset(new TLabel(pCategory));
        pLabel->Name             =  "laTransformTranslate";
        pLabel->Caption          = L"Translate";
        pLabel->Align            = alTop;
        pLabel->Margins->Left    = 3;
        pLabel->Margins->Top     = 0;
        pLabel->Margins->Right   = 3;
        pLabel->Margins->Bottom  = 0;
        pLabel->AlignWithMargins = true;
        pLabel->Font->Size       = 10;
        pLabel->Parent           = pCategory;
        controls.push_back(pLabel.get());
        pLabel.release();

        // create the transform translate panel
        pVector3Frame.reset(new TVector3Frame(pCategory));
        pVector3Frame->Name                        = "vfTransformTranslate";
        pVector3Frame->Align                       = alTop;
        pVector3Frame->Parent                      = pCategory;
        m_DesignerProperties.m_pTransformTranslate = pVector3Frame.release();
        m_DesignerProperties.m_pTransformTranslate->Set_OnValueChanged(OnPropertiesValueChanged);
        controls.push_back(m_DesignerProperties.m_pTransformTranslate);

        CSR_Vector3 translation;
        csrMat4TranslationFrom(&matrix, &translation.m_X, &translation.m_Y, &translation.m_Z);
        m_DesignerProperties.m_pTransformTranslate->SetVector(translation);

        // create the transform translate label
        pLabel.reset(new TLabel(pCategory));
        pLabel->Name             =  "laTransformRotate";
        pLabel->Caption          = L"Rotate";
        pLabel->Align            = alTop;
        pLabel->Margins->Left    = 3;
        pLabel->Margins->Top     = 5;
        pLabel->Margins->Right   = 3;
        pLabel->Margins->Bottom  = 0;
        pLabel->AlignWithMargins = true;
        pLabel->Font->Size       = 10;
        pLabel->Parent           = pCategory;
        controls.push_back(pLabel.get());
        pLabel.release();

        // create the transform rotate panel
        pVector3Frame.reset(new TVector3Frame(pCategory));
        pVector3Frame->Name                     = "vfTransformRotate";
        pVector3Frame->Align                    = alTop;
        pVector3Frame->Parent                   = pCategory;
        m_DesignerProperties.m_pTransformRotate = pVector3Frame.release();
        m_DesignerProperties.m_pTransformRotate->Set_OnValueChanged(OnPropertiesValueChanged);
        controls.push_back(m_DesignerProperties.m_pTransformRotate);

        CSR_Vector3 rotation;
        csrMat4RotationFrom(&matrix, &rotation.m_X, &rotation.m_Y, &rotation.m_Z);
        m_DesignerProperties.m_pTransformRotate->SetVector(rotation);

        // create the transform translate label
        pLabel.reset(new TLabel(pCategory));
        pLabel->Name             =  "laTransformScale";
        pLabel->Caption          = L"Scale";
        pLabel->Align            = alTop;
        pLabel->Margins->Left    = 3;
        pLabel->Margins->Top     = 5;
        pLabel->Margins->Right   = 3;
        pLabel->Margins->Bottom  = 0;
        pLabel->AlignWithMargins = true;
        pLabel->Font->Size       = 10;
        pLabel->Parent           = pCategory;
        controls.push_back(pLabel.get());
        pLabel.release();

        // create the transform scale panel
        pVector3Frame.reset(new TVector3Frame(pCategory));
        pVector3Frame->Name                    = "vfTransformScale";
        pVector3Frame->Align                   = alTop;
        pVector3Frame->Parent                  = pCategory;
        m_DesignerProperties.m_pTransformScale = pVector3Frame.release();
        m_DesignerProperties.m_pTransformScale->Set_OnValueChanged(OnPropertiesValueChanged);
        controls.push_back(m_DesignerProperties.m_pTransformScale);

        CSR_Vector3 factor;
        csrMat4ScalingFrom(&matrix, &factor.m_X, &factor.m_Y, &factor.m_Z);
        m_DesignerProperties.m_pTransformScale->SetVector(factor);

        CSR_VCLHelper::DistributeCtrlsTTB(controls);

        // calculate the category height. NOTE the hardcoded value is the panel header height, which
        // is, of course, impossible to get from the panel itself. Perhaps one day Embarcadero will
        // provide a really working IDE?
        pCategory->Height = 26                                          +
                            m_DesignerProperties.m_pTransformScale->Top +
                            m_DesignerProperties.m_pTransformScale->Height;

        Application->MainForm->ActiveControl = NULL;
    }
    __finally
    {
        // unlock component painting
        ::LockWindowUpdate(NULL);

         // scroll to first control
         cgProperties->VertScrollBar->Position = 0;

         // show properties
         cgProperties->Visible = true;
    }
}
//------------------------------------------------------------------------------
void TMainForm::InitializeViewPoint(const CSR_Matrix4* pMatrix)
{
    // set the viewpoint radius
    m_ViewSphere.m_Radius = 0.1f;

    // do initialize the viewpoint from an existing matrix?
    if (pMatrix)
    {
        CSR_Vector3 translation;

        // extract the translation from the matrix
        csrMat4TranslationFrom(pMatrix, &translation.m_X, &translation.m_Y, &translation.m_Z);

        // set the viewpoint bounding sphere position
        m_ViewSphere.m_Center.m_X = -translation.m_X;
        m_ViewSphere.m_Center.m_Y =  translation.m_Y;
        m_ViewSphere.m_Center.m_Z = -translation.m_Z;

        return;
    }

    // reset the viewpoint bounding sphere to his default position
    m_ViewSphere.m_Center.m_X = 0.0f;
    m_ViewSphere.m_Center.m_Y = 0.0f;
    m_ViewSphere.m_Center.m_Z = 3.08f;
}
//------------------------------------------------------------------------------
void TMainForm::CreateScene()
{
    // close the previously opened document
    CloseDocument();

    // create the level scene
    CSR_Scene* pScene = m_pLevel->CreateScene();

    if (!pScene)
        throw std::runtime_error("Internal error - could not create the scene");

    // link the scene to the designer view
    m_pDesignerView->SetScene(pScene);

    // initialize the viewpoint
    InitializeViewPoint(NULL);

    // rebuild the viewport
    m_pLevel->CreateViewport(paEngineView->ClientWidth, paEngineView->ClientHeight);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // configure the scene context
    m_pLevel->m_SceneContext.m_fOnSceneBegin    = OnSceneBegin;
    m_pLevel->m_SceneContext.m_fOnSceneEnd      = OnSceneEnd;
    m_pLevel->m_SceneContext.m_fOnGetShader     = OnGetShader;
    m_pLevel->m_SceneContext.m_fOnGetID         = OnGetID;
    m_pLevel->m_SceneContext.m_fOnDeleteTexture = OnDeleteTexture;

    // build the main scene shader
    if (!m_pLevel->BuildSceneShader())
    {
        // show the error message to the user
        ::MessageDlg(L"Failed to load the shader.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // create the default scene (IMPORTANT should be done AFTER the shader is built)
    CreateScene();

    // get a default texture file name
    const std::string textureFile = m_SceneDir + "\\Textures\\mountain.jpg";

    // load the landscape model from a grayscale bitmap file
    if (!m_pLevel->AddLandscapeFromBitmap(m_SceneDir + "\\Bitmaps\\playfield.bmp",
                                          textureFile,
                                          NULL,
                                          OnUpdateDesigner))
    {
        // show the error message to the user
        ::MessageDlg(L"An error occurred while the default landscape model was created.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // get back the scene item containing the model
    const CSR_SceneItem* pItem = m_pLevel->GetLandscape();

    // found it?
    if (!pItem)
    {
        // show the error message to the user
        ::MessageDlg(L"The landscape was not found in the scene.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // load the texture
    const GLuint textureID = LoadTexture(textureFile);

    // failed?
    if (textureID == M_CSR_Error_Code)
    {
        // show the error message to the user
        ::MessageDlg(L"Unknown texture format.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // add the texture to the OpenGL resources
    m_pLevel->AddTexture(&static_cast<CSR_Model*>(pItem->m_pModel)->m_pMesh[0].m_Skin.m_Texture, textureID);

    // open the level ambient sound
    m_pLevel->OpenSound(m_SceneDir + "\\Sounds\\landscape_ambient_sound.wav", NULL);

    m_Initialized = true;

    // show the changes on the designer view
    paDesignerView->Invalidate();

    // to allow the engine view to be scrolled directly
    Application->MainForm->ActiveControl = paEngineView;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    m_Initialized = false;

    // delete the level
    m_pLevel.reset();
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    // keep the old position to revert it in case of error
    const CSR_Vector3 oldPos = m_ViewSphere.m_Center;

    // is player rotating?
    if (m_DirVelocity)
    {
        // calculate the player direction
        m_Angle += m_DirVelocity * elapsedTime;

        // validate and apply it
        if (m_Angle > M_PI * 2.0f)
            m_Angle -= M_PI * 2.0f;
        else
        if (m_Angle < 0.0f)
            m_Angle += M_PI * 2.0f;
    }

    // is player moving?
    if (m_PosVelocity)
    {
        // calculate the next position
        m_ViewSphere.m_Center.m_X += m_PosVelocity * sinf(m_Angle + (M_PI * 0.5f)) * elapsedTime;
        m_ViewSphere.m_Center.m_Z -= m_PosVelocity * cosf(m_Angle + (M_PI * 0.5f)) * elapsedTime;
    }

    // enable the shader program
    m_pLevel->EnableShader();

    // calculate and update the view position on the ground
    m_pLevel->ApplyGroundCollision(&m_ViewSphere, m_Angle, oldPos);

    GLint viewPort[4];
    glGetIntegerv(GL_VIEWPORT, viewPort);

    // calculate the designer view origin in relation to the current view position
    m_pDesignerView->SetOrigin
            (-m_pDesignerView->GetRatio() *
                    ((m_pLevel->GetViewXPos() * paDesignerView->ClientWidth) / viewPort[2]));
}
//---------------------------------------------------------------------------
void TMainForm::OnPropertiesValueChanged(TObject* pSender, float x, float y, float z)
{
    // get the selection
    const CSR_DesignerView::ISelection* pSelection = m_pDesignerView->GetSelection();

    // found it?
    if (!pSelection)
        return;

    // get the selected scene item
    const CSR_SceneItem* pItem = m_pLevel->GetSceneItem(pSelection->m_pKey);

    // not found?
    if (!pItem || !pItem->m_pMatrixArray->m_Count)
        return;

    CSR_Vector3 t;

    // get the translation values to apply
    t.m_X = m_DesignerProperties.m_pTransformTranslate->GetX();
    t.m_Y = m_DesignerProperties.m_pTransformTranslate->GetY();
    t.m_Z = m_DesignerProperties.m_pTransformTranslate->GetZ();

    CSR_Vector3 r;

    // get the rotation values to apply
    r.m_X = m_DesignerProperties.m_pTransformRotate->GetX();
    r.m_Y = m_DesignerProperties.m_pTransformRotate->GetY();
    r.m_Z = m_DesignerProperties.m_pTransformRotate->GetZ();

    CSR_Vector3 factor;

    // get the scale values to apply
    factor.m_X = m_DesignerProperties.m_pTransformScale->GetX();
    factor.m_Y = m_DesignerProperties.m_pTransformScale->GetY();
    factor.m_Z = m_DesignerProperties.m_pTransformScale->GetZ();

    // build the model matrix
    CSR_OpenGLHelper::BuildMatrix(&t,
                                  &r,
                                  &factor,
                                   static_cast<CSR_Matrix4*>(pItem->m_pMatrixArray->m_pItem[pSelection->m_MatrixIndex].m_pData));

    // update the designer view
    paDesignerView->Invalidate();
}
//---------------------------------------------------------------------------
void TMainForm::OnDrawScene(bool resize)
{
    // is level created?
    if (!m_pLevel.get() || !m_pLevel->GetScene())
    {
        paEngineView->Invalidate();
        paDesignerView->Invalidate();
        return;
    }

    // do draw the scene for a resize?
    if (resize)
    {
        if (!m_Initialized)
            return;

        // just process a minimal draw
        UpdateScene(0.0);
        m_pLevel->Draw();

        ::SwapBuffers(m_hDC);
        return;
    }

    // calculate time interval
    const unsigned __int64 now            = ::GetTickCount();
    const double           elapsedTime    = (now - m_PreviousTime) / 1000.0;
                           m_PreviousTime =  now;

    if (!m_Initialized)
        return;

    ++m_FrameCount;

    // calculate the FPS
    if (m_FrameCount >= 100)
    {
        const double      smoothing = 0.1;
        const std::size_t fpsTime   = now > m_StartTime ? now - m_StartTime : 1;
        const std::size_t newFPS    = (m_FrameCount * 100) / fpsTime;
        m_StartTime                 = ::GetTickCount();
        m_FrameCount                = 0;
        m_FPS                       = (newFPS * smoothing) + (m_FPS * (1.0 - smoothing));
    }

    // update and draw the scene
    UpdateScene(elapsedTime);
    m_pLevel->Draw();

    ::SwapBuffers(m_hDC);

    // is in play mode?
    if (!miDesignerPlayMode->Checked)
    {
        bool allowed = false;

        // check if designer should be refreshed
        if (miDesignerRefreshImmediately->Checked)
            allowed = m_pDesignerView->GetOrigin() != m_PrevOrigin;
        else
        if (miDesignerRefreshEvery500ms->Checked)
            allowed = m_pDesignerView->GetOrigin() != m_PrevOrigin && (now - m_PrevDesignerTime) > 500.0;
        else
        if (miDesignerRefreshEvery1s->Checked)
            allowed = m_pDesignerView->GetOrigin() != m_PrevOrigin && (now - m_PrevDesignerTime) > 1000.0;
        else
        if (miDesignerRefreshEvery2s->Checked)
            allowed = m_pDesignerView->GetOrigin() != m_PrevOrigin && (now - m_PrevDesignerTime) > 2000.0;
        else
        if (miDesignerRefreshEvery5s->Checked)
            allowed = m_pDesignerView->GetOrigin() != m_PrevOrigin && (now - m_PrevDesignerTime) > 5000.0;
        else
        if (miDesignerRefreshEvery10s->Checked)
            allowed = m_pDesignerView->GetOrigin() != m_PrevOrigin && (now - m_PrevDesignerTime) > 10000.0;

        // invalidate the designer view, if required
        if (allowed)
        {
            m_PrevOrigin       = m_pDesignerView->GetOrigin();
            m_PrevDesignerTime = now;

            paDesignerView->Invalidate();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;
    OnDrawScene(false);
}
//---------------------------------------------------------------------------
