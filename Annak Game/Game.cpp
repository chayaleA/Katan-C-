#include "Game.h"

#pragma region Declare variables
bool continueRunning = true;
ReadJson& r = ReadJson::getInstance();

vector<vector<Square>> worldMap;
int boardWidth;
int boardHeight;

int boardWithFramesWidth;
int boardWithFramesHeight;

int squareSize = 50;
int frameSize = 2;

pair<int, int> tileSize = r.getConfig()["Sizes"]["Tile"];
pair<int, int> citySize = r.getConfig()["Sizes"]["City"];
pair<int, int> villageSize = r.getConfig()["Sizes"]["Village"];
pair<int, int> carSize = r.getConfig()["Sizes"]["Car"];
pair<int, int> truckSize = r.getConfig()["Sizes"]["Truck"];
pair<int, int> helicopterSize = r.getConfig()["Sizes"]["Helicopter"];

int cityWidth = citySize.first * (squareSize + frameSize) - frameSize;
int cityHeight = citySize.second * (squareSize + frameSize) - frameSize;
int villageWidth = villageSize.first * (squareSize + frameSize) - frameSize;
int villageHeight = villageSize.second * (squareSize + frameSize) - frameSize;
int carWidth = carSize.first * (squareSize + frameSize) - frameSize;
int carHeight = carSize.second * (squareSize + frameSize) - frameSize;
int truckWidth = truckSize.first * (squareSize + frameSize) - frameSize;
int truckHeight = truckSize.second * (squareSize + frameSize) - frameSize;
int helicopterWidth = helicopterSize.first * (squareSize + frameSize) - frameSize;
int helicopterHeight = helicopterSize.second * (squareSize + frameSize) - frameSize;

#pragma endregion

void CallBackFunc(int event, int x, int y, int flags, void* userdata) {
	Game* game = reinterpret_cast<Game*>(userdata);
	game->handleMouseEvent(event, x, y, flags);
}

