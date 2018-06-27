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

#ifndef CSR_LevelSerializerH
#define CSR_LevelSerializerH

// compactStar engine
#include "CSR_Scene.h"

// classes
#include "CSR_LevelManager.h"

// xml
#include "sxmlc.h"

/**
* Level serializer
*@author Jean-Milost Reymond
*/
class CSR_LevelSerializer
{
    public:
        /**
        * Constructor
        *@param writeFileName - if true, the file name will be written instead of the file content
        */
        CSR_LevelSerializer(bool writeFileName = true);

        virtual ~CSR_LevelSerializer();

        /**
        * Saves the level
        *@param fileName - level file name
        *@param pScene - scene belonging to the level to save
        *@param level - level to save
        *@return true on success, otherwise false
        */
        virtual bool Save(const std::string&      fileName,
                          const CSR_Scene*        pScene,
                          const CSR_LevelManager& level) const;

    private:
        bool m_WriteFileName;

        /**
        * Writes the level
        *@param doc - xml document in which the level should be written
        *@param pScene - scene belonging to the level to save
        *@param level - level to save
        *@return true on success, otherwise false
        */
        bool Write(XMLDoc&           doc,
             const CSR_Scene*        pScene,
             const CSR_LevelManager& level) const;

        /**
        * Writes the level
        *@param doc - xml document in which the level should be written
        *@param pScene - scene belonging to the level to save
        *@param level - level to save
        *@return true on success, otherwise false
        */
        bool WriteLevel(XMLDoc&           doc,
                  const CSR_Scene*        pScene,
                  const CSR_LevelManager& level) const;

        /**
        * Writes the scene belonging to the level
        *@param pNode - xml node in which the scene should be written
        *@param pScene - scene belonging to the level to save
        *@param level - level to save
        *@return true on success, otherwise false
        */
        bool WriteScene(XMLNode*          pNode,
                  const CSR_Scene*        pScene,
                  const CSR_LevelManager& level) const;

        /**
        * Writes a scene item
        *@param pNode - xml node in which the scene item should be written
        *@param pSceneItem - scene item to save
        *@param pLevelItem - level item to save
        *@return true on success, otherwise false
        */
        bool WriteSceneItem(XMLNode*                 pNode,
                      const CSR_SceneItem*           pSceneItem,
                      const CSR_LevelManager::IItem* pLevelItem) const;

        /**
        * Writes the skybox
        *@param pNode - xml node in which the skybox should be written
        *@param level - level at which the skybox belongs
        *@return true on success, otherwise false
        */
        bool WriteSkybox(XMLNode* pNode, const CSR_LevelManager& level) const;

        /**
        * Writes the sound
        *@param pNode - xml node in which the sound should be written
        *@param level - level at which the skybox belongs
        *@return true on success, otherwise false
        */
        bool WriteSound(XMLNode* pNode, const CSR_LevelManager& level) const;

        /**
        * Writes a file
        *@param pNode - xml node in which the file should be written
        *@param fileName - file name to write
        *@return true on success, otherwise false
        */
        bool WriteFile(XMLNode* pNode, const std::string& fileName) const;

        /**
        * Writes a matrix
        *@param pNode - xml node in which the matrix should be written
        *@param name - matrix name
        *@param pMat4 - matrix to write
        *@return true on success, otherwise false
        */
        bool WriteMatrix(XMLNode* pNode, const std::string& name, const CSR_Matrix4* pMat4) const;

        /**
        * Writes a vector
        *@param pNode - xml node in which the vector should be written
        *@param name - vector name
        *@param pVec3 - vector to write
        *@return true on success, otherwise false
        */
        bool WriteVector(XMLNode* pNode, const std::string& name, const CSR_Vector3* pVec3) const;

        /**
        * Writes a color
        *@param pNode - xml node in which the color should be written
        *@param name - color name
        *@param pColor - color to write
        *@return true on success, otherwise false
        */
        bool WriteColor(XMLNode* pNode, const std::string& name, const CSR_Color* pColor) const;

        /**
        * Adds a tag
        *@param doc - xml document to add to
        *@param name - tag name
        *@return newly added tag, NULL on error
        */
        XMLNode* AddTag(XMLDoc& doc, const std::string& name) const;

        /**
        * Adds a tag (in the form <name>value</name>)
        *@param doc - xml document to add to
        *@param name - tag name
        *@param value - tag value
        *@return newly added tag, NULL on error
        */
        XMLNode* AddTag(XMLDoc& doc, const std::string& name, const std::string& value) const;

        /**
        * Adds a tag (in the form <name>value</name>)
        *@param doc - xml document to add to
        *@param name - tag name
        *@param value - tag value
        *@return newly added tag, NULL on error
        */
        XMLNode* AddTag(XMLDoc& doc, const std::string& name, int value) const;

        /**
        * Adds a tag (in the form <name>value</name>)
        *@param doc - xml document to add to
        *@param name - tag name
        *@param value - tag value
        *@return newly added tag, NULL on error
        */
        XMLNode* AddTag(XMLDoc& doc, const std::string& name, float value) const;

        /**
        * Adds a tag
        *@param pParent - parent tag to add to
        *@param name - tag name
        *@return newly added tag, NULL on error
        */
        XMLNode* AddTag(XMLNode* pParent, const std::string& name) const;

        /**
        * Adds a tag (in the form <name>value</name>)
        *@param pParent - parent tag to add to
        *@param name - tag name
        *@param value - tag value
        *@return newly added tag, NULL on error
        */
        XMLNode* AddTag(XMLNode* pParent, const std::string& name, const std::string& value) const;

        /**
        * Adds a tag (in the form <name>value</name>)
        *@param pParent - parent tag to add to
        *@param name - tag name
        *@param value - tag value
        *@return newly added tag, NULL on error
        */
        XMLNode* AddTag(XMLNode* pParent, const std::string& name, int value) const;

        /**
        * Adds a tag (in the form <name>value</name>)
        *@param pParent - parent tag to add to
        *@param name - tag name
        *@param value - tag value
        *@return newly added tag, NULL on error
        */
        XMLNode* AddTag(XMLNode* pParent, const std::string& name, float value) const;

        /**
        * Adds a value to a node (i.e. in the form <tag>value</tag>)
        *@param pNode - node for which value should be added
        *@param value - value
        *@returns true on success, otherwise false
        */
        bool AddValue(XMLNode* pNode, const std::string& value) const;

        /**
        * Adds an attribute to a node (i.e. in the form name="value")
        *@param pNode - node for which attribute should be added
        *@param name - attribute name
        *@param value - attribute value
        *@returns true on success, otherwise false
        */
        bool AddAttribute(XMLNode* pNode, const std::string& name, const std::string& value) const;

        /**
        * Adds an attribute to a node (i.e. in the form name="value")
        *@param pNode - node for which attribute should be added
        *@param name - attribute name
        *@param value - attribute value
        *@returns true on success, otherwise false
        */
        bool AddAttribute(XMLNode* pNode, const std::string& name, int value) const;

        /**
        * Adds an attribute to a node (i.e. in the form name="value")
        *@param pNode - node for which attribute should be added
        *@param name - attribute name
        *@param value - attribute value
        *@returns true on success, otherwise false
        */
        bool AddAttribute(XMLNode* pNode, const std::string& name, float value) const;
};

#endif
