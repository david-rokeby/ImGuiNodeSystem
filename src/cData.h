//
//  cData.h
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-09.
//

#ifndef cData_h
#define cData_h

#include <vector>
#include <map>

enum T_NodeResult
{
    t_NodeNoChange,
    t_NodePropertyChange,
    t_NodeTrigger
};


enum T_DataType
{
    t_DataNone,
    t_DataFloat,
    t_DataInt,
    t_DataBool,
    t_DataString,
    t_DataFloatVector,
    t_DataIntVector,
    t_DataBoolVector,
    t_DataFloatMatrix,
    t_DataIntegerMatrix,
    t_DataBoolMatrix,
    t_DataFloatBlock,
    t_DataIntegerBlock,
    t_DataBoolBlock,
    t_DataStringVector,
    t_DataList,
    t_DataAtom,
    t_DataCode
};

enum T_Code
{
    t_CodeNothing,
    t_CodeBang
};

enum T_PropertyType
{
    t_PropertyBool,
    t_PropertyInt,
    t_PropertyCode,
    t_PropertyFloat,
    t_PropertyBoolVector,
    t_PropertyIntVector,
    t_PropertyFloatVector,
    t_PropertyPosition2,
    t_PropertyPosition3,
    t_PropertyQuaternion,
    t_PropertyColour3,
    t_PropertyColour4,
    t_PropertyString,
    t_PropertyAtom,
    t_PropertyList,
    t_PropertyStringVector,
    t_PropertyNone
};

class cAtom;
class cVariableReferenceBase;

int safe_stoi(std::string inString);
float safe_stof(std::string inString);
void ConvertStringToAtom(std::string& inString, cAtom& outAtom);

#define FloatToInt(a) static_cast<int>((a))
#define FloatToBool(a) ((a) != 0.0f)
#define IntToFloat(a) static_cast<float>((a))
#define IntToBool(a) ((a) != 0)
#define BoolToFloat(a) ((a) ? 1.0f : 0.0f)
#define BoolToInt(a) ((a) ? 1 : 0)
#define StringToFloat(a) safe_stof((a))
#define StringToInt(a) safe_stoi((a))
#define StringToBool(a) ((a) == "true" ? true : false)
#define FloatToString(a) (std::to_string((a))
#define IntToString(a) (std::to_string((a))
#define BoolToString(a) ((a) ? "true" : "false")

class cAtom
{
public:
    std::string dataString;
    float dataFloat = 0.0;
    int dataInt = 0;
    T_Code dataCode = t_CodeNothing;
    bool dataBool = false;
    T_DataType type = t_DataNone;
    
    template<class T>
    cAtom(T inVal){ Reset(); Set(inVal);}
    cAtom(void){ Reset();}
    void Reset(void){dataString = ""; dataFloat = 0.0; dataInt = 0; dataBool = false;}
    void Set(float input){dataFloat = input; type = t_DataFloat;}
    void Set(int input){dataInt = input; type = t_DataInt;}
    void Set(bool input){dataBool = input; type = t_DataBool;}
    void Set(std::string input){dataString = input; type = t_DataString;}
    void Set(void){type = t_DataNone;}
    void Set(T_Code input){dataCode = input; type = t_DataCode;}

    float GetAsFloat(void)
    {
        switch(type)
        {
            case t_DataFloat: return dataFloat;
            case t_DataInt: return static_cast<float>(dataInt);
            case t_DataCode: return static_cast<float>(dataCode);
            case t_DataBool: return dataBool ? 1.0f : 0.0f;
            case t_DataString: return safe_stof(dataString);
        }
        return 0.0f;
    }

    int GetAsInt(void)
    {
        switch(type)
        {
            case t_DataFloat: return static_cast<int>(dataFloat);
            case t_DataInt: return dataInt;
            case t_DataCode: return static_cast<int>(dataCode);
            case t_DataBool: return (dataInt != 0);
            case t_DataString: return safe_stoi(dataString);
        }
        return 0;
    }

    T_Code GetAsCode(void);

    bool GetAsBool(void)
    {
        switch(type)
        {
            case t_DataFloat: return (dataFloat != 0.0f);
            case t_DataInt: return (dataInt != 0);
            case t_DataCode: return (dataCode != t_CodeNothing);
            case t_DataBool: return dataBool;
            case t_DataString: return dataString == "true";
        }
        return false;
    }

    std::string& GetAsString(void);
    
