
//---------------------------------------------------------------------------

// This software is Copyright (c) 2011 Embarcadero Technologies, Inc.
// You may only use this software if you are an authorized licensee
// of Delphi, C++Builder or RAD Studio (Embarcadero Products).
// This software is considered a Redistributable as defined under
// the software license agreement that comes with the Embarcadero Products
// and is subject to that software license agreement.

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <string>


#include "Main.h"
#include "QR_WaveFrontParser.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;
//---------------------------------------------------------------------------
// TMainForm
//---------------------------------------------------------------------------
#include <sstream>
__fastcall TMainForm::TMainForm(TComponent* pOwner) :
    TForm(pOwner),
    //REM m_pMD2(NULL),
    m_PreviousTime(::GetTickCount())
{
    Application->OnIdle = IdleLoop;
    size = 50.0f;

    /*REM
    m_hMD2 = Create_MD2();

    if (m_hMD2)
    {
        LoadMesh_MD2(m_hMD2, "E:\\Jeanmilost\\QREngine\\Projects\\3D models reader\\TestDLL\\Resources\\Models\\Female_animated_2.md2");
        LoadNormalsTable_MD2(m_hMD2, "E:\\Jeanmilost\\QREngine\\Projects\\3D models reader\\TestDLL\\Resources\\Tables\\Normals.bin");
        SetAnimation_MD2(m_hMD2, 0, 39, "WALK");
    }
    */
/*REM
    HRESULT hr;
    hr = ::CoInitialize(NULL);

    CLSID clsid;
    hr = ::CLSIDFromProgID(L"QRAPI.MD2Factory", &clsid);

    //REM hr = ::CoCreateInstance(clsid, NULL, CLSCTX_INPROC_SERVER, IID_IQRMD2, (void**)&m_pMD2);
    hr = ::CoGetClassObject(clsid, CLSCTX_INPROC_SERVER, NULL, IID_IQRMD2, (void**)&m_pMD2);
    //REM hr = pIX->QueryInterface(IID_IQRMD2, (void**)&pIY);

    if (m_pMD2)
    {
        m_pMD2->LoadMesh("E:\\Jeanmilost\\QREngine\\Projects\\3D models reader\\TestDLL\\Resources\\Models\\Female_animated_2.md2", NULL);
        m_pMD2->LoadNormalsTable("E:\\Jeanmilost\\QREngine\\Projects\\3D models reader\\TestDLL\\Resources\\Tables\\Normals.bin", NULL);
        m_pMD2->SetAnimation(0, 39, "WALK");
    }
    */

    QR_WaveFrontParser parser;
    parser.Load(L"E:\\Jeanmilost\\QREngine\\Projects\\Prototypes\\WaveFront reader\\Resources\\Models\\Female_animated.obj");
    for (QR_WaveFrontParser::IObjects::iterator it1 = parser.m_Objects.begin(); it1 != parser.m_Objects.end(); ++it1)
        for (QR_WaveFrontParser::IGroups::iterator it2 = it1->second->m_Groups.begin(); it2 != it1->second->m_Groups.end(); ++it2)
            for (std::size_t k = 0; k < it2->second->m_Faces.size(); ++k)
            {
                std::ostringstream sstr;
                for (std::size_t l = 0; l < it2->second->m_Faces[k]->m_VectorIndexes.size(); ++l)
                    sstr << it2->second->m_Faces[k]->m_VectorIndexes[l] << " - ";
                ::OutputDebugString(sstr.str().c_str());
            }
}
//---------------------------------------------------------------------------
__fastcall TMainForm::~TMainForm()
{
    /*REM
    if (m_pMD2)
        m_pMD2->Release();

    ::CoUninitialize();
    */
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::IdleLoop(TObject*, bool& done)
{
    done = false;
    RenderGLScene();
    SwapBuffers(hdc);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormCreate(TObject *Sender)
{
    hdc = GetDC(Handle);
    SetPixelFormatDescriptor();
    hrc = wglCreateContext(hdc);
    if(hrc == NULL)
        ShowMessage(":-)~ hrc == NULL");
    if(wglMakeCurrent(hdc, hrc) == false)
        ShowMessage("Could not MakeCurrent");
    w = ClientWidth;
    h = ClientHeight;

    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //SetupLighting();
    //SetupTextures();
    //CreateInpriseCorporateLogo();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetPixelFormatDescriptor()
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        24,
        0,0,0,0,0,0,
        0,0,
        0,0,0,0,0,
        32,
        0,
        0,
        PFD_MAIN_PLANE,
        0,
        0,0,
    };
    PixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, PixelFormat, &pfd);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject *Sender)
{

    GLfloat nRange = 50.0;

    w = ClientWidth;
    h = ClientHeight;

    if(h == 0)
    	h = 1;

    glViewport(0, 0, w, h);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    /*REM
    if (w <= h)
        glOrtho (-nRange, nRange, -nRange*h/w, nRange*h/w, -nRange, nRange);
    else
	glOrtho (-nRange*w/h, nRange*w/h, -nRange, nRange, -nRange, nRange);
    */

    //For perspective correction
    GLfloat aspect = (GLfloat)w/(GLfloat)h;
    gluPerspective(45.0f, aspect, 0.1f, 10000.0f);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::RenderGLScene()
{
    const double elapsedTime = (::GetTickCount() - m_PreviousTime) * 10.0f;

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);// | GL_DEPTH_BUFFER_BIT);

    /*REM
    if (m_pMD2)
        m_pMD2->Animate(&elapsedTime, 50);
        */

    DrawObjects();
    glFlush();

    m_PreviousTime = ::GetTickCount();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DrawObjects()
{
/*REM
    //GoldCube
    glBindTexture(GL_TEXTURE_2D, texture1);

    //Body
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f);
    glTranslatef(0.0f, 15.0f, 20.0f);
    glRotatef(90, 1.0, 0.0, 0.0);
    glCallList(startoflist);
    glPopMatrix();

    //Top
    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glColor3f(0.8f, 0.8f, 0.8f);
    glTranslatef(0.0f, 15.0f, 20.0f);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glCallList(startoflist + 1);
    glEnable(GL_CULL_FACE);
    glPopMatrix();

    //Bottom
    glPushMatrix();
    glColor3f(0.8f, 0.8f, 0.8f);
    glTranslatef(0.0f, -10.0f, 20.0f);
    glRotatef(90, 1.0, 0.0, 0.0);
    glCallList(startoflist + 2);
    glPopMatrix();


    //Red Pyramid
    glBindTexture(GL_TEXTURE_2D, texture2);
    //Body
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    glTranslatef(-20.0f, -10.0f, -5.0);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glCallList(startoflist + 3);
    glPopMatrix();

	//Bottom
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 0.0f);
    glTranslatef(-20.0f, -10.0f, -5.0);
    glRotatef(90, 1.0, 0.0, 0.0);
    glCallList(startoflist + 4);
    glPopMatrix();

    //Blue Sphere
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture3);
    glColor3f(0.0f, 0.0f, 1.0f);
    glTranslatef(15.0, 5.0f, -7.5f);
    glRotatef(90, 1.0, 0.0, 0.0);
    glRotatef(90, 0.0, 0.0, 1.0);
    glCallList(startoflist + 5);
    glPopMatrix();

    //Ring
    glPushMatrix();
    glDisable(GL_CULL_FACE);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glCallList(startoflist + 6);
    glEnable(GL_CULL_FACE);
    glPopMatrix();
    */

    /*
    QRAPI_EVertexFormat format = QRAPI_VF_None;//(QRAPI_EVertexFormat)(QRAPI_VF_Normals | QRAPI_VF_TexCoords | QRAPI_VF_Colors);

    // calculate vertex size
    std::size_t vertexSize = 3;

    // do include normals?
    if (format & QRAPI_VF_Normals)
        vertexSize += 3;

    // do include texture coordinates?
    if (format & QRAPI_VF_TexCoords)
        vertexSize += 2;

    // do include vertex color?
    if (format & QRAPI_VF_Colors)
        vertexSize += 4;
        */

    /*REM
        static fuck = false;

        if (fuck)
        return;
    */

    /*REM
    QRAPI_VertexBufferList vbs;
    vbs.resize(2000);
    //GetVertexBuffer_MD2(m_hMD2, &vbs, format, false);

    //fuck = true;
    //return;

    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

    for (unsigned i = 0; i < vbs.size(); ++i)
    {
        QRAPI_VertexBuffer* pBuffer = vbs[i];

        if (!pBuffer)
            continue;

        glPushMatrix();
        //glMatrixMode(GL_MODELVIEW);
        //glLoadIdentity();

        //glEnable(GL_DEPTH_TEST);
        //glDepthMask(GL_TRUE);
        //glDepthFunc(GL_LEQUAL);
        //glDepthRange(0.0f, 1.0f);
        glTranslatef(0.0f, -7.0f, -30.0f);
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        //glRotatef(135, 0.0f, 1.0f, 0.0f);
        //glCullFace(GL_FRONT_AND_BACK);
        //glEnable(GL_FRONT_FACE);
        //glScalef(10.0f, 10.0f, 10.0f);

//        for (unsigned j = 0; j < pBuffer->m_Data.size(); ++j)
//            ::OutputDebugStringW(::FloatToStr(pBuffer->m_Data[j]).c_str());

             // bind vertex array
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, vertexSize * sizeof(M_QRAPI_Precision), &pBuffer->m_Data[0]);

        std::size_t offset = 3;

        // bind normals array
        if (format & QRAPI_VF_Normals)
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT,
                            vertexSize * sizeof(M_QRAPI_Precision),
                            &pBuffer->m_Data[offset]);

            offset += 3;
        }

        // bind texture coordinates array
        if (format & QRAPI_VF_TexCoords)
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2,
                              GL_FLOAT,
                              vertexSize * sizeof(M_QRAPI_Precision),
                              &pBuffer->m_Data[offset]);

            offset += 2;
        }

        // bind colors array
        if (format & QRAPI_VF_Colors)
        {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4,
                           GL_FLOAT,
                           vertexSize * sizeof(M_QRAPI_Precision),
                           &pBuffer->m_Data[offset]);
        }

        // draw MD2 mesh
        switch (pBuffer->m_Type)
        {
            case QRAPI_VT_TriangleStrip:
                glDrawArrays(GL_TRIANGLE_STRIP, 0, pBuffer->m_Data.size() / vertexSize);
                break;

            case QRAPI_VT_TriangleFan:
                glDrawArrays(GL_TRIANGLE_FAN, 0, pBuffer->m_Data.size() / vertexSize);
                break;
        }

        // unbind vertex array
        glDisableClientState(GL_VERTEX_ARRAY);

        // unbind normals array
        if (format & QRAPI_VF_Normals)
            glDisableClientState(GL_NORMAL_ARRAY);

        // unbind texture coordinates array
        if (format & QRAPI_VF_TexCoords)
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        // unbind colors array
        if (format & QRAPI_VF_Colors)
            glDisableClientState(GL_COLOR_ARRAY);

        glFlush();
        glPopMatrix();

        delete pBuffer;
    }
    */

    /*REM
    QRAPI_VertexBufferContainer_Handle hVBs = Create_VertexBufferContainer();

    GetVertexBuffer_MD2(m_hMD2, hVBs, format, false);

    std::size_t vbCount = 0;
    Get_VertexBuffer_Count(hVBs, &vbCount);

    //fuck = true;
    //return;

    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

    for (unsigned i = 0; i < vbCount; ++i)
    {
        QRAPI_VertexBuffer_Handle hVB = Get_VertexBuffer(hVBs, i);

        if (!hVB)
            continue;

        glPushMatrix();
        //glMatrixMode(GL_MODELVIEW);
        //glLoadIdentity();

        //glEnable(GL_DEPTH_TEST);
        //glDepthMask(GL_TRUE);
        //glDepthFunc(GL_LEQUAL);
        //glDepthRange(0.0f, 1.0f);
        glTranslatef(0.0f, -7.0f, -30.0f);
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        //glRotatef(135, 0.0f, 1.0f, 0.0f);
        //glCullFace(GL_FRONT_AND_BACK);
        //glEnable(GL_FRONT_FACE);
        //glScalef(10.0f, 10.0f, 10.0f);

        QRAPI_EVertexType vertexType;
        Get_VertexBuffer_Type(hVB, &vertexType);

        std::size_t vbSize;
        Get_VertexBuffer_Size(hVB, &vbSize);

        M_QRAPI_Precision* pData = new M_QRAPI_Precision[vbSize];
        Get_VertexBuffer_Data(hVB, pData);

        // bind vertex array
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, vertexSize * sizeof(M_QRAPI_Precision), &pData[0]);

        std::size_t offset = 3;

        // bind normals array
        if (format & QRAPI_VF_Normals)
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT,
                            vertexSize * sizeof(M_QRAPI_Precision),
                            &pData[offset]);

            offset += 3;
        }

        // bind texture coordinates array
        if (format & QRAPI_VF_TexCoords)
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2,
                              GL_FLOAT,
                              vertexSize * sizeof(M_QRAPI_Precision),
                              &pData[offset]);

            offset += 2;
        }

        // bind colors array
        if (format & QRAPI_VF_Colors)
        {
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(4,
                           GL_FLOAT,
                           vertexSize * sizeof(M_QRAPI_Precision),
                           &pData[offset]);
        }

        // draw MD2 mesh
        switch (vertexType)
        {
            case QRAPI_VT_TriangleStrip:
                glDrawArrays(GL_TRIANGLE_STRIP, 0, vbSize / vertexSize);
                break;

            case QRAPI_VT_TriangleFan:
                glDrawArrays(GL_TRIANGLE_FAN, 0, vbSize / vertexSize);
                break;
        }

        // unbind vertex array
        glDisableClientState(GL_VERTEX_ARRAY);

        // unbind normals array
        if (format & QRAPI_VF_Normals)
            glDisableClientState(GL_NORMAL_ARRAY);

        // unbind texture coordinates array
        if (format & QRAPI_VF_TexCoords)
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        // unbind colors array
        if (format & QRAPI_VF_Colors)
            glDisableClientState(GL_COLOR_ARRAY);

        glFlush();
        glPopMatrix();

        delete[] pData;
        Delete_VertexBuffer(hVB);
    }

    Delete_VertexBufferContainer(hVBs);
    */

    /*REM
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

    glPushMatrix();
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();

    //glEnable(GL_DEPTH_TEST);
    //glDepthMask(GL_TRUE);
    //glDepthFunc(GL_LEQUAL);
    //glDepthRange(0.0f, 1.0f);
    glTranslatef(0.0f, -7.0f, -30.0f);
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    //glRotatef(135, 0.0f, 1.0f, 0.0f);
    //glCullFace(GL_FRONT_AND_BACK);
    //glEnable(GL_FRONT_FACE);
    //glScalef(10.0f, 10.0f, 10.0f);

    if (m_pMD2)
        m_pMD2->Draw_To_OpenGL_1(QRAPI_VF_None, false);

    glFlush();
    glPopMatrix();
    */
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormPaint(TObject *Sender)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFlush();
	DrawObjects();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
    gluDeleteQuadric(GoldCube);
    gluDeleteQuadric(GoldCubeTop);
    gluDeleteQuadric(GoldCubeBottom);
    gluDeleteQuadric(RedPyramid);
    gluDeleteQuadric(RedPyramidBottom);
    gluDeleteQuadric(BlueSphere);
    gluDeleteQuadric(Ring);
    delete bitmap;
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hrc);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::CreateInpriseCorporateLogo()
{
    startoflist = glGenLists(4);
    DrawGoldCube();
    DrawRedPyramid();
    DrawBlueSphere();
    DrawRing();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DrawGoldCube()
{
    GoldCube = gluNewQuadric();
    GoldCubeTop = gluNewQuadric();
    GoldCubeBottom = gluNewQuadric();

    glBindTexture(GL_TEXTURE_2D, texture1);
    gluQuadricTexture(GoldCube, GL_TRUE);
    gluQuadricDrawStyle(GoldCube, GLU_FILL);
    gluQuadricNormals(GoldCube, GLU_SMOOTH);

    gluQuadricTexture(GoldCubeTop, GL_TRUE);
    gluQuadricDrawStyle(GoldCubeTop, GLU_FILL);
    gluQuadricNormals(GoldCubeTop, GLU_SMOOTH);

    gluQuadricTexture(GoldCubeBottom, GL_TRUE);
    gluQuadricDrawStyle(GoldCubeBottom, GLU_FILL);
    gluQuadricNormals(GoldCubeBottom, GLU_SMOOTH);

    glNewList(startoflist, GL_COMPILE);
        gluCylinder(GoldCube, 15, 15, 25, 4, 10);
    glEndList();

    glNewList(startoflist + 1, GL_COMPILE);
        gluDisk(GoldCubeTop, 0, 15, 4, 1);
    glEndList();

    glNewList(startoflist + 2, GL_COMPILE);
        gluDisk(GoldCubeBottom, 0, 15, 4, 1);
    glEndList();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DrawRedPyramid()
{
    RedPyramid = gluNewQuadric();
    RedPyramidBottom = gluNewQuadric();

    glBindTexture(GL_TEXTURE_2D, texture2);
    gluQuadricTexture(RedPyramid, GL_TRUE);
    gluQuadricDrawStyle(RedPyramid, GLU_FILL);
    gluQuadricNormals(RedPyramid, GLU_SMOOTH);

    gluQuadricTexture(RedPyramidBottom, GL_TRUE);
    gluQuadricDrawStyle(RedPyramidBottom, GLU_FILL);
    gluQuadricNormals(RedPyramidBottom, GLU_SMOOTH);

    glNewList(startoflist + 3, GL_COMPILE);
        gluCylinder(RedPyramid, 16, 0, 35, 4, 10);
    glEndList();

    glNewList(startoflist + 4, GL_COMPILE);
        gluCylinder(RedPyramidBottom, 16, 0, 1, 4, 10);
    glEndList();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DrawBlueSphere()
{
    BlueSphere = gluNewQuadric();

    glBindTexture(GL_TEXTURE_2D, texture3);
    gluQuadricTexture(BlueSphere, GL_TRUE);
    gluQuadricDrawStyle(BlueSphere, GLU_FILL);
    gluQuadricNormals(BlueSphere, GLU_SMOOTH);
    glNewList(startoflist + 5, GL_COMPILE);
        gluSphere(BlueSphere, 15, 25, 25);
    glEndList();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::DrawRing()
{
    Ring = gluNewQuadric();
    glColor3f(1.0f, 1.0f, 1.0f);
    gluQuadricDrawStyle(Ring, GLU_FILL);
    gluQuadricNormals(Ring, GLU_SMOOTH);
    glNewList(startoflist + 6, GL_COMPILE);
        gluDisk(Ring, 24, 25, 50, 50);
    glEndList();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
static float pos = 0.0f;
    if(Key == VK_UP)
        //glRotatef(-5, 1.0, 0.0, 0.0);
        pos += 0.1f;
    if(Key == VK_DOWN)
        //glRotatef(5, 1.0, 0.0, 0.0);
        pos -= 0.1f;
    if(Key == VK_LEFT)
        glRotatef(-5, 0.0, 1.0, 0.0);
    if(Key == VK_RIGHT)
        glRotatef(5, 0.0, 1.0, 0.0);

    glTranslatef(0.0f, 0.0f, pos);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetupLighting()
{
    GLfloat MaterialAmbient[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat MaterialDiffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat MaterialSpecular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat MaterialShininess[] = {50.0};
	GLfloat AmbientLightPosition[] = {0.5, 1.0, 1.0, 0.0};
    GLfloat LightAmbient[] = {0.5, 0.5, 0.5, 1.0};

    glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
    glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);
    glLightfv(GL_LIGHT0, GL_POSITION, AmbientLightPosition);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightAmbient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SetupTextures()
{
    bitmap = new Graphics::TBitmap;
    bitmap->LoadFromFile("..\\..\\Images\\red.bmp");
    GLubyte bits[64][64][4];
    for(int i = 0; i < 64; i++)
    {
    	for(int j = 0; j < 64; j++)
        {
            bits[i][j][0]= (GLbyte)GetRValue(bitmap->Canvas->Pixels[i][j]);
            bits[i][j][1]= (GLbyte)GetGValue(bitmap->Canvas->Pixels[i][j]);
            bits[i][j][2]= (GLbyte)GetBValue(bitmap->Canvas->Pixels[i][j]);
            bits[i][j][3]= (GLbyte)255;
        }
	}
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);

	bitmap->LoadFromFile("..\\..\\Images\\gold.bmp");
	for(int i = 0; i < 64; i++)
    {
    	for(int j = 0; j < 64; j++)
        {
            bits[i][j][0]= (GLbyte)GetRValue(bitmap->Canvas->Pixels[i][j]);
            bits[i][j][1]= (GLbyte)GetGValue(bitmap->Canvas->Pixels[i][j]);
            bits[i][j][2]= (GLbyte)GetBValue(bitmap->Canvas->Pixels[i][j]);
            bits[i][j][3]= (GLbyte)255;
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);

	bitmap->LoadFromFile("..\\..\\Images\\blue.bmp");
	for(int i = 0; i < 64; i++)
    {
    	for(int j = 0; j < 64; j++)
        {
            bits[i][j][0]= (GLbyte)GetRValue(bitmap->Canvas->Pixels[i][j]);
            bits[i][j][1]= (GLbyte)GetGValue(bitmap->Canvas->Pixels[i][j]);
            bits[i][j][2]= (GLbyte)GetBValue(bitmap->Canvas->Pixels[i][j]);
            bits[i][j][3]= (GLbyte)255;
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, bits);

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}
//---------------------------------------------------------------------------

