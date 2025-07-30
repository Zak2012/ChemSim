#include "Billboard.hpp"


void fx_BillboardCircle::Update()
{
    m_Object->SetQuat(glm::quatLookAt(glm::normalize(m_Object->GetPosition() - m_CameraPos), m_CameraUp));

    m_Object->SetDepth(GetCube().x / 2);

    m_Object->SetPosition(m_Position);
    m_Object->SetColour(m_Colour);
    m_Object->SetCube(m_Cube);
}

void fx_BillboardLine::Update()
{
    glm::vec3 MidPoint = (m_Start + m_End)/2.0f;
    
    glm::vec3 CamObjNormal = glm::normalize(glm::cross(m_CameraPos - m_Start, m_CameraPos - m_End));
    
    glm::vec3 LineVec = m_End-m_Start;
    
    glm::vec3 Front;
    Front = glm::normalize(glm::cross(CamObjNormal, LineVec));
    
    if(glm::dot(Front, glm::normalize(m_CameraPos - MidPoint)) >= std::cos(glm::pi<float>()/2.0f))
    {
        Front = -Front;
    }
    
    m_Object->SetDepth((GetCube().x / 2) * std::abs(Front.z));
    
    
    m_Object->SetPosition(MidPoint);
    m_Object->SetAnchor({0.5,0.5,0.0});
    m_Object->SetCube({glm::distance(m_Start,m_End),m_Height,m_Object->GetCube().z});

    m_Object->SetColour(m_Colour);

    // std::cout << Front.x << "," << Front.y << "," << Front.z << "," << glm::dot(Front, glm::normalize(m_CameraPos - m_Start)) << "\n";
    m_Object->SetQuat(glm::quatLookAt(Front, CamObjNormal));
}
