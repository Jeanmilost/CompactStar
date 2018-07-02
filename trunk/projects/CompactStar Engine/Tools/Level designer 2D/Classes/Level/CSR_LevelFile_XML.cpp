/****************************************************************************
 * ==> CSR_LevelFile_XML ---------------------------------------------------*
 ****************************************************************************
 * Description : This module provides the functions required to read and    *
 *               write the level from/to a xml file                         *
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

#include "CSR_LevelFile_XML.h"

// std
#include <sstream>

// classes
#include "CSR_Base64.h"

//---------------------------------------------------------------------------
// Global defines
//---------------------------------------------------------------------------
#define M_CSR_Xml_Header                 "xml version=\"1.0\" encoding=\"UTF-8\""
#define M_CSR_Xml_Tag_Level              "level"
#define M_CSR_Xml_Tag_Scene              "scene"
#define M_CSR_Xml_Tag_Scene_Item         "scene_item"
#define M_CSR_Xml_Tag_Shape_Type         "shape_type"
#define M_CSR_Xml_Tag_Collision_Type     "collision_type"
#define M_CSR_Xml_Tag_Skybox             "skybox"
#define M_CSR_Xml_Tag_Sound              "sound"
#define M_CSR_Xml_Tag_Landscape_Map_File "landscape_map_file"
#define M_CSR_Xml_Tag_Model_File         "model_file"
#define M_CSR_Xml_Tag_Texture_File       "texture_file"
#define M_CSR_Xml_Tag_Bumpmap_File       "bumpmap_file"
#define M_CSR_Xml_Tag_Left_File          "left_file"
#define M_CSR_Xml_Tag_Top_File           "top_file"
#define M_CSR_Xml_Tag_Right_File         "right_file"
#define M_CSR_Xml_Tag_Bottom_File        "bottom_file"
#define M_CSR_Xml_Tag_Front_File         "front_file"
#define M_CSR_Xml_Tag_Back_File          "back_file"
#define M_CSR_Xml_Tag_File               "file"
#define M_CSR_Xml_Tag_Name               "name"
#define M_CSR_Xml_Tag_Content            "content"
#define M_CSR_Xml_Tag_Bg_Color           "bg_color"
#define M_CSR_Xml_Tag_Proj_Mat           "proj_mat"
#define M_CSR_Xml_Tag_View_Mat           "view_mat"
#define M_CSR_Xml_Tag_Model_Mat          "model_mat"
#define M_CSR_Xml_Tag_Ground_Dir         "ground_dir"
#define M_CSR_Xml_Attribute_X            "x"
#define M_CSR_Xml_Attribute_Y            "y"
#define M_CSR_Xml_Attribute_Z            "z"
#define M_CSR_Xml_Attribute_R            "r"
#define M_CSR_Xml_Attribute_G            "g"
#define M_CSR_Xml_Attribute_B            "b"
#define M_CSR_Xml_Attribute_A            "a"
#define M_CSR_Xml_Attribute_11           "_11"
#define M_CSR_Xml_Attribute_12           "_12"
#define M_CSR_Xml_Attribute_13           "_13"
#define M_CSR_Xml_Attribute_14           "_14"
#define M_CSR_Xml_Attribute_21           "_21"
#define M_CSR_Xml_Attribute_22           "_22"
#define M_CSR_Xml_Attribute_23           "_23"
#define M_CSR_Xml_Attribute_24           "_24"
#define M_CSR_Xml_Attribute_31           "_31"
#define M_CSR_Xml_Attribute_32           "_32"
#define M_CSR_Xml_Attribute_33           "_33"
#define M_CSR_Xml_Attribute_34           "_34"
#define M_CSR_Xml_Attribute_41           "_41"
#define M_CSR_Xml_Attribute_42           "_42"
#define M_CSR_Xml_Attribute_43           "_43"
#define M_CSR_Xml_Attribute_44           "_44"
//---------------------------------------------------------------------------
// CSR_LevelFile_XML
//---------------------------------------------------------------------------
CSR_LevelFile_XML::CSR_LevelFile_XML(const std::string& sceneDir) :
    m_SceneDir(sceneDir),
    m_SaveContent(false)
{}
//---------------------------------------------------------------------------
CSR_LevelFile_XML::~CSR_LevelFile_XML()
{
    Clear();
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::Load(const std::string& fileName, CSR_Level& level)
{
    Clear();

    XMLDoc doc;

    try
    {
        // initialize the xml document
        XMLDoc_init(&doc);

        // read the document file content
        if (!XMLDoc_parse_file_DOM(fileName.c_str(), &doc))
            return false;

        // get the root node
        XMLNode* pNode = XMLDoc_root(&doc);

        // found it?
        if (!pNode)
            return false;

        // is the correct node?
        if (std::strcmp(pNode->tag, M_CSR_Xml_Tag_Level) != 0)
            return false;

        // was document created correctly?
        if (!Read(pNode, level))
            return false;
    }
    catch (...)
    {
        XMLDoc_free(&doc);
        return NULL;
    }

    XMLDoc_free(&doc);

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::Save(const std::string& fileName, const CSR_Level& level) const
{
    XMLDoc doc;

    try
    {
        // write the level content
        if (!Write(doc, level))
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
void CSR_LevelFile_XML::Clear()
{
    // delete all the read files
    for (IFiles::iterator it = m_Files.begin(); it != m_Files.end(); ++it)
        delete it->second;

    m_Files.clear();
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::Read(const XMLNode* pNode, CSR_Level& level)
{
    // iterate through node children
    for (int i = 0; i < pNode->n_children; ++i)
    {
        // get child node
        const XMLNode* pChild = pNode->children[i];

        // found it?
        if (!pChild)
            continue;

        // found an useful tag?
        if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_Scene) == 0)
        {
            if (!ReadScene(pChild, level))
                return false;
        }
        else
        if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_Skybox) == 0)
        {
            if (!ReadSkybox(pChild, level))
                return false;
        }
        else
        if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_Sound) == 0)
            if (!ReadSound(pChild, level))
                return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::ReadScene(const XMLNode* pNode, CSR_Level& level)
{
    /*REM
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
    */
    // iterate through node children
    for (int i = 0; i < pNode->n_children; ++i)
    {
        // get child node
        const XMLNode* pChild = pNode->children[i];

        // found it?
        if (!pChild)
            continue;
    }
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::ReadSceneItem(const XMLNode* pNode, CSR_Level& level)
{
    /*REM
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
                    if (!WriteFile(pChild, M_CSR_Xml_Tag_File, pLevelItem->m_Resource.m_Files.m_LandscapeMap))
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
                    if (!WriteFile(pChild, M_CSR_Xml_Tag_File, pLevelItem->m_Resource.m_Files.m_Model))
                    {
                        XMLNode_free(pChild);
                        return false;
                    }

                break;
        }

        // write the texture, if exists
        if (!pLevelItem->m_Resource.m_Files.m_Texture.empty())
            if (!WriteFile(pChild, M_CSR_Xml_Tag_File, pLevelItem->m_Resource.m_Files.m_Texture))
            {
                XMLNode_free(pChild);
                return false;
            }

        // write the bump map, if exists
        if (!pLevelItem->m_Resource.m_Files.m_BumpMap.empty())
            if (!WriteFile(pChild, M_CSR_Xml_Tag_File, pLevelItem->m_Resource.m_Files.m_BumpMap))
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
    */


    /*
    // iterate through node children
    for (int i = 0; i < pNode->n_children; ++i)
    {
        // get child node
        const XMLNode* pChild = pNode->children[i];

        // found it?
        if (!pChild)
            continue;

        // found an useful tag?
        if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_Shape_Type) == 0)
            // found the shape type to build
            shapeType = std::atoi(pChild->text);
        else
        if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_Shape_Type) == 0)
            // found the collision type to apply
            collisionType = std::atoi(pChild->text);
        else
        if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_Landscape_Map_File) == 0)
        {
            // found the landscape map
            if (!ReadFile(pChild, files.m_LandscapeMap))
                return false;
        }
        else
        if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_Model_File) == 0)
        {
            if (!ReadFile(pChild, files.m_Model))
                return false;
        }
        else
        if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_Texture_File) == 0)
        {
            if (!ReadFile(pChild, files.m_Texture))
                return false;
        }
        else
        if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_Bumpmap_File) == 0)
            if (!ReadFile(pChild, files.m_BumpMap))
                return false;
    }

    // if the shape type is a model, write his resources
    switch (shapeType)
    {
        case CSR_LevelManager::IE_S_Landscape:
        {
        }

        case CSR_LevelManager::IE_S_WaveFront:
        {
            break;
        }

        case CSR_LevelManager::IE_S_MDL:
        {
            break;
        }
    }
    */
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::ReadSkybox(const XMLNode* pNode, CSR_Level& level)
{
    std::size_t index = 0;

    // iterate through node children
    for (int i = 0; i < pNode->n_children; ++i)
    {
        // get child node
        const XMLNode* pChild = pNode->children[i];

        // found it?
        if (!pChild)
            continue;

        // found a skybox file?
        if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_File) == 0)
        {
            // load it
            switch (index)
            {
                case 0:
                    if (!ReadFile(pChild, level.m_Skybox.m_Left))
                        return false;

                    break;

                case 1:
                    if (!ReadFile(pChild, level.m_Skybox.m_Top))
                        return false;

                    break;

                case 2:
                    if (!ReadFile(pChild, level.m_Skybox.m_Right))
                        return false;

                    break;

                case 3:
                    if (!ReadFile(pChild, level.m_Skybox.m_Bottom))
                        return false;

                    break;

                case 4:
                    if (!ReadFile(pChild, level.m_Skybox.m_Front))
                        return false;

                    break;

                case 5:
                    if (!ReadFile(pChild, level.m_Skybox.m_Back))
                        return false;

                    break;
            }

            ++index;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::ReadSound(const XMLNode* pNode, CSR_Level& level)
{
    // iterate through node children
    for (int i = 0; i < pNode->n_children; ++i)
    {
        // get child node
        const XMLNode* pChild = pNode->children[i];

        // found it?
        if (!pChild)
            continue;

        // found the node containing the sound file?
        if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_File) == 0)
            if (!ReadFile(pChild, level.m_Sound.m_FileName))
                return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::ReadFile(const XMLNode* pNode, std::string& fileName)
{
    unsigned char* pData  = NULL;
    std::size_t    length = 0;

    try
    {
        // iterate through node children
        for (int i = 0; i < pNode->n_children; ++i)
        {
            // get child node
            const XMLNode* pChild = pNode->children[i];

            // found it?
            if (!pChild)
                continue;

            // found a file info?
            if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_Name) == 0)
                // get the file name
                fileName = pChild->text;
            else
            if (std::strcmp(pChild->tag, M_CSR_Xml_Tag_Content) == 0)
                // decode the file content
                CSR_Base64::Decode(pChild->text, pData, length);
        }

        // file has a name? (empty file name should never happen, so found one may be a corruption)
        if (fileName.empty())
        {
            if (pData)
                delete[] pData;

            return false;
        }

        // search for another file with the same name
        IFiles::const_iterator it = m_Files.find(fileName);

        // found one?
        if (it != m_Files.end())
            // file content exists?
            if (it->second)
            {
                // no new content but file content was previously found?
                if (!length)
                    // nothing else to do (deleting pData is useless in this case)
                    return true;

                // check if their lengths are identical
                if (length != it->second->m_Length)
                {
                    if (pData)
                        delete[] pData;

                    return false;
                }

                // check if their contents are identical
                const bool isIdentical = (std::memcmp(pData, it->second->m_pData, length) == 0);

                if (pData)
                    delete[] pData;

                return isIdentical;
            }
            else
            if (!pData)
                // file names are identical and both file contents are empty, nothing else to do
                return true;

        // found a file content?
        if (pData && length)
        {
            // create a buffer to contain the file data
            std::auto_ptr<CSR_Buffer> pBuffer(new CSR_Buffer());
            pBuffer->m_Length = length;
            pBuffer->m_pData  = malloc(pBuffer->m_Length);

            // copy the file content in the buffer
            std::memcpy(pBuffer->m_pData, pData, pBuffer->m_Length);

            // add the file to the list
            m_Files[fileName] = pBuffer.get();
            pBuffer.release();
        }
        else
            // just keep the file name with an empty content
            m_Files[fileName] = NULL;
    }
    catch (...)
    {
        if (pData)
            delete[] pData;

        return false;
    }

    if (pData)
        delete[] pData;

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::ReadMatrix(const XMLNode* pNode, CSR_Matrix4* pMat4)
{
    // iterate through node attributes
    for (int i = 0; i < pNode->n_attributes; ++i)
    {
        // get attribute
        XMLAttribute* pAttribute = &pNode->attributes[i];

        // found it?
        if (!pAttribute)
            continue;

        // found a matching matrix value?
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_11) == 0)
            pMat4->m_Table[0][0] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_12) == 0)
            pMat4->m_Table[1][0] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_13) == 0)
            pMat4->m_Table[2][0] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_14) == 0)
            pMat4->m_Table[3][0] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_21) == 0)
            pMat4->m_Table[0][1] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_22) == 0)
            pMat4->m_Table[1][1] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_23) == 0)
            pMat4->m_Table[2][1] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_24) == 0)
            pMat4->m_Table[3][1] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_31) == 0)
            pMat4->m_Table[0][2] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_32) == 0)
            pMat4->m_Table[1][2] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_33) == 0)
            pMat4->m_Table[2][2] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_34) == 0)
            pMat4->m_Table[3][2] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_41) == 0)
            pMat4->m_Table[0][3] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_42) == 0)
            pMat4->m_Table[1][3] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_43) == 0)
            pMat4->m_Table[2][3] = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_44) == 0)
            pMat4->m_Table[3][3] = std::atof(pAttribute->value);
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::ReadVector(const XMLNode* pNode, CSR_Vector3* pVec3)
{
    // iterate through node attributes
    for (int i = 0; i < pNode->n_attributes; ++i)
    {
        // get attribute
        XMLAttribute* pAttribute = &pNode->attributes[i];

        // found it?
        if (!pAttribute)
            continue;

        // found a matching vector value?
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_X) == 0)
            pVec3->m_X = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_Y) == 0)
            pVec3->m_Y = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_Z) == 0)
            pVec3->m_Z = std::atof(pAttribute->value);
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::ReadColor(const XMLNode* pNode, CSR_Color* pColor)
{
    // iterate through node attributes
    for (int i = 0; i < pNode->n_attributes; ++i)
    {
        // get attribute
        XMLAttribute* pAttribute = &pNode->attributes[i];

        // found it?
        if (!pAttribute)
            continue;

        // found a matching color value?
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_R) == 0)
            pColor->m_R = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_G) == 0)
            pColor->m_G = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_B) == 0)
            pColor->m_B = std::atof(pAttribute->value);
        else
        if (std::strcmp(pAttribute->name, M_CSR_Xml_Attribute_A) == 0)
            pColor->m_A = std::atof(pAttribute->value);
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::Write(XMLDoc& doc, const CSR_Level& level) const
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
        if (!WriteLevel(doc, level))
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
bool CSR_LevelFile_XML::WriteLevel(XMLDoc& doc, const CSR_Level& level) const
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
        if (!WriteScene(pNode, level))
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
bool CSR_LevelFile_XML::WriteScene(XMLNode* pNode, const CSR_Level& level) const
{
    XMLNode* pChild = NULL;

    try
    {
        // create new xml node to contain the scene data
        pChild = AddTag(pNode, M_CSR_Xml_Tag_Scene);

        // succeeded?
        if (!pChild)
            return false;

        // get the scene to write
        const CSR_Scene* pScene = level.GetScene();

        // found it?
        if (!pScene)
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
bool CSR_LevelFile_XML::WriteSceneItem(      XMLNode*          pNode,
                                       const CSR_SceneItem*    pSceneItem,
                                       const CSR_Level::IItem* pLevelItem) const
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
        if (!AddTag(pChild, M_CSR_Xml_Tag_Shape_Type, pLevelItem->m_Type))
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
        switch (pLevelItem->m_Type)
        {
            case CSR_Level::IE_IT_Landscape:
                // write the landscape map, if exists
                if (!pLevelItem->m_Resources.m_Files.m_LandscapeMap.empty())
                {
                    if (!WriteFile(pChild,
                                   M_CSR_Xml_Tag_Landscape_Map_File,
                                   pLevelItem->m_Resources.m_Files.m_LandscapeMap))
                    {
                        XMLNode_free(pChild);
                        return false;
                    }

                    break;
                }

                // NOTE don't break because the landscape may be a model too

            case CSR_Level::IE_IT_WaveFront:
            case CSR_Level::IE_IT_MDL:
                // write the landscape model, if exists
                if (!pLevelItem->m_Resources.m_Files.m_Model.empty())
                    if (!WriteFile(pChild,
                                   M_CSR_Xml_Tag_Model_File,
                                   pLevelItem->m_Resources.m_Files.m_Model))
                    {
                        XMLNode_free(pChild);
                        return false;
                    }

                break;
        }

        // write the texture, if exists
        if (!pLevelItem->m_Resources.m_Files.m_Texture.empty())
            if (!WriteFile(pChild, M_CSR_Xml_Tag_Texture_File, pLevelItem->m_Resources.m_Files.m_Texture))
            {
                XMLNode_free(pChild);
                return false;
            }

        // write the bump map, if exists
        if (!pLevelItem->m_Resources.m_Files.m_BumpMap.empty())
            if (!WriteFile(pChild, M_CSR_Xml_Tag_Bumpmap_File, pLevelItem->m_Resources.m_Files.m_BumpMap))
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
bool CSR_LevelFile_XML::WriteSkybox(XMLNode* pNode, const CSR_Level& level) const
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
        if (!WriteFile(pChild, M_CSR_Xml_Tag_Left_File, level.m_Skybox.m_Left))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the skybox top file content
        if (!WriteFile(pChild, M_CSR_Xml_Tag_Top_File, level.m_Skybox.m_Top))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the skybox right file content
        if (!WriteFile(pChild, M_CSR_Xml_Tag_Right_File, level.m_Skybox.m_Right))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the skybox bottom file content
        if (!WriteFile(pChild, M_CSR_Xml_Tag_Bottom_File, level.m_Skybox.m_Bottom))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the skybox front file content
        if (!WriteFile(pChild, M_CSR_Xml_Tag_Front_File, level.m_Skybox.m_Front))
        {
            XMLNode_free(pChild);
            return false;
        }

        // write the skybox back file content
        if (!WriteFile(pChild, M_CSR_Xml_Tag_Back_File, level.m_Skybox.m_Back))
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
bool CSR_LevelFile_XML::WriteSound(XMLNode* pNode, const CSR_Level& level) const
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
        if (!WriteFile(pChild, M_CSR_Xml_Tag_File, level.m_Sound.m_FileName))
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
bool CSR_LevelFile_XML::WriteFile(XMLNode* pNode, const std::string& name, const std::string& fileName) const
{
    // no file to write?
    if (fileName.empty())
        // do nothing, but don't return an error
        return true;

    XMLNode* pChild = NULL;

    try
    {
        // create new xml node to contain the sound
        pChild = AddTag(pNode, name.c_str());

        // succeeded?
        if (!pChild)
            return false;

        std::string nameToWrite;

        // do use relative paths?
        if (!m_SceneDir.empty() && fileName.find(m_SceneDir) == 0)
        {
            // remove the scene dir (later the root path will be the path of the xml itself)
            nameToWrite = fileName.substr(m_SceneDir.length(), fileName.length() - m_SceneDir.length());

            // remove the first separator, if any
            if (!nameToWrite.empty() && nameToWrite[0] == '\\')
                nameToWrite = nameToWrite.substr(1, nameToWrite.length() - 1);
        }
        else
            nameToWrite = fileName;

        // no remaining file name?
        if (nameToWrite.empty())
            return false;

        // write the file name
        if (!AddTag(pChild, M_CSR_Xml_Tag_Name, nameToWrite.c_str()))
            return false;

        // do write the file content?
        if (!m_SaveContent)
            return true;

        // open the file
        CSR_Buffer* pBuffer = csrFileOpen(fileName.c_str());

        // succeeded?
        if (!pBuffer)
            return false;

        try
        {
            // base64 encode the file content and write it
            if (!AddTag(pChild,
                        M_CSR_Xml_Tag_Content,
                        CSR_Base64::Encode((unsigned char*)pBuffer->m_pData, pBuffer->m_Length)))
                return false;
        }
        catch (...)
        {
            csrBufferRelease(pBuffer);
            return false;
        }

        csrBufferRelease(pBuffer);
    }
    catch (...)
    {
        XMLNode_free(pChild);
        throw;
    }

    return true;
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::WriteMatrix(XMLNode* pNode, const std::string& name, const CSR_Matrix4* pMat4) const
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
bool CSR_LevelFile_XML::WriteVector(XMLNode* pNode, const std::string& name, const CSR_Vector3* pVec3) const
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
bool CSR_LevelFile_XML::WriteColor(XMLNode* pNode, const std::string& name, const CSR_Color* pColor) const
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
XMLNode* CSR_LevelFile_XML::AddTag(XMLDoc& doc, const std::string& name) const
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
XMLNode* CSR_LevelFile_XML::AddTag(XMLDoc& doc, const std::string& name, const std::string& value) const
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
XMLNode* CSR_LevelFile_XML::AddTag(XMLDoc& doc, const std::string& name, int value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddTag(doc, name, sstr.str());
}
//---------------------------------------------------------------------------
XMLNode* CSR_LevelFile_XML::AddTag(XMLDoc& doc, const std::string& name, float value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddTag(doc, name, sstr.str());
}
//---------------------------------------------------------------------------
XMLNode* CSR_LevelFile_XML::AddTag(XMLNode* pParent, const std::string& name) const
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
XMLNode* CSR_LevelFile_XML::AddTag(XMLNode* pParent, const std::string& name, const std::string& value) const
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
XMLNode* CSR_LevelFile_XML::AddTag(XMLNode* pParent, const std::string& name, int value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddTag(pParent, name, sstr.str());
}
//---------------------------------------------------------------------------
XMLNode* CSR_LevelFile_XML::AddTag(XMLNode* pParent, const std::string& name, float value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddTag(pParent, name, sstr.str());
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::AddValue(XMLNode* pNode, const std::string& value) const
{
    return XMLNode_set_text(pNode, value.c_str());
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::AddAttribute(XMLNode* pNode, const std::string& name, const std::string& value) const
{
    return (XMLNode_set_attribute(pNode, name.c_str(), value.c_str()) >= 0);
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::AddAttribute(XMLNode* pNode, const std::string& name, int value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddAttribute(pNode, name.c_str(), sstr.str().c_str());
}
//---------------------------------------------------------------------------
bool CSR_LevelFile_XML::AddAttribute(XMLNode* pNode, const std::string& name, float value) const
{
    std::ostringstream sstr;
    sstr << value;

    return AddAttribute(pNode, name.c_str(), sstr.str().c_str());
}
//---------------------------------------------------------------------------