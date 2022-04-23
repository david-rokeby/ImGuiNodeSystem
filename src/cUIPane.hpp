//
//  cGUIPane.hpp
//  ImGuiTester
//
//  Created by David Rokeby on 2022-04-15.
//

#ifndef cGUIPane_hpp
#define cGUIPane_hpp

class cUI;
class cUIActionStub;

#include <string>
#include <vector>
#include <map>
#include "cUniqueObject.hpp"
#include "ofxJSON.h"
#include "cUIManager.hpp"

class cUIPane
{
public:
    static cUIManager *manager;
    static void SetUIManager( cUIManager *man ){ cUIPane::manager = man; }

    std::vector<cUniqueObject>objects;
    
    cUIPane(void);
    cUI *CreateNamedUI(std::string name);
    cUI *AddUI(std::string name);

    void Clear(void);
    void Draw(void);
    bool SaveTo(Json::Value& uiPaneContainer);
    bool LoadFrom(Json::Value& uiPaneContainer);
    cUI *FindExistingUIByName(std::string name);
    
    int RegisterUI(cUI *ui);
    int RegisterProperty(cGUIPropertyStub *prop);
    int RegisterAction(cUIActionStub *action);
    int RegisterOther(cUIOtherStub *other);
};

#endif /* cGUIPane_hpp */
