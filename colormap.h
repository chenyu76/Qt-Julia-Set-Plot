#ifndef COLORMAP_H
#define COLORMAP_H

#include <QtGlobal>
#include <functional>
#include <QRgb>
#include <QString>

class ColorMap {
public:
    // 获取颜色映射函数
    static std::function<QRgb(float)> getColorMapFunction(int type, float, float);

    // 颜色函数对应的名称
    static QStringList funcNames;

    // 测试函数，会导出一张所有的颜色映射
    static void generateColorMapImage(const std::function<QRgb(float, float, float)> colorMaps[] = funcs,
                           int colorMapCount = 19,
                           const QString& outputPath = "colormaps.png",
                           int width = 1000,
                           int heightPerRow = 50,
                           float minValue = 0.0f,
                           float maxValue = 1.0f);
private:
    // 各种颜色映射函数的声明
    static std::function<QRgb(float, float, float)> funcs[];
    // 辅助函数：将值限制在[min_x, max_x]范围内
    static float clamp(float x, float min_x, float max_x);

};

#endif // COLORMAP_H
