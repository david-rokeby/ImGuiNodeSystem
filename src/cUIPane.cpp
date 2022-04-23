//
//  cGUIPane.cpp
//  ImGuiTester
//
//  Created by David Rokeby on 2022-04-15.
//

#include "cUIPane.hpp"
#include "cUI.hpp"
#include "cUIAction.hpp"
#include "ofSystemUtils.h"

cUIManager *cUIPane::manager = nullptr;

cUIPane::cUIPane(void)
{
}

cUI *cUIPane::CreateNamedUI(std::string name)
{
    std::vector<std::string> args;
    args.push_back(name);
    cUI *newUI = manager -> factory.Build(name, args);
    return newUI;
}


cUI *cUIPane::AddUI(std::string name)
{
    cUI *newUI = CreateNamedUI(name);
    if(newUI)
    {
        cUniqueObject uiObject;
        uiObject.SetAsUI(newUI);
        objects.push_back(uiObject);
    }
    return newUI;
}

// why not just instantiate widgets?
// what about attaching widget sets to structures?
// create widget set, like node without input / output
// single step attach to class instance?

void cUIPane::Draw(void)
{
    for(cUniqueObject object:objects)
    {
        if( object.type == t_UI)
        {
            cUI *ui = object.GetUI();
            if(ui != nullptr)
                ui -> Draw();
        }
    }
}

bool cUIPane::SaveTo(Json::Value& uiPaneContainer)
{
    Json::Value container;
    int uiIndex = 0;
    
    Json::Value uiSetContainer;
    
    for(cUniqueObject object:objects)
    {
        if(object.type == t_UI)
        {
            Json::Value uiContainer;
            cUI *ui = object.GetUI();
            if(ui)
                ui -> Save(uiContainer);
            uiSetContainer[std::to_string(uiIndex)] = uiContainer;
            uiIndex ++;
        }
    }
    return true;
}

bool cUIPane::LoadFrom(Json::Value& uiPaneContainer)
{
    bool success = false;
    int uiIndex = 0;
    
    if (uiPaneContainer.type() == Json::objectValue)
    {
        int32_t uiCount = uiPaneContainer.size();
        
        for(int i = 0; i < uiCount; i ++)
        {
            Json::Value uiContainer = uiPaneContainer[std::to_string(i)];
            if( uiContainer != Json::nullValue)
            {
                std::string uiName;
                if(StringFromJSON(uiContainer, "name", uiName))
                {
                    cUI *existingUI = FindExistingUIByName(uiName);
                    if(existingUI)
                    {
                        existingUI -> Load(uiContainer);
                    }
                    else
                    {
                        cUI *newUI = CreateNamedUI(uiName);
                        if(newUI)
                        {
                            newUI -> Load(uiContainer);
                        }
                    }
                }
            }
        }
        success = true;
    }
    return success;
}

cUI *cUIPane::FindExistingUIByName(std::string name)
{
    for(cUniqueObject object:objects)
    {
        if( object.type == t_UI)
        {
            cUI *ui = object.GetUI();
            if(ui != nullptr)
            {
                if( ui -> label.compare(name) == 0)
                    return ui;
            }
        }
    }
    return nullptr;
}


int cUIPane::RegisterUI(cUI *ui)
{
    manager -> RegisterUI(ui);
    cUniqueObject object;
    object.SetAsUI(ui);
    objects.push_back(object);
    return ui -> uuid;
}


int cUIPane::RegisterProperty(cGUIPropertyStub *prop)
{
    manager -> RegisterProperty(prop);
    cUniqueObject object;
    object.SetAsProperty(prop);
    objects.push_back(object);
    return prop -> widget_uuid;
}


int cUIPane::RegisterAction(cUIActionStub *action)
{
    manager -> RegisterAction(action);
    cUniqueObject object;
    object.SetAsAction(action);
    objects.push_back(object);
    return action -> uuid;
}

int cUIPane::RegisterOther(cUIOtherStub *other)
{
    manager -> RegisterOther(other);
    cUniqueObject object;
    object.SetAsOther(other);
    objects.push_back(object);
    return other -> uuid;
}

