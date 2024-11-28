#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Person.h"

using namespace std;

class Infrastructure
{
public:
	string nameOfInfrastructure;
	int numOfPeople;
	vector<int> resourcesArr;
	vector<unique_ptr<Person>> people;

	Infrastructure(string nameOfInfrastructure, int numOfPeople) :resourcesArr(4, 0) {
		this->nameOfInfrastructure = nameOfInfrastructure;
		this->numOfPeople = numOfPeople;
	}
};

