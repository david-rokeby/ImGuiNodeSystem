//
//  cNode.cpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-03-14.
//

#include "cNodeManager.h"
#include "cNode.hpp"
#include "cOSCNode.hpp"
#include "cNodeInstances.hpp"
#include "ofSystemUtils.h"
//#include "my_widgets.hpp"

cNodeManager *cNode::manager = nullptr;
cNodeManager *cNodeInput::manager = nullptr;
cNodeManager *cNodePropertyInputStub::manager = nullptr;
cNodeManager *cNodeOutput::manager = nullptr;
cNodeManager *cNodeLink::manager = nullptr;
std::map<std::string, ImVec4> cBezierNode::presets;
std::vector<std::string> cBezierNode::presetNames;


cNodeManager::cNodeManager(void)
{
    plotContext = ImPlot::CreateContext();
    ImPlot::GetStyle().AntiAliasedLines = true;
//    ImNodes::PushAttributeFlag(ImNodesAttributeFlags_EnableLinkDetachWithDragClick);

//    backNodeContext = new cNodeContext;
//    contexts.push_back(*backNodeContext);

    RegisterNodeFactories();
    
    cNode::manager = this;
    cNodeInput::manager = this;
    cNodePropertyInputStub::manager = this;
    cNodeOutput::manager = this;
    cNodeLink::manager = this;

}

cNodeManager::~cNodeManager(void)
{
    ImNodes::DestroyContext();
    ImPlot::DestroyContext();
}

int cNodeManager::CreateNewContext(void)
{
    cNodeContext terminalContext;
    contexts.push_back(terminalContext);
    SetContext( contexts.size() - 1 );
    contexts[currentContext].name += (" " + std::to_string(currentContext));
    return currentContext;
}

bool cNodeManager::SetContext(int which)
{
    if( which < contexts.size())
    {
        ImNodes::SetCurrentContext(contexts[which].nodeContext);
        ImNodes::EditorContextSet(contexts[which].nodeEditorContext);
        currentContext = which;
        currentObjects = &contexts[currentContext].objects;
        if(currentObjects)
            return true;
    }
    return false;
}

void cNodeManager::PrepareNewNode(char *name, cNode *inPlaceHolder)
{
    newNodePos = ImNodes::GetNodeScreenSpacePos(inPlaceHolder -> uuid);
    newNodeName = std::string(name);
    placeHolder = inPlaceHolder;
}

void cNodeManager::RegisterNodeFactories(void)
{
    factory.Register<cArithmeticNode>("+");
    factory.Register<cArithmeticNode>("-");
    factory.Register<cArithmeticNode>("*");
    factory.Register<cArithmeticNode>("/");
    factory.Register<cArithmeticNode>("//");
    factory.Register<cArithmeticNode>("!/");
    factory.Register<cArithmeticNode>("!-");
    factory.Register<cArithmeticNode>("!/");
    factory.Register<cArithmeticNode>("mod");
    factory.Register<cValueNode>("float");
    factory.Register<cValueNode>("int");
    factory.Register<cValueNode>("bool");
    factory.Register<cValueNode>("toggle");
    factory.Register<cValueNode>("slider");
    factory.Register<cValueNode>("string");
    factory.Register<cValueNode>("text");
    factory.Register<cValueNode>("message");
    factory.Register<cMetroNode>("metro");
    factory.Register<cCounterNode>("counter");
    factory.Register<cPlotNode>("plot");
    factory.Register<cSignalNode>("signal");
    factory.Register<cMouseNode>("mouse");
    factory.Register<cTrigNode>("sin");
    factory.Register<cTrigNode>("cos");
    factory.Register<cTrigNode>("tan");
    factory.Register<cTrigNode>("asin");
    factory.Register<cTrigNode>("acos");
    factory.Register<cTrigNode>("atan");
    factory.Register<cMathNode>("sqrt");
    factory.Register<cMathNode>("inverse");
    factory.Register<cMathNode>("log10");
    factory.Register<cMathNode>("log2");
    factory.Register<cMathNode>("exp");
    factory.Register<cFilterNode>("filter");
    factory.Register<cTimerNode>("timer");
    factory.Register<cRandomNode>("random");
    factory.Register<cBezierNode>("bezier");
    factory.Register<cCurveNode>("curve");
    factory.Register<c2DSliderNode>("slider2d");
    factory.Register<cOSCSenderNode>("oscsend");
    factory.Register<cOSCReceiveNode>("oscreceive");
    factory.Register<cOSCTargetNode>("osctarget");
    factory.Register<cOSCSourceNode>("oscsource");
    factory.Register<cUnpackNode>("unpack");
    factory.Register<cPackNode>("pack");
    factory.Register<cPackNode>("pak");
    factory.Register<cButtonNode>("button");
    factory.Register<cTogEdgeNode>("togedge");
    factory.Register<cSelectNode>("select");
    factory.Register<cDecodeNode>("decode");
    factory.Register<cTriggerNode>("trigger");
    factory.Register<cGetVariableNode>("get");
    factory.Register<cSetVariableNode>("set");
    factory.Register<cColorPickerNode>("color");
    
    cData::RegisterCodes();
}

