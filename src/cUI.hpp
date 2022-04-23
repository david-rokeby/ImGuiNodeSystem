//
//  cUI.hpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-14.
//

#ifndef cUI_hpp
#define cUI_hpp

#include <string>
#include "ofxImGui.h"
#include "cUIManager.hpp"
#include "JSON_utilities.h"
#include "ofxJSON.h"
#include "ofVectorMath.h"
#include "implot.h"
#include "my_widgets.hpp"
#include "cGUIProperty.h"
#include "bezier.hpp"
#include "cUniqueObject.hpp"
#include "cUIAction.hpp"

#define MEMBER_FUNCTION(c, f) (std::function<int(c&)>) &c::f

class cUI
{
public:
    static cUIManager *manager;
    static void SetUIManager( cUIManager *man ){ cUI::manager = man; }

    std::string label;
    // should be a label and a name
    // name can distinguish between multiple ui structures
    int uuid;
    std::vector<cUniqueObject> objects;
    bool treeNode = false;
    
    cUI(std::vector<std::string>args)
    {
        if(args.size() > 0 )
        {
            label = args[0];
            uuid = manager -> RegisterUI(this);
        }
    }
    
    cUI(std::string name)
    {
        label = name;
        uuid = manager -> RegisterUI(this);
    }
    
    bool Draw(void)
    {
        bool changed = false;
        
        changed = DrawInternals(changed);
        return changed;
    }
    
