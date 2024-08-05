#include "Physics.hpp"

#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

float HeronsFormula(glm::vec2 A, glm::vec2 B, glm::vec2 C)
{
    return std::abs( (B.x - A.x)*(C.y - A.y) - (C.x - A.x)*(B.y - A.y));
    // float a = glm::distance(A,B);
    // float b = glm::distance(B,C);
    // float c = glm::distance(C,A);

    // float s = 0.5f * (a+b+c);
    // return std::sqrt(s * (s - a) * (s - b) * (s - c));
}

glm::vec2 FindCentroid(Poly2D A)
{
    glm::vec2 Sum = {0.0f,0.0f};
    for (const auto &x: A.Corners)
    {
        Sum = Sum + x;
    }
    return Sum / (float)A.Corners.size();
}

glm::vec2 LinearExtrapolate(Line2D A, float X)
{
    float Y = (A.Start.y * X) + (((X + A.Start.x) / (A.End.x - A.Start.x)) * (A.End.y - A.Start.y));
    return glm::vec2(X, Y);
}

float AngleOfLines(Line2D A, Line2D B)
{
    glm::vec2 L1 = A.End - A.Start;
    glm::vec2 L2 = B.End - B.Start;
    return std::acos(glm::dot(L1,L2)/(glm::distance(A.Start, A.End) * (glm::distance(B.Start, B.End))));
}

//2d
bool fx_Collide(glm::vec2 A, glm::vec2 B)
{
    return A == B;
}

bool fx_Collide(glm::vec2 A, Circle2D B)
{
    return glm::distance(A, B.Pos) <= B.Radius;
}

bool fx_Collide(Circle2D A, Circle2D B)
{
    float distance = glm::distance(A.Pos, B.Pos);
    return A.Radius + B.Radius >= distance;
}

bool fx_Collide(Circle2D A, Rect2D B)
{
    // temporary variables to set edges for testing
    glm::vec2 Test = A.Pos;

    // which edge is closest?
    if (A.Pos.x < B.Min.x)
    {
        Test.x = B.Min.x;  // test left edge
    }
    else if (A.Pos.x > B.Max.x)
    {
        Test.x = B.Max.x;  // right edge
    }
    if (A.Pos.y < B.Min.y)
    {
        Test.y = B.Min.y;  // top edge
    }
    else if (A.Pos.y > B.Max.y)
    {
        Test.y = B.Max.y;  // bottom edge
    }

    // get distance from closest edges
    float distance = glm::distance(A.Pos, Test);

    // if the distance is less than the radius, collision!
    if (distance <= A.Radius) {
        return true;
    }
    return false;
}

bool fx_Collide(Rect2D A, Rect2D B)
{
    return A.Min.x <= B.Max.x && A.Max.x >= B.Min.x &&
           A.Min.y <= B.Max.y && A.Max.y >= B.Min.y;
}

bool fx_Collide(glm::vec2 A, Rect2D B)
{
    return A.x <= B.Max.x && A.x >= B.Min.x &&
           A.y <= B.Max.y && A.y >= B.Min.y;
}

bool fx_Collide(glm::vec2 A, Line2D B)
{
    float d1 = glm::distance(A, B.Start);
    float d2 = glm::distance(A, B.End);
    return (d1 + d2) == glm::distance(B.Start, B.End);
}

bool fx_Collide(Circle2D A, Line2D B)
{
    if (fx_Collide(B.Start, A) || fx_Collide(B.End, A))
    {
        return true;
    }
    float dot = glm::dot((B.End - B.Start), A.Pos);
    glm::vec2 closest = B.Start + (dot * (B.End - B.Start));
    if (fx_Collide(closest, B))
    {
        return true;
    }
    return fx_Collide(closest, A);
}

bool fx_Collide(Line2D A, Line2D B)
{
    glm::vec2 L1 = B.End - B.Start;
    glm::vec2 L2 = A.End - A.Start;
    glm::vec2 L3 = A.Start - B.Start;
    float uA = (L1.x*L3.y - L1.y*L3.x) / (L1.y*L2.x - L1.x*L2.y);
    float uB = (L2.x*L3.y - L2.y*L3.x) / (L1.y*L2.x - L1.x*L2.y);
    if (uA >= 0.0f && uA <= 1.0f && uB >= 0.0f && uB <= 1.0f) 
    {
        return true;
    }
    return false;
}

