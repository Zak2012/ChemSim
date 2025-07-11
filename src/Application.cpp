#define GLEW_STATIC

// Include standard libraries
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <sstream>
#include <set>
#include <map>
#include <mutex>
#include <random>
// #include <memory>

// #define __EMSCRIPTEN__

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GLSL_VER "#version 300 es\n//"
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLES3/gl32.h>
#else
#include <GL/glew.h>
#define GLSL_VER "#version 330 core\n//"
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
// #include <base64.hpp>
// #include <reactphysics3d/reactphysics3d.h>  
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>

// #include "File.hpp"
#include "Shader.hpp"
#include "Object.hpp"
#include "Resource.hpp"
#include "ColorConvert.hpp"
#include "Font.hpp"
#include "Embed.hpp"
// #include "Time.hpp"
#include "Physics.hpp"
// #include "Game.hpp"
#include "Widget.hpp"
#include "Atoms.hpp"

// #include "ARIAL.ttf.h"
// #include "Chemsim.png.h"


// static const std::string Arialb64 =
// {
//     #include "arial.b64.txt"
// };
// static const std::string Chemsimb64 =
// {
//     #include "chemsim.b64.txt"
// };
// static const std::string DecodeArial = base64::from_base64(Arialb64);
// static const std::string DecodeChemsim = base64::from_base64(Chemsimb64);
// static const std::vector<uint8_t> ArialFile(DecodeArial.begin(), DecodeArial.end());
// static const std::vector<uint8_t> ChemsimFile(DecodeChemsim.begin(), DecodeChemsim.end());


// static reactphysics3d::PhysicsCommon physicsCommon;

#include "embed/Res.rc"

// static GLenum ErrorCode;
// static const GLubyte *ErrorString;

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}


#define EraseElement(x,y)\
{\
    auto it = std::find(x.begin(), x.end(), y); \
    if (it != x.end()) { x.erase(it); } \
}\





static GLFWwindow *MainWindow;
static float DeltaTime = glm::epsilon<float>();
// static int FPS = 30;
// static int FrameTimems = int((1.0f/float(FPS))*100.0f);
static glm::ivec2 WindowSize = {1280,720};
// static glm::ivec2 GameSize = {1280,720};
// static glm::ivec2 UISize = {1280,720};
static float GameAspect = 16.0f/9.0f;
static glm::ivec2 ActualGameSize = {1280,720};
static float GameRenderScale = 2.0f;
static float UIRenderScale = 2.0f;
// static float GameScale = 5.0f;
// static float TimeScale = 0.25f;
const static int FPS = 60;
const static float FrameTime = 1.0f / (float)FPS;

static std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> RenderDemandsStack;

static bool RenderDemand;

static std::vector<fx_Program*> Programs;
// static fx_Font_Library Lib;

// static fx_Face TimesFace;
// static fx_Face ArialFace;

static fx_Framebuffer *GameBuffer;
static fx_Framebuffer *UIBuffer;

static fx_Group *Group1;
// static fx_Group *Group2;
static fx_Group *UIGroup;

static fx_Group *UIRender;
static fx_Group *GameRender;

static fx_Sprite *Renderer;
static fx_Sprite *UIRenderer;
static fx_Quad *Background;

// static glm::mat4 LookAtMat;
// static glm::mat4 GameLookAtMat;
// static glm::mat4 UILookAtMat;
static glm::mat4 RenderLookAtMat;

// static bool Nostep = false;

// static int Reactant1Tot = 0;
// static int Reactant2Tot = 0;

// static fx_BillboardCircle *Circle1;
// static fx_BillboardCircle *Circle2;
// static fx_BillboardCircle *Circle3;
// static fx_BillboardCircle *Circle4;
// static fx_BillboardCircle *Circle5;
// static fx_BillboardCircle *Circle6;
// static fx_BillboardCircle *Circle7;
// static fx_BillboardCircle *Circle8;
// static fx_BillboardCircle *Circle9;

// static fx_BillboardLine *Line1;

// static fx_Text *Text;

static fx_Perspective ObjCam({0.0,0.0,10}, GameAspect);
static fx_Orthographic UICam({0.0,0.0,2.5}, GameAspect);

static Line3D MousePos;

static fx_WidgetHandler *WHandler;
static fx_BillboardHandler *BHandler;

btDiscreteDynamicsWorld* dynamicsWorld;

static std::vector<Molecule *> MoleculesList;
static std::mutex mtx;
// static std::set<std::pair<Molecule*,Molecule*>> CollideList;
// static std::vector<std::pair<btRigidBody*,btRigidBody*>> CollideList;
static const int PhysicInterval = 20;
static bool RunPhysics = true;

const static float MoleculeSpawnVel = 5.0f;

static std::default_random_engine Gen;
static std::uniform_real_distribution<float> Veldist(-1.0f, 1.0f);
static std::uniform_real_distribution<float> AngDist(0,2.0f * glm::pi<float>());

void PhysicsUpdate(float dt);
void PhysicsLoop()
{
    static auto LastFrame = std::chrono::high_resolution_clock::now();
    static float PhyDT = 0.0f;

    while (RunPhysics)
    {
        auto start = std::chrono::high_resolution_clock::now();
        mtx.lock();
        PhysicsUpdate(PhyDT);
        mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(PhysicInterval) - (std::chrono::high_resolution_clock::now() - start));
    
        PhyDT = (float)(std::chrono::duration_cast<std::chrono::milliseconds>(start - LastFrame).count())/1000.0f;
        LastFrame = start;

    }
}

