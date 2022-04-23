//
//  cProperty.h
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-09.
//

#ifndef cGUIProperty_h
#define cGUIProperty_h

#include "JSON_utilities.h"
#include "imgui_stdlib.h"
#include "my_widgets.hpp"
#include "cData.h"

enum T_WidgetType
{
    t_WidgetNone,
    t_WidgetDrag,
    t_WidgetDragFloat,
    t_WidgetDragInt,
    t_WidgetSlider,
    t_WidgetSliderFloat,
    t_WidgetSliderInt,
    t_WidgetInput,
    t_WidgetInputFloat,
    t_WidgetInputInt,
    t_WidgetCheckBox,
    t_WidgetToggle,
    t_WidgetCombo,
    t_WidgetButton,
    t_WidgetSeparator,
    t_WidgetPopUp,
    t_WidgetTextField,
    t_WidgetRadioButtons,
    t_WidgetLabel,
    t_WidgetColorPicker,
    t_WidgetColorEdit
};

//?????
struct InputTextCallback_UserData
{
    std::string*            Str;
    ImGuiInputTextCallback  ChainCallback;
    void*                   ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}


class cGUIPropertyStub
{
public:
    bool hasWidget = false;
    std::string label;
    T_WidgetType widgetType = t_WidgetNone;
    bool showWidget = true;
    std::string format = "%.3f";
    ImGuiID guiID;
    int32_t widget_uuid;
    int32_t widget_width = 120;
    bool useInternalStorage;
    std::vector<std::string> comboOptionsVector;
    const char **comboOptions;
    int comboOptionCount = 0;
    T_PropertyType dataType = t_PropertyNone;
    bool sameLine = false;
    bool radioButtonsSameLine = true;

    virtual void InitProperty(std::string name, T_WidgetType type, bool show)
    {
        label = name;
        SetWidgetType(type);
        showWidget = show;
    }
    
    virtual void SetWidgetType(T_WidgetType inType){}
    virtual T_NodeResult Draw(void){}
    
    virtual bool Save(Json::Value& propertyContainer){}
    virtual bool Save(bool, Json::Value& propertyContainer){}
    virtual bool Save(int, Json::Value& propertyContainer){}
    virtual bool Save(float, Json::Value& propertyContainer){}
    virtual bool Save(std::string&, Json::Value& propertyContainer){}
    virtual bool Save(cAtom&, Json::Value& propertyContainer){}
    
    virtual bool Load(const Json::Value& propertyContainer){}
    virtual void LoadIntFromJSON(const Json::Value& propertyContainer){}
    virtual void LoadFloatFromJSON(const Json::Value& propertyContainer){}
    virtual void LoadBoolFromJSON(const Json::Value& propertyContainer){}
    virtual void LoadStringFromJSON(const Json::Value& propertyContainer){}
    virtual void LoadFloatVectorFromJSON(const Json::Value& propertyContainer){}

    virtual void SetData(std::string inValue){}
    virtual void SetData(float inValue){}
    virtual void SetData(int inValue){}
    virtual void SetData(bool inValue){}
    virtual void SetData(cAtom inValue){}
    virtual void SetDataRef(cAtom& inValue){}

    virtual void SetComboOptions( std::vector<std::string> opts ){}
    virtual void SetRadioButtonOptions( std::vector<std::string> opts ){}
};


class cGUIPropertyBase : public cGUIPropertyStub
{
public:
    float speed = 1.0f;

    virtual void InitDataAndType(void){};
    void InitProperty(std::string name, T_WidgetType type, bool show)
    {
        label = name;
        showWidget = show;
        UseInternalStorage();
        InitDataAndType();
        SetWidgetType(type);
    }

    virtual void SetNoMaxMin(void){}
    virtual bool MaxMin(void){return false;}
    
    virtual void UseInternalStorage(void){}
    