    virtual bool Save(Json::Value& uiContainer)
    {
        uiContainer["type"] = "group";
        uiContainer["id"] = uuid;
        uiContainer["name"] = label;
        
        int propertyIndex = 0;
        Json::Value propertiesContainer;

        for( cUniqueObject propertyObject: objects)
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
            else if( propertyObject.type == t_UI)
            {
                Json::Value uiContainer;
                cUI *ui = propertyObject.GetUI();
                if(ui)
                {
                    ui -> Save(uiContainer);
                    propertiesContainer[std::to_string(propertyIndex)] = uiContainer;
                    propertyIndex ++;
                }
            }
        }
        uiContainer["properties"] = propertiesContainer;
    }
    
    virtual bool Load(const Json::Value& uiContainer)
    {
        int32_t file_uuid;
        if(IntFromJSON(uiContainer, "id", file_uuid) == false)
            return false;

        const Json::Value propertiesJSON = uiContainer["properties"];
        if (propertiesJSON.type() == Json::objectValue)
        {
            int32_t propertyCount = propertiesJSON.size();
            for(int i = 0; i < propertyCount; i ++)
            {
                const Json::Value propertyJSON = propertiesJSON[std::to_string(i)];
                if (propertyJSON.type() != Json::nullValue)
                {
                    // somehow we have to indicate whether this is a property ot expands to something else
                    // i.e. another ui
                    std::string type;
                    std::string propertyName;
                    if(StringFromJSON(propertyJSON, "type", type))
                    {
                        if(type.compare("property") == 0)
                        {
                            if(StringFromJSON(propertyJSON, "label", propertyName))
                            {
                                cGUIPropertyStub *existingProperty = GetPropertyByName(propertyName);
                                if(existingProperty)
                                    existingProperty -> Load(propertyJSON);
                            }
                        }
                        else if(type.compare("group") == 0)
                        {
                            bool result = Load(propertyJSON);
                        }
                    }
                }
            }
        }
        return true;
    }
    
    void AddRadioButtons(int *state, std::vector<std::string> options)
    {
        cGUIProperty<int> *radio = AddProperty<int>("", state, t_WidgetRadioButtons);
        if( radio)
            radio -> SetRadioOptions(options);
    }

    template<class TT>
    void AddButton(std::string label, std::function<int(TT&)> f)
    {
        cUIAction<TT>* button = AddAction<TT>(label);
        if(button)
            button -> SetActionFunction("do it", static_cast<TT *>(this), f);
    }

    template<class TT>
    cUIAction<TT>* AddPopupMenu(std::string name)
    {
        cUIAction<TT> *menu = AddAction<TT>(name, t_WidgetPopUp);
        return menu;
    }
    
    void AddSeparator(void){ AddOther(""); }
    
    void AddLabel(std::string name){ AddOther(name, t_WidgetLabel); }
    
    void AddUI(cUI *ui, bool visible = true)
    {
        cUniqueObject uiObject;
        uiObject.SetAsUI(ui);
        objects.push_back(uiObject);
    }
    
    template<class TT>
    cGUIProperty<TT> *AddProperty(std::string label, TT *storage, T_WidgetType inWidgetType = t_WidgetDrag, bool visible = true)
    {
        cGUIProperty<TT> *prop = AddProperty<TT>(label, inWidgetType, visible, false);
        if(prop)
            prop -> SetStorage(storage);
        return prop;
    }
      
    
    template<class TT>
    cGUIProperty<TT> *AddProperty(std::string label, TT initValue = (TT)0, T_WidgetType inWidgetType = t_WidgetDrag, bool visible = true)
    {
        cGUIProperty<TT> *prop = AddProperty<TT>(label, inWidgetType, visible, false);
        if(prop)
            prop -> SetData(initValue);
        return prop;
    }
    
    
    template<class TT>
    cGUIProperty<TT> *AddProperty(std::string label, T_WidgetType inWidgetType, bool visible, bool option)
    {
        cGUIProperty<TT> *prop = new cGUIProperty<TT>;
        if(prop)
        {
            prop -> InitProperty(label, inWidgetType, visible);
        }
        
        int in_uuid = -1;
        
        in_uuid = manager -> RegisterProperty(prop);
        cUniqueObject property;
        property.SetAsProperty(prop);
        objects.push_back(property);
        
        return prop;
    }

    template<class TT>
    cUIAction<TT> *AddAction(std::string label, T_WidgetType inWidgetType = t_WidgetButton, bool visible = true )
    {
        cUIAction<TT> *action = new cUIAction<TT>;
        if(action)
        {
            action -> InitAction(label, inWidgetType, visible);
        }
        
        int in_uuid = -1;
        
        in_uuid = manager -> RegisterAction(action);
        
        cUniqueObject actionObject;
        actionObject.SetAsAction(action);
        objects.push_back(actionObject);
        
        return action;
    }
    
    cUIOther *AddOther(std::string label, T_WidgetType inWidgetType = t_WidgetSeparator, bool visible = true )
    {
        cUIOther *other = new cUIOther;
        if(other)
        {
            other -> SetWidgetType(inWidgetType);
            other -> InitOther(label, inWidgetType, visible);
        }
        
        int in_uuid = -1;
        
        cUniqueObject otherObject;
        in_uuid = manager -> RegisterOther(other);
        otherObject.SetAsOther(other);
        objects.push_back(otherObject);
        
        return other;
    }
    
    virtual bool DrawInternals(bool changed)
    {
        if(treeNode)
        {
            if(ImGui::TreeNode(label.c_str()))
            {
                changed = DrawProperties(changed);
                ImGui::TreePop();
            }
        }
        else
            changed = DrawProperties(changed);
        return changed;
    }

    virtual bool DrawProperties(bool changed)
    {
        bool localChange = false;
        for(cUniqueObject prop:objects)
        {
            switch(prop.type)
            {
                case t_Property:
                {
                    cGUIPropertyStub *property = prop.GetProperty();
                    if(property != nullptr)
                        localChange = property -> Draw();
                    break;
                }
            
                case t_Action:
                {
                    cUIActionStub *action = prop.GetAction();
                    if(action != nullptr)
                        localChange = action -> Draw();
                    break;
                }
           
                case t_Other:
                {
                    cUIOtherStub *other = prop.GetOther();
                    if(other != nullptr)
                        localChange = other -> Draw();
                    break;
                }
                    
                case t_UI:
                {
                    cUI *ui = prop.GetUI();
                    if(ui != nullptr)
                        localChange = ui -> Draw();
                    break;
                }
                    
                case t_TabBar:
                {
                    
                    break;
                }
            }
            changed = changed || localChange;
        }
        return changed;
    }

    cGUIPropertyStub *GetPropertyByName(std::string name)
    {
        for(cUniqueObject object: objects)
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
        return nullptr;
    }
    
    std::string Tag(void)
    {
        return label;
    }
};

class cUITabBar : public cUI
{
public:
    int tabCount = 0;
    
    cUITabBar(std::vector<std::string> args) : cUI(args){}
    
    cUITabBar(std::string name) : cUI(name){}
    
    void AddTab(cUI *tabUI)
    {
        if(tabUI)
        {
            cUniqueObject object;
            object.type = t_UI;
            object.SetAsUI(tabUI);
            objects.push_back(object);
            tabCount ++;
        }
    }
    
    virtual bool DrawInternals(bool changed)
    {
        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 6.0f);

        if (ImGui::BeginTabBar(label.c_str(), tab_bar_flags))
        {
            for(cUniqueObject object:objects)
            {
                if(object.type == t_UI)
                {
                    cUI *ui = object.GetUI();
                    if(ui)
                    {
                        if (ImGui::BeginTabItem(ui -> label.c_str()))
                        {
                            changed = ui -> Draw();
                            ImGui::EndTabItem();
                        }
                    }
                }
            }
            ImGui::EndTabBar();
        }
        ImGui::PopStyleVar();

        return changed;
    }
};

class cUITreeNode : public cUI
{
public:
    
    cUITreeNode(std::string label) : cUI(label)
    {
    }
    
    cUITreeNode(std::vector<std::string> args) : cUI(args)
    {
    }
    
