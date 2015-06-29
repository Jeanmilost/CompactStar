/******************************************************************************
 * ==> QR_MD2Animation -------------------------------------------------------*
 ******************************************************************************
 * Description : Animation class for MD2 models                               *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/


#ifndef QR_MD2AnimationH
#define QR_MD2AnimationH

// std
#include <string>
#include <vector>
#include <map>

// qr engine
#include "QR_Types.h"
#include "QR_Vertex.h"
#include "QR_MD2Common.h"
#include "QR_MD2.h"

/**
* Animation class for MD2 models
*@author Jean-Milost Reymond
*/
class QR_MD2Animation
{
    public:
        /**
        * Animation data structure
        */
        struct IAnimation
        {
            QR_SizeT  m_FrameIndex;
            QR_SizeT  m_InterpolationFrameIndex;
            QR_Double m_InterpolationFactor;

            IAnimation();

            /**
            * Constructor
            *@param frameIndex - frame index to draw
            *@param interpolationFrameIndex - frame index to use for interpolation
            *@param interpolationFactor - interpolation factor in percent (between 0.0f and 1.0f)
            */
            IAnimation(QR_SizeT frameIndex, QR_SizeT interpolationFrameIndex,
                    const QR_Double& interpolationFactor);

            virtual ~IAnimation();
        };

        /**
        * Constructor
        *@param pModel - MD2 model to animate
        *@param useCache - if true, cache usage is enabled (should be false if GetVertices() isn't used)
        */
        QR_MD2Animation(QR_MD2* pModel, bool useCache = false);

        virtual ~QR_MD2Animation();

        /**
        * Adds animation
        *@param name - animation name
        *@param start - frame index at which animation starts in the MD2 model
        *@param end - frame index at which animation ends in the MD2 model
        *@return true on success, otherwise false
        */
        virtual bool Add(const std::string& name, QR_SizeT start, QR_SizeT end);

        /**
        * Deletes animation
        *@param name - animation name to delete
        */
        virtual void Delete(const std::string& name);

        /**
        * Sets animation to use
        *@param name - animation name
        *@return true on success, otherwise false
        */
        virtual bool Set(const std::string& name);

        /**
        * Gets animation
        *@param elapsedTime - elapsed time since last frame was drawn
        *@param fps - number of frames per seconds
        *@return animation
        */
        virtual IAnimation GetAnimation(const QR_Double& elapsedTime, QR_SizeT fps);

        /**
        * Gets vertices
        *@param elapsedTime - elapsed time since last frame was drawn
        *@param fps - number of frames per seconds
        *@param[out] vertices - model verices to draw
        *@return true on success, otherwise false
        *@note For performances reasons, this function should not be used since OpenGL 2.0. Instead,
        *      vertices should be generated once from model and then passed to vertex shader, and
        *      interpolation should be done inside the vertex shader
        */
        virtual bool GetVertices(const QR_Double& elapsedTime, QR_SizeT fps, QR_Vertices& vertices);

    private:
        /**
        * Animation info structure
        */
        struct IInfo
        {
            QR_SizeT m_Start;
            QR_SizeT m_End;

            IInfo();

            /**
            * Constructor
            *@param start - start frame index
            *@param end - end frame index
            */
            IInfo(QR_SizeT start, QR_SizeT end);

            virtual ~IInfo();
        };

        typedef std::map<std::string, IInfo*> IInfoDictionary;
        typedef std::vector<QR_Vertices>      ICache;

        QR_MD2*         m_pModel;
        IInfo*          m_pInfo;
        IInfoDictionary m_InfoDictionary;
        ICache          m_Cache;
        QR_SizeT        m_FrameIndex;
        QR_Double       m_InterpolationFactor;
};

#endif // QR_MD2AnimationH
