/******************************************************************************
 * ==> QR_MD2Animation ----------------------------------------------------------*
 ******************************************************************************
 * Description : Helper class for MD2 model                                   *
 * Developer   : Jean-Milost Reymond                                          *
 ******************************************************************************/


#include "QR_MD2Animation.h"

// qr engine
#include "QR_Exception.h"
#include "QR_STDTools.h"

//------------------------------------------------------------------------------
// QR_MD2Animation::IAnimation - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2Animation::IAnimation::IAnimation() :
    m_FrameIndex(M_MD2_Invalid_Index),
    m_InterpolationFrameIndex(M_MD2_Invalid_Index),
    m_InterpolationFactor(0.0f)
{}
//------------------------------------------------------------------------------
QR_MD2Animation::IAnimation::IAnimation(QR_SizeT frameIndex, QR_SizeT interpolationFrameIndex,
        const QR_Double& interpolationFactor) :
    m_FrameIndex(frameIndex),
    m_InterpolationFrameIndex(interpolationFrameIndex),
    m_InterpolationFactor(interpolationFactor)
{}
//------------------------------------------------------------------------------
QR_MD2Animation::IAnimation::~IAnimation()
{}
//------------------------------------------------------------------------------
// QR_MD2Animation::IInfo - c++ cross-platform
//------------------------------------------------------------------------------
QR_MD2Animation::IInfo::IInfo() : m_Start(0), m_End(0)
{}
//------------------------------------------------------------------------------
QR_MD2Animation::IInfo::IInfo(QR_SizeT start, QR_SizeT end) : m_Start(start), m_End(end)
{}
//------------------------------------------------------------------------------
QR_MD2Animation::IInfo::~IInfo()
{}
//---------------------------------------------------------------------------
// QR_MD2Animation - c++ cross-platform
//---------------------------------------------------------------------------
QR_MD2Animation::QR_MD2Animation(QR_MD2* pModel, bool useCache) :
    m_pModel(pModel),
    m_pInfo(NULL),
    m_FrameIndex(0),
    m_InterpolationFactor(0.0f)
{
    // cannot create an animation without model
    M_ASSERT(m_pModel);

    // can use cache?
    if (useCache)
        // initialize cache
        m_Cache.resize(m_pModel->GetVerticesCount());
}
//---------------------------------------------------------------------------
QR_MD2Animation::~QR_MD2Animation()
{
    QR_STDTools::DelAndClear(m_InfoDictionary);

    // get cache count
    const QR_SizeT cacheCount = m_Cache.size();

    // iterate through cache items
    for (QR_SizeT i = 0; i < cacheCount; ++i)
        // delete cache item
        QR_STDTools::DelAndClear(m_Cache[i]);

    m_Cache.clear();
}
//------------------------------------------------------------------------------
bool QR_MD2Animation::Add(const std::string& name, QR_SizeT start, QR_SizeT end)
{
    // no name?
    if (name.empty())
        return false;

    // no linked MD2 model?
    if (!m_pModel)
        return false;

    // get linked model vertices count
    const QR_SizeT verticesCount = m_pModel->GetVerticesCount();

    // are start or end values out of bounds?
    if (start >= verticesCount || end >= verticesCount)
        return false;

    // search for already existing animation
    IInfoDictionary::iterator it = m_InfoDictionary.find(name);

    // animation already exists?
    if (it != m_InfoDictionary.end())
    {
        // update animation info
        it->second->m_Start = start;
        it->second->m_End   = end;
        return true;
    }

    // create and populate new animation info
    std::auto_ptr<IInfo> pInfo(new IInfo(start, end));

    // add animation info to dictionary
    m_InfoDictionary[name] = pInfo.get();
    pInfo.release();

    return true;
}
//------------------------------------------------------------------------------
void QR_MD2Animation::Delete(const std::string& name)
{
    // no name?
    if (name.empty())
        return;

    // search for animation to delete
    IInfoDictionary::iterator it = m_InfoDictionary.find(name);

    // found it?
    if (it == m_InfoDictionary.end())
        return;

    // deleting current animation?
    if (m_pInfo == it->second)
    {
        // reset animation
        m_pInfo               = NULL;
        m_FrameIndex          = 0;
        m_InterpolationFactor = 0.0f;
    }

    // delete animation
    delete it->second;
    m_InfoDictionary.erase(it);
}
//------------------------------------------------------------------------------
bool QR_MD2Animation::Set(const std::string& name)
{
    // no name?
    if (name.empty())
        return false;

    // search for animation to set
    IInfoDictionary::iterator it = m_InfoDictionary.find(name);

    // found it?
    if (it == m_InfoDictionary.end())
        return false;

    // set animation
    m_pInfo               = it->second;
    m_FrameIndex          = m_pInfo->m_Start;
    m_InterpolationFactor = 0.0f;
    return true;
}
//------------------------------------------------------------------------------
QR_MD2Animation::IAnimation QR_MD2Animation::GetAnimation(const QR_Double& elapsedTime, QR_SizeT fps)
{
    // no animation info?
    if (!m_pInfo)
        return IAnimation();

    // calculate time interval between each frames
    const QR_Double timeInterval = (1000.0f / (QR_Double)fps);

    // calculate next interpolation position
    m_InterpolationFactor += (elapsedTime / timeInterval);

    // max interpolation factor reached?
    if (m_InterpolationFactor >= 1.0f)
    {
        // calculate number of frames to skip
        const QR_SizeT framesToSkip = std::floor(m_InterpolationFactor);

        // update interpolation factor (should always be between 0 and 1)
        m_InterpolationFactor -= framesToSkip;

        // skip frames
        m_FrameIndex += framesToSkip;
    }

    // calculate animation length
    const QR_SizeT animationLength = (m_pInfo->m_End - m_pInfo->m_Start) + 1;

    // current frame end reached?
    if (m_FrameIndex > m_pInfo->m_End)
        m_FrameIndex = m_pInfo->m_Start + ((m_FrameIndex - m_pInfo->m_Start) % animationLength);

    // calculate interpolation frame index, normally frame index + 1, except if hit animation end
    QR_SizeT interpolationFrameIndex = m_FrameIndex + 1;

    // next frame end reached?
    if (interpolationFrameIndex > m_pInfo->m_End)
        interpolationFrameIndex = m_pInfo->m_Start + ((interpolationFrameIndex - m_pInfo->m_Start)
                % animationLength);

    // return animation data
    return IAnimation(m_FrameIndex, interpolationFrameIndex, m_InterpolationFactor);
}
//---------------------------------------------------------------------------
bool QR_MD2Animation::GetVertices(const QR_Double& elapsedTime, QR_SizeT fps, QR_Vertices& vertices)
{
    // no model?
    if (!m_pModel)
        return false;

    // get animation
    IAnimation animation = GetAnimation(elapsedTime, fps);

    // found it?
    if (animation.m_FrameIndex == M_MD2_Invalid_Index)
        return false;

    // can use cache?
    if (!m_Cache.size())
    {
        QR_Vertices frameVertices;
        QR_Vertices interpolationVertices;
        bool        success = false;

        try
        {
            // get frame vertices and vertices to interpolate with
            m_pModel->GetVertices(animation.m_FrameIndex, frameVertices);
            m_pModel->GetVertices(animation.m_InterpolationFrameIndex, interpolationVertices);

            // process interpolation and send resulting vertices to output
            success = QR_MD2::Interpolate(animation.m_InterpolationFactor,
                                          frameVertices,
                                          interpolationVertices,
                                          vertices);
        }
        catch (...)
        {}

        // clear memory
        QR_STDTools::DelAndClear(frameVertices);
        QR_STDTools::DelAndClear(interpolationVertices);

        return success;
    }

    // get and cache frame vertices
    if (!m_Cache[animation.m_FrameIndex].size())
        m_pModel->GetVertices(animation.m_FrameIndex, m_Cache[animation.m_FrameIndex]);

    // get and cache vertices to interpolate with
    if (!m_Cache[animation.m_InterpolationFrameIndex].size())
        m_pModel->GetVertices(animation.m_InterpolationFrameIndex,
                              m_Cache[animation.m_InterpolationFrameIndex]);

    // process interpolation and send resulting vertices to output
    return QR_MD2::Interpolate(animation.m_InterpolationFactor,
                               m_Cache[animation.m_FrameIndex],
                               m_Cache[animation.m_InterpolationFrameIndex],
                               vertices);
}
//---------------------------------------------------------------------------

