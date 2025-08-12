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

#include "Game.hpp"

#include "embed/Res.rc"

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
// static int FPS = 30;
// static int FrameTimems = int((1.0f/float(FPS))*100.0f);
// static glm::ivec2 WindowSize = {1280,720};
// static glm::ivec2 GameSize = {1280,720};
// static glm::ivec2 UISize = {1280,720};
// static float GameAspect = 16.0f/9.0f;
// static glm::ivec2 ActualGameSize = {1280,720};
// static float DeltaTime = glm::epsilon<float>();
// static float GameRenderScale = 2.0f;
// static float UIRenderScale = 2.0f;
// // static float GameScale = 5.0f;
// const static int FPS = 60;
// const static float FrameTime = 1.0f / (float)FPS;

// static float TimeScale = 1.0f;
// static bool UpdateWindow = false;
// static std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> RenderDemandsStack;

// static bool RenderDemand;

// static std::vector<fx_Program*> Programs;
// static fx_Font_Library Lib;

// static fx_Face TimesFace;
// static fx_Face ArialFace;

// static fx_Framebuffer *GameBuffer;
// static fx_Framebuffer *UIBuffer;

// static fx_Group *Group1;
// static fx_Group *Group2;
// static fx_Group *UIGroup;
// static fx_Group *LoadGroup;

// static fx_Group *UIRender;
// static fx_Group *GameRender;

// static fx_Sprite *Renderer;
// static fx_Sprite *UIRenderer;
// static fx_Quad *Background;

// static glm::mat4 RenderLookAtMat;

// static fx_Text *HNum;
// static fx_Text *ClNum;
// static fx_Text *HClNum;

// static int Reactant1Tot = 0;
// static int Reactant2Tot = 0;
// static int Prod2Tot = 0;

// static fx_Perspective ObjCam({0.0,0.0,10}, GameAspect);
// static fx_Orthographic UICam({0.0,0.0,2.5}, GameAspect);

// static Line3D MousePos;

// static fx_WidgetHandler *WHandler;
// static fx_BillboardHandler *BHandler;
// static glm::vec2 BgCube;

// static std::vector<Molecule *> MoleculesList;
// static std::mutex mtx;
// static bool FlagRunPhysics = true;

// static const int PhysicInterval = 20;
// const static float MoleculeSpawnVel = 5.0f;
// const static float MoleculeMaxVel = 10.0f;

// static bool FlagDoneLoadPhysic = false;
// static bool DoneInitMoleculeFlag = false;

// static std::default_random_engine Gen;
// static std::uniform_real_distribution<float> Veldist(-1.0f, 1.0f);
// static std::uniform_real_distribution<float> AngDist(0,2.0f * glm::pi<float>());

// void PhysicsUpdate(float dt);

// void PhysicsLoop()
// {
//     mtx.lock();

//     ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
// 	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

// 	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
// 	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

// 	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
// 	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

// 	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
// 	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

// 	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

// 	dynamicsWorld->setGravity(btVector3(0, 0, 0));
//     static auto LastFrame = std::chrono::high_resolution_clock::now();
//     static float PhyDT = 0.0f;
//     mtx.unlock();

//     // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
//     DoneLoadingPhysicFlag = true;
//     std::cout << "Physics done loading\n";
//     mtx.unlock();


//     btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
//     auto CreateGround = [&](btVector3 Pos)
//     {
//         btTransform groundTransform;
//         groundTransform.setIdentity();
//         groundTransform.setOrigin(btVector3(Pos.getX(), Pos.getY(), Pos.getZ()));

//         btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
//         btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(0.), myMotionState, groundShape, btVector3(0,0,0));
//         btRigidBody* body = new btRigidBody(rbInfo);
//         body->setUserPointer((void*)NULL);
//         body->setFriction(0.0f);
//         body->setRollingFriction(0.0f);
//         body->setSpinningFriction(0.0f);
//         body->setHitFraction(0.0f);
//         body->setActivationState(DISABLE_DEACTIVATION);
//         //add the body to the dynamics world
//         dynamicsWorld->addRigidBody(body);
//     };
//     CreateGround(btVector3(0,-60,0));
//     CreateGround(btVector3(0,60,0));
//     CreateGround(btVector3(60,0,0));
//     CreateGround(btVector3(-60,0,0));
//     CreateGround(btVector3(0,0,60));
//     CreateGround(btVector3(0,0,-60));
//     mtx.unlock();
    
    

//     while (RunPhysics)
//     {
//         auto start = std::chrono::high_resolution_clock::now();
//         mtx.lock();
//         PhysicsUpdate(PhyDT * TimeScale);
//         mtx.unlock();
//         std::this_thread::sleep_for(std::chrono::milliseconds(PhysicInterval) - (std::chrono::high_resolution_clock::now() - start));
    
//         PhyDT = (float)(std::chrono::duration_cast<std::chrono::milliseconds>(start - LastFrame).count())/1000.0f;
//         LastFrame = start;

//     }
// }

// void PhysicsUpdate(float dt)
// {
    
