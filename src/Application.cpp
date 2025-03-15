#define GLEW_STATIC

// Include standard libraries
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <random>
#include <sstream>
// #include <memory>

// #define __EMSCRIPTEN__

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define GLSL_VER "#version 300 es\n"
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GLES3/gl32.h>
#else
#include <GL/glew.h>
#define GLSL_VER "#version 330 core\n"
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>


// #include "File.hpp"
#include "Shader.hpp"
#include "Object.hpp"
#include "Resource.hpp"
#include "ColorConvert.hpp"
#include "Font.hpp"
// #include "Embed.hpp"
// #include "Time.hpp"
#include "Physics.hpp"
// #include "Game.hpp"
#include "Widget.hpp"

#include "ARIAL.ttf.h"
#include "Chemsim.png.h"


#include "Basic.frag.h"
#include "Basic.vert.h"
#include "Circle.frag.h"
#include "Circle.vert.h"
#include "Sprite.frag.h"
#include "Sprite.vert.h"
#include "Text.frag.h"
#include "Text.vert.h"

#include "Res.rc"

static GLenum ErrorCode;
static const GLubyte *ErrorString;

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

// inline glm::vec2 toGlm(const b2Vec2 &v) {
//     return glm::vec2(v.x, v.y);
// }

// inline b2Vec2 toB2(const glm::vec2 &v) {
//     return b2Vec2(v.x, v.y);
// }

// static b2World *world = new b2World(b2Vec2(0.0f,0.0f));

glm::vec2 VecRotate(glm::vec2 Vec, float Ang)
{
    glm::vec2 Result;
    Result.x = (Vec.x * std::cos(Ang)) - (Vec.y * std::sin(Ang));  
    Result.y = (Vec.x * std::sin(Ang)) + (Vec.y * std::cos(Ang)); 
    return Result;
}

// static float Atom2Screen = 0.1f/50.0f;

// enum Elements
// {
//     H,
//     O,
//     N,
//     C,
//     Cl,
//     I
// };

// struct AtomDef
// {
//     glm::vec3 Color = {1,1,1};
//     float Radius;
//     float Mass;
//     Elements Elmnt;
// };

// // CPK colouring
// // https://sciencenotes.org/molecule-atom-colors-cpk-colors/
// const static std::vector<AtomDef> ElementsPreset = {
//     {{1.0f,1.0f,1.0f}, 25.0f, 1.008f, Elements::H},
//     {{1.0f,0.051f,0.051}, 60.0f, 15.999f, Elements::O},
//     {{0.188f, 0.314f, 0.973f}, 65.0f, 14.007f, Elements::N},
//     {{0.5625f,0.5625f,0.5625f}, 70.0f, 12.011f, Elements::C},
//     {{0.122f,0.941f,0.122f}, 100.0f, 35.45f, Elements::Cl},
//     {{0.58,0.0f,0.58f}, 140.0f, 126.90447f, Elements::I},
// };

// class Atom;
// // class Bond;
// // class Molecule;

// static std::vector<Atom*> AtomsObj;
// // static std::vector<Bond*> BondsObj;

// class Atom : public fx_Complex
// {
// private:
// public:
//     fx_Circle *m_Circle;
//     // PhysicsBody2D m_Body;
//     // Circle2D m_Coll;
//     // float m_mass = 1;
//     // void *m_Parent = nullptr;
//     // glm::vec2 m_acc = {0.0f , 0.0f};
//     // glm::vec2 m_vel = {0.0f , 0.0f};
//     // glm::vec2 m_offset = {0.0f, 0.0f};
//     Elements m_Element;
//     bool m_VisibleBond = false;
//     // Molecule *m_Molecule;
//     std::vector<b2Joint*> m_Bonds;
//     std::vector<Atom*> m_ChildAtom;
//     Atom *m_Parent = nullptr;
//     std::vector<fx_Quad*> m_Line;
//     b2Body *m_Body;
//     Atom(glm::vec3 Pos, glm::vec2 Scale, glm::vec4 Color = {1,1,1,1});
//     Atom(AtomDef Definition, glm::vec2 Pos = {0.0f, 0.0f});
//     ~Atom();
//     void Update();
//     void TransferParent(Atom *NewParent);
// };

