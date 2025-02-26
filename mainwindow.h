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
#include <QLabel>

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
    void captureFrame();
    void updateCameraList();  // 新增槽函数，用于更新摄像头列表
    void stopCamera();
void startVirtualCamera();
    void onCameraSelected(int index);
    void handleCameraError(const QString& error);
    void applyResolution();

    void onToggled(bool checked) {
        filterThread->enableSuperResolution(checked);
    }


private:
    // Ui::MainWindow *ui; // 不再使用UI设计文件，注释掉这部分
    cv::VideoCapture cap;
    QSize currentResolution; // 添加当前分辨率跟踪

    QMutex frameMutex;
    QQueue<cv::Mat> captureFrameQueue;
    QQueue<cv::Mat> filteredFrameQueue;
    FrameFilterThread *filterThread;
    FrameRenderThread *renderThread;
    CameraWidget *cameraWidget;
    QPushButton *openButton;
    QCheckBox *srCheckBox;
    QComboBox *resolutionComboBox;
    QComboBox *cameraComboBox;  // 新增摄像头选择下拉框

    QLabel *statusLabel;
    QTimer captureTimer;

    int mSuperResolutionEnable{0};
};


#endif //MY_FSRCNN_GITHUB_MAINWINDOW_H
