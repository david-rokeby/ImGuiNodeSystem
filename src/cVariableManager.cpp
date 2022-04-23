//
//  cVariableManager.cpp
//  ImGuiTester
//
//  Created by David Rokeby on 2022-04-18.
//

#include "cVariableManager.hpp"
#include "cNode.hpp"
#include "cNodeInstances.hpp"

cVariableManager::cVariableManager(void)
{
    cGetVariableNode::variableManager = this;
    cSetVariableNode::variableManager = this;

}
