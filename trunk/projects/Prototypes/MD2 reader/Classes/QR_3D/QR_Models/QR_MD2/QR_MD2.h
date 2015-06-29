/******************************************************************************
 * ==> QR_MD2 ----------------------------------------------------------------*
 ******************************************************************************
 * Description : MD2 model                                                    *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_MD2H
#define QR_MD2H

// std
#include <vector>

// qr engine
#include "QR_Types.h"
#include "QR_Color.h"
#include "QR_Vector3D.h"
#include "QR_Vertex.h"
#include "QR_MD2Parser.h"
#include "QR_Model.h"

/**
* MD2 model
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_MD2 : public QR_Model
{
    public:
        /**
        * Lightning mode
        */
        enum IELightningMode
        {
            IE_LM_None = 0,
            IE_LM_Precalculated,
        };

        /**
        * Precalculated light
        */
        struct ILight
        {
            QR_Color     m_Ambient;
            QR_Color     m_Light;
            QR_Vector3DP m_Direction;

            ILight();

            /**
            * Constructor
            *@param ambient - ambient light color
            *@param light - light color
            *@param direction - light direction
            */
            ILight(const QR_Color&     ambient,
                   const QR_Color&     light,
                   const QR_Vector3DP& direction);

            virtual ~ILight();
        };

        QR_MD2();
        virtual ~QR_MD2();

        /**
        * Loads MD2 from file
        *@param fileName - file name
        *@return 0 on success, otherwise warning (positive value) or error (negative value) code
        */
        virtual QR_Int32 Load(const std::string&  fileName);
        virtual QR_Int32 Load(const std::wstring& fileName);

        /**
        * Loads MD2 from buffer
        *@param buffer - buffer
        *@param length - length to read in buffer, in bytes
        *@return 0 on success, otherwise warning (positive value) or error (negative value) code
        *@note Read will begin from current offset
        */
        virtual QR_Int32 Load(      QR_Buffer&            buffer,
                              const QR_Buffer::ISizeType& length);

        /**
        * Loads normals table from file
        *@param fileName - file name
        *@return 0 on success, otherwise warning (positive value) or error (negative value) code
        */
        virtual QR_Int32 LoadNormals(const std::string&  fileName);
        virtual QR_Int32 LoadNormals(const std::wstring& fileName);

        /**
        * Loads normals table from buffer
        *@param buffer - buffer
        *@param length - length to read in buffer, in bytes (not used here, can be 0)
        *@return 0 on success, otherwise warning (positive value) or error (negative value) code
        */
        virtual QR_Int32 LoadNormals(      QR_Buffer&            buffer,
                                     const QR_Buffer::ISizeType& length);

        /**
        * Checks if left hand to right hand conversion mode is enabled
        *@return true if left hand to right hand conversion is enabled, otherwise false
        */
        virtual bool IsRHLHConversionEnabled() const;

        /**
        * Enables or disables left hand to right hand conversion mode
        *@param value - if true, conversion mode is enabled, disabled otherwise
        */
        virtual void EnableRHLHConversion(bool value);

        /**
        * Gets lightning mode
        *@return lightning mode
        */
        virtual IELightningMode GetLightningMode() const;

        /**
        * Sets lightning mode
        *@param mode - lightning mode
        */
        virtual void SetLightningMode(IELightningMode mode);

        /**
        * Sets pre-calculated light
        *@param light - pre-calculated light
        *@note Lightning mode should be set to IE_LM_Precalculated
        */
        virtual void SetLight(const ILight& light);

        /**
        * Gets vertices
        *@param index - vertices index to get
        *@param[out] vertices - vertices
        *@return 0 on success, otherwise warning (positive value) or error (negative value) code
        *@note vertex buffer content is organized as follow:
        *      [1]x [2]y [3]z [4]nx [5]ny [6]nz [7]tu [8]tv [9]r [10]g [11]b [12]a
        *      where:
        *      x/y/z    - vertex coordinates
        *      nx/ny/nz - vertex normal (if includeNormal is activated)
        *      tu/tv    - vertex texture coordinates(if includeTexture is activated)
        *      r/g/b/a  - vertex color(if includeColor is activated)
        *@note Vertices content should be deleted when useless. WARNING, the output vertices should
        *      be deleted even if function failed
        */
        virtual QR_Int32 GetVertices(QR_SizeT index, QR_Vertices& vertices) const;

        /**
        * Gets vertices count
        *@return vertices count
        */
        virtual QR_SizeT GetVerticesCount() const;

        /**
        * Interpolates vertices
        *@param position - interpolation position, in percent (between 0.0f and 1.0f)
        *@param vertices1 - first vertices to interpolate
        *@param vertices2 - second vertices to interpolate
        *@param[out] result -resulting interpolated vertices
        *@return 0 on success, otherwise warning (positive value) or error (negative value) code
        *@note This function should only be used for compatibility with old OpenGL 1.x versions, as
        *      normally interpolation should be done in vertex shader
        */
        static QR_Int32 Interpolate(const QR_Float& position, const QR_Vertices& vertices1,
                const QR_Vertices& vertices2, QR_Vertices& result);

    protected:
        /**
        * Uncompress MD2 vertex
        *@param pFrame - MD2 frame
        *@param pVertex - MD2 vertex to uncompress
        *@return uncompressed vertex
        */
        virtual QR_Vector3DP UncompressVertex(const QR_MD2Parser::IFrame*  pFrame,
                                              const QR_MD2Parser::IVertex* pVertex) const;

        /**
        * Gets vertex pre-calculated light color
        *@param normal - vertex normal
        *@param light - pre-calculated light
        *@return vertex color
        */
        virtual QR_Color GetPreCalculatedLight(const QR_Vector3DP& normal,
                                               const ILight&       light) const;

    private:
        typedef std::vector<QR_Vector3DP> IVectors3D;

        QR_MD2Parser    m_Mesh;        IVectors3D      m_Normals;
        QR_Color        m_Color;
        IELightningMode m_LightningMode;
        ILight          m_Light;
        bool            m_DoConvertRHLH;
};

#endif // QR_MD2H

