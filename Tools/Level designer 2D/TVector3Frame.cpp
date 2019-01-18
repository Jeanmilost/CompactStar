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

#include <vcl.h>
#pragma hdrstop
#include "TVector3Frame.h"

// std
#include <limits>

// classes
#include "CSR_VCLHelper.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
// TVector3Frame
//---------------------------------------------------------------------------
TVector3Frame* Vector3Frame;
//---------------------------------------------------------------------------
__fastcall TVector3Frame::TVector3Frame(TComponent* pOwner) :
    TFrame(pOwner),
    m_fOnValueChanged(NULL)
{
    // initialize the last known value cache
    m_LastKnowValidValueDictionary[edX] = edX->Text.c_str();
    m_LastKnowValidValueDictionary[edY] = edY->Text.c_str();
    m_LastKnowValidValueDictionary[edZ] = edZ->Text.c_str();

    m_OriginalValue = edX->Text;
}
//---------------------------------------------------------------------------
void __fastcall TVector3Frame::FrameResize(TObject* pSender)
{
    const int controlSize = (ClientWidth - 12) / 3;

    laX->Width = controlSize;
    edX->Width = controlSize;
    laY->Width = controlSize;
    edY->Width = controlSize;
    laZ->Width = controlSize;
    edZ->Width = controlSize;

    Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TVector3Frame::OnEditEnter(TObject* pSender)
{
    // get the edit containing the text to check
    TEdit* pEdit = dynamic_cast<TEdit*>(pSender);

    if (!pEdit)
        return;

    m_OriginalValue = pEdit->Text;
}
//---------------------------------------------------------------------------
void __fastcall TVector3Frame::OnEditExit(TObject* pSender)
{
    if (m_fOnValueChanged)
        m_fOnValueChanged(this, GetX(), GetY(), GetZ());
}
//---------------------------------------------------------------------------
void __fastcall TVector3Frame::OnEditKeyUp(TObject* pSender, WORD& key, TShiftState shift)
{
    // get the edit containing the text to check
    TEdit* pEdit = dynamic_cast<TEdit*>(pSender);

    if (!pEdit)
        return;

    // search for pressed key
    switch (key)
    {
        case VK_ESCAPE:
            // revert to original value
            pEdit->Text = m_OriginalValue;
            pEdit->SelectAll();
            break;

        case VK_RETURN:
            // notify that the value changed
            if (m_fOnValueChanged)
                m_fOnValueChanged(this, GetX(), GetY(), GetZ());

            // select the entire value
            pEdit->SelectAll();
            break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TVector3Frame::OnEditValueChange(TObject* pSender)
{
    OnValueChange(pSender);
}
//---------------------------------------------------------------------------
CSR_Vector3 TVector3Frame::GetVector() const
{
    CSR_Vector3 vector;
    vector.m_X = GetX();
    vector.m_Y = GetY();
    vector.m_Z = GetZ();
    return vector;
}
//---------------------------------------------------------------------------
void TVector3Frame::SetVector(const CSR_Vector3& vector)
{
    SetX(vector.m_X);
    SetY(vector.m_Y);
    SetZ(vector.m_Z);
}
//---------------------------------------------------------------------------
float TVector3Frame::GetX() const
{
    float result;

    try
    {
        // try to convert the text to float
        result = CSR_VCLHelper::StrToFloat(edX->Text.c_str());
    }
    catch (...)
    {
        result = std::numeric_limits<float>::infinity();
    }

    return result;
}
//---------------------------------------------------------------------------
void TVector3Frame::SetX(float value)
{
    edX->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(value).c_str());
}
//---------------------------------------------------------------------------
float TVector3Frame::GetY() const
{
    float result;

    try
    {
        // try to convert the text to float
        result = CSR_VCLHelper::StrToFloat(edY->Text.c_str());
    }
    catch (...)
    {
        result = std::numeric_limits<float>::infinity();
    }

    return result;
}
//---------------------------------------------------------------------------
void TVector3Frame::SetY(float value)
{
    edY->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(value).c_str());
}
//---------------------------------------------------------------------------
float TVector3Frame::GetZ() const
{
    float result;

    try
    {
        // try to convert the text to float
        result = CSR_VCLHelper::StrToFloat(edZ->Text.c_str());
    }
    catch (...)
    {
        result = std::numeric_limits<float>::infinity();
    }

    return result;
}
//---------------------------------------------------------------------------
void TVector3Frame::SetZ(float value)
{
    edZ->Text = UnicodeString(CSR_VCLHelper::FloatToWStr(value).c_str());
}
//---------------------------------------------------------------------------
void TVector3Frame::Set_OnValueChanged(ITfOnValueChanged fHandler)
{
    m_fOnValueChanged = fHandler;
}
//---------------------------------------------------------------------------
void TVector3Frame::OnValueChange(TObject* pSender)
{
    // get the edit containing the text to check
    TEdit* pEdit = dynamic_cast<TEdit*>(pSender);

    if (!pEdit)
        return;

    // is text empty or just containing a "-" value? (Meaning that further info will be entered later)
    if (pEdit->Text.IsEmpty() || pEdit->Text == L"-")
        return;

    // get the last known value for this edit
    IValueDictionary::iterator it = m_LastKnowValidValueDictionary.find(pSender);

    // is value not found? (Should never happen)
    if (it == m_LastKnowValidValueDictionary.end())
        return;

    try
    {
        // try to convert the text to float
        CSR_VCLHelper::StrToFloat(pEdit->Text.c_str());
    }
    catch (...)
    {
        // get the caret position
        const int caret = pEdit->SelStart;

        // on failure revert to the last known valid value
        pEdit->Text = UnicodeString(it->second.c_str());

        pEdit->SelStart  = caret ? caret - 1 : 0;
        pEdit->SelLength = 0;
        return;
    }

    // update the last known value
    it->second = pEdit->Text.c_str();
}
//---------------------------------------------------------------------------
