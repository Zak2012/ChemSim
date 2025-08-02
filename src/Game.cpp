#include "Game.hpp"

#include <map>
#include <set>
#include <algorithm>
#include <chrono>
#include <thread>
#include <random>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <atomic>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GLSL_VER "#version 300 es\n//"
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLES3/gl32.h>
#else
#define GLEW_STATIC
#include <GL/glew.h>
#define GLSL_VER "#version 330 core\n//"
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/projection.hpp>

#include "Atoms.hpp"
#include "Group.hpp"
#include "Embed.hpp"
#include "embed/Res.rc"
#include "ColorConvert.hpp"

#define STR(X) #X

inline std::string GetStr(const std::vector<uint8_t> &Raw){return std::string(Raw.begin(), Raw.end());}

static std::mutex mtx;
static std::atomic_bool FlagDoneLoadPhysic = false;
static std::atomic_bool FlagDoneLoadGameCPU = false;
static std::atomic_bool FlagDoneLoadGame = false;
static std::atomic_bool FlagUpdateMoleculeCounter = true;
static std::atomic_bool FlagRunPhysics = true;
static std::atomic_bool FlagRunGame = true;

btDiscreteDynamicsWorld* dynamicsWorld;
const static int PhysicInterval = 20;
const static float MoleculeSpawnVel = 5.0f;
const static float MoleculeMaxVel = 10.0f;
static std::atomic<float> TimeScale = 1.0f;
static std::vector<Molecule *> MoleculesList;

static std::atomic_int Reactant1Tot = 0;
static std::atomic_int Reactant2Tot = 0;
static std::atomic_int Prod2Tot = 0;


void PhysicsSetup()
{
    mtx.lock();

	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, 0, 0));
    // mtx.unlock();

    // std::this_thread::sleep_for(std::chrono::milliseconds(5000));    
    // mtx.lock();

    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
    auto CreateGround = [&](btVector3 Pos)
    {
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(Pos.getX(), Pos.getY(), Pos.getZ()));

        btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(0.), myMotionState, groundShape, btVector3(0,0,0));
        btRigidBody* body = new btRigidBody(rbInfo);
        body->setUserPointer((void*)NULL);
        body->setFriction(0.0f);
        body->setRollingFriction(0.0f);
        body->setSpinningFriction(0.0f);
        body->setHitFraction(0.0f);
        body->setActivationState(DISABLE_DEACTIVATION);
        //add the body to the dynamics world
        dynamicsWorld->addRigidBody(body);
    };
    CreateGround(btVector3(0,-60,0));
    CreateGround(btVector3(0,60,0));
    CreateGround(btVector3(60,0,0));
    CreateGround(btVector3(-60,0,0));
    CreateGround(btVector3(0,0,60));
    CreateGround(btVector3(0,0,-60));
    mtx.unlock();
}