    bool Same(cAtom& other)
    {
        if(other.type == type)
        {
            switch(type)
            {
                case t_DataFloat: return (dataFloat == other.dataFloat);
                case t_DataInt: return (dataInt == other.dataInt);
                case t_DataBool: return (dataBool == other.dataBool);
                case t_DataCode: return (dataCode == other.dataCode);
                case t_DataString: return (dataString == other.dataString);
            }
        }
        return false;
    }
    
    void FloatToDataString(float inVal, int precision = 3, bool removeTrailingZeros = false)
    {
        dataString = "";
        AppendFloatToDataString(inVal, precision, removeTrailingZeros);
    }
    
    void AppendFloatToDataString(float inVal, int precision = 3, bool removeTrailingZeros = false)
    {
        char s[256];
        switch(precision)
        {
            case 1: sprintf(s, "%.1f", inVal); break;
            case 2: sprintf(s, "%.2f", inVal); break;
            default:
            case 3: sprintf(s, "%.3f", inVal); break;
            case 4: sprintf(s, "%.4f", inVal); break;
            case 5: sprintf(s, "%.5f", inVal); break;
            case 6: sprintf(s, "%.6f", inVal); break;
        }
        std::string str = s;
        if(removeTrailingZeros)
        {
            if(str.find('.') != std::string::npos)
            {
                // Remove trailing zeroes
                str = str.substr(0, str.find_last_not_of('0')+1);
                // If the decimal point is now the last character, remove that as well
                if(str.find('.') == str.size()-1)
                {
                    str = str.substr(0, str.size()-1);
                }
            }
        }
        dataString += str;
    }


};

template<class T> T_DataType GetDataType(T in);

template<class T> T_PropertyType GetPropertyType(T in);


class cData
{
public:
    static std::map<T_Code, std::string> codeBook;
    static std::map<std::string, T_Code> reverseCodeBook;
    std::string dataString;
    std::vector<float> dataFloatVector;
    std::vector<int> dataIntVector;
    std::vector<bool> dataBoolVector;
    std::vector<std::string> dataStringVector;
    std::vector<cAtom> dataList;
    float dataFloat = 0.0;
    int dataInt = 0;
    T_Code dataCode = t_CodeNothing;
    bool dataBool = false;
    cAtom dataAtom;
    T_DataType type = t_DataNone;
    int dims[3] = {1, 1, 1};
    int listIndex = 0;

    cData(void)
    {
    }

    static void RegisterCode( T_Code code, std::string& codeString)
    {
        codeBook[code] = codeString;
        reverseCodeBook[codeString] = code;
    }
    
    static void RegisterCodes(void)
    {
        std::string codeName = "nothing";
        RegisterCode(t_CodeNothing, codeName);
        codeName = "bang";
        RegisterCode(t_CodeBang, codeName);
    }

    virtual T_DataType GetType(void){ return t_DataNone; }

    virtual void Set(float input){ type = t_DataFloat; dataFloat = input;}
    virtual void Set(int input){ type = t_DataInt; dataInt = input;}
    virtual void Set(T_Code input){ type = t_DataCode; dataCode = input; }
    virtual void Set(bool input){ type = t_DataBool; dataBool = input;}
    virtual void Set(std::string input){ type = t_DataString; dataString = input;}
    virtual void Set(cAtom& input){ type = t_DataAtom; dataAtom = input; }

    virtual void Set(std::vector<float>& input){ type = t_DataFloatVector; dataFloatVector = input; }
    virtual void Set(std::vector<int>& input){ type = t_DataIntVector; dataIntVector = input; }
    virtual void Set(std::vector<bool>& input){ type = t_DataBoolVector; dataBoolVector = input; }
    virtual void Set(std::vector<std::string>& input){ type = t_DataStringVector; dataStringVector = input; }
    virtual void Set(std::vector<cAtom>& input){ type = t_DataList; dataList = input; }

    virtual void Set(cData *input)
    {
        switch(input -> type)
        {
            case t_DataFloat: Set(input -> GetAsFloat()); break;
            case t_DataInt: Set(input -> GetAsInt()); break;
            case t_DataCode: Set(input -> GetAsCode()); break;
            case t_DataBool: Set(input -> GetAsBool()); break;
            case t_DataString: Set(input -> GetAsString()); break;
            case t_DataFloatVector: Set(input -> GetAsFloatVector()); break;
            case t_DataIntVector: Set(input -> GetAsIntVector()); break;
            case t_DataBoolVector: Set(input -> GetAsBoolVector()); break;
            case t_DataStringVector: Set(input -> GetAsStringVector()); break;
            case t_DataList: Set(input -> GetAsList()); break;
            case t_DataAtom: Set(input -> GetAsAtom()); break;
            case t_DataNone: type = t_DataNone; break;
        }
    }
    
