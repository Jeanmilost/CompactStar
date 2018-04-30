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
        * OpenGL context
        */
        struct IContext
        {
            HDC   m_hDC;
            HGLRC m_hRC;

            IContext();
            virtual ~IContext();
        };

        typedef std::map<TWinControl*, IContext> IContextDictionary;

        /**
        * Iterator to access the registered OpenGL contexts
        *@note Working just like normal STL iterator
        */
        class IContextIterator
        {
            friend CSR_OpenGLHelper;

            public:
                IContextIterator();

                /**
                * Prefix operator ++
                *@return reference to itself
                */
                IContextIterator& operator ++ ();

                /**
                * Postfix operator ++
                *@return copy of itself
                */
                IContextIterator operator ++ (int);

                /**
                * Equality operator
                *@param it - other iterator to compare
                *@return true if equality, otherwise false
                */
                bool operator == (const IContextIterator& it) const;

                /**
                * Not equality operator
                *@param it - other iterator to compare
                *@return true if equality, otherwise false
                */
                bool operator != (const IContextIterator& it) const;

                /**
                * Operator *
                *@return object pointed by iterator
                */
                IContext* operator * ();

                /**
                * Gets the first value contained in the map item the iterator points to
                *@return the first value contained in the map item the iterator points to
                */
                TWinControl* First() const;

                /**
                * Gets the first value contained in the map item the iterator points to
                *@return the first value contained in the map item the iterator points to
                */
                IContext& Second() const;

            private:
                IContextDictionary::iterator m_Iterator;

                /**
                * Copy constructor
                */
                IContextIterator(IContextDictionary::iterator it);

                /**
                * Sets iterator
                *@param it - iterator to set
                */
                void Set(IContextDictionary::iterator it);
        };

        /**
        * Constant iterator to access the registered OpenGL contexts
        *@note Working just like normal STL constant iterator
        */
        class IContextConstIterator
        {
            friend CSR_OpenGLHelper;

            public:
                IContextConstIterator();

                /**
                * Prefix operator ++
                *@return reference to itself
                */
                IContextConstIterator& operator ++ ();

                /**
                * Postfix operator ++
                *@return copy of itself
                */
                IContextConstIterator operator ++ (int);

                /**
                * Equality operator
                *@param it - other iterator to compare
                *@return true if equality, otherwise false
                */
                bool operator == (const IContextConstIterator& it) const;

                /**
                * Not equality operator
                *@param it - other iterator to compare
                *@return true if equality, otherwise false
                */
                bool operator != (const IContextConstIterator& it) const;

                /**
                * Operator *
                *@return object pointed by iterator
                */
                const IContext* operator * ();

                /**
                * Gets the first value contained in the map item the iterator points to
                *@return the first value contained in the map item the iterator points to
                */
                const TWinControl* First() const;

                /**
                * Gets the first value contained in the map item the iterator points to
                *@return the first value contained in the map item the iterator points to
                */
                const IContext& Second() const;

            private:
                IContextDictionary::const_iterator m_Iterator;

                /**
                * Copy constructor
                */
                IContextConstIterator(IContextDictionary::const_iterator it);

                /**
                * Sets iterator
                *@param it - iterator to set
                */
                void Set(IContextDictionary::const_iterator it);
        };

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
        *@param pShader - shader linked with the viewport
        *@param[out] matrix - the projection matrix used for the viewport
        */
        static void CreateViewport(      float        w,
                                         float        h,
                                   const CSR_Shader*  pShader,
                                         CSR_Matrix4& matrix);

        /**
        * Exports the current OpenGL drawing in a bitmap
        *@param[in, out] pBitmap - bitmap in which the OpenGL scene will be exported
        */
        static void GetBitmapFromOpenGL(TBitmap* pBitmap);

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

        /**
        * Resizes all the views
        *@param pShader - shader linked with the view
        *@param[out] matrix - the projection matrix used for the viewport
        */
        virtual void ResizeViews(const CSR_Shader* pShader, CSR_Matrix4& matrix) const;

        /**
        * Adds an OpenGL context to a control
        *@param pControl - control for which the context should be added
        */
        virtual void AddContext(TWinControl* pControl);

        /**
        * Gets an OpenGL context matching with a control
        *@param pControl - control for which the context should be get
        *@return OpenGL context matching with the control, NULL if not found or on error
        */
        virtual CSR_OpenGLHelper::IContext* GetContext(TWinControl* pControl);

        /**
        * Deletes a context
        *@param pControl - control for which the context should be deleted
        */
        virtual void DeleteContext(TWinControl* pControl);

        /**
        * Clears all the registered OpenGL contexts
        */
        virtual void ClearContexts();

        /**
        * Gets the iterator of the first context in the dictionary
        *@return first context iterator in the dictionary
        */
              IContextIterator      Begin();
        const IContextConstIterator Begin() const;

        /**
        * Gets the iterator of the the last context in the dictionary
        *@return last context iterator in the dictionary
        */
              IContextIterator      End();
        const IContextConstIterator End() const;

    private:
        IContextDictionary m_Contexts;
};

#endif
