//
// Created by 卢泉鸿 on 2025/2/14.
//

#ifndef QT_OPENCV_CAMERA_APP_FRAMECAPTURETHREAD_H
#define QT_OPENCV_CAMERA_APP_FRAMECAPTURETHREAD_H

#include <QThread>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include <QQueue>
#include <QTimer>


// 线程类用于采集帧
class FrameCaptureThread : public QThread {
    Q_OBJECT
public:
    FrameCaptureThread(QMutex *mutex, QQueue<cv::Mat> *frameQueue, cv::VideoCapture *cap, QObject *parent = nullptr);
    ~FrameCaptureThread() override;

    void run() override;  // 添加 run 函数的声明
private slots:
    void captureFrame();
private:
    QMutex *mutex;
    QQueue<cv::Mat> *frameQueue;
    cv::VideoCapture *cap;
    QTimer captureTimer;
};



#endif //QT_OPENCV_CAMERA_APP_FRAMECAPTURETHREAD_H
