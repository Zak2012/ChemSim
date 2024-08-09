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

static b2World world(b2Vec2(0.0f,-10.0f));

glm::vec2 VecRotate(glm::vec2 Vec, float Ang)
{
    glm::vec2 Result;
    Result.x = (Vec.x * std::cos(Ang)) - (Vec.y * std::sin(Ang));  
    Result.y = (Vec.x * std::sin(Ang)) + (Vec.y * std::cos(Ang)); 
    return Result;
}

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
    b2Body *m_body;
    Atom(glm::vec3 Pos, glm::vec2 Scale, glm::vec4 Color = {1,1,1,1});
    ~Atom();
    void Update();
};

Atom::Atom(glm::vec3 Pos, glm::vec2 Scale, glm::vec4 Color)
{
    m_Info.m_Size = glm::vec3(Scale, 1.0f);
    m_Info.m_Position = Pos;
    m_Info.m_Color = Color;
    m_Objects = {};
    m_Drawable = true;
    m_Complex = true;

    b2BodyDef BodyDef;
    BodyDef.type = b2_dynamicBody;
    BodyDef.position.Set(Pos.x, Pos.y);

    m_body = world.CreateBody(&BodyDef);

    b2CircleShape circle;
    // circle.m_p.Set(Pos.x, Pos.y);
    circle.m_radius = Scale.x / 2.0f;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;
    fixtureDef.restitution = 0.9f;

    m_body->CreateFixture(&fixtureDef);

    m_Circle = new fx_Circle(Pos, glm::vec2(m_Info.m_Size.x, m_Info.m_Size.y), Color);
    m_Circle->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    m_Circle->Update();
    m_Objects.push_back(m_Circle);
}

Atom::~Atom()
{
    delete m_Circle;
    world.DestroyBody(m_body);
}

void Atom::Update()
{
    b2Vec2 Pos = m_body->GetPosition();
    m_Info.m_Position.x = Pos.x;
    m_Info.m_Position.y = Pos.y;

    // std::cout << Pos.x << ", " << Pos.y << "\n";

    m_Info.m_Rotation = glm::quat({0,0,m_body->GetAngle()});

    m_Circle->m_Info = m_Info;
    m_Circle->Update();
}

class Bond : public fx_Complex
{
private:
public:
    fx_Quad *m_Line;
    Atom *m_A;
    Atom *m_B;

    Bond(Atom *A, Atom *B, float Width = 0.01f, bool Visible = true, glm::vec4 Color = {1,1,1,1});
    ~Bond();

    void Update();
};

Bond::Bond(Atom *A, Atom *B, float Width, bool Visible, glm::vec4 Color)
{
    m_Info.m_Color = Color;
    m_Objects = {};
    m_Drawable = true;
    m_Complex = true;

    m_A = A;
    m_B = B;

    glm::vec2 Dir = glm::vec2(-m_A->m_Info.m_Position + m_B->m_Info.m_Position);


    m_Line = new fx_Quad({m_A->m_Info.m_Position.x, m_A->m_Info.m_Position.y, -2.0f}, glm::vec2(glm::length(Dir), Width), Color);
    m_Line->m_Info.m_Anchor = {0.0f, 0.5f, 0};
    m_Line->m_Info.m_Rotation = glm::quat(glm::vec3(0,0, std::atan2(Dir.y, Dir.x)));
    m_Line->Update();
    m_Objects.push_back(m_Line);

    m_Enabled = Visible;
}

Bond::~Bond()
{
    delete m_Line;
}

void Bond::Update()
{
    glm::vec2 Dir = glm::vec2(-m_A->m_Info.m_Position + m_B->m_Info.m_Position);


    m_Line->m_Info.m_Position = m_A->m_Info.m_Position;
    m_Line->m_Info.m_Rotation = glm::quat(glm::vec3(0,0, std::atan2(Dir.y, Dir.x)));
    m_Line->Update();
}