void PhysicsUpdate(float dt)
{
    std::map<Molecule*,std::set<Molecule*>> HitList;
    std::map<btRigidBody*,std::set<btRigidBody*>> AList;
    std::map<btRigidBody*,std::set<btRigidBody*>> BList;
    
    btDispatcher* dp = dynamicsWorld->getDispatcher();
    const int numManifolds = dp->getNumManifolds();
    for ( int m=0; m<numManifolds; m++ )
    {
        btPersistentManifold* man = dp->getManifoldByIndexInternal( m );
        btRigidBody* obA = (btRigidBody*)(man->getBody0());
        btRigidBody* obB = (btRigidBody*)(man->getBody1());
        
        Molecule *molA = (Molecule*)obA->getUserPointer();;
        Molecule *molB = (Molecule*)obB->getUserPointer();
        
        if (!obA->isActive() || !obB->isActive() || 
            (!molA) || (!molB) || (molA == molB) || 
            molA->GetAtoms().size() == 0|| molB->GetAtoms().size() == 0 ||
            molA->GetAtoms()[0].second == molB->GetAtoms()[0].second)
        {
            continue;
        }
        
        if (molA->GetAtoms()[0].second < molB->GetAtoms()[0].second)
        {
            std::swap(molA, molB);
            std::swap(obA, obB);
        }
        
        if (molB->GetAtoms().size() > 2)
        {
            continue;
        }
        
        AList[obA].insert(obB);
        BList[obB].insert(obA);
        HitList[molA].insert(molB);
    }

    
    for (auto x : HitList)
    {
        std::vector<Molecule*> NodeDelList;
        for (auto y : x.second)
        {
            std::vector<btRigidBody*> B = y->GetBodies();
            int CollisionCount = 0;
            
            for (unsigned int i = 0; i < B.size(); i++)
            {
                for (auto z : BList[B[i]])
                {
                    if((Molecule*)z->getUserPointer() != x.first)
                    {
                        CollisionCount++;
                    }
                    if (CollisionCount >= B.size())
                    {
                        break;
                    }
                }
                if (CollisionCount >= B.size())
                {
                    break;
                }
            }  
            
            if (CollisionCount >= B.size())
            {
                NodeDelList.push_back(y);
                continue;
            }
        }    
        for (auto z : NodeDelList)     
        {
            x.second.erase(z);
        }   
    }



    const std::vector<std::pair<int, Elements>> Chlorine = {{0,Cl_},{1,Cl_}};
    const std::vector<std::pair<int, Elements>> Hydrogen = {{0,H_},{1,H_}};
    
    std::vector<Molecule*> DeleteList;
    for (auto x : HitList)
    {
        if (x.first->GetStatic())
        {
            continue;
        }
        for (auto y : x.second)
        {
            if (y->GetStatic())
            {
                continue;
            }
            
            
            if (x.first->GetAtoms() == Chlorine && y->GetAtoms() == Hydrogen && x.second.size() == 1)
            {
                std::vector<btRigidBody*> BodiesA = x.first->GetBodies();
                std::vector<std::pair<int, Elements>> AtomsA = x.first->GetAtoms();
                
                std::vector<btRigidBody*> BodiesB = y->GetBodies();
                std::vector<std::pair<int, Elements>> AtomsB = y->GetAtoms();
                
                bool straight = AList[BodiesA[0]].contains(BodiesB[0]);
                int ind2 =  !straight;
                
                if (!(AList[BodiesA[0]].contains(BodiesB[ind2]) && AList[BodiesA[1]].contains(BodiesB[1-ind2])))
                {
                    continue;
                }
                // contruct new molecule
                std::vector<std::pair<int, Elements>> Atom1 = {{0,AtomsA[0].second}, {1,AtomsB[ind2].second}};
                std::vector<std::pair<int, Elements>> Atom2 = {{0,AtomsA[1].second}, {1,AtomsB[1-ind2].second}};
                
                std::vector<btRigidBody*> Bodies1 = {BodiesA[0], BodiesB[ind2]};
                std::vector<btRigidBody*> Bodies2 = {BodiesA[1], BodiesB[1-ind2]};

                // transfer atoms
                // BHandler->DelObject(x.first);
                fx_Group *Group = x.first->GetGroup();
                Group->DelObject(x.first);
                MoleculesList.erase(std::remove(MoleculesList.begin(), MoleculesList.end(), x.first), MoleculesList.end());
                
                Group->DelObject(y);
                MoleculesList.erase(std::remove(MoleculesList.begin(), MoleculesList.end(), y), MoleculesList.end());
                
                x.first->SetStatic(true);
                x.first->SetEnable(false);
                y->SetStatic(true);
                y->SetEnable(false);
                
                DeleteList.push_back(x.first);
                DeleteList.push_back(y);
                
                Molecule *Mol1 = new Molecule(Atom1, Bodies1);
                Molecule *Mol2 = new Molecule(Atom2, Bodies2);
                
                Group->AddObject(Mol1);
                MoleculesList.push_back(Mol1);
                
                Group->AddObject(Mol2);
                MoleculesList.push_back(Mol2);
                FlagUpdateMoleculeCounter = true;

                --Reactant1Tot;
                --Reactant2Tot;
                Prod2Tot += 2;
                
                break;
            }

        }
    }

    for (auto x : DeleteList)
    {
        delete x;
    }
}

void PhysicsLoop()
{
    PhysicsSetup();
    FlagDoneLoadPhysic = true;
    static auto LastFrame = std::chrono::high_resolution_clock::now();
    static float PhyDT = 0.0f;
    while (FlagRunPhysics.load())
    {
        auto start = std::chrono::high_resolution_clock::now();
        static float Accumulator = 0.0f;
        const float TimeStep = float(PhysicInterval)/1000.0f;
        Accumulator += PhyDT * TimeScale;
        while (Accumulator >= TimeStep) 
        {
            mtx.lock();
            dynamicsWorld->stepSimulation(TimeStep, 25);
            PhysicsUpdate(PhyDT * TimeScale);
            mtx.unlock();
            Accumulator -= TimeStep;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(PhysicInterval) - (std::chrono::high_resolution_clock::now() - start));
        PhyDT = (float)(std::chrono::duration_cast<std::chrono::milliseconds>(start - LastFrame).count())/1000.0f;
        LastFrame = start;
    }

    // physicsCommon.destroyPhysicsWorld(dynamicsWorld);

}

