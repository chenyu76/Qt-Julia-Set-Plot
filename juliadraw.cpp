#include "juliadraw.h"
#include <QColor>
#include <thread>
#include <functional>
#include <math.h>
#include <vector>
#include <complex>



// 计算 Mandelbrot 集并返回一个二维矩阵，表示迭代了多少次
std::vector<std::vector<int>> generateMandelbrotMatrix(int width, int height, const int n, const std::complex<double>& constant, int maxIterations) {
    std::vector<std::vector<int>> matrix(height, std::vector<int>(width));
    double scaleX = 3.0 / width;
    double scaleY = 3.0 / height;

    auto computeRow = [&](int startY, int step) {
        for (int y = startY; y < height; y += step) {
            for (int x = 0; x < width; ++x) {
                std::complex<double> c((x - width / 2) * scaleX, (y - height / 2) * scaleY);
                std::complex<double> z(0, 0);
                int iterations = 0;
                while (std::abs(z) < 2.0 && iterations < maxIterations) {
                    z = pow(z, n) + c;
                    ++iterations;
                }
                matrix[y][x] = iterations;
            }
        }
    };

    // 使用多个线程来并行计算
    const int threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(computeRow, i, threadCount);
    }
    for (auto& thread : threads) {
        thread.join();
    }

    return matrix;
}


// 将 Julia 集矩阵返回为为 QImage 图片
QImage getJuliaImage(const std::vector<std::vector<int>>& matrix, std::function<QRgb(float)> getColor) {
    int width = matrix[0].size();
    int height = matrix.size();
    QImage image(width, height, QImage::Format_RGB32);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int iteration = matrix[y][x];
            image.setPixel(x, y, getColor(iteration));
        }
    }
    return image;
}

// 定义复数类型
//using Complex = std::complex<double>;
