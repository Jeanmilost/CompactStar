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

#ifndef CSR_VCLControlHookH
#define CSR_VCLControlHookH

// vcl
#include <Vcl.Controls.hpp>

/**
* VCL control hook
*@author Jean-Milost Reymond
*/
class CSR_VCLControlHook
{
    public:
        /**
        * Called when a hooked control receives a Windows message
        *@param pControl - hooked control
        *@param message - Windows message
        *@param fCtrlOriginalProc - control original Windows procedure
        *@return true if the message was resolved and should no longer be handled, otherwise false
        */
        typedef bool (__closure *ITfOnMessage)(TControl*  pControl,
                                               TMessage&  message,
                                               TWndMethod fCtrlOriginalProc);

        /**
        * Constructor
        *@param pControl - control to hook
        *@param fOnMessage - callback function to call when a Windows message is received
        */
        CSR_VCLControlHook(TControl* pControl, ITfOnMessage fOnMessage);

        virtual ~CSR_VCLControlHook();

    private:
        TControl*    m_pControl;
        ITfOnMessage m_fOnMessage;
        TWndMethod   m_fCtrlOriginalProc;

        /**
        * Hooked control main procedure
        *@param message- Windows procedure message
        */
        void __fastcall ControlWndProc(TMessage& message);
};

#endif