static float DeltaTime = glm::epsilon<float>();
static float GameRenderScale = 1.0f;
static float UIRenderScale = 1.0f;
const static int FPS = 60;
const static float FrameTime = 1.0f / (float)FPS;
// static float GameScale = 5.0f;

static GLFWwindow *MainWindow = NULL;
static std::vector<fx_Program*> Programs;
static glm::ivec2 WindowSize = {1280,720};
static float GameAspect = 16.0f/9.0f;
static glm::ivec2 ActualGameSize = {1280,720};
static glm::mat4 RenderMat = glm::identity<glm::mat4>();
static float PixelDensity = 1.0f;

static fx_Framebuffer *GameBuffer = NULL;
static fx_Framebuffer *UIBuffer = NULL;

static fx_Sprite *Renderer;
static fx_Sprite *UIRenderer;
static fx_Quad *Background;

static fx_Group *UIRender;
static fx_Group *GameRender;

static fx_Group *LoadGroup;

static fx_Group *Group1;
static fx_Group *UIGroup;

static fx_Image Icon;
static fx_Atlas UIAtlas;

static fx_Text *HNum;
static fx_Text *ClNum;
static fx_Text *HClNum;
static fx_Text *SpeedText;
static fx_TextBox *Box;
fx_Button *Button1;
fx_Button *Button2;
fx_Button *Button3;
fx_Button *Button4;
fx_Button *Button5;
fx_Button *Button6;
fx_Button *Button7;
fx_Button *Button8;

static std::default_random_engine Gen;
static std::uniform_real_distribution<float> Veldist(-1.0f, 1.0f);
static std::uniform_real_distribution<float> AngDist(0,2.0f * glm::pi<float>());

static std::atomic_bool FlagRender = true;
static std::atomic_bool FlagUpdateFramebuffer = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{   
    mtx.lock();
    WindowSize = {width,height};
    FlagUpdateFramebuffer = true;
    mtx.unlock();
    // UpdateWindows();
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (FlagDoneLoadGame.load())
    {
        UIGroup->GetWidgetHandler()->SetMouseDown(action == GLFW_PRESS);
    
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos); // on touch, emscripten doesn't emit mouse move event
        glm::vec2 ScreenMousePos;
        
        ScreenMousePos.x = (2.0f * ((xpos - ((float)(WindowSize.x - ActualGameSize.x)/2.0f))/(float)ActualGameSize.x)) - 1.0f;
        ScreenMousePos.y = (2.0f * (1.0f - (ypos - ((float)(WindowSize.y - ActualGameSize.y)/2.0f))/(float)ActualGameSize.y)) - 1.0f;

        UIGroup->SetMousePos(ScreenMousePos);
    }
}

void window_iconify_callback(GLFWwindow* window, int iconified)
{
    FlagRender = !iconified;
}

void UpdateWindows()
{
    float WindowAspect = ((float)WindowSize.x)/((float)WindowSize.y);

    glm::vec2 ViewPort = {WindowAspect, 1.0f};
    
    float Factor = 0.0f;
    if (WindowAspect >= GameAspect)
    {
        ActualGameSize.x = WindowSize.y * GameAspect;
        ActualGameSize.y = WindowSize.y;
        Factor = 1.0f / ViewPort.y;
    }
    else
    {
        ActualGameSize.y = WindowSize.x / GameAspect;
        ActualGameSize.x = WindowSize.x;
        Factor = GameAspect / ViewPort.x;
    }
    
    ViewPort = ViewPort * Factor;
    RenderMat = glm::ortho( -ViewPort.x, ViewPort.x, -ViewPort.y, ViewPort.y, 0.1f, 10.0f );
    if (FlagDoneLoadGame.load())
    {
        PixelDensity = (float)ActualGameSize.y / (UIGroup->m_Camera->GetSize() * 2.0f);
        Background->SetCube({ViewPort.x * 2.0f, ViewPort.y * 2.0f, 1.0f});
        
        GameBuffer->SetSize(glm::vec2(ActualGameSize) * GameRenderScale);
        UIBuffer->SetSize(glm::vec2(ActualGameSize) * UIRenderScale);
    }
}

