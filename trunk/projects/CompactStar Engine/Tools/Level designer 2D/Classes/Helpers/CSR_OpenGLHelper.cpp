/****************************************************************************
 * ==> CSR_OpenGLHelper ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides an helper class for OpenGL            *
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

#include "CSR_OpenGLHelper.h"

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"

#pragma package(smart_init)
#ifdef __llvm__
    #pragma link "glewSL.a"
#else
    #pragma link "glewSL.lib"
#endif

//---------------------------------------------------------------------------
// CSR_OpenGLHelper
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC)
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
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::DisableOpenGL(HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::CreateViewport(      float        w,
                                            float        h,
                                            float        zNear,
                                            float        zFar,
                                      const CSR_Shader*  pShader,
                                            CSR_Matrix4& matrix)
{
    if (!pShader)
        return;

    // prevent the width to reach 0
    if (!w)
        w = 0.1f;

    // prevent the height to reach 0
    if (!h)
        h = 0.1f;

    // calculate matrix items
    const float fov    = 45.0f;
    const float aspect = w / h;

    // create the OpenGL viewport
    glViewport(0, 0, w, h);

    // create the projection matrix
    csrMat4Perspective(fov, aspect, zNear, zFar, &matrix);

    // connect projection matrix to shader
    const GLint projectionUniform = glGetUniformLocation(pShader->m_ProgramID, "csr_uProjection");
    glUniformMatrix4fv(projectionUniform, 1, 0, &matrix.m_Table[0][0]);
}
//---------------------------------------------------------------------------
CSR_Matrix4 CSR_OpenGLHelper::FitModelInView(const CSR_Box* pBox, float fov, bool rotated)
{
    CSR_Matrix4 matrix;

    if (!pBox)
    {
        csrMat4Identity(&matrix);
        return matrix;
    }

    float       angle;
    CSR_Vector3 r;
    CSR_Matrix4 rotateMatrixX;

    // set rotation axis
    r.m_X = 1.0f;
    r.m_Y = 0.0f;
    r.m_Z = 0.0f;

    // set rotation angle
    angle = rotated ? -M_PI * 0.5f : 0.0f;

    csrMat4Rotate(angle, &r, &rotateMatrixX);

    CSR_Matrix4 rotateMatrixY;

    // set rotation axis
    r.m_X = 0.0f;
    r.m_Y = 1.0f;
    r.m_Z = 0.0f;

    // set rotation angle
    angle = rotated ? -M_PI * 0.25f : M_PI * 0.25f;

    csrMat4Rotate(angle, &r, &rotateMatrixY);

    CSR_Matrix4 rotateMatrixZ;

    // set rotation axis
    r.m_X = 0.0f;
    r.m_Y = 0.0f;
    r.m_Z = 1.0f;

    // set rotation angle
    angle = 0.0f;

    csrMat4Rotate(angle, &r, &rotateMatrixZ);

    CSR_Vector3 factor;
    CSR_Matrix4 scaleMatrix;

    // set scale factor
    factor.m_X = 1.0f;
    factor.m_Y = 1.0f;
    factor.m_Z = 1.0f;

    csrMat4Scale(&factor, &scaleMatrix);

    CSR_Matrix4 combinedMatrix1;
    CSR_Matrix4 combinedMatrix2;
    CSR_Matrix4 combinedMatrix3;

    // calculate the intermediate matrix (on which only the rotation and scaling were applied)
    csrMat4Multiply(&scaleMatrix,     &rotateMatrixX, &combinedMatrix1);
    csrMat4Multiply(&combinedMatrix1, &rotateMatrixY, &combinedMatrix2);
    csrMat4Multiply(&combinedMatrix2, &rotateMatrixZ, &combinedMatrix3);

    CSR_Box rotatedBox;
    csrMat4ApplyToVector(&combinedMatrix3, &pBox->m_Min, &rotatedBox.m_Min);
    csrMat4ApplyToVector(&combinedMatrix3, &pBox->m_Max, &rotatedBox.m_Max);

    // calculate the box size (on the x and y axis
    const float x = std::fabs(rotatedBox.m_Max.m_X - rotatedBox.m_Min.m_X);
    const float y = std::fabs(rotatedBox.m_Max.m_Y - rotatedBox.m_Min.m_Y);

    // search for longest axis and the longest viewport edge
    const float longestAxis = std::max(x, y);

    // Calculate the camera distance
    const float distance = std::fabs(longestAxis / std::sinf(fov / 2.0f)) * 0.25f;

    CSR_Vector3 t;
    CSR_Matrix4 translateMatrix;

    // set translation
    t.m_X = -(rotatedBox.m_Min.m_X + rotatedBox.m_Max.m_X) / 2.0f;
    t.m_Y = -(rotatedBox.m_Min.m_Y + rotatedBox.m_Max.m_Y) / 2.0f;
    t.m_Z = -distance;

    csrMat4Translate(&t, &translateMatrix);

    // calculate the final model view matrix
    csrMat4Multiply(&combinedMatrix3, &translateMatrix, &matrix);

    return matrix;
}
//---------------------------------------------------------------------------
