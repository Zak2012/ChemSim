#pragma once

#include "Object.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>
#include <string>

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
    uint8_t m_Nucleon;
    Elements m_Atom;
    void* m_Body;
public:
    Atom(Elements Elem);
    ~Atom(){}

    uint8_t m_Proton;
    uint8_t m_Period;
    uint8_t m_Valence;
    std::vector<Atom*> m_Child;
};


class Molecule : public fx_Billboard
{
private:
    std::vector<fx_BillboardCircle*> m_AtomObj;
    std::vector<fx_BillboardLine*> m_BondObj;
    std::vector<Atom*> m_Atoms;
public:
    Molecule(Atom* ParentAtom);
    virtual ~Molecule(){}

    void Update();



};