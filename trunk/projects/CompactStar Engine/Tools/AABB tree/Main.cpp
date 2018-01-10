#include <vcl.h>
#pragma hdrstop
#include "Main.h"

#include <string>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"
#include "CSR_Renderer.h"
#include "CSR_Sound.h"

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
* Vertex shader program
*/
const char* g_VertexProgram =
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
* Fragment shader program
*/
const char* g_FragmentProgram =
    "precision mediump float;"
    "varying lowp vec4 csr_fColor;"
    "void main(void)"
    "{"
    "    gl_FragColor = csr_fColor;"
    "}";
//---------------------------------------------------------------------------
// TMainForm::ITreeStats
//---------------------------------------------------------------------------
TMainForm::ITreeStats::ITreeStats() :
    m_HitBoxCount(0)
{}
//---------------------------------------------------------------------------
TMainForm::ITreeStats::~ITreeStats()
{}
//---------------------------------------------------------------------------
void TMainForm::ITreeStats::Clear()
{
    m_HitBoxCount = 0;
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
    m_pShader(NULL),
    m_pSphere(NULL),
    m_AngleY(0.0f),
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
void TMainForm::CreateViewport(float w, float h)
{
    if (!m_pShader)
        return;

    // calculate matrix items
    const float zNear  = 1.0f;
    const float zFar   = 1000.0f;
    const float fov    = 45.0f;
    const float aspect = w / h;

    // create the OpenGL viewport
    glViewport(0, 0, w, h);

    csrMat4Perspective(fov, aspect, zNear, zFar, &m_ProjectionMatrix);

    // connect projection matrix to shader
    GLint projectionUniform = glGetUniformLocation(m_pShader->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(projectionUniform, 1, 0, &m_ProjectionMatrix.m_Table[0][0]);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    CSR_Buffer* pVS = csrBufferCreate();
    CSR_Buffer* pFS = csrBufferCreate();

    std::string data = g_VertexProgram;

    pVS->m_Length = data.length();
    pVS->m_pData  = new unsigned char[pVS->m_Length + 1];
    std::memcpy(pVS->m_pData, data.c_str(), pVS->m_Length);
    pVS->m_pData[pVS->m_Length] = 0x0;

    data = g_FragmentProgram;

    pFS->m_Length = data.length();
    pFS->m_pData  = new unsigned char[pFS->m_Length + 1];
    std::memcpy(pFS->m_pData, data.c_str(), pFS->m_Length);
    pFS->m_pData[pFS->m_Length] = 0x0;

    m_pShader = csrShaderLoadFromBuffer(pVS, pFS);

    csrBufferRelease(pVS);
    csrBufferRelease(pFS);

    csrShaderEnable(m_pShader);

    // get shader attributes
    m_pShader->m_VertexSlot = glGetAttribLocation(m_pShader->m_ProgramID, "csr_vVertex");
    m_pShader->m_ColorSlot  = glGetAttribLocation(m_pShader->m_ProgramID, "csr_vColor");

    CSR_VertexFormat vf;
    vf.m_Type = CSR_VT_TriangleStrip;
    vf.m_UseNormals  = 0;
    vf.m_UseTextures = 0;
    vf.m_UseColors   = 1;

    m_pSphere   = csrShapeCreateSphere(&vf, 0.5f, 10, 10, 0xFFFF);
    m_pAABBTree = csrAABBTreeFromMesh(m_pSphere);

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
    csrAABBTreeRelease(m_pAABBTree);
    csrMeshRelease(m_pSphere);
    csrShaderRelease(m_pShader);
}
//------------------------------------------------------------------------------
void TMainForm::UpdateScene(float elapsedTime)
{
    m_AngleY += (elapsedTime * 0.5f);

    while (m_AngleY > M_PI * 2.0f)
        m_AngleY -= M_PI * 2.0f;
}
//------------------------------------------------------------------------------
void TMainForm::DrawScene()
{
    float       xAngle;
    float       yAngle;
    CSR_Vector3 t;
    CSR_Vector3 r;
    CSR_Matrix4 translateMatrix;
    CSR_Matrix4 xRotateMatrix;
    CSR_Matrix4 yRotateMatrix;
    CSR_Matrix4 rotateMatrix;

    csrSceneBegin(0.0f, 0.0f, 0.0f, 1.0f);

    // set translation
    t.m_X =  0.0f;
    t.m_Y =  0.0f;
    t.m_Z = -2.0f;

    csrMat4Translate(&t, &translateMatrix);

    // set rotation on X axis
    r.m_X = 1.0f;
    r.m_Y = 0.0f;
    r.m_Z = 0.0f;

    // rotate 90 degrees
    xAngle = 1.57075f;

    csrMat4Rotate(&xAngle, &r, &xRotateMatrix);

    // set rotation on Y axis
    r.m_X = 0.0f;
    r.m_Y = 1.0f;
    r.m_Z = 0.0f;

    csrMat4Rotate(&m_AngleY, &r, &yRotateMatrix);

    // build model view matrix
    csrMat4Multiply(&xRotateMatrix, &yRotateMatrix,   &rotateMatrix);
    csrMat4Multiply(&rotateMatrix,  &translateMatrix, &m_ModelMatrix);

    // connect model view matrix to shader
    GLint modelUniform = glGetUniformLocation(m_pShader->m_ProgramID, "csr_uModelview");
    glUniformMatrix4fv(modelUniform, 1, 0, &m_ModelMatrix.m_Table[0][0]);

    glDisable(GL_BLEND);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    csrSceneDrawMesh(m_pSphere, m_pShader);

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (ckWireFrame->Checked)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    CalculateMouseRay();

    DrawTreeBoxes(m_pAABBTree);

    csrSceneEnd();
}
//---------------------------------------------------------------------------
void TMainForm::DrawTreeBoxes(const CSR_AABBNode* pTree)
{
    if (pTree->m_pLeft)
        DrawTreeBoxes(pTree->m_pLeft);

    if (pTree->m_pRight)
        DrawTreeBoxes(pTree->m_pRight);

    if (ckShowLeafOnly->Checked && (pTree->m_pLeft || pTree->m_pRight))
        return;

    CSR_Figure3 ray;
    ray.m_Type    = CSR_F3_Ray;
    ray.m_pFigure = (void*)&m_Ray;

    CSR_Figure3 box;
    box.m_Type    = CSR_F3_Box;
    box.m_pFigure = pTree->m_pBox;

    unsigned color;

    if (csrIntersect3(&ray, &box, 0, 0))
    {
        ++m_Stats.m_HitBoxCount;
        color = 0xFF0000;
    }
    else
        color = 0xFF000000;

    CSR_Mesh* pMesh = NULL;

    try
    {
        pMesh = CreateBox(pTree->m_pBox->m_Min,
                          pTree->m_pBox->m_Max,
                          color | ((tbTransparency->Position * 0xFF) / tbTransparency->Max));

        if (pMesh)
            csrSceneDrawMesh(pMesh, m_pShader);
    }
    __finally
    {
        if (pMesh)
            csrMeshRelease(pMesh);
    }

    /*
    try
    {
        // get polygons to check for collision by resolving AABB tree
        pAABBTree->Resolve(pRay.get(), polygons);

        QR_PolygonsP pickedPolygons;
        QR_SizeT     polygonCount = polygons.size();

        #ifdef USE_DETECTION_COLLISION_DEBUG_TOOLS
            #ifdef OS_WIN
                #ifdef CP_EMBARCADERO
                    polysCount = pAABBTree->GetPolygonCount();
                    polysFound = polygonCount;
                #endif
            #endif
        #endif

        // iterate through polygons to check
        if (polygonCount > 0)
            for (QR_PolygonsP::const_iterator it = polygons.begin(); it != polygons.end(); ++it)
                // is polygon intersecting ray?
                if (QR_GeometryHelper::RayIntersectsPolygon(*(pRay.get()), *(*it)))
                    // add polygon in collision to resulting list
                    pickedPolygons.push_back(*it);

        #ifdef USE_DETECTION_COLLISION_DEBUG_TOOLS
            #ifdef OS_WIN
                #ifdef CP_EMBARCADERO
                    polysinCollision = pickedPolygons.size();
                #endif
            #endif
        #endif

        // found collision?
        if (pickedPolygons.size())
        {
            // draw polygons in collision if required
            if (m_ShowCollisionWithMouse)
                DrawPolygons(pickedPolygons, modelMatrix);

            // notify that a picked model was found
            if (m_fOnModelPicked)
                m_fOnModelPicked(pRenderer,
                                 pSender,
                                 id,
                                 pModel,
                                 pMesh,
                                 pAABBTree,
                                 pickedPolygons,
                                 projectionMatrix,
                                 viewMatrix,
                                 modelMatrix,
                                 m_MousePos);
        }
    }
    catch (...)
    {
        QR_STDTools::DelAndClear(polygons);
        throw;
    }

    QR_STDTools::DelAndClear(polygons);
    */
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
CSR_Vector3 TMainForm::MousePosToViewportPos(const TPoint&   mousePos,
                                             const TRect&    clientRect,
                                             const CSR_Rect& viewRect)
{
    CSR_Vector3 result;

    // invalid client width or height?
    if (!ClientWidth || !ClientHeight)
    {
        result.m_X = 0.0f;
        result.m_Y = 0.0f;
        result.m_Z = 0.0f;
        return result;
    }

    // convert mouse position to scene position
    result.m_X = viewRect.m_Min.m_X + ((mousePos.X * (viewRect.m_Max.m_X - viewRect.m_Min.m_X)) / paView->ClientWidth);
    result.m_Y = viewRect.m_Min.m_Y - ((mousePos.Y * (viewRect.m_Min.m_Y - viewRect.m_Max.m_Y)) / paView->ClientHeight);
    result.m_Z = 0.0f;
    return result;
}
//---------------------------------------------------------------------------
void TMainForm::CalculateMouseRay()
{
    // clear the tree stats
    m_Stats.Clear();

    // get the mouse position
    TPoint mousePos = Mouse->CursorPos;

    // convert to view position
    if (!::ScreenToClient(paView->Handle, &mousePos))
        return;

    CSR_Rect viewRect;

    // get the viewport rectangle
    viewRect.m_Min.m_X = -1.0f;
    viewRect.m_Min.m_Y =  1.0f;
    viewRect.m_Max.m_X =  1.0f;
    viewRect.m_Max.m_Y = -1.0f;

    // get the ray in the Windows coordinate
    m_Ray.m_Pos     =  MousePosToViewportPos(mousePos, paView->ClientRect, viewRect);
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
void __fastcall TMainForm::OnIdle(TObject* pSender, bool& done)
{
    // calculate time interval
    const unsigned __int64 now            = ::GetTickCount();
    const double           elapsedTime    = (now - m_PreviousTime) / 1000.0;
                           m_PreviousTime =  now;

    // update and draw the scene
    UpdateScene(elapsedTime);
    DrawScene();

    ::SwapBuffers(m_hDC);

    done = false;
}
//---------------------------------------------------------------------------
