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

    // 保持宽高比缩放
    QRect targetRect = qImage.rect();
    targetRect.setSize(qImage.size().scaled(this->size(), Qt::KeepAspectRatioByExpanding));
    targetRect.moveCenter(rect().center());

    QPainter painter(this);
    painter.drawImage(targetRect, qImage, qImage.rect());
//    painter.drawImage(QPoint(0, 0), qImage);
}