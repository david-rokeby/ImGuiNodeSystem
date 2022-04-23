//
//  cNodeInstances.hpp
//  ImGuiTester
//
//  Created by David Rokeby on 2022-04-19.
//

#ifndef cNodeInstances_h
#define cNodeInstances_h

#include "cNode.hpp"

class cPlaceholderNode : public cNode
{
public:
    char name[256];
    bool grabKeyboardFocus = true;
    
    cPlaceholderNode(std::vector<std::string> args) : cNode(args)
    {
        name[0] = 0;
    }
    
    virtual T_NodeResult DrawInternals(void)
    {
        T_NodeResult result = t_NodeNoChange;
        
        ImGui::PushItemWidth(80);
        if(grabKeyboardFocus)
        {
            ImGui::SetKeyboardFocusHere();
            grabKeyboardFocus = false;
        }
        
        bool entered = ImGui::InputText("", name, 255, ImGuiInputTextFlags_EnterReturnsTrue);
        if(entered == false)
        {
           if(ImGui::IsItemDeactivatedAfterEdit())
                entered = true;
        }
        if(entered)
        {
            manager -> PrepareNewNode(name, this);
        }
        ImGui::PopItemWidth();
        return t_NodeNoChange;
    }
};


class cButtonNode : public cNode
{
public:
    cNodeInput *input = nullptr;
    std::string buttonName = "";
    cNodeOutput *output = nullptr;
    ImVec2 buttonSize = {24, 24};
    int highlighted = 0;

    cButtonNode(std::vector<std::string> args) : cNode(args)
    {
        input = AddTriggerInput("");
        ImGuiContext* g = ImGui::GetCurrentContext();
        const float buttonHeight = g -> FontSize * 2;
        if(args.size() > 1)
        {
            buttonName = args[1];
            buttonSize = ImGui::CalcTextSize(buttonName.c_str());
            buttonSize.x = buttonSize.x + 12;
            if( buttonSize.x < buttonHeight * 2)
                buttonSize.x = buttonHeight * 2;
        }
        buttonSize.y = buttonHeight;
        output = AddOutput("");
        showTitle = false;
    }
    
    virtual T_NodeResult DrawInternals(void)
    {
        T_NodeResult result = t_NodeNoChange;
        
        ImGui::SameLine();
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);
        const ImU32 activeColor = ImGui::GetColorU32(ImGuiCol_ButtonActive);
        if(highlighted > 0)
            ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
        bool changed = ImGui::ButtonEx(buttonName.c_str(), buttonSize, ImGuiButtonFlags_PressedOnClick);
        ImGui::PopStyleVar();
        if(input -> receivedInput || changed) //???????????????
            result = t_NodeTrigger;
        
        if(highlighted > 0)
        {
            ImGui::PopStyleColor();
            highlighted --;
            if(highlighted < 0)
                highlighted = 0;
        }
        return result;
    }
    
    void Execute(void)
    {
        if(input -> receivedInput)
        {
            highlighted = 3;
            input -> receivedInput = false;
        }
        output -> Output(t_CodeBang);
    }
};

enum T_ValueNodeTypes
{
    t_ValueTypeFloat,
    t_ValueTypeInt,
    t_ValueTypeBool,
    t_ValueTypeString,
    t_ValueTypeAtom
};

class cValueNode : public cNode
{
public:
    cNodePropertyInputStub *input = nullptr;
    cGUIPropertyStub *min = nullptr;
    cGUIPropertyStub *max = nullptr;
    cGUIProperty<int> *widthOption = nullptr;

    cNodeOutput *output = nullptr;
    T_ValueNodeTypes type;
    float floatVal = 0;
    int intVal = 0;
    bool boolVal = false;
    std::string stringValue = "";
    cAtom atomValue;
    bool auto_size = true;
    bool grabKeyboardFocus = true;
    
    cValueNode(std::vector<std::string> args) : cNode(args)
    {
        T_WidgetType widget = t_WidgetDrag;
        if(label.compare("float") == 0)
            type = t_ValueTypeFloat;
        else if(label.compare("int") == 0)
            type = t_ValueTypeInt;
        else if(label.compare("bool") == 0)
            type = t_ValueTypeBool;
        else if(label.compare("string") == 0)
            type = t_ValueTypeString;
        else if(label.compare("text") == 0)
            type = t_ValueTypeString;
        else if(label.compare("toggle") == 0)
            type = t_ValueTypeBool;
        else if(label.compare("message") == 0)
            type = t_ValueTypeAtom;
        else if(label.compare("slider") == 0)
        {
            widget = t_WidgetSlider;
            type = t_ValueTypeFloat;
            if(args.size() > 1)
            {
                if(args[1].compare("float") == 0)
                    type = t_ValueTypeFloat;
                else if(args[1].compare("int") == 0)
                    type = t_ValueTypeInt;
            }
        }
        
        switch(type)
        {
            case t_ValueTypeFloat:
            {
                input = AddPropertyInput<float>("", &floatVal, widget);
                cGUIProperty<float> *prop = (cGUIProperty<float> *)input -> GetProperty();
                max = AddOption<std::string>("format", &prop -> format);
                min = AddOption<float>("min", &prop -> min);
                max = AddOption<float>("max", &prop -> max);
                widthOption = AddOption<int>("width", &prop -> widget_width);
                if(widthOption)
                {
                    widthOption -> min = 32;
                    widthOption -> max = 1024;
                }

                break;
            }
                
            case t_ValueTypeInt:
            {
                input = AddPropertyInput<int>("", &intVal, widget);
                cGUIProperty<int> *prop = (cGUIProperty<int> *)input -> GetProperty();
                min = AddOption<int>("min", &prop -> min);
                max = AddOption<int>("max", &prop -> max);
                widthOption = AddOption<int>("width", &prop -> widget_width);
                if(widthOption)
                {
                    widthOption -> min = 32;
                    widthOption -> max = 1024;
                }
break;
            }
                
            case t_ValueTypeBool:
                input = AddPropertyInput<bool>("", &boolVal, widget);
                break;

            case t_ValueTypeString:
            {
                widget = t_WidgetTextField;
                input = AddPropertyInput<std::string>("", &stringValue, widget);
                cGUIProperty<std::string> *prop = (cGUIProperty<std::string> *)input -> GetProperty();
                if(prop)
                {
                    widthOption = AddOption<int>("width", &prop -> widget_width);
                    if(widthOption)
                    {
                        widthOption -> min = 32;
                        widthOption -> max = 1024;
                    }
                    AddOption<bool>("auto_size", &auto_size);
                }
                break;
            }
                
            case t_ValueTypeAtom:
            {
                widget = t_WidgetTextField;
                input = AddPropertyInput<cAtom>("", &atomValue, widget);
                cGUIProperty<cAtom> *prop = (cGUIProperty<cAtom> *)input -> GetProperty();
                if(prop)
                {
                    widthOption = AddOption<int>("width", &prop -> widget_width);
                    if(widthOption)
                    {
                        widthOption -> min = 32;
                        widthOption -> max = 1024;
                    }
                    AddOption<bool>("auto_size", &auto_size);
                }
                break;
            }
        }
        
        if(input)
        {
            input -> triggerNode = this;
            if(widget == t_WidgetSlider)
            {
                input -> GetProperty() -> widget_width = 127;
                switch(type)
                {
                    case t_ValueTypeFloat:
                    {
                        cGUIProperty<float> *prop = (cGUIProperty<float> *)input -> GetProperty();
                        prop -> min = 0.0;
                        prop -> max = 1.0;
                        break;
                    }
                        
                    case t_ValueTypeInt:
                    {
                        cGUIProperty<int> *prop = (cGUIProperty<int> *)input -> GetProperty();
                        prop -> min = 0;
                        prop -> max = 256;
                        break;
                    }
                }
            }
        }
        output = AddOutput("");
        showTitle = false;

    }

