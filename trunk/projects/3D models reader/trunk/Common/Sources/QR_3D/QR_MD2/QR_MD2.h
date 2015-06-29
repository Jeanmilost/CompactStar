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

//------------------------------------------------------------------------------
// Global defines
//------------------------------------------------------------------------------
#define M_MD2_Normals_Table_File_Version 1.0f
//------------------------------------------------------------------------------

/**
* MD2 model
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_MD2 : public QR_Model
{
    public:
        typedef M_Precision            IDataType;
        typedef std::vector<IDataType> IBuffer;

        /**
        * Vertex buffer
        */
        struct IVertexBuffer
        {
            IBuffer           m_Buffer;
            QR_Vertex::IEType m_Type;

            IVertexBuffer();
            virtual ~IVertexBuffer();
        };

        typedef std::vector<IVertexBuffer*> IVertexBufferList;

        /**
        * Lightning mode
        */
        enum IELightningMode
        {
            IE_L_Unknown = 0,
            IE_L_None,
            IE_L_Precalculated,
            IE_L_Native,
        };

        /**
        * Precalculated light values
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

        /**
        * Animation info structure
        */
        struct IAnimationInfo
        {
            QR_SizeT    m_Start;
            QR_SizeT    m_End;
            std::string m_Name;

            IAnimationInfo();

            /**
            * Constructor
            *@param start - start frame index
            *@param end - end frame index
            *@param name - frame name
            */
            IAnimationInfo(QR_SizeT start, QR_SizeT end, const std::string& name);

            virtual ~IAnimationInfo();
        };

        QR_MD2();
        virtual ~QR_MD2();

        /**
        * Loads MD2 mesh file
        *@param fileName - file name
        *@param version - file version
        *@return true on success, otherwise false        */
        virtual bool LoadMesh(const std::string& fileName,
                              const QR_Float&    version = M_MD2_Mesh_File_Version);

        /**
        * Loads normals table from file
        *@param fileName - file name
        *@param version - file version
        *@return true on success, otherwise false        */
        virtual bool LoadNormalsTable(const std::string& fileName,
                                      const QR_Float&    version = M_MD2_Normals_Table_File_Version);

        /**
        * Set animation
        *@param animationInfo - animation info to set
        *@return true on success, otherwise false
        */
        virtual bool SetAnimation(IAnimationInfo animationInfo);

        /**
        * Animate mesh
        *@param elapsedTime - elapsed time since last frame was drawn
        *@param fps - number of frames per seconds
        *@return true on success, otherwise false
        */
        virtual bool Animate(const QR_Double& elapsedTime, QR_SizeT fps);

        /**
        * Gets vertex buffer
        *@param[out] vb - vertex buffer data to populate
        *@param lightningMode - lightning mode
        *@param preCalculatedLight - pre-calculated light, ignored if lighting mode isn't IE_L_Precalculated
        *@param vertexFormat - vertex format (i.e what data the vertex contains)
        *@param convertRHLH - if true, left hand system will be converted to right hand or vice-versa
        *@return true on success, otherwise false
        *@note vertex buffer content is organized as follow:
        *      [1]x [2]y [3]z [4]nx [5]ny [6]nz [7]tu [8]tv [9]r [10]g [11]b [12]a
        *      where:
        *      x/y/z    - vertex coordinates
        *      nx/ny/nz - vertex normal
        *      tu/tv    - vertex texture coordinates
        *      r/g/b/a  - vertex color
        *@note This function ignores completely the OpenGL commands and write
        *      the buffer content as a triangle list
        *@note This function is obsolete, and should no more be used
        */
        #ifdef QRENGINE_USE_OBSOLETE_FUNCTIONS
            virtual bool GetVertexBuffer(IBuffer&            vb,
                                         IELightningMode     lightningMode,
                                         const ILight&       preCalculatedLight,
                                         QR_Vertex::IEFormat vertexFormat,
                                         bool                convertRHLH) const;
        #endif

        /**
        * Gets vertex buffer
        *@param[out] vbs - vertex buffer list to populate
        *@param lightningMode - lightning mode
        *@param preCalculatedLight - pre-calculated light
        *@param vertexFormat - vertex format (i.e what data the vertex contains)
        *@param convertRHLH - if true, left hand system will be converted to right hand or vice-versa
        *@return true on success, otherwise false
        *@note vertex buffer content is organized as follow:
        *      [1]x [2]y [3]z [4]nx [5]ny [6]nz [7]tu [8]tv [9]r [10]g [11]b [12]a
        *      where:
        *      x/y/z    - vertex coordinates
        *      nx/ny/nz - vertex normal (if includeNormal is activated)
        *      tu/tv    - vertex texture coordinates(if includeTexture is activated)
        *      r/g/b/a  - vertex color(if includeColor is activated)
        *@note Pre-calculated light will be ignored if lightning mode is other
        *      than IE_L_Precalculated
        *@note includeNormal and includeColor should at least be activated to
        *      use precalculated light
        *@note Vertex buffer list content must be deleted when useless. WARNING,
        *      the returned list can contain data to delete even if function failed
        */
        virtual bool GetVertexBuffer(IVertexBufferList&  vbs,
                                     IELightningMode     lightningMode,
                                     const ILight&       preCalculatedLight,
                                     QR_Vertex::IEFormat vertexFormat,
                                     bool                convertRHLH) const;

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
        * Gets vertex color
        *@param normal - vertex normal
        *@param lightningMode - lightning mode
        *@param preCalculatedLight - pre-calculated light, ignored if lighting mode isn't IE_L_Precalculated
        *@return vertex color
        */
        virtual QR_Color GetVertexColor(const QR_Vector3DP& normal,
                                        IELightningMode     lightningMode,
                                        const ILight&       preCalculatedLight) const;

        /**
        * Gets vertex pre-calculated light color
        *@param normal - vertex normal
        *@param light - pre-calculated light
        *@return vertex color
        */
        virtual QR_Color GetPreCalculatedLight(const QR_Vector3DP& normal,
                                               const ILight&       light) const;

        /**
        * Calculates interpolation vector between 2 vectors
        *@param v1 - first vector to interpolate
        *@param v2 - second vector to interpolate
        *@param position - interpolation position
        *@return interpolation vector
        */
        virtual QR_Vector3DP Interpolate(const QR_Vector3DP& v1,
                                         const QR_Vector3DP& v2,
                                         const M_Precision&  position) const;

    private:
        typedef std::vector<QR_Vector3DP> IVector3DList;

        QR_MD2Parser   m_Mesh;        IVector3DList  m_Normals;
        IAnimationInfo m_AnimationInfo;
        QR_SizeT       m_CurFrameID;
        QR_SizeT       m_NextFrameID;
        QR_Float       m_InterpolationPos;
};

#endif // QR_MD2H
