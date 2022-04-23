//
//  cNodeInput.h
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-09.
//

#ifndef cNodeInput_h
#define cNodeInput_h

#include "ofxImGui.h"
#include "imnodes_internal.h"
#include "cData.h"
#include "my_widgets.hpp"
#include "cGUIProperty.h"

class cNode;
class cNodeManager;
class cGUIPropertyStub;

class cNodePropertyInputStub{
public:
    static cNodeManager *manager;
    static void SetNodeManager( cNodeManager *man ){ manager = man; }

    std::string label;
    int uuid;
    int parent = -1;
    cNode *triggerNode = nullptr;
    bool receivedInput = false;
    cNode *node = nullptr;
    cAtom dummy;
    bool showInput = false;

    bool Trigger(void);
    void Disconnect(bool disconnectFromOutlet);
    void ConnectFrom(int inParentID) { parent = inParentID; }

    virtual cGUIPropertyStub *GetProperty(void){return nullptr;}
    virtual void ReceiveData(cData *data){}
    virtual void SetWidgetType(T_WidgetType inType){}
    virtual T_NodeResult Draw(void){ return t_NodeNoChange;}
    virtual float DataAsFloat(void){ return 0.0f;}
    virtual int DataAsInt(void){ return 0;}
    virtual bool DataAsBool(void){ return false;}
    virtual std::string& DataAsString(void){ return label;}
    virtual cAtom& DataAsAtom(void){ return dummy;}

    virtual void Execute(void)
    {
        if(receivedInput)
        {
            if(Trigger())
                receivedInput = false;
        }
    }
};


// ideally a class or struct can have a static property structure that can be instantly latched onto the structure
// i.e. multiple triggers can exists and when editing a particular trigger, the gui attaches to the particular trigger class instance,

// object subclasses property structure to attach to its data?

template <class T>
class cNodePropertyInput : public cNodePropertyInputStub
{
public:
    cGUIProperty<T> property;

    cNodePropertyInput<T>(std::string inLabel, T_WidgetType type, bool show)
    {
        label = inLabel;
        property.InitProperty(inLabel, type, show);
    }
    
    virtual cGUIPropertyStub *GetProperty(void)
    {
        return (cGUIPropertyStub *)&property;
    }
    
    virtual void ReceiveData(cData *data)
    {
        switch(property.dataType)
        {
            case t_PropertyFloat:
                property.SetData(data -> GetAsFloat());
                break;

            case t_PropertyInt:
                property.SetData(data -> GetAsInt());
                break;

            case t_PropertyBool:
                property.SetData(data -> GetAsBool());
                break;

        }

        receivedInput = true;
        showInput = true;
        Execute();
    }
    
    virtual void SetWidgetType(T_WidgetType inType){ property.SetWidgetType(inType);}
    
    virtual float DataAsFloat(void){ return (float)property.GetData();}
    virtual int DataAsInt(void){ return (int)property.GetData();}
    virtual bool DataAsBool(void){ return (bool)property.GetData();}
//    virtual std::string& DataAsString(void){ return (std::string&)property.GetData();}
    virtual T Data(void){ return property.GetData();}
    
    virtual void InitializeData(T initValue){property.SetData(initValue);}
    virtual void SetStorage(T *storage){property.SetStorage(storage);}

    virtual T_NodeResult Draw(void)
    {
        T_NodeResult result = t_NodeNoChange;
        
        if(showInput)
        {
            ImNodes::PushColorStyle(ImNodesCol_Pin, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
        }

        ImNodes::BeginInputAttribute(uuid);
        result = property.Draw();
        ImNodes::EndInputAttribute();
        if(showInput)
        {
            ImNodes::PopColorStyle();
            showInput = false;
        }
        if(result > t_NodeNoChange && triggerNode)
            result = t_NodeTrigger;
        return result;
    }
};

template <>
class cNodePropertyInput<std::string> : public cNodePropertyInputStub
{
public:
    cGUIProperty<std::string> property;

    cNodePropertyInput<std::string>(std::string inLabel, T_WidgetType type, bool show)
    {
        label = inLabel;
        property.InitProperty(inLabel, type, show);
    }
    
    virtual cGUIPropertyStub *GetProperty(void)
    {
        return (cGUIPropertyStub *)&property;
    }
    
    virtual void ReceiveData(cData *data)
    {
        property.SetData(data -> GetAsString());
        receivedInput = true;
        showInput = true;
        Execute();
    }
    
    virtual void SetWidgetType(T_WidgetType inType){ property.SetWidgetType(inType);}
    
    virtual float DataAsFloat(void){ return std::stof(property.GetData());}
    virtual int DataAsInt(void){ return std::stoi(property.GetData());}
    virtual bool DataAsBool(void){ return (property.GetData() == "true");}
    virtual std::string& DataAsString(void){ return property.GetData();}
    virtual std::string& Data(void){ return property.GetData();}
    