void GameSetup()
{
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    glfwSetWindowSizeCallback(MainWindow, framebuffer_size_callback);
    glfwSetMouseButtonCallback(MainWindow, mouse_button_callback);
    glfwSetWindowIconifyCallback(MainWindow, window_iconify_callback);

    Programs.resize(4);
    Programs[fx_BasicType::Basic] = new fx_Program(std::vector<fx_Shader *>({
        new fx_Shader(GLSL_VER + GetStr(GetResource(IDR_BSVSDR)), "vert"), new fx_Shader(GLSL_VER + GetStr(GetResource(IDR_BSFSDR)), "frag")}));
    Programs[fx_BasicType::Sprite] = new fx_Program(std::vector<fx_Shader *>({
        new fx_Shader(GLSL_VER + GetStr(GetResource(IDR_SPVSDR)), "vert"), new fx_Shader(GLSL_VER + GetStr(GetResource(IDR_SPFSDR)), "frag")}));
    Programs[fx_BasicType::Circle] = new fx_Program(std::vector<fx_Shader *>({
        new fx_Shader(GLSL_VER + GetStr(GetResource(IDR_CRVSDR)), "vert"), new fx_Shader(GLSL_VER + GetStr(GetResource(IDR_CRFSDR)), "frag")}));
    Programs[fx_BasicType::SDF] = new fx_Program(std::vector<fx_Shader *>({
        new fx_Shader(GLSL_VER + GetStr(GetResource(IDR_TXVSDR)), "vert"), new fx_Shader(GLSL_VER + GetStr(GetResource(IDR_TXFSDR)), "frag")}));

    const std::vector<uint8_t> ChemsimFile = GetResource(IDR_PNGICON);
        
    Icon = fx_Image::LoadPNG(ChemsimFile);
    
    LoadGroup = new fx_Group(Programs, new fx_Texture(Icon));

    fx_Sprite *Loading = new fx_Sprite({0,0,-1}, {1.0f,1.0f}, {0.0,0.0,1.0,1.0});
    Loading->SetAnchor({0.5f,0.5f,0.0f});
    LoadGroup->AddObject(Loading);
    
    UpdateWindows();
        
    // Done Load essential for loading screen
}

