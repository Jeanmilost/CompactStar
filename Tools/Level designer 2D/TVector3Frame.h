/****************************************************************************
 * ==> TVector3Frame -------------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a frame exposing the properties of a  *
 *               3d vector                                                  *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2019, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#ifndef TVector3FrameH
#define TVector3FrameH

// vcl
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>

// std
#include <map>

// compactStar engine
#include "CSR_Geometry.h"

/**
* A frame exposing the properties of a 3d vector
*@author Jean-Milost Reymond
*/
class TVector3Frame : public TFrame
{
    __published:
        TEdit *edX;
        TLabel *laX;
        TPanel *paLabels;
        TPanel *paValues;
        TEdit *edY;
        TEdit *edZ;
        TLabel *laY;
        TLabel *laZ;

        void __fastcall FrameResize(TObject* pSender);
        void __fastcall OnEditEnter(TObject* pSender);
        void __fastcall OnEditExit(TObject* pSender);
        void __fastcall OnEditKeyUp(TObject* pSender, WORD& key, TShiftState shift);
        void __fastcall OnEditValueChange(TObject* pSender);

    public:
        /**
        * Called when a value changed
        *@param pSender - event sender
        *@param x - x value
        *@param y - y value
        *@param z - z value
        */
        typedef void (__closure *ITfOnValueChanged)(TObject* pSender, float x, float y, float z);

        /**
        * Constructor
        *@param pOwner - frame owner
        */
        __fastcall TVector3Frame(TComponent* pOwner);

        /**
        * Gets the vector
        *@return the vector
        */
        CSR_Vector3 GetVector() const;

        /**
        * Sets the vector
        *@param vector - the vector to set
        */
        void SetVector(const CSR_Vector3& vector);

        /**
        * Gets the vector x value
        *@return the vector x value
        */
        float GetX() const;

        /**
        * Sets the vector x value
        *@param value - the vector x value
        */
        void SetX(float value);

        /**
        * Gets the vector y value
        *@return the vector y value
        */
        float GetY() const;

        /**
        * Sets the vector y value
        *@param value - the vector y value
        */
        void SetY(float value);

        /**
        * Gets the vector z value
        *@return the vector z value
        */
        float GetZ() const;

        /**
        * Sets the vector z value
        *@param value - the vector z value
        */
        void SetZ(float value);

        /**
        * Sets the OnValueChanged event
        *@param fHandler - function handler
        */
        void Set_OnValueChanged(ITfOnValueChanged fHandler);

    protected:
        /**
        * Called when a value changes on one of the edit box
        *@aram pSender - event sender
        */
        void OnValueChange(TObject* pSender);

    private:
        typedef std::map<TObject*, std::wstring> IValueDictionary;

        IValueDictionary  m_LastKnowValidValueDictionary;
        UnicodeString     m_OriginalValue;
        ITfOnValueChanged m_fOnValueChanged;
};
extern PACKAGE TVector3Frame* Vector3Frame;
#endif
