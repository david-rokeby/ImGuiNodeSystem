//
//  cUIManager.cpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-14.
//


// N.B. how to deal with multiple windows?
// one manager per window?
// one single registry (static?) then objects list 
#include "cUIManager.hpp"
#include "cUI.hpp"
#include "cUIAction.hpp"
#include "ofSystemUtils.h"

cUIManager::cUIManager(void)
{
    RegisterUIFactories();
    cUI::manager = this;
}

void cUIManager::Clear()
{
    
}

cUI *cUIManager::CreateNamedUI(std::string name)
{
    std::vector<std::string> args;
    args.push_back(name);
    cUI *newUI = factory.Build(name, args);
    cUniqueObject object;
    object.SetAsUI(newUI);
    objects.push_back(object);
    return newUI;
}

// why not just instantiate widgets?
// what about attaching widget sets to structures?
// create widget set, like node without input / output
// single step attach to class instance?

void cUIManager::RegisterUIFactories(void)
{
    factory.Register<cMinMaxUI>("min_max");
}


int cUIManager::RegisterUI(cUI *ui)
{
    int id = registry.size();
    cUniqueObject object;
    object.SetAsUI(ui);
    registry.push_back(object);
    ui -> uuid = id;
    return id;
}


int cUIManager::RegisterProperty(cGUIPropertyStub *prop)
{
    int id = registry.size();
    cUniqueObject object;
    object.SetAsProperty(prop);
    registry.push_back(object);
    prop -> widget_uuid = id;
    return id;
}


int cUIManager::RegisterAction(cUIActionStub *action)
{
    int id = registry.size();
    cUniqueObject object;
    object.SetAsAction(action);
    registry.push_back(object);
    action -> uuid = id;
    return id;
}

int cUIManager::RegisterOther(cUIOtherStub *other)
{
    int id = registry.size();
    cUniqueObject object;
    object.SetAsOther(other);
    registry.push_back(object);
    other -> uuid = id;
    return id;
}

void cUIManager::Draw(void)
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

bool cUIManager::Save(void)
{
    ofFileDialogResult result = ofSystemSaveDialog(std::string("ui_file.json"), "Save UI File");
    if (result.bSuccess)
        SaveTo(result.getPath());
}

bool cUIManager::SaveTo(std::string path)
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
    container["ui"] = uiSetContainer;
    
    ofxJSONElement def(container);
    def.save(path, true);
    return true;
}

bool cUIManager::Load(void)
{
    ofFileDialogResult result = ofSystemLoadDialog("Load UI File");
    if (result.bSuccess)
        LoadFrom(result.getPath());
}

bool cUIManager::LoadFrom(std::string path)
{
    ofxJSON uiSetContainer;
    bool success = false;
    int uiIndex = 0;
    
    if (path.length() == 0)
        success = uiSetContainer.open(std::string("ui_file.json"));
    else
        success = uiSetContainer.open(path);
    if (success)
    {
        const Json::Value uiSetJSON = uiSetContainer["ui"];
        if (uiSetJSON.type() == Json::objectValue)
        {
            int32_t uiCount = uiSetJSON.size();
            
            for(int i = 0; i < uiCount; i ++)
            {
                Json::Value uiContainer = uiSetJSON[std::to_string(i)];
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
        }
    }
    return success;
}

cUI *cUIManager::FindExistingUIByName(std::string name)
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

void cUIManager::AddUI(cUI *ui)
{
    if(ui)
    {
        cUniqueObject uiObject;
        uiObject.SetAsUI(ui);
        objects.push_back(uiObject);
    }
}
