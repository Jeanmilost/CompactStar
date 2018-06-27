/****************************************************************************
 * ==> Main ----------------------------------------------------------------*
 ****************************************************************************
 * Description : Ground collision tool main form                            *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2018, this file is part of the CompactStar Engine.  *
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
#include "CSR_Scene.h"

// classes
#include "CSR_OpenGLHelper.h"
#include "CSR_ShaderHelper.h"
#include "CSR_LevelSerializer.h"

// interface
#include "TLandscapeSelection.h"
#include "TShapeSelection.h"
#include "TModelSelection.h"
#include "TSoundSelection.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif
#pragma link "OpenAL32E.lib"
#pragma resource "*.dfm"

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
    m_pOpenALDevice(NULL),
    m_pOpenALContext(NULL),
    m_pSound(NULL),
    m_pShader(NULL),
    m_pSkyboxShader(NULL),
    m_pScene(NULL),
    m_pLandscapeKey(NULL),
    m_pSelectedObjectKey(NULL),
    m_pEffect(NULL),
    m_pMSAA(NULL),
    m_FrameCount(0),
    m_PrevOrigin(0),
    m_Angle(0.0f),
    m_PosVelocity(0.0f),
    m_DirVelocity(0.0f),
    m_FPS(0.0),
    m_StartTime(0),
    m_PreviousTime(0),
    m_Initialized(false)
{
    // build the model dir from the application exe
    UnicodeString sceneDir = ::ExtractFilePath(Application->ExeName);
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExcludeTrailingPathDelimiter(sceneDir) + L"\\Scenes";
                m_SceneDir =   AnsiString(sceneDir).c_str();

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

    // delete the scene completely and shutdown OpenGL
    DeleteScene();
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
    m_StartTime    = ::GetTickCount();
    m_PreviousTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject* pSender)
{
    if (!m_Initialized)
        return;

    // update the viewport
    CreateViewport(paEngineView->ClientWidth, paEngineView->ClientHeight);
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
    //
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miFileSaveClick(TObject* pSender)
{
    CSR_LevelSerializer serializer;

    if (!serializer.Save("test.xml", m_pScene, m_LevelManager))
    {
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miFileExitClick(TObject* pSender)
{
    Close();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddBoxClick(TObject* pSender)
{
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
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = ::FloatToStr(-m_pScene->m_ViewMatrix.m_Table[3][0]);
    pShapeSelection->vfPosition->edY->Text = ::FloatToStr(collisionOutput.m_GroundPos);

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    const bool repeatTexOnEachFace = pShapeSelection->ckRepeatTextureOnEachFace->Checked;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default box mesh
    CSR_Mesh* pBox = csrShapeCreateBox(1.0f,
                                       1.0f,
                                       1.0f,
                                       repeatTexOnEachFace ? 1 : 0,
                                      &vf,
                                       0,
                                      &material,
                                       0);

    // succeeded?
    if (!pBox)
        return;

    try
    {
        // load the texture
        pBox->m_Shader.m_TextureID = LoadTexture(pShapeSelection->edTextureFileName->Text.c_str());

        // failed?
        if (pBox->m_Shader.m_TextureID == M_CSR_Error_Code)
        {
            // show the error message to the user
            ::MessageDlg(L"Unknown texture format.\r\n\r\nThe box could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // add a new item to the manager
        CSR_LevelManager::IItem* pItem = m_LevelManager.Add(pBox);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            // show the error message to the user
            ::MessageDlg(L"Internal error while creating the scene manager item.\r\n\r\nThe box could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // keep the model resources
        pItem->m_Resource.m_ShapeType               = CSR_LevelManager::IE_S_Box;
        pItem->m_Resource.m_Files.m_Texture         = AnsiString(pShapeSelection->edTextureFileName->Text).c_str();
        pItem->m_Resource.m_RepeatTextureOnEachFace = repeatTexOnEachFace;

        // build the model matrix from the interface
        pShapeSelection->BuildMatrix(pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        csrSceneAddMesh(m_pScene, pBox, 0, 1);
        csrSceneAddModelMatrix(m_pScene, pBox, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        CSR_DesignerView::ISelection selection;
        selection.m_pKey        = pBox;
        selection.m_MatrixIndex = 0;

        // select the newly added model
        m_pDesignerView->SetSelection(selection);

        pBox = NULL;
    }
    __finally
    {
        csrMeshRelease(pBox);
    }

    paDesignerView->Invalidate();

    // refresh the selected model properties
    RefreshProperties();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddCylinderClick(TObject* pSender)
{
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
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = ::FloatToStr(-m_pScene->m_ViewMatrix.m_Table[3][0]);
    pShapeSelection->vfPosition->edY->Text = ::FloatToStr(collisionOutput.m_GroundPos);

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    const int faces = ::StrToInt(pShapeSelection->edFaces->Text);

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default cylinder mesh
    CSR_Mesh* pCylinder = csrShapeCreateCylinder(0.5f, 1.0f, faces, &vf, 0, &material, 0);

    // succeeded?
    if (!pCylinder)
        return;

    try
    {
        // load the texture
        pCylinder->m_Shader.m_TextureID = LoadTexture(pShapeSelection->edTextureFileName->Text.c_str());

        // failed?
        if (pCylinder->m_Shader.m_TextureID == M_CSR_Error_Code)
        {
            // show the error message to the user
            ::MessageDlg(L"Unknown texture format.\r\n\r\nThe cylinder could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // add a new item to the manager
        CSR_LevelManager::IItem* pItem = m_LevelManager.Add(pCylinder);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            // show the error message to the user
            ::MessageDlg(L"Internal error while creating the scene manager item.\r\n\r\nThe cylinder could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // keep the model resources
        pItem->m_Resource.m_ShapeType       = CSR_LevelManager::IE_S_Cylinder;
        pItem->m_Resource.m_Files.m_Texture = AnsiString(pShapeSelection->edTextureFileName->Text).c_str();
        pItem->m_Resource.m_Faces           = faces;

        // build the model matrix from the interface
        pShapeSelection->BuildMatrix(pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        csrSceneAddMesh(m_pScene, pCylinder, 0, 1);
        csrSceneAddModelMatrix(m_pScene, pCylinder, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        CSR_DesignerView::ISelection selection;
        selection.m_pKey        = pCylinder;
        selection.m_MatrixIndex = 0;

        // select the newly added model
        m_pDesignerView->SetSelection(selection);

        pCylinder = NULL;
    }
    __finally
    {
        csrMeshRelease(pCylinder);
    }

    paDesignerView->Invalidate();

    // refresh the selected model properties
    RefreshProperties();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddDiskClick(TObject* pSender)
{
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
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = ::FloatToStr(-m_pScene->m_ViewMatrix.m_Table[3][0]);
    pShapeSelection->vfPosition->edY->Text = ::FloatToStr(collisionOutput.m_GroundPos);

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    const int slices = ::StrToInt(pShapeSelection->edSlices->Text);

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default disk mesh
    CSR_Mesh* pDisk = csrShapeCreateDisk(0.0f, 0.0f, 0.5f, slices, &vf, 0, &material, 0);

    // succeeded?
    if (!pDisk)
        return;

    try
    {
        // load the texture
        pDisk->m_Shader.m_TextureID = LoadTexture(pShapeSelection->edTextureFileName->Text.c_str());

        // failed?
        if (pDisk->m_Shader.m_TextureID == M_CSR_Error_Code)
        {
            // show the error message to the user
            ::MessageDlg(L"Unknown texture format.\r\n\r\nThe disk could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // add a new item to the manager
        CSR_LevelManager::IItem* pItem = m_LevelManager.Add(pDisk);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            // show the error message to the user
            ::MessageDlg(L"Internal error while creating the scene manager item.\r\n\r\nThe disk could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // keep the model resources
        pItem->m_Resource.m_ShapeType       = CSR_LevelManager::IE_S_Cylinder;
        pItem->m_Resource.m_Files.m_Texture = AnsiString(pShapeSelection->edTextureFileName->Text).c_str();
        pItem->m_Resource.m_Slices          = slices;

        // build the model matrix from the interface
        pShapeSelection->BuildMatrix(pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        csrSceneAddMesh(m_pScene, pDisk, 0, 1);
        csrSceneAddModelMatrix(m_pScene, pDisk, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        CSR_DesignerView::ISelection selection;
        selection.m_pKey        = pDisk;
        selection.m_MatrixIndex = 0;

        // select the newly added model
        m_pDesignerView->SetSelection(selection);

        pDisk = NULL;
    }
    __finally
    {
        csrMeshRelease(pDisk);
    }

    paDesignerView->Invalidate();

    // refresh the selected model properties
    RefreshProperties();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddRingClick(TObject* pSender)
{
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
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = ::FloatToStr(-m_pScene->m_ViewMatrix.m_Table[3][0]);
    pShapeSelection->vfPosition->edY->Text = ::FloatToStr(collisionOutput.m_GroundPos);

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    const int slices = ::StrToInt(pShapeSelection->edSlices->Text);
    const int radius = ::StrToInt(pShapeSelection->edMinRadius->Text);

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default ring mesh
    CSR_Mesh* pRing = csrShapeCreateRing(0.0f,
                                         0.0f,
                                         0.5f * (float(radius) * 0.01f),
                                         0.5f,
                                         slices,
                                        &vf,
                                         0,
                                        &material,
                                         0);

    // succeeded?
    if (!pRing)
        return;

    try
    {
        // load the texture
        pRing->m_Shader.m_TextureID = LoadTexture(pShapeSelection->edTextureFileName->Text.c_str());

        // failed?
        if (pRing->m_Shader.m_TextureID == M_CSR_Error_Code)
        {
            // show the error message to the user
            ::MessageDlg(L"Unknown texture format.\r\n\r\nThe ring could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // add a new item to the manager
        CSR_LevelManager::IItem* pItem = m_LevelManager.Add(pRing);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            // show the error message to the user
            ::MessageDlg(L"Internal error while creating the scene manager item.\r\n\r\nThe ring could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // keep the model resources
        pItem->m_Resource.m_ShapeType       = CSR_LevelManager::IE_S_Ring;
        pItem->m_Resource.m_Files.m_Texture = AnsiString(pShapeSelection->edTextureFileName->Text).c_str();
        pItem->m_Resource.m_Slices          = slices;
        pItem->m_Resource.m_Radius          = radius;

        // build the model matrix from the interface
        pShapeSelection->BuildMatrix(pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        csrSceneAddMesh(m_pScene, pRing, 0, 1);
        csrSceneAddModelMatrix(m_pScene, pRing, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        CSR_DesignerView::ISelection selection;
        selection.m_pKey        = pRing;
        selection.m_MatrixIndex = 0;

        // select the newly added model
        m_pDesignerView->SetSelection(selection);

        pRing = NULL;
    }
    __finally
    {
        csrMeshRelease(pRing);
    }

    paDesignerView->Invalidate();

    // refresh the selected model properties
    RefreshProperties();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddSphereClick(TObject* pSender)
{
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
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = ::FloatToStr(-m_pScene->m_ViewMatrix.m_Table[3][0]);
    pShapeSelection->vfPosition->edY->Text = ::FloatToStr(collisionOutput.m_GroundPos);

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    const int slices = ::StrToInt(pShapeSelection->edSlices->Text);
    const int stacks = ::StrToInt(pShapeSelection->edStacks->Text);

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default sphere mesh
    CSR_Mesh* pSphere = csrShapeCreateSphere(0.5f, slices, stacks, &vf, 0, &material, 0);

    // succeeded?
    if (!pSphere)
        return;

    try
    {
        // load the texture
        pSphere->m_Shader.m_TextureID = LoadTexture(pShapeSelection->edTextureFileName->Text.c_str());

        // failed?
        if (pSphere->m_Shader.m_TextureID == M_CSR_Error_Code)
        {
            // show the error message to the user
            ::MessageDlg(L"Unknown texture format.\r\n\r\nThe sphere could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // add a new item to the manager
        CSR_LevelManager::IItem* pItem = m_LevelManager.Add(pSphere);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            // show the error message to the user
            ::MessageDlg(L"Internal error while creating the scene manager item.\r\n\r\nThe sphere could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // keep the model resources
        pItem->m_Resource.m_ShapeType       = CSR_LevelManager::IE_S_Sphere;
        pItem->m_Resource.m_Files.m_Texture = AnsiString(pShapeSelection->edTextureFileName->Text).c_str();
        pItem->m_Resource.m_Slices          = slices;
        pItem->m_Resource.m_Stacks          = stacks;

        // build the model matrix from the interface
        pShapeSelection->BuildMatrix(pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        csrSceneAddMesh(m_pScene, pSphere, 0, 1);
        csrSceneAddModelMatrix(m_pScene, pSphere, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        CSR_DesignerView::ISelection selection;
        selection.m_pKey        = pSphere;
        selection.m_MatrixIndex = 0;

        // select the newly added model
        m_pDesignerView->SetSelection(selection);

        pSphere = NULL;
    }
    __finally
    {
        csrMeshRelease(pSphere);
    }

    paDesignerView->Invalidate();

    // refresh the selected model properties
    RefreshProperties();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddSpiralClick(TObject* pSender)
{
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
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = ::FloatToStr(-m_pScene->m_ViewMatrix.m_Table[3][0]);
    pShapeSelection->vfPosition->edY->Text = ::FloatToStr(collisionOutput.m_GroundPos);

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    const int radius   = ::StrToInt(pShapeSelection->edMinRadius->Text);
    const int deltaMin = ::StrToInt(pShapeSelection->edDeltaMin->Text);
    const int deltaMax = ::StrToInt(pShapeSelection->edDeltaMax->Text);
    const int deltaZ   = ::StrToInt(pShapeSelection->edDeltaZ->Text);
    const int slices   = ::StrToInt(pShapeSelection->edSlices->Text);
    const int stacks   = ::StrToInt(pShapeSelection->edStacks->Text);

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default spiral mesh
    CSR_Mesh* pSpiral = csrShapeCreateSpiral(0.0f,
                                             0.0f,
                                             0.5f * (float(radius) * 0.01f),
                                             0.5f,
                                             float(deltaMin) * 0.001f,
                                             float(deltaMax) * 0.001f,
                                             float(deltaZ)   * 0.001f,
                                             slices,
                                             stacks,
                                            &vf,
                                             0,
                                            &material,
                                             0);

    // succeeded?
    if (!pSpiral)
        return;

    try
    {
        // load the texture
        pSpiral->m_Shader.m_TextureID = LoadTexture(pShapeSelection->edTextureFileName->Text.c_str());

        // failed?
        if (pSpiral->m_Shader.m_TextureID == M_CSR_Error_Code)
        {
            // show the error message to the user
            ::MessageDlg(L"Unknown texture format.\r\n\r\nThe spiral could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // add a new item to the manager
        CSR_LevelManager::IItem* pItem = m_LevelManager.Add(pSpiral);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            // show the error message to the user
            ::MessageDlg(L"Internal error while creating the scene manager item.\r\n\r\nThe spiral could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // keep the model resources
        pItem->m_Resource.m_ShapeType       = CSR_LevelManager::IE_S_Spiral;
        pItem->m_Resource.m_Files.m_Texture = AnsiString(pShapeSelection->edTextureFileName->Text).c_str();
        pItem->m_Resource.m_Radius          = radius;
        pItem->m_Resource.m_DeltaMin        = deltaMin;
        pItem->m_Resource.m_DeltaMax        = deltaMax;
        pItem->m_Resource.m_DeltaZ          = deltaZ;
        pItem->m_Resource.m_Slices          = slices;
        pItem->m_Resource.m_Stacks          = stacks;

        // build the model matrix from the interface
        pShapeSelection->BuildMatrix(pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        csrSceneAddMesh(m_pScene, pSpiral, 0, 1);
        csrSceneAddModelMatrix(m_pScene, pSpiral, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        CSR_DesignerView::ISelection selection;
        selection.m_pKey        = pSpiral;
        selection.m_MatrixIndex = 0;

        // select the newly added model
        m_pDesignerView->SetSelection(selection);

        pSpiral = NULL;
    }
    __finally
    {
        csrMeshRelease(pSpiral);
    }

    paDesignerView->Invalidate();

    // refresh the selected model properties
    RefreshProperties();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddSurfaceClick(TObject* pSender)
{
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
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pShapeSelection->vfPosition->edX->Text = ::FloatToStr(-m_pScene->m_ViewMatrix.m_Table[3][0]);
    pShapeSelection->vfPosition->edY->Text = ::FloatToStr(collisionOutput.m_GroundPos);

    // show the default scaling
    pShapeSelection->vfScaling->edX->Text = L"1.0";
    pShapeSelection->vfScaling->edY->Text = L"1.0";
    pShapeSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pShapeSelection->ShowModal() != mrOk)
        return;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default surface mesh
    CSR_Mesh* pSurface = csrShapeCreateSurface(1.0f, 1.0f, &vf, 0, &material, 0);

    // succeeded?
    if (!pSurface)
        return;

    try
    {
        // load the texture
        pSurface->m_Shader.m_TextureID = LoadTexture(pShapeSelection->edTextureFileName->Text.c_str());

        // failed?
        if (pSurface->m_Shader.m_TextureID == M_CSR_Error_Code)
        {
            // show the error message to the user
            ::MessageDlg(L"Unknown texture format.\r\n\r\nThe surface could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // add a new item to the manager
        CSR_LevelManager::IItem* pItem = m_LevelManager.Add(pSurface);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            // show the error message to the user
            ::MessageDlg(L"Internal error while creating the scene manager item.\r\n\r\nThe surface could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // keep the model resources
        pItem->m_Resource.m_ShapeType       = CSR_LevelManager::IE_S_Surface;
        pItem->m_Resource.m_Files.m_Texture = AnsiString(pShapeSelection->edTextureFileName->Text).c_str();

        // build the model matrix from the interface
        pShapeSelection->BuildMatrix(pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        csrSceneAddMesh(m_pScene, pSurface, 0, 1);
        csrSceneAddModelMatrix(m_pScene, pSurface, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        CSR_DesignerView::ISelection selection;
        selection.m_pKey        = pSurface;
        selection.m_MatrixIndex = 0;

        // select the newly added model
        m_pDesignerView->SetSelection(selection);

        pSurface = NULL;
    }
    __finally
    {
        csrMeshRelease(pSurface);
    }

    paDesignerView->Invalidate();

    // refresh the selected model properties
    RefreshProperties();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddWaveFrontClick(TObject* pSender)
{
    // create a model selection dialog box
    std::auto_ptr<TModelSelection> pModelSelection
            (new TModelSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = 0.5f;

    CSR_CollisionOutput collisionOutput;

    // find the ground position at which the box will be set
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pModelSelection->vfPosition->edX->Text = ::FloatToStr(-m_pScene->m_ViewMatrix.m_Table[3][0]);
    pModelSelection->vfPosition->edY->Text = ::FloatToStr(collisionOutput.m_GroundPos);

    // show the default scaling
    pModelSelection->vfScaling->edX->Text = L"1.0";
    pModelSelection->vfScaling->edY->Text = L"1.0";
    pModelSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pModelSelection->ShowModal() != mrOk)
        return;

    const std::string fileName = AnsiString(pModelSelection->edModelFileName->Text).c_str();

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // load the Wavefront model
    CSR_Model* pModel = csrWaveFrontOpen(fileName.c_str(), &vf, 0, &material, 0, 0);

    // succeeded?
    if (!pModel || !pModel->m_MeshCount)
    {
        // show the error message to the user
        ::MessageDlg(L"Failed to load the WaveFront model.", mtError, TMsgDlgButtons() << mbOK, 0);
        return;
    }

    try
    {
        // load the texture
        pModel->m_pMesh[0].m_Shader.m_TextureID = LoadTexture(pModelSelection->edTextureFileName->Text.c_str());

        // failed?
        if (pModel->m_pMesh[0].m_Shader.m_TextureID == M_CSR_Error_Code)
        {
            // show the error message to the user
            ::MessageDlg(L"Unknown texture format.\r\n\r\nThe WaveFront model could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // add a new item to the manager
        CSR_LevelManager::IItem* pItem = m_LevelManager.Add(pModel);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            // show the error message to the user
            ::MessageDlg(L"Internal error while creating the scene manager item.\r\n\r\nThe WaveFront model could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // keep the model resources
        pItem->m_Resource.m_ShapeType       = CSR_LevelManager::IE_S_WaveFront;
        pItem->m_Resource.m_Files.m_Model   = fileName;
        pItem->m_Resource.m_Files.m_Texture = AnsiString(pModelSelection->edTextureFileName->Text).c_str();

        // build the model matrix from the interface
        pModelSelection->BuildMatrix(pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        csrSceneAddModel(m_pScene, pModel, 0, 1);
        csrSceneAddModelMatrix(m_pScene, pModel, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        CSR_DesignerView::ISelection selection;
        selection.m_pKey        = pModel;
        selection.m_MatrixIndex = 0;

        // select the newly added model
        m_pDesignerView->SetSelection(selection);

        pModel = NULL;
    }
    __finally
    {
        csrModelRelease(pModel);
    }

    paDesignerView->Invalidate();

    // refresh the selected model properties
    RefreshProperties();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miAddMDLModelClick(TObject* pSender)
{
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
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // show the default position
    pModelSelection->vfPosition->edX->Text = ::FloatToStr(-m_pScene->m_ViewMatrix.m_Table[3][0]);
    pModelSelection->vfPosition->edY->Text = ::FloatToStr(collisionOutput.m_GroundPos);

    // show the default scaling
    pModelSelection->vfScaling->edX->Text = L"1.0";
    pModelSelection->vfScaling->edY->Text = L"1.0";
    pModelSelection->vfScaling->edZ->Text = L"1.0";

    // show the dialog box to the user and check if action was canceled
    if (pModelSelection->ShowModal() != mrOk)
        return;

    const std::string fileName = AnsiString(pModelSelection->edModelFileName->Text).c_str();

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // load the MDL model
    CSR_MDL* pMDL = csrMDLOpen(fileName.c_str(), 0, &vf, 0, &material, 0, 0);

    // succeeded?
    if (!pMDL)
    {
        // show the error message to the user
        ::MessageDlg(L"Failed to load the MDL model.", mtError, TMsgDlgButtons() << mbOK, 0);
        return;
    }

    try
    {
        // add a new item to the manager
        CSR_LevelManager::IItem* pItem = m_LevelManager.Add(pMDL);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            // show the error message to the user
            ::MessageDlg(L"Internal error while creating the scene manager item.\r\n\r\nThe MDL model could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // keep the model resources
        pItem->m_Resource.m_ShapeType     = CSR_LevelManager::IE_S_MDL;
        pItem->m_Resource.m_Files.m_Model = fileName;

        // build the model matrix from the interface
        pModelSelection->BuildMatrix(pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        csrSceneAddMDL(m_pScene, pMDL, 0, 1);
        csrSceneAddModelMatrix(m_pScene, pMDL, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        CSR_DesignerView::ISelection selection;
        selection.m_pKey        = pMDL;
        selection.m_MatrixIndex = 0;

        // select the newly added model
        m_pDesignerView->SetSelection(selection);

        pMDL = NULL;
    }
    __finally
    {
        csrMDLRelease(pMDL);
    }

    paDesignerView->Invalidate();

    // refresh the selected model properties
    RefreshProperties();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miLandscapeResetViewportClick(TObject* pSender)
{
    // reset the viewpoint bounding sphere to his default position
    InitializeViewPoint();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miSkyboxAddClick(TObject* pSender)
{
    // create a model selection dialog box
    std::auto_ptr<TSkyboxSelection> pSkyboxSelection
            (new TSkyboxSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // show the dialog box to the user and check if action was canceled
    if (pSkyboxSelection->ShowModal() != mrOk)
        return;

    // is skybox shader still not loaded?
    if (!m_pSkyboxShader)
    {
        const std::string vsTextured = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Skybox);
        const std::string fsTextured = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Skybox);

        // load the shader
        m_pSkyboxShader  = csrShaderLoadFromStr(vsTextured.c_str(),
                                                vsTextured.length(),
                                                fsTextured.c_str(),
                                                fsTextured.length(),
                                                0,
                                                0);

        // succeeded?
        if (!m_pSkyboxShader)
        {
            // show the error message to the user
            ::MessageDlg(L"Failed to load the skybox shader.\r\n\r\nThe skybox could not be created.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return;
        }

        // enable the shader program
        csrShaderEnable(m_pSkyboxShader);

        // get shader attributes
        m_pSkyboxShader->m_VertexSlot  = glGetAttribLocation (m_pSkyboxShader->m_ProgramID, "csr_aVertices");
        m_pSkyboxShader->m_CubemapSlot = glGetUniformLocation(m_pSkyboxShader->m_ProgramID, "csr_sCubemap");
    }

    // do delete the previous skybox?
    if (m_pScene->m_pSkybox)
        csrMeshRelease(m_pScene->m_pSkybox);

    // create the skybox
    m_pScene->m_pSkybox = csrSkyboxCreate(1.0f, 1.0f, 1.0f);

    // get the skybox file names
    TSkyboxSelection::IFileNames fileNames;
    pSkyboxSelection->GetFileNames(fileNames);

    // load the cubemap texture
    m_pScene->m_pSkybox->m_Shader.m_CubeMapID = LoadCubemap(fileNames);

    const std::size_t fileNameCount = fileNames.size();

    // iterate through the cubemap texture files and keep each of them in the level manager resources
    for (std::size_t i = 0; i < fileNameCount; i++)
        switch (i)
        {
            case 0: m_LevelManager.m_Skybox.m_Right  = CSR_VCLHelper::WStrToStr(fileNames[i]); continue;
            case 1: m_LevelManager.m_Skybox.m_Left   = CSR_VCLHelper::WStrToStr(fileNames[i]); continue;
            case 2: m_LevelManager.m_Skybox.m_Top    = CSR_VCLHelper::WStrToStr(fileNames[i]); continue;
            case 3: m_LevelManager.m_Skybox.m_Bottom = CSR_VCLHelper::WStrToStr(fileNames[i]); continue;
            case 4: m_LevelManager.m_Skybox.m_Front  = CSR_VCLHelper::WStrToStr(fileNames[i]); continue;
            case 5: m_LevelManager.m_Skybox.m_Back   = CSR_VCLHelper::WStrToStr(fileNames[i]); continue;
        }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miSoundOpenClick(TObject* pSender)
{
    // create a sound selection dialog box
    std::auto_ptr<TSoundSelection> pSoundSelection
            (new TSoundSelection(this, CSR_VCLHelper::StrToWStr(m_SceneDir)));

    // show it to user
    if (pSoundSelection->ShowModal() != mrOk)
        return;

    // do change the ambient sound?
    if (!pSoundSelection->edSoundFileName->Text.IsEmpty())
    {
        // stop the current sound and delete it
        csrSoundStop(m_pSound);
        csrSoundRelease(m_pSound);

        // keep the sound file in the level manager resources
        m_LevelManager.m_Sound.m_FileName = AnsiString(pSoundSelection->edSoundFileName->Text).c_str();

        // load the sound file
        m_pSound = LoadSound(m_LevelManager.m_Sound.m_FileName);
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::miSoundPauseClick(TObject* pSender)
{
    // do pause the sound?
    if (miSoundPause->Checked)
    {
        // pause the background sound
        if (csrSoundIsPlaying(m_pSound))
            csrSoundPause(m_pSound);

        return;
    }

    // play the background sound
    if (!csrSoundIsPlaying(m_pSound))
        csrSoundPlay(m_pSound);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::spMainViewMoved(TObject* pSender)
{
    // get back the scene item containing the model
    CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);

    // found it?
    if (pItem)
        // calculate the ratio to use to draw the designer view
        if (pItem->m_AABBTreeCount)
            m_pDesignerView->SetRatio
                    (paDesignerView->ClientHeight / (pItem->m_pAABBTree[0].m_pBox->m_Max.m_Z -
                            pItem->m_pAABBTree[0].m_pBox->m_Min.m_Z));

    // update the viewport
    CreateViewport(paEngineView->ClientWidth, paEngineView->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::spViewsMoved(TObject *Sender)
{
    // get back the scene item containing the model
    CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);

    // found it?
    if (pItem)
        // calculate the ratio to use to draw the designer view
        if (pItem->m_AABBTreeCount)
            m_pDesignerView->SetRatio
                    (paDesignerView->ClientHeight / (pItem->m_pAABBTree[0].m_pBox->m_Max.m_Z -
                            pItem->m_pAABBTree[0].m_pBox->m_Min.m_Z));

    // update the viewport
    CreateViewport(paEngineView->ClientWidth, paEngineView->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aeEventsMessage(tagMSG& msg, bool& handled)
{
    m_PosVelocity = 0.0f;
    m_DirVelocity = 0.0f;

    switch (msg.message)
    {
        case WM_KEYDOWN:
            // no scene?
            if (!m_pScene)
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

                    // is the main landscape model?
                    if (pSelection->m_pKey == m_pLandscapeKey)
                        break;

                    // get the model key to delete from scene
                    const void* pKeyToDel = pSelection->m_pKey;

                    // select the next model
                    m_pDesignerView->SelectNext();

                    // delete the currently selected model
                    csrSceneDeleteFrom(m_pScene, pKeyToDel);

                    break;
                }
            }

            return;
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
CSR_Shader* TMainForm::OnGetShaderCallback(const void* pModel, CSR_EModelType type)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return 0;

    return pMainForm->OnGetShader(pModel, type);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneBeginCallback(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnSceneBegin(pScene, pContext);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneEndCallback(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    TMainForm* pMainForm = static_cast<TMainForm*>(Application->MainForm);

    if (!pMainForm)
        return;

    pMainForm->OnSceneEnd(pScene, pContext);
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
            if (!m_pScene)
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
            if (!m_pScene)
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

    const std::wstring textureFileName = pLandscapeSelection->edTextureFileName->Text.c_str();

    // do change the landscape model?
    if (pLandscapeSelection->rbSourceBitmap->Checked)
    {
        // recreate the scene
        CreateScene();

        // load the landscape from a grayscale image
        if (!LoadLandscapeFromBitmap(AnsiString(pLandscapeSelection->edBitmapFileName->Text).c_str(),
                CSR_VCLHelper::WStrToStr(textureFileName)))
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
        if (!LoadLandscape(AnsiString(pLandscapeSelection->edModelFileName->Text).c_str(),
                CSR_VCLHelper::WStrToStr(textureFileName)))
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
        const CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);

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
        glDeleteTextures(1, &pModel->m_pMesh[0].m_Shader.m_TextureID);

        // load the new one
        pModel->m_pMesh[0].m_Shader.m_TextureID = LoadTexture(textureFileName);

        // failed?
        if (pModel->m_pMesh[0].m_Shader.m_TextureID == M_CSR_Error_Code)
        {
            // show the error message to the user
            ::MessageDlg(L"Failed to load the texture.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);
            return false;
        }
    }

    paDesignerView->Invalidate();

    return true;
}
//------------------------------------------------------------------------------
void TMainForm::CloseDocument()
{
    // stop the sound
    csrSoundStop(m_pSound);
    csrSoundRelease(m_pSound);
    m_pSound = NULL;

    // clear the properties view
    ClearProperties();

    // clear the designer view
    m_pDesignerView->Unselect();
    m_pDesignerView->SetScene(NULL);

    // clear the level manager
    m_LevelManager.Clear();

    // release the scene
    csrSceneRelease(m_pScene);
    m_pScene = NULL;

    // clear the local values
    m_pLandscapeKey      = NULL;
    m_pSelectedObjectKey = NULL;
}
//---------------------------------------------------------------------------
bool TMainForm::LoadLandscape(const std::string& fileName, const std::string& textureName)
{
    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexCulling vc;
    vc.m_Type = CSR_CT_Front;
    vc.m_Face = CSR_CF_CW;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // load the model
    CSR_Model* pModel = csrWaveFrontOpen(fileName.c_str(), &vf, &vc, &material, 0, 0);

    // succeeded?
    if (!pModel)
        return false;

    // add a new item to the manager
    CSR_LevelManager::IItem* pItem = m_LevelManager.Add(pModel);

    // succeeded?
    if (!pItem || !pItem->m_Matrices.size())
        return false;

    // keep the model resources
    pItem->m_Resource.m_ShapeType       = CSR_LevelManager::IE_S_Landscape;
    pItem->m_Resource.m_Files.m_Model   = fileName;
    pItem->m_Resource.m_Files.m_Texture = textureName;

    // add the model to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

    // succeeded?
    if (pSceneItem)
    {
        pSceneItem->m_CollisionType = CSR_ECollisionType(CSR_CO_Ground | CSR_CO_Custom);

        // calculate the ratio to use to draw the designer view
        if (pSceneItem->m_AABBTreeCount)
            m_pDesignerView->SetRatio
                    (paDesignerView->ClientHeight / (pSceneItem->m_pAABBTree[0].m_pBox->m_Max.m_Z -
                            pSceneItem->m_pAABBTree[0].m_pBox->m_Min.m_Z));
    }

    CSR_DesignerView::ISelection selection;
    selection.m_pKey        = pModel;
    selection.m_MatrixIndex = 0;

    // select the newly added model
    m_pDesignerView->SetSelection(selection);

    // keep the key
    m_pLandscapeKey = pModel;

    // refresh the selected model properties
    RefreshProperties();

    return true;
}
//---------------------------------------------------------------------------
bool TMainForm::LoadLandscapeFromBitmap(const std::string& fileName, const std::string& textureName)
{
    CSR_Model*       pModel  = NULL;
    CSR_PixelBuffer* pBitmap = NULL;

    try
    {
        CSR_Material material;
        material.m_Color       = 0xFFFFFFFF;
        material.m_Transparent = 0;
        material.m_Wireframe   = 0;

        CSR_VertexCulling vc;
        vc.m_Type = CSR_CT_None;
        vc.m_Face = CSR_CF_CW;

        CSR_VertexFormat vf;
        vf.m_HasNormal         = 0;
        vf.m_HasTexCoords      = 1;
        vf.m_HasPerVertexColor = 1;

        // load a default grayscale bitmap from which a landscape will be generated
        pBitmap = csrPixelBufferFromBitmap(fileName.c_str());

        // create a model to contain the landscape
        pModel = csrModelCreate();

        // succeeded?
        if (!pModel)
            return false;

        // load the landscape mesh from the grayscale bitmap
        pModel->m_pMesh     = csrLandscapeCreate(pBitmap, 3.0f, 0.2f, &vf, &vc, &material, 0);
        pModel->m_MeshCount = 1;
    }
    __finally
    {
        csrPixelBufferRelease(pBitmap);
    }

    // add a new item to the manager
    CSR_LevelManager::IItem* pItem = m_LevelManager.Add(pModel);

    // succeeded?
    if (!pItem || !pItem->m_Matrices.size())
        return false;

    // keep the model resources
    pItem->m_Resource.m_ShapeType            = CSR_LevelManager::IE_S_Landscape;
    pItem->m_Resource.m_Files.m_LandscapeMap = fileName;
    pItem->m_Resource.m_Files.m_Texture      = textureName;

    // add the model to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

    // succeeded?
    if (pSceneItem)
    {
        pSceneItem->m_CollisionType = CSR_ECollisionType(CSR_CO_Ground | CSR_CO_Custom);

        // calculate the ratio to use to draw the designer view
        if (pSceneItem->m_AABBTreeCount)
            m_pDesignerView->SetRatio
                    (paDesignerView->ClientHeight / (pSceneItem->m_pAABBTree[0].m_pBox->m_Max.m_Z -
                            pSceneItem->m_pAABBTree[0].m_pBox->m_Min.m_Z));
    }

    CSR_DesignerView::ISelection selection;
    selection.m_pKey        = pModel;
    selection.m_MatrixIndex = 0;

    // select the newly added model
    m_pDesignerView->SetSelection(selection);

    // keep the key
    m_pLandscapeKey = pModel;

    // refresh the selected model properties
    RefreshProperties();

    return true;
}
//---------------------------------------------------------------------------
GLuint TMainForm::LoadTexture(const std::wstring& fileName) const
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
            textureID = csrTextureFromPixelBuffer(pPixelBuffer);
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
GLuint TMainForm::LoadCubemap(const TSkyboxSelection::IFileNames fileNames) const
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
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }
    catch (...)
    {
        return M_CSR_Error_Code;
    }
}
//---------------------------------------------------------------------------
CSR_Sound* TMainForm::LoadSound(const std::string& fileName) const
{
    // load the sound file
    CSR_Sound* pSound = csrSoundOpenWav(m_pOpenALDevice, m_pOpenALContext, fileName.c_str());

    // set the sound to loop
    csrSoundLoop(pSound, 1);

    // play the sound
    csrSoundPlay(pSound);

    return pSound;
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

        // get the selected scene item
        CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pDesignerView->GetSelection()->m_pKey);

        // not found?
        if (!pItem || !pItem->m_pMatrixArray->m_Count)
            return;

        CSR_Matrix4 matrix = *static_cast<CSR_Matrix4*>(pItem->m_pMatrixArray->m_pItem[0].m_pData);

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
void TMainForm::InitializeViewPoint()
{
    // reset the viewpoint bounding sphere to his default position
    m_ViewSphere.m_Center.m_X = 0.0f;
    m_ViewSphere.m_Center.m_Y = 0.0f;
    m_ViewSphere.m_Center.m_Z = 3.08f;
    m_ViewSphere.m_Radius     = 0.1f;
}
//------------------------------------------------------------------------------
void TMainForm::CreateViewport(float w, float h)
{
    if (!m_pShader)
        return;

    // configure the OpenGL viewport
    CSR_OpenGLHelper::CreateViewport(w, h, 0.001f, 1000.0f, m_pShader, m_pScene->m_ProjectionMatrix);

    // multisampling antialiasing was already created?
    if (!m_pMSAA)
        // create the multisampling antialiasing
        m_pMSAA = csrMSAACreate(w, h, 4);
    else
        // change his size
        csrMSAAChangeSize(w, h, m_pMSAA);

    // oil painting post processing effect was already created?
    if (!m_pEffect)
        // create the oil painting post processing effect
        m_pEffect = new CSR_PostProcessingEffect_OilPainting(w, h, 4);
    else
        // change the effect viewport size
        m_pEffect->ChangeSize(w, h);
}
//------------------------------------------------------------------------------
void TMainForm::CreateScene()
{
    // close the previously opened document
    CloseDocument();

    // initialize the scene
    m_pScene = csrSceneCreate();

    // configure the scene background color
    m_pScene->m_Color.m_R = 0.45f;
    m_pScene->m_Color.m_G = 0.8f;
    m_pScene->m_Color.m_B = 1.0f;
    m_pScene->m_Color.m_A = 1.0f;

    // configure the scene ground direction
    m_pScene->m_GroundDir.m_X =  0.0f;
    m_pScene->m_GroundDir.m_Y = -1.0f;
    m_pScene->m_GroundDir.m_Z =  0.0f;

    // configure the scene view matrix
    csrMat4Identity(&m_pScene->m_ViewMatrix);

    // link the scene to the designer view
    m_pDesignerView->SetScene(m_pScene);

    // initialize the viewpoint
    InitializeViewPoint();
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // create the default scene
    CreateScene();

    // set the viewpoint bounding sphere default position
    m_ViewSphere.m_Center.m_X = 0.0f;
    m_ViewSphere.m_Center.m_Y = 0.0f;
    m_ViewSphere.m_Center.m_Z = 3.08f;
    m_ViewSphere.m_Radius     = 0.1f;

    // configure the scene context
    csrSceneContextInit(&m_SceneContext);
    m_SceneContext.m_fOnGetShader  = OnGetShaderCallback;
    m_SceneContext.m_fOnSceneBegin = OnSceneBeginCallback;
    m_SceneContext.m_fOnSceneEnd   = OnSceneEndCallback;

    const std::string vsTextured = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Texture);
    const std::string fsTextured = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Texture);

    // load the shader
    m_pShader  = csrShaderLoadFromStr(vsTextured.c_str(),
                                      vsTextured.length(),
                                      fsTextured.c_str(),
                                      fsTextured.length(),
                                      0,
                                      0);

    // succeeded?
    if (!m_pShader)
    {
        // show the error message to the user
        ::MessageDlg(L"Failed to load the shader.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // enable the shader program
    csrShaderEnable(m_pShader);

    // get shader attributes
    m_pShader->m_VertexSlot   = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aVertices");
    m_pShader->m_ColorSlot    = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aColor");
    m_pShader->m_TexCoordSlot = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aTexCoord");
    m_pShader->m_TextureSlot  = glGetUniformLocation(m_pShader->m_ProgramID, "csr_sTexture");

    // get a default texture file name
    const std::string textureFile = m_SceneDir + "\\Textures\\mountain2.jpg";

    // load the landscape model from a grayscale bitmap file
    if (!LoadLandscapeFromBitmap(m_SceneDir + "\\Bitmaps\\playfield.bmp", textureFile))
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
    CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);

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
    static_cast<CSR_Model*>(pItem->m_pModel)->m_pMesh[0].m_Shader.m_TextureID =
            LoadTexture(CSR_VCLHelper::StrToWStr(textureFile));

    // failed?
    if (static_cast<CSR_Model*>(pItem->m_pModel)->m_pMesh[0].m_Shader.m_TextureID == M_CSR_Error_Code)
    {
        // show the error message to the user
        ::MessageDlg(L"Unknown texture format.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // initialize OpenAL
    csrSoundInitializeOpenAL(&m_pOpenALDevice, &m_pOpenALContext);

    // keep the sound file name in the level manager resources
    m_LevelManager.m_Sound.m_FileName = m_SceneDir + "\\Sounds\\landscape_ambient_sound.wav";

    // load the ambient sound to play
    m_pSound = LoadSound(m_LevelManager.m_Sound.m_FileName);

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

    // release the scene
    csrSceneRelease(m_pScene);

    // release the shaders
    csrShaderRelease(m_pSkyboxShader);
    csrShaderRelease(m_pShader);

    // release the oil painting post processing effect
    if (m_pEffect)
        delete m_pEffect;

    // release the multisampling antialiasing
    csrMSAARelease(m_pMSAA);

    // release OpenAL interface
    csrSoundReleaseOpenAL(m_pOpenALDevice, m_pOpenALContext);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
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

    // keep the old position to revert it in case of error
    const CSR_Vector3 oldPos = m_ViewSphere.m_Center;

    // is player moving?
    if (m_PosVelocity)
    {
        // calculate the next position
        m_ViewSphere.m_Center.m_X += m_PosVelocity * sinf(m_Angle + (M_PI * 0.5f)) * elapsedTime;
        m_ViewSphere.m_Center.m_Z -= m_PosVelocity * cosf(m_Angle + (M_PI * 0.5f)) * elapsedTime;
    }

    // enable the shader program
    csrShaderEnable(m_pShader);

    // calculate the ground position and check if next position is valid
    if (!ApplyGroundCollision(&m_ViewSphere, &m_pScene->m_ViewMatrix))
    {
        // invalid next position, get the scene item (just one for this scene)
        const CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pLandscapeKey);

        // found it?
        if (pItem)
        {
            // check if the x position is out of bounds, and correct it if yes
            if (m_ViewSphere.m_Center.m_X <= pItem->m_pAABBTree->m_pBox->m_Min.m_X ||
                m_ViewSphere.m_Center.m_X >= pItem->m_pAABBTree->m_pBox->m_Max.m_X)
                m_ViewSphere.m_Center.m_X = oldPos.m_X;

            // do the same thing with the z position. Doing that separately for each axis will make
            // the point of view to slide against the landscape border (this is possible because the
            // landscape is axis-aligned)
            if (m_ViewSphere.m_Center.m_Z <= pItem->m_pAABBTree->m_pBox->m_Min.m_Z ||
                m_ViewSphere.m_Center.m_Z >= pItem->m_pAABBTree->m_pBox->m_Max.m_Z)
                m_ViewSphere.m_Center.m_Z = oldPos.m_Z;
        }
        else
            // failed to get the scene item, just revert the position
            m_ViewSphere.m_Center = oldPos;

        // recalculate the ground value (this time the collision result isn't tested, because the
        // previous position is always considered as valid)
        ApplyGroundCollision(&m_ViewSphere, &m_pScene->m_ViewMatrix);
    }

    GLint viewPort[4];
    glGetIntegerv(GL_VIEWPORT, viewPort);

    // calculate the designer view origin in relation to the current view position
    m_pDesignerView->SetOrigin
            (-m_pDesignerView->GetRatio() *
                    ((m_pScene->m_ViewMatrix.m_Table[3][0] * paDesignerView->ClientWidth) / viewPort[2]));
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    // draw the scene
    csrSceneDraw(m_pScene, &m_SceneContext);
}
//---------------------------------------------------------------------------
bool TMainForm::ApplyGroundCollision(const CSR_Sphere* pBoundingSphere, CSR_Matrix4* pMatrix)
{
    if (!m_pScene)
        return false;

    // validate the inputs
    if (!pBoundingSphere || !pMatrix)
        return false;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = pBoundingSphere->m_Radius;

    CSR_Camera camera;

    // calculate the camera position in the 3d world, without the ground value
    camera.m_Position.m_X = -pBoundingSphere->m_Center.m_X;
    camera.m_Position.m_Y =  0.0f;
    camera.m_Position.m_Z = -pBoundingSphere->m_Center.m_Z;
    camera.m_xAngle       =  0.0f;
    camera.m_yAngle       =  m_Angle;
    camera.m_zAngle       =  0.0f;
    camera.m_Factor.m_X   =  1.0f;
    camera.m_Factor.m_Y   =  1.0f;
    camera.m_Factor.m_Z   =  1.0f;
    camera.m_MatCombType  =  IE_CT_Translate_Scale_Rotate;

    // get the view matrix matching with the camera
    csrSceneCameraToMatrix(&camera, pMatrix);

    CSR_Vector3 modelCenter;

    // get the model center
    modelCenter.m_X = 0.0f;
    modelCenter.m_Y = 0.0f;
    modelCenter.m_Z = 0.0f;

    CSR_Matrix4 invertView;
    float       determinant;

    // calculate the current camera position above the landscape
    csrMat4Inverse(pMatrix, &invertView, &determinant);
    csrMat4Transform(&invertView, &modelCenter, &collisionInput.m_BoundingSphere.m_Center);
    collisionInput.m_CheckPos = collisionInput.m_BoundingSphere.m_Center;

    CSR_CollisionOutput collisionOutput;

    // calculate the collisions in the whole scene
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // update the ground position directly inside the matrix (this is where the final value is required)
    pMatrix->m_Table[3][1] = -collisionOutput.m_GroundPos;

    return (collisionOutput.m_Collision & CSR_CO_Ground);
}
//---------------------------------------------------------------------------
void TMainForm::OnPropertiesValueChanged(TObject* pSender, float x, float y, float z)
{
    // get the selected scene item
    CSR_SceneItem* pItem = csrSceneGetItem(m_pScene, m_pDesignerView->GetSelection()->m_pKey);

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
                                   static_cast<CSR_Matrix4*>(pItem->m_pMatrixArray->m_pItem[0].m_pData));

    // update the designer view
    paDesignerView->Invalidate();
}
//---------------------------------------------------------------------------
void TMainForm::OnDrawScene(bool resize)
{
    // do draw the scene for a resize?
    if (resize)
    {
        if (!m_Initialized)
            return;

        // just process a minimal draw
        UpdateScene(0.0);
        DrawScene();

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
    DrawScene();

    ::SwapBuffers(m_hDC);

    // also invalidate the designer view, if required
    if (m_pDesignerView->GetOrigin() != m_PrevOrigin)
    {
        m_PrevOrigin = m_pDesignerView->GetOrigin();
        paDesignerView->Invalidate();
    }
}
//---------------------------------------------------------------------------
CSR_Shader* TMainForm::OnGetShader(const void* pModel, CSR_EModelType type)
{
    if (pModel == m_pScene->m_pSkybox)
        return m_pSkyboxShader;

    return m_pShader;
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    if (miPostProcessingOilPainting->Checked)
        m_pEffect->DrawBegin(&pScene->m_Color);
    else
    if (miPostProcessingAntialiasing->Checked)
        csrMSAADrawBegin(&pScene->m_Color, m_pMSAA);
    else
        csrDrawBegin(&pScene->m_Color);
}
//---------------------------------------------------------------------------
void TMainForm::OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    if (miPostProcessingOilPainting->Checked)
        m_pEffect->DrawEnd();
    else
    if (miPostProcessingAntialiasing->Checked)
        csrMSAADrawEnd(m_pMSAA);
    else
        csrDrawEnd();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;
    OnDrawScene(false);
}
//---------------------------------------------------------------------------
