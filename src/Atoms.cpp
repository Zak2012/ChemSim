#include "Atoms.hpp"

#include <iostream>

// reactphysics3d::PhysicsWorld *PhysicWorld;

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

static const float Damping = 0.05f;
static const float Stiff = 1.0f;

static btCollisionShape* colShape = new btSphereShape(btScalar(.5));

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
    m_Atoms = Atoms;
    m_Position = Pos;

    float Size = 1;
    fx_BillboardCircle *Central = new fx_BillboardCircle(m_Position, {Size,Size}, ColourTable[m_Atoms[0].second]);
    Central->SetOutline(0.1f);
    Central->SetAnchor({0.5,0.5,0});
    Central->SetPosition(m_Position);
    Central->SetColour(ColourTable[m_Atoms[0].second]);
    
    m_AtomObj.push_back(Central);
    m_Objects.push_back(Central);

    
    {
        btCollisionShape* colShape = new btSphereShape(btScalar(.5));

        btTransform startTransform;
        startTransform.setIdentity();
        startTransform.setOrigin(v3glm2bt(m_Position));

        btScalar mass(1.f);
        btVector3 localInertia(0, 0, 0);
        colShape->calculateLocalInertia(mass, localInertia);

        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
        btRigidBody* Atombody = new btRigidBody(rbInfo);
        Atombody->setUserPointer((void*)this);
        Atombody->setFriction(0.0f);
        Atombody->setRollingFriction(0.0f);
        Atombody->setSpinningFriction(0.0f);
        Atombody->setHitFraction(0.0f);
        Atombody->setActivationState(DISABLE_DEACTIVATION);
        
        m_Bodies.push_back(Atombody);
        dynamicsWorld->addRigidBody(Atombody);
    }
    
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
        float BondLength = 1.2;
        float ASize = 1;
        glm::vec3 Direction = glm::normalize(OrbitalTable[m_Atoms.size() - 1][i-1] * m_Quat);
        glm::vec3 ChildPos = Direction * BondLength;
        fx_BillboardCircle *Cir = new fx_BillboardCircle(Pos, {ASize,ASize});
        fx_BillboardLine *Lin = new fx_BillboardLine(Pos, Pos, 0.2f * m_Atoms[i].first);

        Cir->SetAnchor({0.5,0.5,0});
        Cir->SetPosition(m_Position+ChildPos);
        Cir->SetColour(ColourTable[m_Atoms[i].second]);
        Cir->SetOutline(0.1f);

        Lin->SetStart(m_Position + (ChildPos * 0.4f));
        Lin->SetEnd(m_Position + (ChildPos * 0.6f));
        Lin->SetHeight(0.2f * m_Atoms[i].first);
        Lin->SetColour(BondColourTable[m_Atoms[i].first-1]);


        {
            btTransform startTransform;
            startTransform.setIdentity();
            startTransform.setOrigin(v3glm2bt(Cir->GetPosition()));

            btScalar mass(1.f);
            btVector3 localInertia(0, 0, 0);
            colShape->calculateLocalInertia(mass, localInertia);

            btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
            btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
            btRigidBody* Atombody = new btRigidBody(rbInfo);
            Atombody->setUserPointer((void*)this);
            Atombody->setFriction(0.0f);
            Atombody->setRollingFriction(0.0f);
            Atombody->setSpinningFriction(0.0f);
            Atombody->setHitFraction(0.0f);
            Atombody->setActivationState(DISABLE_DEACTIVATION);
            
            m_Bodies.push_back(Atombody);
            dynamicsWorld->addRigidBody(Atombody);

            btGeneric6DofSpringConstraint *spring = new btGeneric6DofSpringConstraint(
                *m_Bodies[0], *Atombody,
                btTransform(btQuaternion::getIdentity(), v3glm2bt(Direction)),
                btTransform(btQuaternion::getIdentity(), v3glm2bt(-Direction)),
                true
            );
            for (int i = 0; i < 3; i++)
            {
                spring->setLimit(i, 1.0f, 0.0f);
                spring->enableSpring(i,  true);
                spring->setStiffness(i, Stiff);
                spring->setDamping  (i,  Damping);
            }
            spring->setEquilibriumPoint();
            m_Springs.push_back(spring);

            dynamicsWorld->addConstraint(spring);
        }

        m_Objects.push_back(Cir);
        m_Objects.push_back(Lin);
        m_AtomObj.push_back(Cir);
        m_BondObj.push_back(Lin);
    }
}

