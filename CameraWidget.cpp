//
// Created by 卢泉鸿 on 2025/2/14.
//

#include "CameraWidget.h"
#include <opencv2/opencv.hpp>
#include <QPainter>
#include <QImage>
#include <QPaintEvent>
#include <QWidget>


CameraWidget::CameraWidget(QWidget *parent) : QWidget(parent) {}

CameraWidget::~CameraWidget() {}

void CameraWidget::setFrame(const cv::Mat& frame) {
    currentFrame = frame;
    update();
}

void CameraWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    if (currentFrame.empty()) return;
    cv::Mat displayFrame;
    if (currentFrame.channels() == 1) {
        cv::cvtColor(currentFrame, displayFrame, cv::COLOR_GRAY2RGB);
    } else {
        displayFrame = currentFrame;
    }
    QImage qImage((const unsigned char*)(displayFrame.data),
                  displayFrame.cols, displayFrame.rows,
                  displayFrame.cols * displayFrame.channels(),
                  QImage::Format_RGB888);
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), qImage);
}