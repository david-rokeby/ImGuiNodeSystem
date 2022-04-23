//
//  cNode.hpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-03-14.
//

#ifndef cNode_hpp
#define cNode_hpp

#include <string>
#include "ofxImGui.h"
#include "JSON_utilities.h"
#include "imnodes_internal.h"
#include "ofVectorMath.h"
#include "implot.h"
#include "my_widgets.hpp"
#include <map>
#include "cGUIProperty.h"
#include "cNodeManager.h"
#include "cNodeLink.h"
#include "cNodeInput.h"
#include "cNodeOutput.h"
#include "bezier.hpp"
#include "cData.h"
#include "cVariableManager.hpp"
extern cLog *gLog;
extern bool debug;

//#include "dear_widgets.h"
//#include "dear_widgets.hpp"

class cNodeInput;
class cNodeOutput;
class cNode;
class cNodeLink;

class cNode
{
public:
    static cNodeManager *manager;
    static void SetNodeManager( cNodeManager *man ){ cNode::manager = man; }

    std::string label;
    int32_t loaded_uuid;
    int32_t uuid;
    
    std::vector<cUniqueObject> inputObjects;
    std::vector<cUniqueObject> outputObjects;
    std::vector<cUniqueObject> propertyObjects;
    std::vector<cUniqueObject> optionObjects;

    ImVec2 nodeSize = {0, 0};
    bool showOptions = false;
    bool showTitle = true;
    
    cNode(std::vector<std::string> args)
    {
        if(args.size() > 0 )
        {
            label = args[0];
            uuid = manager -> RegisterNode(this);
        }
        if(debug)
        {
            gLog -> AddLog("Created ");
            for(int i = 0; i < args.size(); i ++)
            {
                gLog -> AddLog("%s ", args[i].c_str());
            }
            gLog -> AddLog("\n");
        }
    }
        
    virtual ~cNode(void)
    {
        Destroy();
    }
    
    
    virtual void ReceiveString(std::string message){}
    virtual void OptionChanged(void){}
    virtual T_NodeResult DrawInternals(void){ return DrawProperties(); }
    virtual T_NodeResult Update(void){return t_NodeNoChange;}
    virtual T_NodeResult DrawOptionsProperties(void)
    {
        T_NodeResult result = t_NodeNoChange;
        if(optionObjects.size() > 0)
        {
            if(showOptions)
            {
                DrawSeparator();
                T_NodeResult localResult = DrawOptions();
                if(localResult > result)
                    result = localResult;
            }
        }
        if(result > t_NodeNoChange)
            OptionChanged();
        return result;
    }
    
    // Draw() should only draw and capture interface changes
    // changes in interface could certainly initiate
    
    // Update() should be separated from Draw() so that is can be called properly even if nothing is drawing (window closed)
    
    // Input -> ReceiveData() causes patch signal chain to propagate
    
    // Some things, like OSC sources, need to be polled, or put in threads, to initiate
    
    // the purpose of the 'changed' boolean is not well through through... is here any need to pass it aloong daisy-chain style or does it all stay internal to the functions (i.e. does
    
