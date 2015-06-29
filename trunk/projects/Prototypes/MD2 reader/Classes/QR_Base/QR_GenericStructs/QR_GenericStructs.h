/******************************************************************************
 * ==> QR_GenericStructs -----------------------------------------------------*
 ******************************************************************************
 * Description : Generic structures used in many classes                      *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_GenericStructsH
#define QR_GenericStructsH

// std
#include <vector>

/**
* Structure representing a point
*@author Jean-Milost Reymond
*/
template <class T>
struct QR_Point
{
    T m_X;
    T m_Y;

    inline QR_Point();
    virtual inline ~QR_Point();
};

//---------------------------------------------------------------------------
// QR_Point - c++ cross-platform
//---------------------------------------------------------------------------
template <class T>
QR_Point<T>::QR_Point() :
    m_X(0),
    m_Y(0)
{}
//---------------------------------------------------------------------------
template <class T>
QR_Point<T>::~QR_Point()
{}
//---------------------------------------------------------------------------

typedef std::vector<QR_Point*> QR_Points;

/**
* Structure representing a size
*@author Jean-Milost Reymond
*/
template <class T>
struct QR_Size
{
    T m_Width;
    T m_Height;

    inline QR_Size();
    virtual inline ~QR_Size();
};

//---------------------------------------------------------------------------
// QR_Size - c++ cross-platform
//---------------------------------------------------------------------------
template <class T>
QR_Size<T>::QR_Size() :
    m_Width(0),
    m_Height(0)
{}
//---------------------------------------------------------------------------
template <class T>
QR_Size<T>::~QR_Size()
{}
//---------------------------------------------------------------------------

typedef std::vector<QR_Size*> QR_Sizes;

/**
* Structure representing a 2D coordinate
*@author Jean-Milost Reymond
*/
template <class T>
struct QR_Coord2D
{
    T m_X;
    T m_Y;

    inline QR_Coord2D();
    virtual inline ~QR_Coord2D();
};

//---------------------------------------------------------------------------
// QR_Coord2D - c++ cross-platform
//---------------------------------------------------------------------------
template <class T>
QR_Coord2D<T>::QR_Coord2D() :
    m_X(0),
    m_Y(0)
{}
//---------------------------------------------------------------------------
template <class T>
QR_Coord2D<T>::~QR_Coord2D()
{}
//---------------------------------------------------------------------------

typedef std::vector<QR_Coord2D*> QR_Coords2D;

/**
* Structure representing a 3D coordinate
*@author Jean-Milost Reymond
*/
template <class T>
struct QR_Coord3D
{
    T m_X;
    T m_Y;
    T m_Z;

    inline QR_Coord3D();
    virtual inline ~QR_Coord3D();
};

//---------------------------------------------------------------------------
// QR_Coord3D - c++ cross-platform
//---------------------------------------------------------------------------
template <class T>
QR_Coord3D<T>::QR_Coord3D() :
    m_X(0),
    m_Y(0)
{}
//---------------------------------------------------------------------------
template <class T>
QR_Coord3D<T>::~QR_Coord3D()
{}
//---------------------------------------------------------------------------

typedef std::vector<QR_Coord3D*> QR_Coords3D;

/**
* Structure representing a rectangle
*@author Jean-Milost Reymond
*/
template <class T>
struct QR_Rect
{
    T m_X;
    T m_Y;
    T m_Width;
    T m_Height;

    inline QR_Rect();
    virtual inline ~QR_Rect();

    /**
    * Checks if a point or coordinate is inside a rectangle
    *@param point - point to check
    *@param coord - coordinate to check
    *@param x - x coordinate to check
    *@param y - y coordinate to check
    *@return true if point is inside the rectangle, otherwise false
    */
    inline bool Inside(const QR_Point<T>& point) const;
    inline bool Inside(const QR_Coord2D<T>& coord) const;
    inline bool Inside(const T& x, const T& y) const;

    /**
    * Checks if rect is in collision with another rect
    *@param other - other rect to check against
    *@return true if both rects are in collision, otherwise false
    */
    inline bool Intersect(const QR_Rect<T>& other) const;
};

//---------------------------------------------------------------------------
// QR_Rect - c++ cross-platform
//---------------------------------------------------------------------------
template <class T>
QR_Rect<T>::QR_Rect() :
    m_X(0),
    m_Y(0)
{}
//---------------------------------------------------------------------------
template <class T>
QR_Rect<T>::~QR_Rect()
{}
//------------------------------------------------------------------------------
template <class T>
bool QR_Rect<T>::Inside(const QR_Point<T>& point) const
{
    return Inside(point.m_X, point.m_Y);
}
//------------------------------------------------------------------------------
template <class T>
bool QR_Rect<T>::Inside(const QR_Coord2D<T>& coord) const
{
    return Inside(coord.m_X, coord.m_Y);
}
//------------------------------------------------------------------------------
template <class T>
bool QR_Rect<T>::Inside(const T& x, const T& y) const
{
    return (x >=  m_X && y >=  m_Y && x <= (.m_X + m_Width) && y <= (.m_Y + m_Height));
}
//------------------------------------------------------------------------------
template <class T>
bool QR_Rect<T>::Intersect(const QR_Rect<T>& other) const
{
    return !(m_X > (other.m_X + other.m_Width)  || (m_X + m_Width)  < other.m_X ||
             m_Y > (other.m_Y + other.m_Height) || (m_Y + m_Height) < other.m_Y);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// RAD studio
//---------------------------------------------------------------------------
#ifdef __CODEGEARC__
    // needed to avoid the W8058 error "Cannot create pre-compiled header: header incomplete" warning in BCC compilers
    ;
#endif
//---------------------------------------------------------------------------

#endif
