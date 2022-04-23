//
//  cNodeOutput.h
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-09.
//

#ifndef cNodeOutput_h
#define cNodeOutput_h

#include <string>
#include "ofxImGui.h"
#include "imnodes_internal.h"
#include "cGUIProperty.h"

#include "cData.h"
#include "cNodeManager.h"
#include "cNodeInput.h"
#include "cNodeLink.h"


class cNodeOutput
{
public:
    static cNodeManager *manager;
    static void SetNodeManager( cNodeManager *man ){ manager = man; }

    std::string label;
    int uuid;
    std::vector<int> links;
    cData outputData;
    ImRect pinRect;
    cNode *node = nullptr;
    int outputState = 0;
    
    cNodeOutput(std::string inLabel)
    {
        label = inLabel;
    }
    
    cNodeLink *GetLink(int id);
    
    void ConnectWithLink(int link_id)
    {
        if( link_id == -1 )
            return;
        
        cNodeLink *link = GetLink(link_id);
        
        if(link != nullptr)
        {
            if(std::find(links.begin(), links.end(), link_id) == links.end())
                links.push_back(link_id);
        }
    }
    
    void Disconnect(void);
    
    void DisconnectLink(int in_link_id);
    
    void DisconnectFrom(int child_id);
    
    void Output(float outFloat)
    {
        outputData.Set(outFloat);
        Output();
    }
    
    void Output(int outInt)
    {
        outputData.Set(outInt);
        Output();
    }
    
    void Output(T_Code outInt)
    {
        outputData.Set(outInt);
        Output();
    }
    
    void Output(bool outBool)
    {
        outputData.Set(outBool);
        Output();
    }
    
    void Output(std::string& outString)
    {
        outputData.Set(outString);
        Output(&outputData);
    }
    
    void Output(std::vector<float>& outFloatVector)
    {
        outputData.Set(outFloatVector);
        Output(&outputData);
    }
    
    void Output(std::vector<cAtom>& outList)
    {
        outputData.Set(outList);
        Output(&outputData);
    }
    
    void Output(cAtom& outAtom)
    {
        if(outAtom.type == t_DataString)
        {
            std::string& s = outAtom.GetAsString();
            std::vector<std::string> res;
            std::string delimiter = " ";
            
            size_t pos_start = 0, pos_end, delim_len = delimiter.length();
            std::string token;
            
            while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos)
            {
                token = s.substr (pos_start, pos_end - pos_start);
                pos_start = pos_end + delim_len;
                res.push_back (token);
            }

            std::string last = s.substr (pos_start);
            if( last != "")
                res.push_back (s.substr (pos_start));
            
            if(res.size() == 1)
            {
                bool caughtInt = false;
                int inter;
                try
                {
                    inter = std::stoi(res[0]);
                    outputData.Set(inter);
                    caughtInt = true;
                }
                catch (...){}
                try
                {
                    float floater = std::stof(res[0]);
                    if(caughtInt)
                    {
                        if(abs(floater) > abs((float)inter))
                            outputData.Set(floater);
                        Output(&outputData);
                        return;
                    }
                    else
                    {
                        outputData.Set(floater);
                        Output(&outputData);
                        return;
                    }
                }
                catch (...){}
                outputData.Set(res[0]);
                Output(&outputData);
                return;
            }
            
            std::vector<cAtom>& list = outputData.dataList;
            list.clear();
            outputData.type = t_DataList;
            
            for(std::string substring:res)
            {
                cAtom atom;
                bool caughtInt = false;
                int inter;
                try
                {
                    inter = std::stoi(substring);
                    atom.Set(inter);
                    list.push_back(atom);
                    caughtInt = true;
                }
                catch (...){}
                try
                {
                    float floater = std::stof(substring);
                    if(caughtInt)
                    {
                        if(abs(floater) > abs((float)inter))
                            list.back().Set(floater);
                    }
                    else
                    {
                        atom.Set(floater);
                        list.push_back(atom);
                    }
                    continue;
                }
                catch (...){}
                atom.Set(substring);
                list.push_back(atom);
            }
            Output(&outputData);
        }
        else
        {
            outputData.Set(outAtom);
            Output(&outputData);
        }
    }
    
    void Output(void)
    {
        Output(&outputData);
    }
    
    void Output(cData* data);
    
    void Draw(void);
};

                

#endif /* cNodeOutput_h */