// Atom::Atom(glm::vec3 Pos, glm::vec2 Scale, glm::vec4 Color)
// {
//     AtomsObj.push_back(this);
//     m_Info.m_Size = glm::vec3(Scale, 1.0f);
//     m_Info.m_Position = Pos;
//     m_Info.m_Color = Color;
//     m_Objects = {};
//     m_Drawable = true;
//     m_Complex = true;

//     b2BodyDef BodyDef;
//     BodyDef.type = b2_dynamicBody;
//     BodyDef.position.Set(Pos.x, Pos.y);
//     BodyDef.bullet = true;

//     m_Body = world->CreateBody(&BodyDef);
//     b2BodyUserData &data = m_Body->GetUserData();
//     data.pointer = (uintptr_t)this;

//     m_Body = world->CreateBody(&BodyDef);
//     b2CircleShape circle;
//     // circle.m_p.Set(Pos.x, Pos.y);
//     circle.m_radius = Scale.x / 2.0f;

//     b2FixtureDef fixtureDef;
//     fixtureDef.shape = &circle;
//     fixtureDef.density = 1.0f;
//     fixtureDef.friction = 1.0f;
//     fixtureDef.restitution = 0.9f;

//     m_Body->CreateFixture(&fixtureDef);

//     // m_Body->ApplyForce(b2Vec2(0.0f, -1.0f), m_Body->GetPosition(), true);

//     m_Circle = new fx_Circle(Pos, glm::vec2(m_Info.m_Size.x, m_Info.m_Size.y), Color);
//     m_Circle->m_Info.m_Anchor = {0.5f, 0.5f, 0};
//     m_Circle->Update();
//     m_Objects.push_back(m_Circle);
// }

// Atom::Atom(AtomDef Definition, glm::vec2 Pos)
// {
//     AtomsObj.push_back(this);
//     m_Element = Definition.Elmnt;
//     float Radius = Definition.Radius * Atom2Screen;
//     m_Info.m_Size = glm::vec3(Radius*2, Radius*2, 1.0f);
//     m_Info.m_Position = glm::vec3(Pos,-2.0f);
//     m_Info.m_Color = glm::vec4(Definition.Color, 1.0f);
//     m_Info.m_Anchor = {0.5f, 0.5f, 0};
//     m_Objects = {};
//     m_Drawable = true;
//     m_Complex = true;

//     b2BodyDef BodyDef;
//     BodyDef.type = b2_dynamicBody;
//     BodyDef.position.Set(m_Info.m_Position.x, m_Info.m_Position.y);
//     BodyDef.bullet = true;

//     m_Body = world->CreateBody(&BodyDef);
//     b2BodyUserData &data = m_Body->GetUserData();
//     data.pointer = (uintptr_t)this;

//     b2CircleShape circle;
//     // circle.m_p.Set(Pos.x, Pos.y);
//     circle.m_radius = Radius;

//     b2FixtureDef fixtureDef;
//     fixtureDef.shape = &circle;
//     fixtureDef.density =  Definition.Mass / ((4.0f/3.0f) * glm::pi<float>() * std::pow(Definition.Radius,3));
//     fixtureDef.friction = 1.0f;
//     fixtureDef.restitution = 1.0f;

//     m_Body->CreateFixture(&fixtureDef);

//     // m_Body->ApplyForce(b2Vec2(0.0f, -1.0f), m_Body->GetPosition(), true);

//     m_Circle = new fx_Circle(m_Info.m_Position, glm::vec2(m_Info.m_Size.x, m_Info.m_Size.y), m_Info.m_Color);
//     m_Circle->m_Info.m_Anchor = {0.5f, 0.5f, 0};
//     m_Circle->Update();
//     m_Objects.push_back(m_Circle);
// }

// Atom::~Atom()
// {
//     for (auto &x : m_Line)
//     {
//         delete x;
//     }
//     delete m_Circle;
//     world->DestroyBody(m_Body);
//     EraseElement(AtomsObj,this);
// }

// void Atom::Update()
// {
//     if (!m_Body)
//     {
//         return;
//     }
//     b2Vec2 Pos = m_Body->GetPosition();
//     m_Info.m_Position.x = Pos.x;
//     m_Info.m_Position.y = Pos.y;

//     m_Info.m_Rotation = glm::quat({0,0, m_Body->GetAngle()});

//     for (auto &x: m_Line)
//     {
//         delete x;
//     }
//     m_Line.clear();
//     m_Objects.clear();

//     if (m_VisibleBond)
//     {
//         for (auto &x: m_ChildAtom)
//         {
//             glm::vec2 Dir = glm::vec2(-m_Info.m_Position + x->m_Info.m_Position);