    virtual T_NodeResult DrawInternals(void)
    {
         T_NodeResult result = DrawProperties(); //will catch if trigger input property (i.e. the value) changes
        
        if(grabKeyboardFocus)
         {
             switch(type)
             {
                 case t_ValueTypeString:
                 case t_ValueTypeAtom:
                     ImGuiID id = input -> GetProperty() -> guiID;
                     auto window = ImGui::GetCurrentWindow();
                     ImGui::SetKeyboardFocusHere(-1);
                     grabKeyboardFocus = false;
                     break;
             }
         }
        return result;
    }
    
    virtual void Execute(void)
    {
        if(input)
        {
            switch(type)
            {
                case t_ValueTypeString:
                    if(auto_size)
                    {
                        ImVec2 size = ImGui::CalcTextSize(((cGUIProperty<std::string> *)input -> GetProperty()) -> GetData().c_str());
                        if(widthOption)
                            widthOption -> SetData(size.x);
                    }
                    break;
                    
                case t_ValueTypeAtom:
                    if(auto_size)
                    {
                        cGUIProperty<cAtom> *prop = (cGUIProperty<cAtom> *)(input -> GetProperty());
                        
                        if(prop)
                        {
                            cAtom value = prop -> GetData();
                            prop -> ConvertToString(&value);
                            ImVec2 size = ImGui::CalcTextSize(value.GetAsString().c_str());
                            if(widthOption)
                                widthOption -> SetData(size.x);
                        }
                    }
                    break;
            }

            if(output)
            {
                switch(type)
                {
                    case t_ValueTypeFloat:
                        output -> Output(input -> DataAsFloat());
                        break;
                    
                    case t_ValueTypeInt:
                        output -> Output(input -> DataAsInt());
                        break;
                        
                    case t_ValueTypeBool:
                        output -> Output(input -> DataAsBool());
                        break;

                    case t_ValueTypeString:
                        output -> Output(input -> DataAsString());
                        break;

                    case t_ValueTypeAtom:
                        output -> Output(input -> DataAsAtom());
                        break;
                }
            }
        }
    }
    
    virtual bool Load(Json::Value& nodeContainer)
    {
        bool result = cNode::Load(nodeContainer);
        grabKeyboardFocus = false;
        return result;
    }
    
};

enum T_ArithmeticTypes
{
    t_MathPlus,
    t_MathSubtract,
    t_MathInverseSubtract,
    t_MathMultiply,
    t_MathDivide,
    t_MathInverseDivide,
    t_MathModulo,
    t_MathIntDivide
};

class cArithmeticNode : public cNode
{
public:
    float operand = 0;
    cNodeInput *input = nullptr;
    cNodeOutput *output = nullptr;
    T_ArithmeticTypes type;
    
    cArithmeticNode(std::vector<std::string> args) : cNode(args)
    {
        if(label.compare("+") == 0)
            type = t_MathPlus;
        else if(label.compare("-") == 0)
            type = t_MathSubtract;
        else if(label.compare("*") == 0)
            type = t_MathMultiply;
        else if(label.compare("/") == 0)
            type = t_MathDivide;
        else if(label.compare("//") == 0)
            type = t_MathIntDivide;
        else if(label.compare("!/") == 0)
            type = t_MathInverseDivide;
        else if(label.compare("!-") == 0)
            type = t_MathInverseSubtract;
        else if(label.compare("mod") == 0)
            type = t_MathModulo;

        input = AddTriggerInput("in");
        
        if(args.size() > 1)
            operand = safe_stof(args[1]);
        if(type == t_MathDivide && operand == 0.0 )
            operand = 1.0;
        AddPropertyInput<float>("op", &operand);
        
        output = AddOutput("out");
    }
    
    virtual void Execute(void)
    {
        float inVal = 0;
        if(input)
            inVal = input -> DataAsFloat();
        float outVal = 0;
            
        switch(type)
        {
            case t_MathPlus: outVal = inVal + operand; break;
            case t_MathSubtract: outVal = inVal - operand; break;
            case t_MathInverseSubtract: outVal = operand - inVal; break;
            case t_MathMultiply: outVal = inVal * operand; break;
            case t_MathModulo: outVal = fmod(inVal, operand); break;
            case t_MathDivide:
                if(operand == 0)
                    operand = 0.000001;
                outVal = inVal / operand;
                break;
                
            case t_MathIntDivide:
                if(operand == 0)
                    operand = 0.000001;
                outVal = inVal / operand;
                outVal = (int)outVal;
                break;

            case t_MathInverseDivide:
                if(inVal == 0)
                    inVal = 0.000001;
                outVal = operand / inVal;
                break;
        }
        if(output)
            output -> Output(outVal);
    }
};

enum T_TrigTypes
{
    t_TrigSine,
    t_TrigCosine,
    t_TrigTan,
    t_TrigArcSine,
    t_TrigArcCos,
    t_TrigArcTan
};

class cTrigNode : public cNode
{
public:
    cNodeInput *input = nullptr;
    cNodeOutput *output = nullptr;
    cGUIProperty<bool> *useDegreesProperty = nullptr;
    
    T_TrigTypes type;
    float degreesToRadians = PI / 180.0f;
    bool useDegrees = true;
    
    cTrigNode(std::vector<std::string> args) : cNode(args)
    {
        if(label.compare("sin") == 0)
            type = t_TrigSine;
        else if(label.compare("cos") == 0)
            type = t_TrigCosine;
        else if(label.compare("tan") == 0)
            type = t_TrigTan;
        else if(label.compare("asin") == 0)
            type = t_TrigArcSine;
        else if(label.compare("acos") == 0)
            type = t_TrigArcCos;
        else if(label.compare("atan") == 0)
            type = t_TrigArcTan;

        input = AddTriggerInput("in");
        
        useDegreesProperty = new cGUIProperty<bool>();
        useDegreesProperty -> SetStorage(&useDegrees);
        output = AddOutput("out");
    }
    
    virtual void Execute(void)
    {
        
        float inVal = 0;
        if(input)
            inVal = input -> DataAsFloat();
        float outVal = 0;
        
        switch(type)
        {
            case t_TrigSine:
                if(useDegrees)
                    inVal *= degreesToRadians;
                outVal = sin(inVal);
                break;
                
            case t_TrigCosine:
                if(useDegrees)
                    inVal *= degreesToRadians;
                outVal = cos(inVal);
                break;
                
            case t_TrigTan:
                if(useDegrees)
                    inVal *= degreesToRadians;
                outVal = tan(inVal);
                break;
                
            case t_TrigArcSine:
                if(inVal < -1)
                    inVal = -1;
                else if(inVal > 1)
                    inVal = 1;
                outVal = asin(inVal);
                if(useDegrees)
                    outVal /= degreesToRadians;
                break;
                
            case t_TrigArcCos:
                if(inVal < -1)
                    inVal = -1;
                else if(inVal > 1)
                    inVal = 1;
                outVal = acos(inVal);
                if(useDegrees)
                    outVal /= degreesToRadians;
                break;
                
            case t_TrigArcTan:
                if(inVal == 0)
                    inVal = 0.000001;
                outVal = atan(inVal);
                if(useDegrees)
                    outVal /= degreesToRadians;
                break;
        }
        
        output -> Output(outVal);
    }
};

