//
//  cOSCNode.hpp
//  ImGuiTester
//
//  Created by David Rokeby on 2022-04-16.
//

#ifndef cOSCNode_hpp
#define cOSCNode_hpp

#pragma once

#include "cNode.hpp"
#include "ofxOsc.h"
#include <algorithm>

enum T_OSCFormat
{
    t_Normal = 0,
    t_QLab = 1
};

class cOSCTarget
{
public:
    std::string name = "untitled";
    std::string ipAddress;
    int port;
    T_OSCFormat format;
    bool connected;
    ofxOscSender sender;

    cOSCTarget() { Reset(); }
    cOSCTarget(std::string inIP, int inPort, bool inConnected) { Init( inIP, inPort, inConnected); }
    void Init(std::string inIP, int32_t inPort, bool inConnected) { ipAddress = inIP; port = inPort; connected = inConnected; }
    void Reset(void) { format = t_Normal; Clear(); }
    void Clear(void) { ipAddress = "127.0.0.1"; port = 2500; connected = false; }
    bool Connect(void) { sender.clear(); connected = false; if (sender.setup(ipAddress, port)) { connected = true; } return connected; }
    void SendMessage(ofxOscMessage& m) { sender.sendMessage(m); }
    void Disconnect(void) { sender.clear(); connected = false; }
};

class cOSCReceiver
{
public:
    std::string name = "untitled";
    int port = 2500;
    T_OSCFormat format = t_Normal;
    bool connected = false;
    ofxOscReceiver receiver;
    ofxOscReceiverSettings settings;

    cOSCReceiver() { Reset(); }
    cOSCReceiver(int inPort, bool inConnected) { Init( inPort, inConnected); }
    void Init(int32_t inPort, bool inConnected) { port = inPort; connected = inConnected; }

    void Reset(void) { settings.port = port; settings.start = true; settings.reuse = false; receiver.setup(settings);}
    
    void Stop(void){receiver.stop();}
    
    bool HasWaitingMessages(void)
    {
        if(receiver.hasWaitingMessages())
        {
            return true;
        }
    }
    
    bool GetNextMessage(ofxOscMessage& inMessage)
    {
        if( HasWaitingMessages())
        {
            if(receiver.getNextMessage(inMessage))
            {
                return true;
            }
        }
        return false;
    }
};

class cOSCSenderNode;

class cOSCManager;

class cOSCTargetNode : public cNode
{
public:
    static cOSCManager* oscManager;
    cNodeInput *input = nullptr;
    cGUIProperty<std::string> *ipProperty = nullptr;
    cGUIProperty<int> *portProperty = nullptr;
    cGUIProperty<std::string> *targetName = nullptr;
    std::map<std::string, cOSCSenderNode *> senders;
    std::string registeredName;
    cOSCTarget target;
    
    // ~cOSCTargetNode must contact registered clients?

    cOSCTargetNode(std::vector<std::string> args) : cNode(args)
    {
        int argIndex = 1;
        if(args.size() > 1)
        {
            target.name = args[argIndex ++];
        }
        if(args.size() > 3)
        {
            target.ipAddress = args[argIndex ++];
            target.port = std::stoi(args[argIndex]);
        }
        
        input = AddTriggerInput("");
        targetName = AddProperty<std::string>("target", &target.name, t_WidgetTextField);
        ipProperty = AddProperty<std::string>("ip address", &target.ipAddress, t_WidgetTextField);
        portProperty = AddProperty<int>("port", &target.port, t_WidgetInputInt);
        
        if(target.name != "")
        {
            RegisterTarget();
        }
    }
    
    ~cOSCTargetNode(void)
    {
        UnregisterTarget();
    }
    
    bool RegisterTarget(void);
    void UnregisterTarget(void);

    virtual T_NodeResult DrawInternals(void)
    {
        T_NodeResult result = t_NodeNoChange;
        bool reconnect = false;
        if(target.connected == false)
            target.Connect();
        ImGui::SameLine();
        bool localChange = targetName -> Draw();
        if(localChange)
            RegisterTarget();
        localChange |= ipProperty -> Draw();
        localChange |= portProperty -> Draw();
        if(localChange)
            reconnect = true;

        if(reconnect || (target.connected == false))
        {
            if(target.connected)
                target.Disconnect();
            target.Connect();
        }
        if(localChange)
            return t_NodePropertyChange;
        return t_NodeNoChange;
    }
    
    virtual void Execute(void)
    {
        if( input -> receivedInput)
            SendDataAsMessage(nullptr, input -> receivedData);
    }
    