Molecule::Molecule(std::vector<std::pair<int, Elements>> Atoms, std::vector<btRigidBody*> Bodies)
{
    m_Atoms = Atoms;
    m_Bodies = Bodies;
    // m_Position = v3bt2glm(Bodies[0]->getWorldTransform().getOrigin());
    // m_Quat = qtbt2glm(Bodies[0]->getWorldTransform().getRotation());
    btTransform trans1;
    btTransform trans2;
    Bodies[0]->getMotionState()->getWorldTransform(trans1);
    // Bodies[1]->getMotionState()->getWorldTransform(trans2);

    m_Position = v3bt2glm(trans1.getOrigin());
    m_Quat = qtbt2glm(trans1.getRotation());

    float Size = 1;
    fx_BillboardCircle *Central = new fx_BillboardCircle(m_Position, {Size,Size}, ColourTable[m_Atoms[0].second]);
    Central->SetOutline(0.1f);
    Central->SetAnchor({0.5,0.5,0});
    Central->SetPosition(m_Position);
    Central->SetColour(ColourTable[m_Atoms[0].second]);
    
    m_AtomObj.push_back(Central);
    m_Objects.push_back(Central);

    
    for (unsigned int i = 0; i < m_Atoms.size(); i++)
    {
        m_Bodies[i]->setUserPointer((void*)this);
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
        float BondLength = 1.2;
        float ASize = 1;
        glm::vec3 Direction = glm::normalize(OrbitalTable[m_Atoms.size() - 1][i-1] * m_Quat);
        glm::vec3 ChildPos = Direction * BondLength;
        fx_BillboardCircle *Cir = new fx_BillboardCircle(m_Position, {ASize,ASize});
        fx_BillboardLine *Lin = new fx_BillboardLine(m_Position, m_Position, 0.2f * m_Atoms[i].first);

        Cir->SetAnchor({0.5,0.5,0});
        Cir->SetPosition(m_Position+ChildPos);
        Cir->SetColour(ColourTable[m_Atoms[i].second]);
        Cir->SetOutline(0.1f);

        Lin->SetStart(m_Position + (ChildPos * 0.4f));
        Lin->SetEnd(m_Position + (ChildPos * 0.6f));
        Lin->SetHeight(0.2f * m_Atoms[i].first);
        Lin->SetColour(BondColourTable[m_Atoms[i].first-1]);

        m_Objects.push_back(Cir);
        m_Objects.push_back(Lin);
        m_AtomObj.push_back(Cir);
        m_BondObj.push_back(Lin);

        btTransform realPos;
        m_Bodies[i]->getMotionState()->getWorldTransform(realPos);

        btTransform fakePos = realPos;
        fakePos.setIdentity();
        fakePos.setOrigin(v3glm2bt(ChildPos));

        m_Bodies[i]->getMotionState()->setWorldTransform(fakePos);
        

        btGeneric6DofSpringConstraint *spring = new btGeneric6DofSpringConstraint(
            *m_Bodies[0], *m_Bodies[i],
            btTransform(btQuaternion::getIdentity(), v3glm2bt(Direction)),
            btTransform(btQuaternion::getIdentity(), v3glm2bt(-Direction)),
            true
        );
        for (int i = 0; i < 3; i++)
        {
            spring->setLimit(i, 1.0f, 0.0f);
            spring->enableSpring(i,  true);
            spring->setStiffness(i, Stiff);
            spring->setDamping  (i,  Damping);
        }
        spring->setEquilibriumPoint();

        dynamicsWorld->addConstraint(spring);
        m_Springs.push_back(spring);
        
        m_Bodies[i]->getMotionState()->setWorldTransform(realPos);


        // m_Bodies[i]->setWorldTransform(realPos);
    }
}

Molecule::~Molecule() 
{
    for (auto x : m_AtomObj)
    {
        delete x;
    }

    for (auto x : m_BondObj)
    {
        delete x;
    }

    for (auto x : m_Springs)
    {
        dynamicsWorld->removeConstraint(x);
        delete x;
    }

    if (!m_TransferFlag)
    {
        for (auto x : m_Bodies)
        {
            if (x->getMotionState())
            {
                delete x->getMotionState();
            }
            dynamicsWorld->removeRigidBody(x);
            delete x;
        }
    }
}


void Molecule::Physic()
{
    for (unsigned int i = 0; i < m_Atoms.size(); i++)
    {
        btTransform trans;
        m_Bodies[i]->getMotionState()->getWorldTransform(trans);
        
        m_AtomObj[i]->SetPosition(v3bt2glm(trans.getOrigin()));
        m_AtomObj[i]->SetQuat(qtbt2glm(trans.getRotation()));
        m_FlagUpdateMesh = true;
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
            m_AtomObj[i]->SetColour(ColourTable[m_Atoms[i].second]);
            continue;
        }
        glm::vec3 Dir = glm::normalize(m_AtomObj[i]->GetPosition() - m_AtomObj[0]->GetPosition());
        float lenght = glm::distance2(m_AtomObj[0]->GetPosition(), m_AtomObj[i]->GetPosition());

        m_AtomObj[i]->SetColour(ColourTable[m_Atoms[i].second]);

        m_BondObj[i-1]->SetStart(m_AtomObj[0]->GetPosition() + (Dir * (0.4f/lenght)));
        m_BondObj[i-1]->SetEnd(m_AtomObj[i]->GetPosition() - (Dir * (0.4f/lenght)));
        m_BondObj[i-1]->SetHeight(0.2f * m_Atoms[i].first);
        m_BondObj[i-1]->SetColour(BondColourTable[m_Atoms[i].first-1]);


    }

    
    for (auto x : m_AtomObj)
    {
        // x->SetCameraPos(m_CameraPos + glm::vec3(0,0,1));
        x->SetCameraUp(m_CameraUp);
        x->SetCameraPos(m_CameraPos);
    }

    for (auto x : m_BondObj)
    {
        // x->SetCameraPos(m_CameraPos + glm::vec3(0,0,1));
        x->SetCameraUp(m_CameraUp);
        x->SetCameraPos(m_CameraPos);
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


void Molecule::SetVelocity(glm::vec3 Vel)
{
    for (auto x : m_Bodies)
    {
        x->setLinearVelocity(v3glm2bt(Vel));
    }
}
