//
//  cNode.cpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-13.
//

#include <stdio.h>

#include "cNode.hpp"
#include "cNodeInstances.hpp"
#include "dear_widgets.h"

cVariableManager *cSetVariableNode::variableManager = nullptr;
cVariableManager *cGetVariableNode::variableManager = nullptr;

bool cCurveNode::DrawInternals(bool changed)
{
    int newPointCount = pointCount;
    ImU32 flags = 0;
    if(tangents == false)
        flags = (ImU32)ImWidgets::CurveEditorFlags::NO_TANGENTS;
    int changed_index = ImWidgets::CurveEditor("", (float *)values.data(), pointCount, editorSize, flags, &newPointCount);
    if(newPointCount != pointCount)
    {
        if(newPointCount * 3 > values.size())
            values.resize(newPointCount * 4);
        pointCount = newPointCount;
    }
    if(input -> receivedInput)
        Execute();
    return changed;
}