bool fx_Collide(Line2D A, Rect2D B)
{
    return fx_Collide(A, Line2D({{B.Min.x, B.Min.y},{B.Min.x, B.Max.y}})) || 
           fx_Collide(A, Line2D({{B.Min.x, B.Max.y},{B.Max.x, B.Max.y}})) ||
           fx_Collide(A, Line2D({{B.Max.x, B.Max.y},{B.Max.x, B.Min.y}})) ||
           fx_Collide(A, Line2D({{B.Max.x, B.Min.y},{B.Min.x, B.Min.y}}));
}


bool fx_Collide(Poly2D A, Poly2D B)
{
    for (unsigned int i = 0; i < B.Corners.size(); i++)
    {
        if (fx_Collide(Line2D({B.Corners[i], B.Corners[i + 1 == B.Corners.size()? 0:i+1]}), A))
        {
            return true;
        }
    }
    return fx_Collide(A.Corners[0], B);
}

bool fx_Collide(glm::vec2 A, Poly2D B)
{
    if (B.Corners.size() == 3)
    {
        if (HeronsFormula(A, B.Corners[0], B.Corners[1]) + 
            HeronsFormula(A, B.Corners[1], B.Corners[2]) +
            HeronsFormula(A, B.Corners[2], B.Corners[0]) == 
            HeronsFormula(B.Corners[1], B.Corners[2], B.Corners[3]))
        {
            return true;
        }
        return false;
    }
    else
    {
        bool is_colliding = false;
        for (unsigned int i = 0; i < B.Corners.size(); i++)
        {
            glm::vec2 vc = B.Corners[i];
            glm::vec2 vn = B.Corners[i + 1 == B.Corners.size()? 0:i+1];
            if ( ((vc.y > A.y) != (vn.y > A.y)) && (A.x < (vn.x-vc.x) * (A.y-vc.y) / (vn.y-vc.y) + vc.x) ) {
                is_colliding = !is_colliding;
            }
        }
        return is_colliding;
    }
}

bool fx_Collide(Circle2D A, Poly2D B)
{
    for (unsigned int i = 0; i < B.Corners.size(); i++)
    {
        if (fx_Collide(A, Line2D({B.Corners[i], B.Corners[i + 1 == B.Corners.size()? 0:i+1]})))
        {
            return true;
        }
    }
    return fx_Collide(A.Pos, B);
}

bool fx_Collide(Rect2D A, Poly2D B)
{
    for (unsigned int i = 0; i < B.Corners.size(); i++)
    {
        if (fx_Collide(Line2D({B.Corners[i], B.Corners[i + 1 == B.Corners.size()? 0:i+1]}), A))
        {
            return true;
        }
    }
    return fx_Collide((A.Min+A.Max)/2.0f, B);
}

bool fx_Collide(Line2D A, Poly2D B)
{
    for (unsigned int i = 0; i < B.Corners.size(); i++)
    {
        if (fx_Collide(Line2D({B.Corners[i], B.Corners[i + 1 == B.Corners.size()? 0:i+1]}), A))
        {
            return true;
        }
    }
    return false;
}

bool fx_Collide(Line2D A, glm::vec2 B)
{
    return fx_Collide(B, A);
}

bool fx_Collide(Line2D A, Circle2D B)
{
    return fx_Collide(B, A);
}

bool fx_Collide(Circle2D A, glm::vec2 B)
{
    return fx_Collide(B, A);
}

bool fx_Collide(Rect2D A, glm::vec2 B)
{
    return fx_Collide(B, A);
}

bool fx_Collide(Rect2D A, Circle2D B)
{
    return fx_Collide(B, A);
}

bool fx_Collide(Rect2D A, Line2D B)
{
    return fx_Collide(B, A);
}

bool fx_Collide(Poly2D A, glm::vec2 B)
{
    return fx_Collide(B, A);
}

bool fx_Collide(Poly2D A, Circle2D B)
{
    return fx_Collide(B, A);
}

bool fx_Collide(Poly2D A, Line2D B)
{
    return fx_Collide(B, A);
}

bool fx_Collide(Poly2D A, Rect2D B)
{
    return fx_Collide(B, A);
}

bool fx_Collide(Rect3D A, Rect3D B)
{
    return A.Min.x <= B.Max.x && A.Max.x >= B.Min.x &&
           A.Min.y <= B.Max.y && A.Max.y >= B.Min.y &&
           A.Min.z <= B.Max.z && A.Max.z >= B.Min.z;
}