    void SetWidgetType(T_WidgetType inType)
    {
        hasWidget = true;
        switch(dataType)
        {
            case t_PropertyFloat:
                SetNoMaxMin();
//                min = max = 0.0;
                speed = 0.1;
                widget_width = 80;
                format = "%.3f";
                
                switch(inType)
                {
                    case t_WidgetDrag:
                    case t_WidgetDragFloat:
                        widgetType = t_WidgetDragFloat;
                        break;
                        
                    case t_WidgetSlider:
                    case t_WidgetSliderFloat:
                        widgetType = t_WidgetSliderFloat;
                        break;
                        
                    case t_WidgetInput:
                    case t_WidgetInputFloat:
                        widgetType = t_WidgetInputFloat;
                        widget_width = 100;
                        break;
                }
                break;
        
            case t_PropertyInt:
                SetNoMaxMin();
//                min = max = (int)0;
                speed = 1;
                widget_width = 80;
                format = "%d";

                switch(inType)
                {
                    case t_WidgetDrag:
                    case t_WidgetDragInt:
                        widgetType = t_WidgetDragInt;
                        break;
                        
                    case t_WidgetSlider:
                    case t_WidgetSliderInt:
                        widgetType = t_WidgetSliderInt;
                        break;
                        
                    case t_WidgetInput:
                    case t_WidgetInputInt:
                        widgetType = t_WidgetInputInt;
                        widget_width = 100;
                        break;
                    
                    case t_WidgetCombo:
                        widgetType = t_WidgetCombo;
                        widget_width = 100;
                        break;
                        
                    case t_WidgetRadioButtons:
                        widgetType = t_WidgetRadioButtons;
                        widget_width = 120;
                        break;
                }
                break;
                
            case t_PropertyBool:
                switch(inType)
                {
                    case t_WidgetToggle:
                        widgetType = t_WidgetToggle;
                        break;
                        
                    case t_WidgetCheckBox:
                    default:
                        widgetType = t_WidgetCheckBox;
                        break;
                }
                break;
                
            case t_PropertyString:
            case t_PropertyAtom:
                widgetType = t_WidgetTextField;
                break;
                
            case t_PropertyFloatVector:
                widgetType = t_WidgetNone;
                switch(inType)
                {
                    case t_WidgetColorPicker:
                        widgetType = t_WidgetColorPicker;
                        break;
                }
                break;
        }
    }
    
    virtual void SetRadioOptions( std::vector<std::string> opts, bool sameLine = true ){SetComboOptions(opts); radioButtonsSameLine = sameLine; }
    
    virtual void SetComboOptions( std::vector<std::string> opts )
    {
        comboOptionsVector = opts;
        comboOptions = new const char *[opts.size()];
        for(int i = 0; i < opts.size(); i ++)
            comboOptions[i] = comboOptionsVector[i].c_str();
    }
    
    float AsFloat(float value){ return value;}
    float AsFloat(int value){ return (float)value;}
    float AsFloat(bool value){ return (float)value;}
    float AsFloat(std::string& value){ return 0.0f;}
    float AsFloat(cAtom& value){ return value.GetAsFloat();}
    
    int AsInt(int value){ return value;}
    int AsInt(float value){ return (int)value;}
    int AsInt(bool value){ return (int)value;}
    int AsInt(std::string value){ return 0;}
    int AsInt(cAtom& value){ return value.GetAsInt();}
    
    bool AsBool(int value){ return (value != 0);}
    bool AsBool(float value){ return (value != 0.0);}
    bool AsBool(bool value){ return value;}
    bool AsBool(std::string value){ return false;}
    bool AsBool(cAtom& value){ return value.GetAsBool();}

    std::string* GetStringPointer(float* inFloat){ return nullptr; }
    std::string* GetStringPointer(int* inInt){ return nullptr; }
    std::string* GetStringPointer(bool* inInt){ return nullptr; }
    std::string* GetStringPointer(std::string* inString){ return inString; }
    std::string* GetStringPointer(cAtom* inAtom){ inAtom -> type = t_DataString; return &inAtom -> dataString; }
    void ConvertToString(float* inFloat){}
    void ConvertToString(int* inInt){}
    void ConvertToString(bool* inInt){}
    void ConvertToString(std::string* inString){}
    void ConvertToString(cAtom* inAtom){ inAtom -> GetAsString(); inAtom -> type = t_DataString;}

    virtual bool DrawDragFloat(void){return false;}
    virtual bool DrawDragInt(void){return false;}
    virtual bool DrawSliderFloat(void){return false;}
    virtual bool DrawSliderInt(void){return false;}
    virtual bool DrawInputFloat(void){return false;}
    virtual bool DrawInputInt(void){return false;}
    virtual bool DrawCheckBox(void){return false;}
    virtual bool DrawToggle(void){return false;}
    virtual bool DrawCombo(void){return false;}
    virtual bool DrawRadioButton(int i){return false;}
    virtual bool DrawTextField(void){return false;}
    virtual bool DrawColorPicker(void){return false;}
    virtual bool DrawColorEdit(void){return false;}

