#include "World.h"

void World::setWorldMap(const vector<vector<Square>>& map) {
	const auto& config = r.getConfig();
	int tile_size_x = config["Sizes"]["Tile"][0].get<int>();
	int tile_size_y = config["Sizes"]["Tile"][1].get<int>();

	rows = map.size() * tile_size_x;
	columns = map[0].size() * tile_size_y;
	worldMap.resize(rows, vector<Square>(columns));

	for (int i = 0, row = -1; i < rows; ++i) {
		if (i % tile_size_x == 0) row++;
		for (int j = 0, column = -1; j < columns; ++j) {
			if (j % tile_size_y == 0) column++;
			worldMap[i][j] = map[row][column];
		}
	}
}

vector<vector<Square>> World::getWorldMap() const {
	return worldMap;
}

string World::GetCategoryByIndex(int row, int column) const {
	return (worldMap[column - 1][row - 1]).nameCategory.top();
}

string World::GetNameOfResource(int index1, int index2) const {
	return worldMap[index2 - 1][index1 - 1].nameResource;
}

void World::SetResource(int index1, int index2, int count, const string& nameOfResource) {
	if (GetCategoryByIndex(index1, index2) == "City" || GetCategoryByIndex(index1, index2) == "Village") {
		int index = r.getResourceTypes()[nameOfResource];
		int limitResource = r.getConfig()["Capacities"][GetCategoryByIndex(index1, index2)][index];
		if (limitResource >= count)
			worldMap[index2 - 1][index1 - 1].infrastructure->resourcesArr[index] = count;
		else
			worldMap[index2 - 1][index1 - 1].infrastructure->resourcesArr[index] = limitResource;
	}
	else if (worldMap[index2 - 1][index1 - 1].isTherePerson) {
		int index = r.getResourceTypes()[nameOfResource];
		worldMap[index2 - 1][index1 - 1].person->resourcesArr[index] = count;
	}
	else if (worldMap[index2 - 1][index1 - 1].nameCategory.top() == "Ground") {
		int index = r.getResourceTypes()[nameOfResource];
		if (worldMap[index2 - 1][index1 - 1].helicopter != nullptr) {
			if (r.getConfig()["Capacities"]["Helicopter"][index] >= count)
				worldMap[index2 - 1][index1 - 1].helicopter->resourcesArr[index] = count;
		}
		else if (worldMap[index2 - 1][index1 - 1].manufacture != nullptr)
			if (r.getConfig()["Capacities"][worldMap[index2 - 1][index1 - 1].manufacture->nameOfManufacture][index] >= count)
				worldMap[index2 - 1][index1 - 1].manufacture->resourcesArr[index] = count;
	}
	else
		*(worldMap[index2 - 1][index1 - 1].countResource) = count;
}

string World::GetCategoryByNumber(int num) const {
	if (num == -1) {
		return "Invalid index";
	}

	string category = "";
	auto tiles = r.getTiles();
	for (auto it = tiles.begin(); it != tiles.end(); ++it) {
		if (it->second == num) {
			category += it->first;
			break;
		}
	}
	return category;
}

void World::SetAmountOfPeopleInInfrastructureOrInSquare(int index1, int index2, int numOfPeople) {
	Square& cell = worldMap[index2 - 1][index1 - 1];
	if (cell.infrastructure != nullptr) {
		cell.infrastructure->people.clear();
		int limitPeople = r.getConfig()["Capacities"][GetCategoryByIndex(index1, index2)][4];
		int actualPeople = min(limitPeople, numOfPeople);
		cell.infrastructure->numOfPeople = actualPeople;
		for (int i = 0; i < actualPeople; i++) {
			cell.infrastructure->people.push_back(make_unique<Person>());
		}
	}
	else {
		cell.isTherePerson = true;
		cell.person = make_unique<Person>();
		cell.nameCategory.push("People");
	}
}

