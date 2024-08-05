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
#include "Game.hpp"
#include "Widget.hpp"

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
    float m_mass = 1;
    void *m_Parent = nullptr;
    glm::vec2 m_acc = {0.0f , 0.0f};
    glm::vec2 m_vel = {0.0f , 0.0f};
    glm::vec2 m_offset = {0.0f, 0.0f};
    Atom(glm::vec3 Pos, glm::vec2 Scale, glm::vec4 Color = {1,1,1,1});
    ~Atom();
    void Step(float dt);
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

    m_Circle = new fx_Circle(Pos, glm::vec2(m_Info.m_Size.x, m_Info.m_Size.y), Color);
    m_Circle->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    m_Circle->Update();
    m_Objects.push_back(m_Circle);
}

Atom::~Atom()
{
    delete m_Circle;
}

void Atom::Step(float dt)
{
    m_vel += (m_acc * dt);
    m_Info.m_Position += glm::vec3(m_vel * dt, 0.f);
}

void Atom::Update()
{
    m_Circle->m_Info = m_Info;

    m_Circle->Update();
}

// struct MoleculeNode // not a tree, group of atom / phisics body parent
// {
//     std::vector<MoleculeNode*> Child;
//     Atom *Matter;
//     MoleculeNode *Parent;
//     glm::vec2 COM;
//     float Angle = 0.0f;
//     // PhysicsBody2D Body;
//     glm::ivec2 RelPos;
// };

class Molecule : public fx_Complex
{
public:
    glm::vec2 m_acc = {0.0f, 0.0f};
    glm::vec2 m_vel = {0.0f, 0.0f};
    float m_velr = 0.0f;
    float m_mass = 0.0f;
    float m_moi = 0.0f;
    // glm::vec2 m_pos = {0.0f, 0.0f};
    glm::vec2 m_com = {0.0f, 0.0f};
    // std::vector<Atom*> m_Atoms;
    Molecule(glm::vec3 Pos);
    ~Molecule(){};

    void Add(Atom *Matter);

    void Step(float dt);
    void Update(){};
    void CalculateCOM();


};

Molecule::Molecule(glm::vec3 Pos)
{
    m_Info.m_Position = Pos;
    m_Objects = {};
    m_Drawable = true;
    m_Complex = true;
    
}

void Molecule::Add(Atom *Matter)
{
    m_Objects.push_back(Matter);
    Matter->m_Parent = this;

    CalculateCOM();
    for (auto &x : m_Objects)
    {
        Atom *Item = (Atom*)x;
        glm::vec2 Offset = Item->m_offset - m_com;
        glm::vec2 Tmp = Offset;
        // TmpOffset + glm::vec2(m_Info.m_Position);
        Item->m_Info.m_Position = glm::vec3(Offset + glm::vec2(m_Info.m_Position), x->m_Info.m_Position.z);
    }
}

void Molecule::CalculateCOM()
{
    glm::vec2 Offset;
    {
        m_mass = 0.0f;
        m_moi = 0.0f;
        glm::vec2 avg = {0.0f,0.0f};
        for (auto &x : m_Objects)
        {
            Atom *Item = (Atom*)x;
            avg += Item->m_offset * Item->m_mass;
            m_mass += Item->m_mass;
            m_moi += Item->m_mass * std::pow(glm::length(Item->m_offset), 2);
        }
        Offset = avg/m_mass;
    }
    m_com = Offset;
    // std::cout << m_com.x << ", " << m_com.y << "\n";
}

