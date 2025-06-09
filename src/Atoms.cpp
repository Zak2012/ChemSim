#include "Atoms.hpp"

#include <reactphysics3d/reactphysics3d.h>  
#include <iostream>

reactphysics3d::PhysicsWorld *PhysicWorld;

static const std::vector<glm::vec3> OrbitalTable[] = 
{
    {},
    {{1.0f,0.0f,0.0f}},
    {{1.0f,0.0f,0.0f}, {-1.0f,0.0f,0.0f}},
    {   
        {std::cos(glm::radians(0.0f)),std::sin(glm::radians(0.0f)),0.0f}, 
        {std::cos(glm::radians(120.0f)),std::sin(glm::radians(120.0f)),0.0f}, 
        {std::cos(glm::radians(240.0f)),std::sin(glm::radians(240.0f)),0.0f}
    },
    {   
        {1.0f,0.0f,0.0f}, 
        glm::rotateZ(glm::vec3(1.0f,0.0f,0.0f), glm::radians(109.5f)), 
        glm::rotateX(glm::rotateZ(glm::vec3(1.0f,0.0f,0.0f), glm::radians(109.5f)), glm::radians(120.0f)), 
        glm::rotateX(glm::rotateZ(glm::vec3(1.0f,0.0f,0.0f), glm::radians(109.5f)), glm::radians(240.0f)), 
    },
};

//https://sciencenotes.org/molecule-atom-colors-cpk-colors/
static const glm::vec4 ColourTable[] = 
{
    glm::vec4(255.0f,255.0f,255.0f,255.0f) / 255.0f, //H
    glm::vec4(255.0f,255.0f,255.0f,255.0f) / 255.0f, //He
    glm::vec4(217.0f,255.0f,255.0f,255.0f) / 255.0f, //Li
    glm::vec4(204.0f,128.0f,255.0f,255.0f) / 255.0f, //Be
    glm::vec4(149.0f,255.0f,000.0f,255.0f) / 255.0f, //B
    glm::vec4(255.0f,181.0f,181.0f,255.0f) / 255.0f, //C
    glm::vec4(144.0f,144.0f,144.0f,255.0f) / 255.0f, //N
    glm::vec4(048.0f,080.0f,248.0f,255.0f) / 255.0f, //O
    glm::vec4(255.0f,013.0f,013.0f,255.0f) / 255.0f, //F
    glm::vec4(144.0f,224.0f,080.0f,255.0f) / 255.0f, //Ne   
    glm::vec4(179.0f,227.0f,245.0f,255.0f) / 255.0f,
    glm::vec4(171.0f,092.0f,242.0f,255.0f) / 255.0f,
    glm::vec4(138.0f,255.0f,000.0f,255.0f) / 255.0f,
    glm::vec4(191.0f,166.0f,166.0f,255.0f) / 255.0f,
    glm::vec4(240.0f,200.0f,160.0f,255.0f) / 255.0f,
    glm::vec4(255.0f,255.0f,048.0f,255.0f) / 255.0f,
    glm::vec4(031.0f,240.0f,031.0f,255.0f) / 255.0f,
    glm::vec4(128.0f,209.0f,227.0f,255.0f) / 255.0f,
    glm::vec4(143.0f,064.0f,212.0f,255.0f) / 255.0f,
    glm::vec4(061.0f,255.0f,000.0f,255.0f) / 255.0f
};

float pm2screen = 1.0f/53.0f;

static const float AtomicRadius[] = 
{
    53.0f * pm2screen,
    31.0f * pm2screen,
    167.0f * pm2screen,
    112.0f * pm2screen,
    87.0f * pm2screen,
    67.0f * pm2screen,
    56.0f * pm2screen,
    48.0f * pm2screen,
    42.0f * pm2screen,
    38.0f * pm2screen,
    190.0f * pm2screen,
    145.0f * pm2screen,
    118.0f * pm2screen,
    111.0f * pm2screen,
    98.0f * pm2screen,
    88.0f * pm2screen,
    79.0f * pm2screen,
    71.0f * pm2screen,
    243.0f * pm2screen,
    194.0f * pm2screen
};

static const float AtomicWeight[] = 
{
    1.0f,
    4.0f,
    6.9f,
    9.0f,
    10.8f,
    12.0f,
    14.0f,
    16.0f,
    19.0f,
    20.2f,
    23.0f,
    24.3f,
    27.0f,
    28.1f,
    31.0f,
    32.1f,
    35.5f,
    40.0f,
    39.1f,
    40.1f
};

static const glm::vec4 BondColourTable[] = {
    glm::vec4(000.0f, 000.0f, 200.0f, 255.0f) / 255.0f,
    glm::vec4(000.0f, 200.0f, 000.0f, 255.0f) / 255.0f,
    glm::vec4(200.0f, 000.0f, 000.0f, 255.0f) / 255.0f,
};


// Atom::Atom(Elements Elem)
// {
//     m_Proton = Elem;
//     if (m_Proton <= 2)
//     {
//         m_Valence = m_Proton;
//     }
//     else if (m_Proton <= 10)
//     {
//         m_Valence = m_Proton - 2;
//     }
//     else if (m_Proton <= 18)
//     {
//         m_Valence = m_Proton - 10;
//     }
//     else if (m_Proton <= 36)
//     {
//         m_Valence = m_Proton - 18;
//     }

