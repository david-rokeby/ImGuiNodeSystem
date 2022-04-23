//
//  JSON_utilities.h
//  mySketchimgui_test_2
//
//  Created by David Rokeby on 2022-04-12.
//

#ifndef JSON_utilities_h
#define JSON_utilities_h
#include "ofxJSON.h"

bool StringFromJSON(const Json::Value& container, std::string name, std::string &target);
bool IntFromJSON(const Json::Value& container, std::string name, int32_t& target);
bool FloatFromJSON(const Json::Value& container, std::string name, float& target);
bool BoolFromJSON(const Json::Value& container, std::string name, bool& target);

#endif /* JSON_utilities_h */