void PhysicsUpdate(float dt)
{
    
    static float Accumulator = 0.0f;
    const float TimeStep = float(PhysicInterval)/1000.0f;
    // Add the time difference in the accumulator
    Accumulator += dt;
    
    // While there is enough accumulated time to take
    // one or several physics steps
    while (Accumulator >= TimeStep) 
    {
        // CollideList.clear();
        std::map<Molecule*,std::set<Molecule*>> HitList;
        std::map<btRigidBody*,std::set<btRigidBody*>> AList;
        std::map<btRigidBody*,std::set<btRigidBody*>> BList;
        
        dynamicsWorld->stepSimulation(TimeStep, 2);
        
        btDispatcher* dp = dynamicsWorld->getDispatcher();
        const int numManifolds = dp->getNumManifolds();
        for ( int m=0; m<numManifolds; m++ )
        {
            btPersistentManifold* man = dp->getManifoldByIndexInternal( m );
            btRigidBody* obA = (btRigidBody*)(man->getBody0());
            btRigidBody* obB = (btRigidBody*)(man->getBody1());
            
            Molecule *molA = (Molecule*)obA->getUserPointer();;
            Molecule *molB = (Molecule*)obB->getUserPointer();
            
            if (!obA->isActive())
            {
                continue;
            }
            
            if (!obB->isActive())
            {
                continue;
            }
            
            if ((!molA) || (!molB) || (molA == molB))
            {
                continue;
            }
            
            
            if (molA->GetAtoms().size() == 0|| molB->GetAtoms().size() == 0)
            {
                continue;
            }
            
            if (molA->GetAtoms()[0].second == molB->GetAtoms()[0].second)
            {
                continue;
            }
            
            if (molA->GetAtoms()[0].second < molB->GetAtoms()[0].second)
            {
                Molecule *T = molB;
                molB = molA;
                molA = T;
                
                btRigidBody *G = obB;
                obB = obA;
                obA = G;
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
            if (x.first->GetTransfer())
            {
                continue;
            }
            for (auto y : x.second)
            {
                if (y->GetTransfer())
                {
                    continue;
                }
                
                
                if (x.first->GetAtoms() == Chlorine && y->GetAtoms() == Hydrogen && x.second.size() == 1)
                {
                    std::vector<btRigidBody*> BodiesA = x.first->GetBodies();
                    std::vector<std::pair<int, Elements>> AtomsA = x.first->GetAtoms();
                    
                    std::vector<btRigidBody*> BodiesB = y->GetBodies();
                    std::vector<std::pair<int, Elements>> AtomsB = y->GetAtoms();
                    
                    // float dist = glm::length(v3bt2glm(BodiesA[0]->getWorldTransform().getOrigin()) - v3bt2glm(BodiesB[0]->getWorldTransform().getOrigin()));
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
                    BHandler->DelObject(x.first);
                    Group1->DelObject(x.first);
                    MoleculesList.erase(std::remove(MoleculesList.begin(), MoleculesList.end(), x.first), MoleculesList.end());
                    
                    BHandler->DelObject(y);
                    Group1->DelObject(y);
                    MoleculesList.erase(std::remove(MoleculesList.begin(), MoleculesList.end(), y), MoleculesList.end());
                    
                    x.first->SetTransfer(true);
                    x.first->SetEnable(false);
                    y->SetTransfer(true);
                    y->SetEnable(false);
                    
                    DeleteList.push_back(x.first);
                    DeleteList.push_back(y);
                    
                    Molecule *Mol1 = new Molecule(Atom1, Bodies1);
                    Molecule *Mol2 = new Molecule(Atom2, Bodies2);
                    
                    BHandler->AddObject(Mol1);
                    Group1->AddObject(Mol1);
                    MoleculesList.push_back(Mol1);
                    
                    BHandler->AddObject(Mol2);
                    Group1->AddObject(Mol2);
                    MoleculesList.push_back(Mol2);
                    
                    break;
                }

            }
        }

        for (auto x : DeleteList)
        {
            delete x;
        }
        Accumulator -= TimeStep;

    }
}

void RenderLoop();

void update(float dt)
{
    
    // dt = 0.16;
    dt += glm::epsilon<float>();
    auto UpdateStart = std::chrono::high_resolution_clock::now();

    mtx.lock();

    for (auto x : MoleculesList)
    {
        x->Physic();
    }
    // PhysicsUpdate(dt);
    auto PhysicTime = std::chrono::high_resolution_clock::now();
    

    // float camX = sin(glfwGetTime()) * 5.0f;
    // float camZ = cos(glfwGetTime()) * 5.0f;
    // glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  5.0f * sin(glfwGetTime()));
    // ObjCam.SetPosition(cameraPos);
    // glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    // glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
    // glm::mat4 view;
    // view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    // LookAtMat = glm::perspective(0.5f * glm::radians(180.0f),GameAspect, 0.1f, 10.0f) * view;
    
    // IMPORTANT: run in another thread to ensure continous
    // AtomsObj[0]->m_body->SetLinearVelocity(b2Vec2(0.0f, -10.0f));
    // if (!Nostep)
    // {
        //     world->Step(1.0f/60.0f * TimeScale, 6, 5);
        // }
        
        // Collision respond
        // Reaction(dt);
        
        
        // std::cout << MousePos.x << "," << MousePos.y << "\n";
        // std::cout << Mouse.Start.x << "," << Mouse.Start.y << "\n";
        // for (auto &x: AtomsObj)
        // {
            //     x->Update();
            // }
            
            
            
            // Atom1->Update();
            // Atom2->Update();
            // Bond1->Update();
            // Text->SetText(std::to_string(dt));
            
            BHandler->Update();
            auto BTime = std::chrono::high_resolution_clock::now();
            WHandler->Update();
            auto WTime = std::chrono::high_resolution_clock::now();
            Group1->Update();
            auto GTime = std::chrono::high_resolution_clock::now();
            UIGroup->Update();
            auto UTime = std::chrono::high_resolution_clock::now();

        mtx.unlock();


            auto UpdateTime = std::chrono::high_resolution_clock::now();

            
            
            
            RenderDemand = true;
            if ((RenderDemandsStack.size() > 0 || RenderDemand)
            && WindowSize.x * WindowSize.y != 0) // draws
            {
                // std::cout << DeltaTime*1000.0f << "ms \n";
                // GUI->Update(WindowSize.x, WindowSize.y, GameAspect);
                RenderDemand = false;
                // for (auto x : Programs)
                // {
                    //     x->SetUniform(LookAtMat, "Matrix");
                    // }
                    // glViewport(0, 0, WindowSize.x, WindowSize.y);
                    // for (auto x : Programs)
                    // {
                        //     x->SetUniform(0.0f, "Flat");
                        // }
                        glEnable(GL_DEPTH_TEST);
                        GameBuffer->Bind();
                        glViewport(0, 0, (ActualGameSize.x * GameRenderScale), (ActualGameSize.y * GameRenderScale));
            glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glClearColor(0.0f,0.0f,0.0f,0.0f);
            // std::cout << "gp1 : ";
            Group1->Draw();
            // UIGroup->Draw();
            // Group2->Draw();
            GameBuffer->Unbind();
            // for (auto x : Programs)
            // {
                //     x->SetUniform(1.0f, "Flat");
                // }
                UIBuffer->Bind();
                // glViewport(0, 0, (ActualGameSize.x * GameRenderScale), (ActualGameSize. * GameRenderScale));
                glViewport(0, 0, (ActualGameSize.x * UIRenderScale), (ActualGameSize.y * UIRenderScale));
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                glClearColor(0.0f,0.0f,0.0f,0.0f);
                UIGroup->Draw();
                // glReadPixels(0, 0, Actual(ActualGameSize.x * GameRenderScale), Actual(ActualGameSize. * GameRenderScale), GL_RGBA, GL_UNSIGNED_BYTE, (void *)RenderImage.Data.data());
                UIBuffer->Unbind();
                for (auto x : Programs)
                {
                    x->SetUniform(RenderLookAtMat, "Matrix");
                }
                glViewport(0, 0, WindowSize.x, WindowSize.y);
                glDisable(GL_DEPTH_TEST);
                glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f,0.0f,0.0f,1.0f);
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        // UITexture->Bind();
        // glGetnTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, RenderImage.Data.size(),(void *)RenderImage.Data.data());
        // UITexture->Unbind();
        GameRender->Draw();
        UIRender->Draw();
        // UIRender->GenerateMesh();
        // UIRender->Update();
        
        glfwSwapBuffers(MainWindow);
    }
    
    
    
    auto it = RenderDemandsStack.begin();
    while (it != RenderDemandsStack.end())
    {
        if (UpdateStart >= *it)
        {
            it = RenderDemandsStack.erase(it);
        }
        else
        {
            it++;
        }
    }

    auto RenderTime = std::chrono::high_resolution_clock::now();

    int Phy = std::chrono::duration_cast<std::chrono::milliseconds>(PhysicTime - UpdateStart).count();
    int Upt = std::chrono::duration_cast<std::chrono::milliseconds>(UpdateTime - PhysicTime).count();
    int Rnd = std::chrono::duration_cast<std::chrono::milliseconds>(RenderTime - UpdateTime).count();

    int Bt = std::chrono::duration_cast<std::chrono::milliseconds>(BTime - PhysicTime).count();
    int Wt = std::chrono::duration_cast<std::chrono::milliseconds>(WTime - BTime).count();
    int Gt = std::chrono::duration_cast<std::chrono::milliseconds>(GTime - WTime).count();
    int Ut = std::chrono::duration_cast<std::chrono::milliseconds>(UTime - GTime).count();



    glfwSetWindowTitle(MainWindow, std::string(std::to_string(Phy) + " " + std::to_string(Upt) + " " + std::to_string(Rnd) + " " + std::to_string(Bt) + " " + std::to_string(Wt) + " " + std::to_string(Gt) + " " + std::to_string(Ut) + " " + std::to_string(MoleculesList.size()) + " " + std::to_string(1.0f/dt)).c_str());
}