//             fx_Quad *Line = new fx_Quad({m_Info.m_Position.x, m_Info.m_Position.y, -3.0f}, glm::vec2(glm::length(Dir), 0.05), glm::vec4({0.5f, 0.5f, 0.5f, 1.0f}));
//             Line->m_Info.m_Anchor = {0.0f, 0.5f, 0};
//             Line->m_Info.m_Rotation = glm::quat(glm::vec3(0,0, std::atan2(Dir.y, Dir.x)));
//             Line->Update();
//             m_Line.push_back(Line);
//             m_Objects.push_back(Line);
//         }
//     }
//     for (auto &x: m_ChildAtom)
//     {
//         m_Objects.push_back(x);
//     }

//     m_Objects.push_back(m_Circle);
//     m_Circle->m_Info = m_Info;
//     m_Circle->Update();
// }

// void Atom::TransferParent(Atom *NewParent)
// {
//     NewParent->m_Parent = nullptr;
//     m_Parent = NewParent;
//     for (auto &x : m_ChildAtom)
//     {
//         if (x == NewParent)
//         {
//             continue;
//         }
//         x->m_Parent = NewParent;
//         NewParent->m_ChildAtom.push_back(x);
//     }
//     NewParent->m_ChildAtom.push_back(this);
//     m_ChildAtom.clear();
//     NewParent->m_Bonds = m_Bonds;
//     m_Bonds.clear();
//     Update();
//     NewParent->Update();
// }


// void InitMolecule(glm::vec2 Pos, std::vector<std::pair<Elements,float>> Atoms, fx_Group *Render, glm::vec2 Force = {0.0f,0.0f})
// {
//     float RefAngle = Atoms[0].second;

//     Atom *Atom0 = new Atom(ElementsPreset[Atoms[0].first], Pos);
//     Atom0->m_Body->ApplyForce(toB2(Force), Atom0->m_Body->GetPosition(), true);
//     Render->m_Objects.push_back(Atom0);


//     for (int i = 1; i < (int)Atoms.size(); i++)
//     {
//         float Angle = RefAngle + Atoms[i].second;
//         glm::vec2 RelPos = {std::cos(Angle), std::sin(Angle)};
//         RelPos = (ElementsPreset[Atoms[0].first].Radius + ElementsPreset[Atoms[i].first].Radius) * Atom2Screen * RelPos;


//         Atom *Matter = new Atom(ElementsPreset[Atoms[i].first], Pos + RelPos);
//         Matter->m_Parent = Atom0;
//         Atom0->m_ChildAtom.push_back(Matter);
//         Render->m_Objects.push_back(Matter);

//         b2Vec2 Pivot = toB2(glm::vec2(Atom0->m_Info.m_Position) + (glm::normalize(glm::vec2(-Atom0->m_Info.m_Position + Matter->m_Info.m_Position)) * (Atom0->m_Info.m_Size.x / 2.0f) ));

//         b2WeldJointDef jointDef;
//         jointDef.Initialize(Atom0->m_Body, Matter->m_Body, Pivot);
//         jointDef.collideConnected = false;
//         b2Joint *Joint = world->CreateJoint(&jointDef);
//         Atom0->m_Bonds.push_back(Joint);
//         Matter->m_Body->ApplyForce(toB2(Force), Matter->m_Body->GetPosition(), true);
//     }
// }


// struct ReactionData
// {
//     std::vector<Atom *> Atoms;
//     bool Startflag = false;
//     bool Endflag = false;
// };

// static std::vector<ReactionData> ReactionStack = {};
// static std::vector<std::pair<Atom*,Atom*>> CollisionList = {};

// class AtomContactListener : public b2ContactListener
// {
//     void BeginContact(b2Contact* contact) {
//         Atom *A = (Atom*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
//         Atom *B = (Atom*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
//         if (!A || !B) // check for walls
//         {
//             return;
//         }
//         CollisionList.push_back({A,B});
//         // if (A->m_Bonds.size() == 0) 
//         // {
//         //     return;
//         // }
//         // if (B->m_Bonds.size() == 0)
//         // {
//         //     return;
//         // }
//         // Atom *A1t = (Atom*)A->m_Bonds[0]->GetBodyA()->GetUserData().pointer;
//         // Atom *A2t = (Atom*)A->m_Bonds[0]->GetBodyB()->GetUserData().pointer;
//         // Atom *B1t = (Atom*)B->m_Bonds[0]->GetBodyA()->GetUserData().pointer;
//         // Atom *B2t = (Atom*)B->m_Bonds[0]->GetBodyB()->GetUserData().pointer;
//         Atom *A1 = A;
//         Atom *A2;
//         Atom *B1 = B;
//         Atom *B2;

