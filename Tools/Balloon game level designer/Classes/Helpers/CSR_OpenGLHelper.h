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

#ifndef CSR_OpenGLHelperH
#define CSR_OpenGLHelperH

// vcl
#include <Vcl.Controls.hpp>

// std
#include <map>

// compactStar engine
#include "CSR_Geometry.h"
#include "CSR_Collision.h"
#include "CSR_Shader.h"

/**
* Helper class for OpenGL
*@author Jean-Milost Reymond
*/
class CSR_OpenGLHelper
{
    public:
        CSR_OpenGLHelper();
        virtual ~CSR_OpenGLHelper();

        /**
        * Enables OpenGL
        *@param hWnd - Windows handle
        *@param hDC - device context
        *@param hRC - OpenGL rendering context
        */
        static void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC);

        /**
        * Disables OpenGL
        *@param hWnd - Windows handle
        *@param hDC - device context
        *@param hRC - OpenGL rendering context
        */
        static void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);

        /**
        * Creates the viewport
        *@param w - viewport width
        *@param h - viewport height
        *@param zNear - near clipping plane value
        *@param zFar - far clipping plane value
        *@param pShader - shader linked with the viewport
        *@param[out] matrix - the projection matrix used for the viewport
        */
        static void CreateViewport(      float        w,
                                         float        h,
                                         float        zNear,
                                         float        zFar,
                                   const CSR_Shader*  pShader,
                                         CSR_Matrix4& matrix);

        /**
        * Gets a model matrix which make a model to fit the viewport (as closest as possible)
        *@param pBox - model bounding box, if 0 an identity matrix will be returned
        *@param fov - vield of view used in the viewport, in radians
        *@param rotated - if true, the model is rotated by default of 90° on the x axis (this is
        *                 generally the case for the MDL models)
        *@return the model matrix
        *@note Using this function, the model will be centered in the scene, with a normalized size
        *      and a rotation of 45° on the y axis
        */
        static CSR_Matrix4 FitModelInView(const CSR_Box* pBox, float fov, bool rotated);
};

#endif
