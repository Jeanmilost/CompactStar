/*****************************************************************************
 * ==> CSR_MessageHelper ----------------------------------------------------*
 *****************************************************************************
 * Description : This module provides a helper class for the messages owned  *
 *               by the application                                          *
 * Developer   : Jean-Milost Reymond                                         *
 * Copyright   : 2015 - 2018, this file is part of the Minimal API. You are  *
 *               free to copy or redistribute this file, modify it, or use   *
 *               it for your own projects, commercial or not. This file is   *
 *               provided "as is", without ANY WARRANTY OF ANY KIND          *
 *****************************************************************************/

#include "CSR_MessageHelper.h"

//---------------------------------------------------------------------------
// Global variables
//---------------------------------------------------------------------------
std::auto_ptr<CSR_MessageHelper::IInstance> CSR_MessageHelper::m_pMessageHelper;
//---------------------------------------------------------------------------
// CSR_MessageHelper::IInstance
//---------------------------------------------------------------------------
CSR_MessageHelper::IInstance::IInstance() :
    m_pInstance(NULL)
{
    m_pInstance = new CSR_MessageHelper();
}
//---------------------------------------------------------------------------
CSR_MessageHelper::IInstance::~IInstance()
{
    if (m_pInstance)
        delete m_pInstance;
}
//---------------------------------------------------------------------------
// CSR_MessageHelper
//---------------------------------------------------------------------------
CSR_MessageHelper::CSR_MessageHelper()
{}
//---------------------------------------------------------------------------
CSR_MessageHelper::CSR_MessageHelper(const CSR_MessageHelper& other)
{
    throw std::runtime_error("Copy constructor of this singleton is not allowed");
}
//---------------------------------------------------------------------------
CSR_MessageHelper& CSR_MessageHelper::operator = (const CSR_MessageHelper& other)
{
    throw std::runtime_error("Copy operator of this singleton is not allowed");
}
//---------------------------------------------------------------------------
CSR_MessageHelper::~CSR_MessageHelper()
{}
//---------------------------------------------------------------------------
CSR_MessageHelper* CSR_MessageHelper::Get()
{
    // already created?
    if (!m_pMessageHelper.get())
        // create new and unique instance
        m_pMessageHelper.reset(new IInstance());

    return m_pMessageHelper->m_pInstance;
}
//---------------------------------------------------------------------------
void CSR_MessageHelper::Delete()
{
    // already created?
    if (!m_pMessageHelper.get())
        return;

    m_pMessageHelper.reset(NULL);
}
//---------------------------------------------------------------------------
std::wstring CSR_MessageHelper::GetTexturesDir() const
{
    return L"Textures";
}
//---------------------------------------------------------------------------
std::wstring CSR_MessageHelper::GetModelsDir() const
{
    return L"Models";
}
//---------------------------------------------------------------------------
std::wstring CSR_MessageHelper::GetError_CopyTexture() const
{
    return L"Failed to copy the texture in the scene directory.\r\n\r\nPlease verify if another texture with the same name was not previously copied in this dir:\r\n";
}
//---------------------------------------------------------------------------
std::wstring CSR_MessageHelper::GetError_OpeningModel() const
{
    return L"An error occurred while the model was opened.\n\nPlease check your parameters and try again.";
}
//---------------------------------------------------------------------------
std::wstring CSR_MessageHelper::GetError_LoadingModel() const
{
    return L"The model you're trying to load is invalid.\n\nPlease check the model file and try again.";
}
//---------------------------------------------------------------------------
