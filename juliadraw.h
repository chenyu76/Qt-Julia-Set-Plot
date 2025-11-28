#ifndef JULIADRAW_H
#define JULIADRAW_H

#include <QImage>
#include <vector>
#include <complex>
//#include <string>
#include <functional>
#include <QRgb>
#include <QColor>
#include <string>
#include <regex>
#include <algorithm>
//#include <iostream>
#include <thread>

// 颜色映射函数
QRgb getColor(int iteration, int maxIterations);

//一个生成颜色渐变的函数，
//该函数输入最大和最小的 HSV 分量以及一个最大值 max_x，
//返回一个从最小 HSV 过渡到最大 HSV 的 QRgb 颜色函数。
//这个函数返回的结果可以用于在一个区间 [0, max_x] 内，线性插值 HSV 值，并返回相应的 QRgb 颜色。
std::function<QRgb(int)> createHSVGradientFunction(int minH, int minS, int minV, int maxH, int maxS, int maxV, int max_x);

// 计算 Julia 集并返回一个二维矩阵，表示迭代了多少次
// ==========================================
// 2. generateJuliaMatrix (模板函数必须在头文件中实现)
// ==========================================
template <typename Func>
std::vector<std::vector<int>> generateJuliaMatrix(
    double realRangeMin, double realRangeMax, double imagRangeMin, double imagRangeMax,
    int width, int height,
    const Func& func,
    int maxIterations,
    double escapeRadius = 2.0
    ) {
    std::vector<std::vector<int>> matrix(height, std::vector<int>(width));
    double scaleX = (realRangeMax - realRangeMin) / width;
    double scaleY = (imagRangeMax - imagRangeMin) / height;
    double escapeRadiusSq = escapeRadius * escapeRadius;

    auto computeRow = [&](int startY, int step) {
        for (int y = startY; y < height; y += step) {
            for (int x = 0; x < width; ++x) {
                std::complex<double> z(x * scaleX + realRangeMin,
                                       y * scaleY + imagRangeMin);
                int iterations = 0;
                while (std::norm(z) < escapeRadiusSq && iterations < maxIterations) {
                    z = func(z);
                    ++iterations;
                }
                matrix[y][x] = iterations;
            }
        }
    };

    const int threadCount = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back(computeRow, i, threadCount);
    }
    for (auto& thread : threads) {
        if(thread.joinable()) thread.join();
    }

    return matrix;
}


// 生成 Mandelbrot set
std::vector<std::vector<int>> generateMandelbrotMatrix(int width, int height, int n, const std::complex<double>& c, int maxIterations);

// 将 Julia 集矩阵，转换为有颜色的QImage
QImage getJuliaImage(const std::vector<std::vector<int>>& matrix, std::function<QRgb(float)> getColor);

