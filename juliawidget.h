#ifndef JULIAWIDGET_H
#define JULIAWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QComboBox>
//#include <complex>

class JuliaWidget : public QWidget {
    Q_OBJECT

public:
    explicit JuliaWidget(QWidget* parent = nullptr);

public slots:
    // 通过快捷键移动
    void moveRight(){
        //resolutionInput->setText(QString::number(scrollArea->size().height() < scrollArea->size().width() ? scrollArea->size().height() : scrollArea->size().width()));
        realCenterInput->setText(QString::number(realCenterInput->text().toDouble() + rangeInput->text().toDouble()/5));
        onGenerateButtonClicked(false);
    }
    void moveLeft(){
        realCenterInput->setText(QString::number(realCenterInput->text().toDouble() - rangeInput->text().toDouble()/5));
        onGenerateButtonClicked(false);
    }
    void moveDown(){
        imagCenterInput->setText(QString::number(imagCenterInput->text().toDouble() + rangeInput->text().toDouble()/5));
        onGenerateButtonClicked(false);
    }
    void moveUp(){
        imagCenterInput->setText(QString::number(imagCenterInput->text().toDouble() - rangeInput->text().toDouble()/5));
        onGenerateButtonClicked(false);
    }
    void scaleUp(){
        rangeInput->setText(QString::number(rangeInput->text().toDouble()*0.8));
        onGenerateButtonClicked(false);
    }
    void scaleDown(){
        rangeInput->setText(QString::number(rangeInput->text().toDouble()*1.2));
        onGenerateButtonClicked(false);
    }

    void onGenerateButtonClicked(bool saveImage=true);
    //void onColorMapChanged(int index); // 下拉框的变化

private:
    double epsilon = 1e-13; //用于double比较

    int width = -1;
    int height = -1;
    int maxIterations = -1;
    int resolution = -1;

    //double real = -2; // c real
    //double imag = -2; // c imag
    //double order = -2;// z^order
    std::string func_str = "z^2+(-0.7+0.27015i)";

    std::vector<std::vector<int>> JuliaMatrix;


    // 图像颜色映射使用的HSV
    //int HSV1[3];
    //int HSV2[3];
    std::function<QRgb(float)> colorMapFunc;

    QScrollArea* scrollArea;  // 图片区域
    double scaleFactor = 1.0; // 缩放因子
    QPoint dragStartPosition; // 拖拽起始位置
    bool isDragging = false;  // 是否正在拖拽

    QLineEdit* funcInput;
    QLineEdit* orderInput;
    QLineEdit* realInput;
    QLineEdit* imagInput;
    QLineEdit* resolutionInput;
    QLineEdit* maxIterInput;

    QLineEdit* realCenterInput; // 画面中心的实部
    QLineEdit* imagCenterInput;
    QLineEdit* rangeInput; //查看的范围
    double realCenter = 0;
    double imagCenter = 0;
    double range = 3;

    QComboBox *colorMapComboBox;

    QLabel* displayLabel;
    QLabel* imageLabel;
    QImage originalImage; // 保存原始高分辨率图像
    QPixmap originalPixmap;

    QPushButton* generateButton;  //生成图像的按钮

    void setupUI();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    //bool eventFilter(QObject* obj, QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
};

#endif // JULIAWIDGET_H
