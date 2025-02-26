//
// Created by 卢泉鸿 on 2025/2/14.
//

#include "FrameRenderThread.h"
#include "CameraWidget.h"

FrameRenderThread::FrameRenderThread(QMutex *mutex, QQueue<cv::Mat> *frameQueue, QWidget *renderWidget, QObject *parent)
        : QThread(parent), mutex(mutex), frameQueue(frameQueue), renderWidget(renderWidget), isRunning(true)  {}

FrameRenderThread::~FrameRenderThread() {}

void FrameRenderThread::run() {
    while (isRunning) {
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

void FrameRenderThread::stop() {
    isRunning = false;
}

void FrameRenderThread::start() {
    QThread::start();
    isRunning = true;
}
