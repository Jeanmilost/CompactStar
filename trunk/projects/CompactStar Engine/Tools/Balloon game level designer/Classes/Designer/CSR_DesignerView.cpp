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

#include "CSR_DesignerView.h"

// std
#include <vector>

//---------------------------------------------------------------------------
// CSR_DesignerView
//---------------------------------------------------------------------------
CSR_DesignerView::CSR_DesignerView(TPanel* pPanel) :
    CSR_View(),
    m_pScene(NULL),
    m_pPanel(pPanel),
    m_Ratio(1.0f)
{
    // hook the view panel Windows procedure
    m_pHook.reset(new CSR_VCLControlHook(m_pPanel, OnViewMessage));
}
//---------------------------------------------------------------------------
CSR_DesignerView::~CSR_DesignerView()
{
    // restore the previous view panel Windows procedure
    m_pHook.reset();
}
//---------------------------------------------------------------------------
CSR_Scene* CSR_DesignerView::GetScene() const
{
    return m_pScene;
}
//---------------------------------------------------------------------------
void CSR_DesignerView::SetScene(CSR_Scene* pScene)
{
    m_pScene = pScene;
}
//---------------------------------------------------------------------------
int CSR_DesignerView::GetOrigin() const
{
    return m_GridOptions.m_Origin.X;
}
//---------------------------------------------------------------------------
void CSR_DesignerView::SetOrigin(int origin)
{
    m_GridOptions.m_Origin.X = origin;
}
//---------------------------------------------------------------------------
float CSR_DesignerView::GetRatio() const
{
    return m_Ratio;
}
//---------------------------------------------------------------------------
void CSR_DesignerView::SetRatio(float ratio)
{
    m_Ratio = ratio;
}
//---------------------------------------------------------------------------
bool CSR_DesignerView::OnViewMessage(TControl* pControl, TMessage& message, TWndMethod fCtrlOriginalProc)
{
    switch (message.Msg)
    {
        case WM_ERASEBKGND:
            // do nothing, the background will be fully repainted later
            message.Result = 1L;
            return true;

        case WM_PAINT:
        {
            TPanel* pPanel = dynamic_cast<TPanel*>(pControl);

            if (!pPanel)
                return false;

            ::PAINTSTRUCT ps;
            HDC           hDC;

            try
            {
                // begin paint
                hDC = ::BeginPaint(pPanel->Handle, &ps);

                // succeeded?
                if (!hDC)
                    return false;

                // create an overlay bitmap if still not done
                if (!m_pOverlay.get())
                    m_pOverlay.reset(new TBitmap());

                // check if overlay size should be updated, update it if needed
                if (m_pOverlay->Width  != m_pPanel->ClientWidth ||
                    m_pOverlay->Height != m_pPanel->ClientHeight)
                    m_pOverlay->SetSize(m_pPanel->ClientWidth, m_pPanel->ClientHeight);

                // draw the background grid
                DrawGrid(pControl->ClientRect, m_GridOptions, m_pOverlay->Canvas->Handle);

                // get the current landscape position from the view matrix
                CSR_Vector2 pos;
                pos.m_X = m_pScene->m_Matrix.m_Table[3][0];
                pos.m_Y = 0.0f;

                // draw the scene on the overlay
                Draw(m_pScene,
                     TPoint(m_pPanel->ClientWidth  / 2,
                            m_pPanel->ClientHeight / 2),
                     pos,
                     m_Ratio,
                     m_pOverlay->Canvas->Handle);

                // copy the overlay content onto the designer view
                ::BitBlt(hDC, 0, 0, m_pPanel->ClientWidth, m_pPanel->ClientHeight,
                        m_pOverlay->Canvas->Handle, 0, 0, SRCCOPY);
            }
            __finally
            {
                // end paint
                if (hDC)
                    ::EndPaint(pPanel->Handle, &ps);
            }

            // validate entire client rect (it has just been completely redrawn)
            ::ValidateRect(pPanel->Handle, NULL);

            // notify main Windows procedure that the view was repainted
            message.Result = 0L;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void CSR_DesignerView::Draw(const CSR_Scene*   pScene,
                            const TPoint&      origin,
                            const CSR_Vector2& pos,
                                  float        ratio,
                                  HDC          hDC) const
{
    if (!pScene)
        return;

    HBRUSH hBrush = NULL;
    HPEN   hPen   = NULL;

    try
    {
        // create the brush and pen to use
        hBrush = ::CreateSolidBrush(Graphics::ColorToRGB(TColor(0x555555)));
        hPen   = ::CreatePen(PS_SOLID, 1, Graphics::ColorToRGB(TColor(0x555555)));

        if (!hBrush || !hPen)
            return;

        // select the brush and pen to use
        ::SelectObject(hDC, hBrush);
        ::SelectObject(hDC, hPen);

        // select the background mode to apply
        ::SetBkMode(hDC, OPAQUE);

        // iterate through scene items
        for (std::size_t i = 0; i < pScene->m_ItemCount; ++i)
            // iterate through item model matrices
            for (std::size_t j = 0; j < pScene->m_pItem[i].m_pMatrixArray->m_Count; ++j)
                // search for item model type
                switch (pScene->m_pItem[i].m_Type)
                {
                    case CSR_MT_Mesh:
                    {
                        // item contains a mesh, convert it
                        CSR_Mesh* pMesh = static_cast<CSR_Mesh*>(pScene->m_pItem[i].m_pModel);

                        // found it?
                        if (!pMesh)
                            continue;

                        // iterate through model meshes
                        CSR_IndexedPolygonBuffer* pBuffer = NULL;

                        try
                        {
                            // extract the mesh polygons
                            pBuffer = csrIndexedPolygonBufferFromMesh(pMesh);

                            // succeeded?
                            if (!pBuffer)
                                continue;

                            // iterate through polygons
                            for (std::size_t k = 0; k < pBuffer->m_Count; ++k)
                            {
                                CSR_Polygon3 polygon;

                                // get the next polygon to draw
                                if (!csrIndexedPolygonToPolygon(&pBuffer->m_pIndexedPolygon[k], &polygon))
                                    continue;

                                // draw the polygon
                                DrawPolygon(origin,
                                            pos,
                                            static_cast<CSR_Matrix4*>(pScene->m_pItem[i].m_pMatrixArray->m_pItem[j].m_pData),
                                            polygon,
                                            ratio,
                                            hDC);
                            }
                        }
                        __finally
                        {
                            csrIndexedPolygonBufferRelease(pBuffer);
                        }

                        continue;
                    }

                    case CSR_MT_Model:
                    {
                        // item contains a model, convert it
                        CSR_Model* pModel = static_cast<CSR_Model*>(pScene->m_pItem[i].m_pModel);

                        // found it?
                        if (!pModel)
                            continue;

                        // iterate through model meshes
                        for (std::size_t k = 0; k < pModel->m_MeshCount; ++k)
                        {
                            CSR_IndexedPolygonBuffer* pBuffer = NULL;

                            try
                            {
                                // extract the mesh polygons
                                pBuffer = csrIndexedPolygonBufferFromMesh(&pModel->m_pMesh[k]);

                                // succeeded?
                                if (!pBuffer)
                                    continue;

                                // iterate through polygons
                                for (std::size_t l = 0; l < pBuffer->m_Count; ++l)
                                {
                                    CSR_Polygon3 polygon;

                                    // get the next polygon to draw
                                    if (!csrIndexedPolygonToPolygon(&pBuffer->m_pIndexedPolygon[l], &polygon))
                                        continue;

                                    // draw the polygon
                                    DrawPolygon(origin,
                                                pos,
                                                static_cast<CSR_Matrix4*>(pScene->m_pItem[i].m_pMatrixArray->m_pItem[j].m_pData),
                                                polygon,
                                                ratio,
                                                hDC);
                                }
                            }
                            __finally
                            {
                                csrIndexedPolygonBufferRelease(pBuffer);
                            }
                        }

                        continue;
                    }

                    case CSR_MT_MDL:
                    {
                        // item contains a Quake I model, convert it
                        CSR_MDL* pMDL = static_cast<CSR_MDL*>(pScene->m_pItem[i].m_pModel);

                        // found it?
                        if (!pMDL)
                            continue;

                        // is empty?
                        if (!pMDL->m_ModelCount)
                            continue;

                        // get the first model (enough to be shown on the view)
                        CSR_Model* pModel = &pMDL->m_pModel[0];

                        // found it?
                        if (!pModel)
                            continue;

                        // iterate through model meshes
                        for (std::size_t k = 0; k < pModel->m_MeshCount; ++k)
                        {
                            CSR_IndexedPolygonBuffer* pBuffer = NULL;

                            try
                            {
                                // extract the mesh polygons
                                pBuffer = csrIndexedPolygonBufferFromMesh(&pModel->m_pMesh[k]);

                                // succeeded?
                                if (!pBuffer)
                                    continue;

                                // iterate through polygons
                                for (std::size_t l = 0; l < pBuffer->m_Count; ++l)
                                {
                                    CSR_Polygon3 polygon;

                                    // get the next polygon to draw
                                    if (!csrIndexedPolygonToPolygon(&pBuffer->m_pIndexedPolygon[l], &polygon))
                                        continue;

                                    // draw the polygon
                                    DrawPolygon(origin,
                                                pos,
                                                static_cast<CSR_Matrix4*>(pScene->m_pItem[i].m_pMatrixArray->m_pItem[j].m_pData),
                                                polygon,
                                                ratio,
                                                hDC);
                                }
                            }
                            __finally
                            {
                                csrIndexedPolygonBufferRelease(pBuffer);
                            }
                        }

                        continue;
                    }
                }
    }
    __finally
    {
        if (hPen)
            ::DeleteObject(hPen);

        if (hBrush)
            ::DeleteObject(hBrush);
    }
}
//---------------------------------------------------------------------------
void CSR_DesignerView::DrawPolygon(const TPoint&       origin,
                                   const CSR_Vector2&  pos,
                                   const CSR_Matrix4*  pMatrix,
                                   const CSR_Polygon3& polygon,
                                         float         ratio,
                                         HDC           hDC) const
{
    // apply the model martix to polygon vertices
    CSR_Vector3 v[3];
    csrMat4Transform(pMatrix, &polygon.m_Vertex[0], &v[0]);
    csrMat4Transform(pMatrix, &polygon.m_Vertex[1], &v[1]);
    csrMat4Transform(pMatrix, &polygon.m_Vertex[2], &v[2]);

    // transform the polygon points into view. NOTE vertex 1 and 2 are crossed, otherwise several
    // polygons may not appear on the edges
    std::vector<POINT> points;
    points.push_back(TPoint(origin.X + ((v[0].m_X + pos.m_X) * ratio), origin.Y + ((v[0].m_Z + pos.m_Y) * ratio)));
    points.push_back(TPoint(origin.X + ((v[2].m_X + pos.m_X) * ratio), origin.Y + ((v[2].m_Z + pos.m_Y) * ratio)));
    points.push_back(TPoint(origin.X + ((v[1].m_X + pos.m_X) * ratio), origin.Y + ((v[1].m_Z + pos.m_Y) * ratio)));

    // draw the polygon
    ::Polyline(hDC, &points[0], points.size());
}
//---------------------------------------------------------------------------
