#include <QApplication>
#include "juliawidget.h"
#include "colormap.h"
#include <QFile>
#include <QShortcut>
#include <QMessageBox>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    if(!QFile::exists("colormaps.png"))
        ColorMap::generateColorMapImage();
    JuliaWidget widget;

    // 创建移动快捷键, 绑定快捷键到一个 lambda 函数
    QShortcut *shortcutH = new QShortcut(QKeySequence("H"), &widget);
    QObject::connect(shortcutH, &QShortcut::activated, &widget, &JuliaWidget::moveLeft);
    QShortcut *shortcutJ = new QShortcut(QKeySequence("J"), &widget);
    QObject::connect(shortcutJ, &QShortcut::activated, &widget, &JuliaWidget::moveDown);
    QShortcut *shortcutK = new QShortcut(QKeySequence("K"), &widget);
    QObject::connect(shortcutK, &QShortcut::activated, &widget, &JuliaWidget::moveUp);
    QShortcut *shortcutL = new QShortcut(QKeySequence("L"), &widget);
    QObject::connect(shortcutL, &QShortcut::activated, &widget, &JuliaWidget::moveRight);
    QShortcut *shortcutU = new QShortcut(QKeySequence("="), &widget);
    QObject::connect(shortcutU, &QShortcut::activated, &widget, &JuliaWidget::scaleUp);
    QShortcut *shortcutD = new QShortcut(QKeySequence("-"), &widget);
    QObject::connect(shortcutD, &QShortcut::activated, &widget, &JuliaWidget::scaleDown);
    QShortcut *shortcutSave = new QShortcut(QKeySequence("Ctrl+S"), &widget);
    QObject::connect(shortcutSave, &QShortcut::activated, &widget, [&](){widget.onGenerateButtonClicked(true);});
    QShortcut *shortcutGen = new QShortcut(QKeySequence("Ctrl+D"), &widget);
    QObject::connect(shortcutGen, &QShortcut::activated, &widget, [&](){widget.onGenerateButtonClicked(false);});

    widget.show();
    return app.exec();
}
