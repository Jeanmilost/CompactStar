/****************************************************************************
 * ==> CSR_LevelSerializer -------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the functions required to serialize   *
 *               the level                                                  *
 * Developer   : Jean-Milost Reymond                                        *
 * Copyright   : 2017 - 2018, this file is part of the CompactStar Engine.  *
 *               You are free to copy or redistribute this file, modify it, *
 *               or use it for your own projects, commercial or not. This   *
 *               file is provided "as is", WITHOUT ANY WARRANTY OF ANY      *
 *               KIND. THE DEVELOPER IS NOT RESPONSIBLE FOR ANY DAMAGE OF   *
 *               ANY KIND, ANY LOSS OF DATA, OR ANY LOSS OF PRODUCTIVITY    *
 *               TIME THAT MAY RESULT FROM THE USAGE OF THIS SOURCE CODE,   *
 *               DIRECTLY OR NOT.                                           *
 ****************************************************************************/

#include "CSR_LevelSerializer.h"

// std
#include <sstream>

// compactStar engine
#include "CSR_Common.h"

// classes
#include "CSR_Base64.h"

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------
#define M_CSR_Xml_Header             "xml version=\"1.0\" encoding=\"UTF-8\""
#define M_CSR_Xml_Tag_Level          "level"
#define M_CSR_Xml_Tag_Scene          "scene"
#define M_CSR_Xml_Tag_Scene_Item     "scene_item"
#define M_CSR_Xml_Tag_Shape_Type     "shape_type"
#define M_CSR_Xml_Tag_Collision_Type "collision_type"
#define M_CSR_Xml_Tag_Skybox         "skybox"
#define M_CSR_Xml_Tag_Sound          "sound"
#define M_CSR_Xml_Tag_File           "file"
#define M_CSR_Xml_Tag_Bg_Color       "bg_color"
#define M_CSR_Xml_Tag_Proj_Mat       "proj_mat"
#define M_CSR_Xml_Tag_View_Mat       "view_mat"
#define M_CSR_Xml_Tag_Model_Mat      "model_mat"
#define M_CSR_Xml_Tag_Ground_Dir     "ground_dir"
#define M_CSR_Xml_Attribute_X        "x"
#define M_CSR_Xml_Attribute_Y        "y"
#define M_CSR_Xml_Attribute_Z        "z"
#define M_CSR_Xml_Attribute_R        "r"
#define M_CSR_Xml_Attribute_G        "g"
#define M_CSR_Xml_Attribute_B        "b"
#define M_CSR_Xml_Attribute_A        "a"
#define M_CSR_Xml_Attribute_11       "_11"
#define M_CSR_Xml_Attribute_12       "_12"
#define M_CSR_Xml_Attribute_13       "_13"
#define M_CSR_Xml_Attribute_14       "_14"
#define M_CSR_Xml_Attribute_21       "_21"
#define M_CSR_Xml_Attribute_22       "_22"
#define M_CSR_Xml_Attribute_23       "_23"
#define M_CSR_Xml_Attribute_24       "_24"
#define M_CSR_Xml_Attribute_31       "_31"
#define M_CSR_Xml_Attribute_32       "_32"
#define M_CSR_Xml_Attribute_33       "_33"
#define M_CSR_Xml_Attribute_34       "_34"
#define M_CSR_Xml_Attribute_41       "_41"
#define M_CSR_Xml_Attribute_42       "_42"
#define M_CSR_Xml_Attribute_43       "_43"
#define M_CSR_Xml_Attribute_44       "_44"
//---------------------------------------------------------------------------
// CSR_LevelSerializer
//---------------------------------------------------------------------------
CSR_LevelSerializer::CSR_LevelSerializer(bool writeFileName):
    m_WriteFileName(writeFileName)
{}
//---------------------------------------------------------------------------
CSR_LevelSerializer::~CSR_LevelSerializer()
{}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::Save(const std::string&      fileName,
                               const CSR_Scene*        pScene,
                               const CSR_LevelManager& level) const
{
    if (!pScene)
        return false;

    XMLDoc doc;

    try
    {
        // write the level content
        if (!Write(doc, pScene, level))
        {
            XMLDoc_free(&doc);
            return false;
        }

        std::FILE* pFile;

        // open file for write
        pFile = std::fopen(fileName.c_str(), "w");

        // succeeded?
        if (!pFile)
        {
            XMLDoc_free(&doc);
            return false;
        }

        // write the xml document
        if (!XMLDoc_print(&doc, pFile, "\n", "\t", 0, 0, 0))
        {
            XMLDoc_free(&doc);
            return false;
        }

        std::fclose(pFile);
    }
    catch (...)
    {
        XMLDoc_free(&doc);
        return false;
    }

    XMLDoc_free(&doc);
    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::Write(XMLDoc&           doc,
                          const CSR_Scene*        pScene,
                          const CSR_LevelManager& level) const
{
    // create and initialize new xml document
    XMLDoc_init(&doc);

    // create new xml node to contain the header
    XMLNode* pNode = XMLNode_alloc();

    try
    {
        // succeeded?
        if (!pNode)
            return false;

        // set xml header content
        if (!XMLNode_set_tag(pNode, M_CSR_Xml_Header))
        {
            XMLNode_free(pNode);
            return false;
        }

        // configure header node
        if (!XMLNode_set_type(pNode, TAG_INSTR))
        {
            XMLNode_free(pNode);
            return false;
        }

        // add xml header
        if (!XMLDoc_add_node(&doc, pNode))
        {
            XMLNode_free(pNode);
            return false;
        }

        // write the level
        if (!WriteLevel(doc, pScene, level))
        {
            XMLNode_free(pNode);
            return false;
        }
    }
    catch (...)
    {
        XMLNode_free(pNode);
        throw;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::WriteLevel(XMLDoc&           doc,
                               const CSR_Scene*        pScene,
                               const CSR_LevelManager& level) const
{
    XMLNode* pNode = NULL;

    try
    {
        // create new xml node to contain the level data
        pNode = AddTag(doc, M_CSR_Xml_Tag_Level);

        // succeeded?
        if (!pNode)
            return false;

        // write the scene
        if (!WriteScene(pNode, pScene, level))
        {
            XMLNode_free(pNode);
            return false;
        }

        // write the skybox files to use
        if (!WriteSkybox(pNode, level))
        {
            XMLNode_free(pNode);
            return false;
        }

        // write the ambient sound file to use
        if (!WriteSound(pNode, level))
        {
            XMLNode_free(pNode);
            return false;
        }
    }
    catch (...)
    {
        XMLNode_free(pNode);
        throw;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::WriteScene(XMLNode*          pNode,
                               const CSR_Scene*        pScene,
                               const CSR_LevelManager& level) const
{
    XMLNode* pChild = NULL;

    try
    {
        // create new xml node to contain the scene data
        pChild = AddTag(pNode, M_CSR_Xml_Tag_Scene);

        // succeeded?
        if (!pChild)
            return false;

        // write the scene backgroud color
        if (!WriteColor(pChild, M_CSR_Xml_Tag_Bg_Color, &pScene->m_Color))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the scene projection matrix
        if (!WriteMatrix(pChild, M_CSR_Xml_Tag_Proj_Mat, &pScene->m_ProjectionMatrix))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the scene view matrix
        if (!WriteMatrix(pChild, M_CSR_Xml_Tag_View_Mat, &pScene->m_ViewMatrix))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the scene ground dir
        if (!WriteVector(pChild, M_CSR_Xml_Tag_Ground_Dir, &pScene->m_GroundDir))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the scene items
        for (std::size_t i = 0; i < pScene->m_ItemCount; ++i)
            if (!WriteSceneItem(pChild, &pScene->m_pItem[i], level.Get(pScene->m_pItem[i].m_pModel)))
            {
                XMLNode_free(pChild);
                return false;
            }

        // write the scene transparent items
        for (std::size_t i = 0; i < pScene->m_TransparentItemCount; ++i)
            if (!WriteSceneItem(pChild,
                               &pScene->m_pTransparentItem[i],
                                level.Get(pScene->m_pTransparentItem[i].m_pModel)))
            {
                XMLNode_free(pChild);
                return false;
            }
    }
    catch (...)
    {
        XMLNode_free(pChild);
        throw;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::WriteSceneItem(XMLNode*                 pNode,
                                   const CSR_SceneItem*           pSceneItem,
                                   const CSR_LevelManager::IItem* pLevelItem) const
{
    // validate the inputs
    if (!pSceneItem || !pLevelItem)
        return false;

    XMLNode* pChild = NULL;

    try
    {
        // create new xml node to contain the skybox
        pChild = AddTag(pNode, M_CSR_Xml_Tag_Scene_Item);

        // succeeded?
        if (!pChild)
            return false;

        // write the shape type
        if (!AddTag(pChild, M_CSR_Xml_Tag_Shape_Type, pLevelItem->m_Resource.m_ShapeType))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the collision type
        if (!AddTag(pChild, M_CSR_Xml_Tag_Collision_Type, pSceneItem->m_CollisionType))
        {
            XMLNode_free(pChild);
            return false;
        }

        // if the shape type is a model, write his resources
        switch (pLevelItem->m_Resource.m_ShapeType)
        {
            case CSR_LevelManager::IE_S_Landscape:
                // write the landscape map, if exists
                if (!pLevelItem->m_Resource.m_Files.m_LandscapeMap.empty())
                {
                    if (!WriteFile(pChild, pLevelItem->m_Resource.m_Files.m_LandscapeMap))
                    {
                        XMLNode_free(pChild);
                        return false;
                    }

                    break;
                }

                // NOTE don't break because the landscape may be a model too

            case CSR_LevelManager::IE_S_WaveFront:
            case CSR_LevelManager::IE_S_MDL:
                // write the landscape model, if exists
                if (!pLevelItem->m_Resource.m_Files.m_Model.empty())
                    if (!WriteFile(pChild, pLevelItem->m_Resource.m_Files.m_Model))
                    {
                        XMLNode_free(pChild);
                        return false;
                    }

                break;
        }

        // write the texture, if exists
        if (!pLevelItem->m_Resource.m_Files.m_Texture.empty())
            if (!WriteFile(pChild, pLevelItem->m_Resource.m_Files.m_Texture))
            {
                XMLNode_free(pChild);
                return false;
            }

        // write the bump map, if exists
        if (!pLevelItem->m_Resource.m_Files.m_BumpMap.empty())
            if (!WriteFile(pChild, pLevelItem->m_Resource.m_Files.m_BumpMap))
            {
                XMLNode_free(pChild);
                return false;
            }

        // write the model matrices
        if (pSceneItem->m_pMatrixArray)
            for (std::size_t i = 0; i < pSceneItem->m_pMatrixArray->m_Count; ++i)
                if (!WriteMatrix(pChild,
                                 M_CSR_Xml_Tag_Model_Mat,
                                 (CSR_Matrix4*)(pSceneItem->m_pMatrixArray[i]).m_pItem->m_pData))
                {
                    XMLNode_free(pChild);
                    return false;
                }
    }
    catch (...)
    {
        XMLNode_free(pChild);
        throw;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::WriteSkybox(XMLNode* pNode, const CSR_LevelManager& level) const
{
    // no skybox to write?
    if (level.m_Skybox.m_Left.empty()   ||
        level.m_Skybox.m_Top.empty()    ||
        level.m_Skybox.m_Right.empty()  ||
        level.m_Skybox.m_Bottom.empty() ||
        level.m_Skybox.m_Front.empty()  ||
        level.m_Skybox.m_Back.empty())
        // do nothing, but don't return an error
        return true;

    XMLNode* pChild = NULL;

    try
    {
        // create new xml node to contain the skybox
        pChild = AddTag(pNode, M_CSR_Xml_Tag_Skybox);

        // succeeded?
        if (!pChild)
            return false;

        // write the skybox left file content
        if (!WriteFile(pChild, level.m_Skybox.m_Left))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the skybox top file content
        if (!WriteFile(pChild, level.m_Skybox.m_Top))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the skybox right file content
        if (!WriteFile(pChild, level.m_Skybox.m_Right))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the skybox bottom file content
        if (!WriteFile(pChild, level.m_Skybox.m_Bottom))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the skybox front file content
        if (!WriteFile(pChild, level.m_Skybox.m_Front))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the skybox back file content
        if (!WriteFile(pChild, level.m_Skybox.m_Back))
        {
            XMLNode_free(pChild);
            return false;
        }
    }
    catch (...)
    {
        XMLNode_free(pChild);
        throw;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::WriteSound(XMLNode* pNode, const CSR_LevelManager& level) const
{
    // no sound to write?
    if (level.m_Sound.m_FileName.empty())
        // do nothing, but don't return an error
        return true;

    XMLNode* pChild = NULL;

    try
    {
        // create new xml node to contain the sound
        pChild = AddTag(pNode, M_CSR_Xml_Tag_Sound);

        // succeeded?
        if (!pChild)
            return false;

        // write the sound file content
        if (!WriteFile(pChild, level.m_Sound.m_FileName))
        {
            XMLNode_free(pChild);
            return false;
        }
    }
    catch (...)
    {
        XMLNode_free(pChild);
        throw;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::WriteFile(XMLNode* pNode, const std::string& fileName) const
{
    // do write the file name instead of the file content?
    if (m_WriteFileName)
        return AddTag(pNode, M_CSR_Xml_Tag_File, fileName.c_str());

    // open the file
    CSR_Buffer* pBuffer = csrFileOpen(fileName.c_str());

    // succeeded?
    if (!pBuffer)
        return false;

    try
    {
        // base64 encode the file content and write it
        if (!AddTag(pNode,
                    M_CSR_Xml_Tag_File,
                    CSR_Base64::Encode((unsigned char*)pBuffer->m_pData, pBuffer->m_Length)))
            return false;
    }
    __finally
    {
        csrBufferRelease(pBuffer);
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::WriteMatrix(XMLNode* pNode, const std::string& name, const CSR_Matrix4* pMat4) const
{
    // is vector empty?
    if (pMat4->m_Table[0][0] == 1.0f && !pMat4->m_Table[1][0]         && !pMat4->m_Table[2][0]         && !pMat4->m_Table[3][0] &&
       !pMat4->m_Table[0][1]         &&  pMat4->m_Table[1][1] == 1.0f && !pMat4->m_Table[2][1]         && !pMat4->m_Table[3][1] &&
       !pMat4->m_Table[0][2]         && !pMat4->m_Table[1][2]         &&  pMat4->m_Table[2][2] == 1.0f && !pMat4->m_Table[3][2] &&
       !pMat4->m_Table[0][3]         && !pMat4->m_Table[1][3]         && !pMat4->m_Table[2][3]         &&  pMat4->m_Table[3][3] == 1.0f)
        // do nothing, but don't return an error
        return true;

    XMLNode* pChild = NULL;

    try
    {
        // create new xml node to contain the sound
        pChild = AddTag(pNode, name);

        // succeeded?
        if (!pChild)
            return false;

        // write the matrix _11 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_11, pMat4->m_Table[0][0]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _12 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_12, pMat4->m_Table[1][0]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _13 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_13, pMat4->m_Table[2][0]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _14 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_14, pMat4->m_Table[3][0]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _21 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_21, pMat4->m_Table[0][1]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _22 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_22, pMat4->m_Table[1][1]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _23 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_23, pMat4->m_Table[2][1]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _24 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_24, pMat4->m_Table[3][1]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _31 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_31, pMat4->m_Table[0][2]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _32 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_32, pMat4->m_Table[1][2]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _33 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_33, pMat4->m_Table[2][2]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _34 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_34, pMat4->m_Table[3][2]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _41 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_41, pMat4->m_Table[0][3]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _42 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_42, pMat4->m_Table[1][3]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _43 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_43, pMat4->m_Table[2][3]))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the matrix _44 attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_44, pMat4->m_Table[3][3]))
        {
            XMLNode_free(pChild);
            return false;
        }
    }
    catch (...)
    {
        XMLNode_free(pChild);
        throw;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::WriteVector(XMLNode* pNode, const std::string& name, const CSR_Vector3* pVec3) const
{
    // is vector empty?
    if (!pVec3->m_X && !pVec3->m_Y && !pVec3->m_Z)
        // do nothing, but don't return an error
        return true;

    XMLNode* pChild = NULL;

    try
    {
        // create new xml node to contain the sound
        pChild = AddTag(pNode, name);

        // succeeded?
        if (!pChild)
            return false;

        // write the vector x attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_X, pVec3->m_X))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the vector x attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_Y, pVec3->m_Y))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the vector x attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_Z, pVec3->m_Z))
        {
            XMLNode_free(pChild);
            return false;
        }
    }
    catch (...)
    {
        XMLNode_free(pChild);
        throw;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::WriteColor(XMLNode* pNode, const std::string& name, const CSR_Color* pColor) const
{
    // is color empty?
    if (!pColor->m_R && !pColor->m_G && !pColor->m_B && !pColor->m_A)
        // do nothing, but don't return an error
        return true;

    XMLNode* pChild = NULL;

    try
    {
        // create new xml node to contain the sound
        pChild = AddTag(pNode, name);

        // succeeded?
        if (!pChild)
            return false;

        // write the vector red attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_R, pColor->m_R))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the vector green attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_G, pColor->m_G))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the vector blue attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_B, pColor->m_B))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the vector alpha attribute
        if (!AddAttribute(pChild, M_CSR_Xml_Attribute_A, pColor->m_A))
        {
            XMLNode_free(pChild);
            return false;
        }
    }
    catch (...)
    {
        XMLNode_free(pChild);
        throw;
    }

    return true;
}
//---------------------------------------------------------------------------
XMLNode* CSR_LevelSerializer::AddTag(XMLDoc& doc, const std::string& name) const
{
    XMLNode* pNode = XMLNode_alloc();

    try
    {
        // set tag name
        if (!XMLNode_set_tag(pNode, name.c_str()))
        {
            XMLNode_free(pNode);
            return NULL;
        }

        // add tag to document
        if (!XMLDoc_add_node(&doc, pNode))
        {
            XMLNode_free(pNode);
            return NULL;
        }
    }
    catch (...)
    {
        XMLNode_free(pNode);
        throw;
    }

    return pNode;
}
//---------------------------------------------------------------------------
XMLNode* CSR_LevelSerializer::AddTag(XMLDoc& doc, const std::string& name, const std::string& value) const
{
    XMLNode* pNode = AddTag(doc, name);

    if (!pNode)
        return NULL;

    try
    {
        // add value to tag
        if (!AddValue(pNode, value))
        {
            XMLNode_free(pNode);
            return NULL;
        }
    }
    catch (...)
    {
        XMLNode_free(pNode);
        throw;
    }

    return pNode;
}
//---------------------------------------------------------------------------
XMLNode* CSR_LevelSerializer::AddTag(XMLDoc& doc, const std::string& name, int value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddTag(doc, name, sstr.str());
}
//---------------------------------------------------------------------------
XMLNode* CSR_LevelSerializer::AddTag(XMLDoc& doc, const std::string& name, float value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddTag(doc, name, sstr.str());
}
//---------------------------------------------------------------------------
XMLNode* CSR_LevelSerializer::AddTag(XMLNode* pParent, const std::string& name) const
{
    XMLNode* pNode = XMLNode_alloc();

    try
    {
        // set tag name
        if (!XMLNode_set_tag(pNode, name.c_str()))
        {
            XMLNode_free(pNode);
            return NULL;
        }

        // add tag to parent
        if (!XMLNode_add_child(pParent, pNode))
        {
            XMLNode_free(pNode);
            return NULL;
        }
    }
    catch (...)
    {
        XMLNode_free(pNode);
        throw;
    }

    return pNode;
}
//---------------------------------------------------------------------------
XMLNode* CSR_LevelSerializer::AddTag(XMLNode* pParent, const std::string& name, const std::string& value) const
{
    XMLNode* pNode = AddTag(pParent, name);

    if (!pNode)
        return NULL;

    try
    {
        // add value to tag
        if (!AddValue(pNode, value))
        {
            XMLNode_free(pNode);
            return NULL;
        }
    }
    catch (...)
    {
        XMLNode_free(pNode);
        throw;
    }

    return pNode;
}
//---------------------------------------------------------------------------
XMLNode* CSR_LevelSerializer::AddTag(XMLNode* pParent, const std::string& name, int value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddTag(pParent, name, sstr.str());
}
//---------------------------------------------------------------------------
XMLNode* CSR_LevelSerializer::AddTag(XMLNode* pParent, const std::string& name, float value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddTag(pParent, name, sstr.str());
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::AddValue(XMLNode* pNode, const std::string& value) const
{
    return XMLNode_set_text(pNode, value.c_str());
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::AddAttribute(XMLNode* pNode, const std::string& name, const std::string& value) const
{
    return (XMLNode_set_attribute(pNode, name.c_str(), value.c_str()) >= 0);
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::AddAttribute(XMLNode* pNode, const std::string& name, int value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddAttribute(pNode, name.c_str(), sstr.str().c_str());
}
//---------------------------------------------------------------------------
bool CSR_LevelSerializer::AddAttribute(XMLNode* pNode, const std::string& name, float value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddAttribute(pNode, name.c_str(), sstr.str().c_str());
}
//---------------------------------------------------------------------------