using Complex = std::complex<double>;
// 解析单个复数
inline Complex parseComplexCoeff(std::string s) {
    // 1. 去除所有空格
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());

    if (s.empty()) return {1, 0};

    // 2. 处理最前面的符号 (处理如 "-(1+i)" 或 "+(-0.7...)" 的情况)
    double signMultiplier = 1.0;
    if (s.front() == '-') {
        signMultiplier = -1.0;
        s.erase(0, 1);
    } else if (s.front() == '+') {
        s.erase(0, 1);
    }

    // 再次判空 (处理只有 "-" 或 "+" 的情况)
    if (s.empty()) return {1.0 * signMultiplier, 0};

    // 3. 去除括号
    if (s.front() == '(' && s.back() == ')') {
        s = s.substr(1, s.length() - 2);
    }

    // 4. 如果没有 'i'，说明是纯实数
    if (s.find('i') == std::string::npos) {
        try {
            return {std::stod(s) * signMultiplier, 0};
        } catch (...) {
            return {0, 0}; // 容错
        }
    }

    // 5. 去除末尾的 'i'
    s.pop_back();

    // 6. 寻找实部和虚部的分割点
    // 我们需要从后往前找最后一个 '+' 或 '-'
    // 注意：要小心科学计数法 (例如 1.2e-5)，不能把 e 后面的 - 当作分割符
    size_t splitPos = std::string::npos;
    for (int i = static_cast<int>(s.length()) - 1; i >= 0; --i) {
        char c = s[i];
        if (c == '+' || c == '-') {
            // 检查是否是科学计数法 (前一个字符是 e 或 E)
            bool isScientific = (i > 0 && (s[i-1] == 'e' || s[i-1] == 'E'));

            // 如果不是科学计数法，且不是第一个字符（处理 "-5i" 这种只有虚部且带负号的情况）
            // 这里的逻辑是：如果是 a+bi 或 a-bi，splitPos 肯定 > 0
            // 如果是 -bi (即实部为0)，splitPos 为 0。
            if (!isScientific) {
                if (i == 0) {
                    // 符号在第一位，说明只有虚部 (例如 "-0.5")
                    // splitPos 保持 npos，让后面逻辑处理
                } else {
                    splitPos = i;
                }
                break; // 找到了，停止循环
            }
        }
    }

    double real = 0;
    double imag = 0;

    try {
        if (splitPos == std::string::npos) {
            // 没有分割符，或者符号在第一位 -> 纯虚数
            // s 此时只剩下数字部分 (例如 "-0.5" 或 "0.27")
            imag = std::stod(s);
        } else {
            // 有分割符 -> 复数
            // substr(0, splitPos) 是实部
            // substr(splitPos) 是虚部 (包含符号)
            std::string realPart = s.substr(0, splitPos);
            std::string imagPart = s.substr(splitPos);

            real = std::stod(realPart);
            imag = std::stod(imagPart);
        }
    } catch (...) {
        // 解析失败容错
        //return {0, 0};
        throw std::invalid_argument("解析系数失败: '" + s + "' 不是有效的复数格式。");
    }

    return {real * signMultiplier, imag * signMultiplier};
};

// 由字符串生成lambda// 必须定义在头文件中，以便编译器推导 auto 返回类型
// inline 关键字防止多个 cpp 包含该头文件时出现 "重定义" 错误
/**
 * 解析复数多项式字符串并返回一个高性能求值 Lambda。
 *
 * 输入格式示例: "(1+2i)x^3 + 4x^2 + (0-3i)x + 5"
 * 优化策略:
 * 1. 预解析为系数向量，Lambda 内部无字符串操作。
 * 2. Lambda 内部使用霍纳法则 (Horner's Method)。
 */
inline auto getPolynomialLambda(const std::string& input) {


    // --- 2. 解析逻辑 ---
    std::vector<std::pair<int, Complex>> terms;
    int maxExp = 0;
    // 正则表达式
    std::regex termRegex(R"(([+-]?\s*(?:\([^\)]+\)|[\d\.]+)?)?\s*(x)?(?:\^(\d+))?)");
    auto begin = std::sregex_iterator(input.begin(), input.end(), termRegex);
    auto end = std::sregex_iterator();

    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        std::string fullMatch = match.str();
        bool isWhitespace = std::all_of(fullMatch.begin(), fullMatch.end(), ::isspace);
        if (fullMatch.empty() || isWhitespace) continue;

        std::string coeffStr = match[1].str();
        bool hasX = match[2].matched;
        std::string expStr = match[3].str();

        Complex coeff = parseComplexCoeff(coeffStr);
        int exponent = 0;
        if (hasX) exponent = expStr.empty() ? 1 : std::stoi(expStr);

        if (exponent > maxExp) maxExp = exponent;
        terms.push_back({exponent, coeff});
    }

    std::vector<Complex> coeffs(maxExp + 1, {0, 0});
    for (const auto& term : terms) coeffs[term.first] += term.second;

    if (terms.empty()) {
        throw std::invalid_argument("输入为空或无法识别任何多项式项。");
    }

    // --- 3. 返回 Lambda ---
    return [coeffs](Complex z) -> Complex {
        if (coeffs.empty()) return {0,0};
        Complex result = coeffs.back();
        for (int i = static_cast<int>(coeffs.size()) - 2; i >= 0; --i) {
            result = result * z + coeffs[i];
        }
        return result;
    };
}

#endif // JULIADRAW_H
