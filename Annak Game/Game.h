#pragma once
#ifdef _DEBUG
#pragma comment(lib, "opencv_world4100d.lib")
#else
#pragma comment(lib, "opencv_world4100.lib")
#endif

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc.hpp>
#include "Input.h"
#include "Actions.h"
#include "Square.h"
#include "ReadJson.h"
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <fstream> 
#include <random>
#include "DraggableLabel.h"
#include "ResourcesLabel.h"
#include "Label.h"

using namespace cv;
using namespace std;

class Game
{
private:
    //std::vector<std::unique_ptr<Label>> labels;
    map<string, Mat> tileImages;
    Mat board;
    Actions actions;
    unordered_map<string, pair<Point, Point>> objectPositions;
    unordered_map<string, bool> isMoving;

   // ResourcesLabel* resourcesLabel;
public:
    Game();
    void handleMouseEvent(int event, int x, int y, int flags);
    void moveObjects();

    //string getTileCategory(const Point& pos);
    //void drawLabels();

    
    Point generateRandomPosition(int boardWidth, int boardHeight, mt19937& gen);

    bool moveObjectStep(Point& current, const Point& target, const string& category, Mat& boardSnapshot);

    Mat readPicture(string path);

    void loadTileImages();

    Mat createBoard();

    void drawLabels(cv::Mat& board);
};