cNode *cNodeManager::CreateNamedNode(std::string name)
{
    std::vector<std::string> args;
    args.push_back(name);
    cNode *newNode = factory.Build(name, args);
    if(newNode)
        ImNodes::SetNodeScreenSpacePos(newNode -> uuid, newNodePos);
    return newNode;
}

cNode *cNodeManager::CreateNamedNode(std::vector<std::string> args)
{
    if( args.size() == 0)
        return nullptr;
    cNode *newNode = factory.Build(args[0], args);
    if(newNode)
        ImNodes::SetNodeScreenSpacePos(newNode -> uuid, newNodePos);
    return newNode;
}

void cNodeManager::CreateNewNode(void)
{
    DeleteNode(placeHolder);
    placeHolder = nullptr;
    cNode *newNode = nullptr;
    
    std::vector<std::string> args;
    char delim = ' ';
    std::istringstream ss(newNodeName);
    std::string word;
    while (std::getline(ss, word, delim))
    {
        args.push_back(word);
    }

    CreateNamedNode(args);
}


void cNodeManager::HandleKeys(void)
{
    for(int i = 0; i < contexts.size(); i ++)
    {
        if(contexts[i].isFront)
        {
            SetContext(i);
            break;
        }
    }
    bool respondToKeyboard = !ImGui::GetIO().WantCaptureKeyboard;
    if(respondToKeyboard)
    {
        std::vector<std::string> args;
        if(ImGui::IsKeyPressed('N'))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            args.push_back(std::string("node"));
            cPlaceholderNode *node = new cPlaceholderNode(args);
            if(node)
            {
//                ImNodes::GetCurrentContext() -> CanvasRectScreenSpace
                ImNodes::SetNodeScreenSpacePos(node -> uuid, mousePos);
            }
        }
        else if(ImGui::IsKeyPressed('F'))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            args.push_back(std::string("float"));
            cValueNode *node = new cValueNode(args);
            if(node)
            {
                ImNodes::SetNodeScreenSpacePos(node -> uuid, mousePos);
            }
        }
        else if(ImGui::IsKeyPressed('I'))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            args.push_back(std::string("int"));

            cValueNode *node = new cValueNode(args);
            if(node)
            {
                ImNodes::SetNodeScreenSpacePos(node -> uuid, mousePos);
            }
        }
        else if(ImGui::IsKeyPressed('T'))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            args.push_back(std::string("toggle"));

            cValueNode *node = new cValueNode(args);
            if(node)
            {
                ImNodes::SetNodeScreenSpacePos(node -> uuid, mousePos);
            }
        }
        else if(ImGui::IsKeyPressed('M'))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            args.push_back(std::string("message"));

            cValueNode *node = new cValueNode(args);
            if(node)
            {
                ImNodes::SetNodeScreenSpacePos(node -> uuid, mousePos);
            }
        }
        else if(ImGui::IsKeyPressed('B'))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            args.push_back(std::string("button"));

            cButtonNode *node = new cButtonNode(args);
            if(node)
            {
                ImNodes::SetNodeScreenSpacePos(node -> uuid, mousePos);
            }
        }
        else if(ImGui::IsKeyPressed('O'))
        {
            ToggleShowOptionsForSelectedNodes();
        }
        
        else if(ImGui::IsKeyPressed('D'))
        {
            DuplicateSelectedNodes();
        }
        
        else if(ImGui::IsKeyPressed(259)) // backspace / delete
            DeleteSelectedNodes();
        
