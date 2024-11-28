#ifndef RESOURCES_LABEL_H
#define RESOURCES_LABEL_H

#include "DraggableLabel.h"

class ResourcesLabel : public DraggableLabel {
private:
    std::string resourceName;
    cv::Rect boundingBox; // Add boundingBox here

public:
    ResourcesLabel(const cv::Rect& rect, const std::string& name)
        : DraggableLabel(rect), resourceName(name), boundingBox(rect) {} // Initialize boundingBox in the constructor

    void draw(cv::Mat& board) const override {
        DraggableLabel::draw(board);
        cv::putText(board, resourceName, cv::Point(boundingBox.x, boundingBox.y - 10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255), 1);
    }
};

#endif // RESOURCES_LABEL_H
