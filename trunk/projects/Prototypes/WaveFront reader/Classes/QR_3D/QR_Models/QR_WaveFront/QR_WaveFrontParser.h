/******************************************************************************
 * ==> QR_WaveFrontParser ----------------------------------------------------*
 ******************************************************************************
 * Description : Reads and exposes WaveFront (.obj) file content              *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#ifndef QR_WaveFrontParserH
#define QR_WaveFrontParserH

// std
#include <string>
#include <vector>
#include <map>

// qr engine
#include "QR_ModelParser.h"
#include "QR_Vector3D.h"
#include "QR_Color.h"

/**
* Reads and exposes WaveFront (.obj) file content
*@note This class is cross-platform
*@author Jean-Milost Reymond
*/
class QR_WaveFrontParser : public QR_ModelParser
{
    public:
        /**
        * Illumination models
        */
        enum IEIlluminationModel
        {
            IE_IM_Color                            = 0,
            IE_IM_ColorAmbient                     = 1,
            IE_IM_Highlight                        = 2,
            IE_IM_ReflectionRayTrace               = 3,
            IE_IM_GlassRayTrace                    = 4,
            IE_IM_FresnelRayTrace                  = 5,
            IE_IM_RefractionRayTrace               = 6,
            IE_IM_RefractionFresnelRayTrace        = 7,
            IE_IM_Reflection                       = 8,
            IE_IM_Glass                            = 9,
            IE_IM_CastShadowsOntoInvisibleSurfaces = 10,
        };

        /**
        * Surface types
        */
        enum IESurfaceType
        {
            IE_ST_Unknown = 0,
            IE_ST_BasisMatrix,
            IE_ST_BSpline,
            IE_ST_Bezier,
            IE_ST_Cardinal,
            IE_ST_Taylor,
        };

        /**
        * Texture options
        */
        struct ITextureOptions
        {
            float        m_BoostMipMapSharpness;
            float        m_TextureMapBrightness;
            float        m_TextureMapContrast;
            float        m_BumpMultiplier;
            QR_Vector3DP m_OriginOffset;
            QR_Vector3DP m_Scale;
            QR_Vector3DP m_Turbulence;
            float        m_Resolution;
            bool         m_BlendU;
            bool         m_BlendV;
            bool         m_Clamp;
            std::string  m_Channel;

            ITextureOptions();
            virtual ~ITextureOptions();
        };

        /**
        * Texture maps
        */
        struct ITextureMap
        {
            std::wstring    m_Ambient;
            std::wstring    m_Diffuse;
            std::wstring    m_Specular;
            std::wstring    m_SpecularHiglight;
            std::wstring    m_Alpha;
            std::wstring    m_Bump;
            std::wstring    m_Displacement;
            std::wstring    m_Stencil;
            std::wstring    m_CubeMapSphere;
            std::wstring    m_CubeMapTop;
            std::wstring    m_CubeMapBottom;
            std::wstring    m_CubeMapFront;
            std::wstring    m_CubeMapBack;
            std::wstring    m_CubeMapLeft;
            std::wstring    m_CubeMapRight;
            ITextureOptions m_Options;

            ITextureMap();
            virtual ~ITextureMap();
        };

        typedef std::vector<ITextureMap*> ITextureMaps;

        /**
        * Object material
        */
        struct IMaterial
        {
            QR_Color            m_Ambient;
            QR_Color            m_Diffuse;
            QR_Color            m_Specular;
            float               m_SpecularRange;
            float               m_Transparent;
            IEIlluminationModel m_Model;
            ITextureMaps        m_TextureMaps;

            IMaterial();
            virtual ~IMaterial();
        };

        typedef std::map<std::string, IMaterial*> IMaterials;
        typedef std::vector<std::string>          IParams;

        /**
        * External call parameters
        */
        struct ICall
        {
            std::string m_Name;
            IParams     m_Params;

            ICall();
            virtual ~ICall();
        };

        /**
        * Position coordinates
        */
        struct ICoord
        {
            M_Precision m_X;
            M_Precision m_Y;
            M_Precision m_Z;
            M_Precision m_W;

            ICoord();
            virtual ~ICoord();
        };

        /**
        * Texture coordinates
        */
        struct ITexCoord
        {
            M_Precision m_U;
            M_Precision m_V;
            M_Precision m_W;

            ITexCoord();
            virtual ~ITexCoord();
        };

        /**
        * Polynomial degree
        */
        struct IPolynomialDegree
        {
            M_Precision m_U;
            M_Precision m_V;

            IPolynomialDegree();
            virtual ~IPolynomialDegree();
        };

        /**
        * Step size four curves and surfaces
        */
        struct IStepSize
        {
            M_Precision m_U;
            M_Precision m_V;

            IStepSize();
            virtual ~IStepSize();
        };

        typedef std::vector<ICoord*>     ICoords;
        typedef std::vector<ITexCoord*>  ITexCoords;
        typedef std::vector<QR_SizeT>    IIndexes;
        typedef std::vector<M_Precision> IMatrix;

