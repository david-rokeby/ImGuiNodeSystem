//
//  cNodeInput.cpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-10.
//

#include <stdio.h>
#include "cNodeInput.h"
#include "cNodeOutput.h"
#include "cNode.hpp"

void cNodeInput::Disconnect(bool disconnectFromOutlet )
{
    if(parent != -1)
    {
        if( disconnectFromOutlet )
        {
            cNodeOutput *output = manager -> GetOutput(parent);
            if(output)
            {
                output -> DisconnectFrom(this -> uuid);
            }
        }
        parent = -1;
    }
}


bool cNodeInput::Trigger(void)
{
    if(triggerNode)
    {
        triggerNode -> Execute();
        return true;
    }
    return false;
}

void cNodePropertyInputStub::Disconnect(bool disconnectFromOutlet )
{
    if(parent != -1)
    {
        if( disconnectFromOutlet )
        {
            cNodeOutput *output = manager -> GetOutput(parent);
            if(output)
            {
                output -> DisconnectFrom(this -> uuid);
            }
        }
        parent = -1;
    }
}


bool cNodePropertyInputStub::Trigger(void)
{
    if(triggerNode)
    {
        triggerNode -> Execute();
        return true;
    }
    return false;
}
