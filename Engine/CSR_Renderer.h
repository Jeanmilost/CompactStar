/****************************************************************************
 * ==> CSR_Renderer --------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the functions to draw a scene         *
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

#ifndef CSR_RendererH
#define CSR_RendererH

// compactStar engine
#include "CSR_Models.h"
#include "CSR_Shader.h"

#ifdef __cplusplus
    extern "C"
    {
#endif
        //-------------------------------------------------------------------
        // Scene functions
        //-------------------------------------------------------------------

        /**
        * Begins to draw a scene
        *@param r - scene background color red component
        *@param g - scene background color green component
        *@param b - scene background color blue component
        *@param a - scene background color alpha component
        */
        void csrSceneBegin(float r, float g, float b, float a);

        /**
        * Ends to draw a scene
        */
        void csrSceneEnd();

        /**
        * Draws a mesh in a scene
        *@param pMesh - mesh to draw
        *@param pShader - shader to use to draw the mesh
        */
        void csrSceneDrawMesh(const CSR_Mesh* pMesh, CSR_Shader* pShader);

#ifdef __cplusplus
    }
#endif

//---------------------------------------------------------------------------
// Compiler
//---------------------------------------------------------------------------

// needed in mobile c compiler to link the .h file with the .c
#if defined(_OS_IOS_) || defined(_OS_ANDROID_) || defined(_OS_WINDOWS_)
    #include "CSR_Renderer.c"
#endif

#endif
