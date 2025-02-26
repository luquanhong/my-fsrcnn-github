//
// Created by 卢泉鸿 on 2025/2/14.
//

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent) {

    cameraComboBox = new QComboBox(this);
    cameraComboBox->setGeometry(10, 10, 80, 30);
    updateCameraList();  // 初始化时更新摄像头列表
    connect(cameraComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateCameraList);

    openButton = new QPushButton("Open", this);
    openButton->setGeometry(100, 10, 80, 30);
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openCamera);

    srCheckBox = new QCheckBox("SR", this);
    srCheckBox->setGeometry(190, 10, 80, 30);
    // 连接 toggled 信号到槽函数
    connect(srCheckBox, &QCheckBox::toggled, this, &MainWindow::onToggled);

    resolutionComboBox = new QComboBox(this);
    resolutionComboBox->setGeometry(280, 10, 100, 30);
    resolutionComboBox->addItems({"640x360", "1280x720", "1920x1080"});

    cameraWidget = new CameraWidget(this);
    cameraWidget->setGeometry(10, 50, 640, 360);

    resize(650, 360);

    filterThread = new FrameFilterThread(&frameMutex, &captureFrameQueue, &filteredFrameQueue, this);
    filterThread->enableSuperResolution(srCheckBox->isChecked());

    renderThread = new FrameRenderThread(&frameMutex, &filteredFrameQueue, cameraWidget, this);


    captureTimer.setInterval(30);
    connect(&captureTimer, &QTimer::timeout, this, &MainWindow::captureFrame);
}

MainWindow::~MainWindow() {
    delete openButton;
    delete srCheckBox;
    delete resolutionComboBox;
    delete cameraWidget;

    filterThread->quit();
    filterThread->wait();
    renderThread->quit();
    renderThread->wait();
    delete filterThread;
    delete renderThread;
}

void MainWindow::openCamera() {
    if (!cap.isOpened()) {
        cap.open(0);
        if (cap.isOpened()) {
            captureTimer.start();
            filterThread->start();
            renderThread->start();

            openButton->setText("Close");
        } else {
//            qDebug() << "无法打开摄像头";
        }
    } else {
        renderThread->stop();
        filterThread->stop();
        captureTimer.stop();

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

void MainWindow::captureFrame() {
    cv::Mat frame;
    cap >> frame;
    if (!frame.empty()) {
        frameMutex.lock();
        captureFrameQueue.enqueue(frame);
        frameMutex.unlock();
    }
}

// 新增函数，用于更新摄像头列表
void MainWindow::updateCameraList() {
    cameraComboBox->clear();
    int index = 0;
    while (true) {
        cv::VideoCapture tempCap(index);
        if (!tempCap.isOpened()) {
            break;
        }
        cameraComboBox->addItem(QString("Camera %1").arg(index));
        tempCap.release();
        index++;
    }
}