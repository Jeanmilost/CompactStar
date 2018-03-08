/****************************************************************************
 * ==> CSR_VCLControlHook --------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a hook to listen the VCL controls     *
 *               Windows procedure                                          *
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
#include "CSR_VCLControlHook.h"

//---------------------------------------------------------------------------
CSR_VCLControlHook::CSR_VCLControlHook(TControl* pControl, ITfOnMessage fOnMessage) :
    m_pControl(pControl),
    m_fOnMessage(fOnMessage),
    m_fCtrlOriginalProc(NULL)
{
    // hook the control
    m_fCtrlOriginalProc    = m_pControl->WindowProc;
    m_pControl->WindowProc = ControlWndProc;
}
//---------------------------------------------------------------------------
CSR_VCLControlHook::~CSR_VCLControlHook()
{
    m_fOnMessage = NULL;

    if (m_fCtrlOriginalProc)
        m_pControl->WindowProc = m_fCtrlOriginalProc;
}
//---------------------------------------------------------------------------
void __fastcall CSR_VCLControlHook::ControlWndProc(TMessage& message)
{
    if (m_fOnMessage && m_fOnMessage(m_pControl, message, m_fCtrlOriginalProc))
        return;

    m_fCtrlOriginalProc(message);
}
//---------------------------------------------------------------------------
