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

// std
#include <memory>
#include <sstream>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Scene.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif
#pragma resource "*.dfm"

//----------------------------------------------------------------------------
/**
* Vertex shader program to show a colored mesh
*@note The view matrix is implicitly set to identity and combined to the model one
*/
const char g_VertexProgram_ColoredMesh[] =
    "precision mediump float;"
    "attribute vec3 csr_vVertex;"
    "attribute vec4 csr_vColor;"
    "uniform   mat4 csr_uProjection;"
    "uniform   mat4 csr_uModelView;"
    "varying   vec4 csr_fColor;"
    "void main(void)"
    "{"
    "    csr_fColor   = csr_vColor;"
    "    gl_Position  = csr_uProjection * csr_uModelView * vec4(csr_vVertex, 1.0);"
    "}";
//----------------------------------------------------------------------------
/**
* Fragment shader program to show a colored mesh
*/
const char g_FragmentProgram_ColoredMesh[] =
    "precision mediump float;"
    "varying lowp vec4 csr_fColor;"
    "void main(void)"
    "{"
    "    gl_FragColor = csr_fColor;"
    "}";
//----------------------------------------------------------------------------
/**
* Vertex shader program to show a textured mesh
*@note The view matrix is implicitly set to identity and combined to the model one
*/
const char g_VertexProgram_TexturedMesh[] =
    "precision mediump float;"
    "attribute vec3 csr_aVertex;"
    "attribute vec4 csr_aColor;"
    "attribute vec2 csr_aTexCoord;"
    "uniform   mat4 csr_uProjection;"
    "uniform   mat4 csr_uView;"
    "uniform   mat4 csr_uModel;"
    "varying   vec4 csr_vColor;"
    "varying   vec2 csr_vTexCoord;"
    "void main(void)"
    "{"
    "    csr_vColor    = csr_aColor;"
    "    csr_vTexCoord = csr_aTexCoord;"
    "    gl_Position   = csr_uProjection * csr_uView * csr_uModel * vec4(csr_aVertex, 1.0);"
    "}";
//----------------------------------------------------------------------------
/**
* Fragment shader program to show a textured mesh
*/
const char g_FragmentProgram_TexturedMesh[] =
    "precision mediump float;"
    "uniform sampler2D csr_sTexture;"
    "varying lowp vec4 csr_vColor;"
    "varying      vec2 csr_vTexCoord;"
    "void main(void)"
    "{"
    "    gl_FragColor = csr_vColor * texture2D(csr_sTexture, csr_vTexCoord);"
    "}";
