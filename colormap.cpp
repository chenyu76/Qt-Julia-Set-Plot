#include "colormap.h"
#include <QColor>
#include <algorithm>
#include <QImage>
#include <QPainter>

// 将x限制在[min_x, max_x]范围内
float ColorMap::clamp(float x, float min_x, float max_x) {
    return std::max(min_x, std::min(x, max_x));
}


QStringList ColorMap::funcNames = {
    "Jet",
    "Turbo",
    "Viridis",
    "Inferno",
    "Hot",
    "Cool",
    "Gray",
    "Gray Inverted",
    "Spring",
    "Summer",
    "Autumn",
    "Winter",
    "Copper",
    "Pink",
    "Ocean",
    "Terrain",
    "Rainbow",
    "HSV",
    "HSV_alt"
};

std::function<QRgb(float, float, float)> ColorMap::funcs[] = {
    // Jet colormap
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);

        float r, g, b;
        if (x < 0.25f) {
            r = 0.0f;
            g = 4.0f * x;
            b = 1.0f;
        } else if (x < 0.5f) {
            r = 0.0f;
            g = 1.0f;
            b = 1.0f - 4.0f * (x - 0.25f);
        } else if (x < 0.75f) {
            r = 4.0f * (x - 0.5f);
            g = 1.0f;
            b = 0.0f;
        } else {
            r = 1.0f;
            g = 1.0f - 4.0f * (x - 0.75f);
            b = 0.0f;
        }

        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Turbo colormap
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        static const float turboRed[] = {0.19f, 0.07f, 0.0f, 0.49f, 1.0f, 1.0f, 0.84f, 0.5f};
        static const float turboGreen[] = {0.0f, 0.15f, 0.65f, 0.95f, 1.0f, 0.44f, 0.0f, 0.0f};
        static const float turboBlue[] = {0.5f, 0.84f, 1.0f, 1.0f, 0.95f, 0.65f, 0.15f, 0.0f};
        float pos = x * 7.0f;
        int idx = static_cast<int>(pos);
        float frac = pos - idx;
        float r = turboRed[idx] + frac * (turboRed[idx + 1] - turboRed[idx]);
        float g = turboGreen[idx] + frac * (turboGreen[idx + 1] - turboGreen[idx]);
        float b = turboBlue[idx] + frac * (turboBlue[idx + 1] - turboBlue[idx]);
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Viridis colormap
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        static const float viridisRed[] = {0.267f, 0.282f, 0.253f, 0.163f, 0.128f, 0.267f};
        static const float viridisGreen[] = {0.004f, 0.141f, 0.365f, 0.659f, 0.808f, 0.992f};
        static const float viridisBlue[] = {0.329f, 0.458f, 0.659f, 0.788f, 0.878f, 0.984f};
        float pos = x * 5.0f;
        int idx = static_cast<int>(pos);
        float frac = pos - idx;
        float r = viridisRed[idx] + frac * (viridisRed[idx + 1] - viridisRed[idx]);
        float g = viridisGreen[idx] + frac * (viridisGreen[idx + 1] - viridisGreen[idx]);
        float b = viridisBlue[idx] + frac * (viridisBlue[idx + 1] - viridisBlue[idx]);
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Inferno colormap
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        static const float infernoRed[] = {0.001f, 0.302f, 0.601f, 0.903f, 1.000f, 0.988f};
        static const float infernoGreen[] = {0.000f, 0.137f, 0.287f, 0.428f, 0.729f, 0.998f};
        static const float infernoBlue[] = {0.013f, 0.295f, 0.573f, 0.758f, 0.964f, 0.998f};
        float pos = x * 5.0f;
        int idx = static_cast<int>(pos);
        float frac = pos - idx;
        float r = infernoRed[idx] + frac * (infernoRed[idx + 1] - infernoRed[idx]);
        float g = infernoGreen[idx] + frac * (infernoGreen[idx + 1] - infernoGreen[idx]);
        float b = infernoBlue[idx] + frac * (infernoBlue[idx + 1] - infernoBlue[idx]);
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Hot colormap
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float r = std::min(1.0f, 3.0f * x);
        float g = std::min(1.0f, 3.0f * x - 1.0f);
        float b = std::min(1.0f, 3.0f * x - 2.0f);
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Cool colormap
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float r = x;
        float g = 1.0f - x;
        float b = 1.0f;
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Gray colormap
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float gray = x;
        return qRgb(static_cast<int>(gray * 255), static_cast<int>(gray * 255), static_cast<int>(gray * 255));
    },

    // Gray Inverted colormap
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float gray = x;
        return qRgb(static_cast<int>(255 - gray * 255), static_cast<int>(255 - gray * 255), static_cast<int>(255 - gray * 255));
    },

    // Spring colormap (Magenta to Yellow)
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float r = 1.0f;
        float g = x;
        float b = 1.0f - x;
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Summer colormap (Green to Yellow)
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float r = x;
        float g = 0.5f + 0.5f * x;
        float b = 0.4f;
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Autumn colormap (Red to Yellow)
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float r = 1.0f;
        float g = x;
        float b = 0.0f;
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Winter colormap (Blue to Cyan)
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float r = 0.0f;
        float g = x;
        float b = 1.0f - 0.5f * x;
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Copper colormap (Black to Copper)
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float r = x;
        float g = 0.78125f * x;
        float b = 0.4975f * x;
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Pink colormap (Light Pink to White)
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float r = 1.0f;
        float g = 0.75f + 0.25f * x;
        float b = 0.75f + 0.25f * x;
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Ocean colormap (Blue to Green to Cyan)
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float r = 0.0f;
        float g = x;
        float b = 0.5f + 0.5f * x;
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Terrain colormap (Brown to Green to Light Yellow)
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float r = 0.5f + 0.5f * x;
        float g = 0.4f + 0.6f * x;
        float b = 0.3f * (1.0f - x);
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // Rainbow colormap (Red to Orange to Yellow to Green to Blue)
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        float r, g, b;
        if (x < 0.2f) {
            r = 1.0f;
            g = 5.0f * x;
            b = 0.0f;
        } else if (x < 0.4f) {
            r = 1.0f;
            g = 1.0f;
            b = 5.0f * (x - 0.2f);
        } else if (x < 0.6f) {
            r = 1.0f - 5.0f * (x - 0.4f);
            g = 1.0f;
            b = 0.0f;
        } else if (x < 0.8f) {
            r = 0.0f;
            g = 1.0f - 5.0f * (x - 0.6f);
            b = 1.0f;
        } else {
            r = 0.5f * (x - 0.8f);
            g = 0.0f;
            b = 1.0f;
        }
        return qRgb(static_cast<int>(r * 255), static_cast<int>(g * 255), static_cast<int>(b * 255));
    },

    // HSV colormap (Hue Spectrum)
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        QColor color;
        color.setHsvF(x, 1.0f, 1.0f);
        return color.rgb();
    },

    // HSV_alt
    [](float x, float min_x, float max_x) -> QRgb {
        x = ColorMap::clamp((x - min_x) / (max_x - min_x), 0.0f, 1.0f);
        QColor color;
        color.setHsvF(x, x, 0.9+x/10);
        return color.rgb();
    },
};


// 获取颜色映射函数
std::function<QRgb(float)> ColorMap::getColorMapFunction(int type, float min, float max) {
    return [=](float x)->QRgb{return funcs[type](x, min, max);};
}

void ColorMap::generateColorMapImage(const std::function<QRgb(float, float, float)> colorMaps[],
        int colorMapCount, const QString& outputPath, int width, int heightPerRow, float minValue, float maxValue){
    // Calculate image dimensions
    int totalHeight = heightPerRow * colorMapCount;
    QImage image(width, totalHeight, QImage::Format_RGB32);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Iterate through each colormap
    for (int i = 0; i < colorMapCount; ++i) {
        const auto& colorMap = colorMaps[i];
        int yStart = i * heightPerRow;

        // Draw the color gradient for the current colormap
        for (int x = 0; x < width; ++x) {
            float value = minValue + (maxValue - minValue) * (float(x) / (width - 1));
            QRgb color = colorMap(value, minValue, maxValue);
            painter.setPen(color);
            painter.drawLine(x, yStart, x, yStart + heightPerRow - 1);
        }
    }

    painter.end();

    // Save the image to the specified file
    image.save(outputPath);
}
