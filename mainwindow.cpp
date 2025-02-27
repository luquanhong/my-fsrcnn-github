//
// Created by 卢泉鸿 on 2025/2/14.
//

#include <QGridLayout>
#include <QTextEdit>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent) {

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    cameraWidget = new CameraWidget(this);
    mainLayout->addWidget(cameraWidget, 1);

    cameraComboBox = new QComboBox(this);
    updateCameraList();  // 初始化时更新摄像头列表
    connect(cameraComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onCameraSelected);

    openButton = new QPushButton("Open", this);
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openCamera);

    srCheckBox = new QCheckBox("SR", this);
    connect(srCheckBox, &QCheckBox::toggled, this, &MainWindow::onToggled);

    resolutionComboBox = new QComboBox(this);
    resolutionComboBox->addItems({"640x360", "1280x720", "1920x1080"});
    connect(resolutionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onResolutionChanged);

    filterThread = new FrameFilterThread(&frameMutex, &captureFrameQueue, &filteredFrameQueue, this);
    filterThread->enableSuperResolution(srCheckBox->isChecked());
    renderThread = new FrameRenderThread(&frameMutex, &filteredFrameQueue, cameraWidget, this);

    captureTimer.setInterval(30);
    connect(&captureTimer, &QTimer::timeout, this, &MainWindow::captureFrame);

    QHBoxLayout *controlsLayout = new QHBoxLayout();
    controlsLayout->addWidget(cameraComboBox);
    controlsLayout->addWidget(resolutionComboBox);
    controlsLayout->addWidget(openButton);
    controlsLayout->addWidget(srCheckBox);

    statusLabel = new QLabel("就绪 - 请选择摄像头", this);
    statusLabel->setMaximumHeight(30);
    controlsLayout->addWidget(statusLabel);

    mainLayout->addLayout(controlsLayout);
    setCentralWidget(centralWidget);
    setMinimumSize(1300, 800);
//    mainLayout->setStretch(0, 1);  // CameraWidget 占1份（全部空间）
//    mainLayout->setStretch(1, 0);  // 控制栏保持固定高度

    setWindowTitle("智能摄像头查看器 - V2.0");
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

    // 确保摄像头资源释放
    if (cap.isOpened()) {
        cap.release();
    }
}

void MainWindow::openCamera() {
    if (!cap.isOpened()) {
        const QVariant selectedCamera = cameraComboBox->currentData();

        // 处理虚拟摄像头
        if (selectedCamera == -1) {
            startVirtualCamera();
            return;
        }

        // 打开真实摄像头
        try {
            cap.open(selectedCamera.toInt());

            if (cap.isOpened()) {
                applyResolution();
                captureTimer.start();
                filterThread->start();
                renderThread->start();
                openButton->setText("关闭");
                statusLabel->setText("运行中 - 摄像头已连接");
            }
        } catch (const cv::Exception& e) {
            handleCameraError(e.what());
        }
    } else {
        stopCamera();
    }
}

void MainWindow::captureFrame() {
    cv::Mat frame;
    cap >> frame;
    if (!frame.empty()) {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
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

// 新增私有方法实现
void MainWindow::onCameraSelected(int index) {
    if (cap.isOpened()) {
        stopCamera();
    }
    statusLabel->setText(QString("已选择：%1").arg(cameraComboBox->itemText(index)));
}

void MainWindow::applyResolution() {
    if (cap.isOpened()) {
        const QString res = resolutionComboBox->currentText();
        const auto sizes = res.split('x');
        if (sizes.size() == 2) {
            const int targetWidth = sizes[0].toInt();
            const int targetHeight = sizes[1].toInt();

            // 设置请求的分辨率
            cap.set(cv::CAP_PROP_FRAME_WIDTH, targetWidth);
            cap.set(cv::CAP_PROP_FRAME_HEIGHT, targetHeight);

            // 获取实际设置的分辨率
            const int actualWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
            const int actualHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
            currentResolution = QSize(actualWidth, actualHeight);

            // 调整窗口大小（保留控件区域空间）
//            const int controlHeight = 80; // 根据实际控件高度调整
//            resize(actualWidth, actualHeight + controlHeight);

            // 更新状态显示
            statusLabel->setText(QString("运行中 - %1x%2").arg(actualWidth).arg(actualHeight));
        }
    }
}

void MainWindow::startVirtualCamera() {
    captureTimer.start();
    filterThread->start();
    renderThread->start();
    openButton->setText("关闭");
    statusLabel->setText("运行中 - 虚拟摄像头模式");
}

void MainWindow::stopCamera() {
    if (cap.isOpened()) {
        cap.release();
    }

    captureTimer.stop();

    if (filterThread->isRunning()) {
        filterThread->requestInterruption();
        filterThread->quit();
        filterThread->wait(500);
    }

    if (renderThread->isRunning()) {
        renderThread->requestInterruption();
        renderThread->quit();
        renderThread->wait(500);
    }

    frameMutex.lock();
    captureFrameQueue.clear();
    filteredFrameQueue.clear();
    frameMutex.unlock();

    cameraWidget->setFrame(cv::Mat());
    openButton->setText("打开");
    statusLabel->setText("就绪 - 摄像头已断开");
}

void MainWindow::handleCameraError(const QString& error) {
    statusLabel->setText(QString("错误：%1").arg(error));
    openButton->setText("打开");
    if (cap.isOpened()) {
        cap.release();
    }
}

void MainWindow::onResolutionChanged(int index) {
    if (cap.isOpened()) {
        // 如果摄像头已经打开，需要重启摄像头应用新分辨率
        stopCamera();
        openCamera();
    }
    // 更新状态显示
    statusLabel->setText(QString("已选择分辨率：%1").arg(resolutionComboBox->currentText()));
}