class cRandomNode : public cNode
{
public:
    cNodePropertyInput<float> *range = nullptr;
    cNodePropertyInput<bool> *bipolar = nullptr;
    cNodeOutput *output = nullptr;

    cRandomNode(std::vector<std::string> args) : cNode(args)
    {
        cNodeInput *input = AddTriggerInput("trigger");
        float initRange = 1.0;
        if(args.size() > 1)
            initRange = safe_stof(args[1]);
        range = AddPropertyInput<float>("range", initRange);
        bipolar = AddPropertyInput<bool>("bipolar", false);

        output = AddOutput("out");
    }
        
    virtual void Execute(void)
    {
        float currentRange = range -> Data();
        bool currentBipolar = bipolar -> Data();

        float outValue = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * currentRange;
        if(currentBipolar)
            outValue = outValue * 2 - 1;
            
        output -> Output(outValue);
    }
};

enum T_MathTypes
{
    t_MathSquareRoot,
    t_MathInverse,
    t_MathLog10,
    t_MathLog2,
    t_MathExp
};

class cMathNode : public cNode
{
public:
    cNodeInput *input = nullptr;
    cNodeOutput *output = nullptr;
    T_MathTypes type;
    
    cMathNode(std::vector<std::string> args) : cNode(args)
    {
        if(label.compare("sqrt") == 0)
            type = t_MathSquareRoot;
        else if(label.compare("inverse") == 0)
            type = t_MathInverse;
        else if(label.compare("log10") == 0)
            type = t_MathLog10;
        else if(label.compare("log2") == 0)
            type = t_MathLog2;
        else if(label.compare("exp") == 0)
            type = t_MathExp;

        input = AddTriggerInput("in");
        
        output = AddOutput("out");
    }
    
    virtual void Execute(void)
    {
        float inVal = 0;
        if(input)
            inVal = input -> DataAsFloat();
        float outVal = 0;
            
        switch(type)
        {
            case t_MathSquareRoot:
                outVal = sqrt(inVal);
                break;
                
            case t_MathInverse:
                if(inVal == 0)
                    inVal = .00001;
                outVal = 1 / inVal;
                break;
                
            case t_MathLog10:
                if(inVal > 0)
                    outVal = log10(inVal);
                else
                    outVal = -INFINITY;
                break;
                
            case t_MathLog2:
                if(inVal > 0)
                    outVal = log2(inVal);
                else
                    outVal = -INFINITY;
                break;
                
            case t_MathExp:
                outVal = exp(inVal);
                break;
        }
        
        output -> Output(outVal);
    }
};

class cCounterNode : public cNode
{
public:
    ofTime time;
    int max = 100;
    cNodeOutput *output = nullptr;
    int count = 0;
    
    cCounterNode(std::vector<std::string> args) : cNode(args)
    {
        cNodeInput *input = AddTriggerInput("in");
        
        if(args.size() > 1)
        {
            max = safe_stoi(args[1]);
            if( max <= 0 )
                max = 1;
        }
        AddPropertyInput<int>("max", &max);

        output = AddOutput("");
    }
    
    virtual void Execute(void)
    {
        count ++;
        if( count >= max )
            count = 0;
         
        output -> Output(count);
    }
};


class cFilterNode : public cNode
{
public:
    float accum = 0.0;
    cNodeInput *input = nullptr;
    float degree = 0.9;
    cNodeOutput *output = nullptr;

    cFilterNode(std::vector<std::string> args) : cNode(args)
    {
        input = AddTriggerInput("");
        
        if(args.size() > 1)
        {
            degree = safe_stof(args[1]);
            if( degree < 0 )
                degree = 0;
            else if(degree > 1)
                degree = 1;
        }
        
        AddPropertyInput<float>("degree", &degree);

        output = AddOutput("");
    }
    
    virtual void Execute(void)
    {
        float inVal = input -> DataAsFloat();
        
        if( degree < 0 )
            degree = 0;
        else if(degree > 1)
            degree = 1;
        
        accum = accum * degree + (1.0 - degree) * inVal;
        
        output -> Output(accum);
    }
};

class cTogEdgeNode : public cNode
{
public:
    bool state = false;
    cNodeInput *input = nullptr;
    cNodeOutput *onOutput = nullptr;
    cNodeOutput *offOutput = nullptr;

    cTogEdgeNode(std::vector<std::string> args) : cNode(args)
    {
        input = AddTriggerInput("");
        
        onOutput = AddOutput("on");
        offOutput = AddOutput("off");
    }
    
    virtual void Execute(void)
    {
        bool newState = (input -> DataAsFloat() > 0);
        if(state)
        {
            if(!newState)
                offOutput -> Output(t_CodeBang);
        }
        else
        {
            if(newState)
                onOutput -> Output(t_CodeBang);
        }
        state = newState;
    }
};

class cSelectNode : public cNode
{
public:
    cNodeInput *input = nullptr;
    std::vector<cNodeOutput *> outputs;
    std::vector<cAtom> targets;

    cSelectNode(std::vector<std::string> args) : cNode(args)
    {
        input = AddTriggerInput("");
        
        cAtom selectorAtom;
        int selectCount = args.size() - 1;
        for(int i = 1; i <= selectCount; i ++)
        {
            std::string& selector = args[i];
            cNodeOutput *out = AddOutput(selector);
            outputs.push_back(out);
            ConvertStringToAtom(selector, selectorAtom);
            targets.push_back(selectorAtom);
            AddOption<std::string>("selector_" + std::to_string(i), selector );
        }
        AddOption<int>("select_count", selectCount, t_WidgetInput, false);
    }
    
    virtual void Execute(void)
    {
        cAtom& inputAtom = input -> DataAsAtom();
        for(int i = 0; i < targets.size(); i ++)
        {
            cAtom& targetAtom = targets[i];
            if( targetAtom.Same(inputAtom))
                outputs[i] -> Output(t_CodeBang);
        }
    }
};


enum T_DecodeOutput
{
    t_DecodeBang,
    t_DecodeFlag
};

class cDecodeNode : public cNode
{
public:
    cNodeInput *input = nullptr;
    std::vector<cNodeOutput *> outputs;
    int decodeCount = 2;
    int lastState = -1;
    int outputMode = (int)t_DecodeBang;

    cDecodeNode(std::vector<std::string> args) : cNode(args)
    {
        input = AddTriggerInput("");
        
        if( args.size() > 1)
            decodeCount = safe_stoi(args[1]);
        
        for(int i = 0; i < decodeCount; i ++)
        {
            cNodeOutput *out = AddOutput(std::to_string(i));
            outputs.push_back(out);
        }
        AddOption<int>("output_count", &decodeCount, t_WidgetInput );
        cGUIProperty<int> *outputModeOption = AddOption<int>("output_mode", &outputMode, t_WidgetCombo );
        std::vector<std::string> decodeOptions = {"bang", "flag"};
        outputModeOption -> SetComboOptions(decodeOptions);
    }
    
