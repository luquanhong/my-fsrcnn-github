//
// Created by 卢泉鸿 on 2025/2/14.
//

#include "mainwindow.h"
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>

FrameCaptureThread::FrameCaptureThread(QMutex *mutex, QQueue<cv::Mat> *frameQueue, cv::VideoCapture *cap, QObject *parent)
        : QThread(parent), mutex(mutex), frameQueue(frameQueue), cap(cap) {
    captureTimer.setInterval(30);
    connect(&captureTimer, &QTimer::timeout, this, &FrameCaptureThread::captureFrame);
}

FrameCaptureThread::~FrameCaptureThread() {
    captureTimer.stop();
}

void FrameCaptureThread::captureFrame() {
    cv::Mat frame;
    (*cap) >> frame;
    if (!frame.empty()) {
        mutex->lock();
        frameQueue->enqueue(frame);
        mutex->unlock();
    }
}

void FrameCaptureThread::run() {
    captureTimer.start();
    exec();
}

FrameFilterThread::FrameFilterThread(QMutex *mutex, QQueue<cv::Mat> *inputQueue, QQueue<cv::Mat> *outputQueue, QObject *parent)
        : QThread(parent), mutex(mutex), inputQueue(inputQueue), outputQueue(outputQueue) {}

FrameFilterThread::~FrameFilterThread() {}

void FrameFilterThread::run() {
    while (true) {
        cv::Mat frame;
        mutex->lock();
        if (!inputQueue->isEmpty()) {
            frame = inputQueue->dequeue();
        }
        mutex->unlock();
        if (frame.empty()) {
            continue;
        }
        cv::Mat tempFrame;
        cv::cvtColor(frame, tempFrame, cv::COLOR_BGR2GRAY);
        // 这里可以添加更复杂的帧处理操作
        mutex->lock();
        outputQueue->enqueue(tempFrame);
        mutex->unlock();
    }
}

FrameRenderThread::FrameRenderThread(QMutex *mutex, QQueue<cv::Mat> *frameQueue, QWidget *renderWidget, QObject *parent)
        : QThread(parent), mutex(mutex), frameQueue(frameQueue), renderWidget(renderWidget) {}

FrameRenderThread::~FrameRenderThread() {}

void FrameRenderThread::run() {
    while (true) {
        cv::Mat frame;
        mutex->lock();
        if (!frameQueue->isEmpty()) {
            frame = frameQueue->dequeue();
        }
        mutex->unlock();
        if (frame.empty()) {
            continue;
        }
        CameraWidget *widget = dynamic_cast<CameraWidget*>(renderWidget);
        if (widget) {
            widget->setFrame(frame);
        }
    }
}

CameraWidget::CameraWidget(QWidget *parent) : QWidget(parent) {}

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
    QPainter painter(this);
    painter.drawImage(QPoint(0, 0), qImage);
}

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