//         if (!A1->m_Parent && A1->m_ChildAtom.size() > 0)
//         {
//             A2 = A1->m_ChildAtom[0];
//         }
//         else
//         {
//             A2 = A1->m_Parent;
//         }

//         if (!B1->m_Parent && B1->m_ChildAtom.size() > 0)
//         {
//             B2 = B1->m_ChildAtom[0];
//         }
//         else
//         {
//             B2 = B1->m_Parent;
//         }
        
//         if (A1->m_Element != A2->m_Element || B1->m_Element != B2->m_Element || A1->m_Element == B1->m_Element || A2->m_Element == B2->m_Element)
//         {
//             return;
//         }

//         if (A1->m_Bonds.size() == 0 && A2->m_Bonds.size() == 0 && B1->m_Bonds.size() == 0 && B2->m_Bonds.size() == 0)
//         {
//             return;
//         }

//         float BMolDist = glm::length(glm::vec2(-A2->m_Info.m_Position + B2->m_Info.m_Position));

//         if (BMolDist > ((A2->m_Info.m_Size.x/2.0f) + (B2->m_Info.m_Size.x/2.0f))*1.25f) // if close enough
//         {
//             return;
//         }
        
//         auto it = std::find(CollisionList.begin(), CollisionList.end(), std::make_pair(A2,B2)); 
//         if (it == CollisionList.end())
//         {
//             return;
//         }
        
//         ReactionData Result;
//         Result.Atoms.push_back(A1);
//         Result.Atoms.push_back(A2);
//         Result.Atoms.push_back(B1);
//         Result.Atoms.push_back(B2);

//         for (auto &x : ReactionStack)
//         {
//             if (((Result.Atoms[0] == x.Atoms[0]) || (Result.Atoms[0] == x.Atoms[1])) && ((Result.Atoms[2] == x.Atoms[2]) || (Result.Atoms[2] == x.Atoms[3])))
//             {
//                 return;                    
//             }
//         }

//         ReactionStack.push_back(Result);
//         CollisionList.erase(it);
//     }
  
//     void EndContact(b2Contact* contact) {
//         Atom *A = (Atom*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
//         Atom *B = (Atom*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
//         if (A && B)
//         {
//             EraseElement(CollisionList, std::make_pair(A,B));
//         }
//         // std::cout << CollisionList.size() << "\n";

//     }
// };

// static std::unordered_map<fx_BasicType, fx_Program*> Programs;
// static fx_Font_Library Lib;


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
static float GameScale = 5.0f;
static float TimeScale = 0.25f;
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

static glm::mat4 LookAtMat;
static glm::mat4 GameLookAtMat;
static glm::mat4 UILookAtMat;
static glm::mat4 RenderLookAtMat;

static fx_Font *Arial;

static bool Nostep = false;

static int Reactant1Tot = 0;
static int Reactant2Tot = 0;

static fx_BillboardCircle *Circle1;
static fx_BillboardCircle *Circle2;
static fx_BillboardCircle *Circle3;
static fx_BillboardCircle *Circle4;
static fx_BillboardCircle *Circle5;
static fx_BillboardCircle *Circle6;
static fx_BillboardCircle *Circle7;
static fx_BillboardCircle *Circle8;
static fx_BillboardCircle *Circle9;

static fx_BillboardLine *Line1;

static fx_Text *Text;

static fx_Perspective ObjCam({0.0,0.0,2.5}, GameAspect);
static fx_Orthographic UICam({0.0,0.0,2.5}, GameAspect);

static Line3D MousePos;

static fx_WidgetHandler *WHandler;
static fx_BillboardHandler *BHandler;
// static fx_Circle *Circle2;

// static fx_Text *Text;

// static std::vector<fx_Widget*> Buttons;

// static fx_Button *Button1;

// static fx_GUILayer *GUI;

// static  AtomContactListener AtomContactListenerInstance;

// static Atom *Atom1;
// static Atom *Atom2;

// static Bond *Bond1;


