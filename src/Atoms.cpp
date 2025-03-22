#include "Atoms.hpp"

#include <reactphysics3d/reactphysics3d.h>  
#include <iostream>

inline float GetAtomSize(int n, int v)
{

    // return 2.0f * 0.529f * std::pow((float)n,2.0f)/(float)z;
    int Orbital = 0;
    for (int l = 1; l < n; l++)
    {
        int m = (2 * l) + 1;
        Orbital += m; 
    }
    Orbital *= 2;

    // return (float)n * std::exp(-(float)v/(float)Orbital); // space filling model
    return (std::exp2(std::exp(2.0f) * -(float)v/(float)Orbital))+ ((float)(n)*0.75f); // space filling model
    // bond leght = 0.5*(radii + radii)
}

static const std::vector<std::vector<glm::vec3>> OrbitalTable = {
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
static const std::vector<glm::vec4> ColourTable = {
    glm::vec4(255.0f,255.0f,255.0f,255.0f) / 255.0f,
    glm::vec4(255.0f,255.0f,255.0f,255.0f) / 255.0f,
    glm::vec4(217.0f,255.0f,255.0f,255.0f) / 255.0f,
    glm::vec4(204.0f,128.0f,255.0f,255.0f) / 255.0f,
    glm::vec4(149.0f,255.0f,000.0f,255.0f) / 255.0f,
    glm::vec4(255.0f,181.0f,181.0f,255.0f) / 255.0f,
    glm::vec4(144.0f,144.0f,144.0f,255.0f) / 255.0f,
    glm::vec4(048.0f,080.0f,248.0f,255.0f) / 255.0f,
    glm::vec4(255.0f,013.0f,013.0f,255.0f) / 255.0f,
    glm::vec4(144.0f,224.0f,080.0f,255.0f) / 255.0f,
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



Atom::Atom(Elements Elem)
{
    m_Proton = Elem;
    if (m_Proton <= 2)
    {
        m_Valence = m_Proton;
        m_Period = 1;
    }
    else if (m_Proton <= 10)
    {
        m_Valence = m_Proton - 2;
        m_Period = 2;
    }
    else if (m_Proton <= 18)
    {
        m_Valence = m_Proton - 10;
        m_Period = 3;
    }
    else if (m_Proton <= 36)
    {
        m_Valence = m_Proton - 18;
        m_Period = 4;
    }


}

Molecule::Molecule(Atom* ParentAtom)
{
    m_Atoms.push_back(ParentAtom);

    for (auto x : ParentAtom->m_Child)
    {
        m_Atoms.push_back(x);
    }

    float Size = GetAtomSize(ParentAtom->m_Period, ParentAtom->m_Valence);
    fx_BillboardCircle *Central = new fx_BillboardCircle({0.0f,0.0f,0.0f}, {Size,Size}, ColourTable[ParentAtom->m_Proton]);
    m_AtomObj.push_back(Central);
    m_Objects.push_back(Central);

    float BondLength = (GetAtomSize(ParentAtom->m_Period, ParentAtom->m_Valence) + GetAtomSize(ParentAtom->m_Child[0]->m_Period, ParentAtom->m_Child[0]->m_Valence)) * 0.5f * 0.6;
    for (int i = 0; i < ParentAtom->m_Child.size(); i++)
    {
        float ASize = GetAtomSize(ParentAtom->m_Child[i]->m_Period, ParentAtom->m_Child[i]->m_Valence);
        glm::vec3 Pos = OrbitalTable[ParentAtom->m_Child.size()][i] * BondLength;
        fx_BillboardCircle *Cir = new fx_BillboardCircle(Pos, {ASize,ASize}, ColourTable[ParentAtom->m_Child[i]->m_Proton]);
        // fx_BillboardLine *Lin = new fx_BillboardLine(Pos, {0.0f,0.0f,0.0f}, 0.1f);
        m_Objects.push_back(Cir);
        // m_Objects.push_back(Lin);
        m_AtomObj.push_back(Cir);
        // m_BondObj.push_back(Lin);
    }
}

void Molecule::Update()
{
    m_FlagUpdateMesh = m_FlagUpdateMesh || m_FlagUpdateObject;
    if (!m_FlagUpdateMesh)
    {
        return;
    }
    
    for (auto x : m_AtomObj)
    {
        x->SetCameraPos(m_CameraPos);
        x->SetCameraUp(m_CameraUp);
    }

    for (auto x : m_BondObj)
    {
        x->SetCameraPos(m_CameraPos);
        x->SetCameraUp(m_CameraUp);
    }
}