void World::Work(int fromIndex1, int fromIndex2, int toIndex1, int toIndex2) {
	auto& fromCell = worldMap[fromIndex2 - 1][fromIndex1 - 1];
	auto& toCell = worldMap[toIndex2 - 1][toIndex1 - 1];

	toCell.isTherePerson = true;
	toCell.person = move(fromCell.person);
	fromCell.isTherePerson = false;
	fromCell.nameCategory.pop();

	const string& toCategory = GetCategoryByIndex(toIndex1, toIndex2);

	if (toCategory == "City" || toCategory == "Village") {
		MovePersonToInfrastructure(fromIndex1, fromIndex2, toIndex1, toIndex2);
	}
	else {
		int resourceIndex = r.getResourceTypes()[GetNameOfResource(toIndex1, toIndex2)];
		// ��� ����� ��� ������ ��� ��� ���� ���� ��� ������� ��� �JSON
		toCell.person->resourcesArr[resourceIndex] = *(toCell.countResource);
		*(toCell.countResource) = 0;
	}
}

int World::GetAmountOfResource(int index1, int index2, const string& nameResources) {
	const auto& cell = worldMap[index2 - 1][index1 - 1];
	const auto& category = cell.nameCategory.top();

	if (category == "City" || category == "Village") {
		int resourceIndex = r.getResourceTypes()[nameResources];
		return cell.infrastructure->resourcesArr[resourceIndex];
	}

	return *(cell.countResource);
}

bool World::IsTherePersonInSquareByIndex(int index1, int index2) {
	return worldMap[index2 - 1][index1 - 1].isTherePerson;
}

bool World::ValidToBuild(const string& itemToBuild, int index1, int index2, const pair<int, int>& sizeOfInfrastructure) {
	if (index1 == 1 && index2 == 1) return true;

	int roadSize = r.getConfig()["Sizes"]["Road"][0].get<int>();
	int length, count;

	auto checkInfrastructure = [&](int i, int j) -> bool {
		if (worldMap[i][j].infrastructure != nullptr) {
			if (itemToBuild == "Road") count++;
			else if (worldMap[i][j].infrastructure->nameOfInfrastructure == "Road") count++;
		}
		else {
			count = 0;
		}
		return count == roadSize;
	};

	// Check left
	count = 0;
	length = index2 - 1 + sizeOfInfrastructure.first;
	for (int i = index2 - 1; i < length && index2 - 1 + length < rows && index1 - 2 >= 0; i++) {
		if (checkInfrastructure(i, index1 - 2)) return true;
	}

	// Check right
	count = 0;
	length = index2 - 1 + sizeOfInfrastructure.first;
	int column = index1 - 1 + sizeOfInfrastructure.second;
	for (int i = index2 - 1; i < length && index2 - 1 + length < rows && column < columns; i++) {
		if (checkInfrastructure(i, column)) return true;
	}

	// Check top
	count = 0;
	length = index1 - 1 + sizeOfInfrastructure.first;
	for (int i = index1 - 1; i < length && index1 - 1 + length < rows; i++) {
		if (checkInfrastructure(i, index1 - 2)) return true;
	}

	// Check bottom
	count = 0;
	length = index1 - 1 + sizeOfInfrastructure.first;
	int row = index2 - 1 + sizeOfInfrastructure.first;
	for (int i = index1 - 1; i < length && index1 - 1 + length < rows && row < rows; i++) {
		if (checkInfrastructure(i, index1 - 2)) return true;
	}

	return false;
}

bool World::isAllTheAreaIsGroundToBuildOn(int index1, int index2, const pair<int, int>& size) {
	const auto& tileSize = r.getConfig()["Sizes"]["Tile"];
	pair<int, int> sizeOfTile = { tileSize[0], tileSize[1] };

	for (int i = index2 - 1; i < index2 - 1 + size.first; i += sizeOfTile.first) {
		for (int j = index1 - 1; j < index1 - 1 + size.second; j += sizeOfTile.second) {
			if (worldMap[i][j].nameCategory.top() != "Ground") {
				return false;
			}
		}
	}
	return true;
}