// static b2Body* m_body;
// static b2Body* m_walln;
// static b2Body* m_walls;
// static b2Body* m_walle;
// static b2Body* m_wallw;

// static glm::ivec2 StartMouse = {0,0};
// static Atom* MouseSelect = nullptr;

// static std::default_random_engine Gen;
// static std::uniform_real_distribution<float> Posdist(-GameScale + (Atom2Screen*50.0f),GameScale - (Atom2Screen*50.0f));
// static std::uniform_real_distribution<float> AngDist(-glm::pi<float>(),glm::pi<float>());

// float Random()
// {
//     return ((float)std::rand())/((float)RAND_MAX);
// }

// void Reaction(float dt)
// {
//     std::vector<ReactionData> TbDelete = {};
//     for (auto &x : ReactionStack)
//     {
//         Atom *A1 = x.Atoms[0];
//         Atom *A2 = x.Atoms[1];
//         Atom *B1 = x.Atoms[2];
//         Atom *B2 = x.Atoms[3];
//         if (!x.Startflag && !x.Endflag)
//         {
//             x.Startflag = true;

//             // check if A1 is parent
//             if (A1->m_Parent)
//             {
//                 A2->TransferParent(A1);
//             }
            
//             for (auto &x : A1->m_Bonds)
//             {
//                 world->DestroyJoint(x);
//             }
//             A1->m_Bonds.clear();


//             // check if B1 is parent
//             if (!B1->m_Parent)
//             {
//                 B1->TransferParent(B2);
//             }

//             for (auto &x : B2->m_Bonds)
//             {
//                 world->DestroyJoint(x);
//             }
//             B2->m_Bonds.clear();
//             A1->m_VisibleBond = true;
//             B2->m_VisibleBond = true;

//             A1->m_ChildAtom.push_back(B1);
//             B2->m_ChildAtom.push_back(A2);
            
//             glm::vec2 Aaxis = glm::normalize(-A1->m_Info.m_Position + A2->m_Info.m_Position);
//             glm::vec2 Baxis = glm::normalize(-B1->m_Info.m_Position + B2->m_Info.m_Position);
//             A1->m_Body->ApplyForce(toB2(-Aaxis * 0.00005f) ,toB2(A1->m_Info.m_Position), true);
//             A2->m_Body->ApplyForce(toB2(Aaxis * 0.00005f) ,toB2(A2->m_Info.m_Position), true);

//             B1->m_Body->ApplyForce(toB2(-Baxis * 0.00005f) ,toB2(B1->m_Info.m_Position), true);
//             B2->m_Body->ApplyForce(toB2(Baxis * 0.00005f) ,toB2(B2->m_Info.m_Position), true);

//             x.Startflag = true;
//             x.Endflag = false;
//         }
//         else if (x.Startflag && !x.Endflag)
//         {
//             glm::vec2 Aaxis = glm::normalize(-A1->m_Info.m_Position + B1->m_Info.m_Position);
//             // float Adist = glm::length(-A1->m_Info.m_Position + B1->m_Info.m_Position);
//             // float APow = 100.0f * (A1->m_Body->GetMass() * B1->m_Body->GetMass())/Adist;
//             glm::vec2 Baxis = glm::normalize(-A2->m_Info.m_Position + B2->m_Info.m_Position);
//             // float Bdist = glm::length(-A2->m_Info.m_Position + B2->m_Info.m_Position);
//             // float BPow = 100.0f * (A2->m_Body->GetMass() * B2->m_Body->GetMass())/Bdist;

//             A1->m_Body->ApplyForce(toB2(Aaxis * 0.000005f) ,toB2(A1->m_Info.m_Position), true);
//             B1->m_Body->ApplyForce(toB2(-Aaxis * 0.000005f) ,toB2(B1->m_Info.m_Position), true);

//             A2->m_Body->ApplyForce(toB2(Baxis * 0.000005f) ,toB2(A2->m_Info.m_Position), true);
//             B2->m_Body->ApplyForce(toB2(-Baxis * 0.000005f) ,toB2(B2->m_Info.m_Position), true);
            
//             if (A1->m_Bonds.size() + B1->m_Bonds.size() == 0){
//                 float MolDist = glm::length(glm::vec2(-A1->m_Info.m_Position + B1->m_Info.m_Position));
//                 if (MolDist < ((A1->m_Info.m_Size.x/2.0f) + (B1->m_Info.m_Size.x/2.0f))*1.25f) // if close enough
//                 {
//                     auto it = std::find(CollisionList.begin(), CollisionList.end(), std::make_pair(A1,B1)); 
//                     if (it != CollisionList.end())
//                     {
//                         Atom *Parent = A1;
//                         Atom *Child = B1;

