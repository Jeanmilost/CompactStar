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

#ifndef TVertexColorFrameH
#define TVertexColorFrameH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ComCtrls.hpp>

/**
* Vertex color frame
*@author Jean-Milost Reymond
*/
class TVertexColorFrame : public TFrame
{
    __published:
        TPanel *paMain;
        TLabel *laCaption;
        TPanel *paColor;
        TColorDialog *cdColor;
        TTrackBar *tbOpacity;

        void __fastcall paColorClick(TObject* pSender);

    public:
        /**
        * Constructor
        *@param pOwner - frame owner
        */
        __fastcall TVertexColorFrame(TComponent* pOwner);
};
extern PACKAGE TVertexColorFrame* VertexColorFrame;
#endif