// class Molecule : public fx_Complex
// {
// public:
//     glm::vec2 m_acc = {0.0f, 0.0f};
//     glm::vec2 m_vel = {0.0f, 0.0f};
//     float m_velr = 0.0f;
//     float m_mass = 0.0f;
//     float m_moi = 0.0f;
//     // glm::vec2 m_pos = {0.0f, 0.0f};
//     glm::vec2 m_com = {0.0f, 0.0f};
//     // std::vector<Atom*> m_Atoms;
//     Molecule(glm::vec3 Pos);
//     ~Molecule(){};

//     void Add(Atom *Matter);

//     void Step(float dt);
//     void Update(){};
//     void CalculateCOM();


// };

// Molecule::Molecule(glm::vec3 Pos)
// {
//     m_Info.m_Position = Pos;
//     m_Objects = {};
//     m_Drawable = true;
//     m_Complex = true;
    
// }

// void Molecule::Add(Atom *Matter)
// {
//     m_Objects.push_back(Matter);
//     Matter->m_Parent = this;

//     CalculateCOM();
//     for (auto &x : m_Objects)
//     {
//         Atom *Item = (Atom*)x;
//         glm::vec2 Offset = Item->m_offset - m_com;
//         glm::vec2 Tmp = Offset;
//         // TmpOffset + glm::vec2(m_Info.m_Position);
//         Item->m_Info.m_Position = glm::vec3(Offset + glm::vec2(m_Info.m_Position), x->m_Info.m_Position.z);
//     }
// }

// void Molecule::CalculateCOM()
// {
//     glm::vec2 Offset;
//     {
//         m_mass = 0.0f;
//         m_moi = 0.0f;
//         glm::vec2 avg = {0.0f,0.0f};
//         for (auto &x : m_Objects)
//         {
//             Atom *Item = (Atom*)x;
//             avg += Item->m_offset * Item->m_mass;
//             m_mass += Item->m_mass;
//             m_moi += Item->m_mass * std::pow(glm::length(Item->m_offset), 2);
//         }
//         Offset = avg/m_mass;
//     }
//     m_com = Offset;
//     // std::cout << m_com.x << ", " << m_com.y << "\n";
// }

// // static float a = 0.0f;
// void Molecule::Step(float dt)
// {
//     CalculateCOM();
//     // {
//     //     float weights = 0.0f;
//     //     glm::vec2 avg = {0.0f,0.0f};
//     //     for (auto &x : m_Atoms)
//     //     {
//     //         avg += glm::vec2(x->m_Info.m_Position) * x->m_mass;
//     //         weights += x->m_mass;
//     //     }
//     //     m_Info.m_Position = glm::vec3(avg/weights, m_Info.m_Position.z);
//     // }


//     float AngStart = 0.0f;
//     {
//         Atom *Item = (Atom*)m_Objects[0];
//         glm::vec2 Vec = glm::vec2(m_Info.m_Position - Item->m_Info.m_Position);
//         AngStart = std::atan2(Vec.y, Vec.x);
//     }

//     // a += 0.05f;
//     // float angle;
//     // {
//     //     glm::vec3 angles = glm::eulerAngles(m_Info.m_Rotation);
//     //     angle = angles.z;
//     // }
//     // std::cout << angle << " 1\n";
//     // angle = a;

//     // for (auto &x : m_Objects)
//     // {
//     //     Atom *Item = (Atom*)x;
//     //     glm::vec2 Offset = Item->m_offset - m_com;
//     //     glm::vec2 Tmp = Offset;
//     //     Offset.x = ((Tmp.x * std::cos(angle)) - (Tmp.y * std::sin(angle)));
//     //     Offset.y = ((Tmp.x * std::sin(angle)) + (Tmp.y * std::cos(angle)));
//     //     // TmpOffset + glm::vec2(m_Info.m_Position);
//     //     Item->m_Info.m_Position = glm::vec3(Offset + glm::vec2(m_Info.m_Position), x->m_Info.m_Position.z);
//     // }

//     // m_velr = 0.0f;

//     // for (auto &x : m_Objects)
//     // {
//     //     Atom *Item = (Atom*)x;
//     //     glm::vec2 momentum = Item->m_mass * Item->m_vel;
//     //     glm::vec2 Vec = m_Info.m_Position - Item->m_Info.m_Position;
//     //     float dist = glm::length(Vec);
//     //     float AngBet = std::atan2(Vec.y, Vec.x);
//     //     // glm::vec2 rmomentum = dist *  momentum;
//     //     // glm::vec2 lmomentum = momentum - rmomentum;