//                         glm::vec2 Dir = glm::normalize(glm::vec2(-Parent->m_Info.m_Position + Child->m_Info.m_Position));
//                         Child->m_Body->SetTransform(toB2(toGlm(Parent->m_Body->GetPosition()) + (Dir * ((Parent->m_Info.m_Size.x / 2.0f) + (Child->m_Info.m_Size.x / 2.0f)))), Child->m_Body->GetAngle());

//                         b2Vec2 Pivot = toB2(toGlm(Parent->m_Body->GetPosition()) + (Dir * (Parent->m_Info.m_Size.x / 2.0f) ));
//                         b2WeldJointDef jointDef;
//                         jointDef.Initialize(Parent->m_Body, Child->m_Body, Pivot);
//                         jointDef.collideConnected = false;
//                         b2Joint *Joint = world->CreateJoint(&jointDef);
//                         Parent->m_Bonds.push_back(Joint);
//                     }
//                 }
//             }

//             if (A2->m_Bonds.size() + B2->m_Bonds.size() == 0){
//                 float MolDist = glm::length(glm::vec2(-A2->m_Info.m_Position + B2->m_Info.m_Position));
//                 if (MolDist < ((A2->m_Info.m_Size.x/2.0f) + (B2->m_Info.m_Size.x/2.0f))*1.25f) // if close enough
//                 {
//                     auto it = std::find(CollisionList.begin(), CollisionList.end(), std::make_pair(A2,B2)); 
//                     if (it != CollisionList.end())
//                     {
//                         Atom *Parent = B2;
//                         Atom *Child = A2;

//                         glm::vec2 Dir = glm::normalize(glm::vec2(-Parent->m_Info.m_Position + Child->m_Info.m_Position));
//                         Child->m_Body->SetTransform(toB2(toGlm(Parent->m_Body->GetPosition()) + (Dir * ((Parent->m_Info.m_Size.x / 2.0f) + (Child->m_Info.m_Size.x / 2.0f)))), Child->m_Body->GetAngle());

//                         b2Vec2 Pivot = toB2(toGlm(Parent->m_Body->GetPosition()) + (Dir * (Parent->m_Info.m_Size.x / 2.0f) ));
//                         b2WeldJointDef jointDef;
//                         jointDef.Initialize(Parent->m_Body, Child->m_Body, Pivot);
//                         jointDef.collideConnected = false;
//                         b2Joint *Joint = world->CreateJoint(&jointDef);
//                         Parent->m_Bonds.push_back(Joint);
//                     }
//                 }
//             }

//             if (((A2->m_Bonds.size() + B2->m_Bonds.size()) > 0) && ((A1->m_Bonds.size() + B1->m_Bonds.size()) > 0))
//             {
//                 x.Startflag = true;
//                 x.Endflag = true;

//                 EraseElement(A1->m_ChildAtom, A2);
//                 EraseElement(B2->m_ChildAtom, B1);

//                 TbDelete.push_back(x);

//                 A1->m_VisibleBond = false;
//                 B2->m_VisibleBond = false;
//             }
            

//         }
//     }
//     for (int i = 0; i < (int)ReactionStack.size(); i++)
//     {
//         if (!ReactionStack[i].Endflag)
//         {
//             continue;
//         }
//         ReactionStack.erase(ReactionStack.begin() + i);
//         i--;
//     }
    
// }
void RenderLoop();

void update(float dt)
{
    
    // dt = 0.16;
    dt += glm::epsilon<float>();
    auto UpdateStart = std::chrono::high_resolution_clock::now();

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
    WHandler->Update();
    Group1->Update();
    UIGroup->Update();


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
        for (auto x : Programs)
        {
            x->SetUniform(0.0f, "Flat");
        }
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
        for (auto x : Programs)
        {
            x->SetUniform(1.0f, "Flat");
        }
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
}

void UpdateWindows()
{
    float WindowAspect = (float)WindowSize.x/(float)WindowSize.y;
    glm::vec2 CamOffset = {(float)(WindowSize.x%2), (float)(WindowSize.y%2)};
    
    float TotalRatio = WindowAspect / GameAspect;

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
}