void GameLoadCPU()
{
    GLFWimage images[1];
    images[0].width = Icon.Width;
    images[0].height = Icon.Height;
    images[0].pixels = Icon.Data.data();
    glfwSetWindowIcon(MainWindow, 1, images);
    
    // GameBuffer = new fx_Framebuffer();
    // GameBuffer->SetSize(glm::vec2(ActualGameSize) * GameRenderScale);
    GameRender = new fx_Group(Programs, NULL);

    Renderer = new fx_Sprite({0.0f,0.0f,-2.0f}, {2.0f, -2.0f}, {1.0f,0.0f,0.0f,1.0f});
    Renderer->SetAnchor({0.5f,0.5f,0.0f});
    Renderer->SetCube({GameAspect*-2.0f, -2.0f, 1.0f});
    GameRender->AddObject(Renderer);
    
    Background = new fx_Quad({0, 0.0f, -0.5f}, {2.0f, 2.0f});
    Background->SetAnchor({0.5f,0.5f,0.0f});
    GameRender->AddObject(Background);
    
    Background = new fx_Quad({0, 0.0f, -0.5f}, {2.0f, 2.0f});
    Background->SetAnchor({0.5f,0.5f,0.0f});
    GameRender->AddObject(Background);

    // UIBuffer = new fx_Framebuffer();
    // UIBuffer->SetSize(glm::vec2(ActualGameSize) * UIRenderScale);
    UIRender = new fx_Group(Programs, NULL);

    UIRenderer = new fx_Sprite({0.0f,0.0f,-2.0f}, {2.0f, -2.0f}, {1.0f,0.0f,0.0f,1.0f});
    Renderer->SetAnchor({0.5f,0.5f,0.0f});
    UIRenderer->SetCube({GameAspect*-2.0f, -2.0f, 1.0f});
    UIRender->AddObject(UIRenderer);
    
    
    Group1 = new fx_Group(Programs, NULL);
    Group1->m_Camera = new fx_Perspective ({0.0,0.0,10}, GameAspect);
    Group1->m_Camera->SetFar(50.0f);
    // Group1->m_FrameBuffer = GameBuffer;

    UIGroup = new fx_Group(Programs, NULL);
    UIGroup->m_Camera = new fx_Orthographic ({0.0,0.0,2.5}, GameAspect);
    UIGroup->m_Camera->SetSize(2.5f);
    // UIGroup->m_FrameBuffer = UIBuffer;

    
    
    const std::vector<uint8_t> FontFile = GetResource(IDR_FONT);

    fx_Font *FontObj = new fx_Font(FontFile);
    FontObj->m_Atlas = &UIAtlas;

    FontObj->RenderFont();
    // std::vector<fx_Image> UIImageList = FontObj->GetAtlas().ImagesList;
    const std::vector<unsigned int> ImgRes = {
        IDR_ICUP, IDR_ICDN, IDR_ICLF, IDR_ICRG, IDR_IC360, IDR_ICADC, IDR_ICADD, IDR_ICCLOSE, 
        IDR_ICRESET,IDR_ICTEMP, IDR_ICIN, IDR_ICOUT, IDR_ICLSC, IDR_ICSPEED};

    for (auto x : ImgRes)
    {
        UIAtlas.ImagesList["Res_" + std::to_string(x)].Image = fx_Image::LoadPNG(GetResource(x));
    }

    for (auto &x : UIAtlas.ImagesList)
    {
        if (x.second.Image.Component == 0)
        {
            x.second.Image.Component = 4;
        }
        if (x.second.Image.Component == 1)
        {
            x.second.Image.Data = ColorConvert::Gray2RGBA(x.second.Image.Data);
            x.second.Image.Component = 4;
        }
        else if (x.second.Image.Component == 2)
        {
            x.second.Image.Data = ColorConvert::GrayA2RGBA(x.second.Image.Data);
            x.second.Image.Component = 4;
        }
        else if (x.second.Image.Component == 3)
        {
            x.second.Image.Data = ColorConvert::RGB2RGBA(x.second.Image.Data);
            x.second.Image.Component = 4;
        }
        else if (x.second.Image.Component == 4)
        {
            
        }
        else
        {
            std::cout << "Unknown Type\n";
        }
    }

    fx_Atlas::PackImages(UIAtlas);
    
    glm::vec4 BDefault = {0.36,0.36,0.36,1};;
    glm::vec4 BDown = {0.3,0.3,0.3,1};
    glm::vec4 BDisable = {0.16,0.16,0.16,1};


    Button1 = new fx_Button({0.69,2.25,-1}, {0.5f,0.5f}, UIAtlas.ImagesList["Res_" + std::to_string(IDR_IC360)].UV, BDefault, BDown, BDisable, {1,1,1,1});
    Button2 = new fx_Button({1.19,2.25,-1}, {0.5f,0.5f}, UIAtlas.ImagesList["Res_" + std::to_string(IDR_ICIN)].UV, BDefault, BDown, BDisable, {1,1,1,1});
    Button3 = new fx_Button({1.69,2.25,-1}, {0.5f,0.5f}, UIAtlas.ImagesList["Res_" + std::to_string(IDR_ICOUT)].UV, BDefault, BDown, BDisable, {1,1,1,1});
    Button4 = new fx_Button({2.69,2.25,-1}, {0.5f,0.5f}, UIAtlas.ImagesList["Res_" + std::to_string(IDR_ICADC)].UV, BDefault, BDown, BDisable, {1,1,1,1});
    Button5 = new fx_Button({3.19,2.25,-1}, {0.5f,0.5f}, UIAtlas.ImagesList["Res_" + std::to_string(IDR_ICADD)].UV, BDefault, BDown, BDisable, {1,1,1,1});
    Button6 = new fx_Button({3.69,2.25,-1}, {0.5f,0.5f}, UIAtlas.ImagesList["Res_" + std::to_string(IDR_ICRESET)].UV, BDefault, BDown, BDisable, {1,1,1,1});
    Button7 = new fx_Button({2.19,2.25,-1}, {0.5f,0.5f}, UIAtlas.ImagesList["Res_" + std::to_string(IDR_ICTEMP)].UV, BDefault, BDown, BDisable, {1,1,1,1});
    Button8 = new fx_Button({4.19,2.25,-1}, {0.5f,0.5f}, UIAtlas.ImagesList["Res_" + std::to_string(IDR_ICLSC)].UV, BDefault, BDown, BDisable, {1,1,1,1});

    UIGroup->AddObject(Button1);
    UIGroup->AddObject(Button2);
    UIGroup->AddObject(Button3);
    UIGroup->AddObject(Button4);
    UIGroup->AddObject(Button5);
    UIGroup->AddObject(Button6);
    UIGroup->AddObject(Button7);
    UIGroup->AddObject(Button8);
    // 5.0f, 1.0f, 0.0f, FontObj, "ABCDEFGHIJKLM\n aaa aaa aaa bbb OPQRSTUVWXYZ aaa"
    Box = new fx_TextBox({0,0,-1}, 0.25f, 8.0f, FontObj, GetStr(GetResource(IDR_ITXTNT)),{1,1,1,1}, {.5,.5,.5,1});
    Box->SetAnchor({0.5f,0.5f,0.0f});
    Box->SetLineSpacing(0.7f);
    Box->SetEnable(false);
    // Box->SetAlign(0.5f);
    UIGroup->AddObject(Box);

    ModelMolecule *HModel = new ModelMolecule({{0, H_}, {1, H_}},{-4.25f,2.25f,0.0f},0.2f);
    UIGroup->AddObject(HModel);

    HNum = new fx_Text({-3.9f,2.25f,0.0f}, 0.3f, FontObj, "888");
    HNum->SetAnchor({0.0f,0.5f,0.0f});
    UIGroup->AddObject(HNum);

    ModelMolecule *ClModel = new ModelMolecule({{0, Cl_}, {1, Cl_}},{-3.25f,2.25f,0.0f},0.2f);
    UIGroup->AddObject(ClModel);

    ClNum = new fx_Text({-2.9f,2.25f,0.0f}, 0.3f, FontObj, "888");
    ClNum->SetAnchor({0.0f,0.5f,0.0f});
    UIGroup->AddObject(ClNum);

    ModelMolecule *HClModel = new ModelMolecule({{0, Cl_}, {1, H_}},{-2.25f,2.25f,0.0f},0.2f);
    UIGroup->AddObject(HClModel);

    HClNum = new fx_Text({-1.9f,2.25f,0.0f}, 0.3f, FontObj, "888");
    HClNum->SetAnchor({0.0f,0.5f,0.0f});
    UIGroup->AddObject(HClNum);

    fx_Sprite *SpeedPic = new fx_Sprite({-1.25f,2.25f,0.0f}, {0.3f,0.3f}, UIAtlas.ImagesList["Res_" + std::to_string(IDR_ICSPEED)].UV);
    UIGroup->AddObject(SpeedPic);

    SpeedText = new fx_Text({-1.1f,2.25f,0.0f}, 0.3f, FontObj, "888");
    SpeedText->SetAnchor({0.0f,0.5f,0.0f});
    UIGroup->AddObject(SpeedText);
    
    // BHandler->SetCameraPos(ObjCam.GetPosition());
    // BHandler->SetCameraUp(glm::vec3(0,1,0) * ObjCam.GetQuat());
    // std::cout << "b\n";

    // float Angle = 0;

    // ObjCam.SetFar(50.0f);

    // fx_TextBox *Box1 = new fx_TextBox({0,0,-1}, 2.0f, 10.0f, FontObj, "ABC",{1,1,1,1}, {.5,.5,.5,1});
    // Box1->SetAnchor({0.5f,0.5f,0.0f});
    // Box1->SetLineSpacing(0.7f);
    // Box1->SetAlign(0.5f);
    // UIGroup->AddObject(Box1);

    // ObjCam.SetSize(1.0f);
    // HClNum->SetPixelDensity((float)ActualGameSize.y / (UIGroup->m_Camera->GetSize() * 2.0f));


    Button1->m_MainActionCallback = [&]() {
        glm::vec3 CamPos = Group1->m_Camera->GetPosition();
        float Angle = std::atan2(CamPos.x, CamPos.z);
        Angle += glm::pi<float>() * 2.0f * DeltaTime;

        float CamLenght = glm::length(CamPos);

        CamPos.x = std::sin(Angle) * CamLenght;
        CamPos.y = 0;
        CamPos.z = std::cos(Angle) * CamLenght;

        Group1->m_Camera->SetPosition(CamPos);
        Group1->m_Camera->SetQuat(glm::quat(glm::vec3(0.0f,-Angle,0.0f)));
    };
    
    Button1->m_HoldActionCallback = Button1->m_MainActionCallback ;

    Button2->m_MainActionCallback = [&]() {
        glm::vec3 CamPos = Group1->m_Camera->GetPosition();
        float Angle = std::atan2(CamPos.x, CamPos.z);

        float CamLenght = glm::length(CamPos) - (glm::pi<float>() * 5.0f * DeltaTime);

        CamPos.x = std::sin(Angle) * CamLenght;
        CamPos.y = 0;
        CamPos.z = std::cos(Angle) * CamLenght;

        Group1->m_Camera->SetPosition(CamPos);
    };
    Button2->m_HoldActionCallback = Button2->m_MainActionCallback ;


    Button3->m_MainActionCallback = [&]() {
        glm::vec3 CamPos = Group1->m_Camera->GetPosition();
        float Angle = std::atan2(CamPos.x, CamPos.z);

        float CamLenght = glm::length(CamPos) + (glm::pi<float>() * 5.0f * DeltaTime);

        CamPos.x = std::sin(Angle) * CamLenght;
        CamPos.y = 0;
        CamPos.z = std::cos(Angle) * CamLenght;

        Group1->m_Camera->SetPosition(CamPos);
    };
    Button3->m_HoldActionCallback = Button3->m_MainActionCallback ;

    Button4->m_MainActionCallback = [&]() {
        ++Reactant1Tot;
        Molecule *M1 = new Molecule({{0, H_}, {1, H_}},{8.0f,5.0f,0.0f});
        M1->SetQuat(glm::quat(glm::vec3(AngDist(Gen), AngDist(Gen), AngDist(Gen))));
        M1->SetVelocity(glm::normalize(glm::vec3(-std::abs(Veldist(Gen)), Veldist(Gen), Veldist(Gen))) * MoleculeSpawnVel);
        Group1->AddObject(M1);
        MoleculesList.push_back(M1);
    };
    Button4->m_HoldActionCallback = [&]() {

        static float Accumulator = 0.0f;
        const float TimeStep = 0.2;
        Accumulator += DeltaTime;

        while(Accumulator >= TimeStep)
        {
            Button4->m_MainActionCallback();
            Accumulator -= TimeStep;
        }
    };


    Button5->m_MainActionCallback = [&]() {
        Reactant2Tot++;
        Molecule *M1 = new Molecule({{0, Cl_}, {1, Cl_}},{-8.0f,5.0f,0.0f});
        M1->SetQuat(glm::quat(glm::vec3(AngDist(Gen), AngDist(Gen), AngDist(Gen))));
        M1->SetVelocity(glm::normalize(glm::vec3(std::abs(Veldist(Gen)), Veldist(Gen), Veldist(Gen))) * MoleculeSpawnVel);
        Group1->AddObject(M1);
        MoleculesList.push_back(M1);
    };
    Button5->m_HoldActionCallback = [&]() {

        static float Accumulator = 0.0f;
        const float TimeStep = 0.2;
        Accumulator += DeltaTime;

        while(Accumulator >= TimeStep)
        {
            Button5->m_MainActionCallback();
            Accumulator -= TimeStep;
        }
    };

    Button6->m_MainActionCallback = [&]() {
        Reactant1Tot = 0;
        Reactant2Tot = 0;
        Prod2Tot = 0;
        for (auto x : MoleculesList)
        {
            Group1->DelObject(x);
            delete x;

        }
        MoleculesList.clear();
        Group1->Update();
    };

    Button7->m_MainActionCallback = [&]() {
        for (auto x : MoleculesList)
        {
            for (auto y : x->GetBodies())
            {
                glm::vec3 v = v3bt2glm(y->getLinearVelocity());
                glm::vec3 dir = glm::normalize(v);
                float mag = glm::length(v);
                y->setLinearVelocity(v3glm2bt(dir * std::min(mag*1.5f, MoleculeMaxVel)));
            }
        }
    };
    Button7->m_HoldActionCallback = Button7->m_MainActionCallback;

    Button8->m_MainActionCallback = [&]() {
        bool status = Box->GetEnable();
        Box->SetEnable(!status);
        TimeScale = status;
        Button1->SetWidgetEnable(status);
        Button2->SetWidgetEnable(status);
        Button3->SetWidgetEnable(status);
        Button4->SetWidgetEnable(status);
        Button5->SetWidgetEnable(status);
        Button6->SetWidgetEnable(status);
        Button7->SetWidgetEnable(status);
    };

    
    
    // wait until physics is finish
    // FlagDoneLoadPhysic = true;
    while (!FlagDoneLoadPhysic.load())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    std::cout << "Scene done loading\n";
    FlagDoneLoadGameCPU = true;
}

