//
//  cNodeLink.h
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-09.
//

#ifndef cNodeLink_h
#define cNodeLink_h

#include <string>
#include "ofxJSON.h"

class cNodeInput;
class cNodeOutput;
class cNode;
class cNodeManager;

class cNodeLink
{
public:
    static cNodeManager *manager;
    static void SetNodeManager( cNodeManager *man ){ manager = man; }

    int id;
    int from;
    int to;
    
    cNodeLink(int in_link_id, int in_from, int in_to){ id = in_link_id; from = in_from; to = in_to;}
    
    bool Save(Json::Value& linkContainer);
};



#endif /* cNodeLink_h */