void UpdateWindows()
{
    float WindowAspect = (float)WindowSize.x/(float)WindowSize.y;
    glm::vec2 CamOffset = {(float)(WindowSize.x%2), (float)(WindowSize.y%2)};
    
    // float TotalRatio = WindowAspect / GameAspect;

    glm::vec2 GameOffset = {0.0f, 0.0f};

    GameOffset.x = ((std::max(WindowAspect - GameAspect, 0.0f) / 2.0f) * WindowSize.y);
    GameOffset.y = ((std::max((GameAspect/WindowAspect) - 1.0f, 0.0f) / 2.0f) * WindowSize.x);
    
    GameOffset += CamOffset;
    
    
    UIRenderer->SetCube({GameAspect*-2.0f, -2.0f, 1.0f});
    Renderer->SetCube({GameAspect*-2.0f, -2.0f, 1.0f});

    if (WindowAspect >= GameAspect)
    {
        ActualGameSize.x = WindowSize.y * GameAspect;
        ActualGameSize.y = WindowSize.y;
        
        RenderLookAtMat = glm::ortho( -WindowAspect, WindowAspect, -1.0f, 1.0f, 0.1f, 10.0f );
        Background->SetCube({WindowAspect*2.0f, -2.0f, 1.0f});
    }
    else
    {
        ActualGameSize.y = WindowSize.x / GameAspect;
        ActualGameSize.x = WindowSize.x;
        
        RenderLookAtMat = glm::ortho( -GameAspect, GameAspect, -GameAspect/WindowAspect, GameAspect/WindowAspect, 0.1f, 10.0f );
        // UIRenderer->SetCube({-2.0f * GameAspect, -2.0f * (-GameAspect/WindowAspect), 1.0f});
        // Renderer->SetCube({-2.0f * GameAspect, -2.0f * (-GameAspect/WindowAspect), 1.0f});
        Background->SetCube({GameAspect*-2.0f, (-GameAspect/WindowAspect)*-2.0f, 1.0f});
    }
    
    UIRenderer->SetPosition(glm::vec3((CamOffset / (glm::vec2(ActualGameSize) * UIRenderScale)), UIRenderer->GetPosition().z));
    Renderer->SetPosition(glm::vec3((CamOffset / (glm::vec2(ActualGameSize) * UIRenderScale)), Renderer->GetPosition().z));
    // GUI->m_GameOffset = GameOffset;
    // GUI->m_GameSize = ActualGameSize;
    // GUI->m_GameScale = GameScale;
    
    // for (auto &x: Buttons)
    // {
    //     x->Update();
    // }
    // GUI->Update();

    GameBuffer->SetSize(glm::vec2(ActualGameSize) * GameRenderScale);
    UIBuffer->SetSize(glm::vec2(ActualGameSize) * UIRenderScale);
    
    
    // Background->Update();
    // Renderer->Update();
    // UIRenderer->Update();
    // GameRender->GenerateMesh();
    GameRender->Update();
    // UIRender->GenerateMesh();
    UIRender->Update();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{   
    WindowSize = {width,height};
    // GUI->m_GameAspect = GameAspect;


    UpdateWindows();

    auto TimeNow = std::chrono::high_resolution_clock::now();

    static auto LastUpdate = std::chrono::high_resolution_clock::now();

    if ((TimeNow - LastUpdate) > std::chrono::milliseconds((int)(FrameTime * 1000.0f)))
    {
        LastUpdate = TimeNow;
        RenderDemand = true;
        RenderLoop();
    }
}

void move_callback(GLFWwindow* window, int xpos, int ypos)
{
    auto TimeNow = std::chrono::high_resolution_clock::now();

    static auto LastUpdate = std::chrono::high_resolution_clock::now();

    if ((TimeNow - LastUpdate) > std::chrono::milliseconds((int)(FrameTime * 1000.0f)))
    {
        LastUpdate = TimeNow;
        RenderDemand = true;
        RenderLoop();
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    // if (button == GLFW_MOUSE_BUTTON_RIGHT)
    // {
    //     if (action == GLFW_PRESS)
    //     {
    //         if (!MouseSelect)
    //         {
    //             double xpos, ypos;
    //             glfwGetCursorPos(window, &xpos, &ypos);
    //             StartMouse = {xpos, ypos};
    //         }

    //     }
    //     else
    //     {
    //         if (!MouseSelect)
    //         {
    //             StartMouse = {-1,-1};
    //         }
    //     }
    // }
    WHandler->SetMouseDown(action == GLFW_PRESS);

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos); // on touch, emscripten doesn't emit mouse move event
    glm::vec2 ScreenMousePos;
    
    ScreenMousePos.x = (2.0f * ((xpos - ((float)(WindowSize.x - ActualGameSize.x)/2.0f))/(float)ActualGameSize.x)) - 1.0f;
    ScreenMousePos.y = (2.0f * (1.0f - (ypos - ((float)(WindowSize.y - ActualGameSize.y)/2.0f))/(float)ActualGameSize.y)) - 1.0f;

    MousePos = UICam.Screen2World(ScreenMousePos);
    WHandler->SetMousePos(MousePos);
    // WHandler->Update();
}

// void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
// {
//     glm::vec2 ScreenMousePos;
    
//     ScreenMousePos.x = (2.0f * ((xpos - ((float)(WindowSize.x - ActualGameSize.x)/2.0f))/(float)ActualGameSize.x)) - 1.0f;
//     ScreenMousePos.y = (2.0f * (1.0f - (ypos - ((float)(WindowSize.y - ActualGameSize.y)/2.0f))/(float)ActualGameSize.y)) - 1.0f;

//     MousePos = UICam.Screen2World(ScreenMousePos);
//     WHandler->SetMousePos(MousePos);
//     std::cout << "move\n";
//     // std::cout << "{" << MousePos.Start.x << "," << MousePos.Start.y << "," << MousePos.Start.z << "}, {" << MousePos.End.x << "," << MousePos.End.y << "," << MousePos.End.z << "}\n";
//     // WHandler->Update();
// }

void RenderLoop()
{
    static auto LastFrame = std::chrono::high_resolution_clock::now();

    auto start = std::chrono::high_resolution_clock::now();
    update(DeltaTime);
    std::this_thread::sleep_for(std::chrono::milliseconds(int(FrameTime*1000.0f)) - (std::chrono::high_resolution_clock::now() - start));


    // std::this_thread::sleep_for(std::chrono::milliseconds(int(FrameTime*1000.0f)-1) - (std::chrono::high_resolution_clock::now() - start));
    // while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start) < std::chrono::milliseconds(int(FrameTime*1000.0f)))
    // {
    //     std::this_thread::yield();
    // }
    DeltaTime = (float)(std::chrono::duration_cast<std::chrono::milliseconds>(start - LastFrame).count())/1000.0f;
    LastFrame = start;
}

void Loop()
{
    /* Poll for and process events */
    glfwPollEvents();
    RenderLoop();
}

// Entry Point
int main (int argc, char *argv[])
{
    // StartAccurateSleep();
    // std::cout << "Start\n";

    // LoadFileInResource();

    /* Initialize the library */
    if ( !glfwInit() )
    {
        std::cout << "Unable to load glfw\n";
        return -1;
    }

    // glfwSwapInterval(true);
    
#ifdef __EMSCRIPTEN__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW , GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    // SetDPIScale();
    // glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    /* Create a windowed mode window and its OpenGL context */

    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetKeyCallback(window, key_callback);

    // glfwSetWindowAspectRatio(window, 16.0f, 9.0f);

    /* Make the window's context current */
    // glfwMakeContextCurrent(window);

    MainWindow = glfwCreateWindow(WindowSize.x, WindowSize.y, "ChemSim", NULL, NULL);
    if ( !MainWindow )
    {
        std::cout << "unable to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(MainWindow);

    const std::vector<uint8_t> FontFile = GetResource(IDR_FONT);
    const std::vector<uint8_t> ChemsimFile = GetResource(IDR_PNGICON);

    fx_Image Icon = fx_Image::LoadImage(ChemsimFile);
    GLFWimage images[1];
    images[0].width = Icon.Width;
    images[0].height = Icon.Height;
    images[0].pixels = Icon.Data.data();
    glfwSetWindowIcon(MainWindow, 1, images); 

#ifndef __EMSCRIPTEN__
    if ( glewInit() != GLEW_OK)
    {
        std::cout << "Glew Failed to initialize\n";
    }
#endif

    // std::cout << "Created Window\n";

    glfwSetWindowSizeCallback(MainWindow, framebuffer_size_callback);
    glfwSetWindowPosCallback(MainWindow, move_callback);
    glfwSetMouseButtonCallback(MainWindow, mouse_button_callback);
    // glfwSetCursorPosCallback(MainWindow, mouse_pos_callback);

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    #ifndef __EMSCRIPTEN__
    glDebugMessageCallback(MessageCallback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, NULL, GL_TRUE);
    #endif
    // std::cout << glGetString(GL_VERSION) << "\n";
    // gl()

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glEnable(GL_CULL_FACE);  
    glCullFace(GL_BACK); 
    // glEnable(GL_MULTISAMPLE);  

    glActiveTexture(GL_TEXTURE0);

    unsigned int DefaultVao;
    glGenVertexArrays(1, &DefaultVao);

    SetWindowsIcon(MainWindow);
    glfwSwapInterval(0);

    std::vector<uint8_t> Res;
    
    Programs.resize(4);
    Res = GetResource(IDR_BSVSDR);
    fx_Shader BasicVertex = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "vert");
    Res = GetResource(IDR_BSFSDR);
    fx_Shader BasicFragment = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "frag");
    Programs[fx_BasicType::Basic] = new fx_Program(std::vector<fx_Shader *>({&BasicVertex, &BasicFragment}));
    Res = GetResource(IDR_SPVSDR);
    fx_Shader SpriteVertex = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "vert");
    Res = GetResource(IDR_SPFSDR);
    fx_Shader SpriteFragment = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "frag");
    Programs[fx_BasicType::Sprite] = new fx_Program(std::vector<fx_Shader *>({&SpriteVertex, &SpriteFragment}));
    Res = GetResource(IDR_CRVSDR);
    fx_Shader CircleVertex = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "vert");
    Res = GetResource(IDR_CRFSDR);
    fx_Shader CircleFragment = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "frag");
    Programs[fx_BasicType::Circle] = new fx_Program(std::vector<fx_Shader *>({&CircleVertex, &CircleFragment}));
    Res = GetResource(IDR_TXVSDR);
    fx_Shader TextVertex = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "vert");
    Res = GetResource(IDR_TXFSDR);
    fx_Shader TextFragment = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "frag");
    Programs[fx_BasicType::SDF] = new fx_Program(std::vector<fx_Shader *>({&TextVertex, &TextFragment}));
    // Lib = fx_Load_Lib();
    // TODO: Implement the cache
    // TimesFace = fx_Load_Face(Lib, "data/Times.ttf");

    // fx_Atlas Fonts;
    // fx_Load_Font(&Fonts, TimesFace, "Data/cache/Font0.png", 0, false);
    // Fonts.Image.Data = ColorConvert::GetColorConvertFunc(ColorConvert::ColorClass::Gray, ColorConvert::ColorClass::RGBA)(Fonts.Image.Data);
    // Fonts.Image.Component = 4;

    // LookAtMat = glm::perspective(glm::radians(170.0f),GameAspect, 0.1f, 10.0f);
    // LookAtMat = glm::perspective(glm::radians(150.0f),GameAspect, 0.1f, 10.0f) * glm::lookAt();
    
    // LookAtMat = glm::ortho( (-(float)(ActualGameSize.x )/(float)(ActualGameSize.y)) * GameScale, ((float)(ActualGameSize.x)/(float)(ActualGameSize.y)) * GameScale, -1.0f * GameScale, 1.0f * GameScale, 0.1f, 10.0f );
    // glm::mat4 InvLookAtMat = glm::inverse(LookAtMat);
    RenderLookAtMat = glm::identity<glm::mat4>();

    Group1 = new fx_Group(Programs, NULL);
    Group1->SetCamera(&ObjCam);
    // Group2 = new fx_Group(Programs, fx_InitBufferMap(), NULL);
    UIGroup = new fx_Group(Programs, NULL);
    UIGroup->SetCamera(&UICam);

    UIRender = new fx_Group(Programs, NULL);
    GameRender = new fx_Group(Programs, NULL);

    GameBuffer = new fx_Framebuffer();
    UIBuffer = new fx_Framebuffer();

    GameBuffer->SetSize(glm::vec2(ActualGameSize) * GameRenderScale);
    UIBuffer->SetSize(glm::vec2(ActualGameSize) * UIRenderScale);

    GameRender->m_TextureUnit = GameBuffer->GetColorAttachment();
    UIRender->m_TextureUnit = UIBuffer->GetColorAttachment();

    Renderer = new fx_Sprite({0.0f,0.0f,-2.0f}, {2.0f, -2.0f}, {1.0f,0.0f,0.0f,1.0f});

    Renderer->SetAnchor({0.5f,0.5f,0.0f});
    GameRender->AddObject(Renderer);

    UIRenderer = new fx_Sprite({0.0f,0.0f,-2.0f}, {2.0f, -2.0f}, {1.0f,0.0f,0.0f,1.0f});

    UIRenderer->SetAnchor({0.5f,0.5f,0.0f});
    UIRender->AddObject(UIRenderer);

    Background = new fx_Quad({0, 0.0f, -0.5f}, {1.0f, 1.0f});
    Background->SetAnchor({0.5f,0.5f,0.0f});
    GameRender->AddObject(Background);

    // Circle1 = new fx_Circle({-0.5f, 0.1f, -2}, glm::vec2(0.2f, 0.2f), {1.0f,0.0f,0.0f,1.0f});
    // Circle1->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    // Circle1->Update();
    // Group1->m_Objects.push_back(Circle1);

    // Circle2 = new fx_Circle({0.8f, 0.8f, -2}, glm::vec2(0.2f, 0.2f), {0.0f,1.0f,0.0f,1.0f});
    // Circle2->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    // Circle2->Update();
    // Group1->m_Objects.push_back(Circle2);
    
    // b2BodyDef wallBodyDef;
    // // wallBodyDef.type = b2_staticBody;

    // b2PolygonShape dynamicBox;
    // dynamicBox.SetAsBox(GameScale, GameScale);

    // wallBodyDef.position.Set(0, GameScale*2.0f);
    // m_walln = world->CreateBody(&wallBodyDef);
    // wallBodyDef.position.Set(0, -(GameScale*2.0f));
    // m_walls = world->CreateBody(&wallBodyDef);
    // wallBodyDef.position.Set(GameScale*2.0f, 0);
    // m_walle = world->CreateBody(&wallBodyDef);
    // wallBodyDef.position.Set(-(GameScale*2.0f), 0);
    // m_wallw = world->CreateBody(&wallBodyDef);

    // m_walln->CreateFixture(&dynamicBox, 0.0f);
    // m_walls->CreateFixture(&dynamicBox, 0.0f);
    // m_walle->CreateFixture(&dynamicBox, 0.0f);
    // m_wallw->CreateFixture(&dynamicBox, 0.0f);

    // srand(time(0));

    // float Power = 1.0f;

    ///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, 0, 0));

    std::thread Phy(PhysicsLoop);

	///-----initialization_end-----


    

    // std::cout << Posdist(generator) << "\n";

    // TODO: manual add, tutorial, licenses, info, clear screen, drawing meaning, stats, molecule drag

    // Arial = new fx_Font("Arial.ttf");
    // Arial = new fx_Font(fx_ReadBinaryFile("Arial.ttf"));
    fx_Font *FontObj = new fx_Font(FontFile);
    fx_Image FontImg = FontObj->GetAtlas().Image;

    UIGroup->m_TextureUnit = new fx_Texture(FontImg);

    // fx_Sprite *Img = new fx_Sprite({0,1,-1},{1.0f, 1.0f}, {0,0,1,1});
    // UIGroup->AddObject(Img);

    // Text = new fx_Text({0.0f,0.0f,-1.0f},{1.0f}, FontObj, "Testg.aaa");
    // Text->SetAnchor({0.5f,0.5f,0.0f});

    // {
    //     // std::vector<std::string> A = fx_TextBox::Box(5.0f, 1.0f, 0.0f, FontObj, std::string(Res.begin(), Res.end()));
    //     // std::vector<std::string> A = fx_TextBox::Box(5.0f, 1.0f, 0.0f, FontObj, "ABCDEFGHIJKLM\n aaa aaa aaa bbb OPQRSTUVWXYZ aaa");
    // }


    // UIGroup->AddObject(Text);



    //  world->SetContactListener(&AtomContactListenerInstance);

    BHandler = new fx_BillboardHandler();

    // Circle1 = new fx_BillboardCircle({-1,-1,-1}, {1.0f,1.0f}, {1,1,0,1});
    // // Group1->AddObject(Circle1);
    // // BHandler->AddObject(Circle1);

    // Circle2 = new fx_BillboardCircle({-1,1,-1}, {1.0f,1.0f}, {1,0,0,1});
    // // Group1->AddObject(Circle2);
    // // BHandler->AddObject(Circle2);

    // Circle3 = new fx_BillboardCircle({1,-1,-1}, {1.0f,1.0f}, {0,1,0,1});
    // // Group1->AddObject(Circle3);
    // // BHandler->AddObject(Circle3);

    // Circle4 = new fx_BillboardCircle({1,1,-1}, {1.0f,1.0f}, {0,0,1,1});
    // // Group1->AddObject(Circle4);
    // // BHandler->AddObject(Circle4);

    // Circle5 = new fx_BillboardCircle({0,0,-1}, {1.0f,1.0f}, {1,1,1,1});
    // // Group1->AddObject(Circle5);
    // // BHandler->AddObject(Circle5);

    // Circle6 = new fx_BillboardCircle({-1,0,-1}, {1.0f,1.0f}, {1,1,0.5,1});
    // // Group1->AddObject(Circle6);
    // // BHandler->AddObject(Circle6);

    // Circle7 = new fx_BillboardCircle({0,1,-1}, {1.0f,1.0f}, {1,0,1,1});
    // // Group1->AddObject(Circle7);
    // // BHandler->AddObject(Circle7);

    // Circle8 = new fx_BillboardCircle({0,-1,-1}, {1.0f,1.0f}, {0,1,1,1});
    // // Group1->AddObject(Circle8);
    // // BHandler->AddObject(Circle8);

    // Circle9 = new fx_BillboardCircle({1,0,-1}, {1.0f,1.0f}, {0.5,0.5,0.5,1});
    // // Group1->AddObject(Circle9);
    // // BHandler->AddObject(Circle9);

    // Line1 = new fx_BillboardLine({0,0,0}, {1,1,0}, 0.25);
    // Group1->AddObject(Line1);
    // BHandler->AddObject(Line1);

    WHandler = new fx_WidgetHandler();

    // reactphysics3d::PhysicsWorld::WorldSettings settings;
    // settings.defaultVelocitySolverNbIterations = 20;
    // settings.isSleepingEnabled = false;
    // settings.gravity = reactphysics3d::Vector3(0,0,0);
    
    // // Create the physics world with your settings
    // PhysicWorld = physicsCommon.createPhysicsWorld(settings);

    fx_Button *Button1 = new fx_Button({-4,-2,-1}, {1.0f,1.0f}, 0.5f, FontObj, "Rotate", {1,0,0,1}, {0,0,1,1}, {0,1,1,1}, {1,1,1,1});
    Button1->SetAnchor({0.0f,0.0f,0.0f});

    UIGroup->AddObject(Button1);
    WHandler->AddObject(Button1);

    fx_Button *Button2 = new fx_Button({-3,-2,-1}, {1.0f,1.0f}, 0.5f, FontObj, "+", {1,0,0,1}, {0,0,1,1}, {0,1,1,1}, {1,1,1,1});
    Button2->SetAnchor({0.0f,0.0f,0.0f});

    fx_Button *Button3 = new fx_Button({-2,-2,-1}, {1.0f,1.0f}, 0.5f, FontObj, "-", {1,0,0,1}, {0,0,1,1}, {0,1,1,1}, {1,1,1,1});
    Button3->SetAnchor({0.0f,0.0f,0.0f});

    UIGroup->AddObject(Button2);
    WHandler->AddObject(Button2);

    UIGroup->AddObject(Button3);
    WHandler->AddObject(Button3);

    fx_Button *Button4 = new fx_Button({4,-2,-1}, {1.0f,1.0f}, 0.5f, FontObj, "+H", {1,0,0,1}, {0,0,1,1}, {0,1,1,1}, {1,1,1,1});
    Button4->SetAnchor({0.0f,0.0f,0.0f});

    fx_Button *Button5 = new fx_Button({3,-2,-1}, {1.0f,1.0f}, 0.5f, FontObj, "+Cl", {1,0,0,1}, {0,0,1,1}, {0,1,1,1}, {1,1,1,1});
    Button5->SetAnchor({0.0f,0.0f,0.0f});

    UIGroup->AddObject(Button4);
    WHandler->AddObject(Button4);

    UIGroup->AddObject(Button5);
    WHandler->AddObject(Button5);
    // 5.0f, 1.0f, 0.0f, FontObj, "ABCDEFGHIJKLM\n aaa aaa aaa bbb OPQRSTUVWXYZ aaa"

    fx_TextBox *Box = new fx_TextBox({0,0,-1}, 0.5f, 3.0f, FontObj, "ABCDEFGHIJKLM\n aaa aaa aaa bbb OPQRSTUVWXYZ aaa",{1,1,1,1}, {.5,.5,.5,1});
    Box->SetAnchor({0.0f,0.0f,0.0f});
    Box->SetLineSpacing(0.7f);
    UIGroup->AddObject(Box);


    
    // Atom *Carbon = new Atom(C_);
    // Atom *Hydro1 = new Atom(H_);
    // Atom *Hydro2 = new Atom(H_);
    // Atom *Hydro3 = new Atom(H_);
    // Atom *Hydro4 = new Atom(H_);
    
    // Carbon->m_Child.push_back(Hydro1);
    // Carbon->m_Child.push_back(Hydro2);
    // Carbon->m_Child.push_back(Hydro3);
    // Carbon->m_Child.push_back(Hydro4);
    
    // std::cout << "a\n";


    // M23 = new Molecule({{0, H_}, {1, H_}},{0.0f,2.0f,0.0f});
    // BHandler->AddObject(M23);
    // Group1->AddObject(M23);


    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
    {
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0, -60, 0));

        btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(0.), myMotionState, groundShape, btVector3(0,0,0));
        btRigidBody* body = new btRigidBody(rbInfo);
        body->setUserPointer((void*)NULL);

        //add the body to the dynamics world
        dynamicsWorld->addRigidBody(body);
    }
    {
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0, 60, 0));

        btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(0.), myMotionState, groundShape, btVector3(0,0,0));
        btRigidBody* body = new btRigidBody(rbInfo);
        body->setUserPointer((void*)NULL);

        //add the body to the dynamics world
        dynamicsWorld->addRigidBody(body);
    }
    {
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(60, 0, 0));

        btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(0.), myMotionState, groundShape, btVector3(0,0,0));
        btRigidBody* body = new btRigidBody(rbInfo);
        body->setUserPointer((void*)NULL);

        //add the body to the dynamics world
        dynamicsWorld->addRigidBody(body);
    }
    {
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(-60, 0, 0));

        btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(0.), myMotionState, groundShape, btVector3(0,0,0));
        btRigidBody* body = new btRigidBody(rbInfo);
        body->setUserPointer((void*)NULL);

        //add the body to the dynamics world
        dynamicsWorld->addRigidBody(body);
    }
    {
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0, 0, 60));

        btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(0.), myMotionState, groundShape, btVector3(0,0,0));
        btRigidBody* body = new btRigidBody(rbInfo);
        body->setUserPointer((void*)NULL);

        //add the body to the dynamics world
        dynamicsWorld->addRigidBody(body);
    }
    {
        btTransform groundTransform;
        groundTransform.setIdentity();
        groundTransform.setOrigin(btVector3(0, 0, -60));

        btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(0.), myMotionState, groundShape, btVector3(0,0,0));
        btRigidBody* body = new btRigidBody(rbInfo);
        body->setUserPointer((void*)NULL);

        //add the body to the dynamics world
        dynamicsWorld->addRigidBody(body);
    }

    // Molecule *M1 = new Molecule({{0, H_}, {1, H_}},{1.0f,2.0f,0.0f});
    // BHandler->AddObject(M1);
    // Group1->AddObject(M1);
    // MoleculesList.push_back(M1);

    // Molecule *M2 = new Molecule({{0, Cl_}, {1, Cl_}},{1.0f,1.0f,0.0f});
    // BHandler->AddObject(M2);
    // Group1->AddObject(M2);
    // MoleculesList.push_back(M2);

    // Molecule *M3 = new Molecule({{0, H_}, {1, H_}},{-1.0f,2.0f,0.0f});
    // BHandler->AddObject(M3);
    // Group1->AddObject(M3);
    // MoleculesList.push_back(M3);

    // Molecule *M3 = new Molecule({{0, N_}, {3, N_}},{0.0f,-1.2f,0.0f});
    // BHandler->AddObject(M3);
    // Group1->AddObject(M3);
    
    BHandler->SetCameraPos(ObjCam.GetPosition());
    BHandler->SetCameraUp(glm::vec3(0,1,0) * ObjCam.GetQuat());
    // std::cout << "b\n";

    // float Angle = 0;

    ObjCam.SetFar(50.0f);

    // ObjCam.SetSize(1.0f);

    Button1->m_MainActionCallback = [&]() {
        glm::vec3 CamPos = ObjCam.GetPosition();
        float Angle = std::atan2(CamPos.x, CamPos.z);
        Angle += glm::pi<float>() * DeltaTime;

        float CamLenght = glm::length(CamPos);


        CamPos.x = std::sin(Angle) * CamLenght;
        CamPos.y = 0;
        CamPos.z = std::cos(Angle) * CamLenght;

        ObjCam.SetPosition(CamPos);
        ObjCam.SetQuat(glm::quat(glm::vec3(0.0f,-Angle,0.0f)));
        
        BHandler->SetCameraPos(CamPos);
        BHandler->SetCameraUp(glm::vec3(0,1,0) * glm::quat(glm::vec3(0.0f,-Angle,0.0f)));

    };
    
    Button1->m_HoldActionCallback = Button1->m_MainActionCallback ;

    Button2->m_MainActionCallback = [&]() {
        glm::vec3 CamPos = ObjCam.GetPosition();
        float Angle = std::atan2(CamPos.x, CamPos.z);

        // glm::vec3 Delta = glm::normalize(glm::abs(ObjCam.GetPosition()));

        float CamLenght = glm::length(CamPos) - (glm::pi<float>() * DeltaTime);

        // CamPos = CamPos - (Delta * glm::pi<float>() * DeltaTime);

        CamPos.x = std::sin(Angle) * CamLenght;
        CamPos.y = 0;
        CamPos.z = std::cos(Angle) * CamLenght;

        ObjCam.SetPosition(CamPos);
        BHandler->SetCameraPos(CamPos);
    };
    Button2->m_HoldActionCallback = Button2->m_MainActionCallback ;


    Button3->m_MainActionCallback = [&]() {
        // Angle += glm::pi<float>() *DeltaTime;
        glm::vec3 CamPos = ObjCam.GetPosition();
        float Angle = std::atan2(CamPos.x, CamPos.z);

        float CamLenght = glm::length(CamPos) + (glm::pi<float>() * DeltaTime);

        CamPos.x = std::sin(Angle) * CamLenght;
        CamPos.y = 0;
        CamPos.z = std::cos(Angle) * CamLenght;

        ObjCam.SetPosition(CamPos);
        BHandler->SetCameraPos(CamPos);
    };
    Button3->m_HoldActionCallback = Button3->m_MainActionCallback ;

    Button4->m_MainActionCallback = [&]() {
        Molecule *M1 = new Molecule({{0, H_}, {1, H_}},{8.0f,5.0f,0.0f});
        M1->SetQuat(glm::quat(glm::vec3(AngDist(Gen), AngDist(Gen), AngDist(Gen))));
        M1->SetVelocity(glm::normalize(glm::vec3(-std::abs(Veldist(Gen)), Veldist(Gen), Veldist(Gen))) * MoleculeSpawnVel);
        BHandler->AddObject(M1);
        Group1->AddObject(M1);
        MoleculesList.push_back(M1);
    };
    Button4->m_HoldActionCallback = [&]() {

        static float Accumulator = 0.0f;
        const float TimeStep = 0.4;
        Accumulator += DeltaTime;

        while(Accumulator >= TimeStep)
        {
            Button4->m_MainActionCallback();
            Accumulator -= TimeStep;
        }
    };


    Button5->m_MainActionCallback = [&]() {
        Molecule *M1 = new Molecule({{0, Cl_}, {1, Cl_}},{-8.0f,5.0f,0.0f});
        M1->SetQuat(glm::quat(glm::vec3(AngDist(Gen), AngDist(Gen), AngDist(Gen))));
        M1->SetVelocity(glm::normalize(glm::vec3(std::abs(Veldist(Gen)), Veldist(Gen), Veldist(Gen))) * MoleculeSpawnVel);
        BHandler->AddObject(M1);
        Group1->AddObject(M1);
        MoleculesList.push_back(M1);
    };
    Button5->m_HoldActionCallback = [&]() {

        static float Accumulator = 0.0f;
        const float TimeStep = 0.4;
        Accumulator += DeltaTime;

        while(Accumulator >= TimeStep)
        {
            Button5->m_MainActionCallback();
            Accumulator -= TimeStep;
        }
    };


    // UIGroup->GenerateMesh();
    UIGroup->Update();

    // Group1->GenerateMesh();
    Group1->Update();

    // GameRender->GenerateMesh();
    GameRender->Update();

    std::cout << "Scene done loading\n";



