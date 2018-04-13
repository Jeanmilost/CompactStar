/****************************************************************************
 * ==> AABB Tree tool main form --------------------------------------------*
 ****************************************************************************
 * Description : Aligned-Axis Bounding Box tool main form                   *
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

// std
#include <memory>
#include <sstream>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Sound.h"

// interface
#include "TModelSelection.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif
#pragma link "OpenAL32E.lib"
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
    "attribute vec3 csr_vVertex;"
    "attribute vec4 csr_vColor;"
    "attribute vec2 csr_vTexCoord;"
    "uniform   mat4 csr_uProjection;"
    "uniform   mat4 csr_uModelView;"
    "varying   vec4 csr_fColor;"
    "varying   vec2 csr_fTexCoord;"
    "void main(void)"
    "{"
    "    csr_fColor    = csr_vColor;"
    "    csr_fTexCoord = csr_vTexCoord;"
    "    gl_Position   = csr_uProjection * csr_uModelView * vec4(csr_vVertex, 1.0);"
    "}";
//----------------------------------------------------------------------------
/**
* Fragment shader program to show a textured mesh
*/
const char g_FragmentProgram_TexturedMesh[] =
    "precision mediump float;"
    "uniform sampler2D csr_sTexture;"
    "varying lowp vec4 csr_fColor;"
    "varying      vec2 csr_fTexCoord;"
    "void main(void)"
    "{"
    "    gl_FragColor = csr_fColor * texture2D(csr_sTexture, csr_fTexCoord);"
    "}";