bool fx_Collide(Ray3D A, Rect3D B)
{
    glm::vec3 invdir = 1.0f / A.Dir;
    float tmin,tmax,tymin,tymax,tzmin,tzmax;

    tmin = ((invdir.x>=0.0f?B.Min.x:B.Max.x) - A.Pos.x) * invdir.x; 
    tmax = ((invdir.x>=0.0f?B.Max.x:B.Min.x) - A.Pos.x) * invdir.x; 
 
    tymin = ((invdir.y>=0.0f?B.Min.y:B.Max.y) - A.Pos.y) * invdir.y; 
    tymax = ((invdir.y>=0.0f?B.Max.y:B.Min.y) - A.Pos.y) * invdir.y; 

    if ((tmin > tymax) || (tymin > tmax)) 
    {
        return false; 
    }
 
    if (tymin > tmin)
    {
        tmin = tymin; 
    }
 
    if (tymax < tmax) 
    {
        tmax = tymax; 
    }
 
    tzmin = ((invdir.z>=0.0f?B.Min.z:B.Max.z) - A.Pos.z) * invdir.z; 
    tzmax = ((invdir.z>=0.0f?B.Max.z:B.Min.z) - A.Pos.z) * invdir.z; 

    if ((tmin > tzmax) || (tzmin > tmax))
    {
        return false;
    }
 
    if (tzmin > tmin)
    {
        tmin = tzmin; 
    }
 
    if (tzmax < tmax) 
    {
        tmax = tzmax; 
    }

    if (tmax < 0.0f || tmin < 0.0f)
    {
        return false;
    }
    return true;
}

bool fx_Collide(Ray2D A, Rect2D B)
{
    glm::vec2 invdir = 1.0f / A.Dir;
    float tmin,tmax,tymin,tymax;

    tmin = ((invdir.x>=0.0f?B.Min.x:B.Max.x) - A.Pos.x) * invdir.x; 
    tmax = ((invdir.x>=0.0f?B.Max.x:B.Min.x) - A.Pos.x) * invdir.x; 
 
    tymin = ((invdir.y>=0.0f?B.Min.y:B.Max.y) - A.Pos.y) * invdir.y; 
    tymax = ((invdir.y>=0.0f?B.Max.y:B.Min.y) - A.Pos.y) * invdir.y; 

    if ((tmin > tymax) || (tymin > tmax)) 
    {
        return false; 
    }
 
    if (tymin > tmin)
    {
        tmin = tymin; 
    }
 
    if (tymax < tmax) 
    {
        tmax = tymax; 
    }
 
    if (tmax < 0.0f || tmin < 0.0f)
    {
        return false;
    }
    return true;
}

Rect2D fx_GetRect(glm::vec2 A)
{
    return Rect2D({A,A});
}

Rect2D fx_GetRect(Circle2D A)
{
    return Rect2D({A.Pos - A.Radius, A.Pos + A.Radius});
}

Rect2D fx_GetRect(Line2D A)
{
    return Rect2D({{std::min(A.Start.x, A.End.x),std::min(A.Start.y, A.End.y)}, {std::max(A.Start.x, A.End.x),std::max(A.Start.y, A.End.y)}});
}

Rect2D fx_GetRect(Poly2D A)
{
    Rect2D Result;
    std::vector<glm::vec2> SortedPos = A.Corners;
    std::sort(std::begin(SortedPos), std::end(SortedPos),
          [] (const auto& lhs, const auto& rhs) {
        return lhs.x < rhs.x;
        });
    Result.Min.x = SortedPos.front().x;
    Result.Max.x = SortedPos.back().x;

    std::sort(std::begin(SortedPos), std::end(SortedPos),
          [] (const auto& lhs, const auto& rhs) {
        return lhs.y < rhs.y;
        });
    Result.Min.y = SortedPos.front().y;
    Result.Max.y = SortedPos.back().y;
    return Result;
}

Rect2D fx_GetRect(std::vector<Rect2D> A)
{
    Rect2D Result;
    std::sort(std::begin(A), std::end(A),
          [] (const auto& lhs, const auto& rhs) {
        return lhs.Min.x < rhs.Min.x;
        });
    Result.Min.x = A.front().Min.x;

    std::sort(std::begin(A), std::end(A),
          [] (const auto& lhs, const auto& rhs) {
        return lhs.Max.x < rhs.Max.x;
        });
    Result.Max.x = A.front().Max.x;

    std::sort(std::begin(A), std::end(A),
          [] (const auto& lhs, const auto& rhs) {
        return lhs.Min.y < rhs.Min.y;
        });
    Result.Min.y = A.front().Min.y;

    std::sort(std::begin(A), std::end(A),
          [] (const auto& lhs, const auto& rhs) {
        return lhs.Max.y < rhs.Max.y;
        });
    Result.Max.y = A.front().Max.y;
    return Result;
}