    virtual void StartList(void){ dataList.clear(); dataFloatVector.clear(); dataIntVector.clear(); dataBoolVector.clear(); dataStringVector.clear(); type = t_DataList; }
    
    template<class T>
    void AddToList(T input){ cAtom newAtom; newAtom.Set(input); dataList.push_back(newAtom);}

    void GoToStartOfList(void){ listIndex = 0;}
    cAtom* GetNextListElement(void)
    {
        if( listIndex >= dataList.size())
            return nullptr;
        return &dataList[listIndex ++];
    }
    
    virtual float GetAsFloat(void)
    {
        switch(type)
        {
            case t_DataFloat: return dataFloat;
            case t_DataInt: return static_cast<float>(dataInt);
            case t_DataBool: return dataBool ? 1.0f : 0.0f;
            case t_DataCode: return static_cast<float>(dataCode);
            case t_DataString: return safe_stof(dataString);
            case t_DataFloatVector: if(dataFloatVector.size() > 0) return dataFloatVector[0]; return 0.0;
            case t_DataIntVector: if(dataIntVector.size() > 0) return static_cast<float>(dataIntVector[0]); return 0.0;
            case t_DataBoolVector: if(dataBoolVector.size() > 0) return dataBoolVector[0] ? 1.0f : 0.0f; return 0.0;
            case t_DataStringVector: if(dataStringVector.size() > 0) return safe_stof(dataStringVector[0]); return 0.0;
            case t_DataList:
                if(dataList.size() > 0)
                {
                    cAtom *atom = &dataList[0];
                    return atom -> GetAsFloat();
                }
                break;
            case t_DataAtom:
                return dataAtom.GetAsFloat();
        }
        return 0.0f;
    }
 
    virtual int GetAsInt(void)
    {
        switch(type)
        {
            case t_DataFloat: return static_cast<int>(dataFloat);
            case t_DataInt: return dataInt;
            case t_DataCode: return static_cast<int>(dataCode);
            case t_DataBool: return dataBool ? 1 : 0;
            case t_DataString:
            {
                int val = 0;
                try
                {
                    val = safe_stoi(dataString);
                }
                catch(...){}
                return val;
            }
            case t_DataFloatVector: if(dataFloatVector.size() > 0) return static_cast<int>(dataFloatVector[0]); return 0;
            case t_DataIntVector: if(dataIntVector.size() > 0) return dataIntVector[0]; return 0;
            case t_DataBoolVector: if(dataBoolVector.size() > 0) return dataBoolVector[0] ? 1 : 0; return 0;
            case t_DataStringVector: if(dataStringVector.size() > 0) return safe_stoi(dataStringVector[0]); return 0;
            case t_DataList:
                if(dataList.size() > 0)
                {
                    cAtom *atom = &dataList[0];
                    return atom -> GetAsInt();
                }
                break;
            case t_DataAtom:
                return dataAtom.GetAsInt();
        }
        return 0;
    }

    static T_Code FloatAsCode(float value){ T_Code code = static_cast<T_Code>(value);if( cData::codeBook.count(code) > 0) return code; return t_CodeNothing;}
    static T_Code IntAsCode(int value){ T_Code code = static_cast<T_Code>(value);if( cData::codeBook.count(code) > 0) return code; return t_CodeNothing;}
    static T_Code BoolAsCode(bool value){ return value ? t_CodeBang : t_CodeNothing;}
    static T_Code StringAsCode(std::string value){ if( cData::reverseCodeBook.count(value) > 0 ) return cData::reverseCodeBook[value]; return t_CodeNothing;}
    static std::string* CodeAsString(T_Code code){ if( cData::codeBook.count(code) > 0 ) return &cData::codeBook[code]; return nullptr;}