    bool CommittedChange(bool changed, bool checkMouse = false)
    {
        if(changed && checkMouse)
        {
            if(!ImGui::IsMouseDown(ImGuiMouseButton_Left))
                changed = false;
        }
        else if(ImGui::IsItemDeactivatedAfterEdit())
            changed = true;
        return changed;
    }
    
    virtual T_NodeResult Draw(void)
    {
        T_NodeResult result = t_NodeNoChange;
        bool changed = false;
        
        if( showWidget)
        {
            ImGui::PushID(widget_uuid);
            if(sameLine)
                ImGui::SameLine();
            ImGui::PushItemWidth(widget_width);
            guiID = ImGui::GetID(label.c_str());
            
            switch(widgetType)
            {
                case t_WidgetDragFloat:
                    if (dataType == t_PropertyFloat)
                    {
                        changed = DrawDragFloat();
                        changed = CommittedChange(changed, true);
                    }
                    break;
                    
                case t_WidgetDragInt:
                    if (dataType == t_PropertyInt)
                        changed = DrawDragInt();
                    changed = CommittedChange(changed, true);
                    break;
                    
                case t_WidgetSliderFloat:
                    if (dataType == t_PropertyFloat)
                        changed = DrawSliderFloat();
                    changed = CommittedChange(changed, true);
                    break;
                    
                case t_WidgetSliderInt:
                    if (dataType == t_PropertyInt)
                        changed = DrawSliderInt();
                    changed = CommittedChange(changed, true);
                    break;

                case t_WidgetInputFloat:
                    if (dataType == t_PropertyFloat)
                        changed = DrawInputFloat();
                    changed = CommittedChange(changed);
                    break;
                    
                case t_WidgetInputInt:
                    if (dataType == t_PropertyInt)
                        changed = DrawInputInt();
                    changed = CommittedChange(changed);
                    break;
                
                case t_WidgetCheckBox:
                    if (dataType == t_PropertyBool)
                        changed = DrawCheckBox();
                    break;

                case t_WidgetToggle:
                    if (dataType == t_PropertyBool)
                        changed = DrawToggle();
                    break;
                    
                case t_WidgetCombo:
                    if (dataType == t_PropertyInt)
                        changed = DrawCombo();
                    break;
                
                case t_WidgetRadioButtons:
                    if (dataType == t_PropertyInt)
                    {
                        for(int i = 0; i < comboOptionsVector.size(); i ++)
                        {
                            if(i > 0 && radioButtonsSameLine)
                                ImGui::SameLine();
                            changed = DrawRadioButton(i);
                        }
                    }
                    break;
                    
                case t_WidgetTextField:
                    changed = DrawTextField();
                    changed = CommittedChange(changed, false);
                    break;
                    
                case t_WidgetColorPicker:
                    changed = DrawColorPicker();
                    break;
                    
                case t_WidgetColorEdit:
                    changed = DrawColorEdit();
                    break;
            }
            
            ClampData();
            if(changed)
                result = t_NodePropertyChange;

            ImGui::PopItemWidth();
            ImGuiID dd = ImGui::GetItemID();
            ImGui::PopID();
        }
        return result;
    }

    virtual void ClampData(void){}
    
    virtual void LoadIntFromJSON(const Json::Value& propertyContainer)
    {
        int loadedInt = 0;
        if(IntFromJSON(propertyContainer, "value", loadedInt))
            SetData(loadedInt);
    }
    
    virtual void LoadBoolFromJSON(const Json::Value& propertyContainer)
    {
        bool loadedBool = false;
        if(BoolFromJSON(propertyContainer, "value", loadedBool))
            SetData(loadedBool);
    }
    
    virtual void LoadStringFromJSON(const Json::Value& propertyContainer)
    {
        std::string loadedString = "";
        if(StringFromJSON(propertyContainer, "value", loadedString))
            SetData(loadedString);
    }
    
