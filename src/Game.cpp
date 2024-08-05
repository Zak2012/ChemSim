#include "Game.hpp"

#include <chrono>
#include <vector>
#include <thread>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// #include "../Global.hpp"
#include "Object.hpp"
#include "File.hpp"
#include "Font.hpp"
#include "ColorConvert.hpp"

static bool CheckCollision(fx_Objects *Obj, glm::vec2 Movement, Rect2D Check)
{
    Rect2D FutureStep = Rect2D(Obj->m_BoundingBox) + Movement;
    glm::vec2 FuturePos = glm::vec2(Obj->m_Info.m_Position) + Movement;
    return fx_Collide(Line2D({glm::vec2(Obj->m_Info.m_Position), FuturePos}), Check)|| // look for tunneling
           fx_Collide(FutureStep, Check);
}

static bool CheckCollision(fx_Objects *Obj, glm::vec2 Movement, std::vector<Rect2D> Check)
{
    for (const auto &x: Check)
    {
        if (CheckCollision(Obj, Movement, x))
        {
            return true;
        }
    }
    return false;
}

static glm::ivec2 WindowSize = {1280,720};
static glm::ivec2 GameSize = {1280,720};
static glm::ivec2 ActualGameSize = {1280,720};

static std::unordered_map<fx_BasicType, fx_Program*> Programs;
static fx_Font_Library Lib;

static bool isPositive = true;

static float DeltaTime = 0.0f;

static fx_Face TimesFace;
static fx_Face ArialFace;

static fx_Framebuffer *GameBuffer;
static fx_Framebuffer *UIBuffer;

static fx_Group *Group1;
static fx_Group *Group2;
static fx_Group *UIGroup;

static fx_Group *UIRender;
static fx_Group *GameRender;

static fx_Sprite *Renderer;
static fx_Sprite *UIRenderer;
static fx_Quad *Quad2;
static fx_Text *Text;
static fx_Text *Text2;
static std::vector<fx_Sprite*> Stairs;
static std::vector<fx_Sprite*> Grounds;
static fx_Sprite *Sprite1;
static fx_Circle *Circle1;
static fx_Quad *Background;

static fx_Animator AnniGo;
static fx_Animator AnniBack;
static fx_Animator Anni;

static std::vector<Rect2D> GroundCollider;
static std::vector<Rect2D> StairCollider;

static glm::vec2 SpriteVelocity = {0.0f,0.0f};

static glm::mat4 LookAtMat;
static glm::mat4 CamMat;
static glm::mat4 RenderLookAtMat;

static glm::vec2 CamPos = {0.0f,0.0f};

static glm::vec2 CamOffset = {0.0f, 0.0f};
static float CamTime = 0.5f;
static glm::vec2 Camvelocity = {0.0f, 0.0f};
static fx_Animator CamFollow;

static float CoyoteTime = 0.1f;
static float CoyoteTimeCounter;

static float JumpBufferTime = 0.1f;
static float JumpBufferCounter;

static glm::vec2 RespawnPos = {-1.75f,0.0f};


glm::mat4 TranslateCamera(glm::vec2 Offset)
{
    return glm::ortho( Offset.x+(-(float)GameSize.x/(float)GameSize.y), Offset.x+((float)GameSize.x/(float)GameSize.y), Offset.y+(-1.0f), Offset.y+(1.0f), 0.1f, 10.0f );
}

glm::mat4 ZoomCamera(glm::vec2 Offset)
{
    return glm::ortho( Offset.x*(-(float)GameSize.x/(float)GameSize.y), Offset.x*((float)GameSize.x/(float)GameSize.y), Offset.y*(-1.0f), Offset.y*(1.0f), 0.1f, 10.0f );
}


void GameStart()
{
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
    
    LookAtMat = glm::ortho( -(float)GameSize.x/(float)GameSize.y, (float)GameSize.x/(float)GameSize.y, -1.0f, 1.0f, 0.1f, 10.0f );
    glm::mat4 InvLookAtMat = glm::inverse(LookAtMat);
    RenderLookAtMat = glm::identity<glm::mat4>();

    for (auto x : Programs)
    {
        x.second->SetUniform(LookAtMat, "Matrix");
    }

     auto LastFrame = std::chrono::high_resolution_clock::now();

}

void GameUpdate(float dt)
{
    
}