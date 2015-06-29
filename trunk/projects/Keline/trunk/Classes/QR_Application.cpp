/******************************************************************************
 * ==> QR_Application --------------------------------------------------------*
 ******************************************************************************
 * Description : Frame per seconds manager                                    *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QR_Application.h"

// std
#include <fstream>

// qr engine
#include "QR_Exception.h"
#include "QR_Algorithms.h"
#include "QR_Vertex.h"

// libraries
#include <gl/wglew.h>

// resources
#include "Resources.rh"

//------------------------------------------------------------------------------
// MD2 animation list
//------------------------------------------------------------------------------
QR_MD2::IAnimationInfo g_AnimValues[] =
{
    QR_MD2::IAnimationInfo( 0, 39, "WALK"),
};
//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------
QR_Color                        QR_Application::m_Black;
QR_Color                        QR_Application::m_White(255, 255, 255);
QR_Camera_OpenGL                QR_Application::m_Camera;
QR_MD2                          QR_Application::m_Mesh;
QR_MD2::ILight                  QR_Application::m_ModelLight;
QR_Shader_OpenGL                QR_Application::m_Shader;
QR_Renderer_OpenGL::IShaderData QR_Application::m_ShaderData;
QR_Matrix16P                    QR_Application::m_ProjectionMatrix;
QR_Matrix16P                    QR_Application::m_ViewMatrix;
QR_DirectInput                  QR_Application::m_DirectInput;
M_Precision                     QR_Application::m_2PI   = 2.0f * M_PI;
QR_MSAA_OpenGL_Effect*          QR_Application::m_pMSAA = NULL;
//------------------------------------------------------------------------------
// QR_Application - c++ Windows only
//------------------------------------------------------------------------------
QR_Application::QR_Application()
{}
//------------------------------------------------------------------------------
QR_Application::~QR_Application()
{
    // delete anti aliasing filter
    if (m_pMSAA)
    {
        delete m_pMSAA;
        m_pMSAA = NULL;
    }
}
//------------------------------------------------------------------------------
int QR_Application::Execute(HINSTANCE hInstance, const std::string appName)
{
    // try to load application icon from resources
    HICON hIcon = (HICON)::LoadImage(::GetModuleHandle(NULL),
                                     MAKEINTRESOURCE(IDI_MAIN_ICON),
                                     IMAGE_ICON,
                                     16,
                                     16,
                                     0);

    QR_Window          window(hInstance, appName, 480.0f, 320.0f, hIcon);
    QR_Renderer_OpenGL renderer(window.GetHandle(), OnConfigure);
    int                result = EXIT_FAILURE;

    try
    {
        // configure window
        window.Set_OnDrawScene(OnDrawScene);
        window.SetRenderer(&renderer);

        // stop GLEW crashing on OSX :-/
        glewExperimental = GL_TRUE;

        // initialize GLEW
        if (glewInit() != GLEW_OK)
            M_THROW_EXCEPTION("Could not initialize GLEW");

        // initialize DirectInput
        m_DirectInput.Initialize(NULL, window.GetHandle());

        // create 3D scene
        CreateScene(&window, &renderer);

        // run window main loop
        result = window.Run();
    }
    catch (...)
    {}

    return result;
}
//------------------------------------------------------------------------------
void QR_Application::CreateScene(const QR_Window*   pWindow,
                                       QR_Renderer* pRenderer)
{
    // window exists?
    if (!pWindow)
        return;

    // no renderer?
    if (!pRenderer)
        return;

    // get window width and height
    RECT  winRect            = pWindow->GetRect();
    const unsigned winWidth  = (winRect.right  - winRect.left);
    const unsigned winHeight = (winRect.bottom - winRect.top);

    // create projection matrix (will not be modified while execution)
    m_ProjectionMatrix = m_Camera.GetMatrix(22.5f,
                                            winWidth,
                                            winHeight,
                                            1.0f,
                                            200.0f);

    // create view matrix (will not be modified while execution)
    m_ViewMatrix =
            m_Camera.LookAtLH(QR_Vector3DP(0.0f, 0.0f, 0.0f),
                              QR_Vector3DP(0.0f, 0.0f, 1.0f),
                              QR_Vector3DP(0.0f, 1.0f, 0.0f));

    // create post processing multi sampling anti aliasing filter
    m_pMSAA = new QR_MSAA_OpenGL_Effect(winWidth, winHeight);
    pRenderer->SetPostProcessingEffect(m_pMSAA);

    // try to load mesh file
    if (!m_Mesh.LoadMesh("Models\\Female_animated_2.md2"))
        M_THROW_EXCEPTION("Unable to open mesh file");

    // try to load mesh normals table
    if (!m_Mesh.LoadNormalsTable("Tables\\Normals.bin"))
        M_THROW_EXCEPTION("Unable to open normals table file");

    // load and compile shader
    m_Shader.CreateProgram();
    m_Shader.Set_OnBindAttribute(OnBindAttribute);
    m_Shader.AttachFile("Shaders\\vertex.glsl",   GL_VERTEX_SHADER);
    m_Shader.AttachFile("Shaders\\fragment.glsl", GL_FRAGMENT_SHADER);

    // try to link shader
    if (!m_Shader.Link(false))
        M_THROW_EXCEPTION("Could not link shader program");

    // place model in 3D world
    m_Mesh.SetTranslation(QR_Vector3DP(0.0f, -3.0f, -10.0f));
    m_Mesh.SetRotationX(-QR_Algorithms::DegToRad(90));
    m_Mesh.SetScaling(QR_Vector3DP(0.2f, 0.2f, 0.2f));

    // configure mesh animation
    m_Mesh.SetAnimation(g_AnimValues[0]);

    // populate shader data
    m_ShaderData.m_pShader    = &m_Shader;
    m_ShaderData.m_Attrib_Pos = "qr_vPosition";
}
//------------------------------------------------------------------------------
void QR_Application::OnConfigure(const QR_Renderer* pRenderer)
{
    // basic OpenGL configuration
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    // enable culling
    glEnable(GL_CULL_FACE);
}
//------------------------------------------------------------------------------
void QR_Application::OnBindAttribute(GLuint programID, GLenum type)
{
    switch (type)
    {
        case GL_VERTEX_SHADER:
            break;

        case GL_FRAGMENT_SHADER:
            break;

        default:
            M_THROW_EXCEPTION("Unknown shader type");
    }
}
//------------------------------------------------------------------------------
void QR_Application::OnDrawScene(const QR_Window* pWindow,
        const QR_Renderer* pRenderer, const std::clock_t& elapsedTime)
{
    // renderer exists?
    if (!pRenderer)
        return;

    // get action to do based on user input
    QR_DirectInput::IEAction action = m_DirectInput.GetAction();

    // do rotate to the left?
    if (action & QR_DirectInput::IE_AC_Left)
    {
        M_Precision angle = m_Mesh.GetRotationY() + 0.05f;

        if (angle >= m_2PI)
            angle -= m_2PI;

        m_Mesh.SetRotationY(angle);
    }

    // do rotate to the right?
    if (action & QR_DirectInput::IE_AC_Right)
    {
        M_Precision angle = m_Mesh.GetRotationY() - 0.05f;

        if (angle < 0.0f)
            angle += m_2PI;

        m_Mesh.SetRotationY(angle);
    }

    // do move forward?
    if (action & QR_DirectInput::IE_AC_Up)
    {
        M_Precision  velocity        = 0.05f;
        M_Precision  playerDirection = m_Mesh.GetRotationY() + M_PI_2;
        QR_Vector3DP position        = m_Mesh.GetTranslation();

        if (playerDirection >= m_2PI)
            playerDirection -= m_2PI;

        // calculate next position
        position.m_X += (velocity * ((M_Precision)std::sin(playerDirection)));
        position.m_Z += (velocity * ((M_Precision)std::cos(playerDirection)));

        m_Mesh.SetTranslation(position);

        m_Mesh.Animate(elapsedTime, 30);
    }

    /*
    // do move back?
    if (action & QR_DirectInput::IE_AC_Down)
        m_Mesh.MoveBack(1.0f);
    */

    // start to draw scene
    pRenderer->BeginScene(m_White, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind shader program
    m_Shader.Use(true);

    // get perspective (or projection) matrix slot from shader
    GLint uniform = glGetUniformLocation(m_Shader.GetProgramID(),
                                         "qr_uPerspective");

    // found it?
    if (uniform == -1)
        M_THROW_EXCEPTION("Program uniform not found - perspective");

    // connect perspective (or projection) matrix to shader
    glUniformMatrix4fv(uniform, 1, GL_FALSE, m_ProjectionMatrix.GetPtr());

    // get view (or camera) matrix slot from shader
    uniform = glGetUniformLocation(m_Shader.GetProgramID(), "qr_uCamera");

    // found it?
    if (uniform == -1)
        M_THROW_EXCEPTION("Program uniform not found - camera");

    // connect view (or camera) matrix to shader
    glUniformMatrix4fv(uniform, 1, GL_FALSE, m_ViewMatrix.GetPtr());

    // get model matrix slot from shader
    uniform = glGetUniformLocation(m_Shader.GetProgramID(), "qr_uModel");

    // found it?
    if (uniform == -1)
        M_THROW_EXCEPTION("Program uniform not found - model");

    // get and connect model matrix to shader
    QR_Matrix16P modelMatrix = m_Mesh.GetMatrix();
    glUniformMatrix4fv(uniform, 1, GL_FALSE, modelMatrix.GetPtr());

    // set cull face to front (MD2 models are built using this specificity)
    glCullFace(GL_FRONT);

    // draw vertex buffer using shader
    pRenderer->Draw(&m_Mesh,
                    QR_MD2::IE_L_None,
                    m_ModelLight,
                    &m_ShaderData);

    // set cull face to back again
    glCullFace(GL_BACK);

    // unbind shader program
    m_Shader.Use(false);

    // close and present the scene
    pRenderer->EndScene();
}
//------------------------------------------------------------------------------
