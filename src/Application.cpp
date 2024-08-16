#define GLEW_STATIC

// Include standard libraries
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
// #include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>


#include "File.hpp"
#include "Shader.hpp"
#include "Object.hpp"
#include "Resource.hpp"
#include "ColorConvert.hpp"
#include "Font.hpp"
// #include "Time.hpp"
#include "Physics.hpp"
// #include "Game.hpp"
#include "Widget.hpp"

#include <box2d/box2d.h>

#include "../res/Res.h"
// #include "Global.hpp"
// #include "Script.hpp"


/*
Goal:
- Body info struct
- Process raw collision list into item grouped list
- Process collision
*/


#define DEBUG_MODE true

#define gl() \
{\
    ErrorCode = glGetError(); \
    if ( ErrorCode != GL_NO_ERROR ) \
    { \
        std::cout << "GL_ERROR" << " " << \
        "type = 0x"<< std::hex << ErrorCode << std::dec << ", " << \
        __FILE__ << ":" <<  __LINE__ - 1 << "\n";\
    } \
} \

// class fx_InvSprite: public fx_Basic
// {
// protected:
// public:
//     fx_UV m_UV;
//     fx_InvSprite(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Color = {1,1,1,1});
//     ~fx_InvSprite(){}
//     void GenerateMesh();
// };

// fx_InvSprite::fx_InvSprite(glm::vec3 Pos, glm::vec2 Size, fx_UV UV, glm::vec4 Color)
// {
//     m_Info.m_Position = Pos;
//     m_Complex = false;
//     m_Objects = {this};
//     m_Info.m_Size = glm::vec3(Size,1);
//     m_Info.m_Color = Color;
//     m_UV = UV;
//     m_Vertices = {
//         { 0.0f, 0.0f, 0.0f},
//         { 1.0f, 0.0f, 0.0f},
//         { 1.0f, 1.0f, 0.0f},
//         { 0.0f, 1.0f, 0.0f}
//         };
//     m_Type = fx_BasicType::Sprite;
//     Update();
// }

// void fx_InvSprite::GenerateMesh()
// {
//     struct __attribute__((__packed__)) fx_m_Mesh
//     {
//         float x,y,z;
//         float r,g,b,a;
//         float u,v;
//     };

//     std::vector<fx_m_Mesh> Vertices = {
//         {m_ModelVertices[0].x, m_ModelVertices[0].y, m_ModelVertices[0].z, m_Info.m_Color.r, m_Info.m_Color.g, m_Info.m_Color.b, m_Info.m_Color.a, m_UV.X1, m_UV.Y1},
//         {m_ModelVertices[1].x, m_ModelVertices[1].y, m_ModelVertices[1].z, m_Info.m_Color.r, m_Info.m_Color.g, m_Info.m_Color.b, m_Info.m_Color.a, m_UV.X2, m_UV.Y1},
//         {m_ModelVertices[2].x, m_ModelVertices[2].y, m_ModelVertices[2].z, m_Info.m_Color.r, m_Info.m_Color.g, m_Info.m_Color.b, m_Info.m_Color.a, m_UV.X2, m_UV.Y2},
//         {m_ModelVertices[3].x, m_ModelVertices[3].y, m_ModelVertices[3].z, m_Info.m_Color.r, m_Info.m_Color.g, m_Info.m_Color.b, m_Info.m_Color.a, m_UV.X1, m_UV.Y2},
//     };
    
//     m_Mesh = {
//         {},
//         {
//             0,1,2, 0,2,3
//         },
//         {
//             3,4,2
//         },
//         {
//             {GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)}, {GL_FLOAT, sizeof(float)},
//         }
//     };

//     unsigned int VertexSize = 0;

//     for (unsigned int i = 0; i < m_Mesh.VertexComp.size(); i++)
//     {
//         VertexSize += m_Mesh.VertexComp[i] * m_Mesh.VertexType[i].second;
//     }

//     m_Mesh.Vertices = std::vector<unsigned char>((unsigned char *)(Vertices.data()), (unsigned char *)(Vertices.data()) + (VertexSize * Vertices.size()));
// }

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

// void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
// {
//     if (action == GLFW_PRESS)
//     {
//         g_KeyboardState[key] = true;
//     }
//     else if (action == GLFW_RELEASE)
//     {
//         g_KeyboardState[key] = false;
//     }
// }

// void mouse_callback(GLFWwindow* window, double xpos, double ypos)
// {
//     // xpos -= (double)(WindowSize.x - (ActualGameSize.x * GameRenderScale)) / 2.0;
//     g_CursorPos = {(float)xpos, (float) ypos};
//     g_CursorPos -= glm::vec2(g_WindowSize - g_ActualGameSize) / 2.0f;
//     g_CursorPos = 1.0f - (g_CursorPos/glm::vec2(g_ActualGameSize / 2));
//     g_CursorPos.x = -g_CursorPos.x;
//     // CursorPos.x = -(1.0f - ((float)xpos/(float)((ActualGameSize.x * GameRenderScale) / 2)));
//     // CursorPos.y = 1.0f - ((float)ypos/(float)((ActualGameSize.y * GameRenderScale) / 2));
// }

// static bool CheckCollision(fx_Objects *Obj, glm::vec2 Movement, Rect2D Check)
// {
//     Rect2D FutureStep = Rect2D(Obj->m_BoundingBox) + Movement;
//     glm::vec2 FuturePos = glm::vec2(Obj->m_Info.m_Position) + Movement;
//     return fx_Collide(Line2D({glm::vec2(Obj->m_Info.m_Position), FuturePos}), Check)|| // look for tunneling
//            fx_Collide(FutureStep, Check);
// }

