/******************************************************************************
 * ==> QR_Vector4D -----------------------------------------------------------*
 ******************************************************************************
 * Description : Euclidean vector 3D with special w component                 *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_Vector4DH
#define QR_Vector4DH

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
class QR_Vector4D
{
    public:
        T m_X; // x coordinate for the 3D vector
        T m_Y; // y coordinate for the 3D vector
        T m_Z; // z coordinate for the 3D vector
        T m_W; // special w component used e.g. in some animation cases

        /**
        * Constructor
        */
        inline QR_Vector4D();

        /**
        * Constructor
        *@param x - vector x value
        *@param y - vector y value
        *@param z - vector z value
        *@param w - vector w value
        */
        inline QR_Vector4D(const T& x, const T& y, const T& z, const T& w);

        /**
        * Copy constructor
        *@param other - other vector to copy from
        */
        inline QR_Vector4D(const QR_Vector4D& other);

        /**
        * Destructor
        */
        virtual inline ~QR_Vector4D();

        /**
        * Copy operator
        *@param other - other vector to copy from
        *@return this vector
        */
        virtual inline QR_Vector4D& operator =(const QR_Vector4D& other);

        /**
        * Operator + for single value
        *@param value - value to add
        *@return resulting added vector
        */
        virtual inline QR_Vector4D operator + (T value) const;

        /**
        * Operator +
        *@param other - other vector to add
        *@return resulting added vector
        */
        virtual inline QR_Vector4D operator + (const QR_Vector4D& other) const;

        /**
        * Operator -
        *@return inverted vector
        */
        virtual inline QR_Vector4D operator - () const;

        /**
        * Operator - for single value
        *@param value - value to subtract
        *@return resulting subtracted vector
        */
        virtual inline QR_Vector4D operator - (T value) const;

        /**
        * Operator -
        *@param other - other vector to subtract
        *@return resulting subtracted vector
        */
        virtual inline QR_Vector4D operator - (const QR_Vector4D& other) const;

        /**
        * Operator * for single value
        *@param value - value to multiply
        *@return resulting multiplied vector
        */
        virtual inline QR_Vector4D operator * (T value) const;

        /**
        * Operator *
        *@param other - other vector to multiply
        *@return resulting multiplied vector
        */
        virtual inline QR_Vector4D operator * (const QR_Vector4D& other) const;

        /**
        * Operator / for single value
        *@param value - value to divide
        *@return resulting divided vector
        */
        virtual inline QR_Vector4D operator / (T value) const;

        /**
        * Operator /
        *@param other - other vector to divide
        *@return resulting divided vector
        */
        virtual inline QR_Vector4D operator / (const QR_Vector4D& other) const;

        /**
        * Operator += for single value
        *@param value - value to add
        *@return resulting added vector
        */
        virtual inline const QR_Vector4D& operator += (T value);

        /**
        * Operator +=
        *@param other - other vector to add
        *@return resulting added vector
        */
        virtual inline const QR_Vector4D& operator += (const QR_Vector4D& other);

        /**
        * Operator -= for single value
        *@param value - value to subtract
        *@return resulting subtracted vector
        */
        virtual inline const QR_Vector4D& operator -= (T value);

        /**
        * Operator -=
        *@param other - other vector to subtract
        *@return resulting subtracted vector
        */
        virtual inline const QR_Vector4D& operator -= (const QR_Vector4D& other);

        /**
        * Operator *= for single value
        *@param value - value to multiply
        *@return resulting multiplied vector
        */
        virtual inline const QR_Vector4D& operator *= (T value);

        /**
        * Operator *=
        *@param other - other vector to multiply
        *@return resulting multiplied vector
        */
        virtual inline const QR_Vector4D& operator *= (const QR_Vector4D& other);

        /**
        * Operator /= for single value
        *@param value - value to divide
        *@return resulting divided vector
        */
        virtual inline const QR_Vector4D& operator /= (T value);

        /**
        * Operator /=
        *@param other - other vector to divide
        *@return resulting divided vector
        */
        virtual inline const QR_Vector4D& operator /= (const QR_Vector4D& other);

        /**
        * Operator ==
        *@param other - other vector to compare
        *@return true if vectors are identical, otherwise false
        */
        virtual inline bool operator == (const QR_Vector4D& other) const;

        /**
        * Operator !=
        *@param other - other vector to compare
        *@return true if vectors are not identical, otherwise false
        */
        virtual inline bool operator != (const QR_Vector4D& other) const;

        /**
        * Copies vector
        *@param other - other vector to copy from
        */
        virtual inline void Copy(const QR_Vector4D& other);

        /**
        * Gets the vector length
        *@return the vector length
        */
        virtual inline T Length() const;

        /**
        * Normalizes the vector
        *@return normalized vector
        */
        virtual inline QR_Vector4D Normalize() const;

        /**
        * Calculates the cross product between 2 vectors
        *@param u - first vector with which cross product is calculated
        *@param v - second vector with which cross product is calculated
        *@param w - third vector with which cross product is calculated
        *@return cross product
        */
        virtual inline QR_Vector4D Cross(const QR_Vector4D& u,
                                         const QR_Vector4D& v,
                                         const QR_Vector4D& w) const;

        /**
        * Calculates the dot product between 2 vectors
        *@param vector - vector with which dot product is calculated
        *@return dot product
        */
        virtual inline T Dot(const QR_Vector4D& vector) const;
};

