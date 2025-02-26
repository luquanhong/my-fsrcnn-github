//
// Created by 卢泉鸿 on 2025/2/14.
//

#include "FrameRenderThread.h"
#include "CameraWidget.h"

FrameRenderThread::FrameRenderThread(QMutex *mutex, QQueue<cv::Mat> *frameQueue, QWidget *renderWidget, QObject *parent)
        : QThread(parent), mutex(mutex), frameQueue(frameQueue), renderWidget(renderWidget), running(true)  {}

FrameRenderThread::~FrameRenderThread() {}

void FrameRenderThread::run() {
    while (running) {
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
    running = false;
}

void FrameRenderThread::start() {
    QThread::start();
    running = true;
}
