/****************************************************************************
 * ==> CSR_Level -----------------------------------------------------------*
 ****************************************************************************
 * Description : This module represents a game level. It is the keeper of   *
 *               his important data, and manages his resources              *
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

#include "CSR_Level.h"

// classes
#include "CSR_ShaderHelper.h"

#pragma link "OpenAL32E.lib"

//---------------------------------------------------------------------------
// CSR_Level::IItemFiles
//---------------------------------------------------------------------------
CSR_Level::IItemFiles::IItemFiles()
{}
//---------------------------------------------------------------------------
CSR_Level::IItemFiles::~IItemFiles()
{}
//---------------------------------------------------------------------------
// CSR_Level::IItemResources
//---------------------------------------------------------------------------
CSR_Level::IItemResources::IItemResources() :
    m_Faces(0),
    m_Slices(0),
    m_Stacks(0),
    m_Radius(0),
    m_DeltaMin(0),
    m_DeltaMax(0),
    m_DeltaZ(0),
    m_RepeatTextureOnEachFace(false)
{}
//---------------------------------------------------------------------------
CSR_Level::IItemResources::~IItemResources()
{}
//---------------------------------------------------------------------------
// CSR_Level::IItem
//---------------------------------------------------------------------------
CSR_Level::IItem::IItem() :
    m_Type(IE_IT_Unknown)
{}
//---------------------------------------------------------------------------
CSR_Level::IItem::~IItem()
{
    // delete the matrix list
    for (IMatrices::iterator it = m_Matrices.begin(); it != m_Matrices.end(); ++it)
        delete (*it);
}
//---------------------------------------------------------------------------
std::size_t CSR_Level::IItem::AddMatrix()
{
    std::auto_ptr<CSR_Matrix4> pMatrix(new CSR_Matrix4());
    csrMat4Identity(pMatrix.get());
    m_Matrices.push_back(pMatrix.get());
    pMatrix.release();
    return m_Matrices.size() - 1;
}
//---------------------------------------------------------------------------
void CSR_Level::IItem::DeleteMatrix(std::size_t index)
{
    // is index out of bounds?
    if (index >= m_Matrices.size())
        return;

    // delete the matrix
    delete m_Matrices[index];
    m_Matrices.erase(m_Matrices.begin() + index);
}
//---------------------------------------------------------------------------
// CSR_Level::ISkybox
//---------------------------------------------------------------------------
CSR_Level::ISkybox::ISkybox()
{}
//---------------------------------------------------------------------------
CSR_Level::ISkybox::~ISkybox()
{}
//---------------------------------------------------------------------------
void CSR_Level::ISkybox::Clear()
{
    m_Left.clear();
    m_Top.clear();
    m_Right.clear();
    m_Bottom.clear();
    m_Front.clear();
    m_Back.clear();
}
//---------------------------------------------------------------------------
// CSR_Level::ISound
//---------------------------------------------------------------------------
CSR_Level::ISound::ISound()
{}
//---------------------------------------------------------------------------
CSR_Level::ISound::~ISound()
{}
//---------------------------------------------------------------------------
void CSR_Level::ISound::Clear()
{
    // clear the sound file name
    m_FileName.clear();
}
//---------------------------------------------------------------------------
// CSR_Level
//---------------------------------------------------------------------------
CSR_Level::CSR_Level() :
    m_pOpenALDevice(NULL),
    m_pOpenALContext(NULL),
    m_pScene(NULL),
    m_pShader(NULL),
    m_pSkyboxShader(NULL),
    m_pEffect(NULL),
    m_pMSAA(NULL),
    m_pSound(NULL),
    m_pLandscapeKey(NULL),
    m_UseMSAA(false),
    m_UseOilPainting(true)
{
    // initialize OpenAL
    csrSoundInitializeOpenAL(&m_pOpenALDevice, &m_pOpenALContext);

    // initialize the scene context
    csrSceneContextInit(&m_SceneContext);
}
//---------------------------------------------------------------------------
CSR_Level::~CSR_Level()
{
    // NOTE the function domain name is required here to avoid calling a pure virtual function
    CSR_Level::Clear();

    // release the scene
    ClearScene();

    // release the shaders
    csrOpenGLShaderRelease(m_pSkyboxShader);
    csrOpenGLShaderRelease(m_pShader);

    // release the oil painting post processing effect
    if (m_pEffect)
        delete m_pEffect;

    // release the multisampling antialiasing
    csrOpenGLMSAARelease(m_pMSAA);

    // clear the sound
    ClearSound();

    // release OpenAL interface
    csrSoundReleaseOpenAL(m_pOpenALDevice, m_pOpenALContext);
}
//---------------------------------------------------------------------------
void CSR_Level::Clear()
{
    // iterate through the level manager items
    for (IItems::iterator it = m_Items.begin(); it != m_Items.end(); ++it)
        // delete each of them
        delete it->second;

    // clear the level manager items
    m_Items.clear();

    // clear the scene
    ClearScene();

    // clear the sound
    ClearSound();

    // clear the resources
    m_Skybox.Clear();
    m_Sound.Clear();
}
//------------------------------------------------------------------------------
bool CSR_Level::BuildSceneShader()
{
    const std::string vsTextured = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Texture);
    const std::string fsTextured = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Texture);

    // load the shader
    m_pShader = csrOpenGLShaderLoadFromStr(vsTextured.c_str(),
                                           vsTextured.length(),
                                           fsTextured.c_str(),
                                           fsTextured.length(),
                                           0,
                                           0);

    // succeeded?
    if (!m_pShader)
        return false;

    // enable the shader program
    csrShaderEnable(m_pShader);

    // get shader attributes
    m_pShader->m_VertexSlot   = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aVertices");
    m_pShader->m_ColorSlot    = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aColor");
    m_pShader->m_TexCoordSlot = glGetAttribLocation (m_pShader->m_ProgramID, "csr_aTexCoord");
    m_pShader->m_TextureSlot  = glGetUniformLocation(m_pShader->m_ProgramID, "csr_sTexture");

    return true;
}
//------------------------------------------------------------------------------
void CSR_Level::CreateViewport(float w, float h)
{
    if (!m_pShader)
        return;

    // configure the OpenGL viewport
    CSR_OpenGLHelper::CreateViewport(w,
                                     h,
                                     0.001f,
                                     1000.0f,
                                     m_pShader,
                                     m_pScene->m_ProjectionMatrix);

    // multisampling antialiasing was already created?
    if (!m_pMSAA)
        // create the multisampling antialiasing
        m_pMSAA = csrOpenGLMSAACreate(w, h, 4);
    else
        // change his size
        csrOpenGLMSAAChangeSize(w, h, m_pMSAA);

    // oil painting post processing effect was already created?
    if (!m_pEffect)
        // create the oil painting post processing effect
        m_pEffect = new CSR_PostProcessingEffect_OilPainting(w, h, 4);
    else
        // change the effect viewport size
        m_pEffect->ChangeSize(w, h);
}
//---------------------------------------------------------------------------
bool CSR_Level::AddSkybox(const IFileNames& fileNames, ITfLoadCubemap fLoadCubemap)
{
    // not enough or too many file names to load
    if (fileNames.size() != 6)
        return false;

    // no load cubemap texture callback?
    if (!fLoadCubemap)
        return false;

    // is skybox shader still not loaded?
    if (!m_pSkyboxShader)
    {
        const std::string vsTextured = CSR_ShaderHelper::GetVertexShader(CSR_ShaderHelper::IE_ST_Skybox);
        const std::string fsTextured = CSR_ShaderHelper::GetFragmentShader(CSR_ShaderHelper::IE_ST_Skybox);

        // load the shader
        m_pSkyboxShader  = csrOpenGLShaderLoadFromStr(vsTextured.c_str(),
                                                      vsTextured.length(),
                                                      fsTextured.c_str(),
                                                      fsTextured.length(),
                                                      0,
                                                      0);

        // succeeded?
        if (!m_pSkyboxShader)
            return false;

        // enable the shader program
        csrShaderEnable(m_pSkyboxShader);

        // get shader attributes
        m_pSkyboxShader->m_VertexSlot  = glGetAttribLocation (m_pSkyboxShader->m_ProgramID, "csr_aVertices");
        m_pSkyboxShader->m_CubemapSlot = glGetUniformLocation(m_pSkyboxShader->m_ProgramID, "csr_sCubemap");
    }
    else
        // enable the shader program
        csrShaderEnable(m_pSkyboxShader);

    // do delete the previous skybox?
    if (m_pScene->m_pSkybox)
        csrMeshRelease(m_pScene->m_pSkybox, m_SceneContext.m_fOnDeleteTexture);

    // create the skybox
    m_pScene->m_pSkybox = csrSkyboxCreate(1.0f, 1.0f, 1.0f);

    // load the cubemap texture
    CSR_OpenGLHelper::AddTexture(&m_pScene->m_pSkybox->m_Skin.m_CubeMap,
                                  fLoadCubemap(fileNames),
                                  m_OpenGLResources);

    const std::size_t fileNameCount = fileNames.size();

    // iterate through the cubemap texture files and keep each of them in the level manager resources
    for (std::size_t i = 0; i < fileNameCount; i++)
        switch (i)
        {
            case 0: m_Skybox.m_Right  = fileNames[i]; continue;
            case 1: m_Skybox.m_Left   = fileNames[i]; continue;
            case 2: m_Skybox.m_Top    = fileNames[i]; continue;
            case 3: m_Skybox.m_Bottom = fileNames[i]; continue;
            case 4: m_Skybox.m_Front  = fileNames[i]; continue;
            case 5: m_Skybox.m_Back   = fileNames[i]; continue;
        }

    return true;
}
//---------------------------------------------------------------------------
void* CSR_Level::AddBox(const CSR_Matrix4&       matrix,
                        const std::string&       textureName,
                              bool               repeatTexOnEachFace,
                              CSR_ECollisionType collisionType,
                              ITfLoadTexture     fLoadTexture,
                              ITfSelectModel     fSelectModel)
{
    // no load texture callback?
    if (!fLoadTexture)
        return NULL;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default box mesh
    CSR_Mesh* pBox = csrShapeCreateBox(1.0f,
                                       1.0f,
                                       1.0f,
                                       repeatTexOnEachFace ? 1 : 0,
                                      &vf,
                                       0,
                                      &material,
                                       0);

    // succeeded?
    if (!pBox)
        return NULL;

    try
    {
        // load the texture
        const GLuint textureID = fLoadTexture(textureName);

        // failed?
        if (textureID == M_CSR_Error_Code)
        {
            csrMeshRelease(pBox, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // add texture to OpenGL resources
        CSR_OpenGLHelper::AddTexture(&pBox->m_Skin.m_Texture, textureID, m_OpenGLResources);

        // add a new item to the manager
        CSR_Level::IItem* pItem = Add(pBox);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            csrMeshRelease(pBox, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // keep the model resources
        pItem->m_Type                                = CSR_Level::IE_IT_Box;
        pItem->m_Resources.m_Files.m_Texture         = textureName;
        pItem->m_Resources.m_RepeatTextureOnEachFace = repeatTexOnEachFace;

        // set the model matrix
        *pItem->m_Matrices[pItem->m_Matrices.size() - 1] = matrix;

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        CSR_SceneItem* pSceneItem = csrSceneAddMesh(m_pScene, pBox, 0, collisionType != CSR_CO_None);
        csrSceneAddModelMatrix(m_pScene, pBox, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // succeeded?
        if (!pSceneItem)
        {
            csrMeshRelease(pBox, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // set the collision type
        pSceneItem->m_CollisionType = collisionType;

        // notify the designer about the new selection
        if (fSelectModel)
            fSelectModel(pBox, 0);
    }
    catch (...)
    {
        csrMeshRelease(pBox, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    return pBox;
}
//---------------------------------------------------------------------------
void* CSR_Level::AddCylinder(const CSR_Matrix4&       matrix,
                             const std::string&       textureName,
                                   int                faces,
                                   CSR_ECollisionType collisionType,
                                   ITfLoadTexture     fLoadTexture,
                                   ITfSelectModel     fSelectModel)
{
    // no load texture callback?
    if (!fLoadTexture)
        return NULL;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default cylinder mesh
    CSR_Mesh* pCylinder = csrShapeCreateCylinder(0.5f, 1.0f, faces, &vf, 0, &material, 0);

    // succeeded?
    if (!pCylinder)
        return NULL;

    try
    {
        // load the texture
        const GLuint textureID = fLoadTexture(textureName);

        // failed?
        if (textureID == M_CSR_Error_Code)
        {
            csrMeshRelease(pCylinder, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // add the texture to the OpenGL resources
        CSR_OpenGLHelper::AddTexture(&pCylinder->m_Skin.m_Texture, textureID, m_OpenGLResources);

        // add a new item to the manager
        CSR_Level::IItem* pItem = Add(pCylinder);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            csrMeshRelease(pCylinder, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // keep the model resources
        pItem->m_Type                        = CSR_Level::IE_IT_Cylinder;
        pItem->m_Resources.m_Files.m_Texture = textureName;
        pItem->m_Resources.m_Faces           = faces;

        // set the model matrix
        *pItem->m_Matrices[pItem->m_Matrices.size() - 1] = matrix;

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        CSR_SceneItem* pSceneItem = csrSceneAddMesh(m_pScene, pCylinder, 0, collisionType != CSR_CO_None);
        csrSceneAddModelMatrix(m_pScene, pCylinder, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // succeeded?
        if (!pSceneItem)
        {
            csrMeshRelease(pCylinder, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // set the collision type
        pSceneItem->m_CollisionType = collisionType;

        // notify the designer about the new selection
        if (fSelectModel)
            fSelectModel(pCylinder, 0);
    }
    catch (...)
    {
        csrMeshRelease(pCylinder, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    return pCylinder;
}
//---------------------------------------------------------------------------
void* CSR_Level::AddDisk(const CSR_Matrix4&       matrix,
                         const std::string&       textureName,
                               int                slices,
                               CSR_ECollisionType collisionType,
                               ITfLoadTexture     fLoadTexture,
                               ITfSelectModel     fSelectModel)
{
    // no load texture callback?
    if (!fLoadTexture)
        return NULL;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default disk mesh
    CSR_Mesh* pDisk = csrShapeCreateDisk(0.0f, 0.0f, 0.5f, slices, &vf, 0, &material, 0);

    // succeeded?
    if (!pDisk)
        return NULL;

    try
    {
        // load the texture
        const GLuint textureID = fLoadTexture(textureName);

        // failed?
        if (textureID == M_CSR_Error_Code)
        {
            csrMeshRelease(pDisk, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // add the texture to OpenGL resources
        CSR_OpenGLHelper::AddTexture(&pDisk->m_Skin.m_Texture, textureID, m_OpenGLResources);

        // add a new item to the manager
        CSR_Level::IItem* pItem = Add(pDisk);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            csrMeshRelease(pDisk, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // keep the model resources
        pItem->m_Type                        = CSR_Level::IE_IT_Disk;
        pItem->m_Resources.m_Files.m_Texture = textureName;
        pItem->m_Resources.m_Slices          = slices;

        // set the model matrix
        *pItem->m_Matrices[pItem->m_Matrices.size() - 1] = matrix;

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        CSR_SceneItem* pSceneItem = csrSceneAddMesh(m_pScene, pDisk, 0, collisionType != CSR_CO_None);
        csrSceneAddModelMatrix(m_pScene, pDisk, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // succeeded?
        if (!pSceneItem)
        {
            csrMeshRelease(pDisk, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // set the collision type
        pSceneItem->m_CollisionType = collisionType;

        // notify the designer about the new selection
        if (fSelectModel)
            fSelectModel(pDisk, 0);
    }
    catch (...)
    {
        csrMeshRelease(pDisk, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    return pDisk;
}
//---------------------------------------------------------------------------
void* CSR_Level::AddRing(const CSR_Matrix4&       matrix,
                         const std::string&       textureName,
                               int                slices,
                               int                radius,
                               CSR_ECollisionType collisionType,
                               ITfLoadTexture     fLoadTexture,
                               ITfSelectModel     fSelectModel)
{
    // no load texture callback?
    if (!fLoadTexture)
        return NULL;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default ring mesh
    CSR_Mesh* pRing = csrShapeCreateRing(0.0f,
                                         0.0f,
                                         0.5f * (float(radius) * 0.01f),
                                         0.5f,
                                         slices,
                                        &vf,
                                         0,
                                        &material,
                                         0);

    // succeeded?
    if (!pRing)
        return NULL;

    try
    {
        // load the texture
        const GLuint textureID = fLoadTexture(textureName);

        // failed?
        if (textureID == M_CSR_Error_Code)
        {
            csrMeshRelease(pRing, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // add the texture to the OpenGL resources
        CSR_OpenGLHelper::AddTexture(&pRing->m_Skin.m_Texture, textureID, m_OpenGLResources);

        // add a new item to the manager
        CSR_Level::IItem* pItem = Add(pRing);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            csrMeshRelease(pRing, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // keep the model resources
        pItem->m_Type                        = CSR_Level::IE_IT_Ring;
        pItem->m_Resources.m_Files.m_Texture = textureName;
        pItem->m_Resources.m_Slices          = slices;
        pItem->m_Resources.m_Radius          = radius;

        // set the model matrix
        *pItem->m_Matrices[pItem->m_Matrices.size() - 1] = matrix;

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        CSR_SceneItem* pSceneItem = csrSceneAddMesh(m_pScene, pRing, 0, collisionType != CSR_CO_None);
        csrSceneAddModelMatrix(m_pScene, pRing, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // succeeded?
        if (!pSceneItem)
        {
            csrMeshRelease(pRing, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // set the collision type
        pSceneItem->m_CollisionType = collisionType;

        // notify the designer about the new selection
        if (fSelectModel)
            fSelectModel(pRing, 0);
    }
    catch (...)
    {
        csrMeshRelease(pRing, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    return pRing;
}
//---------------------------------------------------------------------------
void* CSR_Level::AddSphere(const CSR_Matrix4&       matrix,
                           const std::string&       textureName,
                                 int                slices,
                                 int                stacks,
                                 CSR_ECollisionType collisionType,
                                 ITfLoadTexture     fLoadTexture,
                                 ITfSelectModel     fSelectModel)
{
    // no load texture callback?
    if (!fLoadTexture)
        return NULL;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default sphere mesh
    CSR_Mesh* pSphere = csrShapeCreateSphere(0.5f, slices, stacks, &vf, 0, &material, 0);

    // succeeded?
    if (!pSphere)
        return NULL;

    try
    {
        // load the texture
        const GLuint textureID = fLoadTexture(textureName);

        // failed?
        if (textureID == M_CSR_Error_Code)
        {
            csrMeshRelease(pSphere, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // add the texture to the OpenGL resources
        CSR_OpenGLHelper::AddTexture(&pSphere->m_Skin.m_Texture, textureID, m_OpenGLResources);

        // add a new item to the manager
        CSR_Level::IItem* pItem = Add(pSphere);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            csrMeshRelease(pSphere, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // keep the model resources
        pItem->m_Type                        = CSR_Level::IE_IT_Sphere;
        pItem->m_Resources.m_Files.m_Texture = textureName;
        pItem->m_Resources.m_Slices          = slices;
        pItem->m_Resources.m_Stacks          = stacks;

        // set the model matrix
        *pItem->m_Matrices[pItem->m_Matrices.size() - 1] = matrix;

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        CSR_SceneItem* pSceneItem = csrSceneAddMesh(m_pScene, pSphere, 0, collisionType != CSR_CO_None);
        csrSceneAddModelMatrix(m_pScene, pSphere, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // succeeded?
        if (!pSceneItem)
        {
            csrMeshRelease(pSphere, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // set the collision type
        pSceneItem->m_CollisionType = collisionType;

        // notify the designer about the new selection
        if (fSelectModel)
            fSelectModel(pSphere, 0);
    }
    catch (...)
    {
        csrMeshRelease(pSphere, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    return pSphere;
}
//---------------------------------------------------------------------------
void* CSR_Level::AddSpiral(const CSR_Matrix4&       matrix,
                           const std::string&       textureName,
                                 int                radius,
                                 int                deltaMin,
                                 int                deltaMax,
                                 int                deltaZ,
                                 int                slices,
                                 int                stacks,
                                 CSR_ECollisionType collisionType,
                                 ITfLoadTexture     fLoadTexture,
                                 ITfSelectModel     fSelectModel)
{
    // no load texture callback?
    if (!fLoadTexture)
        return NULL;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default spiral mesh
    CSR_Mesh* pSpiral = csrShapeCreateSpiral(0.0f,
                                             0.0f,
                                             0.5f * (float(radius) * 0.01f),
                                             0.5f,
                                             float(deltaMin) * 0.001f,
                                             float(deltaMax) * 0.001f,
                                             float(deltaZ)   * 0.001f,
                                             slices,
                                             stacks,
                                            &vf,
                                             0,
                                            &material,
                                             0);

    // succeeded?
    if (!pSpiral)
        return NULL;

    try
    {
        // load the texture
        const GLuint textureID = fLoadTexture(textureName);

        // failed?
        if (textureID == M_CSR_Error_Code)
        {
            csrMeshRelease(pSpiral, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // add the texture to the OpenGL resources
        CSR_OpenGLHelper::AddTexture(&pSpiral->m_Skin.m_Texture, textureID, m_OpenGLResources);

        // add a new item to the manager
        CSR_Level::IItem* pItem = Add(pSpiral);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            csrMeshRelease(pSpiral, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // keep the model resources
        pItem->m_Type                        = CSR_Level::IE_IT_Spiral;
        pItem->m_Resources.m_Files.m_Texture = textureName;
        pItem->m_Resources.m_Radius          = radius;
        pItem->m_Resources.m_DeltaMin        = deltaMin;
        pItem->m_Resources.m_DeltaMax        = deltaMax;
        pItem->m_Resources.m_DeltaZ          = deltaZ;
        pItem->m_Resources.m_Slices          = slices;
        pItem->m_Resources.m_Stacks          = stacks;

        // set the model matrix
        *pItem->m_Matrices[pItem->m_Matrices.size() - 1] = matrix;

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        CSR_SceneItem* pSceneItem =csrSceneAddMesh(m_pScene, pSpiral, 0, collisionType != CSR_CO_None);
        csrSceneAddModelMatrix(m_pScene, pSpiral, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // succeeded?
        if (!pSceneItem)
        {
            csrMeshRelease(pSpiral, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // set the collision type
        pSceneItem->m_CollisionType = collisionType;

        // notify the designer about the new selection
        if (fSelectModel)
            fSelectModel(pSpiral, 0);
    }
    catch (...)
    {
        csrMeshRelease(pSpiral, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    return pSpiral;
}
//---------------------------------------------------------------------------
void* CSR_Level::AddSurface(const CSR_Matrix4&       matrix,
                            const std::string&       textureName,
                                  CSR_ECollisionType collisionType,
                                  ITfLoadTexture     fLoadTexture,
                                  ITfSelectModel     fSelectModel)
{
    // no load texture callback?
    if (!fLoadTexture)
        return NULL;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    // create a default surface mesh
    CSR_Mesh* pSurface = csrShapeCreateSurface(1.0f, 1.0f, &vf, 0, &material, 0);

    // succeeded?
    if (!pSurface)
        return NULL;

    try
    {
        // load the texture
        const GLuint textureID = fLoadTexture(textureName);

        // failed?
        if (textureID == M_CSR_Error_Code)
        {
            csrMeshRelease(pSurface, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // add the texture to the OpenGL resources
        CSR_OpenGLHelper::AddTexture(&pSurface->m_Skin.m_Texture, textureID, m_OpenGLResources);

        // add a new item to the manager
        CSR_Level::IItem* pItem = Add(pSurface);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            csrMeshRelease(pSurface, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // keep the model resources
        pItem->m_Type                        = CSR_Level::IE_IT_Surface;
        pItem->m_Resources.m_Files.m_Texture = textureName;

        // set the model matrix
        *pItem->m_Matrices[pItem->m_Matrices.size() - 1] = matrix;

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        CSR_SceneItem* pSceneItem = csrSceneAddMesh(m_pScene, pSurface, 0, collisionType != CSR_CO_None);
        csrSceneAddModelMatrix(m_pScene, pSurface, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // succeeded?
        if (!pSceneItem)
        {
            csrMeshRelease(pSurface, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // set the collision type
        pSceneItem->m_CollisionType = collisionType;

        // notify the designer about the new selection
        if (fSelectModel)
            fSelectModel(pSurface, 0);
    }
    catch (...)
    {
        csrMeshRelease(pSurface, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    return pSurface;
}
//---------------------------------------------------------------------------
void* CSR_Level::AddWaveFront(const CSR_Matrix4&       matrix,
                              const std::string&       fileName,
                              const std::string&       textureName,
                              const CSR_Buffer*        pBuffer,
                                    CSR_ECollisionType collisionType,
                                    ITfLoadTexture     fLoadTexture,
                                    ITfSelectModel     fSelectModel)
{
    // no load texture callback?
    if (!fLoadTexture)
        return NULL;

    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexCulling vc;
    vc.m_Type = CSR_CT_None;
    vc.m_Face = CSR_CF_CW;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    CSR_Model* pModel;

    // load the Wavefront model
    if (pBuffer)
        pModel = csrWaveFrontCreate(pBuffer, &vf, &vc, &material, 0, 0, m_SceneContext.m_fOnDeleteTexture);
    else
        pModel = csrWaveFrontOpen(fileName.c_str(), &vf, &vc, &material, 0, 0, m_SceneContext.m_fOnDeleteTexture);

    // succeeded?
    if (!pModel)
        return NULL;

    // empty model?
    if (!pModel->m_MeshCount)
    {
        csrModelRelease(pModel, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    try
    {
        // load the texture
        const GLuint textureID = fLoadTexture(textureName);

        // failed?
        if (textureID == M_CSR_Error_Code)
        {
            csrModelRelease(pModel, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // add the texture to the OpenGL resources
        CSR_OpenGLHelper::AddTexture(&pModel->m_pMesh[0].m_Skin.m_Texture, textureID, m_OpenGLResources);

        // add a new item to the manager
        CSR_Level::IItem* pItem = Add(pModel);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            csrModelRelease(pModel, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // keep the model resources
        pItem->m_Type                        = CSR_Level::IE_IT_WaveFront;
        pItem->m_Resources.m_Files.m_Model   = fileName;
        pItem->m_Resources.m_Files.m_Texture = textureName;

        // set the model matrix
        *pItem->m_Matrices[pItem->m_Matrices.size() - 1] = matrix;

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        CSR_SceneItem* pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, collisionType != CSR_CO_None);
        csrSceneAddModelMatrix(m_pScene, pModel, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // succeeded?
        if (!pSceneItem)
        {
            csrModelRelease(pModel, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // set the collision type
        pSceneItem->m_CollisionType = collisionType;

        // notify the designer about the new selection
        if (fSelectModel)
            fSelectModel(pModel, 0);
    }
    catch (...)
    {
        csrModelRelease(pModel, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    return pModel;
}
//---------------------------------------------------------------------------
void* CSR_Level::AddMDL(const CSR_Matrix4&       matrix,
                        const std::string&       fileName,
                        const CSR_Buffer*        pBuffer,
                              CSR_ECollisionType collisionType,
                              CSR_fOnApplySkin   fOnApplySkin,
                              ITfSelectModel     fSelectModel)
{
    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    CSR_MDL* pMDL;

    // load the MDL model
    if (pBuffer)
        pMDL = csrMDLCreate(pBuffer, 0, &vf, 0, &material, 0, fOnApplySkin, m_SceneContext.m_fOnDeleteTexture);
    else
        pMDL = csrMDLOpen(fileName.c_str(), 0, &vf, 0, &material, 0, fOnApplySkin, m_SceneContext.m_fOnDeleteTexture);

    // succeeded?
    if (!pMDL)
        return NULL;

    try
    {
        // add a new item to the manager
        CSR_Level::IItem* pItem = Add(pMDL);

        // succeeded?
        if (!pItem || !pItem->m_Matrices.size())
        {
            csrMDLRelease(pMDL, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // keep the model resources
        pItem->m_Type                      = CSR_Level::IE_IT_MDL;
        pItem->m_Resources.m_Files.m_Model = fileName;

        // set the model matrix
        *pItem->m_Matrices[pItem->m_Matrices.size() - 1] = matrix;

        // add the model to the scene. Generate the AABB tree to allow the mouse collision
        CSR_SceneItem* pSceneItem = csrSceneAddMDL(m_pScene, pMDL, 0, collisionType != CSR_CO_None);
        csrSceneAddModelMatrix(m_pScene, pMDL, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

        // succeeded?
        if (!pSceneItem)
        {
            csrMDLRelease(pMDL, m_SceneContext.m_fOnDeleteTexture);
            return NULL;
        }

        // set the collision type
        pSceneItem->m_CollisionType = collisionType;

        // notify the designer about the new selection
        if (fSelectModel)
            fSelectModel(pMDL, 0);
    }
    catch (...)
    {
        csrMDLRelease(pMDL, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    return pMDL;
}
//---------------------------------------------------------------------------
void* CSR_Level::AddLandscape(const std::string&        fileName,
                              const std::string&        textureName,
                              const CSR_Buffer*         pBuffer,
                                    ITfOnUpdateDesigner fOnUpdateDesigner)
{
    CSR_Material material;
    material.m_Color       = 0xFFFFFFFF;
    material.m_Transparent = 0;
    material.m_Wireframe   = 0;

    CSR_VertexCulling vc;
    vc.m_Type = CSR_CT_Front;
    vc.m_Face = CSR_CF_CW;

    CSR_VertexFormat vf;
    vf.m_HasNormal         = 0;
    vf.m_HasTexCoords      = 1;
    vf.m_HasPerVertexColor = 1;

    CSR_Model* pModel;

    // load the model
    if (pBuffer)
        pModel = csrWaveFrontCreate(pBuffer, &vf, &vc, &material, 0, 0, m_SceneContext.m_fOnDeleteTexture);
    else
        pModel = csrWaveFrontOpen(fileName.c_str(), &vf, &vc, &material, 0, 0, m_SceneContext.m_fOnDeleteTexture);

    // succeeded?
    if (!pModel)
        return NULL;

    // add a new item to the manager
    CSR_Level::IItem* pItem = Add(pModel);

    // succeeded?
    if (!pItem || !pItem->m_Matrices.size())
    {
        csrModelRelease(pModel, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    // keep the model resources
    pItem->m_Type                        = CSR_Level::IE_IT_Landscape;
    pItem->m_Resources.m_Files.m_Model   = fileName;
    pItem->m_Resources.m_Files.m_Texture = textureName;

    // add the model to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

    // succeeded?
    if (!pSceneItem)
    {
        csrModelRelease(pModel, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    // set the collision type
    pSceneItem->m_CollisionType = CSR_ECollisionType(CSR_CO_Ground | CSR_CO_Custom);

    // keep the key
    m_pLandscapeKey = pModel;

    // do update the level designer?
    if (fOnUpdateDesigner)
    {
        // calculate the model length (required later for some calculation)
        CSR_Vector3 modelLength = CalculateModelLength(pSceneItem);

        // notify the designer that it should be updated
        fOnUpdateDesigner(pModel, 0, modelLength);
    }

    return pModel;
}
//---------------------------------------------------------------------------
void* CSR_Level::AddLandscapeFromBitmap(const std::string&        fileName,
                                        const std::string&        textureName,
                                        const CSR_Buffer*         pBuffer,
                                              ITfOnUpdateDesigner fOnUpdateDesigner)
{
    CSR_Model*       pModel  = NULL;
    CSR_PixelBuffer* pBitmap = NULL;

    try
    {
        CSR_Material material;
        material.m_Color       = 0xFFFFFFFF;
        material.m_Transparent = 0;
        material.m_Wireframe   = 0;

        CSR_VertexCulling vc;
        vc.m_Type = CSR_CT_None;
        vc.m_Face = CSR_CF_CW;

        CSR_VertexFormat vf;
        vf.m_HasNormal         = 0;
        vf.m_HasTexCoords      = 1;
        vf.m_HasPerVertexColor = 1;

        // load a default grayscale bitmap from which a landscape will be generated
        if (pBuffer)
            pBitmap = csrPixelBufferFromBitmapBuffer(pBuffer);
        else
            pBitmap = csrPixelBufferFromBitmapFile(fileName.c_str());

        // succeeded?
        if (!pBitmap)
            return NULL;

        // create a model to contain the landscape
        pModel = csrModelCreate();

        // succeeded?
        if (!pModel)
            return NULL;

        // load the landscape mesh from the grayscale bitmap
        pModel->m_pMesh     = csrLandscapeCreate(pBitmap, 3.0f, 0.2f, &vf, &vc, &material, 0);
        pModel->m_MeshCount = 1;
    }
    catch (...)
    {
        csrPixelBufferRelease(pBitmap);
        return NULL;
    }

    csrPixelBufferRelease(pBitmap);

    // add a new item to the manager
    CSR_Level::IItem* pItem = Add(pModel);

    // succeeded?
    if (!pItem || !pItem->m_Matrices.size())
    {
        csrModelRelease(pModel, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    // keep the model resources
    pItem->m_Type                             = CSR_Level::IE_IT_Landscape;
    pItem->m_Resources.m_Files.m_LandscapeMap = fileName;
    pItem->m_Resources.m_Files.m_Texture      = textureName;

    // add the model to the scene
    CSR_SceneItem* pSceneItem = csrSceneAddModel(m_pScene, pModel, 0, 1);
    csrSceneAddModelMatrix(m_pScene, pModel, pItem->m_Matrices[pItem->m_Matrices.size() - 1]);

    // succeeded?
    if (!pSceneItem)
    {
        csrModelRelease(pModel, m_SceneContext.m_fOnDeleteTexture);
        return NULL;
    }

    // set the collision type
    pSceneItem->m_CollisionType = CSR_ECollisionType(CSR_CO_Ground | CSR_CO_Custom);

    // keep the key
    m_pLandscapeKey = pModel;

    // do update the level designer?
    if (fOnUpdateDesigner)
    {
        // calculate the model length (required later for some calculation)
        CSR_Vector3 modelLength = CalculateModelLength(pSceneItem);

        // notify the designer that it should be updated
        fOnUpdateDesigner(pModel, 0, modelLength);
    }

    return pModel;
}
//---------------------------------------------------------------------------
int CSR_Level::AddDuplicate(void* pKey, const CSR_Matrix4& matrix, ITfSelectModel fSelectModel)
{
    // no key?
    if (!pKey)
        return -1;

    // duplicate the model
    const IItem* pItem = Add(pKey);

    // succeeded?
    if (!pItem)
        return -1;

    // get the matrix index
    const int index = pItem->m_Matrices.size() - 1;

    // set the matrix content
    *pItem->m_Matrices[index] = matrix;

    // add the duplicate matrix in the scene item
    if (!csrSceneAddModelMatrix(m_pScene, pKey, pItem->m_Matrices[index]))
        return -1;

    // notify the designer about the new selection
    if (fSelectModel)
        fSelectModel(pKey, index);

    return index;
}
//---------------------------------------------------------------------------
bool CSR_Level::OpenSound(const std::string& fileName, const CSR_Buffer* pBuffer)
{
    // do change the ambient sound?
    if (fileName.empty())
        return false;

    // stop the current sound and delete it
    csrSoundStop(m_pSound);
    csrSoundRelease(m_pSound);

    // keep the sound file in the level manager resources
    m_Sound.m_FileName = fileName;

    // load the sound file
    m_pSound = LoadSound(fileName, pBuffer);

    return true;
}
//---------------------------------------------------------------------------
void CSR_Level::PauseSound(bool pause)
{
    // do pause the sound?
    if (pause)
    {
        // pause the background sound
        if (csrSoundIsPlaying(m_pSound))
            csrSoundPause(m_pSound);

        return;
    }

    // play the background sound
    if (!csrSoundIsPlaying(m_pSound))
        csrSoundPlay(m_pSound);
}
//---------------------------------------------------------------------------
float CSR_Level::GetViewXPos() const
{
    return m_pScene->m_ViewMatrix.m_Table[3][0];
}
//---------------------------------------------------------------------------
float CSR_Level::GetViewYPos() const
{
    return m_pScene->m_ViewMatrix.m_Table[3][1];
}
//---------------------------------------------------------------------------
float CSR_Level::GetViewZPos() const
{
    return m_pScene->m_ViewMatrix.m_Table[3][2];
}
//---------------------------------------------------------------------------
CSR_Scene* CSR_Level::CreateScene()
{
    if (!m_pScene)
        BuildScene();

    return m_pScene;
}
//---------------------------------------------------------------------------
CSR_Scene* CSR_Level::GetScene() const
{
    return m_pScene;
}
//---------------------------------------------------------------------------
void CSR_Level::EnableShader() const
{
    // enable the shader program
    csrShaderEnable(m_pShader);
}
//---------------------------------------------------------------------------
void CSR_Level::EnableMSAA(bool value)
{
    m_UseMSAA = value;
}
//---------------------------------------------------------------------------
void CSR_Level::EnableOilPainting(bool value)
{
    m_UseOilPainting = value;
}
//---------------------------------------------------------------------------
void CSR_Level::AddTexture(const void* pKey, GLuint textureID)
{
    // add the texture to the OpenGL resources
    CSR_OpenGLHelper::AddTexture(pKey, textureID, m_OpenGLResources);
}
//---------------------------------------------------------------------------
void CSR_Level::DeleteTexture(const void* pKey)
{
    // add the texture to the OpenGL resources
    CSR_OpenGLHelper::DeleteTexture(pKey, m_OpenGLResources);
}
//---------------------------------------------------------------------------
CSR_Level::IItem* CSR_Level::Add(void* pKey)
{
    // search for an item matching with key
    IItems::iterator it = m_Items.find(pKey);

    // found one?
    if (it != m_Items.end())
    {
        // just add a new matrix for the cloned model and return the existing item
        it->second->AddMatrix();
        return it->second;
    }

    // create and initialize a new level manager item
    std::auto_ptr<IItem> pItem(new IItem());
    pItem->AddMatrix();

    // link the scene item to the manager
    m_Items[pKey] = pItem.get();

    return pItem.release();
}
//---------------------------------------------------------------------------
void CSR_Level::Delete(void* pKey, std::size_t index)
{
    // search for the item matching with key
    IItems::iterator it = m_Items.find(pKey);

    // found it?
    if (it == m_Items.end())
        // nothing to do
        return;

    // delete the matrix at the index
    it->second->DeleteMatrix(index);

    // is item empty?
    if (!it->second->m_Matrices.size())
    {
        // delete and remove it from the manager
        delete it->second;
        m_Items.erase(it);
    }
}
//---------------------------------------------------------------------------
void CSR_Level::Delete(void* pKey)
{
    // search for the item matching with key
    IItems::iterator it = m_Items.find(pKey);

    // found it?
    if (it == m_Items.end())
        // nothing to do
        return;

    // delete the level item and remove it from the manager
    delete it->second;
    m_Items.erase(it);
}
//---------------------------------------------------------------------------
CSR_Level::IItem* CSR_Level::Get(void* pKey) const
{
    // search for the item matching with key
    IItems::const_iterator it = m_Items.find(pKey);

    // found it?
    if (it == m_Items.end())
        return NULL;

    return it->second;
}
//---------------------------------------------------------------------------
CSR_SceneItem* CSR_Level::GetLandscape() const
{
    return csrSceneGetItem(m_pScene, m_pLandscapeKey);
}
//---------------------------------------------------------------------------
CSR_SceneItem* CSR_Level::GetSceneItem(void* pKey) const
{
    return csrSceneGetItem(m_pScene, pKey);
}
//---------------------------------------------------------------------------
void CSR_Level::ApplyGroundCollision(CSR_Sphere*  pBoundingSphere,
                                     float        angle,
                               const CSR_Vector3& oldPos) const
{
    if (!pBoundingSphere)
        return;

    // calculate the ground position and check if next position is valid
    if (ApplyGroundCollision(pBoundingSphere, angle, &m_pScene->m_ViewMatrix))
        return;

    // invalid next position, get the scene item (just one for this scene)
    const CSR_SceneItem* pItem = GetLandscape();

    // found it?
    if (pItem)
    {
        // check if the x position is out of bounds, and correct it if yes
        if (pBoundingSphere->m_Center.m_X <= pItem->m_pAABBTree->m_pBox->m_Min.m_X ||
            pBoundingSphere->m_Center.m_X >= pItem->m_pAABBTree->m_pBox->m_Max.m_X)
            pBoundingSphere->m_Center.m_X = oldPos.m_X;

        // do the same thing with the z position. Doing that separately for each axis will make
        // the point of view to slide against the landscape border (this is possible because the
        // landscape is axis-aligned)
        if (pBoundingSphere->m_Center.m_Z <= pItem->m_pAABBTree->m_pBox->m_Min.m_Z ||
            pBoundingSphere->m_Center.m_Z >= pItem->m_pAABBTree->m_pBox->m_Max.m_Z)
            pBoundingSphere->m_Center.m_Z = oldPos.m_Z;
    }
    else
        // failed to get the scene item, just revert the position
        pBoundingSphere->m_Center = oldPos;

    // recalculate the ground value (this time the collision result isn't tested, because the
    // previous position is always considered as valid)
    ApplyGroundCollision(pBoundingSphere, angle, &m_pScene->m_ViewMatrix);
}
//------------------------------------------------------------------------------
void CSR_Level::Draw() const
{
    // draw the scene
    csrSceneDraw(m_pScene, &m_SceneContext);
}
//---------------------------------------------------------------------------
void CSR_Level::OnApplySkin(size_t index, const CSR_Skin* pSkin, int* pCanRelease)
{
    // load the texture from the received pixel buffer
    const GLuint textureID = csrOpenGLTextureFromPixelBuffer(pSkin->m_Texture.m_pBuffer);

    // suceeded?
    if (textureID == M_CSR_Error_Code)
        return;

    // add the texture to the OpenGL resources
    CSR_OpenGLHelper::AddTexture(&pSkin->m_Texture, textureID, m_OpenGLResources);

    // from now the texture resource on the model side may be released (will no longer be used)
    if (pCanRelease)
        *pCanRelease = 1;
}
//---------------------------------------------------------------------------
void* CSR_Level::OnGetShader(const void* pModel, CSR_EModelType type)
{
    if (pModel == m_pScene->m_pSkybox)
        return m_pSkyboxShader;

    return m_pShader;
}
//---------------------------------------------------------------------------
void* CSR_Level::OnGetID(const void* pKey)
{
    return CSR_OpenGLHelper::GetTextureID(pKey, m_OpenGLResources);
}
//---------------------------------------------------------------------------
void CSR_Level::OnSceneBegin(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    if (m_UseOilPainting)
        m_pEffect->DrawBegin(&pScene->m_Color);
    else
    if (m_UseMSAA)
        csrOpenGLMSAADrawBegin(&pScene->m_Color, m_pMSAA);
    else
        csrOpenGLDrawBegin(&pScene->m_Color);
}
//---------------------------------------------------------------------------
void CSR_Level::OnSceneEnd(const CSR_Scene* pScene, const CSR_SceneContext* pContext)
{
    if (m_UseOilPainting)
        m_pEffect->DrawEnd();
    else
    if (m_UseMSAA)
        csrOpenGLMSAADrawEnd(m_pMSAA);
    else
        csrOpenGLDrawEnd();
}
//------------------------------------------------------------------------------
void CSR_Level::OnDeleteTexture(const CSR_Texture* pTexture)
{
    CSR_OpenGLHelper::DeleteTexture(pTexture, m_OpenGLResources);
}
//------------------------------------------------------------------------------
void CSR_Level::BuildScene()
{
    // initialize the scene
    m_pScene = csrSceneCreate();

    // configure the scene background color
    m_pScene->m_Color.m_R = 0.45f;
    m_pScene->m_Color.m_G = 0.8f;
    m_pScene->m_Color.m_B = 1.0f;
    m_pScene->m_Color.m_A = 1.0f;

    // configure the scene ground direction
    m_pScene->m_GroundDir.m_X =  0.0f;
    m_pScene->m_GroundDir.m_Y = -1.0f;
    m_pScene->m_GroundDir.m_Z =  0.0f;

    // configure the scene view matrix
    csrMat4Identity(&m_pScene->m_ProjectionMatrix);
    csrMat4Identity(&m_pScene->m_ViewMatrix);
}
//---------------------------------------------------------------------------
void CSR_Level::ClearScene()
{
    // release the scene
    csrSceneRelease(m_pScene, m_SceneContext.m_fOnDeleteTexture);

    // delete the scene textures
    CSR_OpenGLHelper::ClearResources(m_OpenGLResources);

    // clear the values
    m_pScene        = NULL;
    m_pLandscapeKey = NULL;
}
//---------------------------------------------------------------------------
CSR_Sound* CSR_Level::LoadSound(const std::string& fileName, const CSR_Buffer* pBuffer) const
{
    CSR_Sound* pSound;

    // load the sound file
    if (pBuffer)
        pSound = csrSoundOpenWavBuffer(m_pOpenALDevice, m_pOpenALContext, pBuffer);
    else
        pSound = csrSoundOpenWavFile(m_pOpenALDevice, m_pOpenALContext, fileName.c_str());

    // set the sound to loop
    csrSoundLoop(pSound, 1);

    // play the sound
    csrSoundPlay(pSound);

    return pSound;
}
//---------------------------------------------------------------------------
void CSR_Level::ClearSound()
{
    // stop and release the sound
    csrSoundStop(m_pSound);
    csrSoundRelease(m_pSound);
    m_pSound = NULL;
}
//---------------------------------------------------------------------------
CSR_Vector3 CSR_Level::CalculateModelLength(const CSR_SceneItem* pSceneItem) const
{
    CSR_Vector3 modelLength;

    // no model item, or model item has no collision data?
    if (!pSceneItem || !pSceneItem->m_AABBTreeCount)
    {
        modelLength.m_X = 0.0f;
        modelLength.m_Y = 0.0f;
        modelLength.m_Z = 0.0f;
    }
    else
    {
        // calculate the model length (required later for some calculation)
        modelLength.m_X = (pSceneItem->m_pAABBTree[0].m_pBox->m_Max.m_X -
                           pSceneItem->m_pAABBTree[0].m_pBox->m_Min.m_X);
        modelLength.m_Y = (pSceneItem->m_pAABBTree[0].m_pBox->m_Max.m_Y -
                           pSceneItem->m_pAABBTree[0].m_pBox->m_Min.m_Y);
        modelLength.m_Z = (pSceneItem->m_pAABBTree[0].m_pBox->m_Max.m_Z -
                           pSceneItem->m_pAABBTree[0].m_pBox->m_Min.m_Z);
    }

    return modelLength;
}
//---------------------------------------------------------------------------
bool CSR_Level::ApplyGroundCollision(const CSR_Sphere*  pBoundingSphere,
                                           float        angle,
                                           CSR_Matrix4* pMatrix) const
{
    if (!m_pScene)
        return false;

    // validate the inputs
    if (!pBoundingSphere || !pMatrix)
        return false;

    CSR_CollisionInput collisionInput;
    csrCollisionInputInit(&collisionInput);
    collisionInput.m_BoundingSphere.m_Radius = pBoundingSphere->m_Radius;

    CSR_Camera camera;

    // calculate the camera position in the 3d world, without the ground value
    camera.m_Position.m_X = -pBoundingSphere->m_Center.m_X;
    camera.m_Position.m_Y =  0.0f;
    camera.m_Position.m_Z = -pBoundingSphere->m_Center.m_Z;
    camera.m_xAngle       =  0.0f;
    camera.m_yAngle       =  angle;
    camera.m_zAngle       =  0.0f;
    camera.m_Factor.m_X   =  1.0f;
    camera.m_Factor.m_Y   =  1.0f;
    camera.m_Factor.m_Z   =  1.0f;
    camera.m_MatCombType  =  IE_CT_Translate_Scale_Rotate;

    // get the view matrix matching with the camera
    csrSceneCameraToMatrix(&camera, pMatrix);

    CSR_Vector3 modelCenter;

    // get the model center
    modelCenter.m_X = 0.0f;
    modelCenter.m_Y = 0.0f;
    modelCenter.m_Z = 0.0f;

    CSR_Matrix4 invertView;
    float       determinant;

    // calculate the current camera position above the landscape
    csrMat4Inverse(pMatrix, &invertView, &determinant);
    csrMat4Transform(&invertView, &modelCenter, &collisionInput.m_BoundingSphere.m_Center);
    collisionInput.m_CheckPos = collisionInput.m_BoundingSphere.m_Center;

    CSR_CollisionOutput collisionOutput;

    // calculate the collisions in the whole scene
    csrSceneDetectCollision(m_pScene, &collisionInput, &collisionOutput, 0);

    // update the ground position directly inside the matrix (this is where the final value is required)
    pMatrix->m_Table[3][1] = -collisionOutput.m_GroundPos;

    return (collisionOutput.m_Collision & CSR_CO_Ground);
}
//---------------------------------------------------------------------------
