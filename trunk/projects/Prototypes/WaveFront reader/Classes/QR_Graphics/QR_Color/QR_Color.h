/*****************************************************************************
 * ==> QR_Color -------------------------------------------------------------*
 *****************************************************************************
 * Description : Universal color class                                       *
 * Developer   : Jean-Milost Reymond                                         *
 *****************************************************************************/

#ifndef QR_ColorH
#define QR_ColorH

// do use without others QR Engine classes?
#ifndef USE_QR_COLOR_OLNY
    #include "QR_Types.h"
#else
    // std
    #include <stdint.h>

    #define QR_UInt8  std::uint8_t
    #define QR_UInt32 std::uint32_t
    #define QR_Float  float
#endif

/**
* Universal color class
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_Color
{
    public:
        QR_Color();
        virtual ~QR_Color();

        /**
        * Constructor
        *@param r - color red component
        *@param g - color green component
        *@param b - color blue component
        *@param a - color alpha component
        */
        QR_Color(QR_UInt8 r, QR_UInt8 g, QR_UInt8 b, QR_UInt8 a = 255);

        /**
        * Sets color
        *@param r - color red component
        *@param g - color green component
        *@param b - color blue component
        *@param a - color alpha component
        */
        virtual void SetColor(QR_UInt8 r, QR_UInt8 g, QR_UInt8 b, QR_UInt8 a = 255);

        /**
        * Gets color as 32 bit value
        *@return color as 32 bit value
        */
        virtual inline QR_UInt32 GetRGB() const;
        virtual inline QR_UInt32 GetBGR() const;
        virtual inline QR_UInt32 GetARGB() const;
        virtual inline QR_UInt32 GetRGBA() const;
        virtual inline QR_UInt32 GetABGR() const;
        virtual inline QR_UInt32 GetBGRA() const;

        /**
        * Sets color from 32 bit value
        *@return color as 32 bit value
        */
        virtual inline void SetRGB(QR_UInt32 value);
        virtual inline void SetBGR(QR_UInt32 value);
        virtual inline void SetARGB(QR_UInt32 value);
        virtual inline void SetRGBA(QR_UInt32 value);
        virtual inline void SetABGR(QR_UInt32 value);
        virtual inline void SetBGRA(QR_UInt32 value);

        /**
        * Gets red component
        *@return red component
        */
        virtual QR_UInt8 GetRed() const;

        /**
        * Gets green component
        *@return green component
        */
        virtual QR_UInt8 GetGreen() const;

        /**
        * Gets blue component
        *@return blue component
        */
        virtual QR_UInt8 GetBlue() const;

        /**
        * Gets alpha component
        *@return alpha component
        */
        virtual QR_UInt8 GetAlpha() const;

        /**
        * Sets red component
        *@param value - component value
        */
        virtual void SetRed(QR_UInt8 value);

        /**
        * Sets green component
        *@param value - component value
        */
        virtual void SetGreen(QR_UInt8 value);

        /**
        * Sets blue component
        *@param value - component value
        */
        virtual void SetBlue(QR_UInt8 value);

        /**
        * Sets alpha component
        *@param value - component value
        */
        virtual void SetAlpha(QR_UInt8 value);

        /**
        * Gets red component as float (between 0.0f and 1.0f)
        *@return red component
        */
        virtual QR_Float GetRedF() const;

        /**
        * Gets green component as float (between 0.0f and 1.0f)
        *@return green component
        */
        virtual QR_Float GetGreenF() const;

        /**
        * Gets blue component as float (between 0.0f and 1.0f)
        *@return blue component
        */
        virtual QR_Float GetBlueF() const;

        /**
        * Gets alpha component as float (between 0.0f and 1.0f)
        *@return alpha component
        */
        virtual QR_Float GetAlphaF() const;

        /**
        * Blend color with another color
        *@param other - other color to blend with
        *@param offset - blend offset (from 0.0f to 1.0f)
        *@return blended color
        */
        virtual QR_Color Blend(const QR_Color& other, const QR_Float& offset) const;

    private:
        QR_UInt8 m_R;
        QR_UInt8 m_G;
        QR_UInt8 m_B;
        QR_UInt8 m_A;
};
#endif