//     static float Accumulator = 0.0f;
//     const float TimeStep = float(PhysicInterval)/1000.0f;
//     // Add the time difference in the accumulator
//     Accumulator += dt;
    
//     // While there is enough accumulated time to take
//     // one or several physics steps
//     while (Accumulator >= TimeStep) 
//     {
//         // CollideList.clear();
//         std::map<Molecule*,std::set<Molecule*>> HitList;
//         std::map<btRigidBody*,std::set<btRigidBody*>> AList;
//         std::map<btRigidBody*,std::set<btRigidBody*>> BList;
        
//         dynamicsWorld->stepSimulation(TimeStep, 25);
        
//         btDispatcher* dp = dynamicsWorld->getDispatcher();
//         const int numManifolds = dp->getNumManifolds();
//         for ( int m=0; m<numManifolds; m++ )
//         {
//             btPersistentManifold* man = dp->getManifoldByIndexInternal( m );
//             btRigidBody* obA = (btRigidBody*)(man->getBody0());
//             btRigidBody* obB = (btRigidBody*)(man->getBody1());
            
//             Molecule *molA = (Molecule*)obA->getUserPointer();;
//             Molecule *molB = (Molecule*)obB->getUserPointer();
            
//             if (!obA->isActive() || !obB->isActive() || 
//                 (!molA) || (!molB) || (molA == molB) || 
//                 molA->GetAtoms().size() == 0|| molB->GetAtoms().size() == 0 ||
//                 molA->GetAtoms()[0].second == molB->GetAtoms()[0].second)
//             {
//                 continue;
//             }
            
//             if (molA->GetAtoms()[0].second < molB->GetAtoms()[0].second)
//             {
//                 Molecule *T = molB;
//                 molB = molA;
//                 molA = T;
                
//                 btRigidBody *G = obB;
//                 obB = obA;
//                 obA = G;
//             }
            
//             if (molB->GetAtoms().size() > 2)
//             {
//                 continue;
//             }
            
//             AList[obA].insert(obB);
//             BList[obB].insert(obA);
            
            
//             HitList[molA].insert(molB);
//         }

        
//         for (auto x : HitList)
//         {
//             std::vector<Molecule*> NodeDelList;
//             for (auto y : x.second)
//             {
//                 std::vector<btRigidBody*> B = y->GetBodies();
//                 int CollisionCount = 0;
                
//                 for (unsigned int i = 0; i < B.size(); i++)
//                 {
//                     for (auto z : BList[B[i]])
//                     {
//                         if((Molecule*)z->getUserPointer() != x.first)
//                         {
//                             CollisionCount++;
//                         }
//                         if (CollisionCount >= B.size())
//                         {
//                             break;
//                         }
//                     }
//                     if (CollisionCount >= B.size())
//                     {
//                         break;
//                     }
//                 }  
                
//                 if (CollisionCount >= B.size())
//                 {
//                     NodeDelList.push_back(y);
//                     continue;
//                 }
//             }    
//             for (auto z : NodeDelList)     
//             {
//                 x.second.erase(z);
//             }   
//         }



//         const std::vector<std::pair<int, Elements>> Chlorine = {{0,Cl_},{1,Cl_}};
//         const std::vector<std::pair<int, Elements>> Hydrogen = {{0,H_},{1,H_}};
        
//         std::vector<Molecule*> DeleteList;
//         for (auto x : HitList)
//         {
//             if (x.first->GetStatic())
//             {
//                 continue;
//             }
//             for (auto y : x.second)
//             {
//                 if (y->GetStatic())
//                 {
//                     continue;
//                 }
                
                
//                 if (x.first->GetAtoms() == Chlorine && y->GetAtoms() == Hydrogen && x.second.size() == 1)
//                 {
//                     std::vector<btRigidBody*> BodiesA = x.first->GetBodies();
//                     std::vector<std::pair<int, Elements>> AtomsA = x.first->GetAtoms();
                    
//                     std::vector<btRigidBody*> BodiesB = y->GetBodies();
//                     std::vector<std::pair<int, Elements>> AtomsB = y->GetAtoms();
                    
//                     // float dist = glm::length(v3bt2glm(BodiesA[0]->getWorldTransform().getOrigin()) - v3bt2glm(BodiesB[0]->getWorldTransform().getOrigin()));
//                     bool straight = AList[BodiesA[0]].contains(BodiesB[0]);
                    
//                     int ind2 =  !straight;
                    
//                     if (!(AList[BodiesA[0]].contains(BodiesB[ind2]) && AList[BodiesA[1]].contains(BodiesB[1-ind2])))
//                     {
//                         continue;
//                     }
                    
                    
//                     // contruct new molecule
//                     std::vector<std::pair<int, Elements>> Atom1 = {{0,AtomsA[0].second}, {1,AtomsB[ind2].second}};
//                     std::vector<std::pair<int, Elements>> Atom2 = {{0,AtomsA[1].second}, {1,AtomsB[1-ind2].second}};
                    
//                     std::vector<btRigidBody*> Bodies1 = {BodiesA[0], BodiesB[ind2]};
//                     std::vector<btRigidBody*> Bodies2 = {BodiesA[1], BodiesB[1-ind2]};