void GameLoadGPU()
{
    GameBuffer = new fx_Framebuffer();
    GameRender->m_TextureUnit = GameBuffer->GetColorAttachment();
    Group1->m_FrameBuffer = GameBuffer;
    Group1->m_TextureUnit = NULL;


    UIBuffer = new fx_Framebuffer();
    UIRender->m_TextureUnit = UIBuffer->GetColorAttachment();
    UIGroup->m_FrameBuffer = UIBuffer;

    UIGroup->m_TextureUnit = new fx_Texture(UIAtlas.Image);

    FlagDoneLoadGame = true;
    UpdateWindows();
}

void GameUpdate(float dt)
{
    if (FlagRender.load())
    {
        if (FlagUpdateFramebuffer.load())
        {
            UpdateWindows();
            FlagUpdateFramebuffer = false;
        }
        if (FlagDoneLoadGame.load())
        {
            mtx.lock();

            static uint8_t SpeedCountdown = 0;
            if (SpeedCountdown == 0)
            {
                if (MoleculesList.size() == 0)
                {
                    SpeedText->SetText(std::to_string(0.0f));  
                }
                else
                {
                    double Vrms;
                    int BodyCount = 0;
                    for (auto x : MoleculesList)
                    {
                        std::vector<glm::vec3> Vel = x->GetVelocity();
                        BodyCount += Vel.size();
                        for (auto y : Vel)
                        {
                            Vrms += std::pow(glm::length(y),2);
                        }
                    }
                    Vrms = std::sqrt(Vrms * (1.0f / BodyCount));
                    SpeedText->SetText(std::to_string((float)Vrms));
                }
            }
            SpeedCountdown++;
            SpeedCountdown %= 20;

            if (FlagUpdateMoleculeCounter)
            {
                HNum->SetText(std::to_string(Reactant1Tot));
                ClNum->SetText(std::to_string(Reactant2Tot));
                HClNum->SetText(std::to_string(Prod2Tot));
            }
            
            for (auto x : MoleculesList)
            {
                x->Physic();
            }
            GameRender->Update();
            UIRender->Update();
            Group1->Update();
            UIGroup->Update();
            mtx.unlock();
        
            Group1->m_FrameBuffer->ResetBuffer();
            UIGroup->m_FrameBuffer->ResetBuffer();
        
            glEnable(GL_DEPTH_TEST);
            Group1->Draw();
            UIGroup->Draw();
        }
        else
        {
            LoadGroup->Update();
        }
        
        for (auto x : Programs)
        {
            x->SetUniform(RenderMat, "Matrix");
        }
        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, WindowSize.x, WindowSize.y);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        if (FlagDoneLoadGame.load())
        {
            GameRender->Draw();
            UIRender->Draw();
        }
        else
        {
            LoadGroup->Draw();
        }
        glfwSwapBuffers(MainWindow);
    }
}