//     //     Item->Step(dt);

//     //     // m_Info.m_Position += glm::vec3((lmomentum / Item->m_mass) * dt, 0.f);






//     //     // float COR = 1.0f;
//     //     // glm::vec2 v1 = (((COR * Item->m_mass * (m_vel - Item->m_vel))  + (Item->m_mass * Item->m_vel) + (m_mass * m_vel))/ (Item->m_mass + m_mass));
//     //     // glm::vec2 v2 = (((COR * Item->m_mass * (Item->m_vel - m_vel))  + (Item->m_mass * Item->m_vel) + (m_mass * m_vel))/ (Item->m_mass + m_mass));
//     //     //     glm::vec2 v1 = (((COR * Body1.weight * (Body2.vel - Body1.vel))  + (Body1.weight * Body1.vel) + (Body2.weight * Body2.vel))/ (Body1.weight + Body2.weight));
//     //     //     glm::vec2 v2 = (((COR * Body1.weight * (Body1.vel - Body2.vel))  + (Body1.weight * Body1.vel) + (Body2.weight * Body2.vel))/ (Body1.weight + Body2.weight));
//     //     // Item->m_vel  = {0.0f,0.0f};  
//     //     // m_vel += lmomentum / m_mass;
//     //     // m_velr += glm::length(rmomentum) / m_mass;
//     //     // m_velr = glm::length(rmomentum) / m_mass;
//     //     // Item->m_vel.x = m_velr * 1;

//     //     // glm::vec2 vel = Item->m_vel;

//     //     // Item->m_vel.x = glm::length(vel) * std::sin(AngBet);  
//     //     // Item->m_vel.y = glm::length(vel) * std::cos(AngBet);
//     //     // Item->m_vel *= dist; 
//     //     // Item->Step(dt) ;
//     //     // Item->m_Info.m_Position += glm::vec3(m_vel * dt, 0.f);
//     // }

//     // for (auto &x : m_Objects)
//     // {
//     //     Atom *Item = (Atom*)x;
//     //     Item->Step(dt);
//     // }


//     {
//         glm::vec2 avg = {0.0f,0.0f};
//         for (auto &x : m_Objects)
//         {
//             Atom *Item = (Atom*)x;
//             // Item->Step(dt);
//             avg += (glm::vec2(Item->m_Info.m_Position) + (Item->m_vel * dt)) * Item->m_mass;
//         }
//         m_Info.m_Position = glm::vec3(avg/m_mass, m_Info.m_Position.z);
//     }

//     float AngRel;
//     float AngEnd = 0.0f;

//     {
//         Atom *Item1 = (Atom*)m_Objects[0];
//         glm::vec2 Vec = -(Item1->m_offset - m_com);
//         AngRel = std::atan2(Vec.y, Vec.x);
//         // float dist = glm::length(Vec);
//         // float AngRel = std::atan2(Vec.y, Vec.x);

//         glm::vec2 Vec1 = glm::vec2(m_Info.m_Position - Item1->m_Info.m_Position) + (Item1->m_vel * dt);
//         AngEnd = std::atan2(Vec1.y, Vec1.x);

//         // Vec = VecRotate(Vec, AngEnd - AngRel);

//         // m_Info.m_Position = glm::vec3(glm::vec2(Item1->m_Info.m_Position) + Vec, m_Info.m_Position.z);
//         // Item1->m_vel = VecRotate(Item1->m_vel, AngEnd - AngStart);

//     }
//     for (int i = 0; i < (int)m_Objects.size(); i++)
//     {
//         Atom *Item = (Atom*)m_Objects[i];
//         glm::vec2 Vec = Item->m_offset - m_com;
//         float dist = glm::length(Vec);
//         float Ang = std::atan2(Vec.y, Vec.x);


//         // Item->Step(dt);
//         glm::vec2 RealOffset = Vec = VecRotate(Vec, AngEnd - AngRel);


//         // glm::vec2 Bef = glm::vec2(Item->m_Info.m_Position);
//         Item->m_Info.m_Position = glm::vec3(glm::vec2(m_Info.m_Position) + RealOffset, Item->m_Info.m_Position.z);
//         // glm::vec2 Aft = glm::vec2(Item->m_Info.m_Position);