//     GUI = new fx_GUILayer(MainWindow);
//     // std::cout << "{" << ActualGameSize.x << "," << ActualGameSize.y << "}\n" ;
//     // GUI->m_GameAspectRatio = GameAspect;
//     // GUI->Update(WindowSize.x, WindowSize.y, GameAspect);

//     {
//         fx_Button *Button1 = new fx_Button(GUI, "+ H-H");
//         Buttons.push_back(Button1);
//         // framebuffer_size_callback(MainWindow, WindowSize.x, WindowSize.y);

//         Button1->m_ClickCallback = [&]() {
//             glm::vec2 Dir = {std::cos(AngDist(Gen)), std::sin(AngDist(Gen))};
//             InitMolecule({Posdist(Gen) ,Posdist(Gen)}, {std::make_pair(Elements::H, AngDist(Gen)), std::make_pair(Elements::H, glm::pi<float>())}, Group1, Dir * 0.0005f);
//             Reactant1Tot++;
//         };
//         Button1->m_Info.m_Position = glm::vec4({-1.0f* GameScale, 1.0f* GameScale, 0.0f, -2.0f});
//         Button1->m_Info.m_Anchor = {1.0f, 1.0f, 0.0f};
//         Button1->m_Info.m_Size = {(GameAspect - 1.0f) * GameScale, 0.5f * GameScale, 0.0f};
//         Button1->m_Text = "+ H-H";
//         Button1->Update();
//     }
//     {
//         fx_Button *Button1 = new fx_Button(GUI, "+ Cl-Cl");
//         Buttons.push_back(Button1);
//         // framebuffer_size_callback(MainWindow, WindowSize.x, WindowSize.y);



