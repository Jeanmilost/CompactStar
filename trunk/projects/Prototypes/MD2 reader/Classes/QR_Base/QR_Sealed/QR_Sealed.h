/******************************************************************************
 * ==> QR_Sealed -------------------------------------------------------------*
 ******************************************************************************
 * Description : Class to make a child class sealed                           *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_SealedH
#define QR_SealedH

//------------------------------------------------------------------------------
// Global defines
//------------------------------------------------------------------------------
#define QR_Sealed(className) private virtual QR_SealedBase<className>
//------------------------------------------------------------------------------

/**
* Class to make a child class sealed
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
template <class T>
class QR_SealedBase
{
    protected:
        /**
        * Constructor
        */
        QR_SealedBase();
};

//------------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------------
template <class T>
QR_SealedBase<T>::QR_SealedBase()
{}
//------------------------------------------------------------------------------

#endif // QR_SealedH