    virtual T_Code GetAsCode(void)
    {
        T_Code code = t_CodeNothing;
        
        switch(type)
        {
            case t_DataFloat: return FloatAsCode(dataFloat);
            case t_DataInt: return IntAsCode(dataInt);
            case t_DataCode: return dataCode;
            case t_DataBool: return BoolAsCode(dataBool);
            case t_DataString: return StringAsCode(dataString);
                
            case t_DataFloatVector:
                if(dataFloatVector.size() > 0)
                    return FloatAsCode(dataFloatVector[0]);
                return t_CodeNothing;
                
            case t_DataIntVector:
                if(dataIntVector.size() > 0)
                    return IntAsCode(dataIntVector[0]);
                return t_CodeNothing;

            case t_DataBoolVector:
                if(dataBoolVector.size() > 0)
                    return BoolAsCode(dataBool);
                return t_CodeNothing;
                
            case t_DataStringVector:
                if( dataStringVector.size() == 0 )
                    return StringAsCode(dataString);
                return t_CodeNothing;

            case t_DataList:
                if(dataList.size() > 0)
                {
                    cAtom *atom = &dataList[0];
                    return atom -> GetAsCode();
                }
                break;
                
            case t_DataAtom:
                return dataAtom.GetAsCode();
        }
        return t_CodeNothing;
    }

    virtual bool GetAsBool(void)
    {
        switch(type)
        {
            case t_DataFloat: return (dataFloat != 0.0);
            case t_DataInt: return (dataInt != 0);
            case t_DataBool: return dataBool;
            case t_DataCode: return (dataCode != t_CodeNothing);
            case t_DataString: return (dataString == "true");
            case t_DataFloatVector: if(dataFloatVector.size() > 0) return (dataFloatVector[0] != 0.0); return false;
            case t_DataIntVector: if(dataIntVector.size() > 0) return (dataIntVector[0] != 0); return false;
            case t_DataBoolVector: if(dataBoolVector.size() > 0) return dataBoolVector[0]; return false;
            case t_DataStringVector: if(dataStringVector.size() > 0) return (dataStringVector[0] == "true"); return false;
            case t_DataList:
                if(dataList.size() > 0)
                {
                    cAtom *atom = &dataList[0];
                    return atom -> GetAsBool();
                }
                break;
            case t_DataAtom:
                return dataAtom.GetAsBool();
        }
        return false;
    }

    virtual std::string& GetAsString(void)
    {
        switch(type)
        {
            case t_DataFloat: FloatToDataString(dataFloat); return dataString;
            case t_DataInt: dataString = std::to_string(dataInt); return dataString;
            case t_DataBool: dataString = dataBool ? "true" : "false"; return dataString;
            case t_DataString: return dataString;
            case t_DataCode:
            {
                std::string* codeString = CodeAsString(dataCode);
                if( codeString)
                    dataString = *codeString;
                else
                    dataString = "";
                return dataString;
            }
                
            case t_DataFloatVector:
                dataString.clear();
                for(int i = 0; i < dataFloatVector.size(); i ++)
                {
                    float b = dataFloatVector[i];
                    AppendFloatToDataString(b, 3, true);
                    if(i < dataList.size() - 1)
                        dataString += " ";
                }
                return dataString;
                
            case t_DataIntVector:
                dataString.clear();
                for(int i = 0; i < dataIntVector.size(); i ++)
                {
                    int b = dataIntVector[i];
                    dataString += std::to_string(b);
                    if(i < dataList.size() - 1)
                        dataString += " ";
                }
                return dataString;
                
            case t_DataBoolVector:
                dataString.clear();
                for(int i = 0; i < dataBoolVector.size(); i ++)
                {
                    bool b = dataBoolVector[i];
                    dataString += b ? "true" : "false";
                    if(i < dataList.size() - 1)
                        dataString += " ";
                }
                return dataString;
                
            case t_DataStringVector:
                dataString.clear();
                for(int i = 0; i < dataStringVector.size(); i ++)
                {
                    std::string& s = dataStringVector[i];
                    dataString += s;
                    if(i < dataList.size() - 1)
                        dataString += " ";
                }
                return dataString;
                
            case t_DataAtom: return dataAtom.GetAsString();
                
            case t_DataList:
                dataString.clear();
                for(int i = 0; i < dataList.size(); i ++)
                {
                    cAtom *atom = &dataList[i];
                    dataString += atom -> GetAsString();
                    if(i < dataList.size() - 1)
                        dataString += " ";
                }
                return dataString;
                break;
        }
        dataString = "";
        return dataString;
    }