    void SendDataAsMessage(std::string *address, cData& list)
    {
        ofxOscMessage message;
        bool doSend = false;
    
        bool addressed = false;
        if( address != nullptr)
        {
            message.setAddress(*address);
            addressed = true;
        }
        switch(list.type)
        {
            case t_DataList:
            {
                std::vector<cAtom>& av = list.GetAsList();
                for(cAtom& a:av)
                {
                    switch(a.type)
                    {
                        case t_DataFloat: message.addFloatArg(a.GetAsFloat()); break;
                        case t_DataInt: message.addIntArg(a.GetAsInt()); break;
                        case t_DataBool: message.addBoolArg(a.GetAsBool()); break;
                        case t_DataString:
                            if(addressed == false)
                            {
                                message.setAddress(a.GetAsString());
                                addressed = true;
                            }
                            else
                                message.addStringArg(a.GetAsString()); break;
                        case t_DataCode: message.addStringArg(a.GetAsString()); break;
                    }
                }
                doSend = true;
                break;
            }
        }
        
        if(doSend)
            target.SendMessage(message);
    }
};

class cOSCManager;

class cOSCSenderNode : public cNode
{
public:
    static cOSCManager* oscManager;
    cNodeInput *input = nullptr;
    cGUIProperty<std::string> *targetName = nullptr;
    cGUIProperty<std::string> *address = nullptr;
    
    cOSCTargetNode *target = nullptr;
    
    cOSCSenderNode(std::vector<std::string> args) : cNode(args)
    {
        input = AddTriggerInput("");
        std::string t_name = "";
        std::string t_address = "/untitled";
        if(args.size() > 1)
            t_name = args[1];
        if( args.size() > 2)
            t_address = args[2];
        targetName = AddProperty<std::string>("target", t_name, t_WidgetTextField);
        address = AddProperty<std::string>("osc address", t_address, t_WidgetTextField);

        if(t_name != "")
        {
            if( oscManager)
                FindTargetNode(t_name);
        }
    }
    
    T_NodeResult DrawInternals(void)
    {
        T_NodeResult result = t_NodeNoChange;
        
        ImGui::SameLine();
        result = DrawProperties();
        
        if(result > t_NodeNoChange)
        {
            if(target == nullptr)
            {
                std::string n = targetName -> GetData();
                if(n != "")
                {
                    if( oscManager)
                        FindTargetNode(targetName -> GetData());
                    else
                        target = nullptr;
                }
            }
            if(target)
            {
                if( target -> target.name != targetName -> GetData())
                {
                    if( oscManager)
                        FindTargetNode(targetName -> GetData());
                    else
                        target = nullptr;
                }
            }
        }
        return result;
    }
    
    virtual void Execute(void)
    {
        if(target != nullptr)
        {
            if(VerifyTarget() && input -> receivedInput)
            {
                target -> SendDataAsMessage(&address -> GetData(), input -> receivedData);
            }
        }
    }
    
    bool VerifyTarget(void);
    bool FindTargetNode(std::string inName);

};

class cOSCReceiveNode;

class cOSCSourceNode : public cNode
{
public:
    static cOSCManager* oscManager;

    cGUIProperty<std::string> *sourceName = nullptr;
    cGUIProperty<int> *portProperty = nullptr;
    cNodeOutput *output = nullptr;
    cData list;
    std::string registeredName;
    cOSCReceiver receiver;
    std::map<std::string, cOSCReceiveNode *> destinations;

    cOSCSourceNode(std::vector<std::string> args) : cNode(args)
    {
        std::string name = "";
        int port = 2500;
        
        int argIndex = 1;
        if(args.size() > 1)
        {
            receiver.name = args[argIndex ++];
        }
        if(args.size() > 2)
        {
            receiver.port = std::stoi(args[argIndex]);
        }
        
        sourceName = AddProperty<std::string>("name", &receiver.name, t_WidgetTextField);
        portProperty = AddProperty<int>("port", &receiver.port, t_WidgetInputInt);
        output = AddOutput("");
        receiver.Reset();
        
        if(receiver.name != "")
            RegisterSource();
    }
    
    ~cOSCSourceNode(void)
    {
        receiver.Stop();
        UnregisterSource();
        auto iter = destinations.begin();
        while (iter != destinations.end())
        {
            cOSCReceiveNode *dest = iter -> second;
            UnregisterDestination(iter -> first);
            ++iter;
        }
    }

