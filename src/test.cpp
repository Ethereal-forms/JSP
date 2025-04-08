//
// Created by PC on 25-4-7.
//
#include <iostream>
#include <vector>
std::vector<std::vector<int>>  _N7(std::vector<int> block) {
    int n = block.size();
    std::vector<std::vector<int>> move_set;
    for (int i = 1; i < n-1; i++) {
        std::vector<int> tmp = block;
        tmp.emplace(tmp.begin()+i+1, tmp[0]);
        tmp.erase(tmp.begin());
        move_set.emplace_back(tmp);
    }

    for (int i = 1; i < n-1; i++) {
        std::vector<int> tmp = block;
        tmp.emplace(tmp.begin()+i, tmp[n-1]);
        tmp.erase(tmp.begin()+n);
        move_set.emplace_back(tmp);
    }
    for (int i = 1; i < n-1; i++) {
        std::vector<int> tmp_head = block;
        std::vector<int> tmp_tail = block;
        if (i!=1) {
            tmp_head.emplace(tmp_head.begin(), tmp_head[i]);
            tmp_head.erase(tmp_head.begin()+i+1);
            move_set.emplace_back(tmp_head);
        }
        if (i!=n-2) {
            tmp_tail.push_back(tmp_tail[i]);
            tmp_tail.erase(tmp_tail.begin()+i);
            move_set.emplace_back(tmp_tail);
        }
    }
    return move_set;
};
//
// int main() {
//     std::vector<int> block{4,7,8,9,6,2};
//     std::vector<std::vector<int>> move_set = _N7(block);
//     for (int i = 0; i < move_set.size(); i++) {
//     }
// }