void GameLoop()
{
    glfwMakeContextCurrent(MainWindow);
    GameSetup();
    std::thread Load(GameLoadCPU);
    // Load.detach();
    // FlagDoneLoadGame = true;
    static auto LastFrame = std::chrono::high_resolution_clock::now();
    while (FlagRunGame.load())
    {
        auto start = std::chrono::high_resolution_clock::now();
        if (FlagDoneLoadGameCPU.load() && !FlagDoneLoadGame.load())
        {
            if (Load.joinable())
            {
                Load.join();
            }
            GameLoadGPU();
        }
        GameUpdate(DeltaTime);
        std::this_thread::sleep_for(std::chrono::milliseconds(int(FrameTime*1000.0f)) - (std::chrono::high_resolution_clock::now() - start));
        DeltaTime = (float)(std::chrono::duration_cast<std::chrono::milliseconds>(start - LastFrame).count())/1000.0f;
        LastFrame = start;
    }
    glfwMakeContextCurrent(NULL);
}

static std::thread Game;
static std::thread Phys;

void GameInit(void *Window)
{
    MainWindow = (GLFWwindow*)Window;

    Game = std::thread(GameLoop);
    Phys = std::thread(PhysicsLoop);
}

void GameExit()
{
    FlagRunGame = false;
    FlagRunPhysics = false;
    if (Game.joinable())
    {
        Game.join();
    }
    if (Phys.joinable())
    {
        Phys.join();
    }
}