// static float a = 0.0f;
void Molecule::Step(float dt)
{
    CalculateCOM();
    // {
    //     float weights = 0.0f;
    //     glm::vec2 avg = {0.0f,0.0f};
    //     for (auto &x : m_Atoms)
    //     {
    //         avg += glm::vec2(x->m_Info.m_Position) * x->m_mass;
    //         weights += x->m_mass;
    //     }
    //     m_Info.m_Position = glm::vec3(avg/weights, m_Info.m_Position.z);
    // }


    float AngStart = 0.0f;
    {
        Atom *Item = (Atom*)m_Objects[0];
        glm::vec2 Vec = glm::vec2(m_Info.m_Position - Item->m_Info.m_Position);
        AngStart = std::atan2(Vec.y, Vec.x);
    }

    // a += 0.05f;
    // float angle;
    // {
    //     glm::vec3 angles = glm::eulerAngles(m_Info.m_Rotation);
    //     angle = angles.z;
    // }
    // std::cout << angle << " 1\n";
    // angle = a;

    // for (auto &x : m_Objects)
    // {
    //     Atom *Item = (Atom*)x;
    //     glm::vec2 Offset = Item->m_offset - m_com;
    //     glm::vec2 Tmp = Offset;
    //     Offset.x = ((Tmp.x * std::cos(angle)) - (Tmp.y * std::sin(angle)));
    //     Offset.y = ((Tmp.x * std::sin(angle)) + (Tmp.y * std::cos(angle)));
    //     // TmpOffset + glm::vec2(m_Info.m_Position);
    //     Item->m_Info.m_Position = glm::vec3(Offset + glm::vec2(m_Info.m_Position), x->m_Info.m_Position.z);
    // }

    // m_velr = 0.0f;

    // for (auto &x : m_Objects)
    // {
    //     Atom *Item = (Atom*)x;
    //     glm::vec2 momentum = Item->m_mass * Item->m_vel;
    //     glm::vec2 Vec = m_Info.m_Position - Item->m_Info.m_Position;
    //     float dist = glm::length(Vec);
    //     float AngBet = std::atan2(Vec.y, Vec.x);
    //     // glm::vec2 rmomentum = dist *  momentum;
    //     // glm::vec2 lmomentum = momentum - rmomentum;

    //     Item->Step(dt);

    //     // m_Info.m_Position += glm::vec3((lmomentum / Item->m_mass) * dt, 0.f);






    //     // float COR = 1.0f;
    //     // glm::vec2 v1 = (((COR * Item->m_mass * (m_vel - Item->m_vel))  + (Item->m_mass * Item->m_vel) + (m_mass * m_vel))/ (Item->m_mass + m_mass));
    //     // glm::vec2 v2 = (((COR * Item->m_mass * (Item->m_vel - m_vel))  + (Item->m_mass * Item->m_vel) + (m_mass * m_vel))/ (Item->m_mass + m_mass));
    //     //     glm::vec2 v1 = (((COR * Body1.weight * (Body2.vel - Body1.vel))  + (Body1.weight * Body1.vel) + (Body2.weight * Body2.vel))/ (Body1.weight + Body2.weight));
    //     //     glm::vec2 v2 = (((COR * Body1.weight * (Body1.vel - Body2.vel))  + (Body1.weight * Body1.vel) + (Body2.weight * Body2.vel))/ (Body1.weight + Body2.weight));
    //     // Item->m_vel  = {0.0f,0.0f};  
    //     // m_vel += lmomentum / m_mass;
    //     // m_velr += glm::length(rmomentum) / m_mass;
    //     // m_velr = glm::length(rmomentum) / m_mass;
    //     // Item->m_vel.x = m_velr * 1;

    //     // glm::vec2 vel = Item->m_vel;

    //     // Item->m_vel.x = glm::length(vel) * std::sin(AngBet);  
    //     // Item->m_vel.y = glm::length(vel) * std::cos(AngBet);
    //     // Item->m_vel *= dist; 
    //     // Item->Step(dt) ;
    //     // Item->m_Info.m_Position += glm::vec3(m_vel * dt, 0.f);
    // }

    for (auto &x : m_Objects)
    {
        Atom *Item = (Atom*)x;
        Item->Step(dt);
    }

    glm::vec2 NetMoment = {0.0f, 0.0f};
    for (auto &x : m_Objects)
    {
        Atom *Item = (Atom*)x;
        NetMoment += Item->m_vel * Item->m_mass;
    }


    glm::vec2 global_com;
    {
        glm::vec2 avg = {0.0f,0.0f};
        for (auto &x : m_Objects)
        {
            Atom *Item = (Atom*)x;
            avg += glm::vec2(Item->m_Info.m_Position) * Item->m_mass;
        }
        global_com = avg/m_mass;
    }
    m_Info.m_Position = glm::vec3(global_com, m_Info.m_Position.z);

    {
        Atom *Item1 = (Atom*)m_Objects[0];
        glm::vec2 Vec = -(Item1->m_offset - m_com);
        float AngRel = std::atan2(Vec.y, Vec.x);
        // float dist = glm::length(Vec);
        // float AngRel = std::atan2(Vec.y, Vec.x);

        float AngEnd = 0.0f;
        {
            Atom *Item = (Atom*)m_Objects[0];
            glm::vec2 Vec1 = global_com - glm::vec2(Item->m_Info.m_Position);
            AngEnd = std::atan2(Vec1.y, Vec1.x);
        }

        // Vec = VecRotate(Vec, AngEnd - AngRel);

        // m_Info.m_Position = glm::vec3(glm::vec2(Item1->m_Info.m_Position) + Vec, m_Info.m_Position.z);
        // Item1->m_vel = VecRotate(Item1->m_vel, AngEnd - AngStart);

        for (int i = 0; i < (int)m_Objects.size(); i++)
        {
            Atom *Item = (Atom*)m_Objects[i];
            glm::vec2 Vec = Item->m_offset - m_com;
            float dist = glm::length(Vec);
            float Ang = std::atan2(Vec.y, Vec.x);

            glm::vec2 RealOffset = Vec = VecRotate(Vec, AngEnd - AngRel);

            Item->m_Info.m_Position = glm::vec3(glm::vec2(m_Info.m_Position) + RealOffset, Item->m_Info.m_Position.z);

            // separate orbiting vector with straight vector
            // orbiting vector is tangent with com
            Item->m_vel = VecRotate(Item->m_vel, AngEnd - AngStart);


        }

    }






    // std::cout << m_vel.x << ", " << m_vel.y  << ", " << m_velr << "\n";
    // m_vel += m_acc * dt;
    // m_Info.m_Position += glm::vec3(m_vel * dt, 0.0f);
    // m_Info.m_Rotation *= glm::quat(glm::vec3({0.0f, 0.0f, m_velr * dt}));

}

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