void World::SetInfrastructure(const string& itemToBuild, int index1, int index2, const pair<int, int>& size, const string& stage) {
	if (!isAllTheAreaIsGroundToBuildOn(index1, index2, size)) {
		return;
	}

	if (stage == "Input" && !ValidToBuild(itemToBuild, index1, index2, size)) {
		return;
	}

	int countOfPeople = (itemToBuild != "Road") ? r.getConfig()["StartingResources"][itemToBuild].get<int>() : 0;

	shared_ptr<Infrastructure> infrastructure = make_shared<Infrastructure>(itemToBuild, countOfPeople);

	if (itemToBuild != "Road") {
		for (int i = 0; i < countOfPeople; ++i) {
			infrastructure->people.push_back(make_unique<Person>());
		}
	}

	for (int i = index2 - 1; i < index2 - 1 + size.first; ++i) {
		for (int j = index1 - 1; j < index1 - 1 + size.second; ++j) {
			worldMap[i][j].infrastructure = infrastructure;
			worldMap[i][j].nameCategory.push(itemToBuild);
		}
	}

	if (itemToBuild == "Road") {
		countRoads++;
	}
	else {
		if (itemToBuild == "Village") {
			countVillages++;
			if (points + 1 <= 100) points++;
		}
		else if (itemToBuild == "City") {
			countCities++;
			points += 2;
			if (points > 100)
				points = 100;
		}
	}
}

int World::CountInfrastructure(const string& infrastructure) const {
	if (infrastructure == "Road") {
		return countRoads;
	}
	else if (infrastructure == "Village") {
		return countVillages;
	}
	else if (infrastructure == "City") {
		return countCities;
	}
	else {
		throw invalid_argument("Unknown infrastructure type: " + infrastructure);
	}
}

int World::GetAmountOfPeople(int index1, int index2) const {
	const auto& cell = worldMap[index2 - 1][index1 - 1];
	if (cell.infrastructure != nullptr) {
		return cell.infrastructure->numOfPeople;
	}
	return (cell.isTherePerson) ? 1 : 0;
}

void World::MakeEmptyInfrastructure(int index1, int index2) {
	auto& cell = worldMap[index2 - 1][index1 - 1];
	cell.isTherePerson = false;
	cell.person = nullptr;

	if (cell.infrastructure != nullptr) {
		cell.infrastructure->numOfPeople = 0;
		cell.infrastructure->people.clear();
		cell.infrastructure->resourcesArr.assign(4, 0);
	}

	cell.manufacture = nullptr;
	cell.helicopter = nullptr;
}

void World::SetManufactureName(int index1, int index2, const string& name, const string& stage) {
	int length = r.getConfig()["Sizes"][name][0];
	int width = r.getConfig()["Sizes"][name][1];
	if (IsThereManufacture(index1, index2, { length, width }, name))
		return;
	if (stage == "Input") {
		vector<int> costsToBuild = r.getConfig()["Costs"][name];
		vector<int> resources = worldMap[index2 - 1][index1 - 1].infrastructure->resourcesArr;

		for (int i = 0; i < resources.size(); i++) {
			if (resources[i] < costsToBuild[i])
				return;
		}

		if (costsToBuild[4] > 1)
			return;
		else if (costsToBuild[4] == 1 && !worldMap[index2 - 1][index1 - 1].isTherePerson)
			return;
	}

	shared_ptr<Manufacture> manufacture(new Manufacture(name));

	if (name == "Car")
		carCount++;
	else if (name == "Truck")
		truckCount++;
	else if (name == "Helicopter")
		helicopterCount++;

	for (int i = index2 - 1; i < index2 - 1 + length; i++) {
		for (int j = index1 - 1; j < index1 - 1 + width; j++) {
			if (name == "Helicopter")
			{
				worldMap[i][j].helicopter = manufacture;

				worldMap[i][j].nameCategory.push("Helicopter");
			}
			else
			{
				worldMap[i][j].manufacture = manufacture;

				worldMap[i][j].nameCategory.push(name);
			}
		}
	}
}

