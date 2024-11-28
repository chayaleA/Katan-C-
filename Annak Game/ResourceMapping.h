#pragma once

#include <string>
#include <unordered_map>

using namespace std;

static const unordered_map<string, pair<string, string>> resourceMap = {
    {"Forest", {"Wood", "Forest"}},
    {"Field", {"Wool", "Field"}},
    {"IronMine", {"Iron", "IronMine"}},
    {"BlocksMine", {"Blocks", "BlocksMine"}}
};
