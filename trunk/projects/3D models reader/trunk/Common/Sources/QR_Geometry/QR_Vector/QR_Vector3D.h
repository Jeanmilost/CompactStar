/******************************************************************************
 * ==> QR_Vector3D -----------------------------------------------------------*
 ******************************************************************************
 * Description : Euclidean vector 3D                                          *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_Vector3DH
#define QR_Vector3DH

// std
#include <math.h>

// do use without others QR Engine classes?
#ifndef USE_QR_GEOMETRY_OLNY
    // qr engine
    #include "QR_Types.h"
    #include "QR_Exception.h"
#else
    #define M_THROW_EXCEPTION(message) throw message
#endif

/**
* Vector 3D
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
template <class T>
class QR_Vector3D
{
    public:
        T m_X; // x coordinate for the 3D vector
        T m_Y; // y coordinate for the 3D vector
        T m_Z; // z coordinate for the 3D vector

        /**
        * Constructor
        */
        inline QR_Vector3D();

        /**
        * Constructor
        *@param x - vector x value
        *@param y - vector y value
        *@param z - vector z value
        */
        inline QR_Vector3D(const T& x, const T& y, const T& z);

        /**
        * Copy constructor
        *@param other - other vector to copy from
        */
        inline QR_Vector3D(const QR_Vector3D& other);

        /**
        * Destructor
        */
        virtual inline ~QR_Vector3D();

        /**
        * Copy operator
        *@param other - other vector to copy from
        *@return this vector
        */
        virtual inline QR_Vector3D& operator =(const QR_Vector3D& other);

        /**
        * Operator + for single value
        *@param value - value to add
        *@return resulting added vector
        */
        virtual inline QR_Vector3D operator + (T value) const;

        /**
        * Operator +
        *@param other - other vector to add
        *@return resulting added vector
        */
        virtual inline QR_Vector3D operator + (const QR_Vector3D& other) const;

        /**
        * Operator -
        *@return inverted vector
        */
        virtual inline QR_Vector3D operator - () const;

        /**
        * Operator - for single value
        *@param value - value to subtract
        *@return resulting subtracted vector
        */
        virtual inline QR_Vector3D operator - (T value) const;

        /**
        * Operator -
        *@param other - other vector to subtract
        *@return resulting subtracted vector
        */
        virtual inline QR_Vector3D operator - (const QR_Vector3D& other) const;

        /**
        * Operator * for single value
        *@param value - value to multiply
        *@return resulting multiplied vector
        */
        virtual inline QR_Vector3D operator * (T value) const;

        /**
        * Operator *
        *@param other - other vector to multiply
        *@return resulting multiplied vector
        */
        virtual inline QR_Vector3D operator * (const QR_Vector3D& other) const;

        /**
        * Operator / for single value
        *@param value - value to divide
        *@return resulting divided vector
        */
        virtual inline QR_Vector3D operator / (T value) const;

        /**
        * Operator /
        *@param other - other vector to divide
        *@return resulting divided vector
        */
        virtual inline QR_Vector3D operator / (const QR_Vector3D& other) const;

        /**
        * Operator += for single value
        *@param value - value to add
        *@return resulting added vector
        */
        virtual inline const QR_Vector3D& operator += (T value);

        /**
        * Operator +=
        *@param other - other vector to add
        *@return resulting added vector
        */
        virtual inline const QR_Vector3D& operator += (const QR_Vector3D& other);

        /**
        * Operator -= for single value
        *@param value - value to subtract
        *@return resulting subtracted vector
        */
        virtual inline const QR_Vector3D& operator -= (T value);

        /**
        * Operator -=
        *@param other - other vector to subtract
        *@return resulting subtracted vector
        */
        virtual inline const QR_Vector3D& operator -= (const QR_Vector3D& other);

        /**
        * Operator *= for single value
        *@param value - value to multiply
        *@return resulting multiplied vector
        */
        virtual inline const QR_Vector3D& operator *= (T value);

        /**
        * Operator *=
        *@param other - other vector to multiply
        *@return resulting multiplied vector
        */
        virtual inline const QR_Vector3D& operator *= (const QR_Vector3D& other);

        /**
        * Operator /= for single value
        *@param value - value to divide
        *@return resulting divided vector
        */
        virtual inline const QR_Vector3D& operator /= (T value);

        /**
        * Operator /=
        *@param other - other vector to divide
        *@return resulting divided vector
        */
        virtual inline const QR_Vector3D& operator /= (const QR_Vector3D& other);

        /**
        * Operator ==
        *@param other - other vector to compare
        *@return true if vectors are identical, otherwise false
        */
        virtual inline bool operator == (const QR_Vector3D& other) const;

        /**
        * Operator !=
        *@param other - other vector to compare
        *@return true if vectors are not identical, otherwise false
        */
        virtual inline bool operator != (const QR_Vector3D& other) const;

        /**
        * Copies vector
        *@param other - other vector to copy from
        */
        virtual inline void Copy(const QR_Vector3D& other);

        /**
        * Gets the vector length
        *@return the vector length
        */
        virtual inline T Length() const;

        /**
        * Normalizes the vector
        *@return normalized vector
        */
        virtual inline QR_Vector3D Normalize() const;

        /**
        * Calculates the cross product between 2 vectors
        *@param vector - vector with which cross product is calculated
        *@return cross product
        */
        virtual inline QR_Vector3D Cross(const QR_Vector3D& vector) const;

        /**
        * Calculates the dot product between 2 vectors
        *@param vector - vector with which dot product is calculated
        *@return dot product
        */
        virtual inline T Dot(const QR_Vector3D& vector) const;
};