bool fx_Meet(Line2D A, Line2D B, glm::vec2 &C)
{
    glm::vec2 L1 = B.End - B.Start;
    glm::vec2 L2 = A.End - A.Start;
    glm::vec2 L3 = A.Start - B.Start;
    float uA = (L1.x*L3.y - L1.y*L3.x) / (L1.y*L2.x - L1.x*L2.y);
    float uB = (L2.x*L3.y - L2.y*L3.x) / (L1.y*L2.x - L1.x*L2.y);
    C = A.Start + (uA * L2);
    if (uA >= 0.0f && uA <= 1.0f && uB >= 0.0f && uB <= 1.0f) 
    {
        return true;
    }
    return false;
}

bool fx_Meet(Line2D A, Rect2D B, std::vector<glm::vec2> &C)
{
    glm::vec2 P1;
    bool L1 = fx_Meet(A, Line2D({{B.Min.x, B.Min.y},{B.Min.x, B.Max.y}}), P1);
    if (L1)
    {
        C.push_back(P1);
    }
    bool L2 = fx_Meet(A, Line2D({{B.Min.x, B.Max.y},{B.Max.x, B.Max.y}}), P1);
    if (L2)
    {
        C.push_back(P1);
    }
    bool L3 = fx_Meet(A, Line2D({{B.Max.x, B.Max.y},{B.Max.x, B.Min.y}}), P1);
    if (L3)
    {
        C.push_back(P1);
    }
    bool L4 = fx_Meet(A, Line2D({{B.Max.x, B.Min.y},{B.Min.x, B.Min.y}}), P1);
    if (L4)
    {
        C.push_back(P1);
    }
    return L1 || L2 || L3 || L4;
}

bool fx_Meet(Ray3D A, Rect3D B, glm::vec3 &C)
{
    glm::vec3 invdir = 1.0f / A.Dir;
    float tmin,tmax,tymin,tymax,tzmin,tzmax;

    tmin = ((invdir.x>=0.0f?B.Min.x:B.Max.x) - A.Pos.x) * invdir.x; 
    tmax = ((invdir.x>=0.0f?B.Max.x:B.Min.x) - A.Pos.x) * invdir.x; 
 
    tymin = ((invdir.y>=0.0f?B.Min.y:B.Max.y) - A.Pos.y) * invdir.y; 
    tymax = ((invdir.y>=0.0f?B.Max.y:B.Min.y) - A.Pos.y) * invdir.y; 

    if ((tmin > tymax) || (tymin > tmax)) 
    {
        return false; 
    }
 
    if (tymin > tmin)
    {
        tmin = tymin; 
    }
 
    if (tymax < tmax) 
    {
        tmax = tymax; 
    }
 
    tzmin = ((invdir.z>=0.0f?B.Min.z:B.Max.z) - A.Pos.z) * invdir.z; 
    tzmax = ((invdir.z>=0.0f?B.Max.z:B.Min.z) - A.Pos.z) * invdir.z; 

    if ((tmin > tzmax) || (tzmin > tmax))
    {
        return false;
    }
 
    if (tzmin > tmin)
    {
        tmin = tzmin; 
    }
 
    if (tzmax < tmax) 
    {
        tmax = tzmax; 
    }

    if (tmax < 0.0f || tmin < 0.0f)
    {
        return false;
    }

    C = A.Pos + (A.Dir * tmin);
 
    return true; 
}

bool fx_Meet(Ray2D A, Rect2D B, glm::vec2 &C)
{
    glm::vec2 invdir = 1.0f / A.Dir;
    float tmin,tmax,tymin,tymax;

    tmin = ((invdir.x>=0.0f?B.Min.x:B.Max.x) - A.Pos.x) * invdir.x; 
    tmax = ((invdir.x>=0.0f?B.Max.x:B.Min.x) - A.Pos.x) * invdir.x; 
 
    tymin = ((invdir.y>=0.0f?B.Min.y:B.Max.y) - A.Pos.y) * invdir.y; 
    tymax = ((invdir.y>=0.0f?B.Max.y:B.Min.y) - A.Pos.y) * invdir.y; 

    if ((tmin > tymax) || (tymin > tmax)) 
    {
        return false; 
    }
 
    if (tymin > tmin)
    {
        tmin = tymin; 
    }
 
    if (tymax < tmax) 
    {
        tmax = tymax; 
    }
 
    if (tmax < 0.0f || tmin < 0.0f)
    {
        return false;
    }

    C = A.Pos + (A.Dir * tmin);

    return true;
}


