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
// CSR_OpenGLHelper::IContext
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContext::IContext() :
    m_hDC(NULL),
    m_hRC(NULL)
{}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContext::~IContext()
{}
//---------------------------------------------------------------------------
// CSR_OpenGLHelper::IContextIterator
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContextIterator::IContextIterator()
{}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContextIterator::IContextIterator(IContextDictionary::iterator it) :
    m_Iterator(it)
{}
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::IContextIterator::Set(IContextDictionary::iterator it)
{
    m_Iterator = it;
}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContextIterator& CSR_OpenGLHelper::IContextIterator::operator ++ ()
{
    ++m_Iterator;
    return *this;
}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContextIterator CSR_OpenGLHelper::IContextIterator::operator ++ (int)
{
    IContextDictionary::iterator last = m_Iterator;
    ++m_Iterator;
    return last;
}
//---------------------------------------------------------------------------
bool CSR_OpenGLHelper::IContextIterator::operator == (const IContextIterator& it) const
{
    return m_Iterator == it.m_Iterator;
}
//---------------------------------------------------------------------------
bool CSR_OpenGLHelper::IContextIterator::operator != (const IContextIterator& it) const
{
    return m_Iterator != it.m_Iterator;
}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContext* CSR_OpenGLHelper::IContextIterator::operator * ()
{
    return &m_Iterator->second;
}
//---------------------------------------------------------------------------
TWinControl* CSR_OpenGLHelper::IContextIterator::First() const
{
    return m_Iterator->first;
}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContext& CSR_OpenGLHelper::IContextIterator::Second() const
{
    return m_Iterator->second;
}
//---------------------------------------------------------------------------
// CSR_OpenGLHelper::IContextConstIterator
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContextConstIterator::IContextConstIterator()
{}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContextConstIterator::IContextConstIterator(IContextDictionary::const_iterator it) :
    m_Iterator(it)
{}
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::IContextConstIterator::Set(IContextDictionary::const_iterator it)
{
    m_Iterator = it;
}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContextConstIterator& CSR_OpenGLHelper::IContextConstIterator::operator ++ ()
{
    ++m_Iterator;
    return *this;
}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContextConstIterator CSR_OpenGLHelper::IContextConstIterator::operator ++ (int)
{
    IContextDictionary::const_iterator last = m_Iterator;
    ++m_Iterator;
    return last;
}
//---------------------------------------------------------------------------
bool CSR_OpenGLHelper::IContextConstIterator::operator == (const IContextConstIterator& it) const
{
    return m_Iterator == it.m_Iterator;
}
//---------------------------------------------------------------------------
bool CSR_OpenGLHelper::IContextConstIterator::operator != (const IContextConstIterator& it) const
{
    return m_Iterator != it.m_Iterator;
}
//---------------------------------------------------------------------------
const CSR_OpenGLHelper::IContext* CSR_OpenGLHelper::IContextConstIterator::operator * ()
{
    return &m_Iterator->second;
}
//---------------------------------------------------------------------------
const TWinControl* CSR_OpenGLHelper::IContextConstIterator::First() const
{
    return m_Iterator->first;
}
//---------------------------------------------------------------------------
const CSR_OpenGLHelper::IContext& CSR_OpenGLHelper::IContextConstIterator::Second() const
{
    return m_Iterator->second;
}
//---------------------------------------------------------------------------
// CSR_OpenGLHelper
//---------------------------------------------------------------------------
CSR_OpenGLHelper::CSR_OpenGLHelper()
{
    // NOTE the namespace is required to avoid to call a pure virtual function
    CSR_OpenGLHelper::ClearContexts();
}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::~CSR_OpenGLHelper()
{
    for (IContextDictionary::iterator it = m_Contexts.begin(); it != m_Contexts.end(); ++it)
        CSR_OpenGLHelper::DisableOpenGL(it->first->Handle, it->second.m_hDC, it->second.m_hRC);

    m_Contexts.clear();
}
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
    const float zNear  = 1.0f;
    const float zFar   = 1000.0f;
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
void CSR_OpenGLHelper::GetBitmapFromOpenGL(TBitmap* pBitmap)
{
    // no bitmap?
    if (!pBitmap)
        return;

    GLint dimensions[4];

    // get viewport dimensions
    glGetIntegerv(GL_VIEWPORT, dimensions);

    TRGBQuad* pPixels = NULL;

    try
    {
        // create bits to contain bitmap
        pPixels = new TRGBQuad[dimensions[2] * dimensions[3] * 4];

        // flush OpenGL
        glFinish();
        glPixelStorei(GL_PACK_ALIGNMENT,   4);
        glPixelStorei(GL_PACK_ROW_LENGTH,  0);
        glPixelStorei(GL_PACK_SKIP_ROWS,   0);
        glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

        // get pixels from last OpenGL rendering
        glReadPixels(0, 0, dimensions[2], dimensions[3], GL_RGBA, GL_UNSIGNED_BYTE, pPixels);

        // configure destination bitmap
        pBitmap->PixelFormat = pf32bit;
        pBitmap->SetSize(dimensions[2], dimensions[3]);

        // iterate through lines to copy
        for (GLint y = 0; y < dimensions[3]; ++y)
        {
            // get next line to copy and calculate y position (origin is on the left bottom on the
            // source, but on the left top on the destination)
            TRGBQuad*         pLine = static_cast<TRGBQuad*>(pBitmap->ScanLine[y]);
            const std::size_t yPos  = ((dimensions[3] - 1) - y) * dimensions[2];

            // iterate through pixels to copy
            for (GLint x = 0; x < dimensions[2]; ++x)
            {
                // take the opportunity to swap the pixel RGB values
                pLine[x].rgbRed      = pPixels[yPos + x].rgbBlue;
                pLine[x].rgbGreen    = pPixels[yPos + x].rgbGreen;
                pLine[x].rgbBlue     = pPixels[yPos + x].rgbRed;
                pLine[x].rgbReserved = pPixels[yPos + x].rgbReserved;
            }
        }
    }
    __finally
    {
        if (pPixels)
            delete[] pPixels;
    }
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
void CSR_OpenGLHelper::ResizeViews(const CSR_Shader* pShader, CSR_Matrix4& matrix) const
{
    if (!pShader)
        return;

    // iterate through views to resize
    for (IContextDictionary::const_iterator it = m_Contexts.begin(); it != m_Contexts.end(); ++it)
    {
        // enable view context
        wglMakeCurrent(it->second.m_hDC, it->second.m_hRC);

        // enable the shader
        csrShaderEnable(pShader);

        // update the viewport
        CreateViewport(it->first->ClientWidth, it->first->ClientHeight, pShader, matrix);
    }
}
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::AddContext(TWinControl* pControl)
{
    // no control?
    if (!pControl)
        return;

    // context was already created for this control?
    if (m_Contexts.find(pControl) != m_Contexts.end())
        return;

    IContext context;

    // enable OpenGL
    EnableOpenGL(pControl->Handle, &context.m_hDC, &context.m_hRC);

    // suceeded?
    if (!context.m_hDC || !context.m_hRC)
        return;

    // add it to context dictionary
    m_Contexts[pControl] = context;
}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContext* CSR_OpenGLHelper::GetContext(TWinControl* pControl)
{
    // no control?
    if (!pControl)
        return NULL;

    // get the context matching with the control
    IContextDictionary::iterator it = m_Contexts.find(pControl);

    // context was already created for this control?
    if (it == m_Contexts.end())
        return NULL;

    return &it->second;
}
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::DeleteContext(TWinControl* pControl)
{
    // no control?
    if (!pControl)
        return;

    // get the context matching with the control
    IContextDictionary::iterator it = m_Contexts.find(pControl);

    // context was already created for this control?
    if (it == m_Contexts.end())
        return;

    // disable OpenGL for the context to delete
    DisableOpenGL(it->first->Handle, it->second.m_hDC, it->second.m_hRC);

    // delete the context from the list
    m_Contexts.erase(it);
}
//---------------------------------------------------------------------------
void CSR_OpenGLHelper::ClearContexts()
{
    // iterate through all the contexts and disable OpenGL for each of them
    for (IContextDictionary::iterator it = m_Contexts.begin(); it != m_Contexts.end(); ++it)
        DisableOpenGL(it->first->Handle, it->second.m_hDC, it->second.m_hRC);

    m_Contexts.clear();
}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContextIterator CSR_OpenGLHelper::Begin()
{
    return m_Contexts.begin();
}
//---------------------------------------------------------------------------
const CSR_OpenGLHelper::IContextConstIterator CSR_OpenGLHelper::Begin() const
{
    return m_Contexts.begin();
}
//---------------------------------------------------------------------------
CSR_OpenGLHelper::IContextIterator CSR_OpenGLHelper::End()
{
    return m_Contexts.end();
}
//---------------------------------------------------------------------------
const CSR_OpenGLHelper::IContextConstIterator CSR_OpenGLHelper::End() const
{
    return m_Contexts.end();
}
//---------------------------------------------------------------------------
