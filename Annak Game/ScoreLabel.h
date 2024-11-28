#ifndef SCORE_LABEL_H
#define SCORE_LABEL_H

#include "DraggableLabel.h"

class ScoreLabel : public DraggableLabel {
private:
    int score;
    cv::Rect boundingBox; // Add boundingBox here

public:
    ScoreLabel(const cv::Rect& rect, int initialScore)
        : DraggableLabel(rect), score(initialScore), boundingBox(rect) {} // Initialize boundingBox in the constructor

    void draw(cv::Mat& board) const override {
        DraggableLabel::draw(board);
        std::string scoreText = "Score: " + std::to_string(score);
        cv::putText(board, scoreText, cv::Point(boundingBox.x, boundingBox.y - 10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 255, 255), 1);
    }

    void updateScore(int newScore) {
        score = newScore;
    }
};

#endif // SCORE_LABEL_H