//         Button1->m_ClickCallback = [&]() {
//             glm::vec2 Dir = {std::cos(AngDist(Gen)), std::sin(AngDist(Gen))};
//             InitMolecule({Posdist(Gen) ,Posdist(Gen)}, {std::make_pair(Elements::Cl, AngDist(Gen)), std::make_pair(Elements::Cl, glm::pi<float>())}, Group1, Dir * 0.0005f);
//             Reactant2Tot++;
//         };
//         Button1->m_Info.m_Position = glm::vec4({-1.0f* GameScale, 0.5f * GameScale, 0.0f, -2.0f});
//         Button1->m_Info.m_Anchor = {1.0f, 1.0f, 0.0f};
//         Button1->m_Info.m_Size = {(GameAspect - 1.0f) * GameScale, 0.5f * GameScale, 0.0f};
//         Button1->Update();
//     }
//     {
//         fx_Button *Button1 = new fx_Button(GUI, "+ Temp");
//         Buttons.push_back(Button1);
//         // framebuffer_size_callback(MainWindow, WindowSize.x, WindowSize.y);

//         Button1->m_ClickCallback = [&]() {
//             for (auto &x : AtomsObj)
//             {
//                 // if (x->m_Parent)
//                 // {
//                 //     continue;
//                 // }
//                 glm::vec2 Dir = toGlm(x->m_Body->GetLinearVelocity());
//                 if (glm::length(Dir) == 0.0f)
//                 {
//                     Dir = {std::cos(AngDist(Gen)), std::sin(AngDist(Gen))};
//                 }
//                 glm::vec2 F = glm::normalize(Dir) * 0.0005f;

