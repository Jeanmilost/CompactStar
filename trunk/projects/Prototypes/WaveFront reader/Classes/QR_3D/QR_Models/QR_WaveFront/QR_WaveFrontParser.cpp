/******************************************************************************
 * ==> QR_WaveFrontParser ----------------------------------------------------*
 ******************************************************************************
 * Description : Reads and exposes WaveFront (.obj) file content              *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/

#include "QR_WaveFrontParser.h"

// qr engine
#include "QR_STDTools.h"
#include "QR_StringTools.h"
#include "QR_StdFileBuffer.h"
#include "QR_Exception.h"

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------
#define M_DefaultObjectName "default_qrengine_object"
#define M_DefaultGroupName  "default_qrengine_group"
//---------------------------------------------------------------------------
// QR_WaveFrontParser::ITextureOptions - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::ITextureOptions::ITextureOptions() :
    m_BoostMipMapSharpness(0.0f),
    m_TextureMapBrightness(0.0f),
    m_TextureMapContrast(1.0f),
    m_BumpMultiplier(0.0f),
    m_Scale(QR_Vector3DP(1.0f, 1.0f, 1.0f)),
    m_Resolution(0.0f),
    m_BlendU(true),
    m_BlendV(true),
    m_Clamp(false),
    m_Channel("m")
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::ITextureOptions::~ITextureOptions()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::ITextureMap - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::ITextureMap::ITextureMap()
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::ITextureMap::~ITextureMap()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::IMaterial - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::IMaterial::IMaterial() :
    m_SpecularRange(0.0f),
    m_Transparent(1.0f),
    m_Model(IE_IM_Color)
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::IMaterial::~IMaterial()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::ICall - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::ICall::ICall()
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::ICall::~ICall()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::ICoord - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::ICoord::ICoord() :
    m_X(0.0f),
    m_Y(0.0f),
    m_Z(0.0f),
    m_W(0.0f)
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::ICoord::~ICoord()
{}
//---------------------------------------------------------------------------
// ITexCoord - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::ITexCoord::ITexCoord() :
    m_U(0.0f),
    m_V(0.0f),
    m_W(0.0f)
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::ITexCoord::~ITexCoord()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::IPolynomialDegree - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::IPolynomialDegree::IPolynomialDegree() :
    m_U(0.0f),
    m_V(0.0f)
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::IPolynomialDegree::~IPolynomialDegree()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::IStepSize - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::IStepSize::IStepSize() :
    m_U(0.0f),
    m_V(0.0f)
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::IStepSize::~IStepSize()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::IPoint - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::IPoint::IPoint()
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::IPoint::~IPoint()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::ILine - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::ILine::ILine()
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::ILine::~ILine()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::IFace - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::IFace::IFace() :
    m_SmoothShading(false)
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::IFace::~IFace()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::IGroup - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::IGroup::IGroup() :
    m_SurfaceType(IE_ST_Unknown),
    m_RationalSurface(false)
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::IGroup::~IGroup()
{
    // delete all vectors
    QR_STDTools::DelAndClear(m_Vectors);

    // delete all normals
    QR_STDTools::DelAndClear(m_Normals);

    // delete all texture coordinates
    QR_STDTools::DelAndClear(m_TexCoords);

    // delete all parametric space vertices
    QR_STDTools::DelAndClear(m_PSVertices);

    // delete all points
    QR_STDTools::DelAndClear(m_Points);

    // delete all lines
    QR_STDTools::DelAndClear(m_Lines);

    // delete all faces
    QR_STDTools::DelAndClear(m_Faces);
}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::IObject - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::IObject::IObject()
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::IObject::~IObject()
{
    // delete all groups
    QR_STDTools::DelAndClear(m_Groups);
}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::IActionData - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::IActionData::IActionData() : m_Action(IE_AC_Unknown)
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::IActionData::~IActionData()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser::IResourceMaterial - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::IResourceMaterial::IResourceMaterial() : m_Start(0), m_End(0)
{}
//---------------------------------------------------------------------------
QR_WaveFrontParser::IResourceMaterial::~IResourceMaterial()
{}
//---------------------------------------------------------------------------
// QR_WaveFrontParser - c++ cross-platform
//---------------------------------------------------------------------------
QR_WaveFrontParser::QR_WaveFrontParser() :
    QR_ModelParser(),
    m_pCurrentGroup(NULL),
    m_pCurrentObject(NULL),
    m_CurrentSmoothShadingState(false)
{
    // populate dictionaries
    PopulateActionDict();
    PopulateMtlActionDict();
    PopulateMtlTexOptionDict();
}
//---------------------------------------------------------------------------
QR_WaveFrontParser::~QR_WaveFrontParser()
{
    Clear();
}
//---------------------------------------------------------------------------
void QR_WaveFrontParser::Clear()
{
    m_pCurrentObject            = NULL;
    m_pCurrentGroup             = NULL;
    m_CurrentSmoothShadingState = false;

    // delete all objects
    QR_STDTools::DelAndClear(m_Objects);

    // delete all materials
    QR_STDTools::DelAndClear(m_Materials);

    // delete all material from resources
    QR_STDTools::DelAndClear(m_ResourceMaterialDict);
}
//---------------------------------------------------------------------------
bool QR_WaveFrontParser::Load(const std::string& fileName)
{
    try
    {
        // create a file buffer and open it for read
        QR_StdFileBuffer buffer;
        buffer.Open(fileName, QR_FileBuffer::IE_M_Read);
        buffer.Seek(0, 0);

        // read WaveFront content
        return Load(buffer, buffer.GetSize());
    }
    catch (...)
    {}

    return false;
}
//---------------------------------------------------------------------------
bool QR_WaveFrontParser::Load(const std::wstring& fileName)
{
    try
    {
        // create a file buffer and open it for read
        QR_StdFileBuffer buffer;
        buffer.Open(fileName, QR_FileBuffer::IE_M_Read);
        buffer.Seek(0, 0);

        // read WaveFront content
        return Load(buffer, buffer.GetSize());
    }
    catch (...)
    {}

    return false;
}
//---------------------------------------------------------------------------
bool QR_WaveFrontParser::Load(      QR_Buffer&            buffer,
                              const QR_Buffer::ISizeType& length)
{
    try
    {
        // get data as string (as normally WaveFront data are)
        std::string data = buffer.ToStr();

        // read WaveFront content
        return Parse(data, 0, data.length());
    }
    catch (...)
    {}

    return false;
}
//---------------------------------------------------------------------------
bool QR_WaveFrontParser::Parse(const std::string& data, const QR_SizeT& start, const QR_SizeT& end)
{
    try
    {
        // clear all previous data
        Clear();

        // get data length
        const QR_SizeT dataLength = data.length();

        // no data to read?
        if (!dataLength)
            return true;

        // get line start index and ensure that end index cannot exceed data length
        QR_SizeT lineStart = start;
        QR_SizeT dataEnd   = std::min(end, dataLength);

        //  iterate through chars to parse
        for (QR_SizeT i = start; i < dataEnd; ++i)
            // search for line end
            switch (data[i])
            {
                case '\r':
                case '\n':
                    // read next line
                    ReadLine(data.substr(lineStart, i - lineStart));

                    // next char is a line feed or carriage return?
                    if ((i + 1) < dataLength && (data[i + 1] == '\r' || data[i + 1] == '\n'))
                        // skip it
                        ++i;

                    lineStart = i + 1;
                    break;

                default:
                    break;
            }

        // read last line, if needed
        if (lineStart < dataEnd)
            ReadLine(data.substr(lineStart, dataEnd - lineStart));

        return true;
    }
    catch(...)
    {}

    return false;
}
//---------------------------------------------------------------------------
bool QR_WaveFrontParser::SetMaterial(const std::string&          name,
                                           QR_Buffer&            buffer,
                                     const QR_Buffer::ISizeType& length)
{
    try
    {
        // get data as string (as normally WaveFront material data are)
        std::string data = buffer.ToStr();

        // set WaveFront material
        return SetMaterial(name, data, 0, data.length());
    }
    catch (...)
    {}

    return false;
}
//---------------------------------------------------------------------------
bool QR_WaveFrontParser::SetMaterial(const std::string& name,
                                     const std::string& data,
                                     const QR_SizeT&    start,
                                     const QR_SizeT&    end)
{
    try
    {
        // create new resource material item
        std::auto_ptr<IResourceMaterial> pResMaterial(new IResourceMaterial());
        pResMaterial->m_Data  = data;
        pResMaterial->m_Start = start;
        pResMaterial->m_End   = end;

        // search if anoter resource material was already registered
        IResourceMaterialDict::iterator it = m_ResourceMaterialDict.find(name);

        // found it?
        if (it != m_ResourceMaterialDict.end())
        {
            // delete previous resouce material
            delete it->second;
            m_ResourceMaterialDict.erase(it);
        }

        // set new resource material
        m_ResourceMaterialDict[name] = pResMaterial.get();
        pResMaterial.release();

        return true;
    }
    catch (...)
    {}

    return false;
}
//---------------------------------------------------------------------------
void QR_WaveFrontParser::PopulateActionDict()
{
    // list of all possible actions that can be found in a WaveFront file
    m_ActionDict["#"]          = IE_AC_Comment;
    m_ActionDict["call"]       = IE_AC_Call;
    m_ActionDict["csh"]        = IE_AC_UnixCommand;
    m_ActionDict["cstype"]     = IE_AC_SetSurfaceType;
    m_ActionDict["deg"]        = IE_AC_SetPolymonialDegree;
    m_ActionDict["bmat"]       = IE_AC_SetBasisMatrix;
    m_ActionDict["step"]       = IE_AC_SetStepSize;
    m_ActionDict["parm"]       = IE_AC_SetParamValues;
    m_ActionDict["trim"]       = IE_AC_StartInnerTrimmingLoop;
    m_ActionDict["Hole"]       = IE_AC_StartOuterTrimmingLoop;
    m_ActionDict["end"]        = IE_AC_EndLoop;
    m_ActionDict["scrv"]       = IE_AC_SpecialCurve;
    m_ActionDict["sp"]         = IE_AC_SpecialPoint;
    m_ActionDict["con"]        = IE_AC_Connect;
    m_ActionDict["v"]          = IE_AC_PushVertex;
    m_ActionDict["vt"]         = IE_AC_PushTexCoord;
    m_ActionDict["vn"]         = IE_AC_PushNormal;
    m_ActionDict["vp"]         = IE_AC_PushPSVertice;
    m_ActionDict["o"]          = IE_AC_CreateObject;
    m_ActionDict["g"]          = IE_AC_CreateGroup;
    m_ActionDict["mg"]         = IE_AC_CreateMergingGroup;
    m_ActionDict["p"]          = IE_AC_BuildPoint;
    m_ActionDict["l"]          = IE_AC_BuildLine;
    m_ActionDict["f"]          = IE_AC_BuildFace;
    m_ActionDict["curv"]       = IE_AC_BuildCurve;
    m_ActionDict["curv2"]      = IE_AC_Build2DCurve;
    m_ActionDict["surf"]       = IE_AC_BuildSurface;
    m_ActionDict["s"]          = IE_AC_SmoothShading;
    m_ActionDict["mtllib"]     = IE_AC_LinkMaterial;
    m_ActionDict["usemtl"]     = IE_AC_UseMaterial;
    m_ActionDict["bevel"]      = IE_AC_BevelInterpolation;
    m_ActionDict["c_interp"]   = IE_AC_ColorInterpolation;
    m_ActionDict["d_interp"]   = IE_AC_DissolveInterpolation;
    m_ActionDict["lod"]        = IE_AC_LevelOfDetails;
    m_ActionDict["shadow_obj"] = IE_AC_ShadowCasting;
    m_ActionDict["trace_obj"]  = IE_AC_RayTracing;
    m_ActionDict["ctech"]      = IE_AC_CurveApproxTech;
    m_ActionDict["stech"]      = IE_AC_SurfaceApproxTech;
}
//---------------------------------------------------------------------------
void QR_WaveFrontParser::PopulateMtlActionDict()
{
    // list of all possible actions that can be found in a WaveFront material file
    m_MtlActionDict["Ka"]       = IE_MA_AmbientColor;
    m_MtlActionDict["Kd"]       = IE_MA_DiffuseColor;
    m_MtlActionDict["Ks"]       = IE_MA_SpecularColor;
    m_MtlActionDict["d"]        = IE_MA_Transparency;
    m_MtlActionDict["Tr"]       = IE_MA_Transparency;
    m_MtlActionDict["illum"]    = IE_MA_IlluminationModel;
    m_MtlActionDict["map_Ka"]   = IE_MA_TexMap_Ambient;
    m_MtlActionDict["map_Kd"]   = IE_MA_TexMap_Diffuse;
    m_MtlActionDict["map_Ks"]   = IE_MA_TexMap_SpecularColor;
    m_MtlActionDict["map_Ns"]   = IE_MA_TexMap_SpecularHighlightComp;
    m_MtlActionDict["map_d"]    = IE_MA_TexMap_Alpha;
    m_MtlActionDict["map_Tr"]   = IE_MA_TexMap_Alpha;
    m_MtlActionDict["map_bump"] = IE_MA_TexMap_Bump;
    m_MtlActionDict["bump"]     = IE_MA_TexMap_Bump;
    m_MtlActionDict["disp"]     = IE_MA_TexMap_Displacement;
    m_MtlActionDict["decal"]    = IE_MA_TexMap_StencilDecal;
    m_MtlActionDict["refl"]     = IE_MA_TexMap_Reflexion;
}
//---------------------------------------------------------------------------
void QR_WaveFrontParser::PopulateMtlTexOptionDict()
{
    // list of all possible options that can be found in a WaveFront material file
    m_MtlTexOptionDict["blendu"]  = IE_MO_BlendU;
    m_MtlTexOptionDict["blendv"]  = IE_MO_BlendV;
    m_MtlTexOptionDict["boost "]  = IE_MO_BoostMipMapSharpness;
    m_MtlTexOptionDict["mm"]      = IE_MO_MapModifier;
    m_MtlTexOptionDict["o"]       = IE_MO_OriginOffset;
    m_MtlTexOptionDict["s"]       = IE_MO_Scale;
    m_MtlTexOptionDict["t"]       = IE_MO_Turbulence;
    m_MtlTexOptionDict["texres"]  = IE_MO_TexResolution;
    m_MtlTexOptionDict["clamp"]   = IE_MO_Clamp;
    m_MtlTexOptionDict["bm"]      = IE_MO_BumpMultiplier;
    m_MtlTexOptionDict["imfchan"] = IE_MO_FileChannel;
    m_MtlTexOptionDict["type"]    = IE_MO_ReflexionMapType;
}
//---------------------------------------------------------------------------
bool QR_WaveFrontParser::LoadMaterial(const std::string& name,
                                      const std::string& data,
                                      const QR_SizeT&    start,
                                      const QR_SizeT&    end)
{
    try
    {
        // create new material
        std::auto_ptr<IMaterial> pMaterial(new IMaterial());

        // get data length
        const QR_SizeT dataLength = data.length();

        // no data to read?
        if (!dataLength)
            return true;

        // get line start index and ensure that end index cannot exceed data length
        QR_SizeT lineStart = start;
        QR_SizeT dataEnd   = std::min(end, dataLength);

        //  iterate through chars to parse
        for (QR_SizeT i = start; i < dataEnd; ++i)
            // search for line end
            switch (data[i])
            {
                case '\r':
                case '\n':
                    // read next material line
                    ReadLine(pMaterial.get(), data.substr(lineStart, i - lineStart));

                    // next char is a line feed or carriage return?
                    if ((i + 1) < dataLength && (data[i + 1] == '\r' || data[i + 1] == '\n'))
                        // skip it
                        ++i;

                    lineStart = i + 1;
                    break;

                default:
                    break;
            }

        // read last material line, if needed
        if (lineStart < dataEnd)
            ReadLine(pMaterial.get(), data.substr(lineStart, dataEnd - lineStart));

        // check if another material with same name already exists
        IMaterials::iterator it = m_Materials.find(name);

        // found it?
        if (it != m_Materials.end())
        {
            // delete it
            delete it->second;
            m_Materials.erase(it);
        }

        // add newly created material to list
        m_Materials[name] = pMaterial.get();
        pMaterial.release();

        return true;
    }
    catch(...)
    {}

    return false;
}
//---------------------------------------------------------------------------
void QR_WaveFrontParser::ReadLine(const std::string& line)
{
    try
    {
        // get line length and keywork start
        const QR_SizeT lineLength   = line.length();
              QR_SizeT keywordStart = 0;

        IActionData actionData;

        // iterate through line chars
        for (QR_SizeT i = 0; i < lineLength; ++i)
            // dispatch char
            switch (line[i])
            {
                case ' ':
                {
                    // get next keyword
                    std::string keyword = line.substr(keywordStart, i - keywordStart);

                    // get action matching with keyword (ensure that keyword is in lower case)
                    IEAction action = GetAction(QR_StringTools::ToLowerCase(keyword));

                    // found action to do?
                    if (action != IE_AC_Unknown)
                        // store it
                        actionData.m_Action = action;
                    else
                    if (action == IE_AC_Comment      ||
                        action == IE_AC_CreateObject ||
                        action == IE_AC_CreateGroup  ||
                        action == IE_AC_CreateMergingGroup)
                    {
                        // these action use the entire text so copy entire line directly
                        actionData.m_Values.push_back(line.substr(keywordStart,
                                                                  lineLength - keywordStart));
                        i = keywordStart = lineLength;
                        break;
                    }
                    else
                        // add keyword to value list
                        actionData.m_Values.push_back(keyword);

                    // padding blank spaces follow the current separator?
                    while (i + 1 < lineLength && line[i + 1] == ' ')
                        // skip them
                        ++i;

                    keywordStart = i + 1;
                    break;
                }
            }

        // line end reached
        if (keywordStart < lineLength)
        {
            // get last keyword, ensure that keyword is in lower case
            std::string keyword = QR_StringTools::ToLowerCase(line.substr(keywordStart,
                                                                          lineLength - keywordStart));

            // get action matching with keyword (ensure that keyword is in lower case)
            IEAction action = GetAction(QR_StringTools::ToLowerCase(keyword));

            // found action to do?
            if (action != IE_AC_Unknown)
                // store it
                actionData.m_Action = action;
            else
                // add keyword to value list
                actionData.m_Values.push_back(keyword);
        }

        // execute action to do
        ExecuteAction(actionData);
    }
    catch(...)
    {}
}
//---------------------------------------------------------------------------
void QR_WaveFrontParser::ReadLine(IMaterial* pMaterial, const std::string& line)
{
    try
    {
        // no material to populate?
        if (!pMaterial)
            return;

        /*REM
        // get line length and keywork start
        const QR_SizeT lineLength   = line.length();
              QR_SizeT keywordStart = 0;

        IActionData actionData;

        // iterate through line chars
        for (QR_SizeT i = 0; i < lineLength; ++i)
            // dispatch char
            switch (line[i])
            {
                case ' ':
                {
                    // get next keyword
                    std::string keyword = line.substr(keywordStart, i - keywordStart);

                    // get action matching with keyword (ensure that keyword is in lower case)
                    IEAction action = GetAction(QR_StringTools::ToLowerCase(keyword));

                    // found action to do?
                    if (action != IE_AC_Unknown)
                        // store it
                        actionData.m_Action = action;
                    else
                    if (action == IE_AC_Comment      ||
                        action == IE_AC_CreateObject ||
                        action == IE_AC_CreateGroup  ||
                        action == IE_AC_CreateMergingGroup)
                    {
                        // these action use the entire text so copy entire line directly
                        actionData.m_Values.push_back(line.substr(keywordStart,
                                                                  lineLength - keywordStart));
                        i = keywordStart = lineLength;
                        break;
                    }
                    else
                        // add keyword to value list
                        actionData.m_Values.push_back(keyword);

                    // padding blank spaces follow the current separator?
                    while (i + 1 < lineLength && line[i + 1] == ' ')
                        // skip them
                        ++i;

                    keywordStart = i + 1;
                    break;
                }
            }

        // line end reached
        if (keywordStart < lineLength)
        {
            // get last keyword, ensure that keyword is in lower case
            std::string keyword = QR_StringTools::ToLowerCase(line.substr(keywordStart,
                                                                          lineLength - keywordStart));

            // get action matching with keyword (ensure that keyword is in lower case)
            IEAction action = GetAction(QR_StringTools::ToLowerCase(keyword));

            // found action to do?
            if (action != IE_AC_Unknown)
                // store it
                actionData.m_Action = action;
            else
                // add keyword to value list
                actionData.m_Values.push_back(keyword);
        }

        // execute action to do
        ExecuteAction(actionData);
        */
    }
    catch(...)
    {}
}
//---------------------------------------------------------------------------
QR_WaveFrontParser::IEAction QR_WaveFrontParser::GetAction(const std::string& keyword) const
{
    try
    {
        // search for action to do
        IActionDictionary::const_iterator it = m_ActionDict.find(keyword);

        // found it?
        if (it != m_ActionDict.end())
            return it->second;
    }
    catch (...)
    {}

    return IE_AC_Unknown;
}
//---------------------------------------------------------------------------
bool QR_WaveFrontParser::ExecuteAction(const IActionData& action)
{
    try
    {
        // dispatch action to do
        switch (action.m_Action)
        {
            case IE_AC_Unknown:
                return false;

            case IE_AC_Comment:
                return true;

            case IE_AC_Call:
            {
                // get action value count
                const QR_SizeT valueCount = action.m_Values.size();

                // action value count is incorrect?
                if (!valueCount)
                    return false;

                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // create and populate new call object
                std::auto_ptr<ICall> pCall(new ICall());
                pCall->m_Name = action.m_Values[0];

                // iterate through call arguments
                for (QR_SizeT i = 1; i < valueCount; ++i)
                    // add call argument to list
                    pCall->m_Params.push_back(action.m_Values[i]);

                return true;
            }

            case IE_AC_UnixCommand:
                M_THROW_EXCEPTION("NOT IMPLEMENTED");

            case IE_AC_SetSurfaceType:
            {
                // get action value count
                const QR_SizeT valueCount = action.m_Values.size();

                // action value count is incorrect?
                if (valueCount < 1 || valueCount > 2)
                    return false;

                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // iterate through action values
                for (QR_SizeT i = 0; i < valueCount; ++i)
                {
                    // get value as lower case
                    const std::string value = QR_StringTools::ToLowerCase(action.m_Values[0]);

                    // convert action value to surface type or optional argument
                    if (value == "bmatrix")
                        pGroup->m_SurfaceType = IE_ST_BasisMatrix;
                    else
                    if (value == "bspline")
                        pGroup->m_SurfaceType = IE_ST_BSpline;
                    else
                    if (value == "bezier")
                        pGroup->m_SurfaceType = IE_ST_Bezier;
                    else
                    if (value == "cardinal")
                        pGroup->m_SurfaceType = IE_ST_Cardinal;
                    else
                    if (value == "taylor")
                        pGroup->m_SurfaceType = IE_ST_Taylor;
                    else
                    if (value == "rat")
                        // optional argument that means that surface is rational
                        pGroup->m_RationalSurface = true;
                    else
                        return false;
                }

                return true;
            }

            case IE_AC_SetPolymonialDegree:
            {
                // get action value count
                const QR_SizeT valueCount = action.m_Values.size();

                // action value count is incorrect?
                if (valueCount < 1 || valueCount > 2)
                    return false;

                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // iterate through action values
                for (QR_SizeT i = 0; i < valueCount; ++i)
                {
                    // get action value as numeric value
                    M_Precision value = std::atof(action.m_Values[i].c_str());

                    // Search for value to set
                    switch (i)
                    {
                        case 0:
                            pGroup->m_PolymonialDegree.m_U = value;
                            break;

                        case 1:
                            pGroup->m_PolymonialDegree.m_V = value;
                            break;

                        default:
                            return false;
                    }
                }

                return true;
            }

            case IE_AC_SetBasisMatrix:
            {
                // get action value count
                const QR_SizeT valueCount = action.m_Values.size();

                // action value count is incorrect?
                if (!valueCount)
                    return false;

                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // get first action value
                std::string value = action.m_Values[0];
                bool        uMatrix;

                // is u or v matrix?
                if (value == "u")
                    uMatrix = true;
                else
                if (value == "v")
                    uMatrix = false;
                else
                    return false;

                // iterate through action values
                for (QR_SizeT i = 1; i < valueCount; ++i)
                {
                    // get action value as number
                    M_Precision value = std::atof(action.m_Values[i].c_str());

                    // do add value to u or v matrix?
                    if (uMatrix)
                        pGroup->m_UMatrix.push_back(value);
                    else
                        pGroup->m_VMatrix.push_back(value);
                }

                return true;
            }

            case IE_AC_SetStepSize:
            {
                // get action value count
                const QR_SizeT valueCount = action.m_Values.size();

                // action value count is incorrect?
                if (valueCount < 1 || valueCount > 2)
                    return false;

                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // iterate through action values
                for (QR_SizeT i = 0; i < valueCount; ++i)
                {
                    // get action value as numeric value
                    M_Precision value = std::atof(action.m_Values[i].c_str());

                    // Search for value to set
                    switch (i)
                    {
                        case 0:
                            pGroup->m_StepSize.m_U = value;
                            break;

                        case 1:
                            pGroup->m_StepSize.m_V = value;
                            break;

                        default:
                            return false;
                    }
                }

                return true;
            }

            case IE_AC_SetParamValues:
            case IE_AC_StartInnerTrimmingLoop:
            case IE_AC_StartOuterTrimmingLoop:
            case IE_AC_EndLoop:
            case IE_AC_SpecialCurve:
            case IE_AC_SpecialPoint:
            case IE_AC_Connect:
                M_THROW_EXCEPTION("NOT IMPLEMENTED");

            case IE_AC_PushVertex:
            {
                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // get action values count
                const QR_SizeT count = action.m_Values.size();

                // create and add vertex
                std::auto_ptr<ICoord> pVector(new ICoord());
                pVector->m_X = count     ? std::atof(action.m_Values[0].c_str()) : 0.0f;
                pVector->m_Y = count > 1 ? std::atof(action.m_Values[1].c_str()) : 0.0f;
                pVector->m_Z = count > 2 ? std::atof(action.m_Values[2].c_str()) : 0.0f;
                pVector->m_W = count > 3 ? std::atof(action.m_Values[2].c_str()) : 1.0f;
                pGroup->m_Vectors.push_back(pVector.get());
                pVector.release();

                return true;
            }

            case IE_AC_PushTexCoord:
            {
                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // get action values count
                const QR_SizeT count = action.m_Values.size();

                // create and add texture coordinate
                std::auto_ptr<ITexCoord> pTexCoord(new ITexCoord());
                pTexCoord->m_U = count     ? std::atof(action.m_Values[0].c_str()) : 0.0f;
                pTexCoord->m_V = count > 1 ? std::atof(action.m_Values[1].c_str()) : 0.0f;
                pTexCoord->m_W = count > 2 ? std::atof(action.m_Values[2].c_str()) : 0.0f;
                pGroup->m_TexCoords.push_back(pTexCoord.get());
                pTexCoord.release();

                return true;
            }

            case IE_AC_PushNormal:
            {
                // action value count is incorrect?
                if (action.m_Values.size() != 3)
                    return false;

                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // create and add normal
                std::auto_ptr<ICoord> pNormal(new ICoord());
                pNormal->m_X = std::atof(action.m_Values[0].c_str());
                pNormal->m_Y = std::atof(action.m_Values[1].c_str());
                pNormal->m_Z = std::atof(action.m_Values[2].c_str());
                pGroup->m_Normals.push_back(pNormal.get());
                pNormal.release();

                return true;
            }

            case IE_AC_PushPSVertice:
            {
                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // get action values count
                const QR_SizeT count = action.m_Values.size();

                // create and add vertice
                std::auto_ptr<ICoord> pVertice(new ICoord());
                pVertice->m_X = count     ? std::atof(action.m_Values[0].c_str()) : 0.0f;
                pVertice->m_Y = count > 1 ? std::atof(action.m_Values[1].c_str()) : 0.0f;
                pVertice->m_Z = count > 2 ? std::atof(action.m_Values[2].c_str()) : 0.0f;
                pVertice->m_W = count > 3 ? std::atof(action.m_Values[2].c_str()) : 1.0f;
                pGroup->m_PSVertices.push_back(pVertice.get());
                pVertice.release();

                return true;
            }

            case IE_AC_CreateObject:
            {
                // action value count is incorrect?
                if (action.m_Values.size() != 1)
                    return false;

                // get object name
                const std::string objName = action.m_Values[0];

                // search for twice objects
                IObjects::const_iterator it = m_Objects.find(objName);

                // found twice?
                if (it != m_Objects.end())
                    return false;

                // create new object and set is as current
                std::auto_ptr<IObject> pObject(new IObject());
                pObject->m_Name    = objName;
                m_Objects[objName] = pObject.get();
                m_pCurrentObject   = pObject.release();

                return true;
            }

            case IE_AC_CreateGroup:
            {
                // action value count is incorrect?
                if (action.m_Values.size() != 1)
                    return false;

                // was current object already defined?
                if (!m_pCurrentObject)
                {
                    // create default object
                    std::auto_ptr<IObject> pObject(new IObject());
                    pObject->m_Name                = M_DefaultObjectName;
                    m_Objects[M_DefaultObjectName] = pObject.get();
                    m_pCurrentObject               = pObject.release();
                }

                // get group name
                const std::string groupName = action.m_Values[0];

                // search for twice groups
                IGroups::const_iterator it = m_pCurrentObject->m_Groups.find(groupName);

                // found twice?
                if (it != m_pCurrentObject->m_Groups.end())
                    return false;

                // create new group and set is as current
                std::auto_ptr<IGroup> pGroup(new IGroup());
                pGroup->m_Name                        = groupName;
                m_pCurrentObject->m_Groups[groupName] = pGroup.get();
                m_pCurrentGroup                       = pGroup.release();

                return true;
            }

            case IE_AC_CreateMergingGroup:
                M_THROW_EXCEPTION("NOT IMPLEMENTED");

            case IE_AC_BuildPoint:
            {
                // create new point
                std::auto_ptr<IPoint> pPoint(new IPoint());

                // get value count
                const QR_SizeT count = action.m_Values.size();

                // iterate through face values
                for (QR_SizeT i = 0; i < count; ++i)
                    // convert action value to numerical value and add to list
                    pPoint->m_Indexes.push_back(std::atof(action.m_Values[i].c_str()));

                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // add newly created point to group
                pGroup->m_Points.push_back(pPoint.get());
                pPoint.release();
                break;
            }

            case IE_AC_BuildLine:
            {
                // create new line
                std::auto_ptr<ILine> pLine(new ILine());

                // get value count
                const QR_SizeT count = action.m_Values.size();

                // iterate through face values
                for (QR_SizeT i = 0; i < count; ++i)
                {
                    // get current value
                    QR_SizeT      valueLength = action.m_Values[i].length();
                    QR_SizeT      start       = 0;
                    std::intptr_t index       = 0;

                    // iterate through value chars
                    for (QR_SizeT j = 0; j < valueLength; ++j)
                        // dispatch char
                        switch (action.m_Values[i][j])
                        {
                            case '/':
                            case '\\':
                            {
                                // get value
                                std::string valStr = action.m_Values[i].substr(start, j - start);

                                // is value empty?
                                if (!valStr.empty())
                                {
                                    QR_SizeT value = std::atoi(valStr.c_str());

                                    // search for list to add to
                                    switch (index)
                                    {
                                        case 0:
                                            pLine->m_PointIndexes.push_back(value);
                                            break;

                                        case 1:
                                            pLine->m_TextureIndexes.push_back(value);
                                            break;

                                        default:
                                            return false;
                                    }
                                }

                                // go to next index and value
                                ++index;
                                start = j + 1;
                                break;
                            }
                        }

                    // get last value
                    std::string valStr = action.m_Values[i].substr(start, valueLength - start);

                    // is value empty?
                    if (!valStr.empty())
                    {
                        QR_SizeT value = std::atoi(valStr.c_str());

                        // search for list to add to
                        switch (index)
                        {
                            case 0:
                                pLine->m_PointIndexes.push_back(value);
                                break;

                            case 1:
                                pLine->m_TextureIndexes.push_back(value);
                                break;

                            default:
                                return false;
                        }
                    }
                }

                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // add newly created line to group
                pGroup->m_Lines.push_back(pLine.get());
                pLine.release();
                break;
            }

            case IE_AC_BuildFace:
            {
                // create new face
                std::auto_ptr<IFace> pFace(new IFace());
                pFace->m_SmoothShading = m_CurrentSmoothShadingState;

                // get value count
                const QR_SizeT count = action.m_Values.size();

                // iterate through face values
                for (QR_SizeT i = 0; i < count; ++i)
                {
                    // get current value
                    QR_SizeT      valueLength = action.m_Values[i].length();
                    QR_SizeT      start       = 0;
                    std::intptr_t index       = 0;

                    // iterate through value chars
                    for (QR_SizeT j = 0; j < valueLength; ++j)
                        // dispatch char
                        switch (action.m_Values[i][j])
                        {
                            case '/':
                            case '\\':
                            {
                                // get value
                                std::string valStr = action.m_Values[i].substr(start, j - start);

                                // is value empty?
                                if (!valStr.empty())
                                {
                                    QR_SizeT value = std::atoi(valStr.c_str());

                                    // search for list to add to
                                    switch (index)
                                    {
                                        case 0:
                                            pFace->m_VectorIndexes.push_back(value);
                                            break;

                                        case 1:
                                            pFace->m_TextureIndexes.push_back(value);
                                            break;

                                        case 2:
                                            pFace->m_NormalIndexes.push_back(value);
                                            break;

                                        default:
                                            return false;
                                    }
                                }

                                // go to next index and value
                                ++index;
                                start = j + 1;
                                break;
                            }
                        }

                    // get last value
                    std::string valStr = action.m_Values[i].substr(start, valueLength - start);

                    // is value empty?
                    if (!valStr.empty())
                    {
                        QR_SizeT value = std::atoi(valStr.c_str());

                        // search for list to add to
                        switch (index)
                        {
                            case 0:
                                pFace->m_VectorIndexes.push_back(value);
                                break;

                            case 1:
                                pFace->m_TextureIndexes.push_back(value);
                                break;

                            case 2:
                                pFace->m_NormalIndexes.push_back(value);
                                break;

                            default:
                                return false;
                        }
                    }
                }

                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // found it?
                if (!pGroup)
                    return false;

                // add newly created face to group
                pGroup->m_Faces.push_back(pFace.get());
                pFace.release();
                break;
            }

            case IE_AC_BuildCurve:
            case IE_AC_Build2DCurve:
            case IE_AC_BuildSurface:
                M_THROW_EXCEPTION("NOT IMPLEMENTED");

            case IE_AC_SmoothShading:
            {
                // get value count
                const QR_SizeT count = action.m_Values.size();

                // is value count correct?
                if (count != 1)
                    return false;

                // set new smooth shader value
                m_CurrentSmoothShadingState = QR_StringTools::StrToBool(action.m_Values[0]);
                break;
            }

            case IE_AC_LinkMaterial:
            {
                // get value count
                const QR_SizeT count = action.m_Values.size();

                // is value count correct?
                if (count != 1)
                    return false;

                // search for existing material in resources
                IResourceMaterialDict::iterator it = m_ResourceMaterialDict.find(action.m_Values[0]);

                // found it?
                if (it != m_ResourceMaterialDict.end())
                    // load material from resources
                    return LoadMaterial(action.m_Values[0],
                                        it->second->m_Data,
                                        it->second->m_Start,
                                        it->second->m_End);

                // create a file buffer and open it for read
                QR_StdFileBuffer buffer;
                buffer.Open(action.m_Values[0], QR_FileBuffer::IE_M_Read);
                buffer.Seek(0, 0);

                // get data as string (as normally WaveFront material data are)
                std::string data = buffer.ToStr();

                // load material
                return LoadMaterial(action.m_Values[0], data, 0, data.length());
            }

            case IE_AC_UseMaterial:
            {
                // get value count
                const QR_SizeT count = action.m_Values.size();

                // is value count correct?
                if (count != 1)
                    return false;

                // get active group, create default object and group if needed
                IGroup* pGroup = GetActiveGroup();

                // set material name to use in group
                pGroup->m_Material = action.m_Values[0];

                return true;
            }

            case IE_AC_BevelInterpolation:
            case IE_AC_ColorInterpolation:
            case IE_AC_DissolveInterpolation:
            case IE_AC_LevelOfDetails:
            case IE_AC_ShadowCasting:
            case IE_AC_RayTracing:
            case IE_AC_CurveApproxTech:
            case IE_AC_SurfaceApproxTech:
                M_THROW_EXCEPTION("NOT IMPLEMENTED");

            default:
                return false;
        }
    }
    catch (...)
    {}

    return false;
}
//---------------------------------------------------------------------------
QR_WaveFrontParser::IGroup* QR_WaveFrontParser::GetActiveGroup()
{
    try
    {
        // was current group already defined?
        if (m_pCurrentGroup)
            return m_pCurrentGroup;

        // was current object already defined?
        if (!m_pCurrentObject)
        {
            // create default object
            std::auto_ptr<IObject> pObject(new IObject());
            pObject->m_Name                = M_DefaultObjectName;
            m_Objects[M_DefaultObjectName] = pObject.get();
            m_pCurrentObject               = pObject.release();
        }

        // create default group
        std::auto_ptr<IGroup> pGroup(new IGroup());
        pGroup->m_Name                                 = M_DefaultGroupName;
        m_pCurrentObject->m_Groups[M_DefaultGroupName] = pGroup.get();
        m_pCurrentGroup                                = pGroup.release();
    }
    catch (...)
    {}

    return NULL;
}
//---------------------------------------------------------------------------

