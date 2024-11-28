#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include "Input.h"
#include "World.h"
#include "ReadJson.h"
#include "ResourceMapping.h"

using namespace std;

class Actions
{
private:
    ReadJson& r = ReadJson::getInstance();
    Input in;
    World worldGame;

    int GetMatNumOfRows() const;

public:
    Actions(){}
    Actions(Input in) : in(in) {}

    void setInput(Input in) {
        this->in = in;
    }

    vector<vector<Square>> getWorldMap() const;

    void BuildWorld();
    void BuildStart();
    pair<int, int> DoInput();
    void DoAction();
    int GetCountSetNameResourceForResource(Square& square);

    bool HasManufactureAt(int index1, int index2, const string& manufacture);
    enum CommandType {
        SELECT,
        RESOURCE,
        RESOURCES,
        WORK,
        WAIT,
        RAIN,
        BUILD,
        PEOPLE,
        DEPOSIT,
        TAKE_RESOURCES,
        MANUFACTURE,
        MAKE_EMPTY,
        MOVE,
        SET_POINTS,
        UNKNOWN
    };

    unordered_map<string, CommandType> commandMap = {
        {"Select", SELECT},
        {"Resource", RESOURCE},
        {"Resources", RESOURCES},
        {"Work", WORK},
        {"Wait", WAIT},
        {"Rain", RAIN},
        {"Build", BUILD},
        {"People", PEOPLE},
        {"Deposit", DEPOSIT},
        {"TakeResources", TAKE_RESOURCES},
        {"Manufacture", MANUFACTURE},
        {"MakeEmpty", MAKE_EMPTY},
        {"Move", MOVE},
        {"SetPoints", SET_POINTS}
    };

    enum AssertType {
        SELECTED_CATEGORY,
        SELECTED_RESOURCE,
        CITY_COUNT,
        ROAD_COUNT,
        VILLAGE_COUNT,
        SELECTED_COMPLETE,
        SELECTED_PEOPLE,
        SELECTED_CAR,
        SELECTED_TRUCK,
        CAR_COUNT,
        TRUCK_COUNT,
        HELICOPTER_COUNT,
        SELECTED_COORDINATES,
        POINTS
    };

    unordered_map<string, AssertType> assertMap = {
        {"SelectedCategory", SELECTED_CATEGORY},
        {"SelectedResource", SELECTED_RESOURCE},
        {"CityCount", CITY_COUNT},
        {"RoadCount", ROAD_COUNT},
        {"VillageCount", VILLAGE_COUNT},
        {"SelectedComplete", SELECTED_COMPLETE},
        {"SelectedPeople", SELECTED_PEOPLE},
        {"SelectedCar", SELECTED_CAR},
        {"SelectedTruck", SELECTED_TRUCK},
        {"CarCount", CAR_COUNT},
        {"TruckCount", TRUCK_COUNT},
        {"HelicopterCount", HELICOPTER_COUNT},
        {"SelectedCoordinates", SELECTED_COORDINATES},
        {"Points", POINTS}
    };
};