//                 x->m_Body->ApplyForce(toB2(F), x->m_Body->GetPosition(), true);

//                 for (auto &y: x->m_ChildAtom)
//                 {
//                     y->m_Body->ApplyForce(toB2(F), y->m_Body->GetPosition(), true);
//                 }


//             }
//         };
//         Button1->m_Info.m_Position = glm::vec4({-1.0f* GameScale, 0.0f* GameScale, 0.0f, -2.0f});
//         Button1->m_Info.m_Anchor = {1.0f, 1.0f, 0.0f};
//         Button1->m_Info.m_Size = {(GameAspect - 1.0f) * GameScale, 0.5 * GameScale, 0.0f};
//         Button1->Update();
//     }
//     {
//         fx_Button *Button1 = new fx_Button(GUI,"Clear");
//         Buttons.push_back(Button1);
//         // framebuffer_size_callback(MainWindow, WindowSize.x, WindowSize.y);

//         Button1->m_ClickCallback = [&]() {
//             // InitMolecule({Posdist(Gen) ,Posdist(Gen)}, {std::make_pair(Elements::Cl, AngDist(Gen)), std::make_pair(Elements::Cl, glm::pi<float>())}, Group1);
//             for (auto &x : AtomsObj)
//             {
//                 if (!x->m_Body)
//                 {
//                     continue;
//                 }
//                 //can't delete fx_circle
//                 // FIXME
//                 // delete x;
//                 x->m_Enabled = false;

