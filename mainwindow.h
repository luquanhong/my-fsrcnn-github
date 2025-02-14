//
// Created by 卢泉鸿 on 2025/2/14.
//

#ifndef MY_FSRCNN_GITHUB_MAINWINDOW_H
#define MY_FSRCNN_GITHUB_MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QPixmap>
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>

QT_BEGIN_NAMESPACE
// namespace Ui { class MainWindow; } // 不再使用UI设计文件，注释掉这部分
QT_END_NAMESPACE


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

// 线程类用于渲染帧
class FrameRenderThread : public QThread {
    Q_OBJECT
public:
    FrameRenderThread(QMutex *mutex, QQueue<cv::Mat> *frameQueue, QWidget *renderWidget, QObject *parent = nullptr);
    virtual ~FrameRenderThread();
    void run() override;
private:
    QMutex *mutex;
    QQueue<cv::Mat> *frameQueue;
    QWidget *renderWidget;
};

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

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
            void openCamera();

private:
    // Ui::MainWindow *ui; // 不再使用UI设计文件，注释掉这部分
    cv::VideoCapture cap;
    QMutex frameMutex;
    QQueue<cv::Mat> captureFrameQueue;
    QQueue<cv::Mat> filteredFrameQueue;
    FrameCaptureThread *captureThread;
    FrameFilterThread *filterThread;
    FrameRenderThread *renderThread;
    CameraWidget *cameraWidget;
    QPushButton *openButton;
    QCheckBox *srCheckBox;
    QComboBox *resolutionComboBox;
};


#endif //MY_FSRCNN_GITHUB_MAINWINDOW_H