    virtual T_NodeResult DrawInternals(void)
    {
        T_NodeResult result = t_NodeNoChange;
        
        bool reconnect = false;
        T_NodeResult localResult = sourceName -> Draw();
        if(localResult > t_NodeNoChange)
        {
            RegisterSource();
            result = localResult;
        }
        localResult = portProperty -> Draw();
        if(localResult > t_NodeNoChange)
        {
            reconnect = true;
            result = localResult;
        }
        
        if(reconnect)
            receiver.Reset();
        
        return result;
    }
    
    virtual T_NodeResult Update(void)
    {
        if(receiver.HasWaitingMessages())
        {
//            Execute();
            return t_NodeTrigger;
        }
        return t_NodeNoChange;
    }

    virtual void Execute(void)
    {
        ofxOscMessage message;
        std::string address = "";

        while(receiver.GetNextMessage(message))
        {
            address = message.getAddress();
            cOSCReceiveNode *receiveNode = FindDestination(address);

            int numArgs = message.getNumArgs();
            
            list.StartList();
            
            if(receiveNode == nullptr)
            {
                list.AddToList(address);
            }
            
            for(int i = 0; i < numArgs; i ++)
            {
                ofxOscArgType type = message.getArgType(i);
                switch(type)
                {
                    case OFXOSC_TYPE_INT32:
                    case OFXOSC_TYPE_INT64:
                    {
                        int inputValue = message.getArgAsInt(i);
                        list.AddToList(inputValue);
                        break;
                    }
                        
                    case OFXOSC_TYPE_FLOAT:
                    {
                        float inputValue = message.getArgAsFloat(i);
                        list.AddToList(inputValue);
                        break;
                    }
                        
                    case OFXOSC_TYPE_STRING:
                    {
                        std::string inputValue = message.getArgAsString(i);
                        list.AddToList(inputValue);
                        break;
                    }
                }
            }
            // here we need to distribute to appropriate receivers...
            if(receiveNode)
            {
                if(list.dataList.size() > 0)
                    SendTo(receiveNode, list);
            }
            else
            {
                if(output)
                    output -> Output(&list);
            }
        }
    }

    
    bool RegisterSource(void);
    void UnregisterSource(void);
    bool RegisterDestination(cOSCReceiveNode *dest);
    bool UnregisterDestination(std::string oldName);

    bool SendTo(cOSCReceiveNode *receiver, cData& list);
    cOSCReceiveNode *FindDestination(std::string dest_address);
};


class cOSCReceiveNode : public cNode
{
public:
    static cOSCManager* oscManager;
    cNodeOutput *output = nullptr;
    cData list;
    cGUIProperty<std::string> *sourceName = nullptr;
    cGUIProperty<std::string> *addressProperty = nullptr;
    std::string registeredAddress = "";
    cOSCSourceNode *source = nullptr;
    
    cOSCReceiveNode(std::vector<std::string> args) : cNode(args)
    {
        std::string s_name = "";
        std::string s_address = "/untitled";

        if(args.size() > 1)
            s_name = args[1];
        if( args.size() > 2)
            s_address = args[2];

        sourceName = AddProperty<std::string>("source", s_name, t_WidgetTextField);
        addressProperty = AddProperty<std::string>("address", s_address, t_WidgetTextField);
        output = AddOutput("");
        
        if(s_name != "")
        {
            FindSourceNode(s_name);
        }
    }
    
    ~cOSCReceiveNode(void)
    {
        if(source)
            source -> UnregisterDestination(registeredAddress);
    }
    
    virtual T_NodeResult DrawInternals(void)
    {
        T_NodeResult result = DrawProperties();
        if(result > t_NodeNoChange)
        {
            if(source == nullptr)
            {
                std::string n = sourceName -> GetData();
                if(n != "")
                {
                    if( oscManager)
                        FindSourceNode(sourceName -> GetData());
                    else
                        source = nullptr;
                }
            }
            else if( source -> receiver.name != sourceName -> GetData())
            {
                source -> UnregisterDestination(registeredAddress);
                registeredAddress = "";
                FindSourceNode(sourceName -> GetData());
            }
            else if(registeredAddress != addressProperty -> GetData())
            {
                source -> UnregisterDestination(registeredAddress);
                source -> RegisterDestination(this);
                registeredAddress = addressProperty -> GetData();
            }
        }
        return result;
    }
    
    void Receive(cData& inData)
    {
        if(output)
            output -> Output(&inData);
    }
    
    bool FindSourceNode(std::string inName);
    bool VerifySource(void);

    void SourceGoingAway(cOSCSourceNode *oldSource)
    {
        if(source == oldSource)
        {
            source = nullptr;
            registeredAddress = "";
        }
    }
    


};



#endif /* cOSCNode_hpp */
