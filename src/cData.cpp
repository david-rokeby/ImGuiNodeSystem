//
//  cData.cpp
//  ImGuiTester
//
//  Created by David Rokeby on 2022-04-17.
//

#include <string>
#include <map>
#include "cData.h"
#include "cVariableManager.hpp"

template<class T> T_DataType GetDataType(T in) {return t_DataNone;}
template<> T_DataType GetDataType(float in){ return t_DataFloat; }
template<> T_DataType GetDataType(int in){ return t_DataInt; }
template<> T_DataType GetDataType(bool in){ return t_DataBool; }
template<> T_DataType GetDataType(T_Code in){ return t_DataCode; }
template<> T_DataType GetDataType(std::string in){ return t_DataString; }
template<> T_DataType GetDataType(cAtom in){ return t_DataAtom; }
template<> T_DataType GetDataType(std::vector<float>){ return t_DataFloatVector; }
template<> T_DataType GetDataType(std::vector<int>){ return t_DataIntVector; }
template<> T_DataType GetDataType(std::vector<bool>){ return t_DataBoolVector; }
template<> T_DataType GetDataType(std::vector<std::string>){ return t_DataStringVector; }
template<> T_DataType GetDataType(std::vector<cAtom>){ return t_DataList; }

template<class T> T_PropertyType GetPropertyType(T in) {return t_PropertyNone; }
template<> T_PropertyType GetPropertyType(float in){ return t_PropertyFloat; }
template<> T_PropertyType GetPropertyType(int in){ return t_PropertyInt; }
template<> T_PropertyType GetPropertyType(bool in){ return t_PropertyBool; }
template<> T_PropertyType GetPropertyType(T_Code in){ return t_PropertyCode; }
template<> T_PropertyType GetPropertyType(std::string in){ return t_PropertyString; }
template<> T_PropertyType GetPropertyType(cAtom in){ return t_PropertyAtom; }
template<> T_PropertyType GetPropertyType(std::vector<float>){ return t_PropertyFloatVector; }
template<> T_PropertyType GetPropertyType(std::vector<int>){ return t_PropertyIntVector; }
template<> T_PropertyType GetPropertyType(std::vector<bool>){ return t_PropertyBoolVector; }
template<> T_PropertyType GetPropertyType(std::vector<std::string>){ return t_PropertyStringVector; }
template<> T_PropertyType GetPropertyType(std::vector<cAtom>){ return t_PropertyList; }


std::map<T_Code, std::string> cData::codeBook;
std::map<std::string, T_Code> cData::reverseCodeBook;

std::string& cAtom::GetAsString(void)
{
    switch(type)
    {
        case t_DataFloat:
            FloatToDataString(dataFloat, 3, true);
            return dataString;
            
        case t_DataInt:
            dataString = std::to_string(dataInt);
            return dataString;
            
        case t_DataCode:
            if(cData::codeBook.count(dataCode) > 0)
                dataString = cData::codeBook[dataCode];
            else
                dataString = "";
            return dataString;
            
        case t_DataBool: dataString = dataBool ? "true" : "false"; return dataString ;
        case t_DataString: return dataString;
    }
    dataString = "";
    return dataString;
}

T_Code cAtom::GetAsCode(void)
{
    switch(type)
    {
        case t_DataFloat: return cData::FloatAsCode(dataFloat);
        case t_DataInt: return cData::IntAsCode(dataInt);
        case t_DataCode: return dataCode;
        case t_DataBool: return cData::BoolAsCode(dataBool);
        case t_DataString: return cData::StringAsCode(dataString);
    }
    return t_CodeNothing;
}



int safe_stoi(std::string inString)
{
    int val = 0;
    try
    {
        val = std::stoi(inString);
    }
    catch(...){}
    return val;
}

float safe_stof(std::string inString)
{
    float val = 0;
    try
    {
        val = std::stof(inString);
    }
    catch(...){}
    return val;
}

void ConvertStringToAtom(std::string& inString, cAtom& outAtom)
{
    bool caughtInt = false;
    int inter;
    try
    {
        inter = std::stoi(inString);
        outAtom.Set(inter);
        caughtInt = true;
    }
    catch (...){}
    try
    {
        float floater = std::stof(inString);
        if(caughtInt)
        {
            if(abs(floater) > abs((float)inter))
                outAtom.Set(floater);
            return;
        }
        else
        {
            outAtom.Set(floater);
            return;
        }
    }
    catch (...){}
    outAtom.Set(inString);
}

bool cData::FillReference(cVariableReferenceBase *reference)
{
    if(type == reference -> type)
    {
       switch(type)
        {
            case t_DataFloat: reference -> Set(dataFloat); break;
            case t_DataInt: reference -> Set(dataInt); break;
            case t_DataBool: reference -> Set(dataBool); break;
            case t_DataString: reference -> Set(dataString); break;
            case t_DataCode: reference -> Set(dataCode); break;
            case t_DataAtom: reference -> Set(dataAtom); break;
            case t_DataFloatVector: reference -> Set(dataFloatVector); break;
            case t_DataIntVector: reference -> Set(dataIntVector); break;
            case t_DataBoolVector: reference -> Set(dataBoolVector); break;
            case t_DataStringVector: reference -> Set(dataStringVector); break;
            case t_DataList: reference -> Set(dataList); break;
        }
        return true;
    }
    return false;
}


