#include "juliawidget.h"
#include "juliadraw.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QPixmap>
#include <sstream>
#include <QResizeEvent>
#include <QScrollBar>
#include <QGroupBox>
#include <colormap.h>
#include <QShortcut>
#include <QFile>

JuliaWidget::JuliaWidget(QWidget* parent)
    : QWidget(parent), width(800), height(600), maxIterations(1000) {
    //setFixedSize(width, height);
    resize(400, 600);
    //scrollArea->setFocusPolicy(Qt::StrongFocus);
    setupUI();
}

void JuliaWidget::setupUI() {
    // 主布局
    QVBoxLayout* mainLayout = new QVBoxLayout;

    // 输入 C 组
    QGroupBox* cInputGroup = new QGroupBox("输入方程参数");

    QHBoxLayout* complexInputLayout = new QHBoxLayout;


    QLabel* cLabel = new QLabel("f(z) = z^");
    complexInputLayout->addWidget(cLabel);

    orderInput = new QLineEdit("2");
    orderInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    complexInputLayout->addWidget(orderInput);

    QLabel* plusLabel1 = new QLabel(" + (");
    complexInputLayout->addWidget(plusLabel1);

    realInput = new QLineEdit("-0.7");
    realInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    complexInputLayout->addWidget(realInput);

    QLabel* plusLabel = new QLabel(" + ");
    complexInputLayout->addWidget(plusLabel);

    imagInput = new QLineEdit("0.27015");
    imagInput->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    complexInputLayout->addWidget(imagInput);

    QLabel* iLabel = new QLabel("i) ");
    complexInputLayout->addWidget(iLabel);

    cInputGroup->setLayout(complexInputLayout);
    cInputGroup->setMaximumWidth(500);

    mainLayout->addWidget(cInputGroup);

    // 图像设置输入
    QGroupBox* figCfgInputGroup = new QGroupBox("图像设置");
    QVBoxLayout* figCfgInputGroupLayout = new QVBoxLayout;

    /*
    QHBoxLayout* HSV1InputLayout = new QHBoxLayout;

    QLabel* HSV1Label = new QLabel("最小HSV设置（0~255）");
    HSV1InputLayout->addWidget(HSV1Label);
    HSVInput11 = new QLineEdit("0");
    HSVInput11->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    HSV1InputLayout->addWidget(HSVInput11);

    HSVInput12 = new QLineEdit("0");
    HSVInput12->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    HSV1InputLayout->addWidget(HSVInput12);

    HSVInput13 = new QLineEdit("255");
    HSVInput13->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    HSV1InputLayout->addWidget(HSVInput13);

    figCfgInputGroupLayout->addLayout(HSV1InputLayout);

    QHBoxLayout* HSV2InputLayout = new QHBoxLayout;
    QLabel* HSV2Label = new QLabel("最大HSV设置（0~255）");
    HSV2InputLayout->addWidget(HSV2Label);
    HSVInput21 = new QLineEdit("0");
    HSVInput21->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    HSV2InputLayout->addWidget(HSVInput21);

    HSVInput22 = new QLineEdit("0");
    HSVInput22->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    HSV2InputLayout->addWidget(HSVInput22);

    HSVInput23 = new QLineEdit("0");
    HSVInput23->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    HSV2InputLayout->addWidget(HSVInput23);
    figCfgInputGroupLayout->addLayout(HSV2InputLayout);
*/

    // 创建下拉框并添加选项
    colorMapComboBox = new QComboBox(this);
    for(int i = 0; i < ColorMap::funcNames.length(); i++)
        colorMapComboBox->addItem(ColorMap::funcNames[i], i);

    // init color map set

    QHBoxLayout* colorSelectLayout = new QHBoxLayout;
    QLabel* colorTipLabel = new QLabel("颜色映射函数");
    colorSelectLayout->addWidget(colorTipLabel);
    colorSelectLayout->addWidget(colorMapComboBox);
    // 将下拉框插入到布局中
    figCfgInputGroupLayout->addLayout(colorSelectLayout);

    // 连接下拉框的信号到槽函数
    //connect(colorMapComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onColorMapChanged(int)));



    // 分辨率设置
    QHBoxLayout* resolutionInputLayout = new QHBoxLayout;
    resolutionInputLayout->addWidget(new QLabel("图像分辨率(px):"));
    resolutionInput = new QLineEdit("1080");
    resolutionInputLayout->addWidget(resolutionInput);
    figCfgInputGroupLayout->addLayout(resolutionInputLayout);

    QHBoxLayout* maxIterLayout = new QHBoxLayout;
    maxIterLayout->addWidget(new QLabel("最大迭代次数:"));
    maxIterInput = new QLineEdit("200");
    maxIterLayout->addWidget(maxIterInput);
    figCfgInputGroupLayout->addLayout(maxIterLayout);


    QHBoxLayout* rangeLayout = new QHBoxLayout;
    rangeLayout->addWidget(new QLabel("图像实部中心："));
    realCenterInput = new QLineEdit("0");
    rangeLayout->addWidget(realCenterInput);
    rangeLayout->addWidget(new QLabel("虚部中心："));
    imagCenterInput = new QLineEdit("0");
    rangeLayout->addWidget(imagCenterInput);

    rangeLayout->addWidget(new QLabel("绘制范围："));
    rangeInput = new QLineEdit("3");
    rangeLayout->addWidget(rangeInput);

    figCfgInputGroupLayout->addLayout(rangeLayout);

    figCfgInputGroup->setLayout(figCfgInputGroupLayout);
    figCfgInputGroup->setMaximumWidth(500);

    mainLayout->addWidget(figCfgInputGroup);



    // 生成按钮
    generateButton = new QPushButton("生成 Julia Set 图像");
    // 默认提交按钮，按下回车就会触发
    generateButton->setDefault(true);
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Return), this); // 监听回车键
    connect(shortcut, &QShortcut::activated, generateButton, &QPushButton::click);

    generateButton->setMaximumWidth(500);
    mainLayout->addWidget(generateButton);

    // 显示图像名称
    displayLabel = new QLabel("点击上面按钮生成图像，图像需要一段时间生成，程序可能会无响应，请耐心等待。\n提示：光标不在输入框内时，你可以通过H/J/K/L移动图像范围，-/= 缩放图像；此外，Ctrl+S保存图像，Ctrl+D生成图像（但不保存）");//下图为各个颜色映射对应色卡");
    displayLabel->setWordWrap(true);

    mainLayout->addWidget(displayLabel);

    // 显示图像，默认显示colormap
    //if(QFile::exists("colormaps.png")){
    //    originalPixmap = QPixmap("colormaps.png");
    //    imageLabel->setPixmap(originalPixmap.scaled(scrollArea->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //} else {
    imageLabel = new QLabel("暂无图像");
    //}
    imageLabel->setAlignment(Qt::AlignCenter);
    scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(imageLabel);
    mainLayout->addWidget(scrollArea);

    connect(generateButton, &QPushButton::clicked, this, [&](){this->onGenerateButtonClicked(true);});

    setLayout(mainLayout);
}