//---------------------------------------------------------------------------
// TMainForm::ITreeStats
//---------------------------------------------------------------------------
/*REM
TMainForm::ITreeStats::ITreeStats() :
    m_PolyToCheckCount(0),
    m_MaxPolyToCheckCount(0),
    m_HitBoxCount(0),
    m_HitPolygonCount(0),
    m_FPS(0)
{}
//---------------------------------------------------------------------------
TMainForm::ITreeStats::~ITreeStats()
{}
//---------------------------------------------------------------------------
void TMainForm::ITreeStats::Clear()
{
    m_PolyToCheckCount = 0;
    m_HitBoxCount      = 0;
    m_HitPolygonCount  = 0;
}
*/
//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_hDC(NULL),
    m_hRC(NULL),
    m_pShader(NULL),
    m_pModel(NULL),
    m_pTree(NULL),
    m_pMSAA(NULL),
    m_Angle(0.0f),
    m_PosVelocity(0.0f),
    m_DirVelocity(0.0f),
    m_PreviousTime(0),
    m_Initialized(false),
    m_fViewWndProc_Backup(NULL)
{
    // build the model dir from the application exe
    UnicodeString sceneDir = ::ExtractFilePath(Application->ExeName);
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExtractFilePath(::ExcludeTrailingPathDelimiter(sceneDir));
                  sceneDir = ::ExcludeTrailingPathDelimiter(sceneDir) + L"\\Scenes";
                m_SceneDir =   AnsiString(sceneDir).c_str();

    // enable OpenGL
    EnableOpenGL(paView->Handle, &m_hDC, &m_hRC);

    // stop GLEW crashing on OSX :-/
    glewExperimental = GL_TRUE;

    // initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        // shutdown OpenGL
        DisableOpenGL(Handle, m_hDC, m_hRC);

        // close the app
        Application->Terminate();
    }
}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm()
{
    // restore the normal view procedure
    if (m_fViewWndProc_Backup)
        paView->WindowProc = m_fViewWndProc_Backup;

    DeleteScene();
    DisableOpenGL(paView->Handle, m_hDC, m_hRC);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject* pSender)
{
    // hook the panel procedure
    m_fViewWndProc_Backup = paView->WindowProc;
    paView->WindowProc    = ViewWndProc;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormShow(TObject* pSender)
{
    // initialize the scene
    InitScene(paView->ClientWidth, paView->ClientHeight);

    // initialize the timer
    //REM m_StartTime    = ::GetTickCount();
    m_PreviousTime = ::GetTickCount();

    // listen the application idle
    Application->OnIdle = OnIdle;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject* pSender)
{
    // update the viewport
    CreateViewport(paView->ClientWidth, paView->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormMouseWheel(TObject* pSender, TShiftState shift, int wheelDelta,
        TPoint& mousePos, bool& handled)
{
    tbModelDistance->Position = tbModelDistance->Position + ((wheelDelta > 0) ? 1 : -1);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::spMainViewMoved(TObject* pSender)
{
    // update the viewport
    CreateViewport(paView->ClientWidth, paView->ClientHeight);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btLoadModelClick(TObject* pSender)
{
    /*REM
    // create the select model dialog box
    std::auto_ptr<TModelSelection> pModelSelection(new TModelSelection(this));

    // restore the last known selection
    if (m_LastSelectedModel >= 0)
    {
        pModelSelection->rgShapes->ItemIndex = m_LastSelectedModel;

        if (m_LastSelectedModel == 7 && !m_LastSelectedFile.empty())
            pModelSelection->SetModelFileName(m_LastSelectedFile);
    }

    // show the dialog box
    if (pModelSelection->ShowModal() != mrOk)
        return;

    // keep the current selection
    m_LastSelectedModel = pModelSelection->rgShapes->ItemIndex;
    m_LastSelectedFile  = pModelSelection->edMDLFilelName->Text.IsEmpty() ? L"" :
                          pModelSelection->edMDLFilelName->Text.c_str();

    // clear all the previous models and meshes
    ClearModelsAndMeshes();

    CSR_Material material;
    material.m_Color       = csrColorBGRToRGBA(Graphics::ColorToRGB(pModelSelection->paSelectedColor->Color));
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 0;
    vf.m_HasPerVertexColor = 0;

    // select the model to build
    switch (pModelSelection->rgShapes->ItemIndex)
    {
        // surface
        case 0:
        {
            // create the shape to show
            m_pMesh = csrShapeCreateSurface(1.0f, 1.0f, &vf, 0, &material, 0);

            // create the AABB tree from the mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
            {
                m_AABBTrees.push_back(pTree);

                // calculate the Y position from the bounding box
                m_PosY = -CalculateYPos(pTree, false);
            }

            // initialize the values
            m_AngleX = 0.0f;

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // box
        case 1:
        {
            // create the shape to show
            m_pMesh = csrShapeCreateBox(1.0f, 1.0f, 1.0f, 0, &vf, 0, &material, 0);

            // create the AABB tree from the mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
            {
                m_AABBTrees.push_back(pTree);

                // calculate the Y position from the bounding box
                m_PosY = -CalculateYPos(pTree, false);
            }

            // initialize the values
            m_AngleX = 0.0f;

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // sphere
        case 2:
        {
            // create the shape to show
            m_pMesh = csrShapeCreateSphere(0.5f,
                                         ::StrToInt(pModelSelection->edSlices->Text),
                                         ::StrToInt(pModelSelection->edStacks->Text),
                                          &vf,
                                           0,
                                          &material,
                                           0);

            // create the AABB tree from the mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
            {
                m_AABBTrees.push_back(pTree);

                // calculate the Y position from the bounding box
                m_PosY = -CalculateYPos(pTree, true);
            }

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // cylinder
        case 3:
        {
            CSR_VertexCulling vc;
            vc.m_Type = CSR_CT_None;
            vc.m_Face = CSR_CF_CW;

            // create the shape to show
            m_pMesh = csrShapeCreateCylinder(0.5f,
                                             1.0f,
                                           ::StrToInt(pModelSelection->edFaces->Text),
                                            &vf,
                                            &vc,
                                            &material,
                                             0);

            // create the AABB tree from the mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
            {
                m_AABBTrees.push_back(pTree);

                // calculate the Y position from the bounding box
                m_PosY = -CalculateYPos(pTree, true);
            }

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // disk
        case 4:
        {
            // create the shape to show
            m_pMesh = csrShapeCreateDisk(0.0f,
                                         0.0f,
                                         0.5f,
                                       ::StrToInt(pModelSelection->edSlices->Text),
                                        &vf,
                                         0,
                                        &material,
                                         0);

            // create the AABB tree from the mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
            {
                m_AABBTrees.push_back(pTree);

                // calculate the Y position from the bounding box
                m_PosY = -CalculateYPos(pTree, false);
            }

            // initialize the values
            m_AngleX = 0.0f;

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // ring
        case 5:
        {
            // create the shape to show
            m_pMesh = csrShapeCreateRing(0.0f,
                                         0.0f,
                                         0.25f,
                                         0.5f,
                                       ::StrToInt(pModelSelection->edSlices->Text),
                                        &vf,
                                         0,
                                        &material,
                                         0);

            // create the AABB tree from the mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
            {
                m_AABBTrees.push_back(pTree);

                // calculate the Y position from the bounding box
                m_PosY = -CalculateYPos(pTree, false);
            }

            // initialize the values
            m_AngleX = 0.0f;

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // spiral
        case 6:
        {
            // create the shape to show
            m_pMesh = csrShapeCreateSpiral(0.0f,
                                           0.0f,
                                           0.25f,
                                           0.5f,
                                           0.0f,
                                           0.0f,
                                           0.1f,
                                         ::StrToInt(pModelSelection->edSlices->Text),
                                         ::StrToInt(pModelSelection->edStacks->Text),
                                          &vf,
                                           0,
                                          &material,
                                           0);

            // create the AABB tree from the mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
            {
                m_AABBTrees.push_back(pTree);

                // calculate the Y position from the bounding box
                m_PosY = -CalculateYPos(pTree, true);
            }

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // MDL model
        case 7:
        {
            // get model file name
            const std::wstring modelFileName = pModelSelection->GetModelFileName();

            // do load a MDL model?
            if (modelFileName.empty())
                return;

            CSR_MDL* pMDL = NULL;

            try
            {
                // configure the MDL model vertex format
                vf.m_HasNormal         = 0;
                vf.m_HasTexCoords      = 1;
                vf.m_HasPerVertexColor = 0;

                // load MDL model
                pMDL = csrMDLOpen(AnsiString(UnicodeString(modelFileName.c_str())).c_str(),
                                             0,
                                            &vf,
                                             0,
                                            &material,
                                             0,
                                             0);

                // succeeded?
                if (!pMDL)
                {
                    // build the error message to show
                    std::wostringstream sstr;
                    sstr << L"Failed to load the model:\r\n" << modelFileName;

                    // show the error message to the user
                    ::MessageDlg(sstr.str().c_str(), mtError, TMsgDlgButtons() << mbOK, 0);

                    // clear the scene
                    ClearModelsAndMeshes();
                    return;
                }

                // create the AABB trees for each frame
                for (std::size_t i = 0; i < pMDL->m_ModelCount; ++i)
                    for (std::size_t j = 0; j < pMDL->m_pModel->m_MeshCount; ++j)
                    {
                        // create the AABB tree for the next frame
                        CSR_AABBNode* pTree = csrAABBTreeFromMesh(&pMDL->m_pModel[i].m_pMesh[j]);

                        if (!pTree)
                        {
                            // build the error message to show
                            std::wostringstream sstr;
                            sstr << L"Failed to load the model:\r\n\r\n"
                                 << modelFileName << L"\r\n\r\n"
                                 << L"An error occurred while the AABB trees vere created."
                                 << L"Model index = " << i << L", mesh index = " << j;

                            // show the error message to the user
                            ::MessageDlg(sstr.str().c_str(), mtError, TMsgDlgButtons() << mbOK, 0);

                            // clear the scene
                            ClearModelsAndMeshes();
                            return;
                        }

                        // calculate the y position (based on the 1st model frame)
                        if (!i && !j)
                            m_PosY = -CalculateYPos(pTree, true);

                        m_AABBTrees.push_back(pTree);
                    }

                // get the animation count
                const int animCount = pMDL->m_AnimationCount ? pMDL->m_AnimationCount - 1 : 0;

                // update the interface
                tbModelDistance->Position  = 100;
                tbAnimationNb->Max         = animCount;
                tbAnimationNb->Enabled     = animCount;
                tbAnimationSpeed->Position = 10;
                tbAnimationSpeed->Enabled  = tbAnimationNb->Enabled;

                // keep the model. NOTE don't forget to set his local value to 0, otherwise the model
                // will be deleted while the csrModelRelease() function will be exectued
                m_pMDL = pMDL;
                pMDL   = 0;
            }
            __finally
            {
                csrMDLRelease(pMDL);
            }

            return;
        }
    }
    */
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::aeEventsMessage(tagMSG& msg, bool& handled)
{
    m_PosVelocity = 0.0f;
    m_DirVelocity = 0.0f;

    switch (msg.message)
    {
        case WM_KEYDOWN:
            switch (msg.wParam)
            {
                case VK_LEFT:  m_DirVelocity = -1.0f; break;
                case VK_RIGHT: m_DirVelocity =  1.0f; break;
                case VK_UP:    m_PosVelocity =  1.0f; break;
                case VK_DOWN:  m_PosVelocity = -1.0f; break;
            }

            return;
    }
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ViewWndProc(TMessage& message)
{
    switch (message.Msg)
    {
        case WM_PAINT:
        {
            // is scene initialized?
            if (!m_Initialized)
                break;

            HDC           hDC = NULL;
            ::PAINTSTRUCT ps;

            try
            {
                // get and lock the view device context
                hDC = ::BeginPaint(paView->Handle, &ps);

                // on success, draw the scene
                if (hDC)
                    OnDrawScene(true);
            }
            __finally
            {
                // unlock and release the device context
                ::EndPaint(paView->Handle, &ps);
            }

            return;
        }
    }

    if (m_fViewWndProc_Backup)
        m_fViewWndProc_Backup(message);
}
//---------------------------------------------------------------------------
void TMainForm::EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    // get the device context
    *hDC = ::GetDC(hWnd);

    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize      = sizeof(pfd);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.iLayerType = PFD_MAIN_PLANE;

    // set the pixel format for the device context
    iFormat = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, iFormat, &pfd);

    // create and enable the OpenGL render context
    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);
}
//------------------------------------------------------------------------------
void TMainForm::DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
//------------------------------------------------------------------------------
void TMainForm::CreateViewport(float w, float h)
{
    if (!m_pShader)
        return;

    // calculate matrix items
    const float zNear  = 0.001f;
    const float zFar   = 1000.0f;
    const float fov    = 45.0f;
    const float aspect = w / h;

    // create the OpenGL viewport
    glViewport(0, 0, w, h);

    // create a perspective projection matrix
    csrMat4Perspective(fov, aspect, zNear, zFar, &m_ProjectionMatrix);

    // enable the shader program
    csrShaderEnable(m_pShader);

    // connect projection matrix to shader
    GLint projectionSlot = glGetUniformLocation(m_pShader->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(projectionSlot, 1, 0, &m_ProjectionMatrix.m_Table[0][0]);

    // create a default view matrix (set implicitly to identity in the shader but used later for
    // unprojection in the CalculateMouseRay() function)
    csrMat4Identity(&m_ViewMatrix);

    // multisampling antialiasing was already created?
    if (!m_pMSAA)
        // create the multisampling antialiasing
        m_pMSAA = csrMSAACreate(w, h, 4);
    else
        // change his size
        csrMSAAChangeSize(w, h, m_pMSAA);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // set the scene background color
    m_Background.m_R = 0.45f;
    m_Background.m_G = 0.8f;
    m_Background.m_B = 1.0f;
    m_Background.m_A = 1.0f;

    // set the viewpoint bounding sphere default position
    m_BoundingSphere.m_Center.m_X = 0.0f;
    m_BoundingSphere.m_Center.m_Y = 0.0f;
    m_BoundingSphere.m_Center.m_Z = 0.0f;
    m_BoundingSphere.m_Radius     = 0.1f;

    // load the shader
    m_pShader  = csrShaderLoadFromStr(g_VertexProgram_TexturedMesh,
                                      sizeof(g_VertexProgram_TexturedMesh),
                                      g_FragmentProgram_TexturedMesh,
                                      sizeof(g_FragmentProgram_TexturedMesh),
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
    m_pShader->m_VertexSlot   = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aVertex");
    m_pShader->m_ColorSlot    = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aColor");
    m_pShader->m_TexCoordSlot = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aTexCoord");
    m_pShader->m_TextureSlot  = glGetUniformLocation(m_pShader->m_ProgramID, "csr_sTexture");

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    /*
    const std::string modelFile = m_SceneDir + "\\mountain.obj";

    // load a default model
    m_pModel = csrWaveFrontOpen(modelFile.c_str(), &vf, 0, &material, 0, 0);

    // succeeded?
    if (!m_pModel || !m_pModel->m_MeshCount)
    {
        // show the error message to the user
        ::MessageDlg(L"Failed to load the default mountain scene.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }
    */
    CSR_PixelBuffer* pMap = csrPixelBufferFromBitmap((m_SceneDir + "\\Map\\the_face.bmp").c_str());

    m_pModel              = csrModelCreate();
    m_pModel->m_pMesh     = csrLandscapeCreate(pMap, 3.0f, 0.2f, &vf, 0, &material, 0);
    m_pModel->m_MeshCount = 1;

    csrPixelBufferRelease(pMap);

    // create the AABB tree for the mountain model
    m_pTree = csrAABBTreeFromMesh(&m_pModel->m_pMesh[0]);

    // succeeded?
    if (!m_pTree)
    {
        // show the error message to the user
        ::MessageDlg(L"Could not create the aligned-axis bounding box tree for the scene.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    UnicodeString test = UnicodeString(AnsiString(m_SceneDir.c_str())) + L"\\Mountain\\mountain.jpg";

    std::auto_ptr<TPicture> pPicture(new TPicture());
    pPicture->LoadFromFile(test);

    std::auto_ptr<TBitmap> pTexture(new TBitmap());
    pTexture->Assign(pPicture->Graphic);

    int   pixelSize;
    GLint pixelType;

    // search for bitmap pixel format
    switch (pTexture->PixelFormat)
    {
        case pf24bit: pixelSize = 3; pixelType = GL_RGB;  break;
        case pf32bit: pixelSize = 4; pixelType = GL_RGBA; break;
        default:
        {
            // show the error message to the user
            ::MessageDlg(L"Unknown texture format.\r\n\r\nThe application will quit.",
                         mtError,
                         TMsgDlgButtons() << mbOK,
                         0);

            Application->Terminate();
            return;
        }
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

        // set the texture
        m_pModel->m_pMesh[0].m_Shader.m_TextureID = csrTextureFromPixelBuffer(pPixelBuffer);
    }
    __finally
    {
        csrPixelBufferRelease(pPixelBuffer);
    }

    /*REM
    // set the initial values
    m_pTextureLastTime = 0.0;
    m_pModelLastTime   = 0.0;
    m_pMeshLastTime    = 0.0;
    m_TextureIndex     = 0;
    m_ModelIndex       = 0;
    m_MeshIndex        = 0;
    */

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    // release the aligned axis bounding box tree
    csrAABBTreeNodeRelease(m_pTree);

    // release the scene
    csrModelRelease(m_pModel);

    // release the shader
    csrShaderRelease(m_pShader);

    // release the multisampling antialiasing
    csrMSAARelease(m_pMSAA);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    // no time elapsed?
    if (!elapsedTime)
        return;

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
        CSR_Vector3 newPos = m_BoundingSphere.m_Center;

        // calculate the next player position
        newPos.m_X += m_PosVelocity * sinf(m_Angle) * elapsedTime;
        newPos.m_Z -= m_PosVelocity * cosf(m_Angle) * elapsedTime;

        // validate and apply it
        m_BoundingSphere.m_Center = newPos;

        /*
        // calculate next time where the step sound should be played
        m_StepTime += (elapsedTime * 1000.0f);

        // do play the sound?
        if (m_StepTime > m_StepInterval)
        {
            miniStopSound(m_PlayerStepSoundID);
            miniPlaySound(m_PlayerStepSoundID);
            m_StepTime = 0.0f;
        }
        */
    }

    // enable the shader program
    csrShaderEnable(m_pShader);

    ApplyGroundCollision(&m_BoundingSphere, m_pTree, &m_ViewMatrix);

    // connect the view matrix to shader
    const GLint viewSlot = glGetUniformLocation(m_pShader->m_ProgramID, "csr_uView");
    glUniformMatrix4fv(viewSlot, 1, 0, &m_ViewMatrix.m_Table[0][0]);

    csrMat4Identity(&m_ModelMatrix);

    // connect the model view matrix to shader
    GLint modelSlot = glGetUniformLocation(m_pShader->m_ProgramID, "csr_uModel");
    glUniformMatrix4fv(modelSlot, 1, 0, &m_ModelMatrix.m_Table[0][0]);
}
//---------------------------------------------------------------------------
void TMainForm::ApplyGroundCollision(const CSR_Sphere*   pBoundingSphere,
                                     const CSR_AABBNode* pTree,
                                           CSR_Matrix4*  pMatrix) const
{
    // validate the inputs
    if (!pBoundingSphere || !pTree || !pMatrix)
        return;

    CSR_Sphere transformedSphere = *pBoundingSphere;
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

    CSR_Vector3 groundDir;

    // get the ground direction
    groundDir.m_X =  0.0f;
    groundDir.m_Y = -1.0f;
    groundDir.m_Z =  0.0f;

    CSR_Vector3 modelCenter;

    // get the model center
    modelCenter.m_X = 0.0f;
    modelCenter.m_Y = 0.0f;
    modelCenter.m_Z = 0.0f;

    CSR_Matrix4 invertView;
    float       determinant;

    // calculate the current camera position above the landscape
    csrMat4Inverse(pMatrix, &invertView, &determinant);
    csrMat4Transform(&invertView, &modelCenter, &transformedSphere.m_Center);

    float posY = -transformedSphere.m_Center.m_Y;

    // calculate the y position where to place the point of view
    csrGroundPosY(&transformedSphere, pTree, &groundDir, &posY);

    // update the ground position inside the view matrix
    pMatrix->m_Table[3][1] = -posY;
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    try
    {
        // begin the drawing
        csrMSAADrawBegin(&m_Background, ckAntialiasing->Checked ? m_pMSAA : NULL);

        if (m_pModel)
            // draw the model
            csrDrawModel(m_pModel, 0, m_pShader, 0);
    }
    __finally
    {
        // end the drawing
        csrMSAADrawEnd(ckAntialiasing->Checked ? m_pMSAA : NULL);
    }
}
//---------------------------------------------------------------------------
/*
void TMainForm::ShowStats() const
{
    unsigned    vertexCount = 0;
    unsigned    stride;
    std::size_t polyCount;

    if (m_pMesh)
    {
        // get the mesh stride
        stride = m_pMesh->m_pVB ? m_pMesh->m_pVB->m_Format.m_Stride : 0;

        // count all vertices contained in the mesh
        for (std::size_t i = 0; i < m_pMesh->m_Count; ++i)
            vertexCount += m_pMesh->m_pVB[i].m_Count;

        // calculate the polygons count
        if (!m_pMesh->m_pVB)
            polyCount = 0;
        else
            switch (m_pMesh->m_pVB->m_Format.m_Type)
            {
                case CSR_VT_Triangles:     polyCount = stride ? ((vertexCount / stride) /  3)                     : 0; break;
                case CSR_VT_TriangleStrip:
                case CSR_VT_TriangleFan:   polyCount = stride ? ((vertexCount / stride) - (2 * m_pMesh->m_Count)) : 0; break;
                default:                   polyCount = 0;                                                              break;
            }
    }
    else
    if (m_pMDL)
    {
        // get the current model mesh to draw
        const CSR_Mesh* pMesh = csrMDLGetMesh(m_pMDL, m_ModelIndex, m_MeshIndex);

        // found it?
        if (!pMesh)
            return;

        // get the mesh stride
        stride = pMesh->m_pVB ? pMesh->m_pVB->m_Format.m_Stride : 0;

        // count all vertices contained in the mesh
        for (std::size_t i = 0; i < pMesh->m_Count; ++i)
            vertexCount += pMesh->m_pVB[i].m_Count;

        // calculate the polygons count
        polyCount = stride ? ((vertexCount / stride) / 3) : 0;
    }
    else
        return;

    // show the stats
    laPolygonCount->Caption    = L"Polygons Count: "        + ::IntToStr(int(polyCount));
    laPolygonsToCheck->Caption = L"Polygons To Check: "     + ::IntToStr(int(m_Stats.m_PolyToCheckCount));
    laMaxPolyToCheck->Caption  = L"Max Polygons To Check: " + ::IntToStr(int(m_Stats.m_MaxPolyToCheckCount));
    laHitPolygons->Caption     = L"Hit Polygons: "          + ::IntToStr(int(m_Stats.m_HitPolygonCount));
    laHitBoxes->Caption        = L"Hit Boxes: "             + ::IntToStr(int(m_Stats.m_HitBoxCount));
    laFPS->Caption             = L"FPS:"                    + ::IntToStr(int(m_Stats.m_FPS));
}
*/
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

    /*REM
    // clear the tree stats
    m_Stats.Clear();

    ++m_FrameCount;

    // calculate the FPS
    if (m_FrameCount >= 100)
    {
        const double      smoothing = 0.1;
        const std::size_t fpsTime   = now > m_StartTime ? now - m_StartTime : 1;
        const std::size_t newFPS    = (m_FrameCount * 100) / fpsTime;
        m_StartTime                 = ::GetTickCount();
        m_FrameCount                = 0;
        m_Stats.m_FPS               = (newFPS * smoothing) + (m_Stats.m_FPS * (1.0 - smoothing));
    }
    */

    // update and draw the scene
    UpdateScene(elapsedTime);
    DrawScene();

    ::SwapBuffers(m_hDC);

    // show the stats
    //REM ShowStats();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;
    OnDrawScene(false);
}
//---------------------------------------------------------------------------
bool TMainForm::SetTexture(      int            width,
                                 int            height,
                                 int            pixelType,
                                 bool           bumpMap,
                           const unsigned char* pPixels) const
{
    // create new OpenGL texture
    if (m_pModel)
    {
        glGenTextures(1, &m_pModel->m_pMesh[0].m_Shader.m_TextureID);
        glBindTexture(GL_TEXTURE_2D, m_pModel->m_pMesh[0].m_Shader.m_TextureID);
    }

    // set texture filtering
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // set texture wrapping mode
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // generate texture from pixel array
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 pixelType,
                 width,
                 height,
                 0,
                 pixelType,
                 GL_UNSIGNED_BYTE,
                 pPixels);

    return true;
}
//---------------------------------------------------------------------------
