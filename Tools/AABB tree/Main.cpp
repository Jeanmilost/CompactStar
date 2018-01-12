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
* Fragment shader program
*/
const char* g_FragmentProgram_ColoredMesh =
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
    m_pModel(NULL),
    m_AngleY(0.0f),
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
    if (!m_pShader_ColoredMesh)
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
    GLint projectionUniform = glGetUniformLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(projectionUniform, 1, 0, &m_ProjectionMatrix.m_Table[0][0]);
}
//------------------------------------------------------------------------------
void TMainForm::InitScene(int w, int h)
{
    CSR_Buffer* pVS = csrBufferCreate();
    CSR_Buffer* pFS = csrBufferCreate();

    std::string data = g_VertexProgram_ColoredMesh;

    pVS->m_Length = data.length();
    pVS->m_pData  = new unsigned char[pVS->m_Length + 1];
    std::memcpy(pVS->m_pData, data.c_str(), pVS->m_Length);
    pVS->m_pData[pVS->m_Length] = 0x0;

    data = g_FragmentProgram_ColoredMesh;

    pFS->m_Length = data.length();
    pFS->m_pData  = new unsigned char[pFS->m_Length + 1];
    std::memcpy(pFS->m_pData, data.c_str(), pFS->m_Length);
    pFS->m_pData[pFS->m_Length] = 0x0;

    m_pShader_ColoredMesh = csrShaderLoadFromBuffer(pVS, pFS);

    csrBufferRelease(pVS);
    csrBufferRelease(pFS);

    csrShaderEnable(m_pShader_ColoredMesh);

    // get shader attributes
    m_pShader_ColoredMesh->m_VertexSlot = glGetAttribLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_vVertex");
    m_pShader_ColoredMesh->m_ColorSlot  = glGetAttribLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_vColor");

    CSR_VertexFormat vf;
    vf.m_Type        = CSR_VT_TriangleStrip;
    vf.m_UseNormals  = 0;
    vf.m_UseTextures = 0;
    vf.m_UseColors   = 1;

    m_pModel    = csrShapeCreateSphere(&vf, 0.5f, 10, 10, 0xFFFF);
    m_pAABBTree = csrAABBTreeFromMesh(m_pModel);

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
    csrMeshRelease(m_pModel);
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
    m_AngleY += (elapsedTime * (float(tbSpeedVelocity->Position) * 0.01f));

    // avoid the angle to exceeds the bounds even if a huge time has elapsed since last update
    while (m_AngleY > M_PI * 2.0f)
        m_AngleY -= M_PI * 2.0f;

    // set translation
    t.m_X =  0.0f;
    t.m_Y =  0.0f;
    t.m_Z = -2.0f;

    // build the translation matrix
    csrMat4Translate(&t, &translateMatrix);

    // set rotation on X axis
    r.m_X = 1.0f;
    r.m_Y = 0.0f;
    r.m_Z = 0.0f;

    // rotate 90 degrees
    xAngle = 1.57075f;

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

    // connect the model view matrix to shader
    GLint modelUniform = glGetUniformLocation(m_pShader_ColoredMesh->m_ProgramID, "csr_uModelview");
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

        csrSceneDrawMesh(m_pModel, m_pShader_ColoredMesh);

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

            // draw the AABB tree boxes
            DrawTreeBoxes(m_pAABBTree);
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
    // is the mouse ray not initialized?
    if (!m_Ray.m_Pos.m_X    && !m_Ray.m_Pos.m_Y    && !m_Ray.m_Pos.m_Z &&
        !m_Ray.m_Dir.m_X    && !m_Ray.m_Dir.m_Y    && !m_Ray.m_Dir.m_Z &&
        !m_Ray.m_InvDir.m_X && !m_Ray.m_InvDir.m_Y && !m_Ray.m_InvDir.m_Z)
        return;

    CSR_Polygon3Buffer polygonBuffer;

    // using the mouse ray, resolve aligned-axis bounding box tree
    csrAABBTreeResolve(&m_Ray, m_pAABBTree, 0, &polygonBuffer);

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
        mesh.m_Count                       =  1;
        mesh.m_TextureShader.m_TextureID   = -1;
        mesh.m_TextureShader.m_BumpMapID   = -1;
        mesh.m_pVB                         =  (CSR_VertexBuffer*)csrMemoryAlloc(0, sizeof(CSR_VertexBuffer), 1);
        mesh.m_pVB->m_Format.m_Type        =   CSR_VT_Triangles;
        mesh.m_pVB->m_Format.m_UseNormals  =  0;
        mesh.m_pVB->m_Format.m_UseColors   =  1;
        mesh.m_pVB->m_Format.m_UseTextures =  0;
        mesh.m_pVB->m_Format.m_Stride      =  7;
        mesh.m_pVB->m_pData                =  m_PolygonArray;
        mesh.m_pVB->m_Count                =  21;

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
void TMainForm::ShowStats() const
{
    laPolygonCount->Caption    = L"Polygons Count: "        + ::IntToStr(int(m_pModel && m_pModel->m_pVB ? m_pModel->m_pVB->m_Count : 0));
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