    virtual void OptionChanged(void)
    {
        if(decodeCount != outputs.size())
        {
            if(decodeCount > outputs.size())
            {
                int currentSize = outputs.size();
                for(int i = currentSize; i < decodeCount; i ++ )
                {
                    cNodeOutput *out = AddOutput(std::to_string(i + 1));
                    outputs.push_back(out);
                }
            }
            else
            {
                for(int i = decodeCount; i < outputs.size(); i ++)
                {
                    cNodeOutput *out = outputs[i];
                    if(out)
                        DeleteOutput(out);
                }
                outputs.resize(decodeCount);
            }
        }
    }
        
    virtual void Execute(void)
    {
        int inVal = input -> DataAsInt();
        if(inVal < outputs.size())
        {
            switch(outputMode)
            {
                case t_DecodeBang: outputs[inVal] -> Output(t_CodeBang); break;
                case t_DecodeFlag:
                    if(inVal != lastState)
                    {
                        if( lastState >= 0 && lastState < outputs.size())
                            outputs[lastState] -> Output(0);
                        outputs[inVal] -> Output(1);
                    }
                    break;
            }
        }
        lastState = inVal;
    }
};


class cTriggerNode : public cNode
{
public:
    cNodeInput *input = nullptr;
    std::vector<cNodeOutput *> outputs;
    std::vector<cAtom> triggerAtoms;

    cTriggerNode(std::vector<std::string> args) : cNode(args)
    {
        input = AddTriggerInput("");
        
        cAtom triggerAtom;
        int triggerCount = args.size() - 1;
        for(int i = 1; i <= triggerCount; i ++)
        {
            std::string& trigger = args[i];
            cNodeOutput *out = AddOutput(trigger);
            outputs.push_back(out);
            ConvertStringToAtom(trigger, triggerAtom);
            triggerAtoms.push_back(triggerAtom);
            AddOption<std::string>("trigger_" + std::to_string(i), trigger );
        }
        AddOption<int>("output_count", triggerCount, t_WidgetInput, false);
    }
    
    virtual void Execute(void)
    {
        for(int i = 0; i < triggerAtoms.size(); i ++)
        {
            cAtom& targetAtom = triggerAtoms[i];
            outputs[i] -> Output(targetAtom);
        }
    }
};


enum T_TimerUnits
{
    t_TimeMicroseconds,
    t_TimeMilliseconds,
    t_TimeSeconds,
    t_TimeMinutes,
    t_TimeHours,
    t_TimeHoursMinutesSeconds
};

enum T_MetroUnits
{
    t_MetroTimeMilliseconds,
    t_MetroTimeSeconds,
    t_MetroTimeMinutes,
    t_MetroTimeHours
};


class cMetroNode : public cNode
{
public:
    cNodeOutput *output = nullptr;
    
    bool on = false;
    float period = 30.0;
    T_MetroUnits units = t_MetroTimeMilliseconds;
    std::vector<std::string> unitOptions = { "milliseconds", "seconds", "minutes", "hours" };

    float lastTick = 0;
    
    cMetroNode(std::vector<std::string> args) : cNode(args)
    {
        cNodePropertyInput<bool> *onInput = AddPropertyInput<bool>("on", &on);
        onInput -> triggerNode = this;
        
        if(args.size() > 1)
            period = safe_stof(args[1]);
        AddPropertyInput<float>("period", &period);
        
        output = AddOutput("out");
        
        if(args.size() > 2)
        {
            std::string unitString = args[2];
            if(unitString.compare("seconds") == 0)
                units = t_MetroTimeSeconds;
            else if(unitString.compare("milliseconds") == 0)
                units = t_MetroTimeMilliseconds;
            else if(unitString.compare("minutes") == 0)
                units = t_MetroTimeMinutes;
            else if(unitString.compare("hours") == 0)
                units = t_MetroTimeHours;
        }
        
        AddProperty<int>("units", (int *)&units, t_WidgetCombo) -> SetComboOptions(unitOptions);

        lastTick = ofGetElapsedTimef();
    }
    
    virtual T_NodeResult Update(void)
    {
        float currentTime = ofGetElapsedTimef();
        if(on)
        {
            float currentPeriod = period;
            switch(units)
            {
                case t_MetroTimeMilliseconds: currentPeriod /= 1000; break;
                case t_MetroTimeSeconds: break;
                case t_MetroTimeMinutes: currentPeriod *= 60.0; break;
                case t_MetroTimeHours: currentPeriod *= 3600.0; break;
            }
            
            if(currentTime - lastTick >= (double)currentPeriod)
            {
                Execute();
                lastTick = lastTick + currentPeriod;
                if(lastTick + currentPeriod < currentTime)
                    lastTick = currentTime;
            }
        }
    }
    
    virtual void Execute(void)
    {
        output -> Output(t_CodeBang);
    }
};


class cTimerNode : public cNode
{
public:
    bool on = false;
    cGUIProperty<bool> *outputIntegersProperty = nullptr;
    bool outputIntegers = false;
    cNodeOutput *output = nullptr;
    
    std::vector<std::string> unitOptions = { "microseconds", "milliseconds", "seconds", "minutes", "hours" };

    T_TimerUnits units = t_TimeMilliseconds;
    float baseTimeF = 0;
    uint64_t baseTime = 0;
    int lastTime = 0;
    bool was_on = false;

    cTimerNode(std::vector<std::string> args) : cNode(args)
    {
        AddPropertyInput<bool>("on", &on) -> triggerNode = this;
        
        if(args.size() > 1)
        {
            std::string unitString = args[1];
            if(unitString.compare("seconds") == 0)
                units = t_TimeSeconds;
            else if(unitString.compare("milliseconds") == 0)
                units = t_TimeMilliseconds;
            else if(unitString.compare("microseconds") == 0)
                units = t_TimeMicroseconds;
            else if(unitString.compare("minutes") == 0)
                units = t_TimeMinutes;
            else if(unitString.compare("hours") == 0)
                units = t_TimeHours;
        }
        
        AddProperty<int>("units", (int *)&units, t_WidgetCombo) -> SetComboOptions(unitOptions);
        outputIntegersProperty = AddProperty<bool>("output integers", &outputIntegers);
        
        output = AddOutput("out");
        
        UpdateBaseTime();
    }

    void UpdateBaseTime()
    {
        switch(units)
        {
            case t_TimeMicroseconds:
                baseTime = ofGetElapsedTimeMicros();
                break;
            
            case t_TimeMilliseconds:
                baseTime = ofGetElapsedTimeMillis();
                break;

            case t_TimeSeconds:
            case t_TimeMinutes:
            case t_TimeHours:
                baseTimeF = ofGetElapsedTimef();
                break;
        }
    }
    
