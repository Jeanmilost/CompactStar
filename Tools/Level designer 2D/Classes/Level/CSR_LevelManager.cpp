/****************************************************************************
 * ==> CSR_LevelManager ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a level manager                       *
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

#include "CSR_LevelManager.h"

//---------------------------------------------------------------------------
// CSR_LevelManager::IFiles
//---------------------------------------------------------------------------
CSR_LevelManager::IFiles::IFiles()
{}
//---------------------------------------------------------------------------
CSR_LevelManager::IFiles::~IFiles()
{}
//---------------------------------------------------------------------------
// CSR_LevelManager::IResource
//---------------------------------------------------------------------------
CSR_LevelManager::IResource::IResource() :
    m_ShapeType(IE_S_Unknown),
    m_Faces(0),
    m_Slices(0),
    m_Stacks(0),
    m_Radius(0),
    m_DeltaMin(0),
    m_DeltaMax(0),
    m_DeltaZ(0),
    m_RepeatTextureOnEachFace(false)
{}
//---------------------------------------------------------------------------
CSR_LevelManager::IResource::~IResource()
{}
//---------------------------------------------------------------------------
// CSR_LevelManager::IItem
//---------------------------------------------------------------------------
CSR_LevelManager::IItem::IItem()
{}
//---------------------------------------------------------------------------
CSR_LevelManager::IItem::~IItem()
{
    // delete the matrix list
    for (IMatrices::iterator it = m_Matrices.begin(); it != m_Matrices.end(); ++it)
        delete (*it);
}
//---------------------------------------------------------------------------
std::size_t CSR_LevelManager::IItem::AddMatrix()
{
    std::auto_ptr<CSR_Matrix4> pMatrix(new CSR_Matrix4());
    csrMat4Identity(pMatrix.get());
    m_Matrices.push_back(pMatrix.get());
    pMatrix.release();
    return m_Matrices.size() - 1;
}
//---------------------------------------------------------------------------
void CSR_LevelManager::IItem::DeleteMatrix(std::size_t index)
{
    // is index out of bounds?
    if (index >= m_Matrices.size())
        return;

    // delete the matrix
    delete m_Matrices[index];
    m_Matrices.erase(m_Matrices.begin() + index);
}
//---------------------------------------------------------------------------
// CSR_LevelManager::ISkybox
//---------------------------------------------------------------------------
CSR_LevelManager::ISkybox::ISkybox()
{}
//---------------------------------------------------------------------------
CSR_LevelManager::ISkybox::~ISkybox()
{}
//---------------------------------------------------------------------------
void CSR_LevelManager::ISkybox::Clear()
{
    m_Left.clear();
    m_Top.clear();
    m_Right.clear();
    m_Bottom.clear();
    m_Front.clear();
    m_Back.clear();
}
//---------------------------------------------------------------------------
// CSR_LevelManager::ISound
//---------------------------------------------------------------------------
CSR_LevelManager::ISound::ISound()
{}
//---------------------------------------------------------------------------
CSR_LevelManager::ISound::~ISound()
{}
//---------------------------------------------------------------------------
void CSR_LevelManager::ISound::Clear()
{
    m_FileName.clear();
}
//---------------------------------------------------------------------------
// CSR_LevelManager
//---------------------------------------------------------------------------
CSR_LevelManager::CSR_LevelManager()
{}
//---------------------------------------------------------------------------
CSR_LevelManager::~CSR_LevelManager()
{
    // NOTE the function domain name is required here to avoid calling a pure virtual function
    CSR_LevelManager::Clear();
}
//---------------------------------------------------------------------------
void CSR_LevelManager::Clear()
{
    // iterate through the level manager items
    for (IItems::iterator it = m_Items.begin(); it != m_Items.end(); ++it)
        // delete each of them
        delete it->second;

    // clear the level manager items
    m_Items.clear();

    // clear the standalone resources
    m_Skybox.Clear();
    m_Sound.Clear();
}
//---------------------------------------------------------------------------
CSR_LevelManager::IItem* CSR_LevelManager::Add(void* pKey)
{
    // search for a matching item
    IItems::iterator it = m_Items.find(pKey);

    // found one?
    if (it != m_Items.end())
    {
        // just add a new matrix for the cloned model and return the existing item
        it->second->AddMatrix();
        return it->second;
    }

    // create and initialize a new level manager item
    std::auto_ptr<IItem> pItem(new IItem());
    pItem->AddMatrix();

    // link the scene item to the manager
    m_Items[pKey] = pItem.get();

    return pItem.release();
}
//---------------------------------------------------------------------------
void CSR_LevelManager::Delete(void* pKey, std::size_t index)
{
    // search for the matching item
    IItems::iterator it = m_Items.find(pKey);

    // found it?
    if (it == m_Items.end())
        // nothing to do
        return;

    // delete the matrix at the index
    it->second->DeleteMatrix(index);

    // is item empty?
    if (!it->second->m_Matrices.size())
    {
        // delete and remove it from the manager
        delete it->second;
        m_Items.erase(it);
    }
}
//---------------------------------------------------------------------------