//                 world->DestroyBody(x->m_Body);
//                 x->m_Bonds.clear();
//                 x->m_Body = nullptr;

//             }

//             CollisionList.clear();
//             ReactionStack.clear();
//             AtomsObj.clear();
//             Group1->m_Objects.clear();
//             Reactant1Tot = 0;
//             Reactant2Tot = 0;
//         };
//         Button1->m_Info.m_Position = glm::vec4({-1.0f* GameScale, -0.5f * GameScale, 0.0f, -2.0f});
//         Button1->m_Info.m_Anchor = {1.0f, 1.0f, 0.0f};
//         Button1->m_Info.m_Size = {(GameAspect - 1.0f) * GameScale, 0.5f * GameScale, 0.0f};
//         Button1->Update();
//     }
//     {
//         fx_Button *Button1 = new fx_Button(GUI,"Stats");
//         Buttons.push_back(Button1);
//         // framebuffer_size_callback(MainWindow, WindowSize.x, WindowSize.y);

//         Button1->m_ClickCallback = [&]() {
//             // percentage yield, tot reactant remaining, tot product, ratio
//             int Reactant1 = 0;
//             int Reactant2 = 0;
//             int Product = 0;
//             for (auto &x : AtomsObj)
//             {
//                 if (x->m_Parent)
//                 {
//                     continue;
//                 }
//                 if (x->m_Element == Elements::H && x->m_ChildAtom.size() && x->m_ChildAtom[0]->m_Element == x->m_Element && x->m_Bonds.size() != 0)
//                 {
//                     Reactant1++;
//                 }
//                 else if (x->m_Element == Elements::Cl && x->m_ChildAtom.size() && x->m_ChildAtom[0]->m_Element == x->m_Element && x->m_Bonds.size() != 0)
//                 {
//                     Reactant2++;
//                 }
//             }
//             std::string Limit;
//             std::string Exces;
//             if (Reactant1Tot < Reactant2Tot)
//             {
//                 Product = ((Reactant1Tot - Reactant1) * 2.0f);
//                 Limit = "H-H";
//                 Exces = "Cl-Cl";
//             }
//             else
//             {
//                 Product = ((Reactant2Tot - Reactant2) * 2.0f);
//                 Exces = "H-H";
//                 Limit = "Cl-Cl";
//             }

//             if (Reactant1Tot == Reactant2Tot)
//             {
//                 Exces = "H-H / Cl-Cl";
//                 Limit = "H-H / Cl-Cl";

//             }
//             float React1M = (float)Reactant1 * 2.0f;
//             float React2M = (float)Reactant2 * 71.0f;
//             float ProdM = (float)Product * (35.5f + 1.0f);
//             float PercentageYield = (ProdM / (((float)std::min(Reactant1Tot, Reactant2Tot) *2.0f * (35.5f + 1.0f)) + glm::epsilon<float>())) * 100.0f;
//             std::stringstream Output;
//             Output << \
//             "Percentage Yield : " << std::setprecision(2) << PercentageYield << "%\n" << \
//             "Initial H-H Molecule : " << Reactant1Tot << "\n" << \
//             "Initial Cl-Cl Molecule : " << Reactant2Tot << "\n" << \
//             "Remaining H-H Molecule : " << Reactant1 << "\n" << \
//             "Remaining Cl-Cl Molecule : " << Reactant2 << "\n" << \
//             "Available H-Cl Molecule : " << Product << "\n" << \
//             "Limiting reactant : " << Limit << "\n" << \
//             "Excess reactant : " << Exces << "\n" << \
//             "Available H-Cl Molecule : " << Product << "\n" << \
//             "Chemical equation : H-H + Cl+Cl -> 2H-Cl" << "\n" << \
//             "Equation ratio : 1 : 1 : 2";
//             fx_Message(GUI, "ChemSim Stats", Output.str());

//         };
//         Button1->m_Info.m_Position = glm::vec4({1.0f* GameScale, 1.0f * GameScale, 0.0f, -2.0f});
//         Button1->m_Info.m_Anchor = {0.0f, 1.0f, 0.0f};
//         Button1->m_Info.m_Size = {(GameAspect - 1.0f) * GameScale, 0.5f * GameScale, 0.0f};
//         Button1->Update();
//     }
//     {
//         fx_Button *Button1 = new fx_Button(GUI,"Tutorial");
//         Buttons.push_back(Button1);
//         // framebuffer_size_callback(MainWindow, WindowSize.x, WindowSize.y);

//         Button1->m_ClickCallback = [&]() {
//             for (auto &x : AtomsObj)
//             {
//                 if (!x->m_Body)
//                 {
//                     continue;
//                 }
//                 //can't delete fx_circle
//                 // FIXME
//                 // delete x;
//                 x->m_Enabled = false;

//                 world->DestroyBody(x->m_Body);
//                 x->m_Bonds.clear();
//                 x->m_Body = nullptr;

//             }

//             CollisionList.clear();
//             ReactionStack.clear();
//             AtomsObj.clear();
//             Group1->m_Objects.clear();

//             float CacheGameScale = GameScale;
//             float CacheTimeScale = TimeScale;
//             GameScale = 1.0f;
//             TimeScale = 0.025f;
//             b2BodyDef wallBodyDef;
//             Nostep = true;

//             b2PolygonShape dynamicBox;
//             dynamicBox.SetAsBox(GameScale, GameScale);

//             wallBodyDef.position.Set(0, GameScale*2.0f);
//             b2Body *walln = world->CreateBody(&wallBodyDef);
//             wallBodyDef.position.Set(0, -(GameScale*2.0f));
//             b2Body *walls = world->CreateBody(&wallBodyDef);
//             wallBodyDef.position.Set(GameScale*2.0f, 0);
//             b2Body *walle = world->CreateBody(&wallBodyDef);
//             wallBodyDef.position.Set(-(GameScale*2.0f), 0);
//             b2Body *wallw = world->CreateBody(&wallBodyDef);

