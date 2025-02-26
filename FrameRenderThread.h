//
// Created by 卢泉鸿 on 2025/2/14.
//

#ifndef QT_OPENCV_CAMERA_APP_FRAMERENDERTHREAD_H
#define QT_OPENCV_CAMERA_APP_FRAMERENDERTHREAD_H

#include <QThread>
#include <QMutex>
#include <opencv2/opencv.hpp>
#include <QQueue>
#include <QTimer>

// 线程类用于渲染帧
class FrameRenderThread : public QThread {
    Q_OBJECT
public:
    FrameRenderThread(QMutex *mutex, QQueue<cv::Mat> *frameQueue, QWidget *renderWidget, QObject *parent = nullptr);
    virtual ~FrameRenderThread();
    void run() override;

    virtual void start();
    void stop();
private:
    QMutex *mutex;
    QQueue<cv::Mat> *frameQueue;
    QWidget *renderWidget;

    bool isRunning; // 新增的运行标志
};


#endif //QT_OPENCV_CAMERA_APP_FRAMERENDERTHREAD_H