/*      for(int i = 0; i < 512; i ++ )
        {
            if(ImGui::GetIO().KeysDown[i])
                printf("key_down %d", i);
        }*/
    }
}


void cNodeManager::ToggleShowOptionsForSelectedNodes(void)
{
    if(currentObjects != nullptr)
    {
        const ImNodesEditorContext& editor = ImNodes::EditorContextGet();
        int selectedNodeSize = editor.SelectedNodeIndices.size();
        if(selectedNodeSize > 0)
        {
            std::vector<int> selectedNodes;
        
            selectedNodes.resize(selectedNodeSize);

            ImNodes::GetSelectedNodes(selectedNodes.data());
            for(int i = 0; i < selectedNodes.size(); i ++)
            {
                int uuid = selectedNodes[i];
                for(int j = 0; j < currentObjects->size(); j ++)
                {
                    cUniqueObject object = (*currentObjects)[j];
                    if(object.type == t_Node)
                    {
                        cNode *node = GetNode(j);
                        if(node -> uuid == uuid)
                        {
                            node -> showOptions = !node -> showOptions;
                        }
                    }
                }
            }
        }
    }
}

void cNodeManager::DuplicateSelectedNodes(void)
{
    const ImNodesEditorContext& editor = ImNodes::EditorContextGet();
    int selectedNodeSize = editor.SelectedNodeIndices.size();
    Json::Value duplicateContainer;
    
    if(currentObjects != nullptr)
    {
        if(selectedNodeSize > 0)
        {
            std::vector<int> selectedNodes;
        
            selectedNodes.resize(selectedNodeSize);

            ImNodes::GetSelectedNodes(selectedNodes.data());
            
            int nodeIndex = 0;
            
            Json::Value nodeSetContainer;
            
            for(int nodeID:selectedNodes)
            {
                cUniqueObject object = (*currentObjects)[nodeID];
                if(object.type == t_Node)
                {
                    Json::Value nodeContainer;
                    cNode *node = object.GetNode();
                    if(node)
                        node -> Save(nodeContainer, ImVec2(20, 20));
                    nodeSetContainer[std::to_string(nodeIndex)] = nodeContainer;
                    nodeIndex ++;
                }
            }
            
            duplicateContainer["nodes"] = nodeSetContainer;
            
            int selectedLinkSize = editor.SelectedLinkIndices.size();
            if(selectedLinkSize > 0)
            {
                int linkIndex = 0;
                std::vector<int> selectedLinks;
            
                selectedLinks.resize(selectedLinkSize);

                ImNodes::GetSelectedLinks(selectedLinks.data());
                Json::Value linkSetContainer;

                for(int linkID:selectedLinks)
                {
                    cUniqueObject object = (*currentObjects)[linkID];
                    if(object.type == t_Link)
                    {
                        Json::Value linkContainer;
                        cNodeLink *link = object.GetLink();
                        if(link)
                        {
                            if(SaveLink(linkContainer, link, &selectedNodes))
                            {
                                linkSetContainer[std::to_string(linkIndex)] = linkContainer;
                                linkIndex ++;
                            }
                        }
                    }
                }
                duplicateContainer["links"] = linkSetContainer;
            }
            ImNodes::ClearLinkSelection();
            ImNodes::ClearNodeSelection();
            LoadFrom(duplicateContainer, true);
            return true;
        }
    }
    return false;
}
 