    void Draw(void)
    {
        T_NodeResult result = t_NodeNoChange;

        ImNodes::BeginNode(uuid);
        ImNodes::BeginNodeTitleBar();
        if(showTitle)
            ImGui::TextUnformatted(label.c_str());
        else
            ImGui::Dummy(ImVec2(0, 8));
        ImNodes::EndNodeTitleBar();
        
        // note cNodePropertyInput -----
        for(cUniqueObject inBase:inputObjects)
        {
            switch(inBase.type)
            {
                case t_Input:
                 {
                    cNodeInput *in = inBase.GetInput();
                    if(in != nullptr)
                        in -> Draw();
                 }
                break;

                case t_InputProperty:
                 {
                    cNodePropertyInputStub *in = inBase.GetPropertyInput();
                    if(in != nullptr)
                    {
                        T_NodeResult localResult = in -> Draw(); //changed here might indicate a change in property value
                        if( localResult > result)
                            result = localResult;
                        
/*                        if(in -> triggerNode != nullptr)
                        {
                            trigger = true;
                        }*/
                    }
                }
                break;
            }
        }
        
//        changed |= Update(); //Update() reporting change might indicate that processing or output are necessary...
        
        T_NodeResult localResult = DrawInternals();
        if(localResult > result)
            result = localResult;
        
        if(result == t_NodeTrigger)
            Execute();

        for(cUniqueObject outBase:outputObjects)
        {
            if(outBase.type == t_Output)
            {
                cNodeOutput *out = outBase.GetOutput();
                if(out -> label.size() == 0)
                    ImGui::SameLine();
                if(out != nullptr)
                    out -> Draw();
            }
        }
            
        result = DrawOptionsProperties();
        if(result == t_NodeTrigger)
            Execute();
        
        nodeSize = ImNodes::GetNodeDimensions(uuid);
        ImNodes::EndNode();
    }
    

    virtual void Execute(void){}

    void DrawSeparator(void)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 nodePos = ImNodes::GetNodeScreenSpacePos(uuid);
        float height = ImGui::GetTextLineHeightWithSpacing() / 2;
        draw_list -> AddLine(ImVec2(nodePos.x, p.y + height / 2), ImVec2(nodePos.x + nodeSize.x, p.y + height / 2), ImGui::GetColorU32(ImGuiCol_Separator));
        ImGui::Dummy(ImVec2(0, height ));
    }
    
    cNodeInput *AddTriggerInput(std::string label)
    {
        cNodeInput *input = AddInput(label);
        input -> triggerNode = this;
        return input;
    }
    
    cNodeInput *AddInput(std::string label)
    {
        cNodeInput *in = new cNodeInput(label);
        in -> node = this;
        int in_uuid = manager -> RegisterInput(in);
        
        cUniqueObject input;
        input.SetAsInput(in);
        inputObjects.push_back(input);
        return in;
    }
    
    template<class T>
    cNodePropertyInput<T> *AddPropertyInput(std::string label, T initValue, T_WidgetType widgetType = t_WidgetDrag, bool visible = true)
    {
        cNodePropertyInput<T> *input = AddPropertyInput<T>(label, widgetType, visible);
        if(input)
            input -> InitializeData(initValue);
        return input;
    }
    
    template<class T>
    cNodePropertyInput<T> *AddPropertyInput(std::string label, T *storage, T_WidgetType widgetType = t_WidgetDrag, bool visible = true)
    {
        cNodePropertyInput<T> *input = AddPropertyInput<T>(label, widgetType, visible);
        if(input)
            input -> SetStorage(storage);
        return input;
    }
    
    template<class T>
    cNodePropertyInput<T> *AddPropertyInput(std::string label, T_WidgetType widgetType, bool visible)
    {
        cNodePropertyInput<T> *input = new cNodePropertyInput<T>(label, widgetType, visible);
        if(input)
        {
            input -> node = this;
            input -> SetWidgetType(widgetType);
        
            int in_uuid = manager -> RegisterPropertyInput(input);
            cUniqueObject inputObject;
            inputObject.SetAsPropertyInput(input);
            inputObjects.push_back(inputObject);
        }

        return input;
    }
        
    cNodeOutput *AddOutput(std::string label)
    {
        cNodeOutput *out = new cNodeOutput(label);
        out -> node = this;
        int out_uuid = manager -> RegisterOutput(out);
        cUniqueObject output;
        output.SetAsOutput(out);
        outputObjects.push_back(output);

        return out;
    }
    
    template<class T>
    cGUIProperty<T> *AddProperty(std::string label, T *storage, T_WidgetType widgetType = t_WidgetDrag, bool visible = true)
    {
        cGUIProperty<T> *prop = AddProperty<T>(label, widgetType, visible, false);
        if(prop)
            prop -> SetStorage(storage);
        return prop;
    }
        
    template<class T>
    cGUIProperty<T> *AddProperty(std::string label, T initValue = (T)0, T_WidgetType widgetType = t_WidgetDrag, bool visible = true)
    {
        cGUIProperty<T> *prop = AddProperty<T>(label, widgetType, visible, false);
        if(prop)
            prop -> SetData(initValue);
        return prop;
    }
    
    template<class T>
    cGUIProperty<T> *AddProperty(std::string label, T_WidgetType widgetType, bool visible, bool option)
    {
        cGUIProperty<T> *prop = new cGUIProperty<T>;
        if(prop)
        {
            prop -> SetWidgetType(widgetType);
            prop -> InitProperty(label, widgetType, visible);
        }
        int in_uuid = -1;
        
        cUniqueObject property;
        if(option)
        {
            in_uuid = manager -> RegisterOption(prop);
            property.SetAsOption(prop);
            optionObjects.push_back(property);
        }
        else
        {
            in_uuid = manager -> RegisterProperty(prop);
            property.SetAsProperty(prop);
            propertyObjects.push_back(property);
        }
        
        return prop;
    }

    template<class T>
    cGUIProperty<T> *AddOption(std::string label, T_WidgetType widgetType = t_WidgetDrag, bool visible = true)
    {
        return AddProperty<T>(label, widgetType, visible, true);
    }
    
    template<class T>
    cGUIProperty<T> *AddOption(std::string label, T initValue = false, T_WidgetType widgetType = t_WidgetDrag, bool visible = true)
    {
        cGUIProperty<T> *prop = AddOption<T>(label, widgetType, visible);
        if(prop)
            prop -> SetData(initValue);
        return prop;
    }
    
    template<class T>
    cGUIProperty<T> *AddOption(std::string label, T *storage, T_WidgetType widgetType = t_WidgetDrag, bool visible = true)
    {
        cGUIProperty<T> *prop = AddOption<T>(label, widgetType, visible);
        if(prop)
            prop -> SetStorage(storage);
        return prop;
    }

    // sending node uses this to update a list of receivers, where messages will not be passed through normal methods (i.e. just for a patching interface)
    virtual void ConnectedTo(cNodeOutput *sourceOut, cNode *destNode, cNodeInput *destInput){}
    virtual void ConnectedTo(cNodeOutput *sourceOut, cNode *destNode, cNodePropertyInputStub *destInput){}
    virtual void DisconnectedFrom(cNodeOutput *sourceOut, cNode *destNode, cNodeInput *destInput){}
    virtual void DisconnectedFrom(cNodeOutput *sourceOut, cNode *destNode, cNodePropertyInputStub *destInput){}


