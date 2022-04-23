//
//  cVariableManager.hpp
//  ImGuiTester
//
//  Created by David Rokeby on 2022-04-18.
//

#ifndef cVariableManager_hpp
#define cVariableManager_hpp

#include <string>
#include <vector>

#include "cData.h"

class cVariableReferenceBase
{
public:
    std::string variable_name;
    T_DataType type = t_DataNone;
    bool readOnly = false;
    
    void SetReadOnly(void){ readOnly = true;}
    
    virtual bool Set(float inVal){return false;}
    virtual bool Set(int inVal){return false;}
    virtual bool Set(cAtom inVal){return false;}
    virtual bool Set(bool inVal){return false;}
    virtual bool Set(std::string& inVal){return false;}
    virtual bool Set(std::vector<float> inVal){return false;}
    virtual bool Set(std::vector<int> inVal){return false;}
    virtual bool Set(std::vector<bool> inVal){return false;}
    virtual bool Set(std::vector<std::string> inVal){return false;}
    virtual bool Set(std::vector<cAtom> inVal){return false;}
    virtual bool ValueChanged(cData& oldValue){ return false;}
    virtual void CopyTo(cData& data){ return false;}

};

template<class T>
class cVariableReference : public cVariableReferenceBase
{
public:
    T *reference = nullptr;
    
    cVariableReference<T>(std::string name, T *variableRef, bool inReadOnly = false){variable_name = name; T typeTest; type = GetDataType(typeTest); reference = variableRef; readOnly = inReadOnly;}

    T *GetReference(void){ return reference;}
    void SetReference(std::string name, T *ref){variable_name = name; reference = ref;}
    T Get(void){if(reference) return *reference;}
    virtual bool Set(T inVal){if(reference && !readOnly) *reference = inVal;}
    virtual bool SetRef(T& inVal){if(reference && !readOnly) *reference = inVal;}
    bool ValueChanged(T oldVal){ return (oldVal == *reference); }
    bool ValueChanged(cData& oldValue){ return !oldValue.Same(*reference);}
    virtual void CopyTo(cData& data){ if(reference && !readOnly) data.Set(*reference);}
};


class cVariableManager
{
public:
    std::vector<cVariableReferenceBase *> variableRegistry;
    
    cVariableManager(void);
    
    cVariableReferenceBase *FindVariableByName(std::string name)
    {
        for(int i = 0; i < variableRegistry.size(); i ++)
        {
            cVariableReferenceBase *ref = variableRegistry[i];
            if(ref -> variable_name == name)
            {
                return ref;
            }
        }
        return nullptr;
    }
    
    template<class T>
    void AddVariable(std::string name, T& inValue)
    {
        cVariableReference<T> *variable = new cVariableReference<T>(name, &inValue);
        variableRegistry.push_back(variable);
    }
    
    cVariableReference<float> *FindFloatByName(std::string name){ return ReferenceAsFloat(FindVariableByName(name));}
    
    cVariableReference<int> *FindIntByName(std::string name){ return ReferenceAsInt(FindVariableByName(name));}
    
    cVariableReference<bool> *FindBoolByName(std::string name){ return ReferenceAsBool(FindVariableByName(name));}
    
    cVariableReference<std::string> *FindStringByName(std::string name){ return ReferenceAsString(FindVariableByName(name));}
 
    cVariableReference<float> *ReferenceAsFloat(cVariableReferenceBase *ref)
    {
        if(ref && ref -> type == t_DataFloat)
            return (cVariableReference<float> *)ref;
        return nullptr;
    }

    cVariableReference<int> *ReferenceAsInt(cVariableReferenceBase *ref)
    {
        if(ref && ref -> type == t_DataInt)
            return (cVariableReference<int> *)ref;
        return nullptr;
    }

    cVariableReference<bool> *ReferenceAsBool(cVariableReferenceBase *ref)
    {
        if(ref && ref -> type == t_DataBool)
            return (cVariableReference<bool> *)ref;
        return nullptr;
    }
   
    cVariableReference<std::string> *ReferenceAsString(cVariableReferenceBase *ref)
    {
        if(ref && ref -> type == t_DataString)
            return (cVariableReference<std::string> *)ref;
        return nullptr;
    }
};


#endif /* cVariableManager_hpp */