//                     Reactant1Tot--;
//                     Reactant2Tot--;
//                     Prod2Tot++;
                    
//                     // transfer atoms
//                     BHandler->DelObject(x.first);
//                     Group1->DelObject(x.first);
//                     MoleculesList.erase(std::remove(MoleculesList.begin(), MoleculesList.end(), x.first), MoleculesList.end());
                    
//                     BHandler->DelObject(y);
//                     Group1->DelObject(y);
//                     MoleculesList.erase(std::remove(MoleculesList.begin(), MoleculesList.end(), y), MoleculesList.end());
                    
//                     x.first->SetStatic(true);
//                     x.first->SetEnable(false);
//                     y->SetStatic(true);
//                     y->SetEnable(false);
                    
//                     DeleteList.push_back(x.first);
//                     DeleteList.push_back(y);
                    
//                     Molecule *Mol1 = new Molecule(Atom1, Bodies1);
//                     Molecule *Mol2 = new Molecule(Atom2, Bodies2);
                    
//                     BHandler->AddObject(Mol1);
//                     Group1->AddObject(Mol1);
//                     MoleculesList.push_back(Mol1);
                    
//                     BHandler->AddObject(Mol2);
//                     Group1->AddObject(Mol2);
//                     MoleculesList.push_back(Mol2);
                    
//                     break;
//                 }

//             }
//         }

//         for (auto x : DeleteList)
//         {
//             delete x;
//         }
//         Accumulator -= TimeStep;

//     }
// }

// void update(float dt)
// {
    
//     dt += glm::epsilon<float>();

//     // mtx.lock();
    
//     // if(DoneLoadingPhysicFlag)
//     // {
//     //     for (auto x : MoleculesList)
//     //     {
//     //         x->Physic();
//     //     }

//     //     HNum->SetText(std::to_string(Reactant1Tot));
//     //     ClNum->SetText(std::to_string(Reactant2Tot));
//     //     HClNum->SetText(std::to_string(Prod2Tot));

//     //     BHandler->Update();
//     //     WHandler->Update();
//     //     Group1->Update();
//     //     UIGroup->Update();

//     // }
//     // else
//     // {
//         // }
        
//         // mtx.unlock();
        
//         // glEnable(GL_DEPTH_TEST);
//         // GameBuffer->ResetBuffer();
//         // UIBuffer->ResetBuffer();
//         // if(DoneLoadingPhysicFlag)
//         // {
//             //     Group1->Draw();
//             //     UIGroup->Draw();
//             // }
//             // else
//             // {
//                 // }
//                 glDisable(GL_DEPTH_TEST);
//                 // for (auto x : Programs)
//                 // {
//                     //     x->SetUniform(RenderLookAtMat, "Matrix");
//                     // }
//                     // Background->SetCube(glm::vec3(BgCube,1.0f));
//                     // Background->SetCube({((float)WindowSize.x/(float)WindowSize.y) * 2.0f, 2.0f,1.0f});
//                     // GameRender->Update();
//                     // UIRender->Update();
//                     // glViewport(0, 0, WindowSize.x, WindowSize.y);
//                     // glClear(GL_COLOR_BUFFER_BIT);
//                     // glClearColor(0.0f,0.0f,0.0f,1.0f);
//                     // LoadGroup->Draw();
//                     // GameRender->Draw();
//                     // UIRender->Draw();
                    
//     // LoadGroup->Update();
//     // glViewport(0, 0, WindowSize.x, WindowSize.y);
//     // glClear(GL_COLOR_BUFFER_BIT);
//     // glClearColor(0.0f,0.0f,0.0f,1.0f);
//     // LoadGroup->Draw();
//     // glfwSwapBuffers(MainWindow);
// }

// void UpdateWindows()
// {
//     // UpdateWindow = true;
//     // float WindowAspect = ((float)WindowSize.x)/((float)WindowSize.y);
//     // glm::vec2 CamOffset = {(float)(WindowSize.x%2), (float)(WindowSize.y%2)};
//     // glm::vec2 GameOffset = {0.0f, 0.0f};

//     // GameOffset.x = ((std::max(WindowAspect - GameAspect, 0.0f) / 2.0f) * WindowSize.y);
//     // GameOffset.y = ((std::max((GameAspect/WindowAspect) - 1.0f, 0.0f) / 2.0f) * WindowSize.x);
    
//     // GameOffset += CamOffset;
    
//     // UIRenderer->SetCube({GameAspect*-2.0f, -2.0f, 1.0f});
//     // Renderer->SetCube({GameAspect*-2.0f, -2.0f, 1.0f});


//     // std::cout << WindowAspect << ":Aspect \n";

//     // Background->SetCube({WindowAspect*2.0f, -2.0f, 1.0f});
//     LoadGroup->GetCamera()->SetAspect(WindowAspect);
//     // if (WindowAspect >= GameAspect)
//     // {
//     //     ActualGameSize.x = WindowSize.y * GameAspect;
//     //     ActualGameSize.y = WindowSize.y;
        
