/****************************************************************************
 * ==> CSR_Physics ---------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides physics algorithms and structures     *
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

#include "CSR_Physics.h"

// std
#include <stdlib.h>

//---------------------------------------------------------------------------
// Body functions
//---------------------------------------------------------------------------
CSR_Body* csrBodyCreate(void)
{
    // create a new body
    CSR_Body* pBody = (CSR_Body*)malloc(sizeof(CSR_Body));

    // succeeded?
    if (!pBody)
        return 0;

    // initialize the body content
    csrBodyInit(pBody);

    return pBody;
}
//---------------------------------------------------------------------------
void csrBodyRelease(CSR_Body* pBody)
{
    // no body to release?
    if (!pBody)
        return;

    // free the body
    free(pBody);
}
//---------------------------------------------------------------------------
void csrBodyInit(CSR_Body* pBody)
{
    // no body to initialize?
    if (!pBody)
        return;

    // initialize the body
    pBody->m_InitialForce.m_X = 0.0f;
    pBody->m_InitialForce.m_Y = 0.0f;
    pBody->m_InitialForce.m_Z = 0.0f;
    pBody->m_Mass             = 0.0f;
}
//---------------------------------------------------------------------------
// Physics functions
//---------------------------------------------------------------------------
void csrPhysicsApplyGravitation(const CSR_Body* pBody, float gravitation, CSR_Vector3* pR)
{
    // the formula for the gravitation is F = m * g
    pR->m_X = pBody->m_InitialForce.m_X;
    pR->m_Y = pBody->m_InitialForce.m_Y + (pBody->m_Mass * gravitation);
    pR->m_Z = pBody->m_InitialForce.m_Z;
}
//---------------------------------------------------------------------------