//---------------------------------------------------------------------------
// TMainForm::ITreeStats
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
TMainForm* MainForm;
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    m_hDC(NULL),
    m_hRC(NULL),
    m_pShader_ColoredMesh(NULL),
    m_pShader_TexturedMesh(NULL),
    m_pMSAA(NULL),
    m_pBoxMesh(NULL),
    m_pMesh(NULL),
    m_pMDL(NULL),
    m_LastSelectedModel(-1),
    m_PosY(0.0f),
    m_AngleX(-M_PI / 2.0f),
    m_AngleY(0.0f),
    m_pTextureLastTime(0.0),
    m_pModelLastTime(0.0),
    m_pMeshLastTime(0.0),
    m_TextureIndex(0),
    m_ModelIndex(0),
    m_MeshIndex(0),
    m_FrameCount(0),
    m_StartTime(0),
    m_PreviousTime(0),
    m_Initialized(false),
    m_fViewWndProc_Backup(NULL)
{
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
    m_StartTime    = ::GetTickCount();
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
void TMainForm::ClearModelsAndMeshes()
{
    // release the AABB trees
    for (std::size_t i = 0; i < m_AABBTrees.size(); ++i)
        csrAABBTreeNodeRelease(m_AABBTrees[i]);

    m_AABBTrees.clear();

    // release the scene objects
    csrMDLRelease(m_pMDL);
    csrMeshRelease(m_pMesh);

    // reset the values
    m_pMDL             =  0;
    m_pMesh            =  0;
    m_PosY             =  0.0f;
    m_AngleX           = -M_PI / 2.0f;
    m_AngleY           =  0.0f;
    m_pTextureLastTime =  0.0;
    m_pModelLastTime   =  0.0;
    m_pMeshLastTime    =  0.0;
    m_TextureIndex     =  0;
    m_ModelIndex       =  0;
    m_MeshIndex        =  0;

    // reset the stats
    m_Stats.Clear();
    m_Stats.m_MaxPolyToCheckCount = 0;
    m_Stats.m_FPS                 = 0;
}
//------------------------------------------------------------------------------
void TMainForm::CreateViewport(float w, float h)
{
    if (!m_pShader_ColoredMesh)
        return;

    // calculate matrix items
    const float zNear  = 1.0f;
    const float zFar   = 1000.0f;
    const float fov    = 45.0f;
    const float aspect = w / h;

    // create the OpenGL viewport
    glViewport(0, 0, w, h);

    // create a perspective projection matrix
    csrMat4Perspective(fov, aspect, zNear, zFar, &m_ProjectionMatrix);

    // enable the colored program
    csrShaderEnable(m_pShader_ColoredMesh);

    // connect projection matrix to colored shader
    GLint projectionSlot = glGetUniformLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(projectionSlot, 1, 0, &m_ProjectionMatrix.m_Table[0][0]);

    // enable the textured program
    csrShaderEnable(m_pShader_TexturedMesh);

    // connect projection matrix to textured shader
    projectionSlot = glGetUniformLocation(m_pShader_TexturedMesh->m_ProgramID, "csr_uProjection");
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
    m_Background.m_R = 0.0f;
    m_Background.m_G = 0.0f;
    m_Background.m_B = 0.0f;
    m_Background.m_A = 1.0f;

    // load the shaders
    m_pShader_ColoredMesh  = csrShaderLoadFromStr(g_VertexProgram_ColoredMesh,
                                                  sizeof(g_VertexProgram_ColoredMesh),
                                                  g_FragmentProgram_ColoredMesh,
                                                  sizeof(g_FragmentProgram_ColoredMesh),
                                                  0,
                                                  0);
    m_pShader_TexturedMesh = csrShaderLoadFromStr(g_VertexProgram_TexturedMesh,
                                                  sizeof(g_VertexProgram_TexturedMesh),
                                                  g_FragmentProgram_TexturedMesh,
                                                  sizeof(g_FragmentProgram_TexturedMesh),
                                                  0,
                                                  0);

    // succeeded?
    if (!m_pShader_ColoredMesh || !m_pShader_TexturedMesh)
    {
        // show the error message to the user
        ::MessageDlg(L"Failed to load the shaders.\r\n\r\nThe application will quit.",
                     mtError,
                     TMsgDlgButtons() << mbOK,
                     0);

        Application->Terminate();
        return;
    }

    // enable the colored program
    csrShaderEnable(m_pShader_ColoredMesh);

    // get shader attributes
    m_pShader_ColoredMesh->m_VertexSlot = glGetAttribLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_vVertex");
    m_pShader_ColoredMesh->m_ColorSlot  = glGetAttribLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_vColor");

    // enable the textured program
    csrShaderEnable(m_pShader_TexturedMesh);

    // get shader attributes
    m_pShader_TexturedMesh->m_VertexSlot   = glGetAttribLocation(m_pShader_TexturedMesh->m_ProgramID,  "csr_vVertex");
    m_pShader_TexturedMesh->m_ColorSlot    = glGetAttribLocation(m_pShader_TexturedMesh->m_ProgramID,  "csr_vColor");
    m_pShader_TexturedMesh->m_TexCoordSlot = glGetAttribLocation(m_pShader_TexturedMesh->m_ProgramID,  "csr_vTexCoord");
    m_pShader_TexturedMesh->m_TextureSlot  = glGetUniformLocation(m_pShader_TexturedMesh->m_ProgramID, "csr_sTexture");

    CSR_Material material;
    material.m_Color       = 0xFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 0;
    vf.m_HasPerVertexColor = 0;

    // create a default sphere
    m_pMesh = csrShapeCreateSphere(0.5f, 10, 10, &vf, 0, &material, 0);

    // create the AABB tree for the sphere mesh
    CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

    // succeeded?
    if (pTree)
        m_AABBTrees.push_back(pTree);

    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 0;
    vf.m_HasPerVertexColor = 0;

    // create a generic box
    m_pBoxMesh = csrShapeCreateBox(1.0f, 1.0f, 1.0f, 0, &vf, 0, 0, 0);

    // set the initial values
    m_pTextureLastTime = 0.0;
    m_pModelLastTime   = 0.0;
    m_pMeshLastTime    = 0.0;
    m_TextureIndex     = 0;
    m_ModelIndex       = 0;
    m_MeshIndex        = 0;

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    // clear all the models and meshes composing the scene
    ClearModelsAndMeshes();

    // release the box model
    csrMeshRelease(m_pBoxMesh);

    // release the shaders
    csrShaderRelease(m_pShader_TexturedMesh);
    csrShaderRelease(m_pShader_ColoredMesh);

    // release the multisampling antialiasing
    csrMSAARelease(m_pMSAA);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    CSR_Vector3 t;
    CSR_Vector3 r;
    CSR_Matrix4 translateMatrix;
    CSR_Matrix4 xRotateMatrix;
    CSR_Matrix4 yRotateMatrix;
    CSR_Matrix4 rotateMatrix;

    // calculate the next angle
    m_AngleY += (elapsedTime * (float(tbRotationSpeed->Position) * 0.01f));

    // avoid the angle to exceeds the bounds even if a huge time has elapsed since last update
    while (m_AngleY > M_PI * 2.0f)
        m_AngleY -= M_PI * 2.0f;

    // calculate the next indexes to use for the MDL model
    if (!ckPauseModelAnimation->Checked && m_pMDL)
        csrMDLUpdateIndex(m_pMDL,
                          tbAnimationSpeed->Position,
                          tbAnimationNb->Position,
                         &m_TextureIndex,
                         &m_ModelIndex,
                         &m_MeshIndex,
                         &m_pTextureLastTime,
                         &m_pModelLastTime,
                         &m_pMeshLastTime,
                          elapsedTime);

    // set translation
    t.m_X =  0.0f;
    t.m_Y =  m_PosY;
    t.m_Z = -float(tbModelDistance->Position);

    // build the translation matrix
    csrMat4Translate(&t, &translateMatrix);

    // set rotation on X axis
    r.m_X = 1.0f;
    r.m_Y = 0.0f;
    r.m_Z = 0.0f;

    // build the X rotation matrix
    csrMat4Rotate(m_AngleX, &r, &xRotateMatrix);

    // set rotation on Y axis
    r.m_X = 0.0f;
    r.m_Y = 1.0f;
    r.m_Z = 0.0f;

    // build the Y rotation matrix
    csrMat4Rotate(m_AngleY, &r, &yRotateMatrix);

    // build the model view matrix
    csrMat4Multiply(&xRotateMatrix, &yRotateMatrix,   &rotateMatrix);
    csrMat4Multiply(&rotateMatrix,  &translateMatrix, &m_ModelMatrix);

    GLint shaderSlot;

    // enable the colored program
    csrShaderEnable(m_pShader_ColoredMesh);

    // connect the model view matrix to shader
    shaderSlot = glGetUniformLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_uModelView");
    glUniformMatrix4fv(shaderSlot, 1, 0, &m_ModelMatrix.m_Table[0][0]);

    // enable the textured program
    csrShaderEnable(m_pShader_TexturedMesh);

    // connect the model view matrix to shader
    shaderSlot = glGetUniformLocation(m_pShader_TexturedMesh->m_ProgramID, "csr_uModelView");
    glUniformMatrix4fv(shaderSlot, 1, 0, &m_ModelMatrix.m_Table[0][0]);

    // calculate the ray from current mouse position
    CalculateMouseRay();
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    try
    {
        // begin the drawing
        csrMSAADrawBegin(&m_Background, ckAntialiasing->Checked ? m_pMSAA : NULL);

        if (m_pMesh)
            // draw the mesh
            csrDrawMesh(m_pMesh, m_pShader_ColoredMesh, 0);
        else
        if (m_pMDL)
            // draw the MDL model
            csrDrawMDL(m_pMDL,
                       m_pShader_TexturedMesh,
                       0,
                       m_TextureIndex,
                       m_ModelIndex,
                       m_MeshIndex);
        else
            return;

        // show the polygon intersections (with the mouse ray)
        if (ckShowCollidingPolygons->Checked)
            ResolveTreeAndDrawPolygons();

        // do show the AABB boxes?
        if (ckShowBoxes->Checked)
        {
            // get the AABB tree index
            const std::size_t treeIndex = GetAABBTreeIndex();

            // is index out of bounds?
            if (treeIndex < m_AABBTrees.size())
                // draw the AABB tree boxes
                DrawTreeBoxes(m_AABBTrees[treeIndex]);
        }
    }
    __finally
    {
        // end the drawing
        csrMSAADrawEnd(ckAntialiasing->Checked ? m_pMSAA : NULL);
    }
}
//---------------------------------------------------------------------------
void TMainForm::ResolveTreeAndDrawPolygons()
{
    // get the AABB tree index
    const std::size_t treeIndex = GetAABBTreeIndex();

    // is index out of bounds?
    if (treeIndex >= m_AABBTrees.size())
        return;

    // is the mouse ray not initialized?
    if (!m_Ray.m_Pos.m_X    && !m_Ray.m_Pos.m_Y    && !m_Ray.m_Pos.m_Z &&
        !m_Ray.m_Dir.m_X    && !m_Ray.m_Dir.m_Y    && !m_Ray.m_Dir.m_Z &&
        !m_Ray.m_InvDir.m_X && !m_Ray.m_InvDir.m_Y && !m_Ray.m_InvDir.m_Z)
        return;

    CSR_Polygon3Buffer polygonBuffer;

    // using the mouse ray, resolve aligned-axis bounding box tree
    csrAABBTreeResolve(&m_Ray, m_AABBTrees[treeIndex], 0, &polygonBuffer);

    // update the stats
    m_Stats.m_PolyToCheckCount    = polygonBuffer.m_Count;
    m_Stats.m_MaxPolyToCheckCount = std::max(polygonBuffer.m_Count, m_Stats.m_MaxPolyToCheckCount);

    CSR_Polygon3* pPolygonsToDraw     = NULL;
    CSR_Polygon3* pNewPolygonsToDraw  = NULL;
    std::size_t   polygonsToDrawCount = 0;

    // create a figure for the ray
    CSR_Figure3 ray;
    ray.m_Type    = CSR_F3_Ray;
    ray.m_pFigure = &m_Ray;

    // iterate through polygons to check
    for (std::size_t i = 0; i < polygonBuffer.m_Count; ++i)
    {
        CSR_Figure3 polygon;
        polygon.m_Type    = CSR_F3_Polygon;
        polygon.m_pFigure = (CSR_Polygon3*)&polygonBuffer.m_pPolygon[i];

        // is polygon intersecting ray?
        if (csrIntersect3(&ray, &polygon, 0, 0, 0))
        {
            // reserve memory fort he new polygon to draw
            pNewPolygonsToDraw = (CSR_Polygon3*)csrMemoryAlloc(pPolygonsToDraw,
                                                               sizeof(CSR_Polygon3),
                                                               polygonsToDrawCount + 1);

            // succeeded?
            if (!pNewPolygonsToDraw)
                return;

            // copy his content
            pNewPolygonsToDraw[polygonsToDrawCount] = polygonBuffer.m_pPolygon[i];

            // update the polygon buffer to draw
            pPolygonsToDraw = pNewPolygonsToDraw;
            ++polygonsToDrawCount;
        }
    }

    // delete found polygons (no longer needed from now)
    if (polygonBuffer.m_Count)
        free(polygonBuffer.m_pPolygon);

    // found polygons to draw?
    if (polygonsToDrawCount)
    {
        CSR_Mesh mesh;

        // create and configure a mesh for the polygons
        mesh.m_Count                             =  1;
        mesh.m_Shader.m_TextureID                =  M_CSR_Error_Code;
        mesh.m_Shader.m_BumpMapID                =  M_CSR_Error_Code;
        mesh.m_pVB                               = (CSR_VertexBuffer*)csrMemoryAlloc(0, sizeof(CSR_VertexBuffer), 1);
        mesh.m_pVB->m_Format.m_Type              =  CSR_VT_Triangles;
        mesh.m_pVB->m_Format.m_HasNormal         =  0;
        mesh.m_pVB->m_Format.m_HasTexCoords      =  0;
        mesh.m_pVB->m_Format.m_HasPerVertexColor =  1;
        mesh.m_pVB->m_Format.m_Stride            =  7;
        mesh.m_pVB->m_Culling.m_Type             =  CSR_CT_None;
        mesh.m_pVB->m_Culling.m_Face             =  CSR_CF_CCW;
        mesh.m_pVB->m_pData                      =  m_PolygonArray;
        mesh.m_pVB->m_Count                      =  21;

        // iterate through polygons to draw
        for (std::size_t i = 0; i < polygonsToDrawCount; ++i)
        {
            // set vertex 1 in vertex buffer
            m_PolygonArray[0]  = pPolygonsToDraw[i].m_Vertex[0].m_X;
            m_PolygonArray[1]  = pPolygonsToDraw[i].m_Vertex[0].m_Y;
            m_PolygonArray[2]  = pPolygonsToDraw[i].m_Vertex[0].m_Z;
            m_PolygonArray[3]  = 1.0f;
            m_PolygonArray[4]  = 0.0f;
            m_PolygonArray[5]  = 0.0f;
            m_PolygonArray[6]  = 1.0f;

            // set vertex 2 in vertex buffer
            m_PolygonArray[7]  = pPolygonsToDraw[i].m_Vertex[1].m_X;
            m_PolygonArray[8]  = pPolygonsToDraw[i].m_Vertex[1].m_Y;
            m_PolygonArray[9]  = pPolygonsToDraw[i].m_Vertex[1].m_Z;
            m_PolygonArray[10] = 1.0f;
            m_PolygonArray[11] = 0.0f;
            m_PolygonArray[12] = 0.0f;
            m_PolygonArray[13] = 1.0f;

            // set vertex 3 in vertex buffer
            m_PolygonArray[14] = pPolygonsToDraw[i].m_Vertex[2].m_X;
            m_PolygonArray[15] = pPolygonsToDraw[i].m_Vertex[2].m_Y;
            m_PolygonArray[16] = pPolygonsToDraw[i].m_Vertex[2].m_Z;
            m_PolygonArray[17] = 1.0f;
            m_PolygonArray[18] = 0.0f;
            m_PolygonArray[19] = 0.0f;
            m_PolygonArray[20] = 1.0f;

            // draw the polygon
            csrDrawMesh(&mesh, m_pShader_ColoredMesh, 0);
        }

        free(mesh.m_pVB);
    }

    // delete the polygon list
    if (pPolygonsToDraw)
        free(pPolygonsToDraw);

    // update the stats
    m_Stats.m_HitPolygonCount = polygonsToDrawCount;
}
//---------------------------------------------------------------------------
void TMainForm::DrawTreeBoxes(const CSR_AABBNode* pTree)
{
    // no box model to show?
    if (!m_pBoxMesh)
        return;

    // node contains left children?
    if (pTree->m_pLeft)
        DrawTreeBoxes(pTree->m_pLeft);

    // node contains right children?
    if (pTree->m_pRight)
        DrawTreeBoxes(pTree->m_pRight);

    // do show boxes belonging to leaf only?
    if (ckShowLeafOnly->Checked && (pTree->m_pLeft || pTree->m_pRight))
        return;

    // build a geometrical figure for the ray
    CSR_Figure3 ray;
    ray.m_Type    = CSR_F3_Ray;
    ray.m_pFigure = (void*)&m_Ray;

    // build a geometrical figure for the box
    CSR_Figure3 box;
    box.m_Type    = CSR_F3_Box;
    box.m_pFigure = pTree->m_pBox;

    unsigned color;

    // is ray intersecting box?
    if (csrIntersect3(&ray, &box, 0, 0, 0))
    {
        // yes, count the hit and get the green color
        ++m_Stats.m_HitBoxCount;
        color = 0xFF0000;
    }
    else
    {
        // no, do nothing if only the colliding boxes should be drawn
        if (ckShowCollidingBoxesOnly->Checked)
            return;

        // otherwise get the red color
        color = 0xFF000000;
    }

    CSR_Vector3 t;
    CSR_Vector3 s;
    CSR_Matrix4 translateMatrix;
    CSR_Matrix4 scaleMatrix;
    CSR_Matrix4 buildMatrix;
    CSR_Matrix4 modelViewMatrix;

    // set translation
    t.m_X = (pTree->m_pBox->m_Max.m_X + pTree->m_pBox->m_Min.m_X) / 2.0f;
    t.m_Y = (pTree->m_pBox->m_Max.m_Y + pTree->m_pBox->m_Min.m_Y) / 2.0f;
    t.m_Z = (pTree->m_pBox->m_Max.m_Z + pTree->m_pBox->m_Min.m_Z) / 2.0f;

    // build the translation matrix
    csrMat4Translate(&t, &translateMatrix);

    // set scaling
    csrVec3Sub(&pTree->m_pBox->m_Max, &pTree->m_pBox->m_Min, &s);

    // build the scale matrix
    csrMat4Scale(&s, &scaleMatrix);

    // build the model view matrix
    csrMat4Multiply(&scaleMatrix, &translateMatrix, &buildMatrix);
    csrMat4Multiply(&buildMatrix, &m_ModelMatrix,   &modelViewMatrix);

    // enable the colored program
    csrShaderEnable(m_pShader_ColoredMesh);

    // connect the model view matrix to shader
    GLint shaderSlot = glGetUniformLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_uModelView");
    glUniformMatrix4fv(shaderSlot, 1, 0, &modelViewMatrix.m_Table[0][0]);

    // can draw the AABB box?
    if (m_pBoxMesh)
    {
        // update the box material
        for (std::size_t i = 0; i < m_pBoxMesh->m_Count; ++i)
        {
            m_pBoxMesh->m_pVB[i].m_Material.m_Color       = color | ((tbTransparency->Position * 0xFF) / tbTransparency->Max);
            m_pBoxMesh->m_pVB[i].m_Material.m_Transparent = tbTransparency->Position != tbTransparency->Max;
            m_pBoxMesh->m_pVB[i].m_Material.m_Wireframe   = ckWireFrame->Checked;
        }

        // draw the AABB box
        csrDrawMesh(m_pBoxMesh, m_pShader_ColoredMesh, 0);
    }
}
//---------------------------------------------------------------------------
CSR_Vector3 TMainForm::MousePosToViewportPos(const TPoint& mousePos, const CSR_Rect& viewRect)
{
    CSR_Vector3 result;

    // invalid client width or height?
    if (!paView->ClientWidth || !paView->ClientHeight)
    {
        result.m_X = 0.0f;
        result.m_Y = 0.0f;
        result.m_Z = 0.0f;
        return result;
    }

    // convert mouse position to viewport position
    result.m_X = viewRect.m_Min.m_X + ((mousePos.X * (viewRect.m_Max.m_X - viewRect.m_Min.m_X)) / paView->ClientWidth);
    result.m_Y = viewRect.m_Min.m_Y - ((mousePos.Y * (viewRect.m_Min.m_Y - viewRect.m_Max.m_Y)) / paView->ClientHeight);
    result.m_Z = 0.0f;
    return result;
}
//---------------------------------------------------------------------------
void TMainForm::CalculateMouseRay()
{
    // get the mouse position in screen coordinates
    TPoint mousePos = Mouse->CursorPos;

    // convert to view coordinates
    if (!::ScreenToClient(paView->Handle, &mousePos))
        return;

    CSR_Rect viewRect;

    // get the viewport rectangle
    viewRect.m_Min.m_X = -1.0f;
    viewRect.m_Min.m_Y =  1.0f;
    viewRect.m_Max.m_X =  1.0f;
    viewRect.m_Max.m_Y = -1.0f;

    // get the ray in the Windows coordinate
    m_Ray.m_Pos     =  MousePosToViewportPos(mousePos, viewRect);
    m_Ray.m_Dir.m_X =  m_Ray.m_Pos.m_X;
    m_Ray.m_Dir.m_Y =  m_Ray.m_Pos.m_Y;
    m_Ray.m_Dir.m_Z = -1.0f;

    // put the ray in the world coordinates
    csrMat4Unproject(&m_ProjectionMatrix, &m_ViewMatrix, &m_Ray);

    float determinant;

    CSR_Vector3 rayPos;
    CSR_Vector3 rayDir;
    CSR_Vector3 rayDirN;

    // now transform the ray to match with the model position
    CSR_Matrix4 invertModel;
    csrMat4Inverse(&m_ModelMatrix, &invertModel, &determinant);
    csrMat4ApplyToVector(&invertModel, &m_Ray.m_Pos, &rayPos);
    csrMat4ApplyToNormal(&invertModel, &m_Ray.m_Dir, &rayDir);
    csrVec3Normalize(&rayDir, &rayDirN);

    // get the transformed ray
    csrRay3FromPointDir(&rayPos, &rayDirN, &m_Ray);
}
//---------------------------------------------------------------------------
std::size_t TMainForm::GetAABBTreeIndex() const
{
    // select the index of the AABB tree to draw
    if (m_pMDL && m_pMDL->m_ModelCount == 1 && m_pMDL->m_pModel[m_ModelIndex].m_MeshCount > 1)
        return m_MeshIndex;

    return m_ModelIndex;
}
//---------------------------------------------------------------------------
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
//---------------------------------------------------------------------------
float TMainForm::CalculateYPos(const CSR_AABBNode* pTree, bool rotated) const
{
    // no tree or box?
    if (!pTree || !pTree->m_pBox)
        return 0.0f;

    // is model rotated?
    if (rotated)
        // calculate the y position from the z axis
        return (pTree->m_pBox->m_Max.m_Z + pTree->m_pBox->m_Min.m_Z) / 2.0f;

    // calculate the y position from the z axis
    return (pTree->m_pBox->m_Max.m_Y + pTree->m_pBox->m_Min.m_Y) / 2.0f;
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

    // update and draw the scene
    UpdateScene(elapsedTime);
    DrawScene();

    ::SwapBuffers(m_hDC);

    // show the stats
    ShowStats();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    done = false;
    OnDrawScene(false);
}
//---------------------------------------------------------------------------