    virtual void LoadFloatFromJSON(const Json::Value& propertyContainer)
    {
        float loadedFloat = 0.0f;
        if(FloatFromJSON(propertyContainer, "value", loadedFloat))
            SetData(loadedFloat);
    }
    
    
    virtual bool Load(const Json::Value& propertyContainer)
    {
        switch(dataType)
        {
            case t_PropertyBool:
            {
                bool loadedBool = false;
                if(BoolFromJSON(propertyContainer, "value", loadedBool))
                    SetData(loadedBool);
                break;
            }
            
            case t_PropertyInt:
            {
                LoadIntFromJSON(propertyContainer);
                break;
            }
                
            case t_PropertyFloat:
            {
                LoadFloatFromJSON(propertyContainer);
                break;
            }
                
            case t_PropertyFloatVector:
            {
                LoadFloatVectorFromJSON(propertyContainer);
                break;
            }
                
            case t_PropertyString:
            {
                LoadStringFromJSON(propertyContainer);
                break;
            }

            case t_PropertyAtom:
            {
                cAtom loadedAtom;
                std::string loadedString;
                if(StringFromJSON(propertyContainer, "value", loadedString))
                {
                    loadedAtom.Set(loadedString);
                    SetDataRef(loadedAtom);
                }
                
/*                loadedAtom.Set()
                if(StringFromJSON(propertyContainer, "value", loadedAtom))
                {
                    SetData(loadedString);
                }*/
                break;
            }
            
        }
        return true;
    }
};

template <class T>
class cGUIProperty : public cGUIPropertyBase
{
public:
    T internalStorage;
    T *dataPtr;
    
    virtual void InitDataAndType(void)
    {
        dataType = GetPropertyType(internalStorage);
    }

    virtual void UseInternalStorage(void)
    {
        dataPtr = &internalStorage;
        useInternalStorage = true;
    }
    
    virtual void SetStorage(T *externalStorage)
    {
        dataPtr = externalStorage;
        useInternalStorage = false;
    }

/*    virtual void SetAtomData(cAtom *inValuePtr)
    {
        if( inValuePtr == nullptr) return;
        
        switch(inValuePtr -> type)
        {
            case t_DataString:
                
        }
    }*/
    virtual void SetData(T *inValuePtr){ SetData(*inValuePtr); }
    
    virtual void SetData(T inValue)
    {
        *dataPtr = inValue;
        ClampData();
    }
    
    virtual void SetDataRef(T& inValue)
    {
        *dataPtr = inValue;
        ClampData();
    }

    T& GetData(void){ return *dataPtr; }
    
    virtual bool Save(bool value, Json::Value& propertyContainer)
    {
        propertyContainer["value_type"] = "bool";
        propertyContainer["value"] = value;
        return true;
    }
    
    virtual bool Save(int value, Json::Value& propertyContainer)
    {
        propertyContainer["value_type"] = "int";
        propertyContainer["value"] = value;
        return true;
    }
    
    virtual bool Save(float value, Json::Value& propertyContainer)
    {
        propertyContainer["value_type"] = "float";
        propertyContainer["value"] = value;
        return true;
    }
    
    virtual bool Save(std::vector<float> floatVector, Json::Value& propertyContainer)
    {
        propertyContainer["value_type"] = "float_vector";
        Json::Value array;
        for(int i = 0; i < floatVector.size(); i ++)
            array.append(floatVector[i]);
        propertyContainer["value"] = array;
        return true;
    }
    
    virtual bool Save(std::vector<int> intVector, Json::Value& propertyContainer)
    {
        propertyContainer["value_type"] = "int_vector";
        Json::Value array;
        for(int i = 0; i < intVector.size(); i ++)
            array.append(intVector[i]);
        propertyContainer["value"] = array;
        return true;
    }
    
    virtual bool Save(std::vector<bool> boolVector, Json::Value& propertyContainer)
    {
        propertyContainer["value_type"] = "bool_vector";
        Json::Value array;
        for(int i = 0; i < boolVector.size(); i ++)
            array.append((int)boolVector[i]);
        propertyContainer["value"] = array;
        return true;
    }
    
    virtual bool Save(std::string& value, Json::Value& propertyContainer)
    {
        propertyContainer["value_type"] = "string";
        propertyContainer["value"] = value;
        return true;
    }
    
    virtual bool Save(cAtom& value, Json::Value& propertyContainer)
    {
        switch(value.type)
        {
            case t_DataFloat: return Save(value.GetAsFloat(), propertyContainer);
            case t_DataInt: return Save(value.GetAsInt(), propertyContainer);
            case t_DataCode: return Save(value.GetAsString(), propertyContainer);
            case t_DataBool: return Save(value.GetAsBool(), propertyContainer);
            case t_DataString: return Save(value.GetAsString(), propertyContainer);
        }
        return true;
    }
    
