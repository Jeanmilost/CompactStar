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
#include "CSR_Renderer.h"
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
*/
const char* g_VertexProgram_ColoredMesh =
    "precision mediump float;"
    "attribute vec4 csr_vVertex;"
    "attribute vec4 csr_vColor;"
    "uniform   mat4 csr_uProjection;"
    "uniform   mat4 csr_uModelview;"
    "varying   vec4 csr_fColor;"
    "void main(void)"
    "{"
    "    csr_fColor   = csr_vColor;"
    "    gl_Position  = csr_uProjection * csr_uModelview * csr_vVertex;"
    "}";
//----------------------------------------------------------------------------
/**
* Fragment shader program to show a colored mesh
*/
const char* g_FragmentProgram_ColoredMesh =
    "precision mediump float;"
    "varying lowp vec4 csr_fColor;"
    "void main(void)"
    "{"
    "    gl_FragColor = csr_fColor;"
    "}";
//----------------------------------------------------------------------------
/**
* Vertex shader program to show a textured mesh
*/
const char* g_VertexProgram_TexturedMesh =
    "precision mediump float;"
    "attribute vec4 csr_vVertex;"
    "attribute vec4 csr_vColor;"
    "attribute vec2 csr_vTexCoord;"
    "uniform   mat4 csr_uProjection;"
    "uniform   mat4 csr_uModelview;"
    "varying   vec4 csr_fColor;"
    "varying   vec2 csr_fTexCoord;"
    "void main(void)"
    "{"
    "    csr_fColor    = csr_vColor;"
    "    csr_fTexCoord = csr_vTexCoord;"
    "    gl_Position   = csr_uProjection * csr_uModelview * csr_vVertex;"
    "}";
