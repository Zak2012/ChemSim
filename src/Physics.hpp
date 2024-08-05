#pragma once

#include <vector>
#include <cmath>

#include <glm/glm.hpp>

struct Ray2D
{
    glm::vec2 Pos;
    glm::vec2 Dir;
};

struct Ray3D
{
    glm::vec3 Pos;
    glm::vec3 Dir;
};

struct Circle2D
{
    glm::vec2 Pos;
    float Radius;
};

struct Circle3D
{
    glm::vec3 Pos;
    float Radius;
    operator Circle2D() const
    {
        Circle2D Result;
        Result.Radius = Radius;
        Result.Pos = glm::vec2(Pos.x, Pos.y);
        return Result;
    }

};

struct Line2D
{
    glm::vec2 Start;
    glm::vec2 End;
};

struct Line3D
{
    glm::vec3 Start;
    glm::vec3 End;
    operator Line2D() const
    {
        Line2D Result;
        Result.Start = glm::vec2(Start.x, Start.y);
        Result.End = glm::vec2(End.x, End.y);
        return Result;
    }
};

struct Poly2D
{
    std::vector<glm::vec2> Corners;
};

struct Poly3D
{
    std::vector<glm::vec3> Corners;
    operator Poly2D() const
    {
        Poly2D Result;
        for (const auto &x: Corners)
        {
            Result.Corners.push_back({x.x, x.y});
        }
     return Result;
    }
};

struct Rect2D
{
    glm::vec2 Min;
    glm::vec2 Max;
    Rect2D(){}
    Rect2D(glm::vec2 x, glm::vec2 y)
    {
        Min = x;
        Max = y;
    }
    Rect2D operator+ (const float &rhs) const
    {
        return Rect2D({Min + rhs, Max + rhs});
    }
    Rect2D operator- (const float &rhs) const
    {
        return Rect2D({Min - rhs, Max - rhs});
    }
    Rect2D operator+ (const glm::vec2 &rhs) const
    {
        return Rect2D({Min + rhs, Max + rhs});
    }
    Rect2D operator- (const glm::vec2 &rhs) const
    {
        return Rect2D({Min - rhs, Max - rhs});
    }
    Rect2D& operator+= (const glm::vec2& rhs)
    {
        this->Min += rhs;
        this->Max += rhs;
        return *this;
    }
};

struct Rect3D
{
    glm::vec3 Min;
    glm::vec3 Max;
    operator Rect2D() const
    {
        Rect2D Result;
        Result.Min = glm::vec2(Min.x, Min.y);
        Result.Max = glm::vec2(Max.x, Max.y);
     return Result;
    }
};

struct WorldNode2D
{
    WorldNode2D *Right = NULL;
    WorldNode2D *Left = NULL;
    WorldNode2D *Parent = NULL;
    Rect3D Rect;
    unsigned int ID = 0;
    ~WorldNode2D()
    {
        if (Right != NULL)
        {
            delete Right;
        }
        if (Left != NULL)
        {
            delete Left;
        }
    }
};

struct PhysicsBody2D
{
    glm::vec2 vel;
    glm::vec2 acc;
    // float max_force;
    // float force;
    float weight;
    glm::vec2 pos; 
};

//https://www.jeffreythompson.org/collision-detection/index.php
//2d
bool fx_Collide(glm::vec2 A, glm::vec2 B);
bool fx_Collide(glm::vec2 A, Line2D B);
bool fx_Collide(glm::vec2 A, Circle2D B);
bool fx_Collide(glm::vec2 A, Rect2D B);
bool fx_Collide(glm::vec2 A, Poly2D B);

bool fx_Collide(Line2D A, glm::vec2 B);
bool fx_Collide(Line2D A, Line2D B);
bool fx_Collide(Line2D A, Circle2D B);
bool fx_Collide(Line2D A, Rect2D B);
bool fx_Collide(Line2D A, Poly2D B);

bool fx_Collide(Circle2D A, glm::vec2 B);
bool fx_Collide(Circle2D A, Circle2D B);
bool fx_Collide(Circle2D A, Line2D B);
bool fx_Collide(Circle2D A, Rect2D B);
bool fx_Collide(Circle2D A, Poly2D B);

bool fx_Collide(Rect2D A, glm::vec2 B);
bool fx_Collide(Rect2D A, Circle2D B);
bool fx_Collide(Rect2D A, Line2D B);
bool fx_Collide(Rect2D A, Rect2D B);
bool fx_Collide(Rect2D A, Poly2D B);

bool fx_Collide(Poly2D A, glm::vec2 B);
bool fx_Collide(Poly2D A, Circle2D B);
bool fx_Collide(Poly2D A, Line2D B);
bool fx_Collide(Poly2D A, Rect2D B);
bool fx_Collide(Poly2D A, Poly2D B);

bool fx_Collide(Rect3D A, Rect3D B);
bool fx_Collide(Ray3D A, Rect3D B);
bool fx_Collide(Ray2D A, Rect2D B);

Rect2D fx_GetRect(glm::vec2 A);
Rect2D fx_GetRect(Circle2D A);
Rect2D fx_GetRect(Line2D A);
Rect2D fx_GetRect(Poly2D A);
Rect2D fx_GetRect(std::vector<Rect2D> A);

bool fx_Meet(Line2D A, Line2D B, glm::vec2 &C);
bool fx_Meet(Line2D A, Rect2D B, std::vector<glm::vec2> &C);
bool fx_Meet(Ray3D A, Rect3D B, glm::vec3 &C);
bool fx_Meet(Ray2D A, Rect2D B, glm::vec2 &C);

unsigned int fx_ClosestPoint(Poly2D Polygon, Line2D Line);
glm::vec2 fx_ClosestPoint(glm::vec2 Point, Line2D Line);

// float fx_Balance(Poly2D Polygon, glm::vec2 ForceAxis);

bool fx_CheckWorld(glm::vec2 Start, glm::vec2 End, unsigned int ID, WorldNode2D *World);

glm::vec3 fx_WorldSpace2ScreenSpace(glm::vec3 A, glm::mat4 World);
Rect3D fx_WorldSpace2ScreenSpace(Rect3D A, glm::mat4 World);
Poly3D fx_WorldSpace2ScreenSpace(Poly3D A, glm::mat4 World);
glm::vec3 fx_ScreenSpace2WorldSpace(glm::vec2 A, glm::mat4 InvWorld);

float fx_SmoothDamp(float current, float target, float &currentVelocity, float smoothTime, float deltaTime, float maxSpeed=INFINITY);
glm::vec2 fx_SmoothDamp(glm::vec2 currentPos, glm::vec2 target, glm::vec2 &currentVelocity, float smoothTime, float deltaTime, float maxSpeed=INFINITY);