// static bool CheckCollision(fx_Objects *Obj, glm::vec2 Movement, std::vector<Rect2D> Check)
// {
//     for (const auto &x: Check)
//     {
//         if (CheckCollision(Obj, Movement, x))
//         {
//             return true;
//         }
//     }
//     return false;
// }

#define EraseElement(x,y)\
{\
    auto it = std::find(x.begin(), x.end(), y); \
    if (it != x.end()) { x.erase(it); } \
}\

inline glm::vec2 toGlm(const b2Vec2 &v) {
    return glm::vec2(v.x, v.y);
}

inline b2Vec2 toB2(const glm::vec2 &v) {
    return b2Vec2(v.x, v.y);
}

static b2World *world = new b2World(b2Vec2(0.0f,0.0f));

glm::vec2 VecRotate(glm::vec2 Vec, float Ang)
{
    glm::vec2 Result;
    Result.x = (Vec.x * std::cos(Ang)) - (Vec.y * std::sin(Ang));  
    Result.y = (Vec.x * std::sin(Ang)) + (Vec.y * std::cos(Ang)); 
    return Result;
}

static float Atom2Screen = 0.1f/50.0f;

enum Elements
{
    H,
    O,
    N,
    C,
    Cl,
    I
};

struct AtomDef
{
    glm::vec3 Color = {1,1,1};
    float Radius;
    float Mass;
    Elements Elmnt;
};

// CPK colouring
// https://sciencenotes.org/molecule-atom-colors-cpk-colors/
const static std::vector<AtomDef> ElementsPreset = {
    {{1.0f,1.0f,1.0f}, 25.0f, 1.008f, Elements::H},
    {{1.0f,0.051f,0.051}, 60.0f, 15.999f, Elements::O},
    {{0.188f, 0.314f, 0.973f}, 65.0f, 14.007f, Elements::N},
    {{0.5625f,0.5625f,0.5625f}, 70.0f, 12.011f, Elements::C},
    {{0.122f,0.941f,0.122f}, 100.0f, 35.45f, Elements::Cl},
    {{0.58,0.0f,0.58f}, 140.0f, 126.90447f, Elements::I},
};

class Atom;
// class Bond;
// class Molecule;

static std::vector<Atom*> AtomsObj;
// static std::vector<Bond*> BondsObj;

class Atom : public fx_Complex
{
private:
public:
    fx_Circle *m_Circle;
    // PhysicsBody2D m_Body;
    // Circle2D m_Coll;
    // float m_mass = 1;
    // void *m_Parent = nullptr;
    // glm::vec2 m_acc = {0.0f , 0.0f};
    // glm::vec2 m_vel = {0.0f , 0.0f};
    // glm::vec2 m_offset = {0.0f, 0.0f};
    Elements m_Element;
    bool m_VisibleBond = false;
    // Molecule *m_Molecule;
    std::vector<b2Joint*> m_Bonds;
    std::vector<Atom*> m_ChildAtom;
    Atom *m_Parent = nullptr;
    std::vector<fx_Quad*> m_Line;
    b2Body *m_Body;
    Atom(glm::vec3 Pos, glm::vec2 Scale, glm::vec4 Color = {1,1,1,1});
    Atom(AtomDef Definition, glm::vec2 Pos = {0.0f, 0.0f});
    ~Atom();
    void Update();
    void TransferParent(Atom *NewParent);
};

Atom::Atom(glm::vec3 Pos, glm::vec2 Scale, glm::vec4 Color)
{
    AtomsObj.push_back(this);
    m_Info.m_Size = glm::vec3(Scale, 1.0f);
    m_Info.m_Position = Pos;
    m_Info.m_Color = Color;
    m_Objects = {};
    m_Drawable = true;
    m_Complex = true;

    b2BodyDef BodyDef;
    BodyDef.type = b2_dynamicBody;
    BodyDef.position.Set(Pos.x, Pos.y);

    m_Body = world->CreateBody(&BodyDef);
    b2BodyUserData &data = m_Body->GetUserData();
    data.pointer = (uintptr_t)this;

    m_Body = world->CreateBody(&BodyDef);
    b2CircleShape circle;
    // circle.m_p.Set(Pos.x, Pos.y);
    circle.m_radius = Scale.x / 2.0f;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;
    fixtureDef.restitution = 0.9f;

    m_Body->CreateFixture(&fixtureDef);

    m_Body->ApplyForce(b2Vec2(0.0f, -1.0f), m_Body->GetPosition(), true);

    m_Circle = new fx_Circle(Pos, glm::vec2(m_Info.m_Size.x, m_Info.m_Size.y), Color);
    m_Circle->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    m_Circle->Update();
    m_Objects.push_back(m_Circle);
}

Atom::Atom(AtomDef Definition, glm::vec2 Pos)
{
    AtomsObj.push_back(this);
    m_Element = Definition.Elmnt;
    float Radius = Definition.Radius * Atom2Screen;
    m_Info.m_Size = glm::vec3(Radius*2, Radius*2, 1.0f);
    m_Info.m_Position = glm::vec3(Pos,-2.0f);
    m_Info.m_Color = glm::vec4(Definition.Color, 1.0f);
    m_Info.m_Anchor = {0.5f, 0.5f, 0};
    m_Objects = {};
    m_Drawable = true;
    m_Complex = true;

    b2BodyDef BodyDef;
    BodyDef.type = b2_dynamicBody;
    BodyDef.position.Set(m_Info.m_Position.x, m_Info.m_Position.y);

    m_Body = world->CreateBody(&BodyDef);
    b2BodyUserData &data = m_Body->GetUserData();
    data.pointer = (uintptr_t)this;

    b2CircleShape circle;
    // circle.m_p.Set(Pos.x, Pos.y);
    circle.m_radius = Radius;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density =  Definition.Mass / ((4.0f/3.0f) * glm::pi<float>() * std::pow(Definition.Radius,3));
    fixtureDef.friction = 1.0f;
    fixtureDef.restitution = 1.0f;

    m_Body->CreateFixture(&fixtureDef);

    m_Body->ApplyForce(b2Vec2(0.0f, -1.0f), m_Body->GetPosition(), true);

    m_Circle = new fx_Circle(m_Info.m_Position, glm::vec2(m_Info.m_Size.x, m_Info.m_Size.y), m_Info.m_Color);
    m_Circle->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    m_Circle->Update();
    m_Objects.push_back(m_Circle);
}