//     //     // RenderLookAtMat = glm::ortho( -WindowAspect, WindowAspect, -1.0f, 1.0f, 0.1f, 10.0f );
//     //     // BgCube = {WindowAspect*2.0f, -2.0f};
//     //     // Background->SetCube({WindowAspect*2.0f, 2.0f, 1.0f});
//     // }
//     // else
//     // {
//     //     ActualGameSize.y = WindowSize.x / GameAspect;
//     //     ActualGameSize.x = WindowSize.x;
        
//     //     // RenderLookAtMat = glm::ortho( -GameAspect, GameAspect, -GameAspect/WindowAspect, GameAspect/WindowAspect, 0.1f, 10.0f );
//     //     // BgCube = {GameAspect*-2.0f, (-GameAspect/WindowAspect)*-2.0f};
//     //     // Background->SetCube({GameAspect*2.0f, (GameAspect/WindowAspect)*2.0f, 1.0f});
//     // }
//     // Background->SetCube(glm::vec3(BgCube,1.0f));
//     // std::cout << Background->GetNeedUpdate() << ":Aspect \n";
    
//     // UIRenderer->SetPosition(glm::vec3((CamOffset / (glm::vec2(ActualGameSize) * UIRenderScale)), UIRenderer->GetPosition().z));
//     // Renderer->SetPosition(glm::vec3((CamOffset / (glm::vec2(ActualGameSize) * UIRenderScale)), Renderer->GetPosition().z));
//     // GameBuffer->SetSize(glm::vec2(ActualGameSize) * GameRenderScale);
//     // UIBuffer->SetSize(glm::vec2(ActualGameSize) * UIRenderScale);
    
//     // GameRender->Update();
//     // UIRender->Update();
// }

// void FixRender()
// {
//     auto TimeNow = std::chrono::high_resolution_clock::now();

//     static auto LastUpdate = std::chrono::high_resolution_clock::now();

//     if ((TimeNow - LastUpdate) > std::chrono::milliseconds((int)(FrameTime * 1000.0f)))
//     {
//         LastUpdate = TimeNow;
//         RenderLoop();
//     }
// }

// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {   
//     WindowSize = {width,height};
//     UpdateWindows();
// }
    
    // FixRender();

// void move_callback(GLFWwindow* window, int xpos, int ypos)
// {
//         FixRender();
// }

// void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
// {
//     // WHandler->SetMouseDown(action == GLFW_PRESS);

//     // double xpos, ypos;
//     // glfwGetCursorPos(window, &xpos, &ypos); // on touch, emscripten doesn't emit mouse move event
//     // glm::vec2 ScreenMousePos;
    
//     // ScreenMousePos.x = (2.0f * ((xpos - ((float)(WindowSize.x - ActualGameSize.x)/2.0f))/(float)ActualGameSize.x)) - 1.0f;
//     // ScreenMousePos.y = (2.0f * (1.0f - (ypos - ((float)(WindowSize.y - ActualGameSize.y)/2.0f))/(float)ActualGameSize.y)) - 1.0f;

//     // MousePos = UICam.Screen2World(ScreenMousePos);
//     // WHandler->SetMousePos(MousePos);
// }

// void RenderLoop()
// {
//     static auto LastFrame = std::chrono::high_resolution_clock::now();

//     auto start = std::chrono::high_resolution_clock::now();
//     update(DeltaTime);
//     std::this_thread::sleep_for(std::chrono::milliseconds(int(FrameTime*1000.0f)) - (std::chrono::high_resolution_clock::now() - start));


//     // std::this_thread::sleep_for(std::chrono::milliseconds(int(FrameTime*1000.0f)-1) - (std::chrono::high_resolution_clock::now() - start));
//     // while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start) < std::chrono::milliseconds(int(FrameTime*1000.0f)))
//     // {
//     //     std::this_thread::yield();
//     // }
//     DeltaTime = (float)(std::chrono::duration_cast<std::chrono::milliseconds>(start - LastFrame).count())/1000.0f;
//     LastFrame = start;
// }

void Loop()
{
    /* Poll for and process events */
    // glfwPollEvents();
    #ifndef __EMSCRIPTEN__
    glfwWaitEvents();
    #else
        EventLoop();
        glfwPollEvents();
    #endif
    // RenderLoop();
}

// void LoadGame()
// {

// }