void JuliaWidget::onGenerateButtonClicked(bool saveImage) {
    if( // 参数改变时才重新计算矩阵
        abs(real - realInput->text().toDouble()) > epsilon ||
        abs(imag - imagInput->text().toDouble()) > epsilon ||
        order != orderInput->text().toInt()  ||
        resolution != resolutionInput->text().toInt() ||
        maxIterations != maxIterInput->text().toInt() ||
        abs(realCenter - realCenterInput->text().toDouble()) > epsilon ||
        abs(imagCenter - imagCenterInput->text().toDouble()) > epsilon ||
        abs(range - rangeInput->text().toDouble()) > epsilon
    ){
        // 获取C的数据
        real = realInput->text().toDouble();
        imag = imagInput->text().toDouble();
        order = orderInput->text().toInt();
        resolution = resolutionInput->text().toInt();
        maxIterations = maxIterInput->text().toInt();

        //范围
        realCenter = realCenterInput->text().toDouble();
        imagCenter = imagCenterInput->text().toDouble();
        range = rangeInput->text().toDouble();

        width = resolution;
        height = resolution;
        std::complex<double> c(real, imag);

        // 计算出julia矩阵
        JuliaMatrix = generateJuliaMatrix(
            realCenter - range/2, realCenter + range/2, imagCenter - range/2, imagCenter + range/2,
            width, height, order, c, maxIterations
            );
        //auto matrix = generateMandelbrotMatrix(width, height, n, c, maxIterations);
    }

    int minIter = maxIterations; // 最小的 迭代次数
    for(auto& i:JuliaMatrix)
        for(auto& j:i)
            if(j < minIter)
                minIter = j;

    // 获取下拉框的数据
    colorMapFunc = ColorMap::getColorMapFunction(colorMapComboBox->currentIndex(), minIter, maxIterations);

    //originalImage = saveJuliaImage(matrix, filename, createHSVGradientFunction(HSV1[0], HSV1[1], HSV1[2], HSV2[0], HSV2[1], HSV2[2], maxIterations));
    originalImage = getJuliaImage(JuliaMatrix, colorMapFunc);

    if(saveImage){
        // 生成文件名
        std::ostringstream oss;
        oss << "julia_z" << order
                << (real < 0 ? "" : "+")  << real
                << (imag < 0 ? "" : "+") << imag << "i_" << maxIterations << "_"
            << resolution << "p_" << colorMapComboBox->currentText().toStdString() << "_z("
            << realCenter << "," << imagCenter <<")_"<< range
            << ".png";
        std::string filename = oss.str();
        originalImage.save(QString::fromStdString(filename));
        displayLabel->setText("图像已保存： " + QString::fromStdString(filename));
    }
    else{
        displayLabel->setText("完成计算");
    }

    // 加载并显示图像
    scaleFactor = 1.0;
    // 加载完整分辨率的图像
    originalPixmap = QPixmap::fromImage(originalImage);
    imageLabel->setPixmap(originalPixmap.scaled(scrollArea->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 避免滚轮事件触发滚动条
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->installEventFilter(this);

    // 还原生成按钮
    generateButton->setText("重新生成 Julia Set 图像并保存");
}

void JuliaWidget::resizeEvent(QResizeEvent* event) {
    if (!imageLabel->pixmap()) return;

    QPixmap scaledPixmap = imageLabel->pixmap().scaled(
        scrollArea->size() * scaleFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imageLabel->setPixmap(scaledPixmap);

    QWidget::resizeEvent(event);

    imageLabel->setPixmap(originalPixmap.scaled(scrollArea->size() * scaleFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void JuliaWidget::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) {
        scaleFactor *= 1.1;  // 放大
    } else {
        scaleFactor *= 0.9;  // 缩小
    }

    imageLabel->setPixmap(originalPixmap.scaled(scrollArea->size() * scaleFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void JuliaWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        dragStartPosition = event->pos();
        isDragging = true;
    }
}

void JuliaWidget::mouseMoveEvent(QMouseEvent* event) {
    if (isDragging) {
        int dx = event->pos().x() - dragStartPosition.x();
        int dy = event->pos().y() - dragStartPosition.y();
        scrollArea->horizontalScrollBar()->setValue(scrollArea->horizontalScrollBar()->value() - dx);
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->value() - dy);
        dragStartPosition = event->pos();
    }
}

void JuliaWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
}
