//
// Created by 卢泉鸿 on 2025/2/14.
//

#include "FrameFilterThread.h"

FrameFilterThread::FrameFilterThread(QMutex *mutex, QQueue<cv::Mat> *inputQueue, QQueue<cv::Mat> *outputQueue, QObject *parent)
        : QThread(parent), mutex(mutex), inputQueue(inputQueue), outputQueue(outputQueue), running(true)  {}

FrameFilterThread::~FrameFilterThread() {}

void FrameFilterThread::run() {
    while (running) {
        cv::Mat frame;
        mutex->lock();
        if (!inputQueue->isEmpty()) {
            frame = inputQueue->dequeue();
        }
        mutex->unlock();
        if (frame.empty()) {
            continue;
        }

        if (isSurperResolution) {
            cv::Mat tempFrame;
            cv::cvtColor(frame, tempFrame, cv::COLOR_BGR2GRAY);
            // 这里可以添加更复杂的帧处理操作

            mutex->lock();
            outputQueue->enqueue(tempFrame);
            mutex->unlock();
        } else {
            mutex->lock();
            outputQueue->enqueue(frame);
            mutex->unlock();
        }
    }
}

void FrameFilterThread::stop() {
    running = false;
}

void FrameFilterThread::start() {
    QThread::start();
    running = true;
}
