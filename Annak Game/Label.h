// Base class for all labels
#ifndef LABEL_H
#define LABEL_H

#include <opencv2/opencv.hpp>
class Label {
public:
    virtual void draw(cv::Mat& board) const = 0;
    virtual bool contains(const cv::Point& point) const = 0;
    virtual void startDragging(const cv::Point& point) = 0;
    virtual void drag(const cv::Point& point) = 0;
    virtual void stopDragging() = 0;
    virtual ~Label() {}
};

#endif