    bool DrawCheckBox(void){ return ImGui::Checkbox(label.c_str(), (bool *)dataPtr );}
    bool DrawToggle(void){ return ToggleButton(label.c_str(), (bool *)dataPtr );}
    
    bool DrawTextField(void)
    {
        bool changed = false;
        InputTextCallback_UserData userData;
        userData.ChainCallback = nullptr;
        userData.ChainCallbackUserData = nullptr;
//        ImGui::PushTextWrapPos(widget_width);
        switch(dataType)
        {
            case t_PropertyString:
                userData.Str = GetStringPointer(dataPtr);
                changed = ImGui::InputText(label.c_str(), GetStringPointer(dataPtr), ImGuiInputTextFlags_EnterReturnsTrue, InputTextCallback, &userData);
                break;
                
            case t_PropertyAtom:
                ConvertToString(dataPtr);
                userData.Str = GetStringPointer(dataPtr);
                changed = ImGui::InputText(label.c_str(), GetStringPointer(dataPtr), ImGuiInputTextFlags_EnterReturnsTrue, InputTextCallback, &userData);
                break;

        }
//        ImGui::PopTextWrapPos();
        
        return changed;
    }
    
    bool Save(Json::Value& propertyContainer)
    {
        propertyContainer["type"] = "property";
        propertyContainer["label"] = label;
        switch(dataType)
        {
            case t_PropertyBool: return Save(GetData(), propertyContainer); break;
            case t_PropertyInt: return Save(GetData(), propertyContainer); break;
            case t_PropertyFloat: return Save(GetData(), propertyContainer); break;
            case t_PropertyString: return Save(GetData(), propertyContainer); break;

            case t_PropertyAtom:
                return Save(GetData(), propertyContainer);
                break;
        }
        return true;
    }
};

template <>
class cGUIProperty<int> : public cGUIPropertyBase
{
public:
    int internalStorage;
    int *dataPtr;
    int min;
    int max;
    
    virtual void InitDataAndType(void)
    {
        SetData(0);
        dataType = t_PropertyInt;
    }


    int GetData(void){ return *dataPtr; }
    virtual void UseInternalStorage(void)
    {
        dataPtr = &internalStorage;
        useInternalStorage = true;
    }
    
    virtual void SetStorage(int *externalStorage)
    {
        dataPtr = externalStorage;
        useInternalStorage = false;
    }

    virtual void SetData(int *inValuePtr){ *dataPtr = *inValuePtr; ClampData(); }
    virtual void SetData(int inValue){ *dataPtr = inValue; ClampData(); }

    virtual void SetNoMaxMin(void){min = max = 0;}
    virtual bool MaxMin(void){ return (min != max); }
    virtual void SetMin(int inMin){ min = inMin; }
    virtual void SetMax(int inMax){ max = inMax; }
    virtual bool Save(int inValue, Json::Value& propertyContainer)
    {
        propertyContainer["value_type"] = "int";
        propertyContainer["value"] = inValue;
        propertyContainer["min"] = min;
        propertyContainer["max"] = max;
        return true;
    }
    
    virtual void ClampData(void)
    {
        switch(dataType)
        {
            case t_PropertyFloat:
            case t_PropertyInt:
                if(MaxMin())
                {
                    if(*dataPtr > max)
                        *dataPtr = max;
                    else if(*dataPtr < min)
                        *dataPtr = min;
                }
                break;
        }
    }

    virtual void LoadIntFromJSON(const Json::Value& propertyContainer)
    {
        int loadedInt = 0;
        if(IntFromJSON(propertyContainer, "value", loadedInt))
            SetData(loadedInt);
        if(IntFromJSON(propertyContainer, "min", loadedInt))
            SetMin(loadedInt);
        if(IntFromJSON(propertyContainer, "max", loadedInt))
            SetMax(loadedInt);
    }