bool World::HasManufactureAt(int index1, int index2, const string& manufacture) {
	auto& cell = worldMap[index2 - 1][index1 - 1];
	return (cell.manufacture != nullptr && cell.manufacture->nameOfManufacture == manufacture) || (cell.helicopter != nullptr
		&& manufacture == "Helicopter");
}

bool World::HasAnyManufactureAt(int index1, int index2) {
	return (worldMap[index2 - 1][index1 - 1].manufacture != nullptr) || (worldMap[index2 - 1][index1 - 1].helicopter != nullptr);
}

void World::UpdateResourcesDueToRain(int amount) {
	int sizeRowOfTile = r.getConfig()["Sizes"]["Tile"][0];
	int sizeColumnOfTile = r.getConfig()["Sizes"]["Tile"][1];

	int amountOfWoodPerRain = r.getConfig()["Rains"]["Wood"];
	int amountOfWoolPerRain = r.getConfig()["Rains"]["Wool"];

	for (int i = 0; i < rows; i += sizeRowOfTile)
	{
		for (int j = 0; j < columns; j += sizeColumnOfTile)
		{
			if (worldMap[i][j].nameCategory.top() == "Forest")
				*(worldMap[i][j].countResource) = amount / amountOfWoodPerRain;
			else if (worldMap[i][j].nameCategory.top() == "Field")
				*(worldMap[i][j].countResource) = amount / amountOfWoolPerRain;
		}
	}
}

void World::MovePersonToInfrastructure(int fromIndex1, int fromIndex2, int toIndex1, int toIndex2) {
	auto& fromCell = worldMap[fromIndex2 - 1][fromIndex1 - 1];
	auto& toCell = worldMap[toIndex2 - 1][toIndex1 - 1];

	if (GetCategoryByIndex(toIndex1, toIndex2) == "City" || GetCategoryByIndex(toIndex1, toIndex2) == "Village") {
		int limitPeople = r.getConfig()["Capacities"][GetCategoryByIndex(toIndex1, toIndex2)][4];
		if (toCell.infrastructure->numOfPeople < limitPeople) {
			toCell.infrastructure->people.push_back(move(fromCell.person));
			toCell.infrastructure->numOfPeople++;
		}
	}
	else {
		toCell.person = move(fromCell.person);
		toCell.isTherePerson = true;
		fromCell.isTherePerson = false;
		fromCell.nameCategory.pop();
	}
}

void World::MoveManufactureToInfrastructure(int fromIndex1, int fromIndex2, int toIndex1, int toIndex2) {
	if (GetCategoryByIndex(toIndex1, toIndex2) == "City" || GetCategoryByIndex(toIndex1, toIndex2) == "Village") {
		shared_ptr<Manufacture> fromManufacture;
		if (worldMap[fromIndex2 - 1][fromIndex1 - 1].manufacture != nullptr)
			fromManufacture = worldMap[fromIndex2 - 1][fromIndex1 - 1].manufacture;
		else if (worldMap[fromIndex2 - 1][fromIndex1 - 1].helicopter != nullptr)
			fromManufacture = worldMap[fromIndex2 - 1][fromIndex1 - 1].helicopter;
		auto& toInfrastructure = worldMap[toIndex2 - 1][toIndex1 - 1].infrastructure;

		vector<int>& resources = fromManufacture->resourcesArr;
		vector<int> capacities = r.getConfig()["Capacities"][GetCategoryByIndex(toIndex1, toIndex2)];

		for (size_t i = 0; i < resources.size(); ++i) {
			int availableCapacity = capacities[i] - toInfrastructure->resourcesArr[i];
			int transferAmount = min(resources[i], availableCapacity);

			toInfrastructure->resourcesArr[i] += transferAmount;
			fromManufacture->resourcesArr[i] -= transferAmount;
		}
	}
}

