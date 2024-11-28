#pragma once
#include <vector>
#include <stack>

using namespace std;

class Person
{
public:
	vector<int> resourcesArr;

	Person() :resourcesArr(4, 0) {
	}

	Person& operator=(Person& person) {
		for (int i = 0; i < person.resourcesArr.size(); i++)
		{
			resourcesArr[i] = person.resourcesArr[i];
		}
		return *this;
	}
};