    virtual bool DrawDragInt(void){ return ImGui::DragInt(label.c_str(), (int *)dataPtr, speed, min, max, format.c_str() );}
    virtual bool DrawSliderInt(void){ return ImGui::SliderInt(label.c_str(), (int *)dataPtr, min, max, format.c_str() );}
    virtual bool DrawInputInt(void){ return ImGui::InputInt(label.c_str(), (int *)dataPtr, speed, speed * 10, ImGuiInputTextFlags_EnterReturnsTrue );}
    virtual bool DrawCombo(void){ return ImGui::Combo(label.c_str(), (int *)dataPtr, comboOptions, comboOptionsVector.size() );}
    virtual bool DrawRadioButton(int i){return ImGui::RadioButton(comboOptionsVector[i].c_str(), (int *)dataPtr, i );}
    
    virtual bool Save(Json::Value& propertyContainer)
    {
        propertyContainer["type"] = "property";
        propertyContainer["label"] = label;
        switch(dataType)
        {
            case t_PropertyBool: return Save(GetData(), propertyContainer); break;
            case t_PropertyInt: return Save(GetData(), propertyContainer); break;
            case t_PropertyFloat: return Save(GetData(), propertyContainer); break;
            case t_PropertyString: return Save(GetData(), propertyContainer); break;

            case t_PropertyAtom:
                return Save(GetData(), propertyContainer);
                break;
        }
        return true;
    }

};

template <>
class cGUIProperty<float> : public cGUIPropertyBase
{
public:
    float internalStorage;
    float *dataPtr;
    float min;
    float max;
    
    virtual void InitDataAndType(void)
    {
        SetData(0.0f);
        dataType = t_PropertyFloat;
    }
    
    float GetData(void){ return *dataPtr; }
    
    virtual void UseInternalStorage(void)
    {
        dataPtr = &internalStorage;
        useInternalStorage = true;
    }
    
    virtual void SetStorage(float *externalStorage)
    {
        dataPtr = externalStorage;
        useInternalStorage = false;
    }

    virtual void SetData(float *inValuePtr){ *dataPtr = *inValuePtr; ClampData(); }
    virtual void SetData(float inValue){ *dataPtr = inValue; ClampData(); }

    virtual void SetNoMaxMin(void){min = max = 0.0;}
    virtual bool MaxMin(void){ return (min != max); }
    virtual void SetMin(float inMin){ min = inMin; }
    virtual void SetMax(float inMax){ max = inMax; }
    
    virtual bool Save(float inValue, Json::Value& propertyContainer)
    {
        propertyContainer["value_type"] = "float";
        propertyContainer["value"] = inValue;
        propertyContainer["min"] = min;
        propertyContainer["max"] = max;
        return true;
    }
        
    virtual void LoadFloatFromJSON(const Json::Value& propertyContainer)
    {
        float loadedFloat = 0.0f;
        if(FloatFromJSON(propertyContainer, "value", loadedFloat))
            SetData(loadedFloat);
        if(FloatFromJSON(propertyContainer, "min", loadedFloat))
            SetMin(loadedFloat);
        if(FloatFromJSON(propertyContainer, "max", loadedFloat))
            SetMin(loadedFloat);
    }

    virtual void ClampData(void)
    {
        switch(dataType)
        {
            case t_PropertyFloat:
            case t_PropertyInt:
                if(MaxMin())
                {
                    if(*dataPtr > max)
                        *dataPtr = max;
                    else if(*dataPtr < min)
                        *dataPtr = min;
                }
                break;
        }
    }
        
    virtual bool DrawDragFloat(void){ return ImGui::DragFloat(label.c_str(), (float *)dataPtr, speed, min, max, format.c_str() );}
    virtual bool DrawSliderFloat(void){ return ImGui::SliderFloat(label.c_str(), (float *)dataPtr, min, max, format.c_str() );}
    virtual bool DrawInputFloat(void){ return ImGui::InputFloat(label.c_str(), (float *)dataPtr, speed, speed * 10, format.c_str(), ImGuiInputTextFlags_EnterReturnsTrue );}

    virtual bool Save(Json::Value& propertyContainer)
    {
        propertyContainer["type"] = "property";
        propertyContainer["label"] = label;
        switch(dataType)
        {
            case t_PropertyBool: return Save(GetData(), propertyContainer); break;
            case t_PropertyInt: return Save(GetData(), propertyContainer); break;
            case t_PropertyFloat: return Save(GetData(), propertyContainer); break;
            case t_PropertyString: return Save(GetData(), propertyContainer); break;

            case t_PropertyAtom:
                return Save(GetData(), propertyContainer);
                break;
        }
        return true;
    }

};

