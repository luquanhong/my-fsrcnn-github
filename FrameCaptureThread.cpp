//
// Created by 卢泉鸿 on 2025/2/14.
//

#include "FrameCaptureThread.h"

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