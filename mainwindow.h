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

#include "FrameCaptureThread.h"
#include "FrameFilterThread.h"
#include "FrameRenderThread.h"
#include "CameraWidget.h"


QT_BEGIN_NAMESPACE
// namespace Ui { class MainWindow; } // 不再使用UI设计文件，注释掉这部分
QT_END_NAMESPACE








class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

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