// Entry Point
int main (int argc, char *argv[])
{
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_SAMPLES, 0);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW , GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    // glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_TRUE);

    // glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    /* Create a windowed mode window and its OpenGL context */

    // glfwSetWindowAspectRatio(window, 16.0f, 9.0f);

    /* Make the window's context current */
    // glfwMakeContextCurrent(window);

    MainWindow = glfwCreateWindow(1280, 720, "ChemSim", NULL, NULL);
    if ( !MainWindow )
    {
        std::cout << "unable to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(MainWindow);
    // RegisterFixRender(MainWindow, FixRender);
    // #ifndef __EMSCRIPTEN__
    // SetWindowsIcon(MainWindow);
    // #endif

#ifndef __EMSCRIPTEN__
    if ( glewInit() != GLEW_OK)
    {
        std::cout << "Glew Failed to initialize\n";
    }
#endif
    // std::cout << "Created Window\n";

    // glfwSetWindowSizeCallback(MainWindow, framebuffer_size_callback);
    // glfwSetWindowPosCallback(MainWindow, move_callback);
    // glfwSetMouseButtonCallback(MainWindow, mouse_button_callback);
    // glfwSetWindowRefreshCallback(MainWindow, window_refresh_callback);
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
    

    // release context
    #ifndef __EMSCRIPTEN__
    glfwMakeContextCurrent(NULL);
    #endif
    // Start thread

    EventSetup((void*) MainWindow);
    
    // std::vector<uint8_t> Res;
    
    // Programs.resize(4);
    // Res = GetResource(IDR_BSVSDR);
    // fx_Shader BasicVertex = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "vert");
    // Res = GetResource(IDR_BSFSDR);
    // fx_Shader BasicFragment = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "frag");
    // Programs[fx_BasicType::Basic] = new fx_Program(std::vector<fx_Shader *>({&BasicVertex, &BasicFragment}));
    // Res = GetResource(IDR_SPVSDR);
    // fx_Shader SpriteVertex = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "vert");
    // Res = GetResource(IDR_SPFSDR);
    // fx_Shader SpriteFragment = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "frag");
    // Programs[fx_BasicType::Sprite] = new fx_Program(std::vector<fx_Shader *>({&SpriteVertex, &SpriteFragment}));
    // Res = GetResource(IDR_CRVSDR);
    // fx_Shader CircleVertex = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "vert");
    // Res = GetResource(IDR_CRFSDR);
    // fx_Shader CircleFragment = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "frag");
    // Programs[fx_BasicType::Circle] = new fx_Program(std::vector<fx_Shader *>({&CircleVertex, &CircleFragment}));
    // Res = GetResource(IDR_TXVSDR);
    // fx_Shader TextVertex = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "vert");
    // Res = GetResource(IDR_TXFSDR);
    // fx_Shader TextFragment = fx_Shader(GLSL_VER + std::string(Res.begin(), Res.end()), "frag");
    // Programs[fx_BasicType::SDF] = new fx_Program(std::vector<fx_Shader *>({&TextVertex, &TextFragment}));
    // Lib = fx_Load_Lib();
    // TODO: Implement the cache
    // TimesFace = fx_Load_Face(Lib, "data/Times.ttf");
    // UIRender = new fx_Group(Programs, NULL);
    // GameRender = new fx_Group(Programs, NULL);


    // GameBuffer = new fx_Framebuffer();
    // UIBuffer = new fx_Framebuffer();

    // glm::mat4 RenderLookAtMat = glm::ortho( -GameAspect, GameAspect, -1.0f, 1.0f, 0.1f, 10.0f );
    
    // LoadGroup = new fx_Group(Programs, new fx_Texture(Icon));
    // LoadGroup->SetCamera(new fx_Orthographic({0.0,0.0,2.5}, GameAspect));

    // fx_Sprite *Loading = new fx_Sprite({0,0,-1}, {1.0f,1.0f}, {0.0,0.0,1.0,1.0});
    // Loading->SetAnchor({0.5f,0.5f,0.0f});
    // LoadGroup->AddObject(Loading);

    // LoadGroup->Update();


    // glfwSwapBuffers(MainWindow);
    // for (auto x : Programs)
    // {
    //     x->SetUniform(RenderLookAtMat, "Matrix");
    // }
    // GameRender->Draw();
    // UIRender->Draw();
    // glfwSwapBuffers(MainWindow);
    
    // LoadGroup->m_FrameBuffer = GameBuffer;
    
    // RenderLookAtMat = glm::identity<glm::mat4>();
    // GameBuffer->SetSize(glm::vec2(ActualGameSize) * GameRenderScale);
    // UIBuffer->SetSize(glm::vec2(ActualGameSize) * UIRenderScale);

    // GameRender->m_TextureUnit = GameBuffer->GetColorAttachment();
    // UIRender->m_TextureUnit = UIBuffer->GetColorAttachment();

    // Renderer = new fx_Sprite({0.0f,0.0f,-2.0f}, {2.0f, -2.0f}, {1.0f,0.0f,0.0f,1.0f});

    // Renderer->SetAnchor({0.5f,0.5f,0.0f});
    // GameRender->AddObject(Renderer);

    // UIRenderer = new fx_Sprite({0.0f,0.0f,-2.0f}, {2.0f, -2.0f}, {1.0f,0.0f,0.0f,1.0f});

    // UIRenderer->SetAnchor({0.5f,0.5f,0.0f});
    // UIRender->AddObject(UIRenderer);

    // Background = new fx_Quad({0, 0.0f, -0.5f}, {2.0f, 2.0f});
    // Background->SetAnchor({0.5f,0.5f,0.0f});
    // Background->SetEnable(false);
    // GameRender->AddObject(Background);

    // UpdateWindows();
    // RenderLoop();

    // Background->SetEnable(true);
    
    // std::thread Phy(PhysicsLoop);

	// ///-----initialization_end-----


    // Group1 = new fx_Group(Programs, NULL);
    // Group1->SetCamera(&ObjCam);
    // // Group2 = new fx_Group(Programs, fx_InitBufferMap(), NULL);
    // UIGroup = new fx_Group(Programs, NULL);
    // UIGroup->SetCamera(&UICam);
    



    // Group1->m_FrameBuffer = GameBuffer;
    // UIGroup->m_FrameBuffer = UIBuffer;




    // // TODO: manual add, tutorial, licenses, info, clear screen, drawing meaning, stats, molecule drag
    
    // // Handle minimize, ogl keep throwing error

    // // put initialize in loading thread
    // //  cannot, opengl call need to be in 1 thread

    // fx_Font *FontObj = new fx_Font(FontFile);
    // std::vector<fx_Image> UIImageList = FontObj->GetAtlas().ImagesList;

    // const unsigned int FontOffset = UIImageList.size();

    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICUP)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICDN)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICLF)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICRG)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_IC360)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICADC)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICADD)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICCLOSE)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICRESET)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICTEMP)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICIN)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICOUT)));
    // UIImageList.push_back(fx_Image::LoadPNG(GetResource(IDR_ICLSC)));

    // for (auto &x : UIImageList)
    // {
    //     if (x.Component == 1)
    //     {
    //         x.Data = ColorConvert::Gray2RGBA(x.Data);
    //         x.Component = 4;
    //     }
    //     else if (x.Component == 2)
    //     {
    //         x.Data = ColorConvert::GrayA2RGBA(x.Data);
    //         x.Component = 4;
    //     }
    //     else if (x.Component == 3)
    //     {
    //         x.Data = ColorConvert::RGB2RGBA(x.Data);
    //         x.Component = 4;
    //     }
    //     else if (x.Component == 4)
    //     {
            
    //     }
    //     else
    //     {
    //         std::cout << "Unknown Type\n";
    //     }
    // }

    // fx_Atlas UIAtlas = fx_Atlas::PackImages(UIImageList);
    // FontObj->SetAtlas(UIAtlas);
    
    // fx_Texture *UITexture = new fx_Texture(UIAtlas.Image);

    // // FontImg.EncodePNG()

    // UIGroup->m_TextureUnit = UITexture;
    // LoadGroup->m_TextureUnit = UITexture;

    // BHandler = new fx_BillboardHandler();

    // WHandler = new fx_WidgetHandler();

    // glm::vec4 BDefault = {0.36,0.36,0.36,1};;
    // glm::vec4 BDown = {0.3,0.3,0.3,1};
    // glm::vec4 BDisable = {0.16,0.16,0.16,1};


    // fx_Button *Button1 = new fx_Button({0.69,2.25,-1}, {0.5f,0.5f}, fx_Atlas::GetUV(IDR_IC360 - IDR_ICUP + FontOffset, UIAtlas), BDefault, BDown, BDisable, {1,1,1,1});

    // UIGroup->AddObject(Button1);
    // WHandler->AddObject(Button1);

    // fx_Button *Button2 = new fx_Button({1.19,2.25,-1}, {0.5f,0.5f}, fx_Atlas::GetUV(IDR_ICIN - IDR_ICUP + FontOffset, UIAtlas), BDefault, BDown, BDisable, {1,1,1,1});

    // fx_Button *Button3 = new fx_Button({1.69,2.25,-1}, {0.5f,0.5f}, fx_Atlas::GetUV(IDR_ICOUT - IDR_ICUP + FontOffset, UIAtlas), BDefault, BDown, BDisable, {1,1,1,1});

    // UIGroup->AddObject(Button2);
    // WHandler->AddObject(Button2);

    // UIGroup->AddObject(Button3);
    // WHandler->AddObject(Button3);

    // fx_Button *Button4 = new fx_Button({2.69,2.25,-1}, {0.5f,0.5f}, fx_Atlas::GetUV(IDR_ICADC - IDR_ICUP + FontOffset, UIAtlas), BDefault, BDown, BDisable, {1,1,1,1});

    // fx_Button *Button5 = new fx_Button({3.19,2.25,-1}, {0.5f,0.5f}, fx_Atlas::GetUV(IDR_ICADD - IDR_ICUP + FontOffset, UIAtlas), BDefault, BDown, BDisable, {1,1,1,1});

    // UIGroup->AddObject(Button4);
    // WHandler->AddObject(Button4);

    // UIGroup->AddObject(Button5);
    // WHandler->AddObject(Button5);

    // fx_Button *Button6 = new fx_Button({3.69,2.25,-1}, {0.5f,0.5f}, fx_Atlas::GetUV(IDR_ICRESET - IDR_ICUP + FontOffset, UIAtlas), BDefault, BDown, BDisable, {1,1,1,1});

    // fx_Button *Button7 = new fx_Button({2.19,2.25,-1}, {0.5f,0.5f}, fx_Atlas::GetUV(IDR_ICTEMP - IDR_ICUP + FontOffset, UIAtlas), BDefault, BDown, BDisable, {1,1,1,1});

    // fx_Button *Button8 = new fx_Button({4.19,2.25,-1}, {0.5f,0.5f}, fx_Atlas::GetUV(IDR_ICLSC - IDR_ICUP + FontOffset, UIAtlas), BDefault, BDown, BDisable, {1,1,1,1});

    // UIGroup->AddObject(Button6);
    // WHandler->AddObject(Button6);

    // UIGroup->AddObject(Button7);
    // WHandler->AddObject(Button7);

    // UIGroup->AddObject(Button8);
    // WHandler->AddObject(Button8);
    // // 5.0f, 1.0f, 0.0f, FontObj, "ABCDEFGHIJKLM\n aaa aaa aaa bbb OPQRSTUVWXYZ aaa"
    // Res = GetResource(IDR_ITXTNT);
    // fx_TextBox *Box = new fx_TextBox({0,0,-1}, 0.25f, 8.0f, FontObj, std::string(Res.begin(), Res.end()),{1,1,1,1}, {.5,.5,.5,1});
    // Box->SetAnchor({0.5f,0.5f,0.0f});
    // Box->SetLineSpacing(0.7f);
    // Box->SetEnable(false);
    // // Box->SetAlign(0.5f);
    // UIGroup->AddObject(Box);

    // ModelMolecule *HModel = new ModelMolecule({{0, H_}, {1, H_}},{-4.25f,2.25f,0.0f},0.2f);
    // UIGroup->AddObject(HModel);

    // HNum = new fx_Text({-3.9f,2.25f,0.0f}, 0.3f, FontObj, "888");
    // HNum->SetAnchor({0.0f,0.5f,0.0f});
    // UIGroup->AddObject(HNum);

    // ModelMolecule *ClModel = new ModelMolecule({{0, Cl_}, {1, Cl_}},{-3.25f,2.25f,0.0f},0.2f);
    // UIGroup->AddObject(ClModel);

    // ClNum = new fx_Text({-2.9f,2.25f,0.0f}, 0.3f, FontObj, "888");
    // ClNum->SetAnchor({0.0f,0.5f,0.0f});
    // UIGroup->AddObject(ClNum);

    // ModelMolecule *HClModel = new ModelMolecule({{0, Cl_}, {1, H_}},{-2.25f,2.25f,0.0f},0.2f);
    // UIGroup->AddObject(HClModel);

    // HClNum = new fx_Text({-1.9f,2.25f,0.0f}, 0.3f, FontObj, "888");
    // HClNum->SetAnchor({0.0f,0.5f,0.0f});
    // UIGroup->AddObject(HClNum);
    
    // BHandler->SetCameraPos(ObjCam.GetPosition());
    // BHandler->SetCameraUp(glm::vec3(0,1,0) * ObjCam.GetQuat());
    // // std::cout << "b\n";

    // // float Angle = 0;

    // ObjCam.SetFar(50.0f);

    // // fx_TextBox *Box1 = new fx_TextBox({0,0,-1}, 2.0f, 10.0f, FontObj, "ABC",{1,1,1,1}, {.5,.5,.5,1});
    // // Box1->SetAnchor({0.5f,0.5f,0.0f});
    // // Box1->SetLineSpacing(0.7f);
    // // Box1->SetAlign(0.5f);
    // // UIGroup->AddObject(Box1);

    // // ObjCam.SetSize(1.0f);

    // Button1->m_MainActionCallback = [&]() {
    //     glm::vec3 CamPos = ObjCam.GetPosition();
    //     float Angle = std::atan2(CamPos.x, CamPos.z);
    //     Angle += glm::pi<float>() * DeltaTime;

    //     float CamLenght = glm::length(CamPos);


    //     CamPos.x = std::sin(Angle) * CamLenght;
    //     CamPos.y = 0;
    //     CamPos.z = std::cos(Angle) * CamLenght;

    //     ObjCam.SetPosition(CamPos);
    //     ObjCam.SetQuat(glm::quat(glm::vec3(0.0f,-Angle,0.0f)));
        
    //     BHandler->SetCameraPos(CamPos);
    //     BHandler->SetCameraUp(glm::vec3(0,1,0) * glm::quat(glm::vec3(0.0f,-Angle,0.0f)));

    // };
    
    // Button1->m_HoldActionCallback = Button1->m_MainActionCallback ;

    // Button2->m_MainActionCallback = [&]() {
    //     glm::vec3 CamPos = ObjCam.GetPosition();
    //     float Angle = std::atan2(CamPos.x, CamPos.z);

    //     // glm::vec3 Delta = glm::normalize(glm::abs(ObjCam.GetPosition()));

    //     float CamLenght = glm::length(CamPos) - (glm::pi<float>() * 2.0f * DeltaTime);

    //     // CamPos = CamPos - (Delta * glm::pi<float>() * DeltaTime);

    //     CamPos.x = std::sin(Angle) * CamLenght;
    //     CamPos.y = 0;
    //     CamPos.z = std::cos(Angle) * CamLenght;

    //     ObjCam.SetPosition(CamPos);
    //     BHandler->SetCameraPos(CamPos);
    // };
    // Button2->m_HoldActionCallback = Button2->m_MainActionCallback ;


    // Button3->m_MainActionCallback = [&]() {
    //     // Angle += glm::pi<float>() *DeltaTime;
    //     glm::vec3 CamPos = ObjCam.GetPosition();
    //     float Angle = std::atan2(CamPos.x, CamPos.z);

    //     float CamLenght = glm::length(CamPos) + (glm::pi<float>() * 2.0f * DeltaTime);

    //     CamPos.x = std::sin(Angle) * CamLenght;
    //     CamPos.y = 0;
    //     CamPos.z = std::cos(Angle) * CamLenght;

    //     ObjCam.SetPosition(CamPos);
    //     BHandler->SetCameraPos(CamPos);
    // };
    // Button3->m_HoldActionCallback = Button3->m_MainActionCallback ;

    // Button4->m_MainActionCallback = [&]() {
    //     Reactant1Tot++;
    //     Molecule *M1 = new Molecule({{0, H_}, {1, H_}},{8.0f,5.0f,0.0f});
    //     M1->SetQuat(glm::quat(glm::vec3(AngDist(Gen), AngDist(Gen), AngDist(Gen))));
    //     M1->SetVelocity(glm::normalize(glm::vec3(-std::abs(Veldist(Gen)), Veldist(Gen), Veldist(Gen))) * MoleculeSpawnVel);
    //     BHandler->AddObject(M1);
    //     Group1->AddObject(M1);
    //     MoleculesList.push_back(M1);
    // };
    // Button4->m_HoldActionCallback = [&]() {

    //     static float Accumulator = 0.0f;
    //     const float TimeStep = 0.4;
    //     Accumulator += DeltaTime;

    //     while(Accumulator >= TimeStep)
    //     {
    //         Button4->m_MainActionCallback();
    //         Accumulator -= TimeStep;
    //     }
    // };


    // Button5->m_MainActionCallback = [&]() {
    //     Reactant2Tot++;
    //     Molecule *M1 = new Molecule({{0, Cl_}, {1, Cl_}},{-8.0f,5.0f,0.0f});
    //     M1->SetQuat(glm::quat(glm::vec3(AngDist(Gen), AngDist(Gen), AngDist(Gen))));
    //     M1->SetVelocity(glm::normalize(glm::vec3(std::abs(Veldist(Gen)), Veldist(Gen), Veldist(Gen))) * MoleculeSpawnVel);
    //     BHandler->AddObject(M1);
    //     Group1->AddObject(M1);
    //     MoleculesList.push_back(M1);
    // };
    // Button5->m_HoldActionCallback = [&]() {

    //     static float Accumulator = 0.0f;
    //     const float TimeStep = 0.4;
    //     Accumulator += DeltaTime;

    //     while(Accumulator >= TimeStep)
    //     {
    //         Button5->m_MainActionCallback();
    //         Accumulator -= TimeStep;
    //     }
    // };

    // Button6->m_MainActionCallback = [&]() {
    //     Reactant1Tot = 0;
    //     Reactant2Tot = 0;
    //     Prod2Tot = 0;
    //     for (auto x : MoleculesList)
    //     {
    //         BHandler->DelObject(x);
    //         Group1->DelObject(x);
    //         delete x;

    //     }
    //     MoleculesList.clear();
    //     Group1->Update();
    // };

    // Button7->m_MainActionCallback = [&]() {
    //     for (auto x : MoleculesList)
    //     {
    //         for (auto y : x->GetBodies())
    //         {
    //             glm::vec3 v = v3bt2glm(y->getLinearVelocity());
    //             glm::vec3 dir = glm::normalize(v);
    //             float mag = glm::length(v);
    //             y->setLinearVelocity(v3glm2bt(dir * std::min(mag*1.2f, MoleculeMaxVel)));
    //         }
    //     }
    // };
    // Button7->m_HoldActionCallback = Button7->m_MainActionCallback;

    // Button8->m_MainActionCallback = [&]() {
    //     bool status = Box->GetEnable();
    //     Box->SetEnable(!status);
    //     TimeScale = status;
    //     Button1->SetWidgetEnable(status);
    //     Button2->SetWidgetEnable(status);
    //     Button3->SetWidgetEnable(status);
    //     Button4->SetWidgetEnable(status);
    //     Button5->SetWidgetEnable(status);
    //     Button6->SetWidgetEnable(status);
    //     Button7->SetWidgetEnable(status);
    // };


    // // UIGroup->GenerateMesh();
    // UIGroup->Update();

    // // Group1->GenerateMesh();
    // Group1->Update();

    // // GameRender->GenerateMesh();
    // GameRender->Update();

    // std::cout << "Scene done loading\n";

    // UpdateWindows();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(Loop, 0, true);
#else
    
    while ( !glfwWindowShouldClose(MainWindow) )
    {
        Loop();
    }

#endif
    // mtx.lock();
    // FlagRunPhysics = false;
    // mtx.unlock();
    EventExit();
    glfwTerminate();
    // Phy.join();
    // physicsCommon.destroyPhysicsWorld(PhysicWorld);

    //End Program
    return 0;
}