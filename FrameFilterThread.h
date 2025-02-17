//
// Created by 卢泉鸿 on 2025/2/14.
//

#ifndef QT_OPENCV_CAMERA_APP_FRAMEFILTERTHREAD_H
#define QT_OPENCV_CAMERA_APP_FRAMEFILTERTHREAD_H

#include <QThread>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include <QQueue>
#include <QTimer>

// 线程类用于处理帧
class FrameFilterThread : public QThread {
    Q_OBJECT
public:
    FrameFilterThread(QMutex *mutex, QQueue<cv::Mat> *inputQueue, QQueue<cv::Mat> *outputQueue, QObject *parent = nullptr);
    virtual ~FrameFilterThread();
    void run() override;
private:
    QMutex *mutex;
    QQueue<cv::Mat> *inputQueue;
    QQueue<cv::Mat> *outputQueue;
};


#endif //QT_OPENCV_CAMERA_APP_FRAMEFILTERTHREAD_H