Atom::~Atom()
{
    delete m_Circle;
    world->DestroyBody(m_Body);
    EraseElement(AtomsObj,this);
}

void Atom::Update()
{
    b2Vec2 Pos = m_Body->GetPosition();
    m_Info.m_Position.x = Pos.x;
    m_Info.m_Position.y = Pos.y;

    m_Info.m_Rotation = glm::quat({0,0, m_Body->GetAngle()});

    for (auto &x: m_Line)
    {
        delete x;
    }
    m_Line.clear();
    m_Objects.clear();

    if (m_VisibleBond)
    {
        for (auto &x: m_ChildAtom)
        {
            glm::vec2 Dir = glm::vec2(-m_Info.m_Position + x->m_Info.m_Position);

            fx_Quad *Line = new fx_Quad({m_Info.m_Position.x, m_Info.m_Position.y, -3.0f}, glm::vec2(glm::length(Dir), 0.01), glm::vec4({0.5f, 0.5f, 0.5f, 1.0f}));
            Line->m_Info.m_Anchor = {0.0f, 0.5f, 0};
            Line->m_Info.m_Rotation = glm::quat(glm::vec3(0,0, std::atan2(Dir.y, Dir.x)));
            Line->Update();
            m_Line.push_back(Line);
            m_Objects.push_back(Line);
        }
    }
    for (auto &x: m_ChildAtom)
    {
        m_Objects.push_back(x);
    }

    m_Objects.push_back(m_Circle);
    m_Circle->m_Info = m_Info;
    m_Circle->Update();
}

void Atom::TransferParent(Atom *NewParent)
{
    NewParent->m_Parent = nullptr;
    m_Parent = NewParent;
    for (auto &x : m_ChildAtom)
    {
        if (x == NewParent)
        {
            continue;
        }
        x->m_Parent = NewParent;
        NewParent->m_ChildAtom.push_back(x);
    }
    NewParent->m_ChildAtom.push_back(this);
    m_ChildAtom.clear();
    NewParent->m_Bonds = m_Bonds;
    m_Bonds.clear();
    Update();
    NewParent->Update();
}


void InitMolecule(glm::vec2 Pos, std::vector<std::pair<Elements,float>> Atoms, fx_Group *Render)
{
    float RefAngle = Atoms[0].second;

    Atom *Atom0 = new Atom(ElementsPreset[Atoms[0].first], Pos);
    Render->m_Objects.push_back(Atom0);


    for (int i = 1; i < (int)Atoms.size(); i++)
    {
        float Angle = RefAngle + Atoms[i].second;
        glm::vec2 RelPos = {std::cos(Angle), std::sin(Angle)};
        RelPos = (ElementsPreset[Atoms[0].first].Radius + ElementsPreset[Atoms[i].first].Radius) * Atom2Screen * RelPos;


        Atom *Matter = new Atom(ElementsPreset[Atoms[i].first], Pos + RelPos);
        Matter->m_Parent = Atom0;
        Atom0->m_ChildAtom.push_back(Matter);
        Render->m_Objects.push_back(Matter);

        b2Vec2 Pivot = toB2(glm::vec2(Atom0->m_Info.m_Position) + (glm::normalize(glm::vec2(-Atom0->m_Info.m_Position + Matter->m_Info.m_Position)) * (Atom0->m_Info.m_Size.x / 2.0f) ));

        b2WeldJointDef jointDef;
        jointDef.Initialize(Atom0->m_Body, Matter->m_Body, Pivot);
        jointDef.collideConnected = false;
        b2Joint *Joint = world->CreateJoint(&jointDef);
        Atom0->m_Bonds.push_back(Joint);
    }
}


struct ReactionData
{
    std::vector<Atom *> Atoms;
    bool Startflag = false;
    bool Endflag = false;
};

static std::vector<ReactionData> ReactionStack = {};
static std::vector<std::pair<Atom*,Atom*>> CollisionList = {};