    virtual T_NodeResult DrawInternals(bool changed)
    {
        T_NodeResult result = t_NodeNoChange;
        
        switch(units)
        {
            case t_TimeHours:
            case t_TimeMinutes:
            case t_TimeSeconds:
                if( outputIntegersProperty )
                    outputIntegersProperty -> showWidget = true;
                break;
                
            default:
                if( outputIntegersProperty )
                    outputIntegersProperty -> showWidget = false;
                break;
        }
        
        result = DrawProperties();

        T_TimerUnits holdUnits = units;
        
        if(units != holdUnits)
        {
            switch(units)
            {
                case t_TimeMicroseconds:
                    switch(holdUnits)
                    {
                        case t_TimeMilliseconds: baseTime *= 1000; break;
                        case t_TimeSeconds: baseTime *= 1000000; break;
                        case t_TimeMinutes: baseTime *= 60000000; break;
                        case t_TimeHours: baseTime *= 3600000000; break;
                    }
                    break;
                    
                case t_TimeMilliseconds:
                    switch(holdUnits)
                    {
                        case t_TimeMicroseconds: baseTime /= 1000; break;
                        case t_TimeSeconds: baseTime *= 1000; break;
                        case t_TimeMinutes: baseTime *= 60000; break;
                        case t_TimeHours: baseTime *= 3600000; break;
                    }
                    break;
                    
                case t_TimeSeconds:
                    switch(holdUnits)
                    {
                        case t_TimeMicroseconds: baseTime /= 1000000; break;
                        case t_TimeMilliseconds: baseTime /= 1000; break;
                        case t_TimeMinutes: baseTime *= 60; break;
                        case t_TimeHours: baseTime *= 3600; break;
                    }
                    break;

                case t_TimeMinutes:
                    switch(holdUnits)
                    {
                        case t_TimeMicroseconds: baseTime /= 60000000; break;
                        case t_TimeMilliseconds: baseTime /= 60000; break;
                        case t_TimeSeconds: baseTime /= 60; break;
                        case t_TimeHours: baseTime *= 60; break;
                    }
                    break;

                case t_TimeHours:
                    switch(holdUnits)
                    {
                        case t_TimeMicroseconds: baseTime /= 3600000000; break;
                        case t_TimeMilliseconds: baseTime /= 3600000; break;
                        case t_TimeSeconds: baseTime /= 3600; break;
                        case t_TimeMinutes: baseTime /= 60; break;
                    }
                    break;
            }
        }
        return result;
    }
    
    virtual T_NodeResult Update(void)
    {
        T_NodeResult result = t_NodeNoChange;
        
        if(on)
        {
            if(was_on == false)
                UpdateBaseTime();
                
            switch(units)
            {
                case t_TimeMicroseconds:
                {
                    uint64_t currentTime = ofGetElapsedTimeMicros();
                    uint64_t elapsed = currentTime - baseTime;
                    output -> outputData.Set((int)elapsed);
             //       output -> Output((int)elapsed);
                    result = t_NodeTrigger;
                    break;
                }
                
                case t_TimeMilliseconds:
                {
                    uint64_t currentTime = ofGetElapsedTimeMillis();
                    uint64_t elapsed = currentTime - baseTime;
                    output -> outputData.Set((int)elapsed);
//                    output -> Output((int)elapsed);
                    result = t_NodeTrigger;
                    break;
                }

                case t_TimeSeconds:
                case t_TimeMinutes:
                case t_TimeHours:
                {
                    float currentTime = ofGetElapsedTimef();
                    float elapsed = currentTime - baseTimeF;
                    switch(units)
                    {
                        case t_TimeMinutes:
                            elapsed /= 60.0f;
                            break;
                            
                        case t_TimeHours:
                            elapsed /= 3600.0f;
                            break;
                    }
                    if(outputIntegers)
                    {
                        int intMinutes = elapsed;
                        if(intMinutes != lastTime)
                        {
                            output -> outputData.Set((int)intMinutes);
//                            output -> Output((int)intMinutes);
                            result = t_NodeTrigger;
                            lastTime = intMinutes;
                        }
                    }
                    else
                    {
                        output -> outputData.Set(elapsed);
 //                       output -> Output(elapsed);
                        result = t_NodeTrigger;
                    }
                    break;
                }
            }
        }
        was_on = on;
        return result;
    }
    
    void Execute(void){output -> Output();}
};


enum T_PlotStyle
{
    t_LinePlot,
    t_ScatterPlot,
    t_StairPlot,
    t_StemPlot,
    t_BarPlot
};

class cPlotNode : public cNode
{
public:
    cNodeInput *xInput = nullptr;
    cNodeInput *yInput = nullptr;
    T_PlotStyle plotStyle = t_LinePlot;
    float plotMinY = -1.0;
    float plotMaxY = 1.0;
    std::vector<std::string>plotOptions = { "line", "scatter", "stair", "stem", "bar" };
    
    std::vector<float> x_data;
    std::vector<float> y_data;
    std::vector<float> temp;
    bool xy = false;

    cPlotNode(std::vector<std::string> args) : cNode(args)
    {
        yInput = AddTriggerInput("y");
        xInput = AddInput("x");
        AddOption<int>("style", (int *)&plotStyle, t_WidgetCombo) -> SetComboOptions(plotOptions);
        AddOption<float>("min y", &plotMinY);
        AddOption<float>("max y", &plotMaxY);

        int size = 256;
        if(args.size() > 1)
        {
            size = safe_stoi(args[1]);
            if( size <= 0 )
                size = 256;
        }
        x_data.resize(size);
        for(int i = 0; i < x_data.size(); i ++)
            x_data[i] = i;
        y_data.resize(size);
        temp.resize(size);
    }
    
    virtual T_NodeResult DrawInternals(void)
    {
        ImPlot::SetCurrentContext(manager->plotContext);
        ImGui::PushItemWidth(300);
        xy = false;
        
        if(xInput)
        {
            if(xInput -> parent != -1)
                xy = true;
        }
        if(ImPlot::BeginPlot("", ImVec2(300, 300)))
        {
            ImPlot::SetupAxisLimits(ImAxis_Y1, plotMinY, plotMaxY, ImPlotCond_Always);
            ImPlot::SetupAxes("", "");
            switch(plotStyle)
            {
                case t_LinePlot:
                    if(xy)
                        ImPlot::PlotLine("data", x_data.data(), y_data.data(), x_data.size());
                    else
                        ImPlot::PlotLine("data", y_data.data(), x_data.size());
                    break;
                    
                case t_ScatterPlot:
                    if(xy)
                        ImPlot::PlotScatter("data", x_data.data(), y_data.data(), x_data.size());
                    else
                        ImPlot::PlotScatter("data", y_data.data(), x_data.size());

                    break;

                case t_StairPlot:
                    if(xy)
                        ImPlot::PlotStairs("data", x_data.data(), y_data.data(), x_data.size());
                    else
                        ImPlot::PlotStairs("data", y_data.data(), x_data.size());
                        break;
                    
                case t_StemPlot:
                    if(xy)
                        ImPlot::PlotStems("data", x_data.data(), y_data.data(), x_data.size());
                    else
                        ImPlot::PlotStems("data", y_data.data(), x_data.size());
                    break;
                    
                case t_BarPlot:
                    if(xy)
                        ImPlot::PlotBars("data", x_data.data(), y_data.data(), x_data.size(), 1);
                    else
                        ImPlot::PlotBars("data", y_data.data(), x_data.size(), 1);
                    break;
            }
            ImPlot::EndPlot();
        }
        ImGui::PopItemWidth();
        
        return t_NodeNoChange;
    }
    
    virtual void Execute(void)
    {
        if(yInput)
        {
            if(yInput -> receivedInput)
            {
                switch(yInput -> receivedData.type)
                {
                    case t_DataFloatVector:
                    {
                        std::vector<float>& inVector = yInput -> receivedData.GetAsFloatVector();
                        int vectorSize = inVector.size();
                        if(vectorSize > y_data.size())
                            vectorSize = y_data.size();
                        for(int i = 0; i < vectorSize; i ++)
                        {
                            y_data.pop_back();
                            y_data.insert(y_data.begin(), inVector[i]);
                            yInput -> receivedInput = false;
                        }
                        break;
                    }
                        
                    default:
                        y_data.pop_back();
                        y_data.insert(y_data.begin(), yInput -> DataAsFloat());
                        yInput -> receivedInput = false;
                        break;
                }
            }
        }
        if(xInput)
        {
            if(xInput -> receivedInput)
            {
                switch(xInput -> receivedData.type)
                {
                    case t_DataFloatVector:
                        
                        break;
                        
                    default:
                        x_data.pop_back();
                        x_data.insert(x_data.begin(), xInput -> DataAsFloat());
                        xInput -> receivedInput = false;
                        break;
                }
            }
        }
    }
};