//         // float len = glm::length(Aft-Bef);

//         glm::vec2 RotVel = (((AngEnd - AngStart) * dist) / dt) * glm::vec2(-std::sin(AngStart), std::cos(AngStart));
//         float w = (((AngEnd - AngStart) * dist) / dt);
//         glm::vec2 LinVel = Item->m_vel - RotVel;

//         // std::cout << i << ": {" << LinVel.x << ", " << LinVel.y << "} {" << RotVel.x << ", " << RotVel.y << "} ," << w << "\n";


//         // separate orbiting vector with straight vector
//         // orbiting vector is tangent with com
//         // RotVel = VecRotate(RotVel, AngEnd - AngStart);
//         Item->m_vel = VecRotate(Item->m_vel, AngEnd - AngStart);
//         Item->Step(dt);


//         // Item->m_vel = RotVel;
//         // Item->Step(dt);



//     }







    // std::cout << m_vel.x << ", " << m_vel.y  << ", " << m_velr << "\n";
    // m_vel += m_acc * dt;
    // m_Info.m_Position += glm::vec3(m_vel * dt, 0.0f);
    // m_Info.m_Rotation *= glm::quat(glm::vec3({0.0f, 0.0f, m_velr * dt}));

// }

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

static Atom *Atom1;
static Atom *Atom2;

static Bond *Bond1;


// static b2Body* m_body;
static b2Body* m_walln;
static b2Body* m_walls;
static b2Body* m_walle;
static b2Body* m_wallw;

static std::vector<Atom*> ColliderList;

