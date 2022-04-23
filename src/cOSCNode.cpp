//
//  cOSCNode.cpp
//  ImGuiTester
//
//  Created by David Rokeby on 2022-04-16.
//

#include "cOSCNode.hpp"
#include "cOSCManager.h"

cOSCManager* cOSCSenderNode::oscManager = nullptr;
cOSCManager* cOSCTargetNode::oscManager = nullptr;
cOSCManager* cOSCSourceNode::oscManager = nullptr;
cOSCManager* cOSCReceiveNode::oscManager = nullptr;

bool cOSCSenderNode::VerifyTarget(void)
{
    return (target -> registeredName == targetName -> GetData());
}

bool cOSCSenderNode::FindTargetNode(std::string inName)
{
    if( oscManager )
    {
        target = oscManager -> FindTargetNode(inName);
    }
    else
        target = nullptr;
}

bool cOSCTargetNode::RegisterTarget(void)
{
    if( oscManager )
    {
        oscManager -> RemoveTarget(registeredName);
        oscManager -> RegisterTarget(this);
        registeredName = target.name;
    }
}

void cOSCTargetNode::UnregisterTarget(void)
{
    if( oscManager )
    {
        if(registeredName != "")
        {
            oscManager -> RemoveTarget(registeredName);
            registeredName = "";
        }
    }
}

bool cOSCReceiveNode::VerifySource(void)
{
    return (source -> registeredName == sourceName -> GetData());
}

bool cOSCReceiveNode::FindSourceNode(std::string inName)
{
    if( oscManager )
    {
        source = oscManager -> FindSourceNode(inName);
        if(source)
        {
            source -> RegisterDestination(this);
            registeredAddress = addressProperty -> GetData();
        }
    }
    else
        source = nullptr;
}

bool cOSCSourceNode::RegisterSource(void)
{
    if( oscManager )
    {
        oscManager -> RemoveSource(registeredName);
        oscManager -> RegisterSource(this);
        registeredName = receiver.name;
    }
}

void cOSCSourceNode::UnregisterSource(void)
{
    if( oscManager )
    {
        if(registeredName != "")
            oscManager -> RemoveSource(registeredName);
    }
}

bool cOSCSourceNode::RegisterDestination(cOSCReceiveNode *dest)
{
    std::string& dest_address = dest -> addressProperty -> GetData();
    if(destinations.count(dest_address) == 0)
    {
        destinations[dest_address] = dest;
        return true;
    }
    return false;
}

bool cOSCSourceNode::UnregisterDestination(std::string oldName)
{
    if(destinations.count(oldName) > 0)
    {
        destinations[oldName] -> SourceGoingAway(this);
        destinations.erase(oldName);
        return true;
    }
    return false;
}

cOSCReceiveNode *cOSCSourceNode::FindDestination(std::string dest_address)
{
    if(destinations.count(dest_address) > 0)
    {
        return destinations[dest_address];
    }
    return nullptr;
}

bool cOSCSourceNode::SendTo(cOSCReceiveNode *receiver, cData& list)
{
    if(receiver)
        receiver -> Receive(list);
}

