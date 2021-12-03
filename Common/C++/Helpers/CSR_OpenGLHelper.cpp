/****************************************************************************
 * ==> CSR_OpenGLHelper ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides an helper class for OpenGL            *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2022, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#include "CSR_OpenGLHelper.h"

// std
#include <math.h>
#include <memory>
#include <stdexcept>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Geometry.h"

// visual studio specific code
#ifdef _MSC_VER
    // std
    #define _USE_MATH_DEFINES
    #include <math.h>
#endif

// code specific for all compilers but not for visual studio
#ifndef _MSC_VER
    #pragma package(smart_init)
    #ifdef __llvm__
        #pragma link "glewSL.a"
    #else
        #pragma link "glewSL.lib"
    #endif
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
    ::ReleaseDC(hwnd, hDC);
}
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::CreateViewport(float             w,
                                      float             h,
                                      float             zNear,
                                      float             zFar,
                                const CSR_OpenGLShader* pShader,
                                      CSR_Matrix4&      matrix)
{
    if (!pShader)
        return;

    csrShaderEnable(pShader);

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
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

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

    #ifdef _MSC_VER
        float       angle         = 0.0f;
        CSR_Vector3 r             = {};
        CSR_Matrix4 rotateMatrixX = {};
    #else
        float       angle;
        CSR_Vector3 r;
        CSR_Matrix4 rotateMatrixX;
    #endif

    // set rotation axis
    r.m_X = 1.0f;
    r.m_Y = 0.0f;
    r.m_Z = 0.0f;

    // set rotation angle
    angle = float(rotated ? -M_PI * 0.5 : 0.0);

    csrMat4Rotate(angle, &r, &rotateMatrixX);

    CSR_Matrix4 rotateMatrixY;

    // set rotation axis
    r.m_X = 0.0f;
    r.m_Y = 1.0f;
    r.m_Z = 0.0f;

    // set rotation angle
    angle = float(rotated ? -M_PI * 0.25 : M_PI * 0.25);

    csrMat4Rotate(angle, &r, &rotateMatrixY);

    CSR_Matrix4 rotateMatrixZ;

    // set rotation axis
    r.m_X = 0.0f;
    r.m_Y = 0.0f;
    r.m_Z = 1.0f;

    // set rotation angle
    angle = 0.0f;

    csrMat4Rotate(angle, &r, &rotateMatrixZ);

    #ifdef _MSC_VER
        CSR_Vector3 factor      = {};
        CSR_Matrix4 scaleMatrix = {};
    #else
        CSR_Vector3 factor;
        CSR_Matrix4 scaleMatrix;
    #endif

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

    #ifdef _MSC_VER
    CSR_Box rotatedBox = {};
    #else
        CSR_Box rotatedBox;
    #endif
    csrMat4ApplyToVector(&combinedMatrix3, &pBox->m_Min, &rotatedBox.m_Min);
    csrMat4ApplyToVector(&combinedMatrix3, &pBox->m_Max, &rotatedBox.m_Max);

    // calculate the box size (on the x and y axis
    #ifdef __CODEGEARC__
        const float x = std::fabs(rotatedBox.m_Max.m_X - rotatedBox.m_Min.m_X);
        const float y = std::fabs(rotatedBox.m_Max.m_Y - rotatedBox.m_Min.m_Y);
    #else
        const float x = fabsf(rotatedBox.m_Max.m_X - rotatedBox.m_Min.m_X);
        const float y = fabsf(rotatedBox.m_Max.m_Y - rotatedBox.m_Min.m_Y);
    #endif

    // search for longest axis and the longest viewport edge
    #ifdef _MSC_VER
        const float longestAxis = max(x, y);
    #else
        const float longestAxis = std::max(x, y);
    #endif

    // Calculate the camera distance
    #ifdef __CODEGEARC__
        const float distance = std::fabs(longestAxis / std::sinf(fov / 2.0f)) * 0.25f;
    #else
        const float distance = fabsf(longestAxis / sinf(fov / 2.0f)) * 0.25f;
    #endif

    #ifdef _MSC_VER
        CSR_Vector3 t               = {};
        CSR_Matrix4 translateMatrix = {};
    #else
        CSR_Vector3 t;
        CSR_Matrix4 translateMatrix;
    #endif

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
void CSR_OpenGLHelper::BuildMatrix(const CSR_Vector3* pTranslation,
                                   const CSR_Vector3* pRotation,
                                   const CSR_Vector3* pScaling,
                                         CSR_Matrix4* pMatrix)
{
    if (!pTranslation || !pRotation || !pScaling || !pMatrix)
        return;

    #ifdef _MSC_VER
        CSR_Vector3 axis            = {};
        CSR_Matrix4 matrixTranslate = {};
        CSR_Matrix4 matrixX         = {};
        CSR_Matrix4 matrixY         = {};
        CSR_Matrix4 matrixZ         = {};
        CSR_Matrix4 matrixScale     = {};
        CSR_Matrix4 buildMatrix1    = {};
        CSR_Matrix4 buildMatrix2    = {};
        CSR_Matrix4 buildMatrix3    = {};
    #else
        CSR_Vector3 axis;
        CSR_Matrix4 matrixTranslate;
        CSR_Matrix4 matrixX;
        CSR_Matrix4 matrixY;
        CSR_Matrix4 matrixZ;
        CSR_Matrix4 matrixScale;
        CSR_Matrix4 buildMatrix1;
        CSR_Matrix4 buildMatrix2;
        CSR_Matrix4 buildMatrix3;
    #endif

    // create a translation matrix
    csrMat4Translate(pTranslation, &matrixTranslate);

    // get the rotation x axis
    axis.m_X = 1.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 0.0f;

    // create a rotation matrix on the x axis
    csrMat4Rotate(pRotation->m_X, &axis, &matrixX);

    // get the rotation y axis
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    // create a rotation matrix on the y axis
    csrMat4Rotate(pRotation->m_Y, &axis, &matrixY);

    // get the rotation z axis
    axis.m_X = 0.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 1.0f;

    // create a rotation matrix on the z axis
    csrMat4Rotate(pRotation->m_Z, &axis, &matrixZ);

    // create a scale matrix
    csrMat4Scale(pScaling, &matrixScale);

    // rebuild the selected object model matrix
    csrMat4Multiply(&matrixScale,  &matrixX,         &buildMatrix1);
    csrMat4Multiply(&buildMatrix1, &matrixY,         &buildMatrix2);
    csrMat4Multiply(&buildMatrix2, &matrixZ,         &buildMatrix3);
    csrMat4Multiply(&buildMatrix3, &matrixTranslate, pMatrix);
}
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::AddTexture(const void* pKey, GLuint id, IResources& resources)
{
    // search a matching texture in the OpenGL resources
    CSR_OpenGLHelper::IResources::iterator it = resources.find((void*)pKey);

    // found one?
    if (it != resources.end())
    {
        if (it->second->m_ID != GLint(id))
            throw new std::range_error("The existing resource texture identifier differs from the identifier passed in arguments");

        // increase its use count
        ++it->second->m_UseCount;

        return;
    }

    // create and add a new texture identifier
    std::auto_ptr<CSR_OpenGLID> pID(new CSR_OpenGLID());
    csrOpenGLIDInit(pID.get());
    pID->m_pKey            = (void*)pKey;
    pID->m_ID              = id;
    pID->m_UseCount        = 1;
    resources[(void*)pKey] = pID.get();
    pID.release();
}
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::DeleteTexture(const void* pKey, IResources& resources)
{
    // search the matching texture in the OpenGL resources
    CSR_OpenGLHelper::IResources::iterator it = resources.find((void*)pKey);

    // found one?
    if (it == resources.end())
        return;

    // release one usage
    if (it->second->m_UseCount)
        --it->second->m_UseCount;

    // still used?
    if (it->second->m_UseCount)
        return;

    // texture was used?
    if (it->second->m_ID != GLint(M_CSR_Error_Code))
        // release it
        glDeleteTextures(1, (GLuint*)(&it->second->m_ID));

    // erase the resource (no longer used)
    delete it->second;
    resources.erase(it);
}
//---------------------------------------------------------------------------
void* CSR_OpenGLHelper::GetTextureID(const void* pKey, IResources& resources)
{
    // search the matching texture in the OpenGL resources
    CSR_OpenGLHelper::IResources::const_iterator it = resources.find((void*)pKey);

    // found it?
    if (it != resources.end())
        return it->second;

    return NULL;
}
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::ClearResources(IResources& resources)
{
    // delete the scene textures
    for (CSR_OpenGLHelper::IResources::iterator it = resources.begin(); it != resources.end(); ++it)
        delete it->second;

    resources.clear();
}
//---------------------------------------------------------------------------
