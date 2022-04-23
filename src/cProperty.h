//
//  cProperty.h
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-09.
//

#ifndef cProperty_h
#define cProperty_h

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
    t_WidgetToggle
};



// ideally a class or struct can have a static property structure that can be instantly latched onto the structure
// i.e. multiple triggers can exists and when editing a particular trigger, the gui attaches to the particular trigger class instance,

// object subclasses property structure to attach to its data?
template <class T>
class cGUIProperty
{
public:
    // property can be up to 4 components?
    
    // ideally this is a container for saved node properties that should be saved / loaded
    // this includes properties for the inputs where they have gui elements
    // is a property always connected to a gui (even if hidden?)
    
    // should data be local? should data point to the actual item?
    bool hasWidget = false;
    std::string label;
    T_WidgetType widgetType = t_WidgetNone;
    bool showWidget = true;
    T min;
    T max;
    float speed = 1.0f;
    char format[256];
    int32_t widget_uuid;
    int32_t widget_width = 120;
    T_PropertyType type = t_DataNone;
    T data;

    void InitProperty(std::string name, T_WidgetType type, bool show)
    {
        type = GetPropertyType(data);
        switch(type)
        {
            case t_PropertyFloat: data = 0.0; break;
            case t_PropertyInt: data = 0; break;
            case t_PropertyBool: data = false; break;
            case t_PropertyCode: data = t_CodeNothing; break;
        }
    }

    void SetWidgetType(T_WidgetType inType)
    {
        hasWidget = true;
        switch(propertyType)
        {
            case t_DataFloat:
                min = max = 0;
                speed = 0.1;
                widget_width = 80;
                strcpy(format, "%3f");
                
                switch(widgetType)
                {
                    case t_WidgetDrag:
                        widgetType = t_WidgetDragFloat;
                        break;
                        
                    case t_WidgetSlider:
                        widgetType = t_WidgetSliderFloat;
                        break;
                        
                    case t_WidgetInput:
                        widgetType = t_WidgetInputFloat;
                        widget_width = 100;
                        break;
                }
                break;
            
            case t_DataInt:
                min = max = 0;
                speed = 1;
                widget_width = 80;
                strcpy(format, "%d");

                switch(widgetType)
                {
                    case t_WidgetDrag:
                        widgetType = t_WidgetDragInt;
                        break;
                        
                    case t_WidgetSlider:
                        widgetType = t_WidgetSliderInt;
                        break;
                        
                    case t_WidgetInput:
                        widgetType = t_WidgetInputInt;
                        widget_width = 100;
                        break;
                }
                break;
                
            case t_DataBool:
                if (std::is_same<T, bool>::value)
                {
                    switch(widgetType)
                    {
                        case t_WidgetCheckBox:
                            widgetType = t_WidgetCheckBox;
                            break;

                        case t_WidgetToggle:
                            widgetType = t_WidgetToggle;
                            break;
                    }
                }
    }
    /Users/drokeby/Dev/of_v0.11.2_osx_release/apps/myApps/ImGuiTester/src/cProperty.h:        if (std::is_same<T, int>::value)
    bool Draw(void)
    {
        bool changed = false;
        if( showWidget)
        {
            ImGui::PushItemWidth(widget_width);
            switch(widgetType)
            {
                case t_WidgetDragFloat:
                    changed = ImGui::DragFloat(label.c_str(), &data, speed, min, max, format );
                    break;
                    
                case t_WidgetDragInt:
                    changed = ImGui::DragInt(label.c_str(), &data, speed, min, max, format );
                    break;
                    
                case t_WidgetSliderFloat:
                    changed = ImGui::SliderFloat(label.c_str(), &data, min, max, format );
                    break;
                    
                case t_WidgetSliderInt:
                    changed = ImGui::SliderInt(label.c_str(), &data, min, max, format );
                    break;

                case t_WidgetInputFloat:
                    changed = ImGui::InputFloat(label.c_str(), &data, speed, speed * 10, format );
                    break;
                    
                case t_WidgetInputInt:
                    changed = ImGui::InputInt(label.c_str(), &data, speed, speed * 10 );
                    break;
                
                case t_WidgetCheckBox:
                    changed = ImGui::Checkbox(label.c_str(), &data );
                    break;

                case t_WidgetToggle:
                    changed = ToggleButton(label.c_str(), &data );
                    break;
            }
            ImGui::PopItemWidth();
        }
        return changed;
    }

    T GetData(void){ return data; }
    
    bool Save(void);
    bool Load(void);
};


class cGUIPropertyStructure
{
public:
    std::vector<cGUIProperty<void>> propertySet;
    void *currentInstance;
};


#endif /* cProperty_h */
