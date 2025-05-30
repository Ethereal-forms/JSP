//
// Created by qiming on 25-4-1.
//

#ifndef JOBSHOPSCHEDULING_SCHEDULE_H
#define JOBSHOPSCHEDULING_SCHEDULE_H
#include "graph.h"
#include "instance.h"
#include "operation.h"
#include <algorithm>
#include <random>


/**
 * 用于近似邻域评估策略
 * forward_path_length : R[i] 表示从起始工序到工序i的最长路径长度
 *  - 求法：R[i] = Max{ R[j]+T[j], R[k]+T[k] } （j、 k 为 i 的前序工序）
 * backward_path_length : Q[i] 表示从工序i到终止工序的最长路径长度
 * - 求法：Q[i] = Max{ Q[j], Q[k]+T[i] }  （j、 k 为 i 的后序工序）
 */
struct OperationTimeInfo {
    int operator_id;
    int forward_path_length;
    int backward_path_length;
    int end_time;// forward_path_length + op_time

    bool operator<(const OperationTimeInfo &other) const {
        return forward_path_length < other.forward_path_length;
    }
};


class Schedule {
public:
    explicit Schedule(const Instance &instance) noexcept : operation_list(instance), machine_operation(instance), makespan(0) {
        graph = generate_random_initial_solution(instance);
        calculate_time_info();
        //critical_path();
        update_critical_blocks();
    }

    void export_to_csv(const std::string &filename);

    bool is_critical_operation(int operation_id);
    void calculate_time_info();
    void update_critical_blocks();
    void critical_path();

    OperationList operation_list;
    MachineOperation machine_operation;
    Graph graph;

    int makespan;
    std::vector<std::vector<int>> path;
    std::vector<OperationTimeInfo> time_info;
    std::vector<std::vector<int>> critical_blocks;

};

inline bool Schedule::is_critical_operation(int operation_id) {
    return (time_info[operation_id].forward_path_length + time_info[operation_id].backward_path_length == makespan);
}


#endif//JOBSHOPSCHEDULING_SCHEDULE_H
