//
//  cOSCManager.h
//  ImGuiTester
//
//  Created by David Rokeby on 2022-04-20.
//

#ifndef cOSCManager_h
#define cOSCManager_h

#include "cOSCNode.hpp"

class cOSCManager
{
public:
    std::map<std::string, cOSCTargetNode *> targets;
    std::vector<std::string> targetNames;
    std::map<std::string, cOSCSourceNode *> sources;
    std::vector<std::string> sourceNames;

    cOSCManager(void)
    {
        cOSCSenderNode::oscManager = this;
        cOSCTargetNode::oscManager = this;
        cOSCReceiveNode::oscManager = this;
        cOSCSourceNode::oscManager = this;
    }
    
    void RegisterTarget(cOSCTargetNode *targetNode)
    {
        if(targetNode)
        {
            std::string& name = targetNode -> target.name;
            if(name != "" && targets.count(name) == 0)
            {
                targets[name] = targetNode;
                targetNames.push_back(name);
            }
            else
            {
                //duplicate or enpty
            }
        }
    }
    
    void RemoveTarget(std::string name)
    {
        if(targets.count(name) > 0)
            targets.erase(name);
        auto found = find(targetNames.begin(), targetNames.end(), name);
        if( found != targetNames.end())
            targetNames.erase(found);
    }
    
    cOSCTargetNode *FindTargetNode(std::string inName)
    {
        if(inName != "" && targets.count(inName) > 0)
            return targets[inName];
        return nullptr;
    }
    
    std::vector<std::string>& GetTargetList(void){ return targetNames; }

    
    void RegisterSource (cOSCSourceNode *sourceNode)
    {
        if(sourceNode)
        {
            std::string& name = sourceNode -> receiver.name;
            if(name != "" && sources.count(name) == 0)
            {
                sources[name] = sourceNode;
                sourceNames.push_back(name);
            }
            else
            {
                // duplicate or empty name
            }
        }
    }
    
    cOSCSourceNode *FindSourceNode(std::string inName)
    {
        if(inName != "" && sources.count(inName) > 0)
            return sources[inName];
        return nullptr;
    }

    std::vector<std::string>& GetSourceList(void){ return sourceNames; }

    void RemoveSource(std::string name)
    {
        if(sources.count(name) > 0)
            sources.erase(name);
        auto found = std::find(sourceNames.begin(), sourceNames.end(), name);

        if( found != sourceNames.end())
            sourceNames.erase(found);
    }

};


#endif /* cOSCManager_h */
