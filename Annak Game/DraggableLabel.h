// DraggableLabel.h

#ifndef DRAGGABLE_LABEL_H
#define DRAGGABLE_LABEL_H

#include <opencv2/opencv.hpp>
#include "Label.h"

// Draggable label implementation
class DraggableLabel : public Label {
private:
    cv::Rect boundingBox;
    bool isDragging;
    cv::Point offset;

public:
    DraggableLabel(const cv::Rect& rect) : boundingBox(rect), isDragging(false) {}

    void draw(cv::Mat& board) const override {
        cv::rectangle(board, boundingBox, cv::Scalar(255, 255, 255), -1); // Filled rectangle
    }

    bool contains(const cv::Point& point) const override {
        return boundingBox.contains(point);
    }

    void startDragging(const cv::Point& point) override {
        isDragging = true;
        offset = point - boundingBox.tl(); // Calculate offset from top-left corner
    }

    void drag(const cv::Point& point) override {
        boundingBox.x = point.x - offset.x;
        boundingBox.y = point.y - offset.y;
    }

    void stopDragging() override {
        isDragging = false;
    }

    bool getIsDragging() const {
        return isDragging;
    }

    cv::Rect getBoundingBox() const {
        return boundingBox;
    }
};


#endif // DRAGGABLE_LABEL_H
