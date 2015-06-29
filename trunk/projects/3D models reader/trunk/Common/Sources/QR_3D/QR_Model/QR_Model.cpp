/******************************************************************************
 * ==> QR_Model --------------------------------------------------------------*
 ******************************************************************************
 * Description : Basic 3D model, can be a sprite, a MD2, ...                  *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QR_Model.h"

//------------------------------------------------------------------------------
// Struct QR_Model::IRotation - c++ cross-platform
//------------------------------------------------------------------------------
QR_Model::IRotation::IRotation() : m_Angle(0.0f)
{}
//------------------------------------------------------------------------------
QR_Model::IRotation::IRotation(const M_Precision& angle, const QR_Vector3DP& axis) :
    m_Angle(angle),
    m_Axis(axis)
{}
//------------------------------------------------------------------------------
QR_Model::IRotation::~IRotation()
{}
//------------------------------------------------------------------------------
// Class QR_Model - c++ cross-platform
//------------------------------------------------------------------------------
QR_Model::QR_Model() :
    m_Scaling(QR_Vector3DP(1.0f, 1.0f, 1.0f)),
    m_Translation(QR_Vector3DP(1.0f, 1.0f, 1.0f)),
    m_RotationX(IRotation(0.0f, QR_Vector3DP(1.0f, 0.0f, 0.0f))),
    m_RotationY(IRotation(0.0f, QR_Vector3DP(0.0f, 1.0f, 0.0f))),
    m_RotationZ(IRotation(0.0f, QR_Vector3DP(0.0f, 0.0f, 1.0f)))
{}
//------------------------------------------------------------------------------
QR_Model::~QR_Model()
{}
//------------------------------------------------------------------------------
QR_Vector3DP QR_Model::GetTranslation() const
{
    return m_Translation;
}
//------------------------------------------------------------------------------
void QR_Model::SetTranslation(const QR_Vector3DP& translation)
{
    m_Translation = translation;
}
//------------------------------------------------------------------------------
M_Precision QR_Model::GetRotationX() const
{
    return m_RotationX.m_Angle;
}
//------------------------------------------------------------------------------
void QR_Model::SetRotationX(const M_Precision& angle)
{
    m_RotationX.m_Angle = angle;
}
//------------------------------------------------------------------------------
M_Precision QR_Model::GetRotationY() const
{
    return m_RotationY.m_Angle;
}
//------------------------------------------------------------------------------
void QR_Model::SetRotationY(const M_Precision& angle)
{
    m_RotationY.m_Angle = angle;
}
//------------------------------------------------------------------------------
M_Precision QR_Model::GetRotationZ() const
{
    return m_RotationZ.m_Angle;
}
//------------------------------------------------------------------------------
void QR_Model::SetRotationZ(const M_Precision& angle)
{
    m_RotationZ.m_Angle = angle;
}
//------------------------------------------------------------------------------
QR_Vector3DP QR_Model::GetScaling() const
{
    return m_Scaling;
}
//------------------------------------------------------------------------------
void QR_Model::SetScaling(const QR_Vector3DP& scaling)
{
    m_Scaling = scaling;
}
//------------------------------------------------------------------------------
QR_Matrix16P QR_Model::GetMatrix(IECombinationType   type,
                                 const QR_Matrix16P* pInitialMatrix) const
{
    // initialize matrix
    QR_Matrix16P scaleMatrix     = QR_Matrix16P::Identity();
    QR_Matrix16P rotateXMatrix   = QR_Matrix16P::Identity();
    QR_Matrix16P rotateYMatrix   = QR_Matrix16P::Identity();
    QR_Matrix16P rotateZMatrix   = QR_Matrix16P::Identity();
    QR_Matrix16P translateMatrix = QR_Matrix16P::Identity();
    QR_Matrix16P modelMatrix     = pInitialMatrix ? *pInitialMatrix : QR_Matrix16P::Identity();

    // build scaling, rotation and translation matrix
    scaleMatrix.Scale(m_Scaling);
    rotateXMatrix.Rotate(m_RotationX.m_Angle, m_RotationX.m_Axis);
    rotateYMatrix.Rotate(m_RotationY.m_Angle, m_RotationY.m_Axis);
    rotateZMatrix.Rotate(m_RotationZ.m_Angle, m_RotationZ.m_Axis);
    translateMatrix.Translate(m_Translation);

    // build model matrix
    switch (type)
    {
        case IE_CT_Scale_Rotate_Translate:
            modelMatrix = modelMatrix.Multiply(scaleMatrix);
            modelMatrix = modelMatrix.Multiply(rotateXMatrix);
            modelMatrix = modelMatrix.Multiply(rotateYMatrix);
            modelMatrix = modelMatrix.Multiply(rotateZMatrix);
            modelMatrix = modelMatrix.Multiply(translateMatrix);
            break;

        case IE_CT_Scale_Translate_Rotate:
            modelMatrix = modelMatrix.Multiply(scaleMatrix);
            modelMatrix = modelMatrix.Multiply(translateMatrix);
            modelMatrix = modelMatrix.Multiply(rotateXMatrix);
            modelMatrix = modelMatrix.Multiply(rotateYMatrix);
            modelMatrix = modelMatrix.Multiply(rotateZMatrix);
            break;

        case IE_CT_Rotate_Translate_Scale:
            modelMatrix = modelMatrix.Multiply(rotateXMatrix);
            modelMatrix = modelMatrix.Multiply(rotateYMatrix);
            modelMatrix = modelMatrix.Multiply(rotateZMatrix);
            modelMatrix = modelMatrix.Multiply(translateMatrix);
            modelMatrix = modelMatrix.Multiply(scaleMatrix);
            break;

        case IE_CT_Rotate_Scale_Translate:
            modelMatrix = modelMatrix.Multiply(rotateXMatrix);
            modelMatrix = modelMatrix.Multiply(rotateYMatrix);
            modelMatrix = modelMatrix.Multiply(rotateZMatrix);
            modelMatrix = modelMatrix.Multiply(scaleMatrix);
            modelMatrix = modelMatrix.Multiply(translateMatrix);
            break;

        case IE_CT_Translate_Rotate_Scale:
            modelMatrix = modelMatrix.Multiply(translateMatrix);
            modelMatrix = modelMatrix.Multiply(rotateXMatrix);
            modelMatrix = modelMatrix.Multiply(rotateYMatrix);
            modelMatrix = modelMatrix.Multiply(rotateZMatrix);
            modelMatrix = modelMatrix.Multiply(scaleMatrix);
            break;

        case IE_CT_Translate_Scale_Rotate:
            modelMatrix = modelMatrix.Multiply(translateMatrix);
            modelMatrix = modelMatrix.Multiply(scaleMatrix);
            modelMatrix = modelMatrix.Multiply(rotateXMatrix);
            modelMatrix = modelMatrix.Multiply(rotateYMatrix);
            modelMatrix = modelMatrix.Multiply(rotateZMatrix);
            break;

        default:
            M_THROW_EXCEPTION("Unknown model matrix combination type");
    }

    return modelMatrix;
}
//------------------------------------------------------------------------------

