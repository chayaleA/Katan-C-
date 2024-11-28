#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

class ReadJson {
public:
    static ReadJson& getInstance() {
        static ReadJson instance;
        return instance;
    }

    ReadJson(const ReadJson&) = delete;
    void operator=(const ReadJson&) = delete;

    const json& getConfig() const {
        return config;
    }

    unordered_map<string, int> getTiles() const {
        unordered_map<string, int> tiles;
        if (config.contains("Tiles")) {
            for (const auto& item : config["Tiles"].items()) {
                tiles[item.key()] = item.value().get<int>();
            }
        }
        return tiles;
    }

    unordered_map<string, int> getResourceTypes() const {
        unordered_map<string, int> resourceToIndex;
        if (config.contains("ResourceTypes")) {
            vector<string> resourcesTypes = config["ResourceTypes"].get<vector<string>>();
            for (size_t i = 0; i < resourcesTypes.size(); i++) {
                resourceToIndex[resourcesTypes[i]] = i;
            }
        }
        return resourceToIndex;
    }

private:
    json config;

    ReadJson() {
        ifstream f("configuration.json");
        if (!f.is_open()) {
            throw runtime_error("Failed to open configuration file");
        }
        try {
            f >> config;
        } catch (const json::parse_error& e) {
            throw runtime_error("Failed to parse configuration file: " + string(e.what()));
        }
    }
};
