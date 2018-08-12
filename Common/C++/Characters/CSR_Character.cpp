/****************************************************************************
 * ==> CSR_Character -------------------------------------------------------*
 ****************************************************************************
 * Description : Basic character class for games                            *
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

#pragma hdrstop
#include "CSR_Character.h"

// std
#include <stdexcept>

//---------------------------------------------------------------------------
// CSR_Character
//---------------------------------------------------------------------------
CSR_Character::CSR_Character(CSR_Scene* pScene, CSR_fOnDeleteTexture fOnDeleteTexture) :
    m_pScene(pScene),
    m_pKey(NULL),
    m_pTextureKey(NULL),
    m_fOnDeleteTexture(fOnDeleteTexture)
{
    // initialize the model matrix
    csrMat4Identity(&m_Matrix);
}
//---------------------------------------------------------------------------
CSR_Character::~CSR_Character()
{}
//---------------------------------------------------------------------------
CSR_Scene* CSR_Character::GetScene() const
{
    return m_pScene;
}
//---------------------------------------------------------------------------
void CSR_Character::SetScene(CSR_Scene* pScene)
{
    // delete the character model from the current scene
    csrSceneDeleteFrom(m_pScene, m_pKey, m_fOnDeleteTexture);

    m_pKey   = NULL;
    m_pScene = pScene;

    // rebuild the character
    Build();
}
//---------------------------------------------------------------------------
void* CSR_Character::GetKey() const
{
    return m_pKey;
}
//---------------------------------------------------------------------------
void* CSR_Character::GetTextureKey() const
{
    return m_pTextureKey;
}
//---------------------------------------------------------------------------
void CSR_Character::Situate(const CSR_Vector3& pos, const CSR_Vector3& dir, const CSR_Vector3& scale)
{
    CSR_Vector3 axis;
    CSR_Matrix4 translateMatrix;
    CSR_Matrix4 rotateMatrixX;
    CSR_Matrix4 rotateMatrixY;
    CSR_Matrix4 rotateMatrixZ;
    CSR_Matrix4 scaleMatrix;
    CSR_Matrix4 combinedMatrix1;
    CSR_Matrix4 combinedMatrix2;
    CSR_Matrix4 combinedMatrix3;

    // set translation
    csrMat4Translate(&pos, &translateMatrix);

    // set rotation axis
    axis.m_X = 1.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 0.0f;

    // set rotation on x axis
    csrMat4Rotate(dir.m_X, &axis, &rotateMatrixX);

    // set rotation axis
    axis.m_X = 0.0f;
    axis.m_Y = 1.0f;
    axis.m_Z = 0.0f;

    // set rotation on y axis
    csrMat4Rotate(dir.m_Y, &axis, &rotateMatrixY);

    // set rotation axis
    axis.m_X = 0.0f;
    axis.m_Y = 0.0f;
    axis.m_Z = 1.0f;

    // set rotation on z axis
    csrMat4Rotate(dir.m_Z, &axis, &rotateMatrixZ);

    // set scale factor
    csrMat4Scale(&scale, &scaleMatrix);

    // build model matrix
    csrMat4Multiply(&scaleMatrix,     &rotateMatrixX,   &combinedMatrix1);
    csrMat4Multiply(&combinedMatrix1, &rotateMatrixY,   &combinedMatrix2);
    csrMat4Multiply(&combinedMatrix2, &rotateMatrixZ,   &combinedMatrix3);
    csrMat4Multiply(&combinedMatrix3, &translateMatrix, &m_Matrix);
}
//---------------------------------------------------------------------------
