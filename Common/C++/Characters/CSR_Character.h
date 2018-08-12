/****************************************************************************
 * ==> CSR_Character -------------------------------------------------------*
 ****************************************************************************
 * Description : Basic character class for games                            *
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

#ifndef CSR_CharacterH
#define CSR_CharacterH

// compactStar engine
#include "CSR_Geometry.h"
#include "CSR_Renderer_OpenGL.h"
#include "CSR_Scene.h"

/**
* Basic character class for games
*@author Jean-Milost Reymond
*/
class CSR_Character
{
    public:
        /**
        * Constructor
        *@param pScene - scene to which the character will be added
        *@param fOnDeleteTexture - callback to call when a texture should be deleted
        */
        CSR_Character(CSR_Scene* pScene, CSR_fOnDeleteTexture fOnDeleteTexture);

        virtual ~CSR_Character();

        /**
        * Builds the character
        *@return true on success, otherwise false
        */
        virtual bool Build() = 0;

        /**
        * Gets the scene
        *@return the scene
        */
        virtual CSR_Scene* GetScene() const;

        /**
        * Sets the current scene
        *@param pScene - scene to set, if NULL will be considered as no scene
        */
        virtual void SetScene(CSR_Scene* pScene);

        /**
        * Gets the model key
        *@return the model key
        */
        virtual void* GetKey() const;

        /**
        * Gets the texture key
        *@return the texture key
        */
        virtual void* GetTextureKey() const;

        /**
        * Situates the character in the 3d world
        *@param pos - position
        *@param dir - direction
        *@param scale - scale factor
        */
        virtual void Situate(const CSR_Vector3& pos, const CSR_Vector3& dir, const CSR_Vector3& scale);

    protected:
        CSR_Scene*           m_pScene;
        CSR_Matrix4          m_Matrix;
        void*                m_pKey;
        void*                m_pTextureKey;
        CSR_fOnDeleteTexture m_fOnDeleteTexture;
};

#endif