enum T_SignalType
{
    t_Sine,
    t_Cosine,
    t_Square,
    t_Saw,
    t_Triangle,
    t_Random
};

class cSignalNode : public cNode
{
public:
    bool on = false;
    float period = 1.0;
    float range = 1.0;
    bool bipolar = true;
    int vectorSize = 1;
    T_SignalType signalType = t_Sine;
    std::vector<std::string> signalOptions = { "sine", "cosine", "square", "saw", "triangle", "random" };
    cNodeOutput *output = nullptr;

    float time = 0;
    float lastTime = 0;
    float outValue = 0;
    std::vector<float> outVector;

    cSignalNode(std::vector<std::string> args) : cNode(args)
    {
        AddPropertyInput<bool>("on", &on);

        if(args.size() > 1)
            period = safe_stof(args[1]);
        AddPropertyInput("period", &period);
        
        AddProperty<int>("signal", (int *)&signalType, t_WidgetCombo) -> SetComboOptions(signalOptions);
        
        output = AddOutput("out");
        
        AddOption<float>("range", &range);
        AddOption<bool>("bipolar", &bipolar);
        AddOption<int>("vector size", &vectorSize);
    }
    
/*    virtual bool DrawInternals(bool changed)
    {
        changed = DrawProperties(changed);
        return changed;
    }*/
    
    virtual T_NodeResult Update(void)
    {
        if(on)
        {
            if(period < .001)
                period = .001;
            
            float currentTime = ofGetElapsedTimef();
                
            if(vectorSize == 1)
            {
                float elapsed = currentTime - lastTime;
                time += elapsed / period;
                float delta = time - (float)(int)time;
                switch(signalType)
                {
                    case t_Sine: outValue = sin(delta * PI * 2.0f); break;
                    case t_Cosine: outValue = cos(delta * PI * 2.0f); break;
                    case t_Saw: outValue = delta * 2.0f - 1.0f; break;
                    case t_Square: outValue = (int)(delta * 2) * 2 - 1; break;
                    case t_Triangle: outValue = abs(0.5 - delta) * 4 - 1; break;
                    case t_Random: outValue = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
                }
                if(bipolar == false)
                    outValue = outValue / 2.0f + 0.5f;
                outValue *= range;
            }
            else
            {
                if(outVector.size() != vectorSize)
                    outVector.resize(vectorSize);
                float elapsed = currentTime - lastTime;
                
                float samplePeriod = period * (vectorSize);
                
                for(int i = 0; i < vectorSize; i ++)
                {
                    time += elapsed / samplePeriod;
                    float delta = time - (float)(int)time;
                    switch(signalType)
                        
                    {
                        case t_Sine: outVector[i] = sin(delta * PI * 2.0f); break;
                        case t_Cosine: outVector[i] = cos(delta * PI * 2.0f); break;
                        case t_Saw: outVector[i] = delta * 2.0f - 1.0f; break;
                        case t_Square: outVector[i] = (int)(delta * 2) * 2 - 1; break;
                        case t_Triangle: outVector[i] = abs(0.5 - delta) * 4 - 1; break;
                        case t_Random: outVector[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1;
                    }
                    if(bipolar == false)
                        outVector[i] = outVector[i] / 2.0f + 0.5f;
                    outVector[i] *= range;
                }
            }
            lastTime = currentTime;
//            Execute();
            return t_NodeTrigger;
        }
        return t_NodeNoChange;
    }
    
    virtual void Execute(void)
    {
        if( vectorSize == 1)
            output -> Output(outValue);
        else
            output -> Output(outVector);
    }
};

class cMouseNode : public cNode
{
public:
    bool on = true;
    cNodeOutput *xOut = nullptr;
    cNodeOutput *yOut = nullptr;
    int mouseRef = 0;

    cMouseNode(std::vector<std::string> args) : cNode(args)
    {
        AddPropertyInput<bool>("on", &on) -> triggerNode = this;
        
        xOut = AddOutput("mouse x out");
        yOut = AddOutput("mouse y out");
        cGUIProperty<int> *ref = AddOption<int>("ref", &mouseRef, t_WidgetCombo);
        std::vector<std::string> refOptions = {"screen", "editor", "grid"};
        ref->SetComboOptions(refOptions);
    }
    
    virtual T_NodeResult Update(void)
    {
        if(on)
            return t_NodeTrigger;
        return t_NodeNoChange;
    }

    virtual void Execute(void)
    {
        ImVec2 mousePos = ImGui::GetMousePos();
        yOut -> Output(mousePos.y);
        xOut -> Output(mousePos.x);
   }
};

class cBezierNode : public cNode
{
public:
    static std::map<std::string, ImVec4> presets;
    static std::vector<std::string> presetNames;
    int preset;
    
    cNodeOutput *output = nullptr;
    cNodeInput *input = nullptr;
    float params[5];
    
    cBezierNode(std::vector<std::string> args) : cNode(args)
    {
        input = AddTriggerInput("");
        output = AddOutput("");
        
        if(presets.size() == 0)
            InitPresets();
        
        AddOption<int>("preset", &preset, t_WidgetCombo) -> SetComboOptions(presetNames);
        AddOption<float>("param 1", &params[0]);
        AddOption<float>("param 2", &params[1]);
        AddOption<float>("param 3", &params[2]);
        AddOption<float>("param 4", &params[3]);
        ImVec4 inOutSine = presets["InOut Sine"];
        params[0] = inOutSine.x;
        params[1] = inOutSine.y;
        params[2] = inOutSine.z;
        params[3] = inOutSine.w;
    }
    
    void InitPresets(void)
    {
        presets["Linear"] = {0.000f, 0.000f, 1.000f, 1.000f};
        presets["In Sine"] = {0.470f, 0.000f, 0.745f, 0.715f};
        presets["In Quad"] = {0.550f, 0.085f, 0.680f, 0.530f};
        presets["In Cubic"] = {0.550f, 0.055f, 0.675f, 0.190f};
        presets["In Quart"] = {0.895f, 0.030f, 0.685f, 0.220f};
        presets["In Quint"] = {0.755f, 0.050f, 0.855f, 0.060f};
        presets["In Expo"] = {0.950f, 0.050f, 0.795f, 0.035f};
        presets["In Circ"] = {0.600f, 0.040f, 0.980f, 0.335f};
        presets["In Back"] = {0.600f, -0.28f, 0.735f, 0.045f};

        presets["Out Sine"] = {0.390f, 0.575f, 0.565f, 1.000f};
        presets["Out Quad"] = {0.250f, 0.460f, 0.450f, 0.940f};
        presets["Out Cubic"] = {0.215f, 0.610f, 0.355f, 1.000f};
        presets["Out Quart"] = {0.165f, 0.840f, 0.440f, 1.000f};
        presets["Out Quint"] = {0.230f, 1.000f, 0.320f, 1.000f};
        presets["Out Expo"] = {0.190f, 1.000f, 0.220f, 1.000f};
        presets["Out Circ"] = {0.075f, 0.820f, 0.165f, 1.000f};
        presets["Out Back"] = {0.175f, 0.885f, 0.320f, 1.275f};

        presets["InOut Sine"] = {0.445f, 0.050f, 0.550f, 0.950f};
        presets["InOut Quad"] = {0.455f, 0.030f, 0.515f, 0.955f};
        presets["InOut Cubic"] = {0.645f, 0.045f, 0.355f, 1.000f};
        presets["InOut Quart"] = {0.770f, 0.000f, 0.175f, 1.000f};
        presets["InOut Quint"] = {0.860f, 0.000f, 0.070f, 1.000f};
        presets["InOut Expo"] = {1.000f, 0.000f, 0.000f, 1.000f};
        presets["InOut Circ"] = {0.785f, 0.135f, 0.150f, 0.860f};
        presets["InOut Back"] = {0.680f, -0.55f, 0.265f, 1.550f};
        
        presetNames.reserve(presets.size());
        for(auto item:presets)
            presetNames.push_back(item.first);
    }
    