void World::TakeResourcesToManufacture(int fromIndex1, int fromIndex2, int toIndex1, int toIndex2) {
	auto& fromInfrastructure = worldMap[fromIndex2 - 1][fromIndex1 - 1].infrastructure;
	shared_ptr<Manufacture> toManufacture;
	if (worldMap[toIndex2 - 1][toIndex1 - 1].manufacture != nullptr)
		toManufacture = worldMap[toIndex2 - 1][toIndex1 - 1].manufacture;
	else if (worldMap[toIndex2 - 1][toIndex1 - 1].helicopter != nullptr)
		toManufacture = worldMap[toIndex2 - 1][toIndex1 - 1].helicopter;

	if (fromInfrastructure == nullptr || toManufacture == nullptr)
		return;

	vector<int>& resources = fromInfrastructure->resourcesArr;
	vector<int>& manufactureResources = toManufacture->resourcesArr;
	string manufactureName = toManufacture->nameOfManufacture;
	vector<int> capacities = r.getConfig()["Capacities"][manufactureName];

	for (size_t i = 0; i < resources.size(); ++i) {
		int amountInManufacture = manufactureResources[i];
		int amountInInfrastructure = resources[i];
		int capacity = capacities[i];

		if (amountInManufacture + amountInInfrastructure <= capacity) {
			manufactureResources[i] += amountInInfrastructure;
			resources[i] = 0;
		}
		else {
			int amountToTake = capacity - amountInManufacture;
			manufactureResources[i] = capacity;
			resources[i] -= amountToTake;
		}
	}
}

void World::SetVectorOfResources(vector<int> resources, int index1, int index2) {
	string category = GetCategoryByIndex(index1, index2);
	if (category == "City" || category == "Village") {
		auto& infrastructure = worldMap[index2 - 1][index1 - 1].infrastructure;
		vector<int> capacity = r.getConfig()["Capacities"][category];
		for (size_t i = 0; i < resources.size(); ++i) {
			infrastructure->resourcesArr[i] = min(resources[i], capacity[i]);
		}
	}
	else if (category == "Ground") {
		shared_ptr<Manufacture> manufacture;
		if (worldMap[index2 - 1][index1 - 1].manufacture != nullptr)
			manufacture = worldMap[index2 - 1][index1 - 1].manufacture;
		else if (worldMap[index2 - 1][index1 - 1].helicopter != nullptr)
			manufacture = worldMap[index2 - 1][index1 - 1].helicopter;
		else
			return;
		for (size_t i = 0; i < resources.size(); ++i) {
			manufacture->resourcesArr[i] = resources[i];
		}
	}
}

int World::GetCountOfManufacture(string manufacture) {
	if (manufacture == "Car")
		return carCount;
	else if (manufacture == "Truck")
		return truckCount;
	return helicopterCount;
}

bool World::IsThereManufacture(int index1, int index2, pair<int, int> size, const string& nameManufacture) {
	int startRow = index2 - 1;
	int startColumn = index1 - 1;
	int endRow = startRow + size.first;
	int endColumn = startColumn + size.second;

	for (int i = startRow; i < endRow; ++i) {
		for (int j = startColumn; j < endColumn; ++j) {
			if (worldMap[i][j].manufacture != nullptr) {
				return true;
			}
		}
	}
	return false;
}

