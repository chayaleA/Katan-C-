#pragma once
#include <string>
#include <vector>
using namespace std;

class Manufacture
{
public:
	string nameOfManufacture;
	vector<int> resourcesArr;

	Manufacture(string nameOfManufacture) : resourcesArr(4, 0) {
		this->nameOfManufacture = nameOfManufacture;
	}
};

