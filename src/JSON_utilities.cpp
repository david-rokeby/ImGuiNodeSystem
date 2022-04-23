//
//  JSON_utilities.cpp
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-12.
//

#include <stdio.h>
#include "JSON_utilities.h"

bool StringFromJSON(const Json::Value& container, std::string name, std::string &target)
{
    const Json::Value stringValue = container[name];
    if (stringValue.type() != Json::nullValue)
    {
        target = stringValue.asString();
        return true;
    }
    return false;
}

bool IntFromJSON(const Json::Value& container, std::string name, int32_t& target)
{
    const Json::Value intValue = container[name];
    if (intValue.type() != Json::nullValue)
    {
        target = intValue.asInt();
        return true;
    }
    return false;
}

bool FloatFromJSON(const Json::Value& container, std::string name, float& target)
{
    const Json::Value floatValue = container[name];
    if (floatValue.type() != Json::nullValue)
    {
        target = floatValue.asFloat();
        return true;
    }
    return false;
}

bool BoolFromJSON(const Json::Value& container, std::string name, bool& target)
{
    const Json::Value boolValue = container[name];
    if (boolValue.type() != Json::nullValue)
    {
        target = boolValue.asBool();
        return true;
    }
    return false;
}