void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
    glm::vec2 ScreenMousePos;
    
    ScreenMousePos.x = (2.0f * ((xpos - ((float)(WindowSize.x - ActualGameSize.x)/2.0f))/(float)ActualGameSize.x)) - 1.0f;
    ScreenMousePos.y = (2.0f * (1.0f - (ypos - ((float)(WindowSize.y - ActualGameSize.y)/2.0f))/(float)ActualGameSize.y)) - 1.0f;

    MousePos = UICam.Screen2World(ScreenMousePos);
    WHandler->SetMousePos(MousePos);
    WHandler->Update();
}

void RenderLoop()
{
    static auto LastFrame = std::chrono::high_resolution_clock::now();

    auto start = std::chrono::high_resolution_clock::now();
    update(DeltaTime);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(int(FrameTime*1000.0f*0.75f)) - (std::chrono::high_resolution_clock::now() - start));
    while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start) < std::chrono::milliseconds(int(FrameTime*1000.0f)))
    {
        std::this_thread::yield();
    }
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
    glfwWindowHint(GLFW_SAMPLES, 4);
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

    fx_Image Icon = fx_Image::LoadImage(std::vector<uint8_t>(embed_Chemsim_png, embed_Chemsim_png+embed_Chemsim_png_len));
    GLFWimage images[1];
    images[0].width = Icon.Width;
    images[0].height = Icon.Height;
    images[0].pixels = Icon.Data.data();
    glfwSetWindowIcon(MainWindow, 1, images); 
    // glfwSwapInterval(1);

    // SetWindowsIcon(MainWindow);
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
    glfwSetCursorPosCallback(MainWindow, mouse_pos_callback);

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

    Programs.resize(4);
    fx_Shader BasicVertex = fx_Shader(GLSL_VER + std::string((char*)embed_Basic_vert, embed_Basic_vert_len), "vert");
    fx_Shader BasicFragment = fx_Shader(GLSL_VER + std::string((char*)embed_Basic_frag, embed_Basic_frag_len), "frag");
    Programs[fx_BasicType::Basic] = new fx_Program(std::vector<fx_Shader *>({&BasicVertex, &BasicFragment}));
    fx_Shader SpriteVertex = fx_Shader(GLSL_VER + std::string((char*)embed_Sprite_vert, embed_Sprite_vert_len), "vert");
    fx_Shader SpriteFragment = fx_Shader(GLSL_VER + std::string((char*)embed_Sprite_frag, embed_Sprite_frag_len), "frag");
    Programs[fx_BasicType::Sprite] = new fx_Program(std::vector<fx_Shader *>({&SpriteVertex, &SpriteFragment}));
    fx_Shader CircleVertex = fx_Shader(GLSL_VER + std::string((char*)embed_Circle_vert, embed_Circle_vert_len), "vert");
    fx_Shader CircleFragment = fx_Shader(GLSL_VER + std::string((char*)embed_Circle_frag, embed_Circle_frag_len), "frag");
    Programs[fx_BasicType::Circle] = new fx_Program(std::vector<fx_Shader *>({&CircleVertex, &CircleFragment}));
    fx_Shader TextVertex = fx_Shader(GLSL_VER + std::string((char*)embed_Text_vert, embed_Text_vert_len), "vert");
    fx_Shader TextFragment = fx_Shader(GLSL_VER + std::string((char*)embed_Text_frag, embed_Text_frag_len), "frag");
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

    

    // std::cout << Posdist(generator) << "\n";

    // TODO: manual add, tutorial, licenses, info, clear screen, drawing meaning, stats, molecule drag

    // Arial = new fx_Font("Arial.ttf");
    // Arial = new fx_Font(fx_ReadBinaryFile("Arial.ttf"));
    Arial = new fx_Font(std::vector<uint8_t>(embed_ARIAL_ttf, embed_ARIAL_ttf+embed_ARIAL_ttf_len));
    fx_Image FontImg = Arial->GetAtlas().Image;

    UIGroup->m_TextureUnit = new fx_Texture(FontImg);

    // fx_Sprite *Img = new fx_Sprite({0,1,-1},{1.0f, 1.0f}, {0,0,1,1});
    // UIGroup->AddObject(Img);

    Text = new fx_Text({0,0,-1},{1.0f}, Arial, "Testg.aaa");
    Text->SetAnchor({0.5f,0.5f,0.0f});

    UIGroup->AddObject(Text);



    //  world->SetContactListener(&AtomContactListenerInstance);

    BHandler = new fx_BillboardHandler();

    Circle1 = new fx_BillboardCircle({-1,-1,-1}, {1.0f,1.0f}, {1,1,0,1});
    Group1->AddObject(Circle1);
    BHandler->AddObject(Circle1);

    Circle2 = new fx_BillboardCircle({-1,1,-1}, {1.0f,1.0f}, {1,0,0,1});
    Group1->AddObject(Circle2);
    BHandler->AddObject(Circle2);

    Circle3 = new fx_BillboardCircle({1,-1,-1}, {1.0f,1.0f}, {0,1,0,1});
    Group1->AddObject(Circle3);
    BHandler->AddObject(Circle3);

    Circle4 = new fx_BillboardCircle({1,1,-1}, {1.0f,1.0f}, {0,0,1,1});
    Group1->AddObject(Circle4);
    BHandler->AddObject(Circle4);

    Circle5 = new fx_BillboardCircle({0,0,-1}, {1.0f,1.0f}, {1,1,1,1});
    Group1->AddObject(Circle5);
    BHandler->AddObject(Circle5);

    Circle6 = new fx_BillboardCircle({-1,0,-1}, {1.0f,1.0f}, {1,1,0.5,1});
    Group1->AddObject(Circle6);
    BHandler->AddObject(Circle6);

    Circle7 = new fx_BillboardCircle({0,1,-1}, {1.0f,1.0f}, {1,0,1,1});
    Group1->AddObject(Circle7);
    BHandler->AddObject(Circle7);

    Circle8 = new fx_BillboardCircle({0,-1,-1}, {1.0f,1.0f}, {0,1,1,1});
    Group1->AddObject(Circle8);
    BHandler->AddObject(Circle8);

    Circle9 = new fx_BillboardCircle({1,0,-1}, {1.0f,1.0f}, {0.5,0.5,0.5,1});
    Group1->AddObject(Circle9);
    BHandler->AddObject(Circle9);

    Line1 = new fx_BillboardLine({0,0,0}, {1,1,0}, 0.25);
    Group1->AddObject(Line1);
    BHandler->AddObject(Line1);

    WHandler = new fx_WidgetHandler();

    fx_Button *Button1 = new fx_Button({-1,-1,-1}, {1.0f,1.0f}, 0.5f, Arial, "Test", {1,0,0,1}, {0,1,0,1}, {0,0,1,1}, {0,1,1,1}, {1,1,1,1});
    Button1->SetAnchor({0.0f,0.0f,0.0f});

    UIGroup->AddObject(Button1);
    WHandler->AddObject(Button1);

    BHandler->SetCameraPos(ObjCam.GetPosition());
    BHandler->SetCameraUp(glm::vec3(0,1,0) * ObjCam.GetQuat());

    float Angle = 0;

    Button1->m_MainActionCallback = [&]() {
        Angle += glm::pi<float>() *0.1;

        glm::vec3 CamPos;
        CamPos.x = std::sin(Angle) * 2.5;
        CamPos.y = 0;
        CamPos.z = std::cos(Angle) * 2.5;

        ObjCam.SetPosition(CamPos);
        ObjCam.SetQuat(glm::quat(glm::vec3(0.0f,-Angle,0.0f)));
        
        BHandler->SetCameraPos(CamPos);
        BHandler->SetCameraUp(glm::vec3(0,1,0) * glm::quat(glm::vec3(0.0f,-Angle,0.0f)));

    };
    
    Button1->m_HoldActionCallback = [&]() {
        Angle += glm::pi<float>() * DeltaTime;

        glm::vec3 CamPos;
        CamPos.x = std::sin(Angle) * 2.5;
        CamPos.y = 0;
        CamPos.z = std::cos(Angle) * 2.5;

        ObjCam.SetPosition(CamPos);
        ObjCam.SetQuat(glm::quat(glm::vec3(0.0f,-Angle,0.0f)));
        
        BHandler->SetCameraPos(CamPos);
        BHandler->SetCameraUp(glm::vec3(0,1,0) * glm::quat(glm::vec3(0.0f,-Angle,0.0f)));
    };

    // UIGroup->GenerateMesh();
    UIGroup->Update();

    // Group1->GenerateMesh();
    Group1->Update();

    // GameRender->GenerateMesh();
    GameRender->Update();


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

    glfwTerminate();

    //End Program
    return 0;
}