//----------------------------------------------------------------------------
/**
* Fragment shader program to show a textured mesh
*/
const char* g_FragmentProgram_TexturedMesh =
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
    m_pDocCanvas(NULL),
    m_pShader_ColoredMesh(NULL),
    m_pShader_TexturedMesh(NULL),
    m_pMesh(NULL),
    m_pMDL(NULL),
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

    if (m_pDocCanvas)
        delete m_pDocCanvas;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject* pSender)
{
    if (!m_pDocCanvas)
        m_pDocCanvas = new TCanvas();

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

    // show the dialog box
    if (pModelSelection->ShowModal() != mrOk)
        return;

    // clear all the previous models and meshes
    ClearModelsAndMeshes();

    CSR_VertexFormat vf;
    vf.m_UseNormals  = 0;
    vf.m_UseTextures = 0;
    vf.m_UseColors   = 1;

    // get the color to apply
    const unsigned rgbColor = Graphics::ColorToRGB(pModelSelection->paSelectedColor->Color);
    const unsigned color    =  ((rgbColor        & 0xff) << 24) |
                              (((rgbColor >> 8)  & 0xff) << 16) |
                              (((rgbColor >> 16) & 0xff) << 8)  |
                                                   0xFF;

    // select the model to build
    switch (pModelSelection->rgShapes->ItemIndex)
    {
        // surface
        case 0:
        {
            // enable the colored program
            csrShaderEnable(m_pShader_ColoredMesh);

            // create the shape to show
            m_pMesh = csrShapeCreateSurface(&vf, 0.5f, 0.5f, color);

            // create the AABB tree for the sphere mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
                m_AABBTrees.push_back(pTree);

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // box
        case 1:
        {
            // enable the colored program
            csrShaderEnable(m_pShader_ColoredMesh);

            // create the shape to show
            m_pMesh = csrShapeCreateBox(&vf, 0.5f, 0.5f, 0.5f, color, 0);

            // create the AABB tree for the sphere mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
                m_AABBTrees.push_back(pTree);

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // sphere
        case 2:
        {
            // enable the colored program
            csrShaderEnable(m_pShader_ColoredMesh);

            // create the shape to show
            m_pMesh = csrShapeCreateSphere(&vf,
                                            0.5f,
                                          ::StrToInt(pModelSelection->edSlices->Text),
                                          ::StrToInt(pModelSelection->edStacks->Text),
                                            color);

            // create the AABB tree for the sphere mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
                m_AABBTrees.push_back(pTree);

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // cylinder
        case 3:
        {
            // enable the colored program
            csrShaderEnable(m_pShader_ColoredMesh);

            // create the shape to show
            m_pMesh = csrShapeCreateCylinder(&vf,
                                              0.5f,
                                              1.0f,
                                            ::StrToInt(pModelSelection->edFaces->Text),
                                              color);

            // create the AABB tree for the sphere mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
                m_AABBTrees.push_back(pTree);

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // disk
        case 4:
        {
            // enable the colored program
            csrShaderEnable(m_pShader_ColoredMesh);

            // create the shape to show
            m_pMesh = csrShapeCreateDisk(&vf,
                                          0.0f,
                                          0.0f,
                                          0.5f,
                                        ::StrToInt(pModelSelection->edSlices->Text),
                                          color);

            // create the AABB tree for the sphere mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
                m_AABBTrees.push_back(pTree);

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // ring
        case 5:
        {
            // enable the colored program
            csrShaderEnable(m_pShader_ColoredMesh);

            // create the shape to show
            m_pMesh = csrShapeCreateRing(&vf,
                                          0.0f,
                                          0.0f,
                                          0.25f,
                                          0.5f,
                                        ::StrToInt(pModelSelection->edSlices->Text),
                                          color,
                                          color);

            // create the AABB tree for the sphere mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
                m_AABBTrees.push_back(pTree);

            // update the interface
            tbModelDistance->Position = 2;
            tbAnimationNb->Enabled    = false;
            tbAnimationSpeed->Enabled = false;
            return;
        }

        // spiral
        case 6:
        {
            // enable the colored program
            csrShaderEnable(m_pShader_ColoredMesh);

            // create the shape to show
            m_pMesh = csrShapeCreateSpiral(&vf,
                                            0.0f,
                                            0.0f,
                                            0.25f,
                                            0.5f,
                                            0.0f,
                                            0.0f,
                                            0.1f,
                                          ::StrToInt(pModelSelection->edSlices->Text),
                                          ::StrToInt(pModelSelection->edStacks->Text),
                                            color,
                                            color);

            // create the AABB tree for the sphere mesh
            CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

            // succeeded?
            if (pTree)
                m_AABBTrees.push_back(pTree);

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
                // enable the textured program
                csrShaderEnable(m_pShader_TexturedMesh);

                CSR_VertexFormat vertexFormat;
                vertexFormat.m_UseNormals  = 0;
                vertexFormat.m_UseTextures = 1;
                vertexFormat.m_UseColors   = 1;

                // load MDL model
                pMDL = csrMDLOpen(AnsiString(UnicodeString(modelFileName.c_str())).c_str(),
                                             0,
                                            &vertexFormat,
                                             color);

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
        case WM_WINDOWPOSCHANGED:
        {
            if (!m_Initialized)
                break;

            if (!m_pDocCanvas)
                break;

            if (m_fViewWndProc_Backup)
                m_fViewWndProc_Backup(message);

            HDC hDC = NULL;

            try
            {
                hDC = ::GetDC(paView->Handle);

                if (hDC)
                {
                    m_pDocCanvas->Handle = hDC;

                    // redraw here, thus the view will be redrawn to the correct size in real time
                    // while the size changes
                    DrawScene();
                }
            }
            __finally
            {
                if (hDC)
                    ::ReleaseDC(paView->Handle, hDC);
            }

            return;
        }

        case WM_PAINT:
        {
            if (!m_Initialized)
                break;

            if (!m_pDocCanvas)
                break;

            HDC           hDC = NULL;
            ::PAINTSTRUCT ps;

            try
            {
                hDC = ::BeginPaint(paView->Handle, &ps);

                if (hDC)
                {
                    m_pDocCanvas->Handle = hDC;
                    DrawScene();
                }
            }
            __finally
            {
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
        csrAABBTreeRelease(m_AABBTrees[i]);

    m_AABBTrees.clear();

    // release the scene objects
    csrMDLRelease(m_pMDL);
    csrMeshRelease(m_pMesh);

    // reset the values
    m_pMDL             = 0;
    m_pMesh            = 0;
    m_pTextureLastTime = 0.0;
    m_pModelLastTime   = 0.0;
    m_pMeshLastTime    = 0.0;
    m_TextureIndex     = 0;
    m_ModelIndex       = 0;
    m_MeshIndex        = 0;

    // reset the stats
    m_Stats.Clear();
    m_Stats.m_MaxPolyToCheckCount = 0;
    m_Stats.m_FPS                 = 0;
}
//------------------------------------------------------------------------------
void TMainForm::LoadShader(const std::string& vertex, const std::string& fragment, CSR_Shader*& pShader)
{
    CSR_Buffer* pVS = NULL;
    CSR_Buffer* pFS = NULL;

    try
    {
        // create buffers to contain vertex and fragment programs
        pVS = csrBufferCreate();
        pFS = csrBufferCreate();

        // copy the vertex program to read
        pVS->m_Length = vertex.length();
        pVS->m_pData  = new unsigned char[pVS->m_Length + 1];
        std::memcpy(pVS->m_pData, vertex.c_str(), pVS->m_Length);
        pVS->m_pData[pVS->m_Length] = 0x0;

        // copy the fragment program to read
        pFS->m_Length = fragment.length();
        pFS->m_pData  = new unsigned char[pFS->m_Length + 1];
        std::memcpy(pFS->m_pData, fragment.c_str(), pFS->m_Length);
        pFS->m_pData[pFS->m_Length] = 0x0;

        // compile and build the shader
        pShader = csrShaderLoadFromBuffer(pVS, pFS);
    }
    __finally
    {
        // release the buffers
        csrBufferRelease(pVS);
        csrBufferRelease(pFS);
    }
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
    GLint projectionUniform = glGetUniformLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(projectionUniform, 1, 0, &m_ProjectionMatrix.m_Table[0][0]);

    // enable the textured program
    csrShaderEnable(m_pShader_TexturedMesh);

    // connect projection matrix to textured shader
    projectionUniform = glGetUniformLocation(m_pShader_TexturedMesh->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(projectionUniform, 1, 0, &m_ProjectionMatrix.m_Table[0][0]);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    // load the shaders
    LoadShader(g_VertexProgram_ColoredMesh,  g_FragmentProgram_ColoredMesh,  m_pShader_ColoredMesh);
    LoadShader(g_VertexProgram_TexturedMesh, g_FragmentProgram_TexturedMesh, m_pShader_TexturedMesh);

    // enable the colored program
    csrShaderEnable(m_pShader_ColoredMesh);

    // get shader attributes
    m_pShader_ColoredMesh->m_VertexSlot = glGetAttribLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_vVertex");
    m_pShader_ColoredMesh->m_ColorSlot  = glGetAttribLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_vColor");

    // enable the textured program
    csrShaderEnable(m_pShader_TexturedMesh);

    // get shader attributes
    m_pShader_TexturedMesh->m_VertexSlot   = glGetAttribLocation(m_pShader_TexturedMesh->m_ProgramID, "csr_vVertex");
    m_pShader_TexturedMesh->m_ColorSlot    = glGetAttribLocation(m_pShader_TexturedMesh->m_ProgramID, "csr_vColor");
    m_pShader_TexturedMesh->m_TexCoordSlot = glGetAttribLocation(m_pShader_TexturedMesh->m_ProgramID, "csr_vTexCoord");
    m_pShader_TexturedMesh->m_TextureSlot  = glGetAttribLocation(m_pShader_TexturedMesh->m_ProgramID, "csr_sTexture");

    // enable the colored program
    csrShaderEnable(m_pShader_ColoredMesh);

    CSR_VertexFormat vf;
    vf.m_UseNormals  = 0;
    vf.m_UseTextures = 0;
    vf.m_UseColors   = 1;

    // create a default sphere
    m_pMesh = csrShapeCreateSphere(&vf, 0.5f, 10, 10, 0xFFFF);

    // create the AABB tree for the sphere mesh
    CSR_AABBNode* pTree = csrAABBTreeFromMesh(m_pMesh);

    // succeeded?
    if (pTree)
        m_AABBTrees.push_back(pTree);

    // set the initial values
    m_pTextureLastTime = 0.0;
    m_pModelLastTime   = 0.0;
    m_pMeshLastTime    = 0.0;
    m_TextureIndex     = 0;
    m_ModelIndex       = 0;
    m_MeshIndex        = 0;

    // configure OpenGL depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRangef(0.0f, 1.0f);

    // enable culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    m_Initialized = true;
}
//------------------------------------------------------------------------------
void TMainForm::DeleteScene()
{
    // clear all the models and meshes composing the scene
    ClearModelsAndMeshes();

    // release the shaders
    csrShaderRelease(m_pShader_TexturedMesh);
    csrShaderRelease(m_pShader_ColoredMesh);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    float       xAngle;
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
    t.m_Y =  0.0f;
    t.m_Z = -float(tbModelDistance->Position);

    // build the translation matrix
    csrMat4Translate(&t, &translateMatrix);

    // set rotation on X axis
    r.m_X = 1.0f;
    r.m_Y = 0.0f;
    r.m_Z = 0.0f;

    // rotate 90 degrees
    xAngle = -M_PI / 2.0f;

    // build the X rotation matrix
    csrMat4Rotate(&xAngle, &r, &xRotateMatrix);

    // set rotation on Y axis
    r.m_X = 0.0f;
    r.m_Y = 1.0f;
    r.m_Z = 0.0f;

    // build the Y rotation matrix
    csrMat4Rotate(&m_AngleY, &r, &yRotateMatrix);

    // build the model view matrix
    csrMat4Multiply(&xRotateMatrix, &yRotateMatrix,   &rotateMatrix);
    csrMat4Multiply(&rotateMatrix,  &translateMatrix, &m_ModelMatrix);

    // enable the colored program
    csrShaderEnable(m_pShader_ColoredMesh);

    // connect the model view matrix to shader
    GLint modelUniform = glGetUniformLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_uModelview");
    glUniformMatrix4fv(modelUniform, 1, 0, &m_ModelMatrix.m_Table[0][0]);

    // enable the textured program
    csrShaderEnable(m_pShader_TexturedMesh);

    // connect the model view matrix to shader
    modelUniform = glGetUniformLocation(m_pShader_TexturedMesh->m_ProgramID, "csr_uModelview");
    glUniformMatrix4fv(modelUniform, 1, 0, &m_ModelMatrix.m_Table[0][0]);

    // calculate the ray from current mouse position
    CalculateMouseRay();
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    try
    {
        // begin the scene
        csrSceneBegin(0.0f, 0.0f, 0.0f, 1.0f);

        // restore normal drawing parameters
        glDisable(GL_BLEND);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (m_pMesh)
        {
            // enable the colored program
            csrShaderEnable(m_pShader_ColoredMesh);

            // draw the mesh
            csrSceneDrawMesh(m_pMesh, m_pShader_ColoredMesh);
        }
        else
        if (m_pMDL)
        {
            // enable the textured program
            csrShaderEnable(m_pShader_TexturedMesh);

            // draw the MDL model
            csrSceneDrawMDL(m_pMDL,
                            m_pShader_TexturedMesh,
                            m_TextureIndex,
                            m_ModelIndex,
                            m_MeshIndex);
        }
        else
            return;

        // enable the colored program
        csrShaderEnable(m_pShader_ColoredMesh);

        // show the polygon intersections (with the mouse ray)
        if (ckShowCollidingPolygons->Checked)
            ResolveTreeAndDrawPolygons();

        // do show the AABB boxes?
        if (ckShowBoxes->Checked)
        {
            // enable alpha blending
            glEnable(GL_BLEND);
            glBlendEquation(GL_FUNC_ADD);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // do show the boxes in wireframe mode?
            if (ckWireFrame->Checked)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
        // end the scene
        csrSceneEnd();
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

    // disable the culling to show the polygons from any point of view
    glDisable(GL_CULL_FACE);

    // found polygons to draw?
    if (polygonsToDrawCount)
    {
        CSR_Mesh mesh;

        // create and configure a mesh for the polygons
        mesh.m_Count                       = 1;
        mesh.m_Shader.m_TextureID          = GL_INVALID_VALUE;
        mesh.m_Shader.m_BumpMapID          = GL_INVALID_VALUE;
        mesh.m_pVB                         = (CSR_VertexBuffer*)csrMemoryAlloc(0, sizeof(CSR_VertexBuffer), 1);
        mesh.m_pVB->m_Format.m_Type        = CSR_VT_Triangles;
        mesh.m_pVB->m_Format.m_UseNormals  = 0;
        mesh.m_pVB->m_Format.m_UseColors   = 1;
        mesh.m_pVB->m_Format.m_UseTextures = 0;
        mesh.m_pVB->m_Format.m_Stride      = 7;
        mesh.m_pVB->m_pData                = m_PolygonArray;
        mesh.m_pVB->m_Count                = 21;

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
            csrSceneDrawMesh(&mesh, m_pShader_ColoredMesh);
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

    CSR_Mesh* pMesh = NULL;

    try
    {
        // get the mesh for the box (this is weak for performances, and in a real productive code I
        // would not do that this way, but thus the box is get to his final location directly, and
        // this simplify the code, which is only used to visualize graphically the tree content)
        pMesh = CreateBox(pTree->m_pBox->m_Min,
                          pTree->m_pBox->m_Max,
                          color | ((tbTransparency->Position * 0xFF) / tbTransparency->Max));

        // draw the AABB box
        if (pMesh)
            csrSceneDrawMesh(pMesh, m_pShader_ColoredMesh);
    }
    __finally
    {
        // delete the mesh
        if (pMesh)
            csrMeshRelease(pMesh);
    }
}
//---------------------------------------------------------------------------
CSR_Mesh* TMainForm::CreateBox(const CSR_Vector3& min, const CSR_Vector3& max, unsigned color) const
{
    size_t      i;
    CSR_Vector3 vertices[8];
    CSR_Mesh*   pMesh;

    // create a new mesh for the box
    pMesh = csrMeshCreate();

    // succeeded?
    if (!pMesh)
        return 0;

    // prepare the vertex buffer for each box edges
    pMesh->m_pVB   = (CSR_VertexBuffer*)csrMemoryAlloc(0, sizeof(CSR_VertexBuffer), 6);
    pMesh->m_Count = 6;

    // succeeded?
    if (!pMesh->m_pVB)
    {
        csrMeshRelease(pMesh);
        return 0;
    }

    // iterate through each edges
    for (i = 0; i < 6; ++i)
    {
        // set vertex format and calculate the stride
        pMesh->m_pVB[i].m_Format.m_Type        = CSR_VT_TriangleStrip;
        pMesh->m_pVB[i].m_Format.m_UseNormals  = 0;
        pMesh->m_pVB[i].m_Format.m_UseTextures = 0;
        pMesh->m_pVB[i].m_Format.m_UseColors   = 1;
        csrVertexFormatCalculateStride(&pMesh->m_pVB[i].m_Format);

        // initialize the vertex buffer data
        pMesh->m_pVB[i].m_pData = 0;
        pMesh->m_pVB[i].m_Count = 0;
    }

    // iterate through vertices to create. Vertices are generated as follow:
    //     v2 *--------* v6
    //      / |      / |
    // v4 *--------* v8|
    //    |   |    |   |
    //    |v1 *----|---* v5
    //    | /      | /
    // v3 *--------* v7
    for (i = 0; i < 8; ++i)
    {
        // generate the 4 first vertices on the left, and 4 last on the right
        if (!(i / 4))
            vertices[i].m_X = min.m_X;
        else
            vertices[i].m_X = max.m_X;

        // generate 2 vertices on the front, then 2 vertices on the back
        if (!((i / 2) % 2))
            vertices[i].m_Z = min.m_Z;
        else
            vertices[i].m_Z = max.m_Z;

        // for each vertices, generates one on the top, and one on the bottom
        if (!(i % 2))
            vertices[i].m_Y = min.m_Y;
        else
            vertices[i].m_Y = max.m_Y;
    }

    // create box edge 1
    csrVertexBufferAdd(&vertices[1], NULL, NULL, color, &pMesh->m_pVB[0]);
    csrVertexBufferAdd(&vertices[0], NULL, NULL, color, &pMesh->m_pVB[0]);
    csrVertexBufferAdd(&vertices[3], NULL, NULL, color, &pMesh->m_pVB[0]);
    csrVertexBufferAdd(&vertices[2], NULL, NULL, color, &pMesh->m_pVB[0]);

    // create box edge 2
    csrVertexBufferAdd(&vertices[3], NULL, NULL, color, &pMesh->m_pVB[1]);
    csrVertexBufferAdd(&vertices[2], NULL, NULL, color, &pMesh->m_pVB[1]);
    csrVertexBufferAdd(&vertices[7], NULL, NULL, color, &pMesh->m_pVB[1]);
    csrVertexBufferAdd(&vertices[6], NULL, NULL, color, &pMesh->m_pVB[1]);

    // create box edge 3
    csrVertexBufferAdd(&vertices[7], NULL, NULL, color, &pMesh->m_pVB[2]);
    csrVertexBufferAdd(&vertices[6], NULL, NULL, color, &pMesh->m_pVB[2]);
    csrVertexBufferAdd(&vertices[5], NULL, NULL, color, &pMesh->m_pVB[2]);
    csrVertexBufferAdd(&vertices[4], NULL, NULL, color, &pMesh->m_pVB[2]);

    // create box edge 4
    csrVertexBufferAdd(&vertices[5], NULL, NULL, color, &pMesh->m_pVB[3]);
    csrVertexBufferAdd(&vertices[4], NULL, NULL, color, &pMesh->m_pVB[3]);
    csrVertexBufferAdd(&vertices[1], NULL, NULL, color, &pMesh->m_pVB[3]);
    csrVertexBufferAdd(&vertices[0], NULL, NULL, color, &pMesh->m_pVB[3]);

    // create box edge 5
    csrVertexBufferAdd(&vertices[1], NULL, NULL, color, &pMesh->m_pVB[4]);
    csrVertexBufferAdd(&vertices[3], NULL, NULL, color, &pMesh->m_pVB[4]);
    csrVertexBufferAdd(&vertices[5], NULL, NULL, color, &pMesh->m_pVB[4]);
    csrVertexBufferAdd(&vertices[7], NULL, NULL, color, &pMesh->m_pVB[4]);

    // create box edge 6
    csrVertexBufferAdd(&vertices[2], NULL, NULL, color, &pMesh->m_pVB[5]);
    csrVertexBufferAdd(&vertices[0], NULL, NULL, color, &pMesh->m_pVB[5]);
    csrVertexBufferAdd(&vertices[6], NULL, NULL, color, &pMesh->m_pVB[5]);
    csrVertexBufferAdd(&vertices[4], NULL, NULL, color, &pMesh->m_pVB[5]);

    return pMesh;
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

    CSR_Matrix4 identity;
    csrMat4Identity(&identity);

    // put the ray in the world coordinates
    csrMat4Unproject(&m_ProjectionMatrix, &identity, &m_Ray);

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
    unsigned vertexCount = 0;
    unsigned stride      = 0;

    if (m_pMesh)
    {
        // get the mesh stride
        stride = m_pMesh->m_pVB ? m_pMesh->m_pVB->m_Format.m_Stride : 0;

        // count all vertices contained in the mesh
        for (std::size_t i = 0; i < m_pMesh->m_Count; ++i)
            vertexCount += m_pMesh->m_pVB[i].m_Count;
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
    }
    else
        return;

    std::size_t polyCount = 0;

    // calculate the polygons count
    polyCount = stride ? ((vertexCount / stride) / 3) : 0;

    // show the stats
    laPolygonCount->Caption    = L"Polygons Count: "        + ::IntToStr(int(polyCount));
    laPolygonsToCheck->Caption = L"Polygons To Check: "     + ::IntToStr(int(m_Stats.m_PolyToCheckCount));
    laMaxPolyToCheck->Caption  = L"Max Polygons To Check: " + ::IntToStr(int(m_Stats.m_MaxPolyToCheckCount));
    laHitPolygons->Caption     = L"Hit Polygons: "          + ::IntToStr(int(m_Stats.m_HitPolygonCount));
    laHitBoxes->Caption        = L"Hit Boxes: "             + ::IntToStr(int(m_Stats.m_HitBoxCount));
    laFPS->Caption             = L"FPS:"                    + ::IntToStr(int(m_Stats.m_FPS));
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    // calculate time interval
    const unsigned __int64 now            = ::GetTickCount();
    const double           elapsedTime    = (now - m_PreviousTime) / 1000.0;
                           m_PreviousTime =  now;

    done = false;

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