    virtual void InitializeData(std::string& initValue){property.SetData(initValue);}
    virtual void SetStorage(std::string *storage){property.SetStorage(storage);}

    virtual T_NodeResult Draw(void)
    {
        T_NodeResult result = t_NodeNoChange;
        ImNodes::BeginInputAttribute(uuid);
        result = property.Draw();
        ImNodes::EndInputAttribute();

        if(result > t_NodeNoChange && triggerNode)
            result = t_NodeTrigger;
        return result;
    }
};


template <>
class cNodePropertyInput<cAtom> : public cNodePropertyInputStub
{
public:
    cGUIProperty<cAtom> property;

    cNodePropertyInput<cAtom>(std::string inLabel, T_WidgetType type, bool show)
    {
        label = inLabel;
        property.InitProperty(inLabel, type, show);
    }
    
    virtual cGUIPropertyStub *GetProperty(void)
    {
        return (cGUIPropertyStub *)&property;
    }
    
    virtual void ReceiveData(cData *data)
    {
        cAtom& received = data -> GetAsAtom();
        bool wazBang = false;
        if(received.type == t_DataCode)
        {
            if(received.GetAsCode() == t_CodeBang)
            {
                wazBang = true;
            }
        }
        if(!wazBang)
            property.SetData(data -> GetAsAtom());
        receivedInput = true;
        showInput = true;
        Execute();
    }
    
    virtual void SetWidgetType(T_WidgetType inType){ property.SetWidgetType(inType);}
    
    virtual float DataAsFloat(void){ return property.GetData().GetAsFloat();}
    virtual int DataAsInt(void){ return property.GetData().GetAsInt();}
    virtual bool DataAsBool(void){ return property.GetData().GetAsBool();}
    virtual std::string& DataAsString(void){ property.GetData().GetAsString();}
    virtual cAtom& DataAsAtom(void){ return property.GetData();}
    virtual cAtom& Data(void){ return property.GetData();}
    
    virtual void InitializeData(cAtom& initValue){property.SetData(initValue);}
    virtual void SetStorage(cAtom *storage){property.SetStorage(storage);}

    virtual T_NodeResult Draw(void)
    {
        T_NodeResult result = t_NodeNoChange;
        if(showInput)
            ImNodes::PushColorStyle(ImNodesCol_Pin, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));

        ImNodes::BeginInputAttribute(uuid);
        result = property.Draw();
        ImNodes::EndInputAttribute();
        
        if(showInput)
        {
            ImNodes::PopColorStyle();
            showInput = false;
        }

        if(result > t_NodeNoChange && triggerNode)
            result = t_NodeTrigger;
        return result;
    }
};


class cNodeInput
{
public:
    static cNodeManager *manager;
    static void SetNodeManager( cNodeManager *man ){ manager = man; }

    std::string label;
    int uuid;
    int parent = -1;
    cNode *node = nullptr;
    cNode *triggerNode = nullptr;
    bool receivedInput = false;
    bool showInput = false;
    
    cData receivedData;
    
    cNodeInput(std::string inLabel){label = inLabel;}
    
    void ConnectFrom(int inParentID) { parent = inParentID; }
    
    bool Trigger(void);
    
    virtual void ReceiveData(cData *data)
    {
        receivedData.Set(data);
        receivedInput = true;
        showInput = true;
        Execute();
    }

    float DataAsFloat(void){ return receivedData.GetAsFloat(); }
    int DataAsInt(void){ return receivedData.GetAsInt(); }
    bool DataAsBool(void){ return receivedData.GetAsBool(); }
    T_Code DataAsCode(void){ return receivedData.GetAsCode(); }
    std::string& DataAsString(void){ return receivedData.GetAsString(); }
    cAtom& DataAsAtom(void){ return receivedData.GetAsAtom();}
    std::vector<float>& DataAsFloatVector(void){ return receivedData.GetAsFloatVector(); }
    std::vector<int>& DataAsIntVector(void){ return receivedData.GetAsIntVector(); }
    std::vector<bool>& DataAsBoolVector(void){ return receivedData.GetAsBoolVector(); }
    std::vector<std::string>& DataAsStringVector(void){ return receivedData.GetAsStringVector(); }
    std::vector<cAtom>& DataAsList(void){ return receivedData.GetAsList(); }

    void Disconnect(bool disconnectFromOutlet );
    
    void Draw(void)
    {
        T_NodeResult result = t_NodeNoChange;
        
        if(showInput)
        {
            ImNodes::PushColorStyle(ImNodesCol_Pin, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
        }
        ImNodes::BeginInputAttribute(uuid);
        ImGui::Text((const char *)label.c_str());
        ImNodes::EndInputAttribute();
        if(showInput)
        {
            ImNodes::PopColorStyle();
            showInput = false;
        }
        return result;
    }
    
    void Execute(void)
    {
        if(receivedInput)
        {
            if(Trigger())
                receivedInput = false;
        }
    }
};


#endif /* cNodeInput_h */
