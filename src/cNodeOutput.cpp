//
//  cNodeOutput.cpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-12.
//

#include "cNodeOutput.h"
#include "cNode.hpp"

void cNodeOutput::Output(cData *data)
{
    outputState = 1;
    for(int i = 0; i < links.size(); i ++)
    {
        int link_id = links[i];
        if(link_id != -1)
        {
            cNodeLink *link = GetLink(link_id);
            if(link != nullptr && link -> to != -1)
            {
                cNodePropertyInputStub *destProperty = manager -> GetPropertyInput(link -> to);
                if(destProperty != nullptr)
                {
                    outputState = 2;
                    destProperty -> ReceiveData(data);
                }
                else
                {
                    cNodeInput *dest = manager -> GetInput(link -> to);
                    if(dest != nullptr)
                    {
                        outputState = 2;
                        dest -> ReceiveData(data);
                    }
                }
            }
        }
    }
}

void cNodeOutput::Draw(void)
{
    if(outputState != 0)
    {
        switch(outputState)
        {
            case 2:
                ImNodes::PushColorStyle(ImNodesCol_Pin, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)));
                break;
                
            default:
                ImNodes::PushColorStyle(ImNodesCol_Pin, ImGui::GetColorU32(ImVec4(0.0f, 1.0f, 1.0f, 1.0f)));
                break;
        }
    }
    ImNodes::BeginOutputAttribute(uuid);
    
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const char *label_start = label.c_str();
    const char *label_end = ImGui::FindRenderedTextEnd(label_start);
    ImVec2 text_size = ImGui::CalcTextSize(label_start, label_end, false, 0.0f);
    float lineHeight = ImGui::GetTextLineHeight();
    if(node)
        p.x += (node -> nodeSize.x - text_size.x - lineHeight);

    draw_list -> AddText( p, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), label_start, label_end);
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    ImGui::Dummy(ImVec2(text_size.x + style.FramePadding.x * 4, lineHeight));

    ImNodes::EndOutputAttribute();
    
    if(outputState != 0)
    {
        outputState = 0;
        ImNodes::PopColorStyle();
    }

}

cNodeLink *cNodeOutput::GetLink(int id)
{
    return manager -> GetLink(id);
}

void cNodeOutput::Disconnect(void) // disconnect from attached inputs but do not have inputs disconnect back
{
    for(int i = 0; i < links.size(); i ++)
    {
        int link_id = links[i];
        if(link_id != -1)
        {
            cNodeLink *link = GetLink(link_id);
            if(link != nullptr)
            {
                if(link -> to != -1)
                {
                    cNodePropertyInputStub *propertyInput = manager -> GetPropertyInput(link -> to);
                    if(propertyInput != nullptr)
                    {
                        propertyInput -> Disconnect(false);
                    }
                    else
                    {
                        cNodeInput *input = manager -> GetInput(link -> to);
                        if(input != nullptr)
                        {
                            input -> Disconnect(false);
                        }
                    }
                }
                links[i] = -1;
                manager -> RemoveLink(link_id);
            }
        }
    }
}

void cNodeOutput::DisconnectLink(int in_link_id) // disconnect only this link
{
    for(int i = 0; i < links.size(); i ++)
    {
        int link_id = links[i];
        if(link_id == in_link_id)
        {
            cNodeLink *link = GetLink(link_id);
            if(link != nullptr)
            {
                if(link -> to != -1)
                {
                    cNodePropertyInputStub *propertyInput = manager -> GetPropertyInput(link -> to);
                    if(propertyInput != nullptr)
                    {
                        cNode *sourceNode = node;
                        cNode *destNode = propertyInput -> node;
                        propertyInput -> Disconnect(false);
                        node -> DisconnectedFrom(this, destNode, propertyInput);
                    }
                    else
                    {
                        cNodeInput *input = manager -> GetInput(link -> to);
                        if(input != nullptr)
                        {
                            cNode *sourceNode = node;
                            cNode *destNode = propertyInput -> node;
                            input -> Disconnect(false);
                            node -> DisconnectedFrom(this, destNode, input);
                        }
                    }
                }
                links[i] = -1;
                manager -> RemoveLink(link_id);
            }
            break;
        }
    }
}

void cNodeOutput::DisconnectFrom(int child_id) //disconnect from this inlet
{
    int link_found = -1;
    
    for(int i = 0; i < links.size(); i ++)
    {
        int link_id = links[i];
        if(link_id != -1)
        {
            cNodeLink *link = GetLink(link_id);
            if(link != nullptr)
            {
                if(link -> to == child_id)
                {
                    links[i] = -1;
                    manager -> RemoveLink(link_id);
                }
            }
        }
    }
}
