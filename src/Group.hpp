#pragma once

#include <vector>
#include <map>
#include <set>

#include "Object.hpp"
#include "Widget.hpp"
#include "Billboard.hpp"

// TODO: also handle image and texture
class fx_Group
{
protected:
    std::map<fx_BasicType,std::set<fx_Basic*>> m_Basics;
    std::map<fx_BasicType, fx_Mesh> m_Meshes;
    std::map<fx_BasicType, fx_Buffer*> m_Buffers;
    std::map<fx_BasicType, fx_Program*> m_Programs;
    std::set<fx_Objects*> m_Objects;
    fx_BillboardHandler *m_BHandler;
    fx_WidgetHandler *m_WHandler;
    fx_TextHandler *m_THandler;
    bool m_FlagUpdateMesh = false;
    bool m_FlagUpdateObject = false;
    bool m_FlagUpdateBuffer = false;
    // uint32_t m_ObjCount = 0;
    void GenerateMesh();
    void CreateBuffer();
    void UpdateDFS(std::set<fx_Objects*> Objects);
    void CombineBasicDFS(std::map<fx_BasicType,std::set<fx_Basic*>> &Basics, std::set<fx_Objects*> Objects);
    
public:
    fx_Texture *m_TextureUnit = NULL;
    fx_Framebuffer *m_FrameBuffer = NULL;
    fx_Camera *m_Camera = NULL;
    fx_Group(std::map<fx_BasicType, fx_Program*> Programs, fx_Texture *TextureUnit = NULL);
    ~fx_Group(){delete m_BHandler; delete m_WHandler; delete m_THandler; for (auto x : m_Buffers){delete x.second;}}
    // ~fx_Group(std::vector<fx_Program*> Programs, fx_Texture *TextureUnit);

    fx_BillboardHandler* GetBillboardHandler(){return m_BHandler;}
    fx_WidgetHandler* GetWidgetHandler(){return m_WHandler;}
    fx_TextHandler* GetTextHandler(){return m_THandler;}

    void SetMousePos(glm::vec2 ScreenMousePos) {if (m_Camera){m_WHandler->SetMousePos(m_Camera->Screen2World(ScreenMousePos));}}

    void Update();
    void Draw();

    void AddObject(fx_Objects *Obj){m_Objects.insert(Obj); Obj->m_Group = this; m_FlagUpdateObject = true;}
    void AddObject(fx_Billboard *Obj){AddObject((fx_Objects*)Obj); m_BHandler->AddObject(Obj);}
    void AddObject(fx_Widget *Obj){AddObject((fx_Objects*)Obj); m_WHandler->AddObject(Obj);}
    void AddObject(fx_TextBase *Obj){AddObject((fx_Objects*)Obj); m_THandler->AddObject(Obj);}

    void DelObject(fx_Objects *Obj){m_Objects.erase(Obj); Obj->m_Group = nullptr; m_FlagUpdateObject = true;}
    void DelObject(fx_Billboard *Obj){DelObject((fx_Objects*)Obj); m_BHandler->DelObject(Obj);}
    void DelObject(fx_Widget *Obj){DelObject((fx_Objects*)Obj); m_WHandler->DelObject(Obj);}
    void DelObject(fx_TextBase *Obj){DelObject((fx_Objects*)Obj); m_THandler->DelObject(Obj);}
};