    virtual cAtom& GetAsAtom(void)
    {
        switch(type)
        {
            case t_DataFloat: dataAtom.Set(dataFloat); return dataAtom;
            case t_DataInt: dataAtom.Set(dataInt); return dataAtom;
            case t_DataCode: dataAtom.Set(dataCode); return dataAtom;
            case t_DataBool: dataAtom.Set(dataBool); return dataAtom;
            case t_DataString: dataAtom.Set(dataString); return dataAtom;
            case t_DataFloatVector: dataAtom.Set(GetAsString()); return dataAtom;
            case t_DataIntVector: dataAtom.Set(GetAsString()); return dataAtom;
            case t_DataBoolVector: dataAtom.Set(GetAsString()); return dataAtom;
            case t_DataStringVector: dataAtom.Set(GetAsString()); return dataAtom;
            case t_DataList: dataAtom.Set(GetAsString()); return dataAtom;
            case t_DataAtom: return dataAtom;
        }
        dataAtom.Set();
        return dataAtom;
    }
    
    virtual std::vector<float>& GetAsFloatVector(void)
    {
        switch(type)
        {
            case t_DataFloat: dataFloatVector.assign(1, dataFloat); return dataFloatVector;
            case t_DataInt: dataFloatVector.assign(1, dataInt); return dataFloatVector;
            case t_DataCode: dataFloatVector.assign(1, dataCode); return dataFloatVector;
            case t_DataBool: dataFloatVector.assign(1, dataBool); return dataFloatVector;
            case t_DataString: dataFloatVector.assign(1, safe_stof(dataString)); return dataFloatVector;
            
            case t_DataFloatVector: return dataFloatVector;
            case t_DataIntVector: dataFloatVector.assign(dataIntVector.begin(), dataIntVector.end()); return dataFloatVector;
            case t_DataBoolVector: dataFloatVector.assign(dataBoolVector.begin(), dataBoolVector.end()); return dataFloatVector;
            case t_DataStringVector:
                {
                    dataFloatVector.resize(dataStringVector.size());
                    for(int i = 0; i < dataStringVector.size(); i ++)
                    {
                        dataFloatVector[i] = safe_stof(dataStringVector[i]);
                    }
                return dataFloatVector;
                }
            case t_DataList:
                {
                    dataFloatVector.resize(dataList.size());
                    for(int i = 0; i < dataList.size(); i ++)
                        dataFloatVector[i] = dataList[i].GetAsFloat();
                return dataFloatVector;
                }
        }
        dataFloatVector.assign(1, 0.0);
        return dataFloatVector;
    }

    virtual std::vector<int>& GetAsIntVector(void)
    {
        switch(type)
        {
            case t_DataFloat: dataIntVector.assign(1, dataFloat); return dataIntVector;
            case t_DataInt: dataIntVector.assign(1, dataInt); return dataIntVector;
            case t_DataCode: dataIntVector.assign(1, dataCode); return dataIntVector;
            case t_DataBool: dataIntVector.assign(1, dataBool); return dataIntVector;
            case t_DataString: dataIntVector.assign(1, safe_stof(dataString)); return dataIntVector;
            
            case t_DataFloatVector: dataIntVector.assign(dataFloatVector.begin(), dataFloatVector.end()); return dataIntVector;
            case t_DataIntVector: return dataIntVector;
            case t_DataBoolVector: dataIntVector.assign(dataBoolVector.begin(), dataBoolVector.end()); return dataIntVector;
            case t_DataStringVector:
                {
                    dataIntVector.resize(dataStringVector.size());
                    for(int i = 0; i < dataStringVector.size(); i ++)
                    {
                        dataIntVector[i] = safe_stoi(dataStringVector[i]);
                    }
                return dataIntVector;
                }
            case t_DataList:
                {
                    dataIntVector.resize(dataList.size());
                    for(int i = 0; i < dataList.size(); i ++)
                        dataIntVector[i] = dataList[i].GetAsInt();
                return dataIntVector;
                }
        }
        dataIntVector.assign(1, 0);
        return dataIntVector;
    }