bool cNodeManager::SaveLink(Json::Value& linkContainer, cNodeLink *link, std::vector<int>*selectedNodes)
{
    cNodeOutput *out = GetOutput(link -> from);
    if(out && currentObjects != nullptr)
    {
        int linkSourceNodeID = out -> node -> uuid;
        
        bool doSave = true;
        if(selectedNodes != nullptr)
            doSave = (std::find(selectedNodes -> begin(), selectedNodes -> end(), linkSourceNodeID) != selectedNodes -> end());
        if(doSave)
        {
            int linkSourceOutputIndex = out -> node -> OutputLocalIndex(out);
            int linkDestNodeID = -1;
            int linkDestInputIndex = -1;
            
            cUniqueObject obj = (*currentObjects)[link -> to];
            if(obj.type == t_Input)
            {
                cNodeInput *in = GetInput(link -> to);
                if(in)
                {
                    linkDestInputIndex = in -> node -> InputLocalIndex(in);
                    linkDestNodeID = in -> node -> uuid;
                }
            }
            else if(obj.type == t_InputProperty)
            {
                cNodePropertyInputStub *in = GetPropertyInput(link -> to);
                if(in)
                {
                    linkDestInputIndex = in -> node -> InputLocalIndex(in);
                    linkDestNodeID = in -> node -> uuid;
                }
            }
            if(linkDestNodeID != -1)
            {
                if(selectedNodes != nullptr)
                    doSave = ( std::find(selectedNodes -> begin(), selectedNodes -> end(), linkDestNodeID) != selectedNodes -> end());
                if(doSave)
                {
                    if( linkSourceOutputIndex != -1 && linkDestInputIndex != -1)
                    {
                        linkContainer["source_node"] = linkSourceNodeID;
                        linkContainer["source_output_index"] = linkSourceOutputIndex;
                        linkContainer["dest_node"] = linkDestNodeID;
                        linkContainer["dest_input_index"] = linkDestInputIndex;
//                        link -> Save(linkContainer);
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void cNodeManager::DeleteSelectedNodes(void)
{
    if(currentObjects != nullptr)
    {
        const ImNodesEditorContext& editor = ImNodes::EditorContextGet();
        int selectedNodeSize = editor.SelectedNodeIndices.size();
        if(selectedNodeSize > 0)
        {
            std::vector<int> selectedNodes;
        
            selectedNodes.resize(selectedNodeSize);

            ImNodes::GetSelectedNodes(selectedNodes.data());
            for(int i = 0; i < selectedNodes.size(); i ++)
            {
                int uuid = selectedNodes[i];
                for(int j = 0; j < currentObjects->size(); j ++)
                {
                    cUniqueObject object = (*currentObjects)[j];
                    if(object.type == t_Node)
                    {
                        cNode *node = GetNode(j);
                        if(node -> uuid == uuid)
                        {
                            DeleteNode(node);
                        }
                    }
                }
            }
        }
        int selectedLinkSize = editor.SelectedLinkIndices.size();
        if(selectedLinkSize > 0)
        {
            std::vector<int> selectedLinks;
        
            selectedLinks.resize(selectedLinkSize);

            ImNodes::GetSelectedLinks(selectedLinks.data());
            for(int i = 0; i < selectedLinks.size(); i ++)
            {
                int uuid = selectedLinks[i];
                for(int j = 0; j < currentObjects->size(); j ++)
                {
                    cUniqueObject object = (*currentObjects)[j];
                    if(object.type == t_Link)
                    {
                        cNodeLink *link = GetLink(j);
                        if(link -> id == uuid)
                        {
                            RemoveLink(uuid);
                        }
                    }
                }
            }
        }
        ImNodes::ClearLinkSelection();
        ImNodes::ClearNodeSelection();
    }
}

void cNodeManager::DeleteLink(int linkID)
{
   //must delete links and disconnect
    cNodeLink *link = GetLink(linkID);
    if(link)
    {
        cNodeOutput *output = GetOutput(link -> from);
        if(output)
        {
            output -> DisconnectLink(link -> id);
        }
    }
}

void cNodeManager::DeleteInput(cNodeInput *input)
{
    RemoveInput(input -> uuid);
    delete input;
}

void cNodeManager::DeleteOutput(cNodeOutput *output)
{
    RemoveOutput(output -> uuid);
    delete output;
}

void cNodeManager::DeleteNode(cNode *node)
{
    RemoveNode(node -> uuid);
    delete node;
}

cNodeInput *cNodeManager::GetInput(int id)
{
    if(currentObjects != nullptr)
    {
        if(id >= 0 && id < currentObjects->size())
        {
            cUniqueObject obj = (*currentObjects)[id];
            switch(obj.type)
            {
                case t_Input:
                    return (cNodeInput *)obj.object;
            }
        }
    }
    return nullptr;
}

cNodePropertyInputStub *cNodeManager::GetPropertyInput(int id)
{
    if(currentObjects != nullptr)
    {
        if(id >= 0 && id < currentObjects->size())
        {
            cUniqueObject obj = (*currentObjects)[id];
            switch(obj.type)
            {
                case t_InputProperty:
                    return (cNodePropertyInputStub *)obj.object;
            }
        }
    }
    return nullptr;
}

cNodeOutput *cNodeManager::GetOutput(int id)
{
    if(currentObjects != nullptr)
    {
        if(id >= 0 && id < currentObjects->size())
        {
            cUniqueObject obj = (*currentObjects)[id];
            if(obj.type == t_Output)
                return (cNodeOutput *)obj.object;
        }
    }
    return nullptr;
}

cNode *cNodeManager::GetNode(int id)
{
    if(currentObjects != nullptr)
    {
        if(id >= 0 && id < currentObjects->size())
        {
            cUniqueObject obj = (*currentObjects)[id];
            if(obj.type == t_Node)
                return (cNode *)obj.object;
        }
    }
    return nullptr;
}

cNodeLink *cNodeManager::GetLink(int id)
{
    if(currentObjects != nullptr)
    {
        if(id >= 0 && id < currentObjects->size())
        {
            cUniqueObject obj = (*currentObjects)[id];
            if(obj.type == t_Link)
                return (cNodeLink *)obj.object;
        }
    }
    return nullptr;
}

int cNodeManager::AddObject(cUniqueObject obj)
{
    int id = -1;
    if(currentObjects != nullptr)
    {
        id = FindEmptyObject();
        if(id != -1)
        {
            (*currentObjects)[id] = obj;
        }
        else
        {
            id = currentObjects->size();
            currentObjects->push_back(obj);
        }
    }
    return id;
}

int cNodeManager::FindEmptyObject(void)
{
    if(currentObjects != nullptr)
    {
        for(int i = 1; i < currentObjects->size(); i ++)
        {
            if((*currentObjects)[i].type == t_Empty)
                return i;
                
        }
    }
    return -1;
}


int cNodeManager::RegisterLink(int from, int to)
{
    if(currentObjects != nullptr)
    {
        int id = currentObjects->size();
        cNodeLink *link = new cNodeLink(id, from, to);
        cNodeOutput *source = GetOutput(from);
        if( source != nullptr )
        {
            cNode *sourceNode = source -> node;
            cNodePropertyInputStub *destProperty = GetPropertyInput(to);

            if( destProperty != nullptr)
            {
                cNode *destNode = destProperty -> node;
                cUniqueObject object;
                object.SetAsLink(link);
                int id = AddObject(object);
                link -> id = id;
                source -> ConnectWithLink(id);
                destProperty -> ConnectFrom(link -> from);
                sourceNode -> ConnectedTo(source, destNode, destProperty);
                return id;
            }
            else
            {
                cNodeInput *dest = GetInput(to);

                if( dest != nullptr)
                {
                    cNode *destNode = dest -> node;
                    cUniqueObject object;
                    object.SetAsLink(link);
                    int id = AddObject(object);
                    link -> id = id;
                    source -> ConnectWithLink(id);
                    dest -> ConnectFrom(link -> from);
                    sourceNode -> ConnectedTo(source, destNode, dest);
                    return id;
                }
            }
        }
    }
    return -1;
}

int cNodeManager::RegisterInput(cNodeInput *in)
{
    cUniqueObject object;
    object.SetAsInput(in);
    int id = AddObject(object);
    in -> uuid = id;
    return id;
}

int cNodeManager::RegisterPropertyInput(cNodePropertyInputStub *in)
{
    cUniqueObject object;
    object.SetAsPropertyInput(in);
    int id = AddObject(object);
    in -> uuid = id;
    return id;
}

int cNodeManager::RegisterProperty(cGUIPropertyStub *prop)
{
    cUniqueObject object;
    object.SetAsProperty(prop);
    int id = AddObject(object);
    prop -> widget_uuid = id;
    return id;
}

int cNodeManager::RegisterOutput(cNodeOutput *out)
{
    cUniqueObject object;
    object.SetAsOutput(out);
    int id = AddObject(object);
    out -> uuid = id;
    return id;
}

int cNodeManager::RegisterOption(cGUIPropertyStub *prop)
{
    cUniqueObject object;
    object.SetAsOption(prop);
    int id = AddObject(object);
    prop -> widget_uuid = id;
    return id;
}


int cNodeManager::RegisterNode(cNode *node)
{
    cUniqueObject object;
    object.SetAsNode(node);
    int id = AddObject(object);
    node -> uuid = id;
    return id;
}

void cNodeManager::Update(void)
{
    int holdContextIndex = currentContext;
    for(int i = 0; i < contexts.size(); i ++)
    {
        SetContext(i);
        if(currentObjects != nullptr)
        {
            for(cUniqueObject object:*currentObjects)
            {
                if( object.type == t_Node)
                {
                    cNode *node = object.GetNode();
                    if(node != nullptr)
                    {
                        T_NodeResult result = node -> Update();
                        if(result == t_NodeTrigger)
                            node -> Execute();
                    }
                }
            }
        }
    }
    SetContext(currentContext);
}

void cNodeManager::DrawAll(ImGuiWindow *toBeFocussed)
{
    for(int i = 0; i < contexts.size(); i ++ )
    {
        if(contexts[i].open)
        {
            SetContext(i);
            if(ImGui::Begin(contexts[i].name.c_str(), &contexts[i].open, ImGuiWindowFlags_MenuBar))
            {
                ImGui::PushID(i);
                SetContext(i);
                contexts[currentContext].contextWindow = ImGui::GetCurrentWindow();
                ImGuiContext& g = *GImGui;
                if(toBeFocussed != nullptr && toBeFocussed == contexts[currentContext].contextWindow)
                {
                    ImGui::FocusWindow(contexts[currentContext].contextWindow);
                }
                contexts[currentContext].isFront = (g.WindowsFocusOrder.back() == contexts[currentContext].contextWindow);
                Draw();
                ImGui::PopID();
            }
            ImGui::End();
        }
    }
}

void cNodeManager::Draw(void)
{
    if(currentObjects != nullptr)
    {
        ImGui::BeginMenuBar();

        if(ImGui::BeginMenu( "File"))
        {
            if(ImGui::MenuItem( "Save Nodes" ))
                Save();
            if(ImGui::MenuItem( "Load Nodes" ))
                Load();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();

        ImNodes::PushStyleVar(ImNodesStyleVar_GridSpacing, 16);
        ImNodes::PushColorStyle(ImNodesCol_GridLine, IM_COL32(60, 60, 60, 255));

        ImNodes::BeginNodeEditor();

        ImNodes::PushStyleVar(ImNodesStyleVar_NodePadding, ImVec2(4, 1));
        ImNodes::PushStyleVar(ImNodesStyleVar_PinOffset, 2);
        ImNodes::PushStyleVar(ImNodesStyleVar_LinkThickness, 1);
        
        for(cUniqueObject object:*currentObjects)
        {
            if( object.type == t_Node)
            {
                cNode *node = object.GetNode();
                if(node != nullptr)
                    node -> Draw();
            }
        }
        
        for(cUniqueObject object:*currentObjects)
        {
            if( object.type == t_Link)
            {
                cNodeLink *link = (cNodeLink *)object.object;
                if(link != nullptr)
                {
                    if( link -> id != -1 )
                        ImNodes::Link(link -> id, link -> from, link -> to);
                }
            }
        }
        if(showMiniMap)
            ImNodes::MiniMap();
        
        ImNodes::PopStyleVar();
        ImNodes::PopStyleVar();
        ImNodes::PopStyleVar();

        ImNodes::EndNodeEditor();

        ImNodes::PopStyleVar();
        ImNodes::PopColorStyle();

        int start_attr, end_attr;
        
        if (ImNodes::IsLinkCreated(&start_attr, &end_attr))
        {
            RegisterLink(start_attr, end_attr);
        }
        
        int destroyedLink = -1;
        
        if (ImNodes::IsLinkDestroyed(&destroyedLink))
        {
            if( destroyedLink != -1)
                DeleteLink(destroyedLink);
        }
        
        if(placeHolder != nullptr)
            CreateNewNode();
    }
}

bool cNodeManager::Save(void)
{
    ofFileDialogResult result = ofSystemSaveDialog(std::string("node_file.json"), "Save Node File");
    if (result.bSuccess)
        SaveTo(result.getPath());
}

bool cNodeManager::SaveTo(std::string path)
{
    Json::Value container;
    if(SaveTo(container))
    {
        ofxJSONElement def(container);
        def.save(path, true);
        return true;
    }
    return false;
}

bool cNodeManager::SaveTo(Json::Value& container)
{
    int nodeIndex = 0;
    
    Json::Value nodeSetContainer;
    if( currentObjects)
    {
        for(cUniqueObject object:*currentObjects)
        {
            if(object.type == t_Node)
            {
                Json::Value nodeContainer;
                cNode *node = object.GetNode();
                if(node)
                    node -> Save(nodeContainer);
                nodeSetContainer[std::to_string(nodeIndex)] = nodeContainer;
                nodeIndex ++;
            }
        }
        container["nodes"] = nodeSetContainer;
        
        int linkIndex = 0;
        Json::Value linkSetContainer;
        for(cUniqueObject object:*currentObjects)
        {
            if(object.type == t_Link)
            {
                Json::Value linkContainer;
                cNodeLink *link = object.GetLink();
                if(link)
                {
                    if(SaveLink(linkContainer, link, nullptr))
                    {
                        linkSetContainer[std::to_string(linkIndex)] = linkContainer;
                        linkIndex ++;
                    }
                }
            }
        }
        container["links"] = linkSetContainer;
    }
    
//    ofxJSONElement def(container);
    return true;
}

bool cNodeManager::Load(void)
{
    ofFileDialogResult result = ofSystemLoadDialog("Save Node File");
    if (result.bSuccess)
        LoadFrom(result.getPath());
}

bool cNodeManager::LoadFrom(const Json::Value& nodeSetContainer, bool select)
{
    int nodeIndex = 0;
    std::map<int, int> linkIndex;
    
    ImNodes::ClearLinkSelection();
    ImNodes::ClearNodeSelection();
    
    const Json::Value nodesJSON = nodeSetContainer["nodes"];
    if (nodesJSON.type() == Json::objectValue)
    {
        int32_t nodeCount = nodesJSON.size();
        
        for(int i = 0; i < nodeCount; i ++)
        {
            Json::Value nodeContainer = nodesJSON[std::to_string(i)];
            if( nodeContainer != Json::nullValue)
            {
                std::string nodeName;
                if(StringFromJSON(nodeContainer, "name", nodeName))
                {
                    cNode *newNode = CreateNamedNode(nodeName);
                    newNode -> Load(nodeContainer);
                    linkIndex[newNode -> loaded_uuid] = newNode -> uuid;
                    if(select)
                        ImNodes::SelectNode(newNode -> uuid);
                }
            }
        }
    }
    const Json::Value linksJSON = nodeSetContainer["links"];
    if (linksJSON.type() == Json::objectValue)
    {
        int32_t linkCount = linksJSON.size();
        for(int i = 0; i < linkCount; i ++)
        {
            Json::Value linkContainer = linksJSON[std::to_string(i)];
            if( linkContainer != Json::nullValue)
            {
                int loadedSourceID, loadedDestID, sourceOutputIndex, destInputIndex;
                
                int sourceOutputID = -1;
                int destInputID = -1;
                
                if(IntFromJSON(linkContainer, "source_node", loadedSourceID))
                {
                    if(IntFromJSON(linkContainer, "source_output_index", sourceOutputIndex))
                    {
                        if(IntFromJSON(linkContainer, "dest_node", loadedDestID))
                        {
                            if(IntFromJSON(linkContainer, "dest_input_index", destInputIndex))
                            {
                                if(linkIndex.count(loadedSourceID) > 0)
                                {
                                    int sourceID = linkIndex[loadedSourceID];
                                    cNode *sourceNode = GetNode(sourceID);
                                    if(sourceNode)
                                    {
                                        if(sourceOutputIndex < sourceNode -> outputObjects.size())
                                        {
                                            cNodeOutput *output = sourceNode -> outputObjects[sourceOutputIndex].GetOutput();
                                            if(output)
                                                sourceOutputID = output -> uuid;
                                        }
                                    }
                                }
                                if(linkIndex.count(loadedDestID) > 0)
                                {
                                    int destID = linkIndex[loadedDestID];
                                    cNode *destNode = GetNode(destID);
                                    if(destNode)
                                    {
                                        if(destInputIndex < destNode -> inputObjects.size())
                                        {
                                            cUniqueObject obj = destNode -> inputObjects[destInputIndex];
                                            if(obj.type == t_Input)
                                            {
                                                cNodeInput *input = obj.GetInput();
                                                if(input)
                                                {
                                                    destInputID = input -> uuid;
                                                }
                                            }
                                            else if(obj.type == t_InputProperty)
                                            {
                                                cNodePropertyInputStub *input = obj.GetPropertyInput();
                                                if(input)
                                                {
                                                    destInputID = input -> uuid;
                                                }
                                            }
                                        }
                                    }
                                    if(sourceOutputID != -1 && destInputID != -1)
                                    {
                                        int linkID = RegisterLink(sourceOutputID, destInputID);
                                        ImNodesEditorContext& editor = ImNodes::EditorContextGet();
                                        ImLinkData& link = ImNodes::ObjectPoolFindOrCreateObject(editor.Links, linkID);

                                        if(select)
                                            ImNodes::SelectLink(linkID);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool cNodeManager::LoadFrom(std::string path)
{
    ofxJSON nodeSetContainer;
    bool success = false;
    int nodeIndex = 0;
    std::map<int, int> linkIndex;
    
    if (path.length() == 0)
        success = nodeSetContainer.open(std::string("node_file.json"));
    else
        success = nodeSetContainer.open(path);
    if (success)
    {
        success = LoadFrom(nodeSetContainer);
    }
    return success;
}

cNodeMemberSpecifier cNodeManager::GetInputSpecifier(int input_uuid)
{
    cNodeMemberSpecifier spec;
    if( currentObjects != nullptr)
    {
        if(input_uuid < currentObjects->size())
        {
            cUniqueObject inputObject = (*currentObjects)[input_uuid];
            switch(inputObject.type)
            {
                case t_Input:
                {
                    cNodeInput *input = inputObject.GetInput();
                    if(input)
                    {
                        spec.node_uuid = input -> node -> uuid;
                        spec.name = input -> label;
                    }
                    break;
                }
                    
                case t_InputProperty:
                {
                    cNodePropertyInputStub *propertyInput = inputObject.GetPropertyInput();
                    if(propertyInput)
                    {
                        spec.node_uuid = propertyInput -> node -> uuid;
                        spec.name = propertyInput -> label;
                    }
                    break;
                }
            }
        }
    }
    return spec;
}

cNodeMemberSpecifier cNodeManager::GetOutputSpecifier(int output_uuid)
{
    cNodeMemberSpecifier spec;
    if( currentObjects != nullptr)
    {
        if(output_uuid < currentObjects->size())
        {
            cUniqueObject outputObject = (*currentObjects)[output_uuid];
            switch(outputObject.type)
            {
                case t_Output:
                    cNodeOutput *output = outputObject.GetOutput();
                    if(output)
                    {
                        spec.node_uuid = output -> node -> uuid;
                        spec.name = output -> label;
                    }
                    break;
            }
        }
    }
    return spec;
}