pair<int, int> World::MoveManufactureOrPeopleToSquare(int fromIndex1, int fromIndex2, int toIndex1, int toIndex2) {
	string fromCategory = GetCategoryByIndex(fromIndex1, fromIndex2);
	string toCategory = GetCategoryByIndex(toIndex1, toIndex2);

	if (worldMap[fromIndex2 - 1][fromIndex1 - 1].manufacture != nullptr || worldMap[fromIndex2 - 1][fromIndex1 - 1].helicopter != nullptr) {
		string nameManufacture;
		shared_ptr<Manufacture> manufacture;
		if (worldMap[fromIndex2 - 1][fromIndex1 - 1].manufacture != nullptr) {
			nameManufacture = worldMap[fromIndex2 - 1][fromIndex1 - 1].manufacture->nameOfManufacture;
			manufacture = worldMap[fromIndex2 - 1][fromIndex1 - 1].manufacture;
		}
		else {
			nameManufacture = "Helicopter";
			manufacture = worldMap[fromIndex2 - 1][fromIndex1 - 1].helicopter;
		}

		int sizeOfRows = r.getConfig()["Sizes"][nameManufacture][0];
		int sizeOfColumns = r.getConfig()["Sizes"][nameManufacture][1];

		if (IsThereManufacture(toIndex1, toIndex2, { sizeOfRows, sizeOfColumns }, nameManufacture))
			toIndex1 -= sizeOfColumns;

			for (int i = toIndex2 - 1; i < toIndex2 - 1 + sizeOfRows; i++) {
				for (int j = toIndex1 - 1; j < toIndex1 - 1 + sizeOfColumns; j++) {
					worldMap[i][j].manufacture = manufacture;
				}
			}

		for (int i = fromIndex2 - 1; i < fromIndex2 - 1 + sizeOfRows; i++)
		{
			for (int j = fromIndex1; j < fromIndex2 - 1 + sizeOfColumns; j++)
			{
				for (int m = -1 * sizeOfRows; m < sizeOfRows; m++)
				{
					for (int k = -1 * sizeOfColumns; k < sizeOfColumns; k++)
					{
						if (i + m >= 0 && i + m < rows && j + k >= 0 && j + k < columns)
							if (worldMap[i + m][j + k].manufacture == manufacture)
								worldMap[i + m][j + k].manufacture = nullptr;
					}
				}
			}
		}
	}
	else if (fromCategory == "People" && toCategory != "Water") {
		if (toCategory == "City" || toCategory == "Village") {
			MovePersonToInfrastructure(fromIndex1, fromIndex2, toIndex1, toIndex2);
		}
		else {
			if (worldMap[toIndex2 - 1][toIndex1 - 1].isTherePerson)
				if (toIndex1 == 1 || toIndex2 == 1)
					return { fromIndex1 , fromIndex2 };
				else
					//���� �� �������� ����� ��� �� ����� ����
					toIndex1--;
			worldMap[fromIndex2 - 1][fromIndex1 - 1].isTherePerson == false;
			worldMap[fromIndex2 - 1][fromIndex1 - 1].nameCategory.pop();
			worldMap[toIndex2 - 1][toIndex1 - 1].person = move(worldMap[fromIndex2 - 1][fromIndex1 - 1].person);
			worldMap[toIndex2 - 1][toIndex1 - 1].isTherePerson = true;
			worldMap[toIndex2 - 1][toIndex1 - 1].nameCategory.push("People");
		}
	}
	return { toIndex1, toIndex2 };
}

vector<int> World::GetVectorOfresources(int index1, int index2) {
	if (index1 == 0 && index2 == 0) {
		return vector<int>(4, 0);
	}

	string category = GetCategoryByIndex(index1, index2);

	if (category == "City" || category == "Village") {
		return worldMap[index2 - 1][index1 - 1].infrastructure->resourcesArr;
	}
	else if (worldMap[index2 - 1][index1 - 1].manufacture != nullptr) {
		return worldMap[index2 - 1][index1 - 1].manufacture->resourcesArr;
	}
	else if (worldMap[index2 - 1][index1 - 1].isTherePerson) {
		return worldMap[index2 - 1][index1 - 1].person->resourcesArr;
	}

	vector<int> resourcesArr(4, 0);
	int index = r.getResourceTypes()[GetNameOfResource(index1, index2)];
	resourcesArr[index] = GetAmountOfResource(index1, index2, GetNameOfResource(index1, index2));
	return resourcesArr;
}

int World::GetPoints() {
	return points;
}

void World::SetPoints(int points) {
	if (points <= 100)
		this->points = points;
}