        /**
        * Object point list
        *@note Specifies a point element and its vertex. Multiple points can be specified with this
        *      statement. Although points cannot be shaded or rendered, they are used by other
        *      Advanced Visualizer programs
        *@note Points are organized as follow: p v1 v2 v3 ...
        */
        struct IPoint
        {
            IIndexes m_Indexes;

            IPoint();
            virtual ~IPoint();
        };

        typedef std::vector<IPoint*> IPoints;

        /**
        * Object line list
        *@note Specifies a line and its vertex reference numbers. Texture vertex reference numbers
        *      can optionally be included. Although lines cannot be shaded or rendered, they are
        *      used by other Advanced Visualizer programs
        *@note Lines are organized as follow:
        *      - l v1 v2 v3 ...                         => line indexes
        *      - l v1/vt1 v2/vt2 v3/vt3 ...             => line/texture coord indexes
        */
        struct ILine
        {
            IIndexes m_PointIndexes;
            IIndexes m_TextureIndexes;

            ILine();
            virtual ~ILine();
        };

        typedef std::vector<ILine*> ILines;

        /**
        * Object face list
        *@note Faces are organized as follow:
        *      - each face contains 3 or more vertexes, if more than 3 vertexes, each new polygon is
        *        organized as triangle fan
        *      - f v1 v2 v3 ...                         => vertex indexes
        *      - f v1/vt1 v2/vt2 v3/vt3 ...             => vertex/texture coord indexes
        *      - f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ... => vertex/texture coord/normal indexes
        *      - f v1//vn1 v2//vn2 v3//vn3 ...          => vertex/normal indexes
        */
        struct IFace
        {
            IIndexes m_VectorIndexes;
            IIndexes m_NormalIndexes;
            IIndexes m_TextureIndexes;
            bool     m_SmoothShading;

            IFace();
            virtual ~IFace();
        };

        typedef std::vector<IFace*> IFaces;

        /**
        * Object sub-group
        */
        struct IGroup
        {
            std::string       m_Name;
            std::string       m_Material;
            ICoords           m_Vectors;
            ICoords           m_Normals;
            ICoords           m_PSVertices; // parameter space vertices or free form geometry statement
            ITexCoords        m_TexCoords;
            IPoints           m_Points;
            ILines            m_Lines;
            IFaces            m_Faces;
            IESurfaceType     m_SurfaceType;
            IPolynomialDegree m_PolymonialDegree;
            IStepSize         m_StepSize;
            IMatrix           m_UMatrix;
            IMatrix           m_VMatrix;
            bool              m_RationalSurface;

            IGroup();
            virtual ~IGroup();
        };

        typedef std::map<std::string, IGroup*> IGroups;

        /**
        * WaveFront object
        */
        struct IObject
        {
            std::string m_Name;
            IGroups     m_Groups;

            IObject();
            virtual ~IObject();
        };

        typedef std::map<std::string, IObject*> IObjects;

        IObjects   m_Objects;
        IMaterials m_Materials;

        QR_WaveFrontParser();
        virtual ~QR_WaveFrontParser();

        /**
        * Clears all previous data
        */
        virtual void Clear();

        /**
        * Loads WaveFront model from file
        *@param fileName - file name
        *@return true on success, otherwise false
        */
        virtual bool Load(const std::string&  fileName);
        virtual bool Load(const std::wstring& fileName);

        /**
        * Loads model from buffer
        *@param buffer - buffer
        *@param length - length to read in buffer
        *@return true on success, otherwise false
        *@note Read will begin from current offset
        */
        virtual bool Load(      QR_Buffer&            buffer,
                          const QR_Buffer::ISizeType& length);

        /**
        * Parses WaveFront object
        *@param data - data to parse from
        *@param start - data start position to read from
        *@param end - data end position to read to
        *@return true on success, otherwise false
        */
        virtual bool Parse(const std::string& data, const QR_SizeT& start, const QR_SizeT& end);

        /**
        * Sets material data from buffer
        *@param name - material name (should be the same as WaveFront file content)
        *@param buffer - buffer containing material data
        *@param length - length to read in buffer
        *@return true on success, otherwise false
        *@note Read will begin from current offset
        */
        virtual bool SetMaterial(const std::string&          name,
                                       QR_Buffer&            buffer,
                                 const QR_Buffer::ISizeType& length);

        /**
        * Sets material data to read from
        *@param name - material name (should be the same as WaveFront file content)
        *@param data - material data to parse from
        *@param start - material data start position to read from
        *@param end - material data end position to read to
        *@return true on success, otherwise false
        */
        virtual bool SetMaterial(const std::string& name,
                                 const std::string& data,
                                 const QR_SizeT&    start,
                                 const QR_SizeT&    end);