Game::Game() {
	ifstream inputFile("C:/Users/user1/Downloads/input.txt");
	Input in;
	in.parse_and_store(inputFile);
	actions.setInput(in);
	actions.BuildWorld();
	actions.BuildStart();

	worldMap = actions.getWorldMap();
	boardWidth = worldMap[0].size();
	boardHeight = worldMap.size();

	loadTileImages();
	board = createBoard();


	/* cv::Rect resourcesRect(10, 10, 100, 50);
	 labels.push_back(std::make_unique<ResourcesLabel>(resourcesRect, "Resources"));*/


	namedWindow("KATAN GAME BOARD", WINDOW_NORMAL);
	setMouseCallback("KATAN GAME BOARD", CallBackFunc, this);
	setWindowProperty("KATAN GAME BOARD", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

	imshow("KATAN GAME BOARD", board);

	while (continueRunning) {
		moveObjects();

		int key = waitKey(10000);
		if (key == 27)
			break;
	}
	int key = waitKey(10000);
	if (key == 27)
		return;

	destroyAllWindows();
}

Point Game::generateRandomPosition(int boardWidth, int boardHeight, mt19937& gen) {
	uniform_int_distribution<> disX(0, boardWidth - 1);
	uniform_int_distribution<> disY(0, boardHeight - 1);

	int x = disX(gen) * (squareSize + frameSize) + frameSize;
	int y = disY(gen) * (squareSize + frameSize) + frameSize;

	return Point(x, y);
}

bool Game::moveObjectStep(Point& current, const Point& target, const string& category, Mat& boardSnapshot) {
	int dx = abs(target.x - current.x);
	int dy = abs(target.y - current.y);
	int sx = (current.x < target.x) ? 1 : -1;
	int sy = (current.y < target.y) ? 1 : -1;
	int err = dx - dy;

	int e2 = 2 * err;
	if (e2 > -dy) {
		err -= dy;
		current.x += sx;
	}
	if (e2 < dx) {
		err += dx;
		current.y += sy;
	}

	Mat tile;
	if (category == "People") {
		static int step = 0;
		string basePath = "C:/Users/user1/Downloads/People/WALKING ASTRONOUT/" + to_string(step) + ".png";
		tile = readPicture(basePath);
		resize(tile, tile, Size(squareSize, squareSize));

		// Rotate the tile to face the walking direction
		double angle = atan2(target.y - current.y, target.x - current.x) * 180 / CV_PI;
		Mat rotMatrix = getRotationMatrix2D(Point(tile.cols / 2, tile.rows / 2), angle, 1.0);
		warpAffine(tile, tile, rotMatrix, tile.size());

		step = (step + 1) % 11; // Assumes 11 frames of animation
	}
	else {
		tile = tileImages[category];
		resize(tile, tile, Size(squareSize, squareSize));
	}

	int x_end = current.x + tile.cols;
	int y_end = current.y + tile.rows;
	if ((x_end > boardSnapshot.cols) || (y_end > boardSnapshot.rows)) {
		return false;
	}

	Rect roiRect(current, tile.size());
	tile.copyTo(boardSnapshot(roiRect));

	return current == target;
}

void Game::moveObjects() {
	vector<tuple<Point, Point, string, bool>> objectsToMove;
	random_device rd;
	mt19937 gen(rd());

	for (int row = 0; row < worldMap.size(); ++row) {
		for (int col = 0; col < worldMap[0].size(); ++col) {
			string category = worldMap[row][col].nameCategory.top();

			if (category == "Car" || category == "Truck" || category == "Helicopter" || category == "People") {
				bool isTopLeftSquare = (row == 0 || worldMap[row - 1][col].nameCategory.top() != category) &&
					(col == 0 || worldMap[row][col - 1].nameCategory.top() != category);

				if (isTopLeftSquare) {
					//worldMap[row][col].nameCategory.pop();
					Point current(col * (squareSize + frameSize) + frameSize, row * (squareSize + frameSize) + frameSize);
					Point target = generateRandomPosition(boardWidth, boardHeight, gen);
					objectsToMove.emplace_back(current, target, category, false);
				}
			}
		}
	}
	for (const auto& obj : objectsToMove) {
		const Point& source = get<0>(obj);
		int col = (source.x - frameSize) / (squareSize + frameSize);
		int row = (source.y - frameSize) / (squareSize + frameSize);
		const string& category = get<2>(obj);
		pair<int, int> size = r.getConfig()["Sizes"][category];

		for (int i = 0; i < size.first; ++i) {
			for (int j = 0; j < size.second; ++j) {
				worldMap[row + i][col + j].nameCategory.pop();
			}
		}
	}

	// Move all objects simultaneously until all reach their targets
	bool allObjectsReachedTarget = false;
	while (!allObjectsReachedTarget) {
		Mat boardSnapshot = createBoard();

		allObjectsReachedTarget = true; // Assume all objects reached target until proven otherwise
		for (auto& obj : objectsToMove) {
			int k = waitKey(1);
			if (k == 27)
				return;
			Point& current = get<0>(obj);
			const Point& target = get<1>(obj);
			string& category = get<2>(obj);
			bool& reachedDestination = get<3>(obj);

			// If object hasn't reached target, move it one step closer
			if (!reachedDestination) {
				if (moveObjectStep(current, target, category, boardSnapshot)) {
					const Point& dest = get<1>(obj);
					const string& category = get<2>(obj);
					pair<int, int> size = r.getConfig()["Sizes"][category];

					for (int i = 0; i < size.first; ++i) {
						for (int j = 0; j < size.second; ++j) {
							worldMap[dest.x + i][dest.y + j].nameCategory.push(category);
						}
					}

					int k = waitKey(1);
					if (k == 27)
						return;
					reachedDestination = true; // Mark as reached destination
				}
				allObjectsReachedTarget = false; // There are still objects moving
			}
		}

		// Update the board with the new positions
		boardSnapshot.copyTo(board);
		imshow("KATAN GAME BOARD", board);
		waitKey(10);
	}

	//Place objects on the board once they've reached their targets
	for (const auto& obj : objectsToMove) {
		const Point& target = get<1>(obj);
		const string& category = get<2>(obj);
		Mat tile = tileImages[category];
		Rect roiRect(target, tile.size());
		tile.copyTo(board(roiRect));
	}

	imshow("KATAN GAME BOARD", board);
	waitKey(10);
}

Mat Game::readPicture(string path) {
	Mat picture = imread(path, IMREAD_COLOR);
	if (picture.empty()) {
		cerr << "Could not create the board!" << endl;
		exit(1);
	}
	return picture;
}

void Game::loadTileImages() {
	string basePath = "C:/Users/user1/Downloads/TILES/TILES/";

	tileImages["Ground"] = readPicture(basePath + "tile_ground.png");
	tileImages["Water"] = readPicture(basePath + "tile_water.png");
	tileImages["Forest"] = readPicture(basePath + "tile_forest.png");
	tileImages["Field"] = readPicture(basePath + "tile_field.png");
	tileImages["IronMine"] = readPicture(basePath + "tile_iron_mine.png");
	tileImages["BlocksMine"] = readPicture(basePath + "tile_blocks_mine.png");
	tileImages["Road"] = readPicture(basePath + "tile_road.png");

	basePath = "C:/Users/user1/Downloads/Settlements/Settlements/";

	tileImages["City"] = readPicture(basePath + "city.png");
	tileImages["Village"] = readPicture(basePath + "village.png");

	basePath = "C:/Users/user1/Downloads/People/WALKING ASTRONOUT/";
	tileImages["People"] = readPicture(basePath + "4.png");

	basePath = "C:/Users/user1/Downloads/VEHICLES/VEHICLES/";
	tileImages["Car"] = readPicture(basePath + "Car.png");
	tileImages["Truck"] = readPicture(basePath + "truck.png");
	tileImages["Helicopter"] = readPicture(basePath + "helicopter.png");

	resize(tileImages["City"], tileImages["City"], Size(cityWidth, cityHeight));
	resize(tileImages["Village"], tileImages["Village"], Size(villageWidth, villageHeight));
	resize(tileImages["Car"], tileImages["Car"], Size(carWidth, carHeight));
	resize(tileImages["Truck"], tileImages["Truck"], Size(truckWidth, truckHeight));
	resize(tileImages["Helicopter"], tileImages["Helicopter"], Size(helicopterWidth, helicopterHeight));
}

Mat Game::createBoard() {
	boardWithFramesWidth = boardWidth * squareSize + (boardWidth + 1) * frameSize;
	boardWithFramesHeight = boardHeight * squareSize + (boardHeight + 1) * frameSize;

	Mat board(boardWithFramesHeight, boardWithFramesWidth, CV_8UC3, Scalar(0, 0, 0));

	for (int row = 0; row < boardHeight; row += tileSize.first) {
		for (int col = 0; col < boardWidth; col += tileSize.second) {
			int x = col * (squareSize + frameSize);
			int y = row * (squareSize + frameSize);
			Rect frameRect(x, y, tileSize.first * squareSize + (tileSize.first - 1) * frameSize, tileSize.second * squareSize + (tileSize.second - 1) * frameSize);
			rectangle(board, frameRect, Scalar(255, 0, 0), frameSize);
		}
	}

	for (int row = 0; row < boardHeight; ++row) {
		for (int col = 0; col < boardWidth; ++col) {
			string category = worldMap[row][col].nameCategory.top();
			if (tileImages.count(category)) {
				Mat tile = tileImages[category];

				int x = col * (squareSize + frameSize) + frameSize;
				int y = row * (squareSize + frameSize) + frameSize;

				if (category == "City" || category == "Village" || category == "Car" || category == "Truck" || category == "Helicopter") {
					bool isTopLeftSquare = (row == 0 || worldMap[row - 1][col].nameCategory.top() != category) &&
						(col == 0 || worldMap[row][col - 1].nameCategory.top() != category);

					if (isTopLeftSquare) {
						pair<int, int> size = r.getConfig()["Sizes"][category];

						int objectWidth = size.first * (squareSize + frameSize) - frameSize;
						int objectHeight = size.second * (squareSize + frameSize) - frameSize;

						resize(tile, tile, Size(objectWidth, objectHeight));
						Rect roiRect(x, y, objectWidth, objectHeight);
						tile.copyTo(board(roiRect));
						col += size.first - 1;
					}
				}
				else {
					resize(tile, tile, Size(squareSize, squareSize));
					Rect roiRect(x, y, squareSize, squareSize);
					tile.copyTo(board(roiRect));
				}
			}
		}
	}

	return board;
}

void Game::handleMouseEvent(int event, int x, int y, int flags) {
	static Point topLeft, bottomRight;
	static bool rectangleDrawn = false;

	if (event == EVENT_LBUTTONDOWN) {
		topLeft = Point(x, y);
		rectangleDrawn = false;  // Reset rectangle drawn flag
	}
	else if (event == EVENT_LBUTTONUP) {
		bottomRight = Point(x, y);

		// Ensure top-left and bottom-right are within bounds
		if (topLeft.x < 0) topLeft.x = 0;
		if (topLeft.y < 0) topLeft.y = 0;
		if (bottomRight.x >= boardWithFramesWidth) bottomRight.x = boardWithFramesWidth - 1;
		if (bottomRight.y >= boardWithFramesHeight) bottomRight.y = boardWithFramesHeight - 1;

		// Draw rectangle on the board to visualize selection
		rectangle(board, topLeft, bottomRight, Scalar(0, 255, 0), 2);
		imshow("KATAN GAME BOARD", board);
		rectangleDrawn = true;  // Set rectangle drawn flag
	}
	else if (event == EVENT_LBUTTONDBLCLK && rectangleDrawn) {
		// Double click to move selected objects to this location
		Point destination(x, y);

		// Iterate through selected objects within the rectangle
		for (int row = 0; row < worldMap.size(); ++row) {
			for (int col = 0; col < worldMap[0].size(); ++col) {
				int objX = col * (squareSize + frameSize) + frameSize;
				int objY = row * (squareSize + frameSize) + frameSize;

				// Check if object's position is within the drawn rectangle
				if (objX >= topLeft.x && objX <= bottomRight.x && objY >= topLeft.y && objY <= bottomRight.y) {
					string category = worldMap[row][col].nameCategory.top();
					if (category == "Car" || category == "Truck" || category == "Helicopter" || category == "People") {
						Point current(objX, objY);
						Point target(destination.x, destination.y);
						moveObjectStep(current, target, category, board);
					}
				}
			}
		}

		imshow("KATAN GAME BOARD", board);
		waitKey(10); // Wait for a moment after moving objects
	}
}

//void Game::handleMouseEvent(int event, int x, int y, int flags) {
//    switch (event) {
//    case cv::EVENT_LBUTTONDOWN:
//        for (auto label : labels) {
//            if (label->contains(cv::Point(x, y))) {
//                label->startDragging(cv::Point(x, y));
//                break; // Only drag one label at a time
//            }
//        }
//        break;
//    case cv::EVENT_MOUSEMOVE:
//        for (auto label : labels) {
//            if (label->getIsDragging()) { // Use getIsDragging method
//                label->drag(cv::Point(x, y));
//                break; // Only drag one label at a time
//            }
//        }
//        break;
//    case cv::EVENT_LBUTTONUP:
//        for (auto label : labels) {
//            if (label->getIsDragging()) { // Use getIsDragging method
//                label->stopDragging();
//                break; // Only drag one label at a time
//            }
//        }
//        break;
//    }
//}
//
//void Game::drawLabels() {
//    board = board.clone(); // Refresh board
//    for (auto label : labels) {
//        label->draw(board);
//    }
//    cv::imshow("Game Board", board);
//    cv::waitKey(1);
//}

//
//void Game::drawLabels(cv::Mat& board) {
//    for (const auto& label : labels) {
//        label->draw(board);
//    }
//}
//
//void Game::handleMouseEvent(int event, int x, int y, int flags) {
//    switch (event) {
//    case cv::EVENT_LBUTTONDOWN:
//        for (auto& label : labels) {
//            if (label->contains(cv::Point(x, y))) {
//                label->startDragging(cv::Point(x, y));
//                break; // Only drag one label at a time
//            }
//        }
//        break;
//    case cv::EVENT_MOUSEMOVE:
//        for (auto& label : labels) {
//            if (label->getIsDragging()) {
//                label->drag(cv::Point(x, y));
//                break; // Only drag one label at a time
//            }
//        }
//        break;
//    case cv::EVENT_LBUTTONUP:
//        for (auto& label : labels) {
//            if (label->getIsDragging()) {
//                label->stopDragging();
//                break; // Only drag one label at a time
//            }
//        }
//        break;
//    }
//}