/*    int GetNamedOutput(std::string outputName)
    {
        for(cUniqueObject outObject:outputObjects)
        {
            if(outObject.type == t_Output)
            {
                cNodeOutput *out = outObject.GetOutput();
                if(out)
                {
                    if(out -> label.compare(outputName) == 0)
                        return out -> uuid;
                }
            }
        }
        return -1;
    }
    
    int GetNamedInput(std::string inputName)
    {
        for(cUniqueObject inObject:inputObjects)
        {
            if(inObject.type == t_Input)
            {
                cNodeInput *in = inObject.GetInput();
                if(in)
                {
                    if(in -> label.compare(inputName) == 0)
                        return in -> uuid;
                }
            }
            else if(inObject.type == t_InputProperty)
            {
                cNodePropertyInputStub *in = inObject.GetPropertyInput();
                if(in)
                {
                    if(in -> label.compare(inputName) == 0)
                        return in -> uuid;
                }
            }
        }
        return -1;
    }*/

    virtual T_NodeResult DrawProperties(void)
    {
        T_NodeResult result = t_NodeNoChange;
        
        for(cUniqueObject prop:propertyObjects)
        {
            if(prop.type == t_Property)
            {
                cGUIPropertyStub *property = prop.GetProperty();
                if(property != nullptr)
                {
                    T_NodeResult localResult = property -> Draw();
                    if(localResult > result)
                        result = localResult;
                }
            }
        }
        return result;
    } //how to handle properties???

    
    virtual T_NodeResult DrawOptions(void)
    {
        T_NodeResult result = t_NodeNoChange;
        
        for(cUniqueObject prop:optionObjects)
        {
            if(prop.type == t_Option)
            {
                cGUIPropertyStub *property = prop.GetProperty();
                if(property != nullptr)
                {
                    T_NodeResult localResult = property -> Draw();
                    if(localResult > result)
                        result = localResult;

                }
            }
        }
        return result;
    }

    cGUIPropertyStub *GetPropertyByName(std::string name)
    {
        for(cUniqueObject object: inputObjects)
        {
            if(object.type == t_InputProperty)
            {
                cNodePropertyInputStub *input = object.GetPropertyInput();
                if(input)
                {
                    if(name.compare(input -> label) == 0)
                        return input -> GetProperty();
                }
            }
        }
        for(cUniqueObject object: propertyObjects)
        {
            if(object.type == t_Property)
            {
                cGUIPropertyStub *property = object.GetProperty();
                if(property)
                {
                    if(name.compare(property -> label) == 0)
                        return property;
                }
            }
        }
        for(cUniqueObject object: optionObjects)
        {
            if(object.type == t_Option)
            {
                cGUIPropertyStub *property = object.GetProperty();
                if(property)
                {
                    if(name.compare(property -> label) == 0)
                        return property;
                }
            }
        }
        return nullptr;
    }
    
    void Destroy(void)
    {
        for(cUniqueObject inObject:inputObjects)
        {
            if(inObject.type == t_Input)
            {
                cNodeInput *in = inObject.GetInput();
                if(in)
                    in -> Disconnect(true);
            }
            else if(inObject.type == t_InputProperty)
            {
                cNodePropertyInputStub *in = inObject.GetPropertyInput();
                if(in)
                    in -> Disconnect(true);
            }
        }
        for(cUniqueObject outObject:outputObjects)
        {
            cNodeOutput *out = outObject.GetOutput();
            if(out)
            {
                out -> Disconnect();
            }
        }
    }
    
    void DeleteInput(cNodeInput *in)
    {
        if(in)
        {
            in -> Disconnect(true);
            for(int i = 0; i < inputObjects.size(); i ++)
            {
                if(inputObjects[i].GetInput() == in)
                {
                    if(i == inputObjects.size() - 1)
                    {
                        inputObjects.resize(i);
                    }
                    else
                    {
                        inputObjects[i].Reset();
                    }
                    break;
                }
            }
            manager -> DeleteInput(in);
        }
    }
    
    void DeleteInputProperty(cNodePropertyInputStub *in)
    {
        if(in)
        {
            in -> Disconnect(true);
            for(int i = 0; i < inputObjects.size(); i ++)
            {
                if(inputObjects[i].GetPropertyInput() == in)
                {
                    if(i == inputObjects.size() - 1)
                    {
                        inputObjects.resize(i);
                    }
                    else
                    {
                        inputObjects[i].Reset();
                    }
                    break;
                }
            }
            manager -> DeletePropertyInput(in);
        }
    }
    
    void DeleteOutput(cNodeOutput *out)
    {
        if(out)
        {
            out -> Disconnect();
            for(int i = 0; i < outputObjects.size(); i ++)
            {
                if(outputObjects[i].GetOutput() == out)
                {
                    if(i == outputObjects.size() - 1)
                    {
                        outputObjects.resize(i);
                    }
                    else
                    {
                        outputObjects[i].Reset();
                    }
                    break;
                }
            }
            manager -> DeleteOutput(out);
        }
    }
    
    std::string Tag(void)
    {
        return label;
    }
    
    virtual bool Save(Json::Value& nodeContainer, ImVec2 offset = {0.0, 0.0})
    {
        nodeContainer["id"] = uuid;
        nodeContainer["name"] = label;
        ImVec2 position = ImNodes::GetNodeEditorSpacePos(uuid);

        nodeContainer["position_x"] = position.x + offset.x;
        nodeContainer["position_y"] = position.y + offset.y;
        
        int propertyIndex = 0;
        Json::Value propertiesContainer;
        for( cUniqueObject propertyInputObject: inputObjects)
        {
            if( propertyInputObject.type == t_InputProperty)
            {
                cNodePropertyInputStub *propertyInput = propertyInputObject.GetPropertyInput();
                if(propertyInput)
                {
                    cGUIPropertyStub *property = propertyInput -> GetProperty();
                    if(property)
                    {
                        Json::Value propertyContainer;
                        property -> Save(propertyContainer);
                        propertiesContainer[std::to_string(propertyIndex)] = propertyContainer;
                        propertyIndex ++;
                    }
                }
            }
        }
        
        for( cUniqueObject propertyObject: propertyObjects)
        {
            if( propertyObject.type == t_Property)
            {
                cGUIPropertyStub *property = propertyObject.GetProperty();
                if(property)
                {
                    Json::Value propertyContainer;
                    property -> Save(propertyContainer);
                    propertiesContainer[std::to_string(propertyIndex)] = propertyContainer;
                    propertyIndex ++;
                }
            }
        }

        for( cUniqueObject optionObject: optionObjects)
        {
            if( optionObject.type == t_Option)
            {
                cGUIPropertyStub *option = optionObject.GetProperty();
                if(option)
                {
                    Json::Value propertyContainer;
                    option -> Save(propertyContainer);
                    propertiesContainer[std::to_string(propertyIndex)] = propertyContainer;
                    propertyIndex ++;
                }
            }
        }

        nodeContainer["properties"] = propertiesContainer;
        
        // save:
        //      label
        //      position
        //      uuid (for temporary maintenance of connections.. may have to be reassigned on load??
        //      inputs and outputs themselves are given so not saved, except where an inlet has a property
        //      in which case it should be saved ?? in the subclasses save?
        //      if we create cNodeProperty members in all subclasses will simply saving them do it, so that subclasses do not bother at all?
        // WELL there may be large unusual data constructs that are better handled by the subclass
        
        // save label
        // save uuid
        // propertyInputs and properties save as properties and are simply there to hold property values to be recovered on load
    }
    
    virtual bool Load(Json::Value& nodeContainer)
    {
        int32_t file_uuid;
        if(IntFromJSON(nodeContainer, "id", file_uuid) == false)
            return false;
        loaded_uuid = file_uuid;
        
        ImVec2 position;
        if(FloatFromJSON(nodeContainer, "position_x", position.x) == false)
            return false;
        if(FloatFromJSON(nodeContainer, "position_y", position.y) == false)
            return false;

        ImNodes::SetNodeEditorSpacePos(uuid, position);

        const Json::Value propertiesJSON = nodeContainer["properties"];
        if (propertiesJSON.type() == Json::objectValue)
        {
            int32_t propertyCount = propertiesJSON.size();
            for(int i = 0; i < propertyCount; i ++)
            {
                const Json::Value propertyJSON = propertiesJSON[std::to_string(i)];
                if (propertyJSON.type() != Json::nullValue)
                {
                    std::string propertyName;
                    if(StringFromJSON(propertyJSON, "label", propertyName))
                    {
                        cGUIPropertyStub *existingProperty = GetPropertyByName(propertyName);
                        if(existingProperty)
                            existingProperty -> Load(propertyJSON);
                    }
                }
            }
        }
        OptionChanged();
    }
    

    int OutputLocalIndex(cNodeOutput *out)
    {
        for(int i = 0; i < outputObjects.size(); i ++)
        {
            cUniqueObject outputObject = outputObjects[i];
            if(outputObject.GetOutput() == out)
                return i;
        }
        return -1;
    }
    
    int InputLocalIndex(void *in)
    {
        for(int i = 0; i < inputObjects.size(); i ++)
        {
            cUniqueObject inputObject = inputObjects[i];
            if(inputObject.object == in)
                return i;
        }
        return -1;
    }
    

};


#endif /* cNode_hpp */
