//
// Created by 祁明 on 25-4-7.
//

#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H
#include <mutex>
#include <random>

#define RAND_INT(x) RandomGenerator::instance().getInt(x)

class RandomGenerator {
public:
    static RandomGenerator &instance() {
        static RandomGenerator instance;
        return instance;
    }

    // 修改随机数种子
    void setSeed(unsigned seed) {
        std::lock_guard<std::mutex> lock(mtx);
        gen.seed(seed);
    }

    // 使用默认随机种子
    RandomGenerator()
        : gen(std::random_device{}()) {}

    // 整数 [min, max]
    int getInt(int min, int max) {
        std::lock_guard<std::mutex> lock(mtx);
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }

    // 整数 [0, max)
    int getInt(int max) {
        return getInt(0, max - 1);
    }

    // 浮点数 [min, max)
    double getDouble(double min, double max) {
        std::lock_guard<std::mutex> lock(mtx);
        std::uniform_real_distribution<double> dist(min, max);
        return dist(gen);
    }

    double getDouble(double max) {
        return getDouble(0, max);
    }

    // true 或 false
    bool getBool() {
        return getInt(0, 1) == 1;
    }

    // 正态分布随机数（均值 mean，标准差 stddev）
    double getNormal(double mean = 0.0, double stddev = 1.0) {
        std::lock_guard<std::mutex> lock(mtx);
        std::normal_distribution<double> dist(mean, stddev);
        return dist(gen);
    }

    RandomGenerator(const RandomGenerator &) = delete;
    RandomGenerator &operator=(const RandomGenerator &) = delete;

private:
    std::mt19937 gen;
    std::mutex mtx;// 保证线程安全
};

#endif//RANDOM_GENERATOR_H