//     m_Weight = AtomicWeight[Elem];


// }

Molecule::Molecule(std::vector<std::pair<int, Elements>> Atoms, glm::vec3 Pos)
{
    // m_Atoms.push_back(ParentAtom);

    for (auto x : Atoms)
    {
        m_Atoms.push_back(x);
        // Initial position and orientation of the rigid body
        // reactphysics3d::Vector3 position(0.0, 0.0, 0.0);
        // reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
        // reactphysics3d::Transform transform(position, orientation);
        
        // Create a rigid body in the world
        // m_Bodies.push_back(PhysicWorld->createRigidBody(transform));
    }

    m_Position = Pos;

    // float Size = AtomicRadius[ParentAtom->m_Proton] * 2.0f;
    float Size = 1;
    fx_BillboardCircle *Central = new fx_BillboardCircle(m_Position, {Size,Size}, ColourTable[m_Atoms[0].second]);
    Central->SetOutline(0.1f);
    m_AtomObj.push_back(Central);
    m_Objects.push_back(Central);

    // reactphysics3d::Vector3 position(Pos.x, Pos.y, Pos.z);
    // reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion(m_Quat.x, m_Quat.y, m_Quat.z, m_Quat.w);
    // reactphysics3d::Transform transform(position, orientation);
        
    // Create a rigid body in the world
    // m_Bodies.push_back(PhysicWorld->createRigidBody(transform));

    for (unsigned int i = 0; i < m_Atoms.size(); i++)
    {
        if (m_Atoms[i].second == Elements::None)
        {
            continue;
        }
        if (m_Atoms[i].first == 0)
        {
            continue;
        }
        // float BondLength = (AtomicRadius[ParentAtom->m_Proton] + AtomicRadius[ParentAtom->m_Child[i]->m_Proton]) * 0.5f;
        // float ASize = AtomicRadius[ParentAtom->m_Child[i]->m_Proton] * 2.0f;
        float ASize = 1;
        // glm::vec3 ChildPos = glm::normalize(OrbitalTable[m_Atoms.size() - 1][i-1] * m_Quat) * BondLength;
        fx_BillboardCircle *Cir = new fx_BillboardCircle(Pos, {ASize,ASize}, ColourTable[m_Atoms[i].second]);
        fx_BillboardLine *Lin = new fx_BillboardLine(Pos, Pos, 0.2f * m_Atoms[i].first);
        // Lin->SetColour(BondColourTable[m_Atoms[i].first-1]);
        Cir->SetOutline(0.1f);
        m_Objects.push_back(Cir);
        m_Objects.push_back(Lin);
        m_AtomObj.push_back(Cir);
        m_BondObj.push_back(Lin);
    }
}

void Molecule::Update()
{
    m_FlagUpdateMesh = m_FlagUpdateMesh || m_FlagUpdateObject;
    if (!m_FlagUpdateMesh)
    {
        return;
    }

    for (unsigned int i = 0; i < m_Atoms.size(); i++)
    {
        if (m_Atoms[i].second == Elements::None)
        {
            continue;
        }
        if (m_Atoms[i].first == 0)
        {
        m_AtomObj[i]->SetAnchor({0.5,0.5,0});

            m_AtomObj[i]->SetPosition(m_Position);
            m_AtomObj[i]->SetColour(ColourTable[m_Atoms[i].second]);
            continue;
        }
        float BondLength = 1.2;
        // float ASize = AtomicRadius[ParentAtom->m_Child[i]->m_Proton] * 2.0f;
        glm::vec3 ChildPos = glm::normalize(OrbitalTable[m_Atoms.size() - 1][i-1] * m_Quat) * BondLength;

        m_AtomObj[i]->SetAnchor({0.5,0.5,0});
        m_AtomObj[i]->SetPosition(m_Position+ChildPos);
        m_AtomObj[i]->SetColour(ColourTable[m_Atoms[i].second]);

        m_BondObj[i-1]->SetStart(m_Position + (ChildPos * 0.4f));
        m_BondObj[i-1]->SetEnd(m_Position + (ChildPos * 0.6f));
        m_BondObj[i-1]->SetHeight(0.2f * m_Atoms[i].first);
        m_BondObj[i-1]->SetColour(BondColourTable[m_Atoms[i].first-1]);


    }

    
    for (auto x : m_AtomObj)
    {
        x->SetCameraPos(m_CameraPos + glm::vec3(0,0,1));
        x->SetCameraUp(m_CameraUp);
        x->SetCameraPos(m_CameraPos - glm::vec3(0,0,1));
    }

    for (auto x : m_BondObj)
    {
        x->SetCameraPos(m_CameraPos + glm::vec3(0,0,1));
        x->SetCameraUp(m_CameraUp);
        x->SetCameraPos(m_CameraPos - glm::vec3(0,0,1));
    }
}

void Molecule::SetQuat(glm::quat Quat)
{
    m_FlagUpdateMesh |= m_Quat!=Quat; 
    m_Quat = Quat;
    // reactphysics3d::Vector3 position(Pos.x, Pos.y, Pos.z);
    // reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion(m_Quat.x, m_Quat.y, m_Quat.z, m_Quat.w);
    // reactphysics3d::Transform transform(position, orientation);
}