//
//  cGUIAction.hpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-14.
//

#ifndef cUIAction_hpp
#define cUIAction_hpp

#include "JSON_utilities.h"
#include <string>
#include "ofxImGui.h"
#include "cGUIProperty.h"
#include "imgui_internal.h"


class cUIActionStub
{
public:
    std::string label;
    bool showWidget = true;
    T_WidgetType widgetType = t_WidgetNone;
    int uuid;
    ImVec2 widgetSize = {120, 12};
    std::vector<std::string> menuOptionsVector;

    virtual void SetActionFunction(std::function<int()> *function){}
    
    virtual int DoIt(void){ return 0; }
    virtual void InitAction(std::string inLabel, T_WidgetType type = t_WidgetButton, bool visible = true)
    {
        label = inLabel;
        SetWidgetType(type);
        showWidget = visible;
        ImGuiContext* g = ImGui::GetCurrentContext();
        const float lineHeight = g -> FontSize;
        widgetSize.y = lineHeight;
    }
    
    virtual bool Draw(void){}
    virtual void SetWidgetType(T_WidgetType inType){ widgetType = inType; }

    void SetMenuOptions( std::vector<std::string> opts )
    {
        menuOptionsVector = opts;
    }
};

template<class T>
class cUIAction : public cUIActionStub
{
public:
    std::vector<T *>actionObjects;
    std::vector<std::function<int(T&)>> actionFunctions;
    
    cUIAction<T>(void){}
    
    virtual void SetActionFunction(std::string actionName, T *uiObject, std::function<int(T&)> function, int optionIndex = 0)
    {
        if(optionIndex >= actionFunctions.size())
        {
            actionFunctions.resize(optionIndex + 1);
            actionObjects.resize(optionIndex + 1);
            menuOptionsVector.resize(optionIndex + 1);
        }
        menuOptionsVector[optionIndex] = actionName;
        actionFunctions[optionIndex] = function;
        actionObjects[optionIndex] = uiObject;
    }
        
    virtual int DoIt(int which = 0)
    {
        if(which < actionFunctions.size())
        {
            if(actionFunctions[which] != nullptr && actionObjects[which] != nullptr )
                return actionFunctions[which](*actionObjects[which]);
        }
        return 0;
    }
    
    virtual bool Draw(void)
    {
        if(showWidget)
        {
            ImGui::PushItemWidth(widgetSize.x);
            switch(widgetType)
            {
                case t_WidgetPopUp:
                {
                    int selectedOption = -1;
                    if(ImGui::Button(label.c_str(), widgetSize))
                        ImGui::OpenPopup(label.c_str());
                    if(ImGui::BeginPopup(label.c_str()))
                    {
                        for(int i = 0; i < menuOptionsVector.size(); i ++)
                        {
                            if (ImGui::Selectable(menuOptionsVector[i].c_str()))
                                selectedOption = i;
                        }
                        ImGui::EndPopup();
                        
                        if(selectedOption != -1)
                        {
                            if(actionFunctions[selectedOption] != nullptr)
                                DoIt(selectedOption);
                        }
                    }
                    break;
                }
                    
                case t_WidgetButton:
                default:
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
                    bool clicked = ImGui::ButtonEx(label.c_str(), widgetSize, ImGuiButtonFlags_PressedOnClick);
                    ImGui::PopStyleVar();
                    if(clicked && actionFunctions[0] != nullptr)
                        DoIt(0);
                    break;
                }
            }
            ImGui::PopItemWidth();
        }
        return false;
    }
    
    void AddPopupMenuItem(std::string itemName, T* actionObject, std::function<int(T&)> f, int index)
    {
        SetActionFunction(itemName, actionObject, f, index);
    }

};


class cUIOtherStub
{
public:
    std::string label;
    bool showWidget = true;
    T_WidgetType widgetType = t_WidgetNone;
    int uuid;
    ImVec2 widgetSize = {120, 12};

    virtual void InitOther(std::string inLabel, T_WidgetType type = t_WidgetSeparator, bool visible = true)
    {
        label = inLabel;
        SetWidgetType(type);
        showWidget = visible;
    }
    
    virtual bool Draw(void){}
    virtual void SetWidgetType(T_WidgetType inType){ widgetType = inType; }
};

class cUIOther : public cUIOtherStub
{
public:
    virtual bool Draw(void)
    {
        if(showWidget)
        {
            ImGui::PushItemWidth(widgetSize.x);
            switch(widgetType)
            {
                case t_WidgetSeparator:
                {
                    ImGui::Separator();
                    break;
                }
                    
                case t_WidgetLabel:
                {
                    ImGui::TextEx(label.c_str());
                    break;
                }
            }
            ImGui::PopItemWidth();
        }
        return false;
    }
};
    

#endif /* cUIAction_hpp */
