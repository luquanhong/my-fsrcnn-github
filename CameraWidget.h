//
// Created by 卢泉鸿 on 2025/2/14.
//

#ifndef QT_OPENCV_CAMERA_APP_CAMERAWIDGET_H
#define QT_OPENCV_CAMERA_APP_CAMERAWIDGET_H

#include <QWidget>
#include <opencv2/opencv.hpp>

// 自定义的用于渲染画面的QWidget
class CameraWidget : public QWidget {
    Q_OBJECT
public:
    CameraWidget(QWidget *parent = nullptr);
    virtual ~CameraWidget();
    void setFrame(const cv::Mat& frame);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    cv::Mat currentFrame;
};


#endif //QT_OPENCV_CAMERA_APP_CAMERAWIDGET_H
