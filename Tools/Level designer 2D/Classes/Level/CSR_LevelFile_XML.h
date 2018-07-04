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

#ifndef CSR_LevelFile_XMLH
#define CSR_LevelFile_XMLH

// std
#include <map>

// compactStar engine
#include "CSR_Common.h"
#include "CSR_Scene.h"

// classes
#include "CSR_LevelFile.h"

// xml
#include "sxmlc.h"

/**
* Level serializer
*@author Jean-Milost Reymond
*/
class CSR_LevelFile_XML
{
    public:
        /**
        * Constructor
        *@param levelDir - path to the level dir
        *@param saveContent - if true, the file content will also be saved
        */
        CSR_LevelFile_XML(const std::string& levelDir, bool saveContent);

        virtual ~CSR_LevelFile_XML();

        /**
        * Loads the level
        *@param fileName - level file name
        *@param[in, out] level - level to load
        *@return true on success, otherwise false
        */
        virtual bool Load(const std::string& fileName, CSR_Level& level);

        /**
        * Saves the level
        *@param fileName - level file name
        *@param level - level to save
        *@return true on success, otherwise false
        */
        virtual bool Save(const std::string& fileName, const CSR_Level& level) const;

        /**
        * Gets a file buffer from cache
        *@param fileName - file name for which the buffer should be get
        *@return file buffer, NULL if not found or on error
        */
        virtual const CSR_Buffer* GetFile(const std::string& fileName);

        /**
        * Sets the OnLoadCubemap callback
        *@param fHandler - function handler
        */
        void Set_OnLoadCubemap(CSR_Level::ITfLoadCubemap fHandler);

        /**
        * Sets the OnLoadTexture callback
        *@param fHandler - function handler
        */
        void Set_OnLoadTexture(CSR_Level::ITfLoadTexture fHandler);

        /**
        * Sets the OnSelectModel callback
        *@param fHandler - function handler
        */
        void Set_OnSelectModel(CSR_Level::ITfSelectModel fHandler);

        /**
        * Sets the OnUpdateDesigner callback
        *@param fHandler - function handler
        */
        void Set_OnUpdateDesigner(CSR_Level::ITfOnUpdateDesigner fHandler);

    private:
        typedef std::map<std::string, CSR_Buffer*> IFiles;

        std::string                    m_LevelDir;
        IFiles                         m_Files;
        bool                           m_SaveContent;
        CSR_Level::ITfLoadCubemap      m_fOnLoadCubemap;
        CSR_Level::ITfLoadTexture      m_fOnLoadTexture;
        CSR_Level::ITfSelectModel      m_fOnSelectModel;
        CSR_Level::ITfOnUpdateDesigner m_fOnUpdateDesigner;

        /**
        * Clears the serializer
        */
        void Clear();

        /**
        * Converts a relative file name to absolute
        *@param fileName - relative file name to convert
        *@return absolute file name
        */
        std::string RelativeToAbsolute(const std::string& fileName) const;

        /**
        * Converts an absolute file name to relative
        *@param fileName - absolute file name to convert
        *@return relative file name
        */
        std::string AbsoluteToRelative(const std::string& fileName) const;

        /**
        * Reads a level
        *@param pNode - xml node containing the scene to read
        *@param[in, out] level - level to populate with the read values
        *@return true on success, otherwise false
        */
        bool Read(const XMLNode* pNode, CSR_Level& level);

        /**
        * Reads a scene
        *@param pNode - xml node containing the scene to read
        *@param[in, out] level - level in which the scene should be added
        *@return true on success, otherwise false
        */
        bool ReadScene(const XMLNode* pNode, CSR_Level& level);

        /**
        * Reads a scene item
        *@param pNode - xml node containing the scene item to read
        *@param[in, out] level - level in which the scene item should be added
        *@return true on success, otherwise false
        */
        bool ReadSceneItem(const XMLNode* pNode, CSR_Level& level);

        /**
        * Reads a skybox from an xml node
        *@param pNode - xml node containing the skybox to read
        *@param[in, out] level - level for which the skybox should be load
        *@return true on success, otherwise false
        */
        bool ReadSkybox(const XMLNode* pNode, CSR_Level& level);