#ifdef USE_QR_GEOMETRY_OLNY
    // formatted quaternion using float or double
    typedef QR_Vector4D<QR_Float>  QR_Vector4DF;
    typedef QR_Vector4D<QR_Double> QR_Vector4DD;
#else
    // formatted vector 3d using global precision
    typedef QR_Vector4D<M_Precision> QR_Vector4DP;
#endif

//------------------------------------------------------------------------------
// QR_Vector4D implementation - c++ cross-platform
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T>::QR_Vector4D() :
    m_X(0),
    m_Y(0),
    m_Z(0),
    m_W(0)
{}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T>::QR_Vector4D(const T& x, const T& y, const T& z, const T& w) :
    m_X(x),
    m_Y(y),
    m_Z(z),
    m_W(w)
{}
//---------------------------------------------------------------------------
template<class T>
QR_Vector4D<T>::QR_Vector4D(const QR_Vector4D& other)
{
    Copy(other);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T>::~QR_Vector4D()
{}
//---------------------------------------------------------------------------
template<class T>
QR_Vector4D<T>& QR_Vector4D<T>::operator =(const QR_Vector4D& other)
{
    Copy(other);
    return *this;
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T> QR_Vector4D<T>::operator + (T value) const
{
    return QR_Vector4D<T>(m_X + value, m_Y + value, m_Z + value, m_W + value);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T> QR_Vector4D<T>::operator + (const QR_Vector4D<T>& other) const
{
    return QR_Vector4D<T>(m_X + other.m_X, m_Y + other.m_Y, m_Z + other.m_Z, m_W + other.m_W);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T> QR_Vector4D<T>::operator - () const
{
    return QR_Vector4D<T>(-m_X, -m_Y, -m_Z, -m_W);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T> QR_Vector4D<T>::operator - (T value) const
{
    return QR_Vector4D<T>(m_X - value, m_Y - value, m_Z - value, m_W - value);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T> QR_Vector4D<T>::operator - (const QR_Vector4D<T>& other) const
{
    return QR_Vector4D<T>(m_X - other.m_X, m_Y - other.m_Y, m_Z - other.m_Z, m_W - other.m_W);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T> QR_Vector4D<T>::operator * (T value) const
{
    return QR_Vector4D<T>(m_X * value, m_Y * value, m_Z * value, m_W * value);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T> QR_Vector4D<T>::operator * (const QR_Vector4D<T>& other) const
{
    return QR_Vector4D<T>(m_X * other.m_X, m_Y * other.m_Y, m_Z * other.m_Z, m_W * other.m_W);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T> QR_Vector4D<T>::operator / (T value) const
{
    if (value == 0.0f)
        M_THROW_EXCEPTION("Division by 0 is prohibited");

    return QR_Vector4D<T>(m_X / value, m_Y / value, m_Z / value, m_W / value);
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T> QR_Vector4D<T>::operator / (const QR_Vector4D<T>& other) const
{
    if (other.m_X == 0.0f)
        M_THROW_EXCEPTION("Vector x axis - division by 0 is prohibited");

    if (other.m_Y == 0.0f)
        M_THROW_EXCEPTION("Vector y axis - division by 0 is prohibited");

    if (other.m_Z == 0.0f)
        M_THROW_EXCEPTION("Vector z axis - division by 0 is prohibited");

    if (other.m_W == 0.0f)
        M_THROW_EXCEPTION("Vector z axis - division by 0 is prohibited");

    return QR_Vector4D<T>(m_X / other.m_X, m_Y / other.m_Y, m_Z / other.m_Z, m_W / other.m_W);
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector4D<T>& QR_Vector4D<T>::operator += (T value)
{
    m_X += value;
    m_Y += value;
    m_Z += value;
    m_W += value;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector4D<T>& QR_Vector4D<T>::operator += (const QR_Vector4D<T>& other)
{
    m_X += other.m_X;
    m_Y += other.m_Y;
    m_Z += other.m_Z;
    m_W += other.m_W;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector4D<T>& QR_Vector4D<T>::operator -= (T value)
{
    m_X -= value;
    m_Y -= value;
    m_Z -= value;
    m_W -= value;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector4D<T>& QR_Vector4D<T>::operator -= (const QR_Vector4D<T>& other)
{
    m_X -= other.m_X;
    m_Y -= other.m_Y;
    m_Z -= other.m_Z;
    m_W -= other.m_W;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector4D<T>& QR_Vector4D<T>::operator *= (T value)
{
    m_X *= value;
    m_Y *= value;
    m_Z *= value;
    m_W *= value;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector4D<T>& QR_Vector4D<T>::operator *= (const QR_Vector4D<T>& other)
{
    m_X *= other.m_X;
    m_Y *= other.m_Y;
    m_Z *= other.m_Z;
    m_W *= other.m_W;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector4D<T>& QR_Vector4D<T>::operator /= (T value)
{
    if (value == 0.0f)
        M_THROW_EXCEPTION("Division by 0 is prohibited");

    m_X /= value;
    m_Y /= value;
    m_Z /= value;
    m_W /= value;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
const QR_Vector4D<T>& QR_Vector4D<T>::operator /= (const QR_Vector4D<T>& other)
{
    if (other.m_X == 0.0f)
        M_THROW_EXCEPTION("Vector x axis - division by 0 is prohibited");

    if (other.m_Y == 0.0f)
        M_THROW_EXCEPTION("Vector y axis - division by 0 is prohibited");

    if (other.m_Z == 0.0f)
        M_THROW_EXCEPTION("Vector z axis - division by 0 is prohibited");

    if (other.m_W == 0.0f)
        M_THROW_EXCEPTION("Vector z axis - division by 0 is prohibited");

    m_X /= other.m_X;
    m_Y /= other.m_Y;
    m_Z /= other.m_Z;
    m_W /= other.m_W;

    return *this;
}
//------------------------------------------------------------------------------
template <class T>
bool QR_Vector4D<T>::operator == (const QR_Vector4D<T>& other) const
{
    return ((m_X == other.m_X) && (m_Y == other.m_Y) && (m_Z == other.m_Z) && (m_W == other.m_W));
}
//------------------------------------------------------------------------------
template <class T>
bool QR_Vector4D<T>::operator != (const QR_Vector4D<T>& other) const
{
    return ((m_X != other.m_X) || (m_Y != other.m_Y) || (m_Z != other.m_Z) || (m_W != other.m_W));
}
//------------------------------------------------------------------------------
template <class T>
void QR_Vector4D<T>::Copy(const QR_Vector4D& other)
{
    m_X = other.m_X;
    m_Y = other.m_Y;
    m_Z = other.m_Z;
    m_W = other.m_W;
}
//------------------------------------------------------------------------------
template <class T>
T QR_Vector4D<T>::Length() const
{
    return sqrt((m_X * m_X) + (m_Y * m_Y) + (m_Z * m_Z) + (m_W * m_W));
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T> QR_Vector4D<T>::Normalize() const
{
    T len = Length();

    if (len == 0.0f)
        return QR_Vector4D<T>();

    return QR_Vector4D<T>((m_X / len), (m_Y / len), (m_Z / len), (m_W / len));
}
//------------------------------------------------------------------------------
template <class T>
QR_Vector4D<T> QR_Vector4D<T>::Cross(const QR_Vector4D<T>& u,
                                     const QR_Vector4D<T>& v,
                                     const QR_Vector4D<T>& w) const
{
    // calculate intermediate values
    T a = (v.m_X * w.m_Y) - (v.m_Y * w.m_X);
    T b = (v.m_X * w.m_Z) - (v.m_Z * w.m_X);
    T c = (v.m_X * w.m_W) - (v.m_W * w.m_X);
    T d = (v.m_Y * w.m_Z) - (v.m_Z * w.m_Y);
    T e = (v.m_Y * w.m_W) - (v.m_W * w.m_Y);
    T f = (v.m_Z * w.m_W) - (v.m_W * w.m_Z);

    QR_Vector4D<T> result;

    // calculate resulting vector components
    result.m_X =  (u.m_Y * f) - (u.m_Z * e) + (u.m_W * d);
    result.m_Y = -(u.m_X * f) + (u.m_Z * c) - (u.m_W * b);
    result.m_Z =  (u.m_X * e) - (u.m_Y * c) + (u.m_W * a);
    result.m_W = -(u.m_X * d) + (u.m_Y * b) - (u.m_Z * a);

    return result;
}
//------------------------------------------------------------------------------
template <class T>
T QR_Vector4D<T>::Dot(const QR_Vector4D<T>& vector) const
{
    return ((m_X * vector.m_X) + (m_Y * vector.m_Y) + (m_Z * vector.m_Z) + (m_W * vector.m_W));
}
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
// RAD studio
//---------------------------------------------------------------------------
#ifdef __CODEGEARC__
    // needed to avoid the W8058 error "Cannot create pre-compiled header: header incomplete" warning in BCC compilers
    ;
#endif
//---------------------------------------------------------------------------

#endif // QR_Vector4DH