    private:
        /**
        * Actions to do for a WaveFront line
        */
        enum IEAction
        {
            IE_AC_Unknown = 0,
            IE_AC_Comment,
            IE_AC_Call,
            IE_AC_UnixCommand,
            IE_AC_SetSurfaceType,
            IE_AC_SetPolymonialDegree,
            IE_AC_SetBasisMatrix,
            IE_AC_SetStepSize,
            IE_AC_SetParamValues,
            IE_AC_StartInnerTrimmingLoop,
            IE_AC_StartOuterTrimmingLoop,
            IE_AC_EndLoop,
            IE_AC_SpecialCurve,
            IE_AC_SpecialPoint,
            IE_AC_Connect,
            IE_AC_PushVertex,
            IE_AC_PushTexCoord,
            IE_AC_PushNormal,
            IE_AC_PushPSVertice,
            IE_AC_CreateObject,
            IE_AC_CreateGroup,
            IE_AC_CreateMergingGroup,
            IE_AC_BuildPoint,
            IE_AC_BuildLine,
            IE_AC_BuildFace,
            IE_AC_BuildCurve,
            IE_AC_Build2DCurve,
            IE_AC_BuildSurface,
            IE_AC_SmoothShading,
            IE_AC_LinkMaterial,
            IE_AC_UseMaterial,
            IE_AC_BevelInterpolation,
            IE_AC_ColorInterpolation,
            IE_AC_DissolveInterpolation,
            IE_AC_LevelOfDetails,
            IE_AC_ShadowCasting,
            IE_AC_RayTracing,
            IE_AC_CurveApproxTech,
            IE_AC_SurfaceApproxTech,
        };

        /**
        * Actions to do for a WaveFront material line
        */
        enum IEMtlAction
        {
            IE_MA_Unknown = 0,
            IE_MA_AmbientColor,
            IE_MA_DiffuseColor,
            IE_MA_SpecularColor,
            IE_MA_Transparency,
            IE_MA_IlluminationModel,
            IE_MA_TexMap_Ambient,
            IE_MA_TexMap_Diffuse,
            IE_MA_TexMap_SpecularColor,
            IE_MA_TexMap_SpecularHighlightComp,
            IE_MA_TexMap_Alpha,
            IE_MA_TexMap_Bump,
            IE_MA_TexMap_Displacement,
            IE_MA_TexMap_StencilDecal,
            IE_MA_TexMap_Reflexion,
        };

        /**
        * Material texture options
        */
        enum IEMtlTexOption
        {
            IE_MO_None = 0,
            IE_MO_BlendU,
            IE_MO_BlendV,
            IE_MO_BoostMipMapSharpness,
            IE_MO_MapModifier,
            IE_MO_OriginOffset,
            IE_MO_Scale,
            IE_MO_Turbulence,
            IE_MO_TexResolution,
            IE_MO_Clamp,
            IE_MO_BumpMultiplier,
            IE_MO_FileChannel,
            IE_MO_ReflexionMapType,
        };

        typedef std::vector<std::string> IActionValues;

        /**
        * Data needed to execute an action
        */
        struct IActionData
        {
            IEAction      m_Action;
            IActionValues m_Values;

            IActionData();
            virtual ~IActionData();
        };

        typedef std::map<std::string, IEAction>       IActionDictionary;
        typedef std::map<std::string, IEMtlAction>    IMtlActionDictionary;
        typedef std::map<std::string, IEMtlTexOption> IMtlTexOptionDictionary;

        /**
        * In memory material file (in case loaded from resources)
        */
        struct IResourceMaterial
        {
            std::string m_Data;
            QR_SizeT    m_Start;
            QR_SizeT    m_End;

            IResourceMaterial();
            virtual ~IResourceMaterial();
        };

        typedef std::map<std::string, IResourceMaterial*> IResourceMaterialDict;

        IActionDictionary        m_ActionDict;
        IMtlActionDictionary     m_MtlActionDict;
        IMtlTexOptionDictionary  m_MtlTexOptionDict;
        IResourceMaterialDict    m_ResourceMaterialDict;
        IObject*                 m_pCurrentObject;
        IGroup*                  m_pCurrentGroup;
        bool                     m_CurrentSmoothShadingState;

        /**
        * Populates action dictionary
        */
        void PopulateActionDict();

        /**
        * Populates material action dictionary
        */
        void PopulateMtlActionDict();

        /**
        * Populates material texture option dictionary
        */
        void PopulateMtlTexOptionDict();

        /**
        * Loads material from data
        *@param name - material name
        *@param data - material data to load
        *@param start - material start offset in data
        *@param end - material end offset in data
        *@returns true on success, otherwise false
        */
        bool LoadMaterial(const std::string& name,
                          const std::string& data,
                          const QR_SizeT&    start,
                          const QR_SizeT&    end);

        /**
        * Reads and parses a wavefront line
        *@param pMaterial - material to populate, in case line is read in a material file
        *@param line - line to parse
        */
        void ReadLine(const std::string& line);
        void ReadLine(IMaterial* pMaterial, const std::string& line);

        /**
        * Gets action to do from read keyword
        *@param keyword - read keyword
        *@return action to get, IE_AC_Unknown if not found or on error
        */
        IEAction GetAction(const std::string& keyword) const;

        /**
        * Executes action
        *@param action - action to execute
        *@return true on success, otherwise false
        */
        bool ExecuteAction(const IActionData& actionData);

        /**
        * Gets current active group, creates a default object and group if no one are created
        *@return active group, NULL on error
        */
        IGroup* GetActiveGroup();
};
#endif