        /**
        * Reads a sound from an xml node
        *@param pNode - xml node containing the sound to read
        *@param[in, out] level - level for which the sound should be load
        *@return true on success, otherwise false
        */
        bool ReadSound(const XMLNode* pNode, CSR_Level& level);

        /**
        * Reads a file from an xml node
        *@param pNode - xml node containing the file name to read
        *@param[out] fileName - file name to populate with the value
        *@return true on success, otherwise false
        */
        bool ReadFile(const XMLNode* pNode, std::string& fileName);

        /**
        * Reads the content of a matrix from an xml node
        *@param pNode - xml node containing the matrix to read
        *@param pMat4 - matrix to populate with the values
        *@return true on success, otherwise false
        */
        bool ReadMatrix(const XMLNode* pNode, CSR_Matrix4* pMat4);

        /**
        * Reads the content of a vector from an xml node
        *@param pNode - xml node containing the vector to read
        *@param pVec3 - vector to populate with the values
        *@return true on success, otherwise false
        */
        bool ReadVector(const XMLNode* pNode, CSR_Vector3* pVec3);

        /**
        * Reads the content of a color from an xml node
        *@param pNode - xml node containing the color to read
        *@param pColor - color to populate with the values
        *@return true on success, otherwise false
        */
        bool ReadColor(const XMLNode* pNode, CSR_Color* pColor);

        /**
        * Writes the level
        *@param doc - xml document in which the level should be written
        *@param level - level to save
        *@return true on success, otherwise false
        */
        bool Write(XMLDoc& doc, const CSR_Level& level) const;

        /**
        * Writes the level
        *@param doc - xml document in which the level should be written
        *@param level - level to save
        *@return true on success, otherwise false
        */
        bool WriteLevel(XMLDoc& doc, const CSR_Level& level) const;

        /**
        * Writes the scene belonging to the level
        *@param pNode - xml node in which the scene should be written
        *@param level - level to save
        *@return true on success, otherwise false
        */
        bool WriteScene(XMLNode* pNode, const CSR_Level& level) const;

        /**
        * Writes a scene item
        *@param pNode - xml node in which the scene item should be written
        *@param pSceneItem - scene item to save
        *@param pLevelItem - level item to save
        *@return true on success, otherwise false
        */
        bool WriteSceneItem(      XMLNode*          pNode,
                            const CSR_SceneItem*    pSceneItem,
                            const CSR_Level::IItem* pLevelItem) const;

        /**
        * Writes the skybox
        *@param pNode - xml node in which the skybox should be written
        *@param level - level containing the skybox to save
        *@return true on success, otherwise false
        */
        bool WriteSkybox(XMLNode* pNode, const CSR_Level& level) const;

        /**
        * Writes the sound
        *@param pNode - xml node in which the sound should be written
        *@param level - level containing the sound to save
        *@return true on success, otherwise false
        */
        bool WriteSound(XMLNode* pNode, const CSR_Level& level) const;

        /**
        * Writes a file
        *@param pNode - xml node in which the file should be written
        *@param name - tag name
        *@param fileName - file name to write
        *@return true on success, otherwise false
        */
        bool WriteFile(XMLNode* pNode, const std::string& name, const std::string& fileName) const;

        /**
        * Writes a matrix
        *@param pNode - xml node in which the matrix should be written
        *@param name - tag name
        *@param pMat4 - matrix to write
        *@return true on success, otherwise false
        */
        bool WriteMatrix(XMLNode* pNode, const std::string& name, const CSR_Matrix4* pMat4) const;

        /**
        * Writes a vector
        *@param pNode - xml node in which the vector should be written
        *@param name - tag name
        *@param pVec3 - vector to write
        *@return true on success, otherwise false
        */
        bool WriteVector(XMLNode* pNode, const std::string& name, const CSR_Vector3* pVec3) const;

        /**
        * Writes a color
        *@param pNode - xml node in which the color should be written
        *@param name - tag name
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