unsigned int fx_ClosestPoint(Poly2D Polygon, Line2D Line)
{
    float ClosestDistance = 0.0f;
    unsigned int ClosestIndex = 0;
    for(unsigned int i = 0; i < Polygon.Corners.size(); i++)
    {
        glm::vec2 L1 = Line.End - Line.Start;
        float dot = glm::dot(L1, Polygon.Corners[i]);
        glm::vec2 closest = Line.Start + (dot * L1);
        float Distance = glm::distance(Polygon.Corners[i], closest);
        if (i == 0 || Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            ClosestIndex = i;
        }
    }
    return ClosestIndex;
}

glm::vec2 fx_ClosestPoint(glm::vec2 Point, Line2D Line)
{
    glm::vec2 L1 = Line.End - Line.Start;
    float dot = glm::dot(L1, Point);
    glm::vec2 closest = Line.Start + (dot * L1);
}

float fx_Balance(Poly2D Polygon, glm::vec2 ForceAxis)
{
    glm::vec2 ForcePerpendicular = glm::vec2(ForceAxis.y, -ForceAxis.x);

    glm::vec2 Centre = FindCentroid(Polygon);
    Rect2D Bound = fx_GetRect(Polygon);
    float Width = Bound.Max.x - Bound.Min.x;
    float Height = Bound.Max.y - Bound.Min.y;
    float Hypotenus = glm::distance(Bound.Min, Bound.Max);

    glm::vec2 BoxCentre = (Bound.Min + Bound.Max) / 2.0f;
    glm::vec2 GroundCentre = BoxCentre + (ForcePerpendicular * Hypotenus);
    glm::vec2 GroundStart = GroundCentre - (ForceAxis * (Hypotenus / 2.0f));

    Line2D Ground = Line2D({GroundCentre - (ForceAxis * (Hypotenus / 2.0f)),GroundCentre + (ForceAxis * (Hypotenus / 2.0f))});

    unsigned int ClosestPoint = fx_ClosestPoint(Polygon, Ground);

    unsigned int PNeighbour1 = (ClosestPoint + 1) == Polygon.Corners.size()? 0:(ClosestPoint + 1);
    unsigned int PNeighbour2 = ClosestPoint == 0? (Polygon.Corners.size() - 1):(ClosestPoint - 1);


    Line2D N1 = {Polygon.Corners[ClosestPoint], Polygon.Corners[PNeighbour1]};
    //extend line to end of box
    N1.Start = LinearExtrapolate(N1, Bound.Min.x);
    N1.End = LinearExtrapolate(N1, Bound.Max.x);

    Poly2D Poly1;
    for (unsigned int i = 0; i < Polygon.Corners.size(); i++)
    {
        if (i == ClosestPoint || i == PNeighbour1)
        {
            continue;
        }
        Poly1.Corners.push_back(Polygon.Corners[i]);
    }
    if (fx_Collide(N1, Poly1))
    {
        PNeighbour1 = (PNeighbour1 + 1) == Polygon.Corners.size()? 0:(PNeighbour1 + 1);
        N1.End = Polygon.Corners[PNeighbour1];
    }

    Line2D N2 = {Polygon.Corners[ClosestPoint], Polygon.Corners[PNeighbour2]};
    N2.Start = LinearExtrapolate(N2, Bound.Min.x);
    N2.End = LinearExtrapolate(N2, Bound.Max.x); // raycast instead of line

    Poly2D Poly2 = Polygon;
    for (unsigned int i = 0; i < Polygon.Corners.size(); i++)
    {
        if (i == ClosestPoint || i == PNeighbour2)
        {
            continue;
        }
        Poly2.Corners.push_back(Polygon.Corners[i]);
    }
    if (fx_Collide(N2, Poly2))// need improvement: not just one vertex, can be many
    {
        PNeighbour2 = ClosestPoint == 0? (Polygon.Corners.size() - 1):(ClosestPoint - 1);
        N2.End = Polygon.Corners[PNeighbour2];
    }

    unsigned int NextClosestPoint = 0;

    if (glm::distance(Centre, fx_ClosestPoint(Centre, N1)) < 
        glm::distance(Centre, fx_ClosestPoint(Centre, N2)))
    {
        NextClosestPoint = PNeighbour1;
    }
    else
    {
        NextClosestPoint = PNeighbour2;
    }

    Line2D stable = {Polygon.Corners[ClosestPoint], Polygon.Corners[NextClosestPoint]};
    float Angle = AngleOfLines(Ground, stable);
    if (Angle > glm::radians(180.0f))
    {
        Angle = Angle - glm::radians(180.0f);
    }
    else if (Angle > glm::radians(90.0f))
    {
        Angle = Angle - glm::radians(90.0f);
    }
    else
    {
        Angle = Angle;
    }
    return Angle;
}

