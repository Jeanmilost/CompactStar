/****************************************************************************
 * ==> CSR_Character_Balloon -----------------------------------------------*
 ****************************************************************************
 * Description : Balloon character for a game                               *
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

#ifndef CSR_Character_BalloonH
#define CSR_Character_BalloonH

// std
#include <vector>
#include <string>

// compactStar engine
#include "CSR_Model.h"
#include "CSR_Physics.h"

// classes
#include "CSR_Character.h"

/**
* Balloon character
*@author Jean-Milost Reymond
*/
class CSR_Character_Balloon : public CSR_Character
{
    public:
        /**
        * Constructor
        *@param pScene - scene to which the character will be added
        *@param color - RGBA color
        *@param hasNormal - if true, the model will contain normals
        *@param hasTexCoords - if true, the model will contain texture coordinates
        *@param collisions - if true, the collision data will be generated
        *@param fOnDeleteTexture - callback to call when a texture should be deleted
        */
        CSR_Character_Balloon(CSR_Scene*           pScene,
                              unsigned             color,
                              bool                 hasNormal,
                              bool                 hasTexCoords,
                              bool                 collisions,
                              CSR_fOnDeleteTexture fOnDeleteTexture);

        virtual ~CSR_Character_Balloon();

        virtual void Swing(float elapsedTime);

    protected:
        /**
        * Builds the character
        *@return true on success, otherwise false
        */
        virtual bool Build();

    private:
        typedef std::vector<CSR_Vector3> IAnchors;
        typedef std::vector<CSR_Line*>   ISegments;

        unsigned  m_Color;
        bool      m_HasNormal;
        bool      m_HasTexCoords;
        bool      m_Collisions;
        IAnchors  m_Anchors;
        ISegments m_Segments;
        CSR_Body  m_Body;

        /**
        * Gets a model from the WaveFront source object
        *@param hasNormal - if true, the model will contain normals
        *@param hasTexCoords - if true, the model will contain texture coordinates
        *@param color - RGBA color
        *@return model, NULL on error
        *@note The model should be deleted when useless
        */
        CSR_Model* BuildModel(bool hasNormal, bool hasTexCoords, unsigned color) const;

        /**
        * Gets the WaveFront source object
        *@return the WaveFront source object
        */
        std::string GetWaveFront() const;
};

#endif