    virtual T_NodeResult DrawInternals(void)
    {
        if(preset != 0)
        {
            ImVec4 newPreset = presets[presetNames[preset]];
            params[0] = newPreset.x;
            params[1] = newPreset.y;
            params[2] = newPreset.z;
            params[3] = newPreset.w;
            params[4] = preset;
            preset = 0;
        }
        bool changed = ImGui::Bezier("", reinterpret_cast<float*>(params));
        if( changed)
            return t_NodePropertyChange;
        return t_NodeNoChange;
    }
    
/*    virtual T_NodeResult Update(void)
    {
        if(input -> receivedInput)
        {
            Execute();
            input -> receivedInput = false;
            return true;
        }
        return false;
    }*/
    
    virtual void Execute(void)
    {
        float inValue = input -> DataAsFloat();
        output -> Output(ImGui::bezier_table_at<256>(inValue, reinterpret_cast<float*>(params)));
    }

};
                         

class cCurveNode : public cNode
{
public:
    cNodeOutput *output = nullptr;
    cNodeInput *input = nullptr;
    cGUIProperty<bool> *tangentsProperty = nullptr;
    std::vector<float> values;
    const ImVec2 editorSize = {256.0, 128.0};
    int pointCount;
    bool tangents = true;
    
    cCurveNode(std::vector<std::string> args) : cNode(args)
    {
        input = AddTriggerInput("in");
        tangentsProperty = AddProperty<bool>("tangents", &tangents);
        output = AddOutput("out");
        values.resize(256);
        values[0] = 0;
        values[1] = 0;
        values[2] = 0;
        values[3] = 0.5;
        values[4] = 0.5;
        values[5] = 0.5;
        values[6] = 1;
        values[7] = 1;
        values[8] = 1;
        pointCount = 3;
    }
    
    virtual bool DrawInternals(bool changed);
    
    virtual void Execute(void)
    {
        float inValue = input -> DataAsFloat();
//        output -> Output(ImWidgets::BezierValue(inValue, reinterpret_cast<float*>(values)));
    }
};
                         


class c2DSliderNode : public cNode
{
public:
    cNodeOutput *outputX = nullptr;
    cNodeOutput *outputY = nullptr;
    
    float xValue = 0.0f;
    float yValue = 0.0f;
    
    float minX = -1.0f;
    float maxX = 1.0f;
    float minY = -1.0f;
    float maxY = 1.0f;
    int size = 127;
    bool showOptions = false;
    bool outputEveryFrame = true;
    bool propertyChanged = false;

    c2DSliderNode(std::vector<std::string> args) : cNode(args)
    {
        outputX = AddOutput("out X");
        outputY = AddOutput("out Y");
        AddOption<float>("min x", &minX);
        AddOption<float>("max x", &maxX);
        AddOption<float>("min y", &minY);
        AddOption<float>("max y", &maxY);
        AddOption<bool>("output always", &outputEveryFrame);
    }
    
    virtual T_NodeResult DrawInternals(void)
    {
        bool changed = MinimalSlider2DFloat("slider_2d", &xValue, &yValue, minX, maxX, minY, maxY);
        if( changed || outputEveryFrame)
            return t_NodeTrigger;
        return t_NodeNoChange;
    }
    
    virtual void Execute(void)
    {
        outputX -> Output(xValue);
        outputY -> Output(yValue);
    }
};

class cPackNode : public cNode
{
public:
    std::vector<cNodeInput *> inputs;
    cNodeOutput *output = nullptr;
    std::vector<cAtom> list;
    int inputCount;
    
    cPackNode(std::vector<std::string> args) : cNode(args)
    {
        inputCount = 2;
        if( args.size() > 1)
            inputCount = safe_stoi(args[1]);
        if(inputCount < 1)
            inputCount = 1;
        for(int i = 0; i < inputCount; i ++)
        {
            cNodeInput *in = AddInput(std::string("in") + std::to_string(i + 1));
            inputs.push_back(in);
        }
        inputs[0] -> triggerNode = this;
        if(label == "pak")
        {
            for(int i = 1; i < inputCount; i ++)
                inputs[i] -> triggerNode = this;
        }
        list.resize(inputCount);
        output = AddOutput("");
        AddOption<int>("count", &inputCount, t_WidgetInputInt);
    }
    
    virtual void OptionChanged(void)
    {
        int count = inputCount;
        if(count != inputs.size())
        {
            if(count > inputs.size())
            {
                int currentSize = inputs.size();
                for(int i = currentSize; i < count; i ++ )
                {
                    cNodeInput *in = AddInput(std::string("in") + std::to_string(i + 1));
                    inputs.push_back(in);
                    if(label == "pak")
                        in -> triggerNode = this;
                }
                list.resize(count);
            }
            else
            {
                for(int i = count; i < inputs.size(); i ++)
                {
                    cNodeInput *in = inputs[i];
                    if(in)
                        DeleteInput(in);
                }
                inputs.resize(count);
                list.resize(count);
            }
        }
    }
    
    //only called by direct input
    
    virtual void Execute(void)
    {
        for(int i = 0; i < inputs.size(); i ++)
        {
            cNodeInput *in = inputs[i];
            list[i] = in -> receivedData.GetAsAtom();
        }
        output -> Output(list);
    }
};

class cUnpackNode : public cNode
{
public:
    cNodeInput *input = nullptr;
    std::vector<cNodeOutput *> outputs;
    int outputCount;

    cUnpackNode(std::vector<std::string> args) : cNode(args)
    {
        input = AddTriggerInput("list in");
        outputCount = 2;
        if( args.size() > 1)
            outputCount = safe_stoi(args[1]);
        if(outputCount < 1)
            outputCount = 1;
        for(int i = 0; i < outputCount; i ++)
        {
            cNodeOutput *out = AddOutput(std::to_string(i + 1));
            outputs.push_back(out);
        }
        AddOption<int>("count", &outputCount, t_WidgetInputInt);
    }
    
    virtual void OptionChanged(void)
    {
        int count = outputCount;
        if(count != outputs.size())
        {
            if(count > outputs.size())
            {
                int currentSize = outputs.size();
                for(int i = currentSize; i < count; i ++ )
                {
                    cNodeOutput *out = AddOutput(std::to_string(i + 1));
                    outputs.push_back(out);
                }
            }
            else
            {
                for(int i = count; i < outputs.size(); i ++)
                {
                    cNodeOutput *out = outputs[i];
                    if(out)
                        DeleteOutput(out);
                }
                outputs.resize(count);
            }
        }
    }
    
