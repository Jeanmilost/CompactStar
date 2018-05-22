/*****************************************************************************
 * ==> TVertexColorFrame ----------------------------------------------------*
 *****************************************************************************
 * Description : This module provides a frame that allow the user to select  *
 *               a vertex color                                              *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#include <vcl.h>
#pragma hdrstop

#include "TVertexColorFrame.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//----------------------------------------------------------------------------
// TVertexColorFrame
//----------------------------------------------------------------------------
TVertexColorFrame* pVertexColorFrame;
//----------------------------------------------------------------------------
__fastcall TVertexColorFrame::TVertexColorFrame(TComponent* pOwner) :
    TFrame(pOwner)
{}
//----------------------------------------------------------------------------
void __fastcall TVertexColorFrame::paColorClick(TObject* pSender)
{
    cdColor->Color = paColor->Color;

    if (!cdColor->Execute())
        return;

    paColor->Color = cdColor->Color;
}
//----------------------------------------------------------------------------