template <>
class cGUIProperty<std::vector<float>> : public cGUIPropertyBase
{
public:
    std::vector<float> internalStorage;
    std::vector<float> *dataPtr;
    float min = 0;
    float max = 0;
    ImGuiColorEditFlags colorPickerFlags = (ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel  | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf);
    
    virtual void InitDataAndType(void)
    {
        dataType = t_PropertyFloatVector;
    }
    
    std::vector<float>& GetData(void){ return *dataPtr; }
    
    virtual void UseInternalStorage(void)
    {
        dataPtr = &internalStorage;
        useInternalStorage = true;
    }
    
    virtual void SetStorage(std::vector<float> *externalStorage)
    {
        dataPtr = externalStorage;
        useInternalStorage = false;
    }

    virtual void SetData(std::vector<float> *inValuePtr){ *dataPtr = *inValuePtr; }
    virtual void SetData(std::vector<float>& inValue){ *dataPtr = inValue; }

    virtual void SetNoMaxMin(void){min = max = 0.0;}
    virtual bool MaxMin(void){ return (min != max); }
    virtual void SetMin(float inMin){ min = inMin; }
    virtual void SetMax(float inMax){ max = inMax; }
    
    virtual bool Save(std::vector<float> inValue, Json::Value& propertyContainer)
    {
        propertyContainer["value_type"] = "float_vector";
        Json::Value array;
        for(int i = 0; i < inValue.size(); i ++)
            array.append(inValue[i]);
        propertyContainer["value"] = array;
        propertyContainer["min"] = min;
        propertyContainer["max"] = max;
        return true;
    }

    virtual void LoadFloatVectorFromJSON(const Json::Value& propertyContainer)
    {
        std::vector<float> loadedFloatVector;
        const Json::Value vectorJSON = propertyContainer["value"];
        if (vectorJSON.type() == Json::arrayValue)
        {
            int32_t vectorCount = vectorJSON.size();
            for(int i = 0; i < vectorCount; i ++)
            {
                const Json::Value& valueJSON = vectorJSON[i];
                if (valueJSON.type() != Json::nullValue)
                {
                    float val = valueJSON.asFloat();
                    loadedFloatVector.push_back(val);
                }
            }
            SetData(loadedFloatVector);
            
            float loadedFloat = 0;
            if(FloatFromJSON(propertyContainer, "min", loadedFloat))
                SetMin(loadedFloat);
            if(FloatFromJSON(propertyContainer, "max", loadedFloat))
                SetMin(loadedFloat);
        }
    }
    
    virtual void ClampData(void)
    {
        switch(dataType)
        {
            case t_PropertyFloatVector:
                if(MaxMin())
                {
                    for(int i = 0; i < dataPtr -> size(); i ++)
                    {
                        if((*dataPtr)[i] > max)
                            (*dataPtr)[i] = max;
                        else if((*dataPtr)[i] < min)
                            (*dataPtr)[i] = min;
                    }
                }
                break;
        }
    }
        
//    virtual bool DrawDragFloat(void){ return ImGui::DragFloat(label.c_str(), (float *)dataPtr, speed, min, max, format );}
//    virtual bool DrawSliderFloat(void){ return ImGui::SliderFloat(label.c_str(), (float *)dataPtr, min, max, format );}
//    virtual bool DrawInputFloat(void){ return ImGui::InputFloat(label.c_str(), (float *)dataPtr, speed, speed * 10, format, ImGuiInputTextFlags_EnterReturnsTrue );}

    virtual bool Save(Json::Value& propertyContainer)
    {
        propertyContainer["type"] = "property";
        propertyContainer["label"] = label;
        return Save(GetData(), propertyContainer);
        return true;
    }
    
    virtual bool DrawColorPicker(void)
    {
        bool changed;
        
        ImGui::PushItemWidth(128);
        if(dataPtr -> size() == 4)
        {
            changed = ImGui::ColorPicker4("##picker", dataPtr -> data(), colorPickerFlags);
        }
        ImGui::PopItemWidth();
        
        return changed;
    }

    virtual bool DrawColorEdit(void)
    {
        bool changed;
        
        ImGui::PushItemWidth(128);
        if(dataPtr -> size() == 4)
        {
            changed = ImGui::ColorEdit4("##edit", dataPtr -> data(), colorPickerFlags);
        }
        ImGui::PopItemWidth();
        
        return changed;
    }

};





#endif /* cGUIProperty_h */