class AtomContactListener : public b2ContactListener
{
    void BeginContact(b2Contact* contact) {
        Atom *A = (Atom*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        Atom *B = (Atom*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
        if (!A || !B) // check for walls
        {
            return;
        }
        CollisionList.push_back({A,B});
        // if (A->m_Bonds.size() == 0) 
        // {
        //     return;
        // }
        // if (B->m_Bonds.size() == 0)
        // {
        //     return;
        // }
        // Atom *A1t = (Atom*)A->m_Bonds[0]->GetBodyA()->GetUserData().pointer;
        // Atom *A2t = (Atom*)A->m_Bonds[0]->GetBodyB()->GetUserData().pointer;
        // Atom *B1t = (Atom*)B->m_Bonds[0]->GetBodyA()->GetUserData().pointer;
        // Atom *B2t = (Atom*)B->m_Bonds[0]->GetBodyB()->GetUserData().pointer;
        Atom *A1 = A;
        Atom *A2;
        Atom *B1 = B;
        Atom *B2;

        if (!A1->m_Parent && A1->m_ChildAtom.size() > 0)
        {
            A2 = A1->m_ChildAtom[0];
        }
        else
        {
            A2 = A1->m_Parent;
        }

        if (!B1->m_Parent && B1->m_ChildAtom.size() > 0)
        {
            B2 = B1->m_ChildAtom[0];
        }
        else
        {
            B2 = B1->m_Parent;
        }
        
        if (A1->m_Element != A2->m_Element || B1->m_Element != B2->m_Element)
        {
            return;
        }

        float BMolDist = glm::length(glm::vec2(-A2->m_Info.m_Position + B2->m_Info.m_Position));

        if (BMolDist > ((A2->m_Info.m_Size.x/2.0f) + (B2->m_Info.m_Size.x/2.0f))*1.25f) // if close enough
        {
            return;
        }
        
        auto it = std::find(CollisionList.begin(), CollisionList.end(), std::make_pair(A2,B2)); 
        if (it == CollisionList.end())
        {
            return;
        }
        
        ReactionData Result;
        Result.Atoms.push_back(A1);
        Result.Atoms.push_back(A2);
        Result.Atoms.push_back(B1);
        Result.Atoms.push_back(B2);

        for (auto &x : ReactionStack)
        {
            if (((Result.Atoms[0] == x.Atoms[0]) || (Result.Atoms[0] == x.Atoms[1])) && ((Result.Atoms[2] == x.Atoms[2]) || (Result.Atoms[2] == x.Atoms[3])))
            {
                return;                    
            }
        }

        ReactionStack.push_back(Result);
        std::cout << "collide\n";
        // A1->m_body->ApplyForce();
        // glm::vec2 Aaxis = (-A1->m_Info.m_Position + A2->m_Info.m_Position);
        // glm::vec2 Baxis = (-B1->m_Info.m_Position + B2->m_Info.m_Position);
        // Aaxis = Aaxis / glm::length(Aaxis);
        // Baxis = Baxis / glm::length(Baxis);
        // A1->m_Bonds[0]->m_Line->m_Enabled = true;
        // world->DestroyJoint(A1->m_Bonds[0]->m_Joint);
        // A1->m_Bonds[0]->m_Joint = nullptr;
        // A2->m_Bonds[0]->m_Joint = nullptr;
        // A1->m_Body->ApplyForce(toB2(-Aaxis * 0.000005f) ,toB2(A1->m_Info.m_Position), true);
        // A2->m_Body->ApplyForce(toB2(Aaxis * 0.000005f) ,toB2(A2->m_Info.m_Position), true);

        // B1->m_Bonds[0]->m_Line->m_Enabled = true;
        // world->DestroyJoint(B1->m_Bonds[0]->m_Joint);
        // B1->m_Bonds[0]->m_Joint = nullptr;
        // B2->m_Bonds[0]->m_Joint = nullptr;
        // B1->m_Body->ApplyForce(toB2(-Baxis * 0.000005f) ,toB2(B1->m_Info.m_Position), true);
        // B2->m_Body->ApplyForce(toB2(Baxis * 0.000005f) ,toB2(B2->m_Info.m_Position), true);

        // timer stack and callback, to delete temp bond, change temp bond to covalent bond, comlete reaction

        // A1->m_Molecule->m_Atoms = {A1, B1};
        // A2->m_Molecule->m_Atoms = {A2, B2};
        // swap the bond


        // world->DestroyJoint(B1->m_Bonds[0]->m_Joint);
        // B1->m_Bonds[0]->m_Joint = nullptr;
        CollisionList.erase(it);
    }
  
    void EndContact(b2Contact* contact) {
        Atom *A = (Atom*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
        Atom *B = (Atom*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;
        if (A && B)
        {
            EraseElement(CollisionList, std::make_pair(A,B));
        }
        // std::cout << CollisionList.size() << "\n";

    }
};

// static std::unordered_map<fx_BasicType, fx_Program*> Programs;
// static fx_Font_Library Lib;


static GLFWwindow *MainWindow;
static float DeltaTime = glm::epsilon<float>();
// static int FPS = 30;
// static int FrameTimems = int((1.0f/float(FPS))*100.0f);

static auto LastResizeTime = std::chrono::high_resolution_clock::now();

static glm::ivec2 WindowSize = {1280,720};
// static glm::ivec2 GameSize = {1280,720};
// static glm::ivec2 UISize = {1280,720};
static float GameAspect = 16.0f/9.0f;
static glm::ivec2 ActualGameSize = {1280,720};
static float GameRenderScale = 1.0f;
static float UIRenderScale = 1.0f;
static float GameScale = 1.0f;

static std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> RenderDemandsStack;

static bool RenderDemand;

static std::unordered_map<fx_BasicType, fx_Program*> Programs;
static fx_Font_Library Lib;

static fx_Face TimesFace;
static fx_Face ArialFace;

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
static glm::mat4 CamMat;
static glm::mat4 RenderLookAtMat;

// static fx_Circle *Circle1;
// static fx_Circle *Circle2;

static fx_Text *Text;

static fx_Button *Button1;

static fx_GUILayer *GUI;

static  AtomContactListener AtomContactListenerInstance;

// static Atom *Atom1;
// static Atom *Atom2;

// static Bond *Bond1;


// static b2Body* m_body;
static b2Body* m_walln;
static b2Body* m_walls;
static b2Body* m_walle;
static b2Body* m_wallw;

void Reaction(float dt)
{
    std::vector<ReactionData> TbDelete = {};
    for (auto &x : ReactionStack)
    {
        Atom *A1 = x.Atoms[0];
        Atom *A2 = x.Atoms[1];
        Atom *B1 = x.Atoms[2];
        Atom *B2 = x.Atoms[3];
        if (!x.Startflag && !x.Endflag)
        {
            x.Startflag = true;

            // check if A1 is parent
            if (A1->m_Parent)
            {
                A2->TransferParent(A1);
            }
            
            for (auto &x : A1->m_Bonds)
            {
                world->DestroyJoint(x);
            }
            A1->m_Bonds.clear();


            // check if B1 is parent
            if (!B1->m_Parent)
            {
                B1->TransferParent(B2);
            }

            for (auto &x : B2->m_Bonds)
            {
                world->DestroyJoint(x);
            }
            B2->m_Bonds.clear();
            A1->m_VisibleBond = true;
            B2->m_VisibleBond = true;

            A1->m_ChildAtom.push_back(B1);
            B2->m_ChildAtom.push_back(A2);
            
            glm::vec2 Aaxis = glm::normalize(-A1->m_Info.m_Position + A2->m_Info.m_Position);
            glm::vec2 Baxis = glm::normalize(-B1->m_Info.m_Position + B2->m_Info.m_Position);
            A1->m_Body->ApplyForce(toB2(-Aaxis * 0.000005f) ,toB2(A1->m_Info.m_Position), true);
            A2->m_Body->ApplyForce(toB2(Aaxis * 0.000005f) ,toB2(A2->m_Info.m_Position), true);

            B1->m_Body->ApplyForce(toB2(-Baxis * 0.000005f) ,toB2(B1->m_Info.m_Position), true);
            B2->m_Body->ApplyForce(toB2(Baxis * 0.000005f) ,toB2(B2->m_Info.m_Position), true);

            x.Startflag = true;
            x.Endflag = false;
        }
        else if (x.Startflag && !x.Endflag)
        {
            glm::vec2 Aaxis = glm::normalize(-A1->m_Info.m_Position + B1->m_Info.m_Position);
            float Adist = glm::length(-A1->m_Info.m_Position + B1->m_Info.m_Position);
            glm::vec2 Baxis = glm::normalize(-A2->m_Info.m_Position + B2->m_Info.m_Position);
            float Bdist = glm::length(-A2->m_Info.m_Position + B2->m_Info.m_Position);

            A1->m_Body->ApplyForce(toB2(Aaxis * Adist * 0.000001f) ,toB2(A1->m_Info.m_Position), true);
            B1->m_Body->ApplyForce(toB2(-Aaxis * Adist * 0.000001f) ,toB2(B1->m_Info.m_Position), true);

            A2->m_Body->ApplyForce(toB2(Baxis * Bdist * 0.000001f) ,toB2(A2->m_Info.m_Position), true);
            B2->m_Body->ApplyForce(toB2(-Baxis * Bdist * 0.000001f) ,toB2(B2->m_Info.m_Position), true);
            
            if (A1->m_Bonds.size() + B1->m_Bonds.size() == 0){
                float MolDist = glm::length(glm::vec2(-A1->m_Info.m_Position + B1->m_Info.m_Position));
                if (MolDist > ((A1->m_Info.m_Size.x/2.0f) + (B1->m_Info.m_Size.x/2.0f))*1.25f) // if close enough
                {
                    continue;
                }
                auto it = std::find(CollisionList.begin(), CollisionList.end(), std::make_pair(A1,B1)); 
                if (it == CollisionList.end())
                {
                    continue;
                }
                Atom *Parent = A1;
                Atom *Child = B1;

                // if (A1->m_Parent)
                // {
                //     Parent = B1;
                //     Child = A1;
                // }
                std::cout << "1\n";
                
                glm::vec2 Dir = glm::normalize(glm::vec2(-Parent->m_Info.m_Position + Child->m_Info.m_Position));
                Child->m_Body->SetTransform(toB2(toGlm(Parent->m_Body->GetPosition()) + (Dir * ((Parent->m_Info.m_Size.x / 2.0f) + (Child->m_Info.m_Size.x / 2.0f)))), Child->m_Body->GetAngle());

                b2Vec2 Pivot = toB2(toGlm(Parent->m_Body->GetPosition()) + (Dir * (Parent->m_Info.m_Size.x / 2.0f) ));
                b2WeldJointDef jointDef;
                jointDef.Initialize(Parent->m_Body, Child->m_Body, Pivot);
                jointDef.collideConnected = false;
                b2Joint *Joint = world->CreateJoint(&jointDef);
                Parent->m_Bonds.push_back(Joint);
            }

            if (A2->m_Bonds.size() + B2->m_Bonds.size() == 0){
                float MolDist = glm::length(glm::vec2(-A2->m_Info.m_Position + B2->m_Info.m_Position));
                if (MolDist > ((A2->m_Info.m_Size.x/2.0f) + (B2->m_Info.m_Size.x/2.0f))*1.25f) // if close enough
                {
                    continue;
                }
                auto it = std::find(CollisionList.begin(), CollisionList.end(), std::make_pair(A2,B2)); 
                if (it == CollisionList.end())
                {
                    continue;
                }
                Atom *Parent = B2;
                Atom *Child = A2;

                // if (A2->m_Parent)
                // {
                //     Parent = B2;
                //     Child = A2;
                // }
                std::cout << "2\n";

                glm::vec2 Dir = glm::normalize(glm::vec2(-Parent->m_Info.m_Position + Child->m_Info.m_Position));
                Child->m_Body->SetTransform(toB2(toGlm(Parent->m_Body->GetPosition()) + (Dir * ((Parent->m_Info.m_Size.x / 2.0f) + (Child->m_Info.m_Size.x / 2.0f)))), Child->m_Body->GetAngle());

                b2Vec2 Pivot = toB2(toGlm(Parent->m_Body->GetPosition()) + (Dir * (Parent->m_Info.m_Size.x / 2.0f) ));
                b2WeldJointDef jointDef;
                jointDef.Initialize(Parent->m_Body, Child->m_Body, Pivot);
                jointDef.collideConnected = false;
                b2Joint *Joint = world->CreateJoint(&jointDef);
                Parent->m_Bonds.push_back(Joint);
            }
            
            x.Startflag = true;
            x.Endflag = true;

            EraseElement(A1->m_ChildAtom, A2);
            EraseElement(B2->m_ChildAtom, B1);

            TbDelete.push_back(x);

            A1->m_VisibleBond = false;
            B2->m_VisibleBond = false;

        }
    }
    for (int i = 0; i < (int)ReactionStack.size(); i++)
    {
        if (!ReactionStack[i].Endflag)
        {
            continue;
        }
        ReactionStack.erase(ReactionStack.begin() + i);
        i--;
    }
    
}


void update(float dt)
{
    
    // dt = 0.16;
    dt += glm::epsilon<float>();
    auto UpdateStart = std::chrono::high_resolution_clock::now();

    // IMPORTANT: run in another thread to ensure continous
    // AtomsObj[0]->m_body->SetLinearVelocity(b2Vec2(0.0f, -10.0f));

    world->Step(dt, 6, 2);

    // Collision respond
    Reaction(dt);


    double xpos, ypos;
    glfwGetCursorPos(MainWindow, &xpos, &ypos);

    for (auto &x: AtomsObj)
    {
        x->Update();
    }



    // Atom1->Update();
    // Atom2->Update();
    // Bond1->Update();

    Group1->Update();


    RenderDemand = true;
    if ((RenderDemandsStack.size() > 0 || RenderDemand)
     && WindowSize.x * WindowSize.y != 0) // draws
    {
        // std::cout << DeltaTime*1000.0f << "ms \n";
        // GUI->Update(WindowSize.x, WindowSize.y, GameAspect);
        RenderDemand = false;
        for (auto x : Programs)
        {
            x.second->SetUniform(LookAtMat, "Matrix");
        }
        // glViewport(0, 0, WindowSize.x, WindowSize.y);
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
            x.second->SetUniform(RenderLookAtMat, "Matrix");
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{   
    WindowSize = {width,height};
    GUI->m_GameAspect = GameAspect;


    float WindowAspect = (float)WindowSize.x/(float)WindowSize.y;
    float CamOffset = (float)(WindowSize.x%2);

    glm::vec2 GameOffset = {0.0f, 0.0f};

    GameOffset.x = ((std::max(WindowAspect - GameAspect, 0.0f) / 2.0f) * WindowSize.y);
    GameOffset.y = ((std::max((1.0f/WindowAspect) - (1.0f/GameAspect), 0.0f) / 2.0f) * WindowSize.x);

    UIRenderer->m_Info.m_Position = {0.0f, 0.0f, UIRenderer->m_Info.m_Position.z};
    Renderer->m_Info.m_Position = {0.0f, 0.0f, Renderer->m_Info.m_Position.z};
    

    if (WindowAspect >= GameAspect)
    {
        ActualGameSize.x = WindowSize.y * GameAspect;
        ActualGameSize.y = WindowSize.y;
        GameOffset.x += CamOffset;

        RenderLookAtMat = glm::ortho( -WindowAspect, WindowAspect, -1.0f, 1.0f, 0.1f, 10.0f );
        UIRenderer->m_Info.m_Size = {GameAspect*2.0f, -2.0f, 1.0f};
        UIRenderer->m_Info.m_Position.x = CamOffset / (ActualGameSize.x * UIRenderScale);
        Renderer->m_Info.m_Size = {GameAspect*2.0f, -2.0f, 1.0f};
        Renderer->m_Info.m_Position.x = CamOffset / (ActualGameSize.x * GameRenderScale);
        Background->m_Info.m_Size = {WindowAspect*2.0f, -2.0f, 1.0f};
    }
    else
    {
        ActualGameSize.y = WindowSize.x / GameAspect;
        ActualGameSize.x = WindowSize.x;
        CamOffset = (WindowSize.y%2);
        GameOffset.y += CamOffset;

        RenderLookAtMat = glm::ortho( -1.0f, 1.0f, -1/WindowAspect, 1/WindowAspect , 0.1f, 10.0f );
        UIRenderer->m_Info.m_Size = {2.0f, 1/GameAspect*-2.0f, 1.0f};
        UIRenderer->m_Info.m_Position.y = CamOffset / (ActualGameSize.y * UIRenderScale);
        Renderer->m_Info.m_Size = {2.0f, 1/GameAspect*-2.0f, 1.0f};
        Renderer->m_Info.m_Position.y = CamOffset / (ActualGameSize.y * GameRenderScale);
        Background->m_Info.m_Size = {2.0f, 1/WindowAspect*-2.0f, 1.0f};
    }

    GUI->m_GameOffset = GameOffset;
    GUI->m_GameSize = ActualGameSize;
    // GUI->Update();

    GameBuffer->SetSize(glm::vec2(ActualGameSize) * GameRenderScale);
    UIBuffer->SetSize(glm::vec2(ActualGameSize) * UIRenderScale);
    
    
    Background->Update();
    Renderer->Update();
    UIRenderer->Update();
    // GameRender->GenerateMesh();
    GameRender->Update();
    // UIRender->GenerateMesh();
    UIRender->Update();


    auto TimeNow = std::chrono::high_resolution_clock::now();

    if ((TimeNow - LastResizeTime) > std::chrono::milliseconds(10))
    {
        RenderDemand = true;
        update(DeltaTime);
        LastResizeTime = std::chrono::high_resolution_clock::now();
        int DeltaTime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(TimeNow - LastResizeTime).count();
        DeltaTime = DeltaTime_ms/1000.0f;
    }
}

void move_callback(GLFWwindow* window, int xpos, int ypos)
{
    auto TimeNow = std::chrono::high_resolution_clock::now();

    if ((TimeNow - LastResizeTime) > std::chrono::milliseconds(10))
    {
        RenderDemand = true;
        update(DeltaTime);
        LastResizeTime = std::chrono::high_resolution_clock::now();
        int DeltaTime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(TimeNow - LastResizeTime).count();
        DeltaTime = DeltaTime_ms/1000.0f;
    }
}

// Entry Point
int main (int argc, char *argv[])
{
    // StartAccurateSleep();
    std::cout << "Start\n";

    /* Initialize the library */
    if ( !glfwInit() )
    {
        std::cout << "Unable to load glfw\n";
        return -1;
    }

    // glfwSwapInterval(true);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    // glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW , GLFW_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    // glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    /* Create a windowed mode window and its OpenGL context */

    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetKeyCallback(window, key_callback);

    // glfwSetWindowAspectRatio(window, 16.0f, 9.0f);

    /* Make the window's context current */
    // glfwMakeContextCurrent(window);

    MainWindow = glfwCreateWindow(WindowSize.x, WindowSize.y, "Hello World", NULL, NULL);
    if ( !MainWindow )
    {
        std::cout << "unable to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(MainWindow);
    glfwSwapInterval(1);

    if ( glewInit() != GLEW_OK)
    {
        std::cout << "Glew Failed to initialize\n";
    }

    std::cout << "Created Window\n";

    glfwSetWindowSizeCallback(MainWindow, framebuffer_size_callback);
    glfwSetWindowPosCallback(MainWindow, move_callback);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(MessageCallback, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, NULL, GL_TRUE);

    std::cout << glGetString(GL_VERSION) << "\n";
    gl()

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    // glEnable(GL_MULTISAMPLE);  

    glActiveTexture(GL_TEXTURE0);

    unsigned int DefaultVao;
    glGenVertexArrays(1, &DefaultVao);

    fx_Shader BasicVertex = fx_Shader(fx_ReadFile("data/Shaders/Basic.vert"), "vert");
    fx_Shader BasicFragment = fx_Shader(fx_ReadFile("data/Shaders/Basic.frag"), "frag");
    Programs[fx_BasicType::Basic] = new fx_Program(std::vector<fx_Shader *>({&BasicVertex, &BasicFragment}));
    fx_Shader SpriteVertex = fx_Shader(fx_ReadFile("data/Shaders/Sprite.vert"), "vert");
    fx_Shader SpriteFragment = fx_Shader(fx_ReadFile("data/Shaders/Sprite.frag"), "frag");
    Programs[fx_BasicType::Sprite] = new fx_Program(std::vector<fx_Shader *>({&SpriteVertex, &SpriteFragment}));
    fx_Shader CircleVertex = fx_Shader(fx_ReadFile("data/Shaders/Circle.vert"), "vert");
    fx_Shader CircleFragment = fx_Shader(fx_ReadFile("data/Shaders/Circle.frag"), "frag");
    Programs[fx_BasicType::Circle] = new fx_Program(std::vector<fx_Shader *>({&CircleVertex, &CircleFragment}));
    fx_Shader TextVertex = fx_Shader(fx_ReadFile("data/Shaders/Text.vert"), "vert");
    fx_Shader TextFragment = fx_Shader(fx_ReadFile("data/Shaders/Text.frag"), "frag");
    Programs[fx_BasicType::SDF] = new fx_Program(std::vector<fx_Shader *>({&TextVertex, &TextFragment}));
    Lib = fx_Load_Lib();

    TimesFace = fx_Load_Face(Lib, "data/Times.ttf");

    fx_Atlas Fonts;
    fx_Load_Font(&Fonts, TimesFace, "Data/cache/Font0.png", 0, false);
    Fonts.Image.Data = ColorConvert::GetColorConvertFunc(ColorConvert::ColorClass::Gray, ColorConvert::ColorClass::RGBA)(Fonts.Image.Data);
    Fonts.Image.Component = 4;
    
    LookAtMat = glm::ortho( (-(float)(ActualGameSize.x )/(float)(ActualGameSize.y)) * GameScale, ((float)(ActualGameSize.x)/(float)(ActualGameSize.y)) * GameScale, -1.0f * GameScale, 1.0f * GameScale, 0.1f, 10.0f );
    // glm::mat4 InvLookAtMat = glm::inverse(LookAtMat);
    RenderLookAtMat = glm::identity<glm::mat4>();

    Group1 = new fx_Group(Programs, fx_InitBufferMap(), NULL);
    // Group2 = new fx_Group(Programs, fx_InitBufferMap(), NULL);
    UIGroup = new fx_Group(Programs, fx_InitBufferMap(), NULL);

    UIRender = new fx_Group(Programs, fx_InitBufferMap(), NULL);
    GameRender = new fx_Group(Programs, fx_InitBufferMap(), NULL);

    GameBuffer = new fx_Framebuffer();
    UIBuffer = new fx_Framebuffer();

    GameBuffer->SetSize(glm::vec2(ActualGameSize) * GameRenderScale);
    UIBuffer->SetSize(glm::vec2(ActualGameSize) * UIRenderScale);

    GameRender->m_TextureUnit = GameBuffer->GetColorAttachment();
    UIRender->m_TextureUnit = UIBuffer->GetColorAttachment();

    Renderer = new fx_Sprite({0.0f,0.0f,-2.0f}, {2.0f, -2.0f}, {0.0f,0.0f,1.0f,1.0f});

    Renderer->m_Info.m_Anchor = {0.5f,0.5f,0.0f};
    GameRender->m_Objects.push_back(Renderer);
    Renderer->Update();

    UIRenderer = new fx_Sprite({0.0f,0.0f,-2.0f}, {2.0f, -2.0f}, {0.0f,0.0f,1.0f,1.0f});

    UIRenderer->m_Info.m_Anchor = {0.5f,0.5f,0.0f};
    UIRender->m_Objects.push_back(UIRenderer);
    UIRenderer->Update();

    Background = new fx_Quad({0, 0.0f, -0.5f}, {1.0f, 1.0f});
    Background->m_Info.m_Anchor = {0.5f,0.5f,0.0f};
    GameRender->m_Objects.push_back(Background);
    Background->Update();

    // Circle1 = new fx_Circle({-0.5f, 0.1f, -2}, glm::vec2(0.2f, 0.2f), {1.0f,0.0f,0.0f,1.0f});
    // Circle1->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    // Circle1->Update();
    // Group1->m_Objects.push_back(Circle1);

    // Circle2 = new fx_Circle({0.8f, 0.8f, -2}, glm::vec2(0.2f, 0.2f), {0.0f,1.0f,0.0f,1.0f});
    // Circle2->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    // Circle2->Update();
    // Group1->m_Objects.push_back(Circle2);
    
    b2BodyDef wallBodyDef;
    // wallBodyDef.type = b2_staticBody;

    b2PolygonShape dynamicBox;
    dynamicBox.SetAsBox(1.0f, 1.0f);

    wallBodyDef.position.Set(0, 2);
    m_walln = world->CreateBody(&wallBodyDef);
    wallBodyDef.position.Set(0, -2);
    m_walls = world->CreateBody(&wallBodyDef);
    wallBodyDef.position.Set(2, 0);
    m_walle = world->CreateBody(&wallBodyDef);
    wallBodyDef.position.Set(-2, 0);
    m_wallw = world->CreateBody(&wallBodyDef);

    m_walln->CreateFixture(&dynamicBox, 0.0f);
    m_walls->CreateFixture(&dynamicBox, 0.0f);
    m_walle->CreateFixture(&dynamicBox, 0.0f);
    m_wallw->CreateFixture(&dynamicBox, 0.0f);

    InitMolecule({0.02f,0.5f}, {std::make_pair(Elements::H, 0.0f), std::make_pair(Elements::H, glm::pi<float>())}, Group1);
    InitMolecule({0.1f,-0.5f}, {std::make_pair(Elements::O, 0.0f), std::make_pair(Elements::O, glm::pi<float>())}, Group1);


     world->SetContactListener(&AtomContactListenerInstance);

    // UIGroup->m_TextureUnit = new fx_Texture(Fonts.Image);
    // Fonts.Image.Data.clear();

    // Text = new fx_Text({0.0f,0.5f, -2.0f}, {.5f, .5f}, TimesFace, "gA\nV", 0, Fonts, {1.0f,1.0f,1.0f,1.0f});
    // Text->m_OutlineColor = {1.0f,0,0,1.0f};
    // Text->m_GlowColor = {0.0f,1,0,1.0f};
    // Text->m_OutlineThreshold = {0.1f,0.2f};
    // Text->m_GlowThreshold = {0.3f,0.5f};
    // Text->Update();
    // Group1->m_Objects.push_back(Text);
    // UIGroup->m_Objects.push_back(Text);

    // UIGroup->GenerateMesh();
    UIGroup->Update();

    // Group1->GenerateMesh();
    Group1->Update();

    // GameRender->GenerateMesh();
    GameRender->Update();

    GUI = new fx_GUILayer(MainWindow);
    // std::cout << "{" << ActualGameSize.x << "," << ActualGameSize.y << "}\n" ;
    // GUI->m_GameAspectRatio = GameAspect;
    // GUI->Update(WindowSize.x, WindowSize.y, GameAspect);


    Button1 = new fx_Button(GUI);

    Button1->m_ClickCallback = []() {
        std::cout << "button1 clicked\n";
        // Circle1->m_Enabled = !Circle1->m_Enabled;
        // Circle2->m_Enabled = !Circle2->m_Enabled;
        // Circle2->m_Info.m_Color.g = Circle1->m_Enabled;
        // Text->m_Enabled = !Text->m_Enabled;
        // Circle1->Update();
        // Circle2->Update();
        // Circle1->GenerateMesh();
        // Text->Update();
        // Circle1->GenerateMesh();
        // UIGroup->Update();
        Group1->Update();
        // Group1->Update();
        RenderDemand = true;
        };

    // Button1->m_Info.m_Position = glm::vec4({-1.0f, -1.0f, 0.0f, 1.0f});
    // Button1->m_Info.m_Anchor = {0.5f, 0.5f, 0.0f};
    // Button1->m_Info.m_Size = {0.5f, 0.5f, 0.0f};
    // GUI->Update();


    // for (auto x : Programs)
    // {
    //     x.second->SetUniform(LookAtMat, "Matrix");
    // }

    framebuffer_size_callback(MainWindow, WindowSize.x, WindowSize.y);
    auto LastFrame = std::chrono::high_resolution_clock::now();
    RenderDemand = true;

    while ( !glfwWindowShouldClose(MainWindow) )
    {
        auto start = std::chrono::high_resolution_clock::now();
        // UIRenderer->Update();
        update(DeltaTime);
        
        /* Poll for and process events */
        glfwPollEvents();
        // glfwWaitEventsTimeout(float(FrameTimems)/1000.0f);

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // std::this_thread::sleep_for(std::chrono::milliseconds(int(FrameTimems*0.75f)) - (std::chrono::high_resolution_clock::now() - start));
        // while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start) < std::chrono::milliseconds(FrameTimems))
        // {
        //     std::this_thread::yield();
        // }
        int DeltaTime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(start - LastFrame).count();
        DeltaTime = DeltaTime_ms/1000.0f;
        LastFrame = start;
    }

    glfwTerminate();

    //End Program
    return 0;
}