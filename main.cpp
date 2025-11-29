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

    auto bindKeys = [&](auto func, auto... keys) {
        // C++17 折叠表达式 (Fold Expression)
        (..., [&](const auto& key) {
            // 这里显式调用 QKeySequence(key)，此时 key 还是 char*
            // 在函数体内进行 char* -> QString -> QKeySequence 是合法的
            auto *shortcut = new QShortcut(QKeySequence(key), &widget);
            shortcut->setAutoRepeat(true);
            QObject::connect(shortcut, &QShortcut::activated, &widget, func);
        }(keys));
    };

    // 调用时更爽快（注意 func 在前）：
    bindKeys(&JuliaWidget::moveLeft,  "H", "A", Qt::Key_Left);
    bindKeys(&JuliaWidget::moveDown,  "J", "S", Qt::Key_Down);
    bindKeys(&JuliaWidget::moveUp,    "K", "W", Qt::Key_Up);
    bindKeys(&JuliaWidget::moveRight, "L", "D", Qt::Key_Right);

    bindKeys(&JuliaWidget::scaleUp,   "=", Qt::Key_Plus);
    bindKeys(&JuliaWidget::scaleDown, "-");

    bindKeys([&](){ widget.onGenerateButtonClicked(true); },  "Ctrl+S");
    bindKeys([&](){ widget.onGenerateButtonClicked(false); }, "Ctrl+D");
    widget.show();
    return app.exec();
}
