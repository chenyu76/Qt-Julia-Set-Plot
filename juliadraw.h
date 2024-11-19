#ifndef JULIADRAW_H
#define JULIADRAW_H

#include <QImage>
#include <vector>
#include <complex>
//#include <string>
#include <functional>
#include <QRgb>
#include <QColor>

// 颜色映射函数
QRgb getColor(int iteration, int maxIterations);

//一个生成颜色渐变的函数，
//该函数输入最大和最小的 HSV 分量以及一个最大值 max_x，
//返回一个从最小 HSV 过渡到最大 HSV 的 QRgb 颜色函数。
//这个函数返回的结果可以用于在一个区间 [0, max_x] 内，线性插值 HSV 值，并返回相应的 QRgb 颜色。
std::function<QRgb(int)> createHSVGradientFunction(int minH, int minS, int minV, int maxH, int maxS, int maxV, int max_x);

// 生成 Julia 集矩阵
std::vector<std::vector<int>> generateJuliaMatrix(double, double, double, double,int width, int height, int n, const std::complex<double>& c, int maxIterations);
// 生成 Mandelbrot set
std::vector<std::vector<int>> generateMandelbrotMatrix(int width, int height, int n, const std::complex<double>& c, int maxIterations);

// 将 Julia 集矩阵，转换为有颜色的QImage
QImage getJuliaImage(const std::vector<std::vector<int>>& matrix, std::function<QRgb(float)> getColor);

#endif // JULIADRAW_H