void fx_UpdateWorld(WorldNode2D *World, std::vector<WorldNode2D> Objects)
{

}


bool fx_CheckWorld(glm::vec2 Start, glm::vec2 End, unsigned int ID, WorldNode2D *World)
{
    Line2D L3 = {Start,End};

    WorldNode2D *CurNode = World;
    unsigned int IDCollide = 0;

    while(CurNode->ID != ID)
    {
        if (CurNode->ID == 0)
        {
            bool RightCollide,LeftCollide;
            if(CurNode->Right != NULL)
            {
                RightCollide = fx_Collide(CurNode->Right->Rect, L3);
            }
            if(CurNode->Left != NULL)
            {
                LeftCollide = fx_Collide(CurNode->Left->Rect, L3);
            }
        }
        else
        {
            IDCollide = CurNode->ID;
            break;
        }
    }

    return IDCollide == 0;
}

glm::vec3 fx_WorldSpace2ScreenSpace(glm::vec3 A, glm::mat4 World)
{
    glm::vec4 Point = World * glm::vec4(A,1.0f);
    return {Point.x, Point.y, Point.z};
}

Rect3D fx_WorldSpace2ScreenSpace(Rect3D A, glm::mat4 World)
{
    return Rect3D({fx_WorldSpace2ScreenSpace(A.Min, World), fx_WorldSpace2ScreenSpace(A.Max, World)});
}

Poly3D fx_WorldSpace2ScreenSpace(Poly3D A, glm::mat4 World)
{
    Poly3D Polygon;
    for (auto x: A.Corners)
    {
        Polygon.Corners.push_back(fx_WorldSpace2ScreenSpace(x, World));
    }
    return Polygon;
}

glm::vec3 fx_ScreenSpace2WorldSpace(glm::vec2 A, glm::mat4 InvWorld)
{
    glm::vec4 Point = InvWorld * glm::vec4(A,1.0f,1.0f);
    return {Point.x, Point.y, Point.z};
}

float fx_SmoothDamp(float current, float target, float &currentVelocity, float smoothTime, float deltaTime, float maxSpeed)
{
    // Based on Game Programming Gems 4 Chapter 1.10
    smoothTime = std::max(0.0001F, smoothTime);
    float omega = 2.0f / smoothTime;

    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    float change = current - target;
    float originalTo = target;

    // Clamp maximum speed
    float maxChange = maxSpeed * smoothTime;
    change = std::clamp(change, -maxChange, maxChange);
    target = current - change;

    float temp = (currentVelocity + omega * change) * deltaTime;
    currentVelocity = (currentVelocity - omega * temp) * exp;
    float output = target + (change + temp) * exp;

    // Prevent overshooting
    if (originalTo - current > 0.0f == output > originalTo)
    {
        output = originalTo;
        currentVelocity = (output - originalTo) / deltaTime;
    }

    return output;
}

glm::vec2 fx_SmoothDamp(glm::vec2 currentPos, glm::vec2 target, glm::vec2 &currentVelocity, float smoothTime, float deltaTime, float maxSpeed)
{
    // Based on Game Programming Gems 4 Chapter 1.10
    smoothTime = std::max(0.0001F, smoothTime);
    float omega = 2.0f / smoothTime;

    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    glm::vec2 change = currentPos - target;
    glm::vec2 originalTo = target;

    // Clamp maximum speed
    float maxChange = maxSpeed * smoothTime;
    change = glm::clamp(change, -maxChange, maxChange);
    target = currentPos - change;

    glm::vec2 temp = (currentVelocity + omega * change) * deltaTime;
    currentVelocity = (currentVelocity - omega * temp) * exp;
    glm::vec2 output = target + (change + temp) * exp;

    // Prevent overshooting
    if ((originalTo.x - currentPos.x > 0.0f == output.x > originalTo.x) &&
        (originalTo.y - currentPos.y > 0.0f == output.y > originalTo.y))
    {
        output = originalTo;
        currentVelocity = (output - originalTo) / deltaTime;
    }

    return output;
}