    virtual std::vector<bool>& GetAsBoolVector(void)
    {
        switch(type)
        {
            case t_DataFloat: dataBoolVector.assign(1, dataFloat); return dataBoolVector;
            case t_DataInt: dataBoolVector.assign(1, dataInt); return dataBoolVector;
            case t_DataCode: dataBoolVector.assign(1, (dataCode != t_CodeNothing)); return dataBoolVector;
            case t_DataBool: dataBoolVector.assign(1, dataBool); return dataBoolVector;
            case t_DataString: dataBoolVector.assign(1, dataString == "true"); return dataBoolVector;
            
            case t_DataFloatVector: dataBoolVector.assign(dataFloatVector.begin(), dataFloatVector.end()); return dataBoolVector;
            case t_DataIntVector: dataBoolVector.assign(dataIntVector.begin(), dataIntVector.end()); return dataBoolVector;
            case t_DataBoolVector: return dataBoolVector;
            case t_DataStringVector:
                {
                    dataBoolVector.resize(dataStringVector.size());
                    for(int i = 0; i < dataStringVector.size(); i ++)
                    {
                        dataBoolVector[i] = (dataStringVector[i] == "true");
                    }
                return dataBoolVector;
                }
            case t_DataList:
                {
                    dataBoolVector.resize(dataList.size());
                    for(int i = 0; i < dataList.size(); i ++)
                        dataBoolVector[i] = dataList[i].GetAsBool();
                return dataBoolVector;
                }
        }
        dataBoolVector.assign(1, false);
        return dataBoolVector;
    }

    virtual std::vector<std::string>& GetAsStringVector(void)
    {
        switch(type)
        {
            case t_DataFloat:
                FloatToDataStringVector(1, dataFloat, 3, true);
                return dataStringVector;
                
            case t_DataInt: dataStringVector.assign(1, std::to_string(dataInt)); return dataStringVector;
            case t_DataCode:
            {
                std::string *codeString = cData::CodeAsString(dataCode);
                if(codeString)
                    dataStringVector.assign(1, *codeString);
                else
                    dataStringVector.assign(1, std::string(""));
                return dataStringVector;
            }
            case t_DataBool: dataStringVector.assign(1, dataBool ? "true" : "false"); return dataStringVector;
            case t_DataString: dataStringVector.assign(1, dataString); return dataStringVector;
            
            case t_DataFloatVector:
            {
                dataStringVector.resize(dataFloatVector.size());
                for(int i = 0; i < dataFloatVector.size(); i ++)
                {
                    FloatToDataStringVector(i, dataFloatVector[i], 3, true);
                }
                return dataStringVector;
            }
                
            case t_DataIntVector:
            {
                dataStringVector.resize(dataIntVector.size());
                for(int i = 0; i < dataIntVector.size(); i ++)
                {
                    dataStringVector[i] = std::to_string(dataIntVector[i]);
                }
                return dataStringVector;
            }
                
            case t_DataBoolVector:
            {
                dataStringVector.resize(dataBoolVector.size());
                for(int i = 0; i < dataBoolVector.size(); i ++)
                {
                    dataStringVector[i] = (dataBoolVector[i]) ? "true" : "false";
                }
                return dataStringVector;
            }
                
            case t_DataStringVector: return dataStringVector;
            
            case t_DataList:
                {
                    dataStringVector.resize(dataList.size());
                    for(int i = 0; i < dataList.size(); i ++)
                        dataStringVector[i] = dataList[i].GetAsString();
                return dataStringVector;
                }
        }
        dataStringVector.assign(1, "");
        return dataStringVector;
    }
    
    virtual std::vector<cAtom>& GetAsList(void)
    {
        switch(type)
        {
            case t_DataFloat: StartList(); AddToList(dataFloat); return dataList;
            case t_DataInt: StartList(); AddToList(dataInt); return dataList;
            case t_DataCode: StartList(); AddToList(dataCode); return dataList;
            case t_DataBool: StartList(); AddToList(dataBool); return dataList;
            case t_DataString: StartList(); AddToList(dataString); return dataList;
            
            case t_DataFloatVector:
            {
                dataList.clear();
                listIndex = 0;
                dataList.reserve(dataFloatVector.size());
                for(int i = 0; i < dataFloatVector.size(); i ++)
                    AddToList(dataFloatVector[i]);
                return dataList;
            }
                
            case t_DataIntVector:
            {
                dataList.clear();
                listIndex = 0;
                dataList.reserve(dataIntVector.size());
                for(int i = 0; i < dataIntVector.size(); i ++)
                    AddToList(dataIntVector[i]);
                 return dataList;
            }
                
            case t_DataBoolVector:
            {
                dataList.clear();
                listIndex = 0;
                dataList.reserve(dataBoolVector.size());
                for(int i = 0; i < dataBoolVector.size(); i ++)
                    AddToList(dataBoolVector[i]);
                 return dataList;
            }

            case t_DataStringVector:
            {
                dataList.clear();
                listIndex = 0;
                dataList.reserve(dataStringVector.size());
                for(int i = 0; i < dataStringVector.size(); i ++)
                    AddToList(dataStringVector[i]);
                 return dataList;
            }

            case t_DataList: return dataList;
        }
        dataList.clear();
        listIndex = 0;
        return dataList;
    }
    
