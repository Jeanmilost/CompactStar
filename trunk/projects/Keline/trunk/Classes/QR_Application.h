/******************************************************************************
 * ==> QR_Application --------------------------------------------------------*
 ******************************************************************************
 * Description : Application manager                                          *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_APPLICATION_H
#define QR_APPLICATION_H

// libraries
#define GLEW_STATIC
#include <GL\glew.h>

// qr engine
#include "QR_MD2.h"
#include "QR_Renderer_OpenGL.h"
#include "QR_Window.h"
#include "QR_Camera_OpenGL.h"
#include "QR_Shader_OpenGL.h"
#include "QR_MSAA_OpenGL_Effect.h"
#include "QR_DirectInput.h"

/**
* Application manager
*@note This class IS NOT cross-platform
*@author Jean-Milost Reymond
*/
class QR_Application
{
    public:
        QR_Application();
        virtual ~QR_Application();

        /**
        * Executes application
        *@param hInstance - application instance
        *@param appName - application name
        *@return application result
        */
        static int Execute(HINSTANCE hInstance, const std::string appName);

    private:
        static QR_Color                        m_Black;
        static QR_Color                        m_White;
        static QR_Camera_OpenGL                m_Camera;
        static QR_MD2                          m_Mesh;
        static QR_MD2::ILight                  m_ModelLight;
        static QR_Shader_OpenGL                m_Shader;        static QR_Renderer_OpenGL::IShaderData m_ShaderData;
        static QR_Matrix16P                    m_ProjectionMatrix;
        static QR_Matrix16P                    m_ViewMatrix;
        static QR_DirectInput                  m_DirectInput;
        static M_Precision                     m_2PI;
        static QR_MSAA_OpenGL_Effect*          m_pMSAA;

        /**
        * Creates 3D scene
        *@param pWindow - window in which scene will be rendered
        *@param pRenderer - scene renderer to use
        */
        static void CreateScene(const QR_Window*   pWindow,
                                      QR_Renderer* pRenderer);

        /**
        * Called when renderer was initialized and should be configured
        *@param pRenderer - event sender
        */
        static void OnConfigure(const QR_Renderer* pRenderer);

        /**
        * Called when a generic attribute should be associated with a named variable
        *@param programID - shader program identifier
        *@param type - shader type
        */
        static void OnBindAttribute(GLuint programID, GLenum type);

        /**
        * Called when scene should be drawn
        *@param pWindow - window in which scene should be drawn
        *@param pRenderer - application renderer
        *@param elapsedTime - elapsed time since last draw
        */
        static void OnDrawScene(const QR_Window*    pWindow,
                                const QR_Renderer*  pRenderer,
                                const std::clock_t& elapsedTime);
};

#endif // QR_APPLICATION_H
