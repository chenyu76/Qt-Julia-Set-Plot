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
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    if (s.empty()) return {1, 0};

    double signMultiplier = 1.0;
    if (s.front() == '-') { signMultiplier = -1.0; s.erase(0, 1); }
    else if (s.front() == '+') { s.erase(0, 1); }

    if (s.empty()) return {1.0 * signMultiplier, 0};

    // 去括号
    if (s.front() == '(' && s.back() == ')') s = s.substr(1, s.length() - 2);

    // 无 'i' 视为纯实数
    if (s.find('i') == std::string::npos) {
        try { return {std::stod(s) * signMultiplier, 0}; } catch (...) { return {0,0}; }
    }

    s.pop_back(); // remove 'i'

    // 寻找分割点
    size_t splitPos = std::string::npos;
    for (int i = static_cast<int>(s.length()) - 1; i >= 0; --i) {
        char c = s[i];
        // 排除科学计数法的 e- 或 E-
        if ((c == '+' || c == '-') && !(i > 0 && (s[i-1] == 'e' || s[i-1] == 'E'))) {
            if (i == 0) {} else { splitPos = i; }
            break;
        }
    }

    double real = 0, imag = 0;
    try {
        if (splitPos == std::string::npos) imag = std::stod(s);
        else {
            real = std::stod(s.substr(0, splitPos));
            imag = std::stod(s.substr(splitPos));
        }
    } catch (...) { throw std::invalid_argument("解析复数失败"); }

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
 *
 * 返回一个pair(lambda, str)
 * 一个可执行的函数和这个函数的字符串表示
 */
inline std::pair<std::function<std::complex<double>(std::complex<double>)>, std::string>
getPolynomialLambda(const std::string& input) {
    using Complex = std::complex<double>;

    std::vector<std::pair<int, Complex>> terms;
    int maxExp = 0;

    std::regex termRegex(R"(([+-]?\s*(?:\([^\)]+\)|[\d\.]+)?)\s*(z)?(?:\^(\d+))?)");

    auto begin = std::sregex_iterator(input.begin(), input.end(), termRegex);
    auto end = std::sregex_iterator();

    for (std::sregex_iterator i = begin; i != end; ++i) {
        std::smatch match = *i;
        std::string fullMatch = match.str();

        // 基础空白检查
        if (fullMatch.empty() || std::all_of(fullMatch.begin(), fullMatch.end(), ::isspace)) continue;

        std::string coeffStr = match[1].str();
        bool hasX = match[2].matched;
        std::string expStr = match[3].str();

        // 修复逻辑：解决 "x^2 + -x" 中间的 "+" 被识别为常数 1 的问题。
        // 如果这一项没有 x (看起来像常数)，但系数部分其实不包含任何有效数字内容
        // (没有数字、小数点、i、右括号)，说明它只是一个被正则孤立出来的连接符。
        if (!hasX) {
            bool hasEffectiveContent = false;
            for (char c : coeffStr) {
                // 只要包含数字、小数点、虚数单位i、或者右括号(表示复数结束)，就是有效常数
                if (isdigit(c) || c == '.' || c == 'i' || c == ')') {
                    hasEffectiveContent = true;
                    break;
                }
            }
            // 如果只有 + / - 或空格，跳过该匹配
            if (!hasEffectiveContent) continue;
        }

        Complex coeff = parseComplexCoeff(coeffStr);
        int exponent = 0;

        if (hasX) {
            exponent = expStr.empty() ? 1 : std::stoi(expStr);
        }

        if (exponent > maxExp) maxExp = exponent;
        terms.push_back({exponent, coeff});
    }

    if (terms.empty()) throw std::invalid_argument("未检测到有效的多项式项");

    std::vector<Complex> coeffs(maxExp + 1, {0, 0});
    for (const auto& term : terms) coeffs[term.first] += term.second;

    // 3. 构建显示字符串 (保持不变)
    std::stringstream ss;
    bool isFirst = true;
    for (int i = static_cast<int>(coeffs.size()) - 1; i >= 0; --i) {
        Complex c = coeffs[i];
        if (std::abs(c.real()) < 1e-10 && std::abs(c.imag()) < 1e-10) {
            if (i == 0 && isFirst) ss << "0";
            continue;
        }

        if (!isFirst) ss << " + ";

        std::string coeffStr;
        if (std::abs(c.imag()) < 1e-10) {
            if (i > 0 && std::abs(c.real() - 1.0) < 1e-10) coeffStr = "";
            else if (i > 0 && std::abs(c.real() + 1.0) < 1e-10) coeffStr = "-";
            else { std::stringstream temp; temp << c.real(); coeffStr = temp.str(); }
        } else if (std::abs(c.real()) < 1e-10) {
            std::stringstream temp;
            if (std::abs(c.imag() - 1.0) < 1e-10) temp << "i";
            else if (std::abs(c.imag() + 1.0) < 1e-10) temp << "-i";
            else temp << c.imag() << "i";
            coeffStr = temp.str();
        } else {
            std::stringstream temp; temp << "(" << c.real() << (c.imag()>=0?"+":"") << c.imag() << "i)";
            coeffStr = temp.str();
        }
        ss << coeffStr;
        if (i > 0) { ss << "z"; if (i > 1) ss << "^" << i; }
        isFirst = false;
    }
    // 将 + -x 替换为-x
    auto ss_str = std::regex_replace(ss.str(), std::regex("\\+ \\-"), "- ");

    // 4. 返回 Lambda
    auto lambda = [coeffs](Complex z) -> Complex {
        if (coeffs.empty()) return {0,0};
        Complex result = coeffs.back();
        for (int i = static_cast<int>(coeffs.size()) - 2; i >= 0; --i) {
            result = result * z + coeffs[i];
        }
        return result;
    };

    return {lambda, ss_str};
}
#endif // JULIADRAW_H
