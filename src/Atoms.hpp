#pragma once

#include "Object.hpp"

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <reactphysics3d/reactphysics3d.h>  

extern reactphysics3d::PhysicsWorld *PhysicWorld;

enum Elements
{
    H_ = 1,
    He_,
    Li_,
    Be_,
    B_,
    C_,
    N_,
    O_,
    F_,
    Ne_,
    Na_,
    Mg_,
    Al_,
    Si_,
    P_,
    S_,
    Cl_,
    Ar_,
    K_,
    Ca_
};

class Atom
{
private:
Elements m_Atom;
public:
Atom(Elements Elem);
~Atom(){}

    uint8_t m_Proton;
    uint8_t m_Weight;
    uint8_t m_Valence;
    std::vector<Atom*> m_Child;
};


class Molecule : public fx_Billboard
{
    private:
    std::vector<fx_BillboardCircle*> m_AtomObj;
    std::vector<fx_BillboardLine*> m_BondObj;
    std::vector<Atom*> m_Atoms;
    std::vector<reactphysics3d::Body*> m_Bodies;
public:
    Molecule(Atom* ParentAtom, glm::vec3 Pos);
    virtual ~Molecule(){}

    void Update();



};