void update(float dt)
{
    
    // dt = 0.16;
    dt += glm::epsilon<float>();
    auto UpdateStart = std::chrono::high_resolution_clock::now();

    world.Step(dt, 6, 2);

    double xpos, ypos;
    glfwGetCursorPos(MainWindow, &xpos, &ypos);


    Atom1->Update();
    Atom2->Update();
    Bond1->Update();

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
    
    LookAtMat = glm::ortho( -(float)(ActualGameSize.x * GameRenderScale)/(float)(ActualGameSize.y * GameRenderScale), (float)(ActualGameSize.x * GameRenderScale)/(float)(ActualGameSize.y * GameRenderScale), -1.0f, 1.0f, 0.1f, 10.0f );
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
    m_walln = world.CreateBody(&wallBodyDef);
    wallBodyDef.position.Set(0, -2);
    m_walls = world.CreateBody(&wallBodyDef);
    wallBodyDef.position.Set(2, 0);
    m_walle = world.CreateBody(&wallBodyDef);
    wallBodyDef.position.Set(-2, 0);
    m_wallw = world.CreateBody(&wallBodyDef);

    m_walln->CreateFixture(&dynamicBox, 0.0f);
    m_walls->CreateFixture(&dynamicBox, 0.0f);
    m_walle->CreateFixture(&dynamicBox, 0.0f);
    m_wallw->CreateFixture(&dynamicBox, 0.0f);


    // b2BodyDef groundBodyDef;
    // // groundBodyDef.position.Set(0.8f, 0.8f);
    // groundBodyDef.type = b2_dynamicBody;

    // m_body = world.CreateBody(&groundBodyDef);

    // b2CircleShape circle;
    // circle.m_p.Set(0.0f, 0.0f);
    // circle.m_radius = Circle2->m_Info.m_Size.x / 2.0f;

    // b2FixtureDef fixtureDef;
    // fixtureDef.shape = &circle;
    // fixtureDef.density = 1.0f;
    // fixtureDef.friction = 1.0f;
    // fixtureDef.restitution = 0.9f;

    // m_body->CreateFixture(&fixtureDef);


    Atom1 = new Atom({0.1f, 0.0f, -2}, glm::vec2(0.2f, 0.2f), {0.0f,0.0f,1.0f,1.0f});
    Atom1->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    // Atom1->m_offset = {0,0};
    // Atom1->m_vel = {-0.05f , 0.1f};
    Atom1->Update();
    Group1->m_Objects.push_back(Atom1);

    Atom2 = new Atom({0.0f, 0.4f, -2}, glm::vec2(0.2f, 0.2f), {0.0f,1.0f,1.0f,1.0f});
    Atom2->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    // Atom2->m_offset = {0.4,0};
    // Atom2->m_vel = {-0.05f , -0.1f};
    // Atom2->m_vel = {0.0f , -0.1f};
    Atom2->Update();
    Group1->m_Objects.push_back(Atom2);

    b2DistanceJointDef jointDef;
    jointDef.Initialize(Atom1->m_body, Atom2->m_body, Atom1->m_body->GetPosition(), Atom2->m_body->GetPosition());
    jointDef.collideConnected = false;

    // Atom1->m_body->ApplyForceToCenter({1.0f,1.0f}, true);

    b2Joint *joint;
    joint = world.CreateJoint(&jointDef);

    Bond1 = new Bond(Atom1, Atom2);
    Bond1->Update();
    Group1->m_Objects.push_back(Bond1);

    // ColliderList.push_back(Atom1);
    // ColliderList.push_back(Atom2);

    // MoleculeNode ParentMolecule;
    // ParentMolecule = new MoleculeNode;
    // ChildMolecule = new MoleculeNode;

    // Test1 = new Molecule({0.0f, 0.0f, -2});

    // Test1->m_Info.m_Rotation = glm::quat({0.0f,0.0f, glm::radians(90.0f)});

    // Test1->Add(Atom1);
    // Test1->Add(Atom2);
    // Group1->m_Objects.push_back(Test1);

    // Test1->CalculateCOM();

    // ParentMolecule->Parent = NULL;
    // ParentMolecule->Matter = Atom1;
    // ParentMolecule->Child.push_back(ParentMolecule);
    // ParentMolecule->RelPos = {0,0};
    // ParentMolecule->Angle = 2.0f;

    // // MoleculeNode ChildMolecule;
    // ChildMolecule->Parent = ParentMolecule;
    // ChildMolecule->Matter = Atom2;
    // ChildMolecule->RelPos = {1,0};


    // ParentMolecule->COM = ((Atom1->m_mass * glm::vec2(Atom1->m_Info.m_Position)) + (Atom2->m_mass * glm::vec2(Atom2->m_Info.m_Position)) / (Atom1->m_mass + Atom2->m_mass));



    // Body1.max_force = 1.0f;
    // Body1.weight = 1.0f;
    // Body1.vel = {1.0f , 1.0f};
    // Body1.acc = {0.0f , 0.0f};
    // Body1.pos = {Circle1->m_Info.m_Position.x, Circle1->m_Info.m_Position.y};

    // Body2 = Body1;
    // Body2.vel = {-0.5f , -0.5f};
    // Body2.weight = 1.0f;
    // Body2.pos = {Circle2->m_Info.m_Position.x, Circle2->m_Info.m_Position.y};

    // Coll1.Pos = Body1.pos;
    // Coll1.Radius = Circle1->m_Info.m_Size.x / 2.0f;

    // Coll2.Pos = Body2.pos;
    // Coll2.Radius = Circle2->m_Info.m_Size.x / 2.0f;

    UIGroup->m_TextureUnit = new fx_Texture(Fonts.Image);
    Fonts.Image.Data.clear();

    Text = new fx_Text({0.0f,0.5f, -2.0f}, {.5f, .5f}, TimesFace, "gA\nV", 0, Fonts, {1.0f,1.0f,1.0f,1.0f});
    Text->m_OutlineColor = {1.0f,0,0,1.0f};
    Text->m_GlowColor = {0.0f,1,0,1.0f};
    Text->m_OutlineThreshold = {0.1f,0.2f};
    Text->m_GlowThreshold = {0.3f,0.5f};
    Text->Update();
    // Group1->m_Objects.push_back(Text);
    UIGroup->m_Objects.push_back(Text);

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

    Button1->m_Info.m_Position = glm::vec4({-1.0f, -1.0f, 0.0f, 1.0f});
    Button1->m_Info.m_Anchor = {0.5f, 0.5f, 0.0f};
    Button1->m_Info.m_Size = {0.5f, 0.5f, 0.0f};
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