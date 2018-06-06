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

        CSR_Scene* GetScene() const;
        void SetScene(CSR_Scene* pScene);

        int GetOrigin() const;
        void SetOrigin(int origin);

        float GetRatio() const;
        void SetRatio(float ratio);

    protected:
        /**
        * Called when a Windows message is sent to the designer view
        *@param pControl - hooked designer view control
        *@param message - Windows message
        *@param fCtrlOriginalProc - control original Windows procedure
        *@return true if the message was resolved and should no longer be handled, otherwise false
        */
        virtual bool OnViewMessage(TControl* pControl, TMessage& message, TWndMethod fCtrlOriginalProc);

        virtual void Draw(const CSR_Scene*   pScene,
                          const TPoint&      origin,
                          const CSR_Vector2& pos,
                                float        ratio,
                                HDC          hDC) const;

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
        IGridOptions                      m_GridOptions;
        float                             m_Ratio;
};

#endif