    bool Same(cData& other)
    {
        if(other.type == type)
        {
            switch(type)
            {
                case t_DataFloat: return (dataFloat == other.dataFloat);
                case t_DataInt: return (dataInt == other.dataInt);
                case t_DataBool: return (dataBool == other.dataBool);
                case t_DataCode: return (dataCode == other.dataCode);
                case t_DataString: return (dataString == other.dataString);
                case t_DataFloatVector: return (dataFloatVector == other.dataFloatVector);
                case t_DataIntVector: return (dataIntVector == other.dataIntVector);
                case t_DataBoolVector: return (dataBoolVector == other.dataBoolVector);
                case t_DataStringVector: return (dataStringVector == other.dataStringVector);
//                case t_DataList: return (dataList == other.dataList);
            }
        }
        return false;
    }
    
    bool Same(float other){return(type == t_DataFloat && dataFloat == other);}
    bool Same(int other){return(type == t_DataInt && dataInt == other);}
    bool Same(bool other){return(type == t_DataBool && dataBool == other);}
    bool Same(T_Code other){return(type == t_DataCode && dataCode == other);}
    bool Same(std::string& other){return(type == t_DataString && dataString == other);}
    bool Same(std::vector<float>& other){return(type == t_DataFloatVector && dataFloatVector == other);}
    bool Same(std::vector<int>& other){return(type == t_DataIntVector && dataIntVector == other);}
    bool Same(std::vector<bool>& other){return(type == t_DataBoolVector && dataBoolVector == other);}
    bool Same(std::vector<std::string>& other){return(type == t_DataStringVector && dataStringVector == other);}
//    bool Same(std::vector<cAtom>& other){return(type == t_DataList && dataList == other);}
    
    bool FillReference(cVariableReferenceBase *reference);
    
    void FloatToDataString(float inVal, int precision = 3, bool removeTrailingZeros = false)
    {
        dataString = "";
        AppendFloatToDataString(inVal, precision, removeTrailingZeros);
    }
    
    void AppendFloatToDataString(float inVal, int precision = 3, bool removeTrailingZeros = false)
    {
        char s[256];
        switch(precision)
        {
            case 1: sprintf(s, "%.1f", inVal); break;
            case 2: sprintf(s, "%.2f", inVal); break;
            default:
            case 3: sprintf(s, "%.3f", inVal); break;
            case 4: sprintf(s, "%.4f", inVal); break;
            case 5: sprintf(s, "%.5f", inVal); break;
            case 6: sprintf(s, "%.6f", inVal); break;
        }
        std::string str = s;
        if(removeTrailingZeros)
        {
            if(str.find('.') != std::string::npos)
            {
                // Remove trailing zeroes
                str = str.substr(0, str.find_last_not_of('0')+1);
                // If the decimal point is now the last character, remove that as well
                if(str.find('.') == str.size()-1)
                {
                    str = str.substr(0, str.size()-1);
                }
            }
        }
        dataString += str;
    }

    void FloatToDataStringVector(int index, float inVal, int precision = 3, bool removeTrailingZeros = false)
    {
        char s[256];
        switch(precision)
        {
            case 1: sprintf(s, "%.1f", inVal); break;
            case 2: sprintf(s, "%.2f", inVal); break;
            default:
            case 3: sprintf(s, "%.3f", inVal); break;
            case 4: sprintf(s, "%.4f", inVal); break;
            case 5: sprintf(s, "%.5f", inVal); break;
            case 6: sprintf(s, "%.6f", inVal); break;
        }
        std::string str = s;
        if(removeTrailingZeros)
        {
            if(str.find('.') != std::string::npos)
            {
                // Remove trailing zeroes
                str = str.substr(0, str.find_last_not_of('0')+1);
                // If the decimal point is now the last character, remove that as well
                if(str.find('.') == str.size()-1)
                {
                    str = str.substr(0, str.size()-1);
                }
            }
        }
        if(index >= dataStringVector.size())
            dataStringVector.resize(index + 1);
        dataStringVector[index] = str;
    }

};

#endif /* cData_h */
