//
//  cUIManager.hpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-14.
//

#ifndef cUIManager_hpp
#define cUIManager_hpp

class cUI;
class cUIActionStub;

#include <string>
#include <vector>
#include <map>
#include "cUniqueObject.hpp"

class cUISpecifier
{
public:
    int uuid;
    std::string name;
};

class cUIBuilderBase
{
public:
  virtual cUI *Build(std::vector<std::string> args){}
};

template <class T>
class cUIBuilder : public cUIBuilderBase
{
public:
    virtual cUI *Build(std::vector<std::string> args)
    {
        return new T(args);
    }
};

class cUIFactory
{
public:
    template <class T>
    void Register(std::string name)
    {
        m_instantiators[name] = new cUIBuilder<T>();
    }
    
    cUI *Build(std::string name, std::vector<std::string> args)
    {
        if( m_instantiators.count(name) > 0)
            return m_instantiators[name] -> Build(args);
        return nullptr;
    }

private:
    std::map<std::string, cUIBuilderBase *> m_instantiators;
};


class cUIManager
{
public:
    cUIFactory factory;
    std::vector<cUniqueObject>registry;
    std::vector<cUniqueObject>objects;
    
    
    cUIManager(void);
    void RegisterUIFactories(void);
    cUI *CreateNamedUI(std::string name);
    int RegisterUI(cUI *ui);
    int RegisterProperty(cGUIPropertyStub *prop);
    int RegisterAction(cUIActionStub *action);
    int RegisterOther(cUIOtherStub *other);

    void Clear(void);
    void Draw(void);
    bool Save(void);
    bool SaveTo(std::string path);
    bool Load(void);
    bool LoadFrom(std::string path);
    cUI *FindExistingUIByName(std::string name);
    
    void AddUI(cUI *ui);

};

#endif /* cUIManager_hpp */
