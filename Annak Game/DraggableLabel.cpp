// DraggableLabel.cpp

#include "DraggableLabel.h"
//
//DraggableLabel::DraggableLabel(const cv::Rect& rect, const std::string& text)
//    : rect(rect), text(text), isDragging(false), offset(0, 0) {
//}
//
//void DraggableLabel::draw(cv::Mat& image) {
//     Draw the label on the image
//}
//
//bool DraggableLabel::contains(const cv::Point& point) {
//    return rect.contains(point);
//}
//
//void DraggableLabel::startDragging(const cv::Point& point) {
//    isDragging = true;
//    offset = rect.tl() - point;
//}
//
//void DraggableLabel::drag(const cv::Point& point) {
//    if (isDragging) {
//        rect.x = point.x + offset.x;
//        rect.y = point.y + offset.y;
//    }
//}
//
//void DraggableLabel::stopDragging() {
//    isDragging = false;
//}
//
//bool DraggableLabel::getIsDragging() const {
//    return isDragging;
//}
