//
// Created by 卢泉鸿 on 2025/2/14.
//

#include "mainwindow.h"
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>







MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent) {
    openButton = new QPushButton("Open", this);
    openButton->setGeometry(10, 10, 80, 30);
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openCamera);

    srCheckBox = new QCheckBox("SR", this);
    srCheckBox->setGeometry(100, 10, 80, 30);

    resolutionComboBox = new QComboBox(this);
    resolutionComboBox->setGeometry(190, 10, 100, 30);
    resolutionComboBox->addItems({"640x360", "800x600"});

    cameraWidget = new CameraWidget(this);
    cameraWidget->setGeometry(10, 50, 640, 360);

    resize(650, 360);

    captureThread = new FrameCaptureThread(&frameMutex, &captureFrameQueue, &cap, this);
    filterThread = new FrameFilterThread(&frameMutex, &captureFrameQueue, &filteredFrameQueue, this);
    renderThread = new FrameRenderThread(&frameMutex, &filteredFrameQueue, cameraWidget, this);
}

MainWindow::~MainWindow() {
    delete openButton;
    delete srCheckBox;
    delete resolutionComboBox;
    delete cameraWidget;
    captureThread->quit();
    captureThread->wait();
    filterThread->quit();
    filterThread->wait();
    renderThread->quit();
    renderThread->wait();
    delete captureThread;
    delete filterThread;
    delete renderThread;
}

void MainWindow::openCamera() {
    if (!cap.isOpened()) {
        cap.open(0);
        if (cap.isOpened()) {
            captureThread->start();
            filterThread->start();
            renderThread->start();
            openButton->setText("Close");
        } else {
//            qDebug() << "无法打开摄像头";
        }
    } else {
        captureThread->quit();
        captureThread->wait();
        filterThread->quit();
        filterThread->wait();
        renderThread->quit();
        renderThread->wait();
        cap.release();
        frameMutex.lock();
        captureFrameQueue.clear();
        filteredFrameQueue.clear();
        frameMutex.unlock();
        cameraWidget->setFrame(cv::Mat());
        openButton->setText("Open");
    }
}
