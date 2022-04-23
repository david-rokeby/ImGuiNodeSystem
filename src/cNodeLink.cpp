//
//  cNodeLink.cpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-12.
//

#include <stdio.h>

#include "cNodeLink.h"
#include "cNodeManager.h"

bool cNodeLink::Save(Json::Value& linkContainer)
{
    cNodeMemberSpecifier fromSpec, toSpec;
    fromSpec = manager -> GetOutputSpecifier(from);
    toSpec = manager -> GetInputSpecifier(to);
    // note: input and output ids are not saved
    // links must use node id and input or output names
    linkContainer["source_node"] = fromSpec.node_uuid;
    linkContainer["source_output_name"] = fromSpec.name;
    linkContainer["dest_node"] = toSpec.node_uuid;
    linkContainer["dest_input_name"] = toSpec.name;
}

