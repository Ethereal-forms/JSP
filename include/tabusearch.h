//
// Created by qiming on 25-4-4.
//

#ifndef JOBSHOPSCHEDULING_TABUSEARCH_H
#define JOBSHOPSCHEDULING_TABUSEARCH_H

#include <vector>
#include <unordered_map>
#include "schedule.h"

struct Action {
    int back_insert;// == 1 表示 u 插入到 v 之后
    int u;
    int v;
};
struct TabuItem {
    std::vector<int> operations;// 禁忌的操作顺序

    // 重载相等运算符，用于哈希表比较
    bool operator==(const TabuItem& other) const {
        return operations == other.operations;
    }
};

namespace std {
#ifdef HASH1
    template<>
    struct hash<TabuItem> {
        size_t operator()(const TabuItem& item) const {
            size_t seed = 0;
            // 组合vector中每个元素的哈希值
            for (const int& op : item.operations) {
                // 使用标准的哈希组合技术
                seed ^= hash<int>()(op) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
#elif HASH2
    template<>
    struct hash<TabuItem> {
        size_t operator()(const TabuItem& item) const {
            // FNV-1a哈希算法
            const size_t prime = 0x100000001b3;
            size_t hash = 0xcbf29ce484222325;

            for (const int& op : item.operations) {
                hash ^= static_cast<size_t>(op);
                hash *= prime;
            }
            return hash;
        }
    };
#elif HASH3
    template<>
    struct hash<TabuItem> {
        size_t operator()(const TabuItem& item) const {
            const auto& ops = item.operations;
            size_t size = ops.size();

            if (size == 0) return 0;

            // 小向量直接使用乘法哈希
            if (size <= 4) {
                size_t h = ops[0];
                for (size_t i = 1; i < size; ++i) {
                    h = h * 31 + ops[i];
                }
                return h;
            }

            // 较大向量采样哈希
            size_t h = size;
            h ^= ops[0] + ops[size-1];
            h ^= ops[size/2];
            h ^= ops[size/3];
            h ^= ops[2*size/3];
            return h;
        }
    };
#elif HASH4
    template<>
    struct hash<TabuItem> {
        size_t operator()(const TabuItem& item) const {
            // 如果vector内部数据连续，可以直接对内存块进行哈希
            if (item.operations.empty()) return 0;

            // 指向数据的指针
            const int* data = item.operations.data();
            // 数据大小（字节数）
            size_t bytes = item.operations.size() * sizeof(int);

            // MurmurHash2
            const uint64_t m = 0xc6a4a7935bd1e995ULL;
            const int r = 47;
            uint64_t h = 0x8445d61a4e774912ULL ^ (bytes * m);

            // 每次处理8字节
            const uint64_t* chunks = reinterpret_cast<const uint64_t*>(data);
            const uint64_t* end = chunks + (bytes / 8);

            while (chunks != end) {
                uint64_t k = *chunks++;
                k *= m;
                k ^= k >> r;
                k *= m;
                h ^= k;
                h *= m;
            }

            // 处理剩余字节
            const unsigned char* tail = reinterpret_cast<const unsigned char*>(chunks);
            switch (bytes & 7) {
                case 7: h ^= uint64_t(tail[6]) << 48; [[fallthrough]];
                case 6: h ^= uint64_t(tail[5]) << 40; [[fallthrough]];
                case 5: h ^= uint64_t(tail[4]) << 32; [[fallthrough]];
                case 4: h ^= uint64_t(tail[3]) << 24; [[fallthrough]];
                case 3: h ^= uint64_t(tail[2]) << 16; [[fallthrough]];
                case 2: h ^= uint64_t(tail[1]) << 8;  [[fallthrough]];
                case 1: h ^= uint64_t(tail[0]);
                    h *= m;
            };

            h ^= h >> r;
            h *= m;
            h ^= h >> r;

            return static_cast<size_t>(h);
        }
    };
#endif
}

/**
 * 禁忌表
 */
struct TabuList {
    // tabu_list[i][j] 表示在机器i上的，从第j个位置开始的所有禁忌操作
    std::vector<std::vector<std::unordered_map<TabuItem, unsigned long long>>> tabu_list;
};

class tabusearch {
public:
    void search(Schedule& sc);
    Action find_move(Schedule& sc,int& _id);
    void update(Schedule& sc,Action action,int block_id);

};


#endif//JOBSHOPSCHEDULING_TABUSEARCH_H
