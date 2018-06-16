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

// interface
#include "TLandscapeSelection.h"
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
// TMainForm::IDesignerItem
//---------------------------------------------------------------------------
TMainForm::IDesignerItem::IDesignerItem()
{
    csrMat4Identity(&m_Matrix);
}
//---------------------------------------------------------------------------
TMainForm::IDesignerItem::~IDesignerItem()
{}
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
void __fastcall TMainForm::miAddBoxClick(TObject* pSender)
{
    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default box mesh
    CSR_Mesh* pBox = csrShapeCreateBox(1.0f, 1.0f, 1.0f, 0, &vf, 0, &material, 0);

    // succeeded?
    if (!pBox)
        return;

    try
    {
        CSR_CollisionInput collisionInput;
        csrCollisionInputInit(&collisionInput);
        collisionInput.m_BoundingSphere.m_Radius = 0.5f;

        CSR_CollisionOutput collisionOutput;

        // calculate the collisions in the whole scene
        csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

        std::auto_ptr<IDesignerItem> pItem(new IDesignerItem());

        // update the position in the model matrix
        pItem->m_Matrix.m_Table[3][0] = -m_pScene->m_Matrix.m_Table[3][0];
        pItem->m_Matrix.m_Table[3][1] =  collisionOutput.m_GroundPos;

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        csrSceneAddMesh(m_pScene, pBox, 0, 1);
        csrSceneAddModelMatrix(m_pScene, pBox, &pItem->m_Matrix);

        // link the scene item to the designer
        m_Designer[pBox].push_back(pItem.get());
        pItem.release();

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
void __fastcall TMainForm::miLandscapeResetViewportClick(TObject* pSender)
{
    // reset the viewpoint bounding sphere to his default position
    InitializeViewPoint();
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
void __fastcall TMainForm::miSoundOpenClick(TObject* pSender)
{
    // create a sound selection dialog box
    std::auto_ptr<TSoundSelection> pSoundSelection
            (new TSoundSelection(this, UnicodeString(AnsiString(m_SceneDir.c_str())).c_str()));

    // show it to user
    if (pSoundSelection->ShowModal() != mrOk)
        return;

    // do change the ambient sound?
    if (!pSoundSelection->edSoundFileName->Text.IsEmpty())
    {
        // stop the current sound and delete it
        csrSoundStop(m_pSound);
        csrSoundRelease(m_pSound);

        // load the sound file
        m_pSound = LoadSound(AnsiString(pSoundSelection->edSoundFileName->Text).c_str());
    }
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
    if (Application->MainForm->ActiveControl &&
        Application->MainForm->ActiveControl->InheritsFrom(__classid(TEdit)))
        return;

    m_PosVelocity = 0.0f;
    m_DirVelocity = 0.0f;

    switch (msg.message)
    {
        case WM_KEYDOWN:
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
            (new TLandscapeSelection(this, UnicodeString(AnsiString(m_SceneDir.c_str())).c_str()));

    // execute the dialog and check if user canceled it
    if (pLandscapeSelection->ShowModal() != mrOk)
        return true;

    // do change the landscape model?
    if (pLandscapeSelection->rbSourceBitmap->Checked)
    {
        // recreate the scene
        CreateScene();

        // load the landscape from a grayscale image
        if (!LoadLandscapeFromBitmap(AnsiString(pLandscapeSelection->edBitmapFileName->Text).c_str()))
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
        if (!LoadLandscape(AnsiString(pLandscapeSelection->edModelFileName->Text).c_str()))
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
    if (!pLandscapeSelection->edTextureFileName->Text.IsEmpty())
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
        const CSR_Model* pModel = (CSR_Model*)pItem->m_pModel;

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
        pModel->m_pMesh[0].m_Shader.m_TextureID =
                LoadTexture(pLandscapeSelection->edTextureFileName->Text.c_str());

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

    // iterate through the designer items
    for (IDesigner::iterator it = m_Designer.begin(); it != m_Designer.end(); ++it)
    {
        // iterate through the designer item sub-objects
        for (std::size_t i = 0; i < it->second.size(); ++i)
            // delete all of them
            delete it->second[i];

        // clear the designer item
        it->second.clear();
    }

    // clear the designer
    m_Designer.clear();

    // release the scene
    csrSceneRelease(m_pScene);
    m_pScene = NULL;

    // clear the local values
    m_pLandscapeKey      = NULL;
    m_pSelectedObjectKey = NULL;
}
//---------------------------------------------------------------------------
bool TMainForm::LoadLandscape(const std::string& fileName)
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

    std::auto_ptr<IDesignerItem> pItem(new IDesignerItem());

    // add the model to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, &pItem->m_Matrix);

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

    // link the scene item to the designer
    m_Designer[pModel].push_back(pItem.get());
    pItem.release();

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
bool TMainForm::LoadLandscapeFromBitmap(const std::string& fileName)
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

    std::auto_ptr<IDesignerItem> pItem(new IDesignerItem());

    // add the model to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, &pItem->m_Matrix);

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

    // link the scene item to the designer
    m_Designer[pModel].push_back(pItem.get());
    pItem.release();

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
        default:      return M_CSR_Error_Code;
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

        // reserve memory for the pixel array
        pPixelBuffer->m_pData = new unsigned char[pPixelBuffer->m_DataLength];

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

        CSR_Matrix4 matrix = *((CSR_Matrix4*)pItem->m_pMatrixArray->m_pItem[0].m_pData);

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
    CSR_OpenGLHelper::CreateViewport(w, h, 0.001f, 1000.0f, m_pShader, m_ProjectionMatrix);

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
    csrMat4Identity(&m_pScene->m_Matrix);

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

    // load the landscape model from a grayscale bitmap file
    if (!LoadLandscapeFromBitmap(m_SceneDir + "\\Bitmaps\\playfield.bmp"))
    {
        // show the error message to the user
        ::MessageDlg(L"An error occurred while the default landscape model was created.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // get a default texture file name
    const UnicodeString textureFile =
            UnicodeString(AnsiString(m_SceneDir.c_str())) + L"\\Textures\\mountain2.jpg";

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
    ((CSR_Model*)pItem->m_pModel)->m_pMesh[0].m_Shader.m_TextureID = LoadTexture(textureFile.c_str());

    // failed?
    if (((CSR_Model*)pItem->m_pModel)->m_pMesh[0].m_Shader.m_TextureID == M_CSR_Error_Code)
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

    // load the ambient sound to play
    m_pSound = LoadSound(m_SceneDir + "\\Sounds\\landscape_ambient_sound.wav");

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

    // release the shader
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
    if (!ApplyGroundCollision(&m_ViewSphere, &m_pScene->m_Matrix))
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
        ApplyGroundCollision(&m_ViewSphere, &m_pScene->m_Matrix);
    }

    GLint viewPort[4];
    glGetIntegerv(GL_VIEWPORT, viewPort);

    // calculate the designer view origin in relation to the current view position
    m_pDesignerView->SetOrigin
            (-m_pDesignerView->GetRatio() * ((m_pScene->m_Matrix.m_Table[3][0] * paDesignerView->ClientWidth) / viewPort[2]));
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
    CSR_Vector3 axis;
    CSR_Vector3 factor;
    CSR_Matrix4 matrixTranslate;
    CSR_Matrix4 matrixX;
    CSR_Matrix4 matrixY;
    CSR_Matrix4 matrixZ;
    CSR_Matrix4 matrixScale;
    CSR_Matrix4 buildMatrix1;
    CSR_Matrix4 buildMatrix2;
    CSR_Matrix4 buildMatrix3;

    // get the translation values to apply
    t.m_X = m_DesignerProperties.m_pTransformTranslate->GetX();
    t.m_Y = m_DesignerProperties.m_pTransformTranslate->GetY();
    t.m_Z = m_DesignerProperties.m_pTransformTranslate->GetZ();

    // create a translation matrix
    csrMat4Translate(&t, &matrixTranslate);

    // get the rotation x axis
    axis.m_X = 1.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 0.0f;

    // create a rotation matrix on the x axis
    csrMat4Rotate(m_DesignerProperties.m_pTransformRotate->GetX(), &axis, &matrixX);

    // get the rotation y axis
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    // create a rotation matrix on the y axis
    csrMat4Rotate(m_DesignerProperties.m_pTransformRotate->GetY(), &axis, &matrixY);

    // get the rotation z axis
    axis.m_X = 0.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 1.0f;

    // create a rotation matrix on the z axis
    csrMat4Rotate(m_DesignerProperties.m_pTransformRotate->GetZ(), &axis, &matrixZ);

    // get the scale values to apply
    factor.m_X = m_DesignerProperties.m_pTransformScale->GetX();
    factor.m_Y = m_DesignerProperties.m_pTransformScale->GetY();
    factor.m_Z = m_DesignerProperties.m_pTransformScale->GetZ();

    // create a scale matrix
    csrMat4Scale(&factor, &matrixScale);

    // rebuild the selected object model matrix
    csrMat4Multiply(&matrixScale,  &matrixX,         &buildMatrix1);
    csrMat4Multiply(&buildMatrix1, &matrixY,         &buildMatrix2);
    csrMat4Multiply(&buildMatrix2, &matrixZ,         &buildMatrix3);
    csrMat4Multiply(&buildMatrix3, &matrixTranslate, ((CSR_Matrix4*)pItem->m_pMatrixArray->m_pItem[0].m_pData));

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
