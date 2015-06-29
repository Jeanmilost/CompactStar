/*****************************************************************************
 * ==> QR_Color -------------------------------------------------------------*
 *****************************************************************************
 * Description : Universal color class                                       *
 * Developer   : Jean-Milost Reymond                                         *
 *****************************************************************************/

#include "QR_Color.h"

//------------------------------------------------------------------------------
// QR_Color - c++ cross-platform
//------------------------------------------------------------------------------
QR_Color::QR_Color() :
    m_R(0),
    m_G(0),
    m_B(0),
    m_A(255)
{}
//------------------------------------------------------------------------------
QR_Color::QR_Color(QR_UInt8 r, QR_UInt8 g, QR_UInt8 b, QR_UInt8 a) :
    m_R(r),
    m_G(g),
    m_B(b),
    m_A(a)
{}
//------------------------------------------------------------------------------
QR_Color::~QR_Color()
{}
//------------------------------------------------------------------------------
void QR_Color::SetColor(QR_UInt8 r, QR_UInt8 g, QR_UInt8 b, QR_UInt8 a)
{
    m_R = r;
    m_G = g;
    m_B = b;
    m_A = a;
}
//------------------------------------------------------------------------------
inline QR_UInt32 QR_Color::GetRGB() const
{
    return (QR_UInt32)((m_R << 16) + (m_G << 8) + m_B);
}
//------------------------------------------------------------------------------
inline QR_UInt32 QR_Color::GetBGR() const
{
    return (QR_UInt32)((m_B << 16) + (m_G << 8) + m_R);
}
//------------------------------------------------------------------------------
inline QR_UInt32 QR_Color::GetARGB() const
{
    return (QR_UInt32)((m_A << 24) + (m_R << 16) + (m_G << 8) + m_B);
}
//------------------------------------------------------------------------------
inline QR_UInt32 QR_Color::GetRGBA() const
{
    return (QR_UInt32)((m_R << 24) + (m_G << 16) + (m_B << 8) + m_A);
}
//------------------------------------------------------------------------------
inline QR_UInt32 QR_Color::GetABGR() const
{
    return (QR_UInt32)((m_A << 24) + (m_B << 16) + (m_G << 8) + m_R);
}
//------------------------------------------------------------------------------
inline QR_UInt32 QR_Color::GetBGRA() const
{
    return (QR_UInt32)((m_B << 24) + (m_G << 16) + (m_R << 8) + m_A);
}
//------------------------------------------------------------------------------
inline void QR_Color::SetRGB(QR_UInt32 value)
{
    m_R = ((value >> 16) & 0xFF);
    m_G = ((value >> 8)  & 0xFF);
    m_B =   value        & 0xFF;
    m_A = 255;
}
//------------------------------------------------------------------------------
inline void QR_Color::SetBGR(QR_UInt32 value)
{
    m_B = ((value >> 16) & 0xFF);
    m_G = ((value >> 8)  & 0xFF);
    m_R =   value        & 0xFF;
    m_A = 255;
}
//------------------------------------------------------------------------------
inline void QR_Color::SetARGB(QR_UInt32 value)
{
    m_A = ((value >> 24) & 0xFF);
    m_R = ((value >> 16) & 0xFF);
    m_G = ((value >> 8)  & 0xFF);
    m_B =   value        & 0xFF;
}
//------------------------------------------------------------------------------
inline void QR_Color::SetRGBA(QR_UInt32 value)
{
    m_R = ((value >> 24) & 0xFF);
    m_G = ((value >> 16) & 0xFF);
    m_B = ((value >> 8)  & 0xFF);
    m_A =   value        & 0xFF;
}
//------------------------------------------------------------------------------
inline void QR_Color::SetABGR(QR_UInt32 value)
{
    m_A = ((value >> 24) & 0xFF);
    m_B = ((value >> 16) & 0xFF);
    m_G = ((value >> 8)  & 0xFF);
    m_R =   value        & 0xFF;
}
//------------------------------------------------------------------------------
inline void QR_Color::SetBGRA(QR_UInt32 value)
{
    m_B = ((value >> 24) & 0xFF);
    m_G = ((value >> 16) & 0xFF);
    m_R = ((value >> 8)  & 0xFF);
    m_A =   value        & 0xFF;
}
//------------------------------------------------------------------------------
QR_UInt8 QR_Color::GetRed() const
{
    return m_R;
}
//------------------------------------------------------------------------------
QR_UInt8 QR_Color::GetGreen() const
{
    return m_G;
}
//------------------------------------------------------------------------------
QR_UInt8 QR_Color::GetBlue() const
{
    return m_B;
}
//------------------------------------------------------------------------------
QR_UInt8 QR_Color::GetAlpha() const
{
    return m_A;
}
//------------------------------------------------------------------------------
void QR_Color::SetRed(QR_UInt8 value)
{
    m_R = value;
}
//------------------------------------------------------------------------------
void QR_Color::SetGreen(QR_UInt8 value)
{
    m_G = value;
}
//------------------------------------------------------------------------------
void QR_Color::SetBlue(QR_UInt8 value)
{
    m_B = value;
}
//------------------------------------------------------------------------------
void QR_Color::SetAlpha(QR_UInt8 value)
{
    m_A = value;
}
//------------------------------------------------------------------------------
QR_Float QR_Color::GetRedF() const
{
    return ((QR_Float)m_R / 255.0f);
}
//------------------------------------------------------------------------------
QR_Float QR_Color::GetGreenF() const
{
    return ((QR_Float)m_G / 255.0f);
}
//------------------------------------------------------------------------------
QR_Float QR_Color::GetBlueF() const
{
    return ((QR_Float)m_B / 255.0f);
}
//------------------------------------------------------------------------------
QR_Float QR_Color::GetAlphaF() const
{
    return ((QR_Float)m_A / 255.0f);
}
//------------------------------------------------------------------------------
QR_Color QR_Color::Blend(const QR_Color& other, const QR_Float& offset) const
{
    // do return internal color?
    if (offset <= 0.0f)
        return *this;

    // do return other color?
    if (offset >= 1.0f)
        return other;

    // calculate blended color components
    QR_UInt8 colorR = m_R + offset * (other.m_R - m_R);
    QR_UInt8 colorG = m_G + offset * (other.m_G - m_G);
    QR_UInt8 colorB = m_B + offset * (other.m_B - m_B);
    QR_UInt8 colorA = m_A + offset * (other.m_A - m_A);

    // return blended color
    return QR_Color(colorR, colorG, colorB, colorA);
}
//------------------------------------------------------------------------------

