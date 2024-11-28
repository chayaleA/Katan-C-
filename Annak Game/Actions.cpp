#include "Actions.h"

vector<vector<Square>> Actions::getWorldMap() const {
	return worldGame.getWorldMap();
}

int Actions::GetMatNumOfRows() const {
	int length = 0;
	for (const auto& command : in.start) {
		try {
			stoi(command->name);
			length++;
		}
		catch (const invalid_argument&) {
			break;
		}
	}
	return length;
}

int Actions::GetCountSetNameResourceForResource(Square& square) {
	const string& category = square.nameCategory.top();
	auto it = resourceMap.find(category);

	if (it != resourceMap.end()) {
		const pair<string, string>& resourcePair = it->second;
		square.nameResource = resourcePair.first;
		return r.getConfig()["StartingResources"][resourcePair.second];
	}
	else {
		return -1;
	}
}

void Actions::BuildWorld() {
	const auto& start = in.start;
	if (start.empty()) return;

	int length = GetMatNumOfRows();
	vector<vector<Square>> mat(length, vector<Square>(start[0]->arguments.size() + 1));

	for (int i = 0; i < length; ++i) {
		mat[i][0].nameCategory.push(worldGame.GetCategoryByNumber(stoi(start[i]->name)));

		auto countResources = make_shared<int>(GetCountSetNameResourceForResource(mat[i][0]));
		if (*countResources != -1) {
			mat[i][0].countResource = countResources;
		}

		for (int j = 0; j < start[i]->arguments.size(); ++j) {
			mat[i][j + 1].nameCategory.push(worldGame.GetCategoryByNumber(stoi(start[i]->arguments[j])));

			auto countResources2 = make_shared<int>(GetCountSetNameResourceForResource(mat[i][j + 1]));
			if (*countResources2 != -1) {
				mat[i][j + 1].countResource = countResources2;
			}
		}
	}

	worldGame.setWorldMap(mat);
}

void Actions::BuildStart() {
	const auto& start = in.start;
	vector<int> resources;
	string val1, val2, val3;

	for (const auto& command : start) {
		try {
			val1 = command->arguments.at(0);
			val2 = command->arguments.at(1);
			val3 = command->arguments.at(2);
		}
		catch (const out_of_range&) {
		}

		CommandType commandType = commandMap.count(command->name) ? commandMap[command->name] : UNKNOWN;

		switch (commandType) {
		case RESOURCE:
			worldGame.SetResource(stoi(val3), stoi(command->arguments[3]), stoi(val1), val2);
			break;

		case PEOPLE:
			worldGame.SetAmountOfPeopleInInfrastructureOrInSquare(stoi(val2), stoi(val3), stoi(val1));
			break;

		case BUILD:
			worldGame.SetInfrastructure(val1, stoi(val2), stoi(val3), r.getConfig()["Sizes"][val1], "Start");
			break;

		case MAKE_EMPTY:
			if (worldGame.GetCategoryByIndex(stoi(val1), stoi(val2)) == "City" || worldGame.GetCategoryByIndex(stoi(val1), stoi(val2)) == "Village") {
				worldGame.MakeEmptyInfrastructure(stoi(val1), stoi(val2));
			}
			else {
				worldGame.SetResource(stoi(val1), stoi(val2), 0);
			}
			break;

		case MANUFACTURE:
			worldGame.SetManufactureName(stoi(val2), stoi(val3), val1, "Start");
			break;

		case RESOURCES:
			for (int m = 0; m < 4; m++)
			{
				resources.push_back(stoi(command->arguments[m]));
			}
			worldGame.SetVectorOfResources(resources, stoi(command->arguments[4]), stoi(command->arguments[5]));
			break;

		case SET_POINTS:
			worldGame.SetPoints(stoi(val1));
			break;
		}
	}
}

