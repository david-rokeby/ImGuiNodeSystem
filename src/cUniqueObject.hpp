//
//  cUniqueObject.hpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-14.
//

#ifndef cUniqueObject_hpp
#define cUniqueObject_hpp

class cNode;
class cNodeInput;
class cNodeOutput;
class cNodePropertyInputStub;
class cGUIPropertyStub;
class cUIActionStub;
class cNodeLink;
class cUI;
class cUIOtherStub;


enum T_ObjectType
{
    t_Node,
    t_Input,
    t_InputProperty,
    t_Property,
    t_Output,
    t_Option,
    t_Link,
    t_UI,
    t_Action,
    t_Other,
    t_Pane,
    t_TabBar,
    t_Tab,
    t_TreeNode,
    t_Empty
};


class cUniqueObject
{
public:
    T_ObjectType type;
    void *object;
    
    void Reset(void){ type = t_Empty; object = nullptr;}
    
    void SetAsInput(cNodeInput *in){ type = t_Input; object = in;}
    void SetAsPropertyInput(cNodePropertyInputStub *in){ type = t_InputProperty; object = in;}
    void SetAsProperty(cGUIPropertyStub *prop){ type = t_Property; object = prop; }
    void SetAsOutput(cNodeOutput *out){ type = t_Output; object = out; }
    void SetAsOption(cGUIPropertyStub *opt){ type = t_Option; object = opt; }
    void SetAsNode(cNode *node){ type = t_Node; object = node; }
    void SetAsLink(cNodeLink *link){ type = t_Link; object = link; }
    void SetAsUI(cUI *ui){ type = t_UI; object = ui; }
    void SetAsAction(cUIActionStub *action){ type = t_Action; object = action; }

    void SetAsOther(cUIOtherStub *other){ type = t_Other; object = other; }
    cNodeInput *GetInput(void){ return static_cast<cNodeInput *>(object); }
    cNodePropertyInputStub *GetPropertyInput(void){ return static_cast<cNodePropertyInputStub *>(object); }
    cGUIPropertyStub *GetProperty(void){ return static_cast<cGUIPropertyStub *>(object); }
    cNodeOutput *GetOutput(void){ return static_cast<cNodeOutput *>(object); }
    cNode *GetNode(void){ return static_cast<cNode *>(object); }
    cNodeLink *GetLink(void){ return static_cast<cNodeLink *>(object); }
    cUI *GetUI(void){ return static_cast<cUI *>(object); }
    cUIActionStub *GetAction(void){ return static_cast<cUIActionStub *>(object); }
    cUIOtherStub *GetOther(void){ return static_cast<cUIOtherStub *>(object); }
};



#endif /* cUniqueObject_hpp */