#ifdef USE_QR_GEOMETRY_OLNY
    // formatted vector 3d using float or double
    typedef QR_Vector3D<QR_Float>  QR_Vector3DF;
    typedef QR_Vector3D<QR_Double> QR_Vector3DD;
#else
    // formatted vector 3d using global precision
    typedef QR_Vector3D<M_Precision> QR_Vector3DP;
#endif

//------------------------------------------------------------------------------
// QR_Vector3D implementation - c++ cross-platform
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T>::QR_Vector3D() :
    m_X(0),
    m_Y(0),
    m_Z(0)
{}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T>::QR_Vector3D(const T& x, const T& y, const T& z) :
    m_X(x),
    m_Y(y),
    m_Z(z)
{}
//---------------------------------------------------------------------------
template<class T>
QR_Vector3D<T>::QR_Vector3D(const QR_Vector3D& other)
{
    Copy(other);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T>::~QR_Vector3D()
{}
//---------------------------------------------------------------------------
template<class T>
QR_Vector3D<T>& QR_Vector3D<T>::operator =(const QR_Vector3D& other)
{
    Copy(other);
    return *this;
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T> QR_Vector3D<T>::operator + (T value) const
{
    return QR_Vector3D<T>(m_X + value, m_Y + value, m_Z + value);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T> QR_Vector3D<T>::operator + (const QR_Vector3D<T>& other) const
{
    return QR_Vector3D<T>(m_X + other.m_X, m_Y + other.m_Y, m_Z + other.m_Z);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T> QR_Vector3D<T>::operator - () const
{
    return QR_Vector3D<T>(-m_X, -m_Y, -m_Z);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T> QR_Vector3D<T>::operator - (T value) const
{
    return QR_Vector3D<T>(m_X - value, m_Y - value, m_Z - value);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T> QR_Vector3D<T>::operator - (const QR_Vector3D<T>& other) const
{
    return QR_Vector3D<T>(m_X - other.m_X, m_Y - other.m_Y, m_Z - other.m_Z);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T> QR_Vector3D<T>::operator * (T value) const
{
    return QR_Vector3D<T>(m_X * value, m_Y * value, m_Z * value);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T> QR_Vector3D<T>::operator * (const QR_Vector3D<T>& other) const
{
    return QR_Vector3D<T>(m_X * other.m_X, m_Y * other.m_Y, m_Z * other.m_Z);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T> QR_Vector3D<T>::operator / (T value) const
{
    if (value == 0.0f)
        M_THROW_EXCEPTION("Division by 0 is prohibited");

    return QR_Vector3D<T>(m_X / value, m_Y / value, m_Z / value);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T> QR_Vector3D<T>::operator / (const QR_Vector3D<T>& other) const
{
    if (other.m_X == 0.0f)
        M_THROW_EXCEPTION("Vector x axis - division by 0 is prohibited");

    if (other.m_Y == 0.0f)
        M_THROW_EXCEPTION("Vector y axis - division by 0 is prohibited");

    if (other.m_Z == 0.0f)
        M_THROW_EXCEPTION("Vector z axis - division by 0 is prohibited");

    return QR_Vector3D<T>(m_X / other.m_X, m_Y / other.m_Y, m_Z / other.m_Z);
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector3D<T>& QR_Vector3D<T>::operator += (T value)
{
    m_X += value;
    m_Y += value;
    m_Z += value;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector3D<T>& QR_Vector3D<T>::operator += (const QR_Vector3D<T>& other)
{
    m_X += other.m_X;
    m_Y += other.m_Y;
    m_Z += other.m_Z;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector3D<T>& QR_Vector3D<T>::operator -= (T value)
{
    m_X -= value;
    m_Y -= value;
    m_Z -= value;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector3D<T>& QR_Vector3D<T>::operator -= (const QR_Vector3D<T>& other)
{
    m_X -= other.m_X;
    m_Y -= other.m_Y;
    m_Z -= other.m_Z;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector3D<T>& QR_Vector3D<T>::operator *= (T value)
{
    m_X *= value;
    m_Y *= value;
    m_Z *= value;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector3D<T>& QR_Vector3D<T>::operator *= (const QR_Vector3D<T>& other)
{
    m_X *= other.m_X;
    m_Y *= other.m_Y;
    m_Z *= other.m_Z;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector3D<T>& QR_Vector3D<T>::operator /= (T value)
{
    if (value == 0.0f)
        M_THROW_EXCEPTION("Division by 0 is prohibited");

    m_X /= value;
    m_Y /= value;
    m_Z /= value;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector3D<T>& QR_Vector3D<T>::operator /= (const QR_Vector3D<T>& other)
{
    if (other.m_X == 0.0f)
        M_THROW_EXCEPTION("Vector x axis - division by 0 is prohibited");

    if (other.m_Y == 0.0f)
        M_THROW_EXCEPTION("Vector y axis - division by 0 is prohibited");

    if (other.m_Z == 0.0f)
        M_THROW_EXCEPTION("Vector z axis - division by 0 is prohibited");

    m_X /= other.m_X;
    m_Y /= other.m_Y;
    m_Z /= other.m_Z;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
bool QR_Vector3D<T>::operator == (const QR_Vector3D<T>& other) const
{
    return ((m_X == other.m_X) && (m_Y == other.m_Y) && (m_Z == other.m_Z));
}
//------------------------------------------------------------------------------
template <class T>
bool QR_Vector3D<T>::operator != (const QR_Vector3D<T>& other) const
{
    return ((m_X != other.m_X) || (m_Y != other.m_Y) || (m_Z != other.m_Z));
}
//------------------------------------------------------------------------------
template <class T>
void QR_Vector3D<T>::Copy(const QR_Vector3D& other)
{
    m_X = other.m_X;
    m_Y = other.m_Y;
    m_Z = other.m_Z;
}
//------------------------------------------------------------------------------
template <class T>
T QR_Vector3D<T>::Length() const
{
    return sqrt((m_X * m_X) + (m_Y * m_Y) + (m_Z * m_Z));
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T> QR_Vector3D<T>::Normalize() const
{
    T len = Length();

    if (len == 0.0f)
        return QR_Vector3D<T>();

    return QR_Vector3D<T>((m_X / len), (m_Y / len), (m_Z / len));
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector3D<T> QR_Vector3D<T>::Cross(const QR_Vector3D<T>& vector) const
{
    return QR_Vector3D<T>((m_Y * vector.m_Z) - (vector.m_Y * m_Z),
                          (m_Z * vector.m_X) - (vector.m_Z * m_X),
                          (m_X * vector.m_Y) - (vector.m_X * m_Y));
}
//------------------------------------------------------------------------------
template <class T>
T QR_Vector3D<T>::Dot(const QR_Vector3D<T>& vector) const
{
    return ((m_X * vector.m_X) + (m_Y * vector.m_Y) + (m_Z * vector.m_Z));
}
//------------------------------------------------------------------------------

#endif // QR_Vector3DH