static fx_Circle *Circle1;
// static fx_Circle *Circle2;
static fx_Text *Text;

static fx_Button *Button1;

static fx_GUILayer *GUI;

// static MoleculeNode *ParentMolecule;
// static MoleculeNode *ChildMolecule; 

static Molecule *Test1;

// static PhysicsBody2D Body1;
// static PhysicsBody2D Body2;

// static Circle2D Coll1;
// static Circle2D Coll2;

static Atom *Atom1;
static Atom *Atom2;

static std::vector<Atom*> ColliderList;

void update(float dt)
{
    
    dt = 0.16;
    dt += glm::epsilon<float>();
    auto UpdateStart = std::chrono::high_resolution_clock::now();

    double xpos, ypos;
    glfwGetCursorPos(MainWindow, &xpos, &ypos);
    // std::cout << (float)RoughWinPos.y << "\n";
    // std::cout << ModelVertex.x << "," << ModelVertex.y << "\n";
    // glm::vec3 Result = GUI->Screen2World(glm::ivec2({xpos, ypos}));

    // Atom1->m_Info.m_Position.x = Result.x;
    // Atom1->m_Info.m_Position.y = Result.y;

    // ParentMolecule->COM;

    // glm::vec2 Offset;
    // Offset.x = (ChildMolecule->RelPos.x * std::cos(ParentMolecule->Angle)) + (ChildMolecule->RelPos.y * std::sin(ParentMolecule->Angle));
    // Offset.y = (ChildMolecule->RelPos.x * std::sin(ParentMolecule->Angle)) + (ChildMolecule->RelPos.y * std::cos(ParentMolecule->Angle));

    // ChildMolecule->Matter->m_Info.m_Position.x = ParentMolecule->Matter->m_Info.m_Position.x + Offset.x;
    // ChildMolecule->Matter->m_Info.m_Position.y = ParentMolecule->Matter->m_Info.m_Position.y + Offset.y;

    Test1->Step(dt);

    // Atom1->Step(dt);
    // Atom2->Step(dt);

    Atom1->Update();
    Atom2->Update();

    // std::cout << Atom1->m_Circle->m_Info.m_Position.x << "\n";

    std::sort(ColliderList.begin(), ColliderList.end(), [](const Atom *a, const Atom *b)
    { 
        return a->m_Info.m_Position.x > b->m_Info.m_Position.x; 
    });
    std::vector<std::pair<int, int>> PossibleCollision;

    {
        // Sweep and Prune
        std::vector<int> Active;
        Active.push_back(0);
        // std::cout << ColliderList.size() << "\n" ;
        for (int i = 1; i < (int)ColliderList.size(); i++)
        {
            // if (std::binary_search(Active.begin(), Active.end(), i))
            // {
            //     continue;
            // }
            glm::vec2 Xpos;
            Xpos.x = ColliderList[i]->m_Info.m_Position.x - (ColliderList[i]->m_Info.m_Size.x / 2.0f);
            Xpos.y = ColliderList[i]->m_Info.m_Position.x + (ColliderList[i]->m_Info.m_Size.x / 2.0f);
            for (int j = 0; j < (int)Active.size(); j++)
            {
                if (i == Active[j])
                {
                    continue;
                }
                if (ColliderList[i]->m_Parent == ColliderList[Active[j]]->m_Parent)
                {
                    continue;
                }
                glm::vec2 Checkpos;
                Checkpos.x = ColliderList[Active[j]]->m_Info.m_Position.x - (ColliderList[Active[j]]->m_Info.m_Size.x / 2.0f);
                Checkpos.y = ColliderList[Active[j]]->m_Info.m_Position.x + (ColliderList[Active[j]]->m_Info.m_Size.x / 2.0f);
                if (Xpos.x <= Checkpos.y && Xpos.y >= Checkpos.x)
                {
                    PossibleCollision.push_back({i,Active[j]});
                    // std::cout << "maybe collide\n";
                }
                else
                {
                    Active.clear();
                }
                Active.push_back(i);
            }



        }
    }


    // {
    //     // glm::vec2 s = 0.0f - Body1.pos;
    //     // glm::vec2 a = (2.0f * (s - (Body1.vel * dt))) / (dt * dt);
    //     // glm::vec2 f;
    //     // f.x = std::min(std::abs(Body1.weight * a.x), 1.0f) * (a.x < 0.0f? -1.0f:1.0f);
    //     // f.y = std::min(std::abs(Body1.weight * a.y), 1.0f) * (a.y < 0.0f? -1.0f:1.0f);
    //     // Body1.acc = f / Body1.weight;

    //     Body1.vel += (Body1.acc * dt);
    //     if (Coll1.Pos.x - Coll1.Radius <-1.0f || Coll1.Pos.x + Coll1.Radius > 1.0f)
    //     {
    //         Body1.vel.x = -Body1.vel.x;
    //     }
    //     else if (Coll1.Pos.y - Coll1.Radius <-1.0f || Coll1.Pos.y + Coll1.Radius > 1.0f)
    //     {
    //         Body1.vel.y = -Body1.vel.y;
    //     }
    //     Body1.pos += (Body1.vel * dt);
    //     Coll1.Pos = Body1.pos;
    // }
    // {
    //     // glm::vec2 s = 0.0f - Body2.pos;
    //     // glm::vec2 a = (2.0f * (s - (Body2.vel * dt))) / (dt * dt);
    //     // glm::vec2 f;
    //     // f.x = std::min(std::abs(Body2.weight * a.x), 1.0f) * (a.x < 0.0f? -1.0f:1.0f);
    //     // f.y = std::min(std::abs(Body2.weight * a.y), 1.0f) * (a.y < 0.0f? -1.0f:1.0f);
    //     // Body2.acc = f / Body2.weight;

    //     Body2.vel += (Body2.acc * dt);
    //     if (Coll2.Pos.x - Coll2.Radius <-1.0f || Coll2.Pos.x + Coll2.Radius > 1.0f)
    //     {
    //         Body2.vel.x = -Body2.vel.x;
    //     }
    //     else if (Coll2.Pos.y - Coll2.Radius <-1.0f || Coll2.Pos.y + Coll2.Radius > 1.0f)
    //     {
    //         Body2.vel.y = -Body2.vel.y;
    //     }
    //     Body2.pos += (Body2.vel * dt);
    //     Coll2.Pos = Body2.pos;

    // }
    for (auto &x : ColliderList)
    {
        float radius = x->m_Info.m_Size.x / 2.0f;
        if (x->m_Info.m_Position.x - radius < -GameAspect)
        {
            x->m_vel.x = std::abs(x->m_vel.x);
        }
        else if (x->m_Info.m_Position.x + radius > GameAspect)
        {
            x->m_vel.x = -std::abs(x->m_vel.x);
        }
        if (x->m_Info.m_Position.y - radius <-1.0f)
        {
            x->m_vel.y = std::abs(x->m_vel.y);
        }
        else if (x->m_Info.m_Position.y + radius > 1.0f)
        {
            x->m_vel.y = -std::abs(x->m_vel.y);
        }
    }

    // if (fx_Collide(Coll1, Coll2))
    // {
        // float v1 =  (((Body1.weight - Body2.weight) / ((Body1.weight + Body2.weight))) * Body1.vel.x) + (((2 * Body2.weight) / (Body1.weight + Body2.weight)) * Body2.vel.x);
        // float v2 = (((2.0f * Body1.weight) / (Body1.weight + Body2.weight)) * Body1.vel.x) - (((Body2.weight - Body1.weight) / ((Body2.weight + Body1.weight))) * Body2.vel.x);
        // std::cout << Body1.vel.x << ", " << v1 << ", " << Body2.vel.x << ", " << v2 << "\n";
        // Body1.vel.x = v1;
        // Body2.vel.x = v2;


        // Body1.vel += (Body1.acc * dt);
        // Body1.pos += (Body1.vel * dt);
        // Coll1.Pos.x = Body1.pos.x;

        // Body2.vel += (Body2.acc * dt);
        // Body2.pos += (Body2.vel * dt);
        // Coll2.Pos.x = Body2.pos.x;

        // float AOA = std::atan2(Body2.pos.y - Body1.pos.y , Body2.pos.x - Body1.pos.x);
        // float AOA2 = (glm::pi<float>() - AOA) / 2;

        // std::cout << glm::degrees(AOA) << "\n";

        // v1.x = (Body1.vel.x * std::cos(AOA)) - (Body1.vel.y * std::sin(AOA));
        // v1.y = (Body1.vel.x * std::sin(AOA)) + (Body1.vel.y * std::cos(AOA));

        // auto t = ((glm::dot(Body1.vel- Body2.vel, Body1.pos - Body2.pos)) / std::pow(glm::length(Body1.pos - Body2.pos),2));

        // glm::vec2 v1 = Body1.vel - (((2.0f * Body2.weight) / (Body1.weight + Body2.weight)) * (float)(glm::dot(Body1.vel - Body2.vel, Body1.pos - Body2.pos) / std::pow(glm::length(Body1.pos - Body2.pos),2)) * (Body1.pos - Body2.pos));
        // glm::vec2 v2 = Body2.vel - (((2.0f * Body1.weight) / (Body1.weight + Body2.weight)) * (float)(glm::dot(Body2.vel - Body1.vel, Body2.pos - Body1.pos) / std::pow(glm::length(Body2.pos - Body1.pos),2)) * (Body2.pos - Body1.pos));
    //     float COR = 1.0f;
    //     glm::vec2 v1 = (((COR * Body1.weight * (Body2.vel - Body1.vel))  + (Body1.weight * Body1.vel) + (Body2.weight * Body2.vel))/ (Body1.weight + Body2.weight));
    //     glm::vec2 v2 = (((COR * Body1.weight * (Body1.vel - Body2.vel))  + (Body1.weight * Body1.vel) + (Body2.weight * Body2.vel))/ (Body1.weight + Body2.weight));
        

    //     // v1.y = (v1.x * std::sin(-AOA));

    //     Body1.vel = v1;
    //     Body2.vel = v2;

    //     // glm::vec2 v2;
    //     // v2.x = (Body2.vel.x * std::cos(AOA)) - (Body2.vel.y * std::sin(AOA));
    //     // v2.y = (Body2.vel.x * std::sin(AOA)) + (Body2.vel.y * std::cos(AOA));
    //     Body1.vel += (Body1.acc * dt);
    //     Body1.pos += (Body1.vel * dt);
    //     Coll1.Pos = Body1.pos;

    //     Body2.vel += (Body2.acc * dt);
    //     Body2.pos += (Body2.vel * dt);
    //     Coll2.Pos = Body2.pos;
    // }



    Circle1->m_Info.m_Position = Test1->m_Info.m_Position;
    // Circle2->m_Info.m_Position = {Body2.pos, Circle2->m_Info.m_Position.z};
    Circle1->Update();
    // Circle2->Update();
    Group1->Update();

    // Button1->m_Info.m_Position.x = Result.x;
    // Button1->m_Info.m_Position.y = Result.y;
    // Button1->Update();
    // GUI->Update();



    // glm::vec4 NewClr;
    // if (glfwGetWindowAttrib(MainWindow, GLFW_HOVERED))
    // {
    //     NewClr = {0.0, 1.0, 0.0, 1.0};
    // }
    // else
    // {
    //     NewClr = {1.0, 0.0, 0.0, 1.0};
    // }
    // if (Circle1->m_Info.m_Color != NewClr)
    // {
    //     Circle1->m_Info.m_Color = NewClr;
    //     RenderDemand = true;
    //     Circle1->Update();
    //     Group1->Update();
    // }

    // Circle1->GenerateMesh();
    // Group1->GenerateMesh();

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

    Circle1 = new fx_Circle({-0.5f, 0.1f, -2}, glm::vec2(0.2f, 0.2f), {1.0f,0.0f,0.0f,1.0f});
    Circle1->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    Circle1->Update();
    Group1->m_Objects.push_back(Circle1);

    // Circle2 = new fx_Circle({0.8f, 0.8f, -2}, glm::vec2(0.2f, 0.2f), {0.0f,1.0f,0.0f,1.0f});
    // Circle2->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    // Circle2->Update();
    // Group1->m_Objects.push_back(Circle2);

    Atom1 = new Atom({0.0f, 0.0f, -2}, glm::vec2(0.2f, 0.2f), {0.0f,0.0f,1.0f,1.0f});
    Atom1->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    Atom1->m_offset = {0,0};
    Atom1->m_vel = {0.05f , 0.1f};
    Atom1->Update();
    // Group1->m_Objects.push_back(Atom1);

    Atom2 = new Atom({0.0f, 0.0f, -2}, glm::vec2(0.2f, 0.2f), {0.0f,1.0f,1.0f,1.0f});
    Atom2->m_Info.m_Anchor = {0.5f, 0.5f, 0};
    Atom2->m_offset = {0.4,0};
    Atom2->m_vel = {-0.05f , -0.1f};
    // Atom2->m_vel = {0.0f , -0.1f};
    Atom2->Update();
    // Group1->m_Objects.push_back(Atom2);

    ColliderList.push_back(Atom1);
    ColliderList.push_back(Atom2);

    // MoleculeNode ParentMolecule;
    // ParentMolecule = new MoleculeNode;
    // ChildMolecule = new MoleculeNode;

    Test1 = new Molecule({0.0f, 0.0f, -2});

    // Test1->m_Info.m_Rotation = glm::quat({0.0f,0.0f, glm::radians(90.0f)});

    Test1->Add(Atom1);
    Test1->Add(Atom2);
    Group1->m_Objects.push_back(Test1);

    Test1->CalculateCOM();

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