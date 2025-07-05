#pragma once

#include "Object.hpp"

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>


// extern reactphysics3d::PhysicsWorld *PhysicWorld;
extern btDiscreteDynamicsWorld* dynamicsWorld;

enum Elements
{
    None = 0,
    H_,
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

inline glm::vec3 v3bt2glm(btVector3 v)
{
    return {v.getX(), v.getY(), v.getZ()};
}

inline glm::vec4 v4bt2glm(btVector4 v)
{
    return {v.getX(), v.getY(), v.getZ(), v.getW()};
}

inline glm::quat qtbt2glm(btQuaternion v)
{
    return {v.getX(), v.getY(), v.getZ(), v.getW()};
}

inline btVector3 v3glm2bt(glm::vec3 v)
{
    return btVector3(v.x, v.y, v.z);
}

inline btVector4 v4glm2bt(glm::vec4 v)
{
    return btVector4(v.x, v.y, v.z, v.w);
}

inline btQuaternion qtglm2bt(glm::quat v)
{
    return btQuaternion(v.x, v.y, v.z, v.w);
}

// class Atom : public fx_Billboard
// {
// private:
//     Elements m_Atom;
//     fx_BillboardCircle* m_Atomobj;
//     std::vector<fx_BillboardLine*> m_BondObj;
// public:
// Atom(Elements Elem);
// ~Atom(){}

//     uint8_t m_Proton;
//     uint8_t m_Weight;
//     uint8_t m_Valence;
//     std::vector<Atom*> m_Child;
// };




class Molecule : public fx_Billboard
{
private:
    std::vector<fx_BillboardCircle*> m_AtomObj;
    std::vector<fx_BillboardLine*> m_BondObj;
    std::vector<std::pair<int, Elements>> m_Atoms;
    std::vector<btRigidBody*> m_Bodies;
    bool m_TransferFlag = false;
    std::vector<btGeneric6DofSpringConstraint *> m_Springs;
public:
    Molecule(std::vector<std::pair<int, Elements>> Atoms, glm::vec3 Pos);
    Molecule(std::vector<std::pair<int, Elements>> Atoms, std::vector<btRigidBody*> Bodies);
    virtual ~Molecule();

    void Physic();
    void Update();
    void SetQuat(glm::quat Quat);
    void SetTransfer(bool Transfer) {m_TransferFlag = Transfer;}

    std::vector<btRigidBody*> GetBodies() {return m_Bodies;}
    std::vector<std::pair<int, Elements>> GetAtoms() {return m_Atoms;}
    bool GetTransfer() {return m_TransferFlag;}



};