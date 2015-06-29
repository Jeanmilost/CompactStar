/******************************************************************************
 * ==> QR_Model --------------------------------------------------------------*
 ******************************************************************************
 * Description : Basic 3D model, can be a sprite, a MD2, ...                  *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_ModelH
#define QR_ModelH

// qr engine
#include "QR_Types.h"
#include "QR_Vector3D.h"
#include "QR_Matrix16.h"

/**
* Basic 3D model
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_Model
{
    public:
        /**
        * Matrix combination type enumeration
        */
        enum IECombinationType
        {
            IE_CT_Scale_Rotate_Translate,
            IE_CT_Scale_Translate_Rotate,
            IE_CT_Rotate_Translate_Scale,
            IE_CT_Rotate_Scale_Translate,
            IE_CT_Translate_Rotate_Scale,
            IE_CT_Translate_Scale_Rotate,
        };

        /**
        * Rotation data
        */
        struct IRotation
        {
            M_Precision  m_Angle;
            QR_Vector3DP m_Axis;

            /**
            * Constructor
            */
            IRotation();

            /**
            * Constructor
            *@param angle - angle
            *@param axis - rotation axis
            */
            IRotation(const M_Precision& angle, const QR_Vector3DP& axis);

            /**
            * Destructor
            */
            virtual ~IRotation();
        };

        QR_Model();
        virtual ~QR_Model();

        /**
        * Gets translation
        *@return Translation
        */
        virtual QR_Vector3DP GetTranslation() const;

        /**
        * Sets translation
        *@param Translation - Translation
        */
        virtual void SetTranslation(const QR_Vector3DP& translation);

        /**
        * Gets rotation on X axis
        *@return rotation angle on X axis in radians
        */
        virtual M_Precision GetRotationX() const;

        /**
        * Sets rotation on X axis
        *@param angle - rotation angle in radians
        */
        virtual void SetRotationX(const M_Precision& angle);

        /**
        * Gets rotation on Y axis
        *@return rotation angle on Y axis in radians
        */
        virtual M_Precision GetRotationY() const;

        /**
        * Sets rotation on Y axis
        *@param angle - rotation angle in radians
        */
        virtual void SetRotationY(const M_Precision& angle);

        /**
        * Gets rotation on Z axis
        *@return rotation angle on Z axis in radians
        */
        virtual M_Precision GetRotationZ() const;

        /**
        * Sets rotation on Z axis
        *@param angle - rotation angle in radians
        */
        virtual void SetRotationZ(const M_Precision& angle);

        /**
        * Gets scaling
        *@return scaling
        */
        virtual QR_Vector3DP GetScaling() const;

        /**
        * Sets scaling
        *@param scaling - scaling
        */
        virtual void SetScaling(const QR_Vector3DP& scaling);

        /**
        * Gets model matrix
        *@param type - combination type
        *@param pInitialMatrix - initial model matrix to start from, can be NULL
        *@return matrix
        */
        virtual QR_Matrix16P GetMatrix(IECombinationType   type           = IE_CT_Scale_Rotate_Translate,
                                       const QR_Matrix16P* pInitialMatrix = NULL) const;

    private:
        QR_Vector3DP m_Scaling;
        QR_Vector3DP m_Translation;
        IRotation    m_RotationX;
        IRotation    m_RotationY;
        IRotation    m_RotationZ;
};

#endif // QR_ModelH
