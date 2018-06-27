/****************************************************************************
 * ==> CSR_LevelManager ----------------------------------------------------*
 ****************************************************************************
 * Description : This module provides a level manager                       *
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

#ifndef CSR_LevelManagerH
#define CSR_LevelManagerH

// std
#include <vector>
#include <map>

// compactStar engine
#include "CSR_Geometry.h"

/**
* Level manager
*@author Jean-Milost Reymond
*/
class CSR_LevelManager
{
    public:
        /**
        * Geometrical shapes available in a scene
        */
        enum IEShapeType
        {
            IE_S_Unknown = 0,
            IE_S_Surface,
            IE_S_Box,
            IE_S_Sphere,
            IE_S_Cylinder,
            IE_S_Disk,
            IE_S_Ring,
            IE_S_Spiral,
            IE_S_Landscape,
            IE_S_WaveFront,
            IE_S_MDL
        };

        typedef std::vector<CSR_Matrix4*> IMatrices;

        /**
        * Source files used to build the scene models
        */
        struct IFiles
        {
            std::string m_Texture;
            std::string m_BumpMap;
            std::string m_LandscapeMap;
            std::string m_Model;

            IFiles();
            virtual ~IFiles();
        };

        /**
        * Resources required to build a shape or a model
        */
        struct IResource
        {
            IEShapeType m_ShapeType;
            IFiles      m_Files;
            int         m_Faces;
            int         m_Slices;
            int         m_Stacks;
            int         m_Radius;
            int         m_DeltaMin;
            int         m_DeltaMax;
            int         m_DeltaZ;
            bool        m_RepeatTextureOnEachFace;

            IResource();
            virtual ~IResource();
        };

        /**
        * Level manager item, contains additional data linked to the scene item, and required by the
        * application, e.g. to load or design the level, but not required by the core engine
        */
        struct IItem
        {
            IMatrices m_Matrices;
            IResource m_Resource;

            IItem();
            virtual ~IItem();

            /**
            * Adds a new matrix in the item
            *@return newly added matrix item
            */
            virtual std::size_t AddMatrix();

            /**
            * Deletes an existing matrix
            *@param index - matrix index to delete
            */
            virtual void DeleteMatrix(std::size_t index);
        };

        /**
        * Skybox resources
        */
        struct ISkybox
        {
            std::string m_Left;
            std::string m_Top;
            std::string m_Right;
            std::string m_Bottom;
            std::string m_Front;
            std::string m_Back;

            ISkybox();
            virtual ~ISkybox();

            /**
            * Clears the data
            */
            virtual void Clear();
        };

        /**
        * Sound resources
        */
        struct ISound
        {
            std::string m_FileName;

            ISound();
            virtual ~ISound();

            /**
            * Clears the data
            */
            virtual void Clear();
        };

        ISkybox m_Skybox;
        ISound  m_Sound;

        CSR_LevelManager();
        virtual ~CSR_LevelManager();

        /**
        * Clears the level manager
        */
        virtual void Clear();

        /**
        * Adds a new item in the level manager, or a new matrix in an existing item
        *@param pKey - key for which the item should be added
        *@return the item containing the newly added matrix, NULL on error
        *@note The item will be added if still not exists, but the matching item will be returned if
        *      the key already exists. In all case the last item matrix is always the one added for
        *      the new item
        */
        virtual IItem* Add(void* pKey);

        /**
        * Deletes an item from the level manager, or a matrix from an existing item
        *@param pKey - key for which the item should be deleted
        *@param index - item matrix index to delete
        *@note The matrix will be deleted in the item at the provided index. In case the item no
        *      longer contains matrix, it will be deleted from the manager
        */
        virtual void Delete(void* pKey, std::size_t index);

        /**
        * Gets the item in the level manager matching with a key
        *@param pKey - key for which the item should be get
        *@return the item matching with the key, NULL if not found or on error
        */
        virtual IItem* Get(void* pKey) const;

    private:
        // IMPORTANT NOTE don't use a CSR_LevelItem as key for the map, because this pointer may
        // change during the document lifecycle, whereas the model pointer not
        typedef std::map<void*, IItem*> IItems;

        IItems m_Items;
};
#endif