    virtual bool DrawInternals(bool changed)
    {
        if(ImGui::TreeNode(label.c_str()))
        {
            changed = DrawProperties(changed);
            ImGui::TreePop();
        }

        return changed;
    }
};



class cMinMax
{
public:
    float min;
    float max;
    std::string ip;
    
    cMinMax(void){min = 0; max = 255; ip = "127.0.0.1";}
};


class cOSCSpec
{
public:
    std::string ip;
    std::string address;
    int port;
    
    cOSCSpec(void){ip = "127.0.0.1"; address = "/test"; port = 2500;}
};


class cOSCUI : public cUI
{
public:
    cGUIProperty<int> *port = nullptr;
    cGUIProperty<std::string> *ip = nullptr;
    cGUIProperty<std::string> *address = nullptr;
    cUIAction<cOSCUI> *button = nullptr;
    cOSCSpec *oscSpec;
    cOSCSpec realOSCSpec;
    
    cOSCUI(std::vector<std::string> args) : cUI(args){Setup();}
    cOSCUI(std::string name) : cUI(name){Setup();}

    void Setup(void)
    {
        oscSpec = &realOSCSpec;
        ip = AddProperty<std::string>("ip address", &(oscSpec->ip));
        port = AddProperty<int>("port", &(oscSpec->port) );
        port -> sameLine = true;
        address = AddProperty<std::string>("osc address", &(oscSpec->address));
        AddButton<cOSCUI>("do it", MEMBER_FUNCTION(cOSCUI, Test));
    }
    
    int Test(void)
    {
        return 0;
    }
    
    void AttachTo(cOSCSpec *externalSpec)
    {
        oscSpec = externalSpec;
        ip -> SetStorage(&(oscSpec->ip));
        address -> SetStorage(&(oscSpec->address));
        port -> SetStorage(&(oscSpec->port));
    }
};

class cMinMaxUI : public cUI
{
public:
    cGUIProperty<float> *min = nullptr;
    cGUIProperty<float> *max = nullptr;
    cGUIProperty<std::string> *ip = nullptr;
    cUIAction<cMinMaxUI> *button = nullptr;
    cUIAction<cMinMaxUI> *menu = nullptr;
    cGUIProperty<int> *radio = nullptr;
    cMinMax *minmax;
    cMinMax realMinMax;
    int radioState = 0;
    cOSCUI *oscUI;

    cMinMaxUI(std::vector<std::string> args) : cUI(args){SetUp();}
    cMinMaxUI(std::string name) : cUI(name){SetUp();}

    void SetUp(void)
    {
        minmax = &realMinMax;
        min = AddProperty<float>("min", &(minmax->min) );
        max = AddProperty<float>("max", &(minmax->max) );
        ip = AddProperty<std::string>("ip address", &(minmax->ip));
        
        std::vector<std::string> radioOptions = {"first", "second", "third"};
        AddRadioButtons(&radioState, radioOptions);
        
        AddSeparator();

        AddButton<cMinMaxUI>("do it", MEMBER_FUNCTION(cMinMaxUI, Increment));

        menu = AddPopupMenu<cMinMaxUI>("do what?");
        menu -> AddPopupMenuItem("increment", this, MEMBER_FUNCTION(cMinMaxUI, Increment), 0);
        menu -> AddPopupMenuItem("decrement", this, MEMBER_FUNCTION(cMinMaxUI, Decrement), 1);
        menu -> AddPopupMenuItem("double", this, MEMBER_FUNCTION(cMinMaxUI, Double), 2);

        AddSeparator();
        
//        std::vector<std::string> title = {"Open Sound Control"};
        cUITreeNode *treeNode = new cUITreeNode("Open Sound Control");
       
        if(treeNode)
        {
            cUITabBar *tabs = new cUITabBar("options");
            if(tabs)
            {
                cOSCUI *oscUI = new cOSCUI("osc");
                if(oscUI)
                {
                    oscUI -> label = "osc1";
                    tabs -> AddTab(oscUI);
                }
                cOSCUI *oscUI2 = new cOSCUI("osc");
                if(oscUI2)
                {
                    oscUI2 -> label = "osc2";
                    tabs -> AddTab(oscUI2);
                }
            }
            treeNode -> AddUI(tabs);
        }
        AddUI(treeNode);
    }

    int Increment(void)
    {
        minmax -> min -= 1;
        minmax -> max += 1;
        return 0;
    }
    
    int Decrement(void)
    {
        minmax -> min += 1;
        minmax -> max -= 1;
        return 0;
    }
    
    int Double(void)
    {
        minmax -> min *= 2;
        minmax -> max *= 2;
        return 0;
    }
    
    void AttachTo(cMinMax *externalMinMax)
    {
        minmax = externalMinMax;
        min -> SetStorage(&(minmax->min));
        max -> SetStorage(&(minmax->max));
        ip -> SetStorage(&(minmax->ip));
    }
};


#endif /* cUI_hpp */