//             walln->CreateFixture(&dynamicBox, 0.0f);
//             walls->CreateFixture(&dynamicBox, 0.0f);
//             walle->CreateFixture(&dynamicBox, 0.0f);
//             wallw->CreateFixture(&dynamicBox, 0.0f);

//             LookAtMat = glm::ortho( (-(float)(ActualGameSize.x )/(float)(ActualGameSize.y)) * GameScale, ((float)(ActualGameSize.x)/(float)(ActualGameSize.y)) * GameScale, -1.0f * GameScale, 1.0f * GameScale, 0.1f, 10.0f );
//             framebuffer_size_callback(MainWindow, WindowSize.x, WindowSize.y);

//             fx_Message(GUI, "ChemSim Tutorial", "This tutorial will show the basics of covalent bond");

//             InitMolecule({-(ElementsPreset[Elements::Cl].Radius * Atom2Screen) ,0.5f}, {std::make_pair(Elements::Cl, glm::pi<float>()), std::make_pair(Elements::Cl, glm::pi<float>())}, Group1, {0.0f, -0.0025f});
//             update(0.0f);
//             glfwPollEvents();
//             fx_Message(GUI, "ChemSim Tutorial", "The green circle represent Chlorine molecule");

//             InitMolecule({-(ElementsPreset[Elements::H].Radius * Atom2Screen) ,-0.5f}, {std::make_pair(Elements::H, glm::pi<float>()), std::make_pair(Elements::H, glm::pi<float>())}, Group1, {0.0f, 0.0025f});
//             update(0.0f);
//             glfwPollEvents();
//             fx_Message(GUI, "ChemSim Tutorial", "The white circle represent Hydrogen molecule");

//             while (ReactionStack.size() == 0)
//             {
//                 Nostep = false;
//                 update(0.0f);
//                 glfwPollEvents();
//             }

//             fx_Message(GUI, "ChemSim Tutorial", "When They collide, \nthey will form temporary bond to break the covalent bond");

//             {
//                 Nostep = false;
//                 auto Start = std::chrono::high_resolution_clock::now();
//                 while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start) < std::chrono::milliseconds(750))
//                 {
//                     update(0.0f);
//                     glfwPollEvents();
//                 }
//             }

//             fx_Message(GUI, "ChemSim Tutorial", "This temporary bond are represented by the grey line");

//             Nostep = false;
//             while (ReactionStack.size() == 1)
//             {
//                 update(0.0f);
//                 glfwPollEvents();
//             }

//             fx_Message(GUI, "ChemSim Tutorial", "When hydrogen and chlorine atom collide,\nthis will complete the reaction\nthe temporary bond will break to form covalent bond");

//             {
//                 Nostep = false;
//                 auto Start = std::chrono::high_resolution_clock::now();
//                 while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start) < std::chrono::milliseconds(2000))
//                 {
//                     update(0.0f);
//                     glfwPollEvents();
//                 }
//             }

//             fx_Message(GUI, "ChemSim Tutorial", "Thus the chemical reaction is done\nThe equation for this reaction would be:\nCl2 + H2 -> 2HCl");
//             fx_Message(GUI, "ChemSim Tutorial", "Now you can explore it to your heart contents");







//             Nostep = false;
//             TimeScale = CacheTimeScale;
//             GameScale = CacheGameScale;
//             LookAtMat = glm::ortho( (-(float)(ActualGameSize.x )/(float)(ActualGameSize.y)) * GameScale, ((float)(ActualGameSize.x)/(float)(ActualGameSize.y)) * GameScale, -1.0f * GameScale, 1.0f * GameScale, 0.1f, 10.0f );
//             framebuffer_size_callback(MainWindow, WindowSize.x, WindowSize.y);

//             world->DestroyBody(walln);
//             world->DestroyBody(walls);
//             world->DestroyBody(walle);
//             world->DestroyBody(wallw);
//             for (auto &x : AtomsObj)
//             {
//                 if (!x->m_Body)
//                 {
//                     continue;
//                 }
//                 //can't delete fx_circle
//                 // FIXME
//                 // delete x;
//                 x->m_Enabled = false;

//                 world->DestroyBody(x->m_Body);
//                 x->m_Bonds.clear();
//                 x->m_Body = nullptr;

//             }
//             CollisionList.clear();
//             ReactionStack.clear();
//             AtomsObj.clear();
//             Group1->m_Objects.clear();
//         };
//         Button1->m_Info.m_Position = glm::vec4({1.0f* GameScale, 0.5f * GameScale, 0.0f, -2.0f});
//         Button1->m_Info.m_Anchor = {0.0f, 1.0f, 0.0f};
//         Button1->m_Info.m_Size = {(GameAspect - 1.0f) * GameScale, 0.5f * GameScale, 0.0f};
//         Button1->Update();
//     }
//     {
//         fx_Button *Button1 = new fx_Button(GUI,"Info");
//         Buttons.push_back(Button1);
//         // framebuffer_size_callback(MainWindow, WindowSize.x, WindowSize.y);

//         Button1->m_ClickCallback = [&]() {

//             fx_Message(GUI, "Chemsim Info", "This app is created to help visualise a chemical reaction \n\n\
// Problem:\nMany students struggle to understand what's happening during a chemical reaction\n\
// This can be cause by the lack of visualisation\n\
// This tool will help them exactly with that, visualisation");
//         };
//         Button1->m_Info.m_Position = glm::vec4({1.0f* GameScale, 0.0f * GameScale, 0.0f, -2.0f});
//         Button1->m_Info.m_Anchor = {0.0f, 1.0f, 0.0f};
//         Button1->m_Info.m_Size = {(GameAspect - 1.0f) * GameScale, 0.5f * GameScale, 0.0f};
//         Button1->Update();
//     }
//     {
//         fx_Button *Button1 = new fx_Button(GUI,"Licenses");
//         Buttons.push_back(Button1);
//         // framebuffer_size_callback(MainWindow, WindowSize.x, WindowSize.y);

//         Button1->m_ClickCallback = [&]() {
//             fx_Message(GUI, "Freetype Licenses", FTLLSC);
//             fx_Message(GUI, "GLFW3 Licenses", GFLLSC);
//             fx_Message(GUI, "GLEW Licenses", GLWLSC);
//             fx_Message(GUI, "TinyXML2 Licenses", XMLLSC);
//             fx_Message(GUI, "Harfbuzz Licenses", HFZLSC);
//             fx_Message(GUI, "GLM Licenses", GLMLSC);
//             fx_Message(GUI, "STB Licenses", STBLSC);
//             fx_Message(GUI, "Rectpack2D Licenses", R2DLSC);
//             fx_Message(GUI, "Box2D Licenses", B2DLSC);
//             fx_Message(GUI, "ChemSim Licenses", SELFLSC);
//         };
//         Button1->m_Info.m_Position = glm::vec4({1.0f* GameScale, -0.5f * GameScale, 0.0f, -2.0f});
//         Button1->m_Info.m_Anchor = {0.0f, 1.0f, 0.0f};
//         Button1->m_Info.m_Size = {(GameAspect - 1.0f) * GameScale, 0.5f * GameScale, 0.0f};
//         Button1->Update();
//     }

    // world->Step(1.0f/60.0f * TimeScale, 6,5);

    // Button1->m_ClickCallback();
    

    // GUI->Update();


    // for (auto x : Programs)
    // {
    //     x.second->SetUniform(LookAtMat, "Matrix");
    // }

    
    
    UpdateWindows();


    RenderDemand = true;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(Loop, 0, true);
#else
    
    while ( !glfwWindowShouldClose(MainWindow) )
    {
        Loop();
    }

#endif
    mtx.lock();
    RunPhysics = false;
    mtx.unlock();
    glfwTerminate();
    Phy.join();
    // physicsCommon.destroyPhysicsWorld(PhysicWorld);

    //End Program
    return 0;
}