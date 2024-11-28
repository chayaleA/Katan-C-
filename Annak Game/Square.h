#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <stack>
#include "Infrastructure.h"
#include "Manufacture.h"
#include "Person.h"

using namespace std;

class Square
{
public:
	stack<string> nameCategory;
	bool isTherePerson = false;
	unique_ptr<Person> person;
	string nameResource = "";
	shared_ptr<int> countResource;
	shared_ptr<Infrastructure> infrastructure = nullptr;
	shared_ptr<Manufacture> manufacture = nullptr;
	shared_ptr<Manufacture> helicopter = nullptr;

	Square() {
	}

	Square(string nameCategory)
	{
		this->nameCategory.push(nameCategory);
	}

	Square(const Square& other)
		:nameCategory(other.nameCategory),
		isTherePerson(other.isTherePerson),
		nameResource(other.nameResource),
		countResource(other.countResource),
		infrastructure(other.infrastructure),
		manufacture(other.manufacture),
		helicopter(other.helicopter) {
		if (other.person) {
			person = make_unique<Person>(*other.person);
		}
	}

	Square& operator=(const Square& other)
	{
		if (this == &other)
			return *this;

		nameCategory = other.nameCategory;
		isTherePerson = other.isTherePerson;
		nameResource = other.nameResource;
		countResource = other.countResource;
		infrastructure = other.infrastructure;
		manufacture = other.manufacture;
		helicopter = other.helicopter;

		if (other.person) {
			person = make_unique<Person>(*other.person);
		}
		return *this;
	}
};