    virtual void Execute(void)
    {
        int numOuts = outputs.size();
        if( numOuts == 0)
            return;
        
        if(input)
        {
            cData& data = input -> receivedData;
            int unpackLength = 1;
            switch(data.type)
            {
                case t_DataFloat: outputs[0] -> Output(data.GetAsFloat()); break;
                case t_DataInt: outputs[0] -> Output(data.GetAsInt()); break;
                case t_DataCode: outputs[0] -> Output(data.GetAsCode()); break;
                case t_DataBool: outputs[0] -> Output(data.GetAsBool()); break;
                case t_DataString: outputs[0] -> Output(data.GetAsString()); break;
                case t_DataFloatVector:
                {
                    int unpackLength = data.dataFloatVector.size();
                    if(unpackLength > outputs.size())
                        unpackLength = outputs.size();
                    std::vector<float>& fv = data.GetAsFloatVector();
                    for(int i = 0; i < unpackLength; i ++)
                        outputs[i] -> Output(fv[i]);
                    break;
                }
                    
                case t_DataIntVector:
                {
                    unpackLength = data.dataIntVector.size();
                    if(unpackLength > outputs.size())
                        unpackLength = outputs.size();
                    std::vector<int>& iv = data.GetAsIntVector();
                    for(int i = 0; i < unpackLength; i ++)
                        outputs[i] -> Output(iv[i]);
                    break;
                }
                    
                case t_DataBoolVector:
                {
                    unpackLength = data.dataBoolVector.size();
                    if(unpackLength > outputs.size())
                        unpackLength = outputs.size();
                    std::vector<bool>& bv = data.GetAsBoolVector();
                    for(int i = 0; i < unpackLength; i ++)
                        outputs[i] -> Output(bv[i]);
                    break;
                }
                    
                case t_DataStringVector:
                {
                    unpackLength = data.dataStringVector.size();
                    if(unpackLength > outputs.size())
                        unpackLength = outputs.size();
                    std::vector<std::string>& sv = data.GetAsStringVector();
                    for(int i = 0; i < unpackLength; i ++)
                        outputs[i] -> Output(sv[i]);
                    break;
                }
                    
                case t_DataList:
                {
                    unpackLength = data.dataList.size();
                    if(unpackLength > outputs.size())
                        unpackLength = outputs.size();
                    std::vector<cAtom>& av = data.GetAsList();
                    for(int i = 0; i < unpackLength; i ++)
                    {
                        cAtom& a = av[i];
                        switch(a.type)
                        {
                            case t_DataFloat: outputs[i] -> Output(a.GetAsFloat()); break;
                            case t_DataInt: outputs[i] -> Output(a.GetAsInt()); break;
                            case t_DataCode: outputs[i] -> Output(a.GetAsCode()); break;
                            case t_DataBool: outputs[i] -> Output(a.GetAsBool()); break;
                            case t_DataString: outputs[i] -> Output(a.GetAsString()); break;
                        }
                    }
                    break;
                }
           }
        }
    }
};

class cGetVariableNode : public cNode
{
public:
    static cVariableManager *variableManager;
    
    cNodeOutput *output;
    std::string variable_name;
    cVariableReferenceBase *reference = nullptr;
    cData currentValue;

    cGetVariableNode(std::vector<std::string> args) : cNode(args)
    {
        if(args.size() > 1)
        {
            variable_name = args[1];
            if(variableManager)
            {
                reference = variableManager -> FindVariableByName(variable_name);
                if(reference)
                    reference -> CopyTo(currentValue);
            }
        }
        output = AddOutput(variable_name);
    }
    
    virtual T_NodeResult Update(void)
    {
        if(reference != nullptr)
        {
            if(reference -> ValueChanged(currentValue))
            {
                reference -> CopyTo(currentValue);
                return t_NodeTrigger;
            }
        }
        return t_NodeNoChange;
    }
    
    virtual void Execute(void)
    {
        if(output)
            output -> Output(&currentValue);
    }
};

// send values to program variables

class cSetVariableNode : public cNode
{
public:
    static cVariableManager *variableManager;
    cNodeInput *input = nullptr;

    std::string variable_name = "";
    cVariableReferenceBase *reference = nullptr;
    cData currentValue;

    cSetVariableNode(std::vector<std::string> args) : cNode(args)
    {
        if(args.size() > 1)
        {
            variable_name = args[1];
            if(variableManager)
            {
                reference = variableManager -> FindVariableByName(variable_name);
            }
        }
        input = AddTriggerInput(variable_name);
    }
    
    virtual void Execute(void)
    {
        if(input != nullptr && input -> receivedInput && reference != nullptr)
        {
            input -> receivedInput = false;
            if(reference -> ValueChanged(input -> receivedData))
            {
                return input -> receivedData.FillReference(reference);
            }
        }
        return false;
    }
};

class cColorPickerNode : public cNode
{
public:
    cNodeInput *input = nullptr;
    cNodeOutput *output = nullptr;
    std::string variable_name = "";
    cVariableReferenceBase *reference = nullptr;
    cData colorData;
    cGUIProperty<std::vector<float>>* colorProperty;
    ImVec4 color = {1.0, 0.0, 0.0, 1.0};
    std::vector<float> colorVector;
    bool hueWheel = true;
    bool showAlpha = false;
    bool inputs = false;
    bool picker = true;

    cColorPickerNode(std::vector<std::string> args) : cNode(args)
    {
        input = AddTriggerInput("");
        output = AddOutput("");
        colorVector.resize(4);
        colorProperty = AddProperty<std::vector<float>>("color", t_WidgetColorPicker, true, false);
//        colorProperty = new cGUIProperty<std::vector<float>>();
        if(colorProperty)
        {
            colorProperty -> SetStorage(&colorVector);
        }
        AddOption<bool>("picker", &picker, t_WidgetCheckBox, true);
        AddOption<bool>("hue wheel", &hueWheel, t_WidgetCheckBox, true);
        AddOption<bool>("alpha", &showAlpha, t_WidgetCheckBox, true);
        AddOption<bool>("inputs", &inputs, t_WidgetCheckBox, true);
    }
    
    virtual T_NodeResult DrawInternals(bool changed)
    {
        ImGui::SameLine();
        if(showAlpha)
            colorProperty -> colorPickerFlags |= (ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf);
        else
            colorProperty -> colorPickerFlags &= ~(ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_AlphaPreviewHalf);
        if(hueWheel)
            colorProperty -> colorPickerFlags |= ImGuiColorEditFlags_PickerHueWheel;
        else
            colorProperty -> colorPickerFlags &= ~ImGuiColorEditFlags_PickerHueWheel;
        if(!inputs)
            colorProperty -> colorPickerFlags |= ImGuiColorEditFlags_NoInputs;
        else
            colorProperty -> colorPickerFlags &= ~ImGuiColorEditFlags_NoInputs;
        if(picker)
            colorProperty -> widgetType = t_WidgetColorPicker;
        else
            colorProperty -> widgetType = t_WidgetColorEdit;
        
        T_NodeResult result = DrawProperties();
        
        if(result > t_NodeNoChange)
            result = t_NodeTrigger;
        return result;
    }
    
    virtual void Execute(void)
    {
        if(input -> receivedInput)
        {
            colorVector = input -> receivedData.GetAsFloatVector();
        }
        if(output)
        {
            colorData.Set(colorVector);
            output -> Output(&colorData);
        }
        return false;
    }
};

#endif /* cNodeInstances_h */
