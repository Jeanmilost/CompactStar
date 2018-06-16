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
// CSR_DesignerView::ISelection
//---------------------------------------------------------------------------
CSR_DesignerView::ISelection::ISelection() :
    m_pKey(NULL),
    m_MatrixIndex(-1)
{}
//---------------------------------------------------------------------------
CSR_DesignerView::ISelection::~ISelection()
{}
//---------------------------------------------------------------------------
// CSR_DesignerView
//---------------------------------------------------------------------------
CSR_DesignerView::CSR_DesignerView(TPanel* pPanel) :
    CSR_View(),
    m_pScene(NULL),
    m_pPanel(pPanel),
    m_ModelColor(TColor(0x555555)),
    m_SelectedModelColor(clRed),
    m_hBrush(NULL),
    m_hPen(NULL),
    m_hSelectedBrush(NULL),
    m_hSelectedPen(NULL),
    m_Ratio(1.0f)
{
    // create the default brush and pen
    m_hBrush         = ::CreateSolidBrush(Graphics::ColorToRGB(m_ModelColor));
    m_hPen           = ::CreatePen(PS_SOLID, 1, Graphics::ColorToRGB(m_ModelColor));
    m_hSelectedBrush = ::CreateSolidBrush(Graphics::ColorToRGB(m_SelectedModelColor));
    m_hSelectedPen   = ::CreatePen(PS_SOLID, 1, Graphics::ColorToRGB(m_SelectedModelColor));

    // hook the view panel Windows procedure
    m_pHook.reset(new CSR_VCLControlHook(m_pPanel, OnViewMessage));
}
//---------------------------------------------------------------------------
CSR_DesignerView::~CSR_DesignerView()
{
    // restore the previous view panel Windows procedure
    m_pHook.reset();

    if (m_hBrush)
        ::DeleteObject(m_hBrush);

    if (m_hPen)
        ::DeleteObject(m_hPen);

    if (m_hSelectedBrush)
        ::DeleteObject(m_hSelectedBrush);

    if (m_hSelectedPen)
        ::DeleteObject(m_hSelectedPen);
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
const CSR_DesignerView::ISelection* CSR_DesignerView::GetSelection() const
{
    return &m_Selection;
}
//---------------------------------------------------------------------------
void CSR_DesignerView::SetSelection(const ISelection& selection)
{
    m_Selection = selection;
}
//---------------------------------------------------------------------------
void CSR_DesignerView::SelectPrev()
{
    if (!m_pScene)
        return;

    try
    {
        // no selected item?
        if (!m_Selection.m_pKey)
            // select the last one
            SelectLast();
        else
        {
            // iterate through scene transparent items to select
            for (std::size_t i = 0; i < m_pScene->m_TransparentItemCount; ++i)
            {
                // get the transparent item
                const CSR_SceneItem* pItem = &m_pScene->m_pTransparentItem[i];
                assert(pItem);

                // found the currently selected model?
                if (pItem->m_pModel == m_Selection.m_pKey)
                {
                    // select the prev matrix in the model, and check if prev model should be selected
                    if (SelectPrevMatrix(pItem))
                    {
                        // is the first item in the list?
                        if (!i)
                        {
                            // select the last matrix of the last model in the standard item list
                            if (m_pScene->m_ItemCount)
                            {
                                // get the prev item to select
                                const CSR_SceneItem* pPrevItem =
                                    &m_pScene->m_pItem[m_pScene->m_ItemCount - 1];

                                // select the prev item
                                m_Selection.m_pKey        = pPrevItem->m_pModel;
                                m_Selection.m_MatrixIndex = pPrevItem->m_pMatrixArray->m_Count     ?
                                                            pPrevItem->m_pMatrixArray->m_Count - 1 :
                                                            -1;
                            }
                            else
                                // select the last item in the scene
                                SelectLast();
                        }
                        else
                        {
                            // get the prev item to select
                            const CSR_SceneItem* pPrevItem = &m_pScene->m_pTransparentItem[i - 1];

                            // select the prev item
                            m_Selection.m_pKey        = pPrevItem->m_pModel;
                            m_Selection.m_MatrixIndex = pPrevItem->m_pMatrixArray->m_Count     ?
                                                        pPrevItem->m_pMatrixArray->m_Count - 1 :
                                                        -1;
                        }
                    }

                    return;
                }
            }

            // iterate through scene transparent items to select
            for (std::size_t i = 0; i < m_pScene->m_ItemCount; ++i)
            {
                // get the item
                const CSR_SceneItem* pItem = &m_pScene->m_pItem[i];
                assert(pItem);

                // found the currently selected model?
                if (pItem->m_pModel == m_Selection.m_pKey)
                {
                    // select the prev matrix in the model, and check if prev model should be selected
                    if (SelectPrevMatrix(pItem))
                    {
                        // is the first item in the list?
                        if (!i)
                            // select the last item in the scene
                            SelectLast();
                        else
                        {
                            // get the prev item to select
                            const CSR_SceneItem* pPrevItem = &m_pScene->m_pItem[i - 1];

                            // select the prev item
                            m_Selection.m_pKey        = pPrevItem->m_pModel;
                            m_Selection.m_MatrixIndex = pPrevItem->m_pMatrixArray->m_Count     ?
                                                        pPrevItem->m_pMatrixArray->m_Count - 1 :
                                                        -1;
                        }
                    }

                    return;
                }
            }
        }
    }
    __finally
    {
        m_pPanel->Invalidate();
    }
}
//---------------------------------------------------------------------------
void CSR_DesignerView::SelectNext()
{
    if (!m_pScene)
        return;

    try
    {
        // no selected item?
        if (!m_Selection.m_pKey)
            // select the first one
            SelectFirst();
        else
        {
            // iterate through scene items to select
            for (std::size_t i = 0; i < m_pScene->m_ItemCount; ++i)
            {
                // get the transparent item
                const CSR_SceneItem* pItem = &m_pScene->m_pItem[i];
                assert(pItem);

                // found the currently selected model?
                if (pItem->m_pModel == m_Selection.m_pKey)
                {
                    // select the next matrix in the model, and check if next model should be selected
                    if (SelectNextMatrix(pItem))
                    {
                        // is the last item in the list?
                        if (i == m_pScene->m_ItemCount - 1)
                        {
                            // select the first matrix of the first model in the transparent item list
                            if (m_pScene->m_TransparentItemCount)
                            {
                                // get the next item to select
                                const CSR_SceneItem* pNextItem = &m_pScene->m_pTransparentItem[0];

                                // select the next item
                                m_Selection.m_pKey        = pNextItem->m_pModel;
                                m_Selection.m_MatrixIndex = pNextItem->m_pMatrixArray->m_Count ? 0 : -1;
                            }
                            else
                                // select the first item in the scene
                                SelectFirst();
                        }
                        else
                        {
                            // get the next item to select
                            const CSR_SceneItem* pNextItem = &m_pScene->m_pItem[i + 1];

                            // select the prev item
                            m_Selection.m_pKey        = pNextItem->m_pModel;
                            m_Selection.m_MatrixIndex = pNextItem->m_pMatrixArray->m_Count ? 0 : -1;
                        }
                    }

                    return;
                }
            }

            // iterate through scene transparent items to select
            for (std::size_t i = 0; i < m_pScene->m_TransparentItemCount; ++i)
            {
                // get the item
                const CSR_SceneItem* pItem = &m_pScene->m_pTransparentItem[i];
                assert(pItem);

                // found the currently selected model?
                if (pItem->m_pModel == m_Selection.m_pKey)
                {
                    // select the next matrix in the model, and check if next model should be selected
                    if (SelectNextMatrix(pItem))
                    {
                        // is the last item in the list?
                        if (i == m_pScene->m_TransparentItemCount - 1)
                            // select the first item in the scene
                            SelectFirst();
                        else
                        {
                            // get the next item to select
                            const CSR_SceneItem* pNextItem = &m_pScene->m_pTransparentItem[i + 1];

                            // select the prev item
                            m_Selection.m_pKey        = pNextItem->m_pModel;
                            m_Selection.m_MatrixIndex = pNextItem->m_pMatrixArray->m_Count ? 0 : -1;
                        }
                    }

                    return;
                }
            }
        }
    }
    __finally
    {
        m_pPanel->Invalidate();
    }
}
//---------------------------------------------------------------------------
void CSR_DesignerView::SelectFirst()
{
    if (!m_pScene)
        return;

    // select the first item, if any
    if (m_pScene->m_ItemCount)
    {
        // get the last transparent item
        const CSR_SceneItem* pItem = &m_pScene->m_pItem[0];
        assert(pItem);

        // select the model
        m_Selection.m_pKey = pItem->m_pModel;

        // select the last model matrix
        if (pItem->m_pMatrixArray->m_Count)
            m_Selection.m_MatrixIndex =  0;
        else
            m_Selection.m_MatrixIndex = -1;

        return;
    }

    // select the last transparent item, if any. Transparent items are always drawn on the end
    if (m_pScene->m_TransparentItemCount)
    {
        // get the last transparent item
        const CSR_SceneItem* pItem = &m_pScene->m_pTransparentItem[0];
        assert(pItem);

        // select the model
        m_Selection.m_pKey = pItem->m_pModel;

        // select the last model matrix
        if (pItem->m_pMatrixArray->m_Count)
            m_Selection.m_MatrixIndex =  0;
        else
            m_Selection.m_MatrixIndex = -1;

        return;
    }

    // nothing to select
    Unselect();
}
//---------------------------------------------------------------------------
void CSR_DesignerView::SelectLast()
{
    if (!m_pScene)
        return;

    // select the last transparent item, if any. Transparent items are always drawn on the end
    if (m_pScene->m_TransparentItemCount)
    {
        // get the last transparent item
        const CSR_SceneItem* pItem =
                &m_pScene->m_pTransparentItem[m_pScene->m_TransparentItemCount - 1];
        assert(pItem);

        // select the model
        m_Selection.m_pKey = pItem->m_pModel;

        // select the last model matrix
        if (pItem->m_pMatrixArray->m_Count)
            m_Selection.m_MatrixIndex =  pItem->m_pMatrixArray->m_Count - 1;
        else
            m_Selection.m_MatrixIndex = -1;

        return;
    }

    // otherwise select the last item, if any
    if (m_pScene->m_ItemCount)
    {
        // get the last transparent item
        const CSR_SceneItem* pItem = &m_pScene->m_pItem[m_pScene->m_ItemCount - 1];
        assert(pItem);

        // select the model
        m_Selection.m_pKey = pItem->m_pModel;

        // select the last model matrix
        if (pItem->m_pMatrixArray->m_Count)
            m_Selection.m_MatrixIndex =  pItem->m_pMatrixArray->m_Count - 1;
        else
            m_Selection.m_MatrixIndex = -1;

        return;
    }

    // nothing to select
    Unselect();
}
//---------------------------------------------------------------------------
void CSR_DesignerView::Unselect()
{
    m_Selection.m_pKey        =  NULL;
    m_Selection.m_MatrixIndex = -1;
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
TColor CSR_DesignerView::GetModelColor() const
{
    return m_ModelColor;
}
//---------------------------------------------------------------------------
void CSR_DesignerView::SetModelColor(TColor color)
{
    if (m_ModelColor == color)
        return;

    m_ModelColor = color;

    if (m_hBrush)
        ::DeleteObject(m_hBrush);

    if (m_hPen)
        ::DeleteObject(m_hPen);

    m_hBrush = ::CreateSolidBrush(Graphics::ColorToRGB(m_ModelColor));
    m_hPen   = ::CreatePen(PS_SOLID, 1, Graphics::ColorToRGB(m_ModelColor));
}
//---------------------------------------------------------------------------
TColor CSR_DesignerView::GetSelectedModelColor() const
{
    return m_SelectedModelColor;
}
//---------------------------------------------------------------------------
void CSR_DesignerView::SetSelectedModelColor(TColor color)
{
    if (m_SelectedModelColor == color)
        return;

    m_SelectedModelColor = color;

    if (m_hSelectedBrush)
        ::DeleteObject(m_hSelectedBrush);

    if (m_hSelectedPen)
        ::DeleteObject(m_hSelectedPen);

    m_hSelectedBrush = ::CreateSolidBrush(Graphics::ColorToRGB(m_SelectedModelColor));
    m_hSelectedPen   = ::CreatePen(PS_SOLID, 1, Graphics::ColorToRGB(m_SelectedModelColor));
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

    if (!m_hBrush || !m_hPen)
        return;

    // select the background mode to apply
    ::SetBkMode(hDC, OPAQUE);

    // iterate through scene items to draw
    for (std::size_t i = 0; i < pScene->m_ItemCount; ++i)
        DrawItem(&pScene->m_pItem[i], origin, pos, ratio, hDC);

    // iterate through scene transparent items to draw
    for (std::size_t i = 0; i < pScene->m_TransparentItemCount; ++i)
        DrawItem(&pScene->m_pTransparentItem[i], origin, pos, ratio, hDC);
}
//---------------------------------------------------------------------------
void CSR_DesignerView::DrawItem(const CSR_SceneItem* pSceneItem,
                                const TPoint&        origin,
                                const CSR_Vector2&   pos,
                                      float          ratio,
                                      HDC            hDC) const
{
    if (!pSceneItem)
        return;

    // iterate through item model matrices
    for (std::size_t i = 0; i < pSceneItem->m_pMatrixArray->m_Count; ++i)
    {
        // select the brush and pen to use
        if (pSceneItem->m_pModel == m_Selection.m_pKey && i == m_Selection.m_MatrixIndex)
        {
            ::SelectObject(hDC, m_hSelectedBrush);
            ::SelectObject(hDC, m_hSelectedPen);
        }
        else
        {
            ::SelectObject(hDC, m_hBrush);
            ::SelectObject(hDC, m_hPen);
        }

        // draw the next model
        DrawModel(pSceneItem->m_pModel,
                  pSceneItem->m_Type,
                  static_cast<CSR_Matrix4*>(pSceneItem->m_pMatrixArray->m_pItem[i].m_pData),
                  origin,
                  pos,
                  ratio,
                  hDC);
    }
}
//---------------------------------------------------------------------------
void CSR_DesignerView::DrawModel(const void*          pModelItem,
                                       CSR_EModelType modelType,
                                 const CSR_Matrix4*   pMatrix,
                                 const TPoint&        origin,
                                 const CSR_Vector2&   pos,
                                       float          ratio,
                                       HDC            hDC) const
{
    if (!pModelItem)
        return;

    // search for item model type
    switch (modelType)
    {
        case CSR_MT_Mesh:
        {
            // item contains a mesh, convert it
            const CSR_Mesh* pMesh = static_cast<const CSR_Mesh*>(pModelItem);

            // found it?
            if (!pMesh)
                return;

            // iterate through model meshes
            CSR_IndexedPolygonBuffer* pBuffer = NULL;

            try
            {
                // extract the mesh polygons
                pBuffer = csrIndexedPolygonBufferFromMesh(pMesh);

                // succeeded?
                if (!pBuffer)
                    return;

                // iterate through polygons
                for (std::size_t i = 0; i < pBuffer->m_Count; ++i)
                {
                    CSR_Polygon3 polygon;

                    // get the next polygon to draw
                    if (!csrIndexedPolygonToPolygon(&pBuffer->m_pIndexedPolygon[i], &polygon))
                        continue;

                    // draw the polygon
                    DrawPolygon(origin,
                                pos,
                                pMatrix,
                                polygon,
                                ratio,
                                hDC);
                }
            }
            __finally
            {
                csrIndexedPolygonBufferRelease(pBuffer);
            }

            return;
        }

        case CSR_MT_Model:
        {
            // item contains a model, convert it
            const CSR_Model* pModel = static_cast<const CSR_Model*>(pModelItem);

            // found it?
            if (!pModel)
                return;

            // iterate through model meshes
            for (std::size_t i = 0; i < pModel->m_MeshCount; ++i)
            {
                CSR_IndexedPolygonBuffer* pBuffer = NULL;

                try
                {
                    // extract the mesh polygons
                    pBuffer = csrIndexedPolygonBufferFromMesh(&pModel->m_pMesh[i]);

                    // succeeded?
                    if (!pBuffer)
                        continue;

                    // iterate through polygons
                    for (std::size_t j = 0; j < pBuffer->m_Count; ++j)
                    {
                        CSR_Polygon3 polygon;

                        // get the next polygon to draw
                        if (!csrIndexedPolygonToPolygon(&pBuffer->m_pIndexedPolygon[j], &polygon))
                            continue;

                        // draw the polygon
                        DrawPolygon(origin,
                                    pos,
                                    pMatrix,
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

            return;
        }

        case CSR_MT_MDL:
        {
            // item contains a Quake I model, convert it
            const CSR_MDL* pMDL = static_cast<const CSR_MDL*>(pModelItem);

            // found it?
            if (!pMDL)
                return;

            // is empty?
            if (!pMDL->m_ModelCount)
                return;

            // get the first model (enough to be shown on the view)
            CSR_Model* pModel = &pMDL->m_pModel[0];

            // found it?
            if (!pModel)
                return;

            // iterate through model meshes
            for (std::size_t i = 0; i < pModel->m_MeshCount; ++i)
            {
                CSR_IndexedPolygonBuffer* pBuffer = NULL;

                try
                {
                    // extract the mesh polygons
                    pBuffer = csrIndexedPolygonBufferFromMesh(&pModel->m_pMesh[i]);

                    // succeeded?
                    if (!pBuffer)
                        continue;

                    // iterate through polygons
                    for (std::size_t j = 0; j < pBuffer->m_Count; ++j)
                    {
                        CSR_Polygon3 polygon;

                        // get the next polygon to draw
                        if (!csrIndexedPolygonToPolygon(&pBuffer->m_pIndexedPolygon[j], &polygon))
                            continue;

                        // draw the polygon
                        DrawPolygon(origin,
                                    pos,
                                    pMatrix,
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

            return;
        }
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
bool CSR_DesignerView::SelectPrevMatrix(const CSR_SceneItem* pItem)
{
    // no item?
    if (!pItem)
        return false;

    // is selected index out of bounds?
    if (m_Selection.m_MatrixIndex < 0 || m_Selection.m_MatrixIndex >= pItem->m_pMatrixArray->m_Count)
        return true;

    // do select the last matrix index of the prev model
    if (!m_Selection.m_MatrixIndex)
        return true;

    // select the prev matrix
    --m_Selection.m_MatrixIndex;

    return false;
}
//---------------------------------------------------------------------------
bool CSR_DesignerView::SelectNextMatrix(const CSR_SceneItem* pItem)
{
    // no item?
    if (!pItem)
        return false;

    // is selected index out of bounds?
    if (m_Selection.m_MatrixIndex < 0 || m_Selection.m_MatrixIndex >= pItem->m_pMatrixArray->m_Count)
        return true;

    // do select the first matrix index of the next model
    if (m_Selection.m_MatrixIndex == pItem->m_pMatrixArray->m_Count - 1)
        return true;

    // select the prev matrix
    ++m_Selection.m_MatrixIndex;

    return false;
}
//---------------------------------------------------------------------------
