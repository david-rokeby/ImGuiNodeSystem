//
//  cNodeManager.h
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-09.
//

#ifndef cNodeManager_h
#define cNodeManager_h

#include <string>
#include "ofxImGui.h"
#include "imnodes_internal.h"
#include "ofVectorMath.h"
#include "implot.h"
#include "my_widgets.hpp"
#include <map>
#include "cGUIProperty.h"
#include "cNodeInput.h"
#include "cNodeOutput.h"
#include "ofxJSON.h"

#include "cUniqueObject.hpp"

class cNodeMemberSpecifier
{
public:
    int node_uuid;
    std::string name;
};

class cNodeBuilderBase
{
public:
  virtual cNode *Build(std::vector<std::string> args){}
};

template <class T>
class cNodeBuilder : public cNodeBuilderBase
{
public:
    virtual cNode *Build(std::vector<std::string> args)
    {
        return new T(args);
    }
};

class cNodeFactory
{
public:
    template <class T>
    void Register(std::string name)
    {
        m_instantiators[name] = new cNodeBuilder<T>();
    }
    
    cNode *Build(std::string name, std::vector<std::string> args)
    {
        if( m_instantiators.count(name) > 0)
            return m_instantiators[name] -> Build(args);
        return nullptr;
    }

private:
    std::map<std::string, cNodeBuilderBase *> m_instantiators;
};


class cNodeContext
{
public:
    ImNodesEditorContext* nodeEditorContext = nullptr; //vector of contexts?
    ImNodesContext* nodeContext = nullptr; //vector of contexts?
    std::vector<cUniqueObject>objects; //vector of vectors?
    ImGuiWindow *contextWindow = nullptr;
    bool isFront = false;
    std::string name = "Node Editor";
    bool open = true;
    
    cNodeContext(void)
    {
        ImNodes::SetCurrentContext(nullptr);
        nodeContext = ImNodes::CreateContext();
        nodeEditorContext = ImNodes::EditorContextCreate();
        ImPlot::GetStyle().AntiAliasedLines = true;
    //    ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);
  
        ImNodesIO& io = ImNodes::GetIO();
        io.LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;
    }
};

class cNodeManager
{
public:
    // PER WINDOW???
    std::vector<cNodeContext> contexts;
    int currentContext = -1;
    
//    cNodeContext* backNodeContext; //fake to have additional context
    ImPlotContext* plotContext = nullptr; // vector of contexts?
//    std::vector<cUniqueObject>objects; //vector of vectors?
    std::vector<cUniqueObject> *currentObjects = nullptr;

    // GLOBAL
    cNodeFactory factory;
    
    cNode *placeHolder = nullptr;
    std::string newNodeName = "";
    ImVec2 newNodePos;
    
    bool showMiniMap = true;
    
    cNodeManager(void);
    ~cNodeManager(void);
    int CreateNewContext(void);

    void RegisterNodeFactories(void);
    cNode *CreateNamedNode(std::string name);
    cNode *CreateNamedNode(std::vector<std::string> args);

    cNodeInput *GetInput(int id);
    cNodePropertyInputStub *GetPropertyInput(int id);
    cNodeOutput *GetOutput(int id);
    cNode *GetNode(int id);
    cNodeLink *GetLink(int id);
    int RegisterInput(cNodeInput *in);
    int RegisterPropertyInput(cNodePropertyInputStub *in);
    int RegisterProperty(cGUIPropertyStub *prop);
    int RegisterOutput(cNodeOutput *out);
    int RegisterOption(cGUIPropertyStub *prop);
    int RegisterNode(cNode *node);
    int RegisterLink(int from, int to);
    
    void PrepareNewNode(char *name, cNode *inPlaceHolder);
    void CreateNewNode();
    void HandleKeys(void);
    void HandleMouse(void);
    void ToggleShowOptionsForSelectedNodes(void);

    void DeleteSelectedNodes();
    
    void DeleteInput(cNodeInput *input);
    
//    int GetLoadedOutputID(int loadedNodeID, std::string outputName );
 //   int GetLoadedInputID(int loadedNodeID, std::string inputName );

    int FindEmptyObject(void);
    int AddObject(cUniqueObject obj);

    void RemoveInput(int id)
    {
        if(currentObjects != nullptr)
        {
            if(id < currentObjects->size())
            {
                cUniqueObject obj = (*currentObjects)[id];
                if(obj.type == t_Input)
                {
                    (*currentObjects)[id].Reset();
                }
            }
        }
    }

    void DeletePropertyInput(cNodePropertyInputStub *input);
    
    void RemovePropertyInput(int id)
    {
        if(currentObjects != nullptr)
        {
            if(id < currentObjects->size())
            {
                cUniqueObject obj = (*currentObjects)[id];
                if(obj.type == t_InputProperty)
                {
                    (*currentObjects)[id].Reset();
                }
            }
        }
    }

    void DeleteProperty(cGUIPropertyStub *prop);
    
    void RemoveProperty(int id)
    {
        if(currentObjects != nullptr)
        {
            if(id < currentObjects->size())
            {
                cUniqueObject obj = (*currentObjects)[id];
                if(obj.type == t_Property)
                {
                    (*currentObjects)[id].Reset();
                }
            }
        }
    }

    void DeleteOutput(cNodeOutput *output);
    
    void RemoveOutput(int id)
    {
        if(currentObjects != nullptr)
        {
            if(id < currentObjects->size())
            {
                cUniqueObject obj = (*currentObjects)[id];
                if(obj.type == t_Output)
                {
                    (*currentObjects)[id].Reset();
                }
            }
        }
    }

    void DeleteNode(cNode *node);
    
    void RemoveNode(int id)
    {
        if(currentObjects != nullptr)
        {
            if(id < currentObjects->size())
            {
                cUniqueObject obj = (*currentObjects)[id];
                if(obj.type == t_Node)
                {
                    (*currentObjects)[id].Reset();
                }
            }
        }
    }

    void DeleteLink(int id);
    
    void RemoveLink(int id)
    {
        if(currentObjects != nullptr)
        {
            if(id < currentObjects->size())
            {
                cUniqueObject obj = (*currentObjects)[id];
                if(obj.type == t_Link)
                {
                    (*currentObjects)[id].Reset();
                }
            }
        }
    }

    bool SetContext(int which);
    std::vector<cUniqueObject>* GetCurrentObjects(void)
    {
        return currentObjects;
    }

    void Update(void);
    void DrawAll(ImGuiWindow *toBeFocussed);

    void Draw(void);
    bool Save(void);
    bool SaveTo(std::string path);
    bool SaveTo(Json::Value& container);
    bool Load(void);
    bool LoadFrom(std::string path);
    bool LoadFrom(const Json::Value& container, bool select = false);
    void DuplicateSelectedNodes(void);
    
    bool SaveLink(Json::Value& linkContainer, cNodeLink *link, std::vector<int>*selectedNodes);

    cNodeMemberSpecifier GetInputSpecifier(int input_uuid);
    
    cNodeMemberSpecifier GetOutputSpecifier(int output_uuid);
};


#endif /* cNodeManager_h */


