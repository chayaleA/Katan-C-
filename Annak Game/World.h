#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include "ReadJson.h"
#include "Square.h"
#include "Infrastructure.h"

using namespace std;

class World
{
private:
	ReadJson& r = ReadJson::getInstance();
protected:
	vector<vector<Square>> worldMap;
	int rows;
	int columns;

	int countVillages = 0;
	int countRoads = 0;
	int countCities = 0;
	int points = 0;

	int carCount = 0;
	int truckCount = 0;
	int helicopterCount = 0;

public:
	void setWorldMap(const vector<vector<Square>>&);
	vector<vector<Square>> getWorldMap() const;
	string GetCategoryByIndex(int, int) const;
	string GetNameOfResource(int, int) const;
	void SetResource(int, int, int, const string& nameOfResource = "");
	string GetCategoryByNumber(int) const;
	void SetAmountOfPeopleInInfrastructureOrInSquare(int, int, int);
	void Work(int, int, int, int);
	int GetAmountOfResource(int, int, const string&);
	bool IsTherePersonInSquareByIndex(int, int);
	bool ValidToBuild(const string&, int, int, const pair<int, int>&);
	bool isAllTheAreaIsGroundToBuildOn(int, int, const pair<int, int>&);
	void SetInfrastructure(const string&, int, int, const pair<int, int>&, const string&);
	int CountInfrastructure(const string&) const;
	int GetAmountOfPeople(int, int) const;
	void MakeEmptyInfrastructure(int, int);
	void SetManufactureName(int, int, const string&, const string&);
	bool HasManufactureAt(int, int, const string&);
	bool HasAnyManufactureAt(int, int);
	void UpdateResourcesDueToRain(int);
	void MovePersonToInfrastructure(int, int, int, int);
	void MoveManufactureToInfrastructure(int, int, int, int);
	void TakeResourcesToManufacture(int, int, int, int);
	void SetVectorOfResources(vector<int>, int, int);
	int GetCountOfManufacture(string);
	bool IsThereManufacture(int, int, pair<int, int>, const string&);
	pair<int, int> MoveManufactureOrPeopleToSquare(int, int, int, int);
	vector<int> GetVectorOfresources(int, int);
	int GetPoints();
	void SetPoints(int);
};