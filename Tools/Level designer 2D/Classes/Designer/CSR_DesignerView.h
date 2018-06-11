/****************************************************************************
 * ==> CSR_DesignerHelper --------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a view on which the designer content  *
 *               may be drawn                                               *
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

#ifndef CSR_DesignerViewH
#define CSR_DesignerViewH

// vcl
#include <Vcl.ExtCtrls.hpp>

// std
#include <memory>

// classes
#include "CSR_VCLHelper.h"
#include "CSR_View.h"

// compactStar engine
#include "CSR_Geometry.h"
#include "CSR_Scene.h"

/**
* Designer view
*@author Jean-Milost Reymond
*/
class CSR_DesignerView : public CSR_View
{
    public:
        /**
        * Constructor
        *@param pPanel - panel owning the view
        */
        CSR_DesignerView(TPanel* pPanel);

        virtual ~CSR_DesignerView();

        /**
        * Gets the current scene
        *@return the scene
        */
        CSR_Scene* GetScene() const;

        /**
        * Sets the current scene
        *@param pScene - scene
        */
        void SetScene(CSR_Scene* pScene);

        /**
        * Gets the currently selected scene item key
        *@return the selected scene item key
        */
        void* GetSelectedKey() const;

        /**
        * Sets the currently selected scene item key
        *@param pKey - scene item key
        */
        void SetSelectedKey(void* pKey);

        /**
        * Selects the previous model in the scene
        */
        void SelectPrev();

        /**
        * Selects the next model in the scene
        */
        void SelectNext();

        /**
        * Gets the view origin
        *@return the view origin, in pixels
        */
        int GetOrigin() const;

        /**
        * Sets the view origin
        *@param origin - the view origin, in pixels
        */
        void SetOrigin(int origin);

        /**
        * Gets the ratio to apply between the view and the scene
        *@return the view ratio
        */
        float GetRatio() const;

        /**
        * Sets the ratio to apply between the view and the scene
        *@param ratio - the view ratio
        */
        void SetRatio(float ratio);

        /**
        * Gets the model color
        *@return the model color
        */
        TColor GetModelColor() const;

        /**
        * Sets the model color
        *@param color - the model color
        */
        void SetModelColor(TColor color);

        /**
        * Gets the selected model color
        *@return the selected model color
        */
        TColor GetSelectedModelColor() const;

        /**
        * Sets the selected model color
        *@param color - the selected model color
        */
        void SetSelectedModelColor(TColor color);

    protected:
        /**
        * Called when a Windows message is sent to the designer view
        *@param pControl - hooked designer view control
        *@param message - Windows message
        *@param fCtrlOriginalProc - control original Windows procedure
        *@return true if the message was resolved and should no longer be handled, otherwise false
        */
        virtual bool OnViewMessage(TControl* pControl, TMessage& message, TWndMethod fCtrlOriginalProc);

        /**
        * Draws the scene content onto the view
        *@param pScene - scene to draw
        *@param origin - view origin in pixels
        *@param pos - position inside the 3d world
        *@param ratio - ratio to apply between the view and the scene
        *@param hDC - view device context on which the scene should be drawn
        */
        virtual void Draw(const CSR_Scene*   pScene,
                          const TPoint&      origin,
                          const CSR_Vector2& pos,
                                float        ratio,
                                HDC          hDC) const;

        /**
        * Draws a scene item content onto the view
        *@param pSceneItem - scene item to draw
        *@param origin - view origin in pixels
        *@param pos - position inside the 3d world
        *@param ratio - ratio to apply between the view and the scene
        *@param hDC - view device context on which the scene should be drawn
        */
        virtual void DrawItem(const CSR_SceneItem* pSceneItem,
                              const TPoint&        origin,
                              const CSR_Vector2&   pos,
                                    float          ratio,
                                    HDC            hDC) const;

        /**
        * Draws a model onto the view
        *@param pModelItem - model to draw
        *@param modelType - model type
        *@param pMatrix - model matrix
        *@param origin - view origin in pixels
        *@param pos - position inside the 3d world
        *@param ratio - ratio to apply between the view and the scene
        *@param hDC - view device context on which the scene should be drawn
        */
        virtual void DrawModel(const void*          pModelItem,
                                     CSR_EModelType modelType,
                               const CSR_Matrix4*   pMatrix,
                               const TPoint&        origin,
                               const CSR_Vector2&   pos,
                                     float          ratio,
                                     HDC            hDC) const;

        /**
        * Draws a polygon onto the view
        *@param origin - view origin in pixels
        *@param pos - position inside the 3d world
        *@param pMatrix - model matrix
        *@param polygon - model polygon to draw
        *@param ratio - ratio to apply between the view and the scene
        *@param hDC - view device context on which the scene should be drawn
        */
        virtual void DrawPolygon(const TPoint&       origin,
                                 const CSR_Vector2&  pos,
                                 const CSR_Matrix4*  pMatrix,
                                 const CSR_Polygon3& polygon,
                                       float         ratio,
                                       HDC           hDC) const;

    private:
        CSR_Scene*                        m_pScene;
        TPanel*                           m_pPanel;
        std::auto_ptr<CSR_VCLControlHook> m_pHook;
        std::auto_ptr<TBitmap>            m_pOverlay;
        void*                             m_pSelectedKey;
        TColor                            m_ModelColor;
        TColor                            m_SelectedModelColor;
        HBRUSH                            m_hBrush;
        HPEN                              m_hPen;
        HBRUSH                            m_hSelectedBrush;
        HPEN                              m_hSelectedPen;
        IGridOptions                      m_GridOptions;
        float                             m_Ratio;
};

#endif