pair<int, int> Actions::DoInput() {
	const auto& steps = in.steps;
	int num1 = 0, num2 = 0;
	string val1, val2, val3, itemToBuild, nameOfManufacture;
	pair<int, int> size, coordinates;

	for (const auto& step : steps) {
		try {
			val1 = step->arguments.at(0);
			val2 = step->arguments.at(1);
			val3 = step->arguments.at(2);
		}
		catch (const out_of_range&) {
		}

		CommandType commandType = commandMap.count(step->name) ? commandMap[step->name] : UNKNOWN;

		switch (commandType) {
		case SELECT:
			num1 = stoi(val1);
			num2 = stoi(val2);
			break;

		case WORK:
			if (worldGame.IsTherePersonInSquareByIndex(num1, num2)) {
				worldGame.Work(num1, num2, stoi(val1), stoi(val2));
			}
			break;

		case WAIT:
			for (int j = 0; j < stoi(val1); j++)
				;
			break;

		case RAIN:
			worldGame.UpdateResourcesDueToRain(stoi(val1));
			break;

		case BUILD:
			itemToBuild = val1;
			size = r.getConfig()["Sizes"][itemToBuild];
			worldGame.SetInfrastructure(itemToBuild, stoi(val2), stoi(val3), size, "Input");
			break;

		case PEOPLE:
			num1 = stoi(val2);
			num2 = stoi(val3);
			worldGame.SetAmountOfPeopleInInfrastructureOrInSquare(num1, num2, stoi(val1));
			break;

		case DEPOSIT:
			if (worldGame.GetCategoryByIndex(num1, num2) == "Ground" || worldGame.GetAmountOfPeople(num1, num2) > 0) {
				if (worldGame.HasAnyManufactureAt(num1, num2))
					worldGame.MoveManufactureToInfrastructure(num1, num2, stoi(val1), stoi(val2));
				else
					worldGame.MovePersonToInfrastructure(num1, num2, stoi(val1), stoi(val2));
			}
			break;

		case TAKE_RESOURCES:
			worldGame.TakeResourcesToManufacture(stoi(val1), stoi(val2), num1, num2);
			break;

		case MANUFACTURE:
			nameOfManufacture = val1;
			worldGame.SetManufactureName(stoi(val2), stoi(val3), nameOfManufacture, "Input");
			break;

		case MOVE:
			coordinates = worldGame.MoveManufactureOrPeopleToSquare(num1, num2, stoi(val1), stoi(val2));
			num1 = coordinates.first;
			num2 = coordinates.second;
			break;

		case UNKNOWN:
			break;
		}
	}
	return { num1, num2 };
}

void Actions::DoAction() {
	BuildWorld();
	BuildStart();

	pair<int, int> inputCoordinates = DoInput();
	int num1 = inputCoordinates.first, num2 = inputCoordinates.second;
	vector<int> resourcesArr;
	vector<string> asserts = in.asserts;

	for (const auto& command : asserts) {
		auto it = assertMap.find(command);

		if (it != assertMap.end()) {
			AssertType assertType = it->second;

			switch (assertType) {
			case SELECTED_CATEGORY:
				cout << "SelectedCategory " << worldGame.GetCategoryByIndex(num1, num2) << endl;
				break;

			case SELECTED_RESOURCE:
				resourcesArr = worldGame.GetVectorOfresources(num1, num2);
				cout << "SelectedResource ";
				for (const auto& resource : resourcesArr) {
					cout << resource << " ";
				}
				cout << endl;
				break;

			case CITY_COUNT:
				cout << "CityCount " << worldGame.CountInfrastructure("City") << endl;
				break;

			case ROAD_COUNT:
				cout << "RoadCount " << worldGame.CountInfrastructure("Road") << endl;
				break;

			case VILLAGE_COUNT:
				cout << "VillageCount " << worldGame.CountInfrastructure("Village") << endl;
				break;

			case SELECTED_COMPLETE:
				//���� �� ������� ����� ���� ���� ��JS
				cout << "SelectedComplete false" << endl;
				break;

			case SELECTED_PEOPLE:
				cout << "SelectedPeople " << worldGame.GetAmountOfPeople(num1, num2) << endl;
				break;

			case SELECTED_CAR:
				cout << "SelectedCar " << HasManufactureAt(num1, num2, "Car") << endl;
				break;

			case SELECTED_TRUCK:
				cout << "SelectedTruck " << HasManufactureAt(num1, num2, "Truck") << endl;
				break;

			case CAR_COUNT:
				cout << "CarCount " << worldGame.GetCountOfManufacture("Car") << endl;
				break;

			case TRUCK_COUNT:
				cout << "TruckCount " << worldGame.GetCountOfManufacture("Truck") << endl;
				break;

			case HELICOPTER_COUNT:
				cout << "HelicopterCount " << worldGame.GetCountOfManufacture("Helicopter") << endl;
				break;

			case SELECTED_COORDINATES:
			{
				cout << "SelectedCoordinates " << num1 << " " << num2 << endl;
			}
			break;

			case POINTS:
				cout << "Points " << worldGame.GetPoints() << endl;
				break;

			default:
				break;
			}
		}
	}
}

bool Actions::HasManufactureAt(int index1, int index2, const string& manufacture) {
	return worldGame.HasManufactureAt(index1, index2, manufacture);
}