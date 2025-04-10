//
// Created by qiming on 25-4-4.
//
// ----4 5 6 8 7 9-----
#include "tabusearch.h"
#include "RandomGenerator.h"

#include <bits/fs_fwd.h>
//u 移动到 v 之后 Q[v] >= Q[J_SUC[u]]
std::vector<Action>  N7(std::vector<int> block) {
    std::vector<Action> actions;
    int n = block.size();
    if (n == 2) {
        actions.push_back({1,0,1});
        return actions;
    }
    //头部 移动到中间
    for (int i = 1; i < n; i++) {
        actions.push_back({1,0,i});
    }

    //尾部移动到中间
    for (int i = 0; i < n-1; i++) {
        actions.push_back({0,n-1,i});
    }

    //中间移动到两端
    for (int i = 1; i < n-1; i++) {
        if (i!=1) {
            actions.push_back({0,i,0});
        }
        if (i!=n-2) {
            actions.push_back({1,i,n-1});
        }
    }
    return actions;
};

void tabusearch::search(Schedule& sc) {
    int iter = 0;
    while (iter<10000) {
        int block_id;
        Action ac = find_move(sc,block_id);
        update(sc,ac,block_id);
        std::cout<<"iter  "<<sc.makespan<<std::endl;
        iter++;
    }

}
bool tabusearch::action_is_legal(Action action,const std::vector<int>& block,Schedule& sc)  {
    if (action.back_insert == 1) {
        // u 插入 v 后
        int operator_v = block[action.v];
        int operator_u = block[action.u];
        int operator_u_suc = sc.graph.operation_edges[operator_u];
        if (sc.time_info[operator_v].end_time < sc.time_info[operator_u_suc].backward_path_length) {
            return false;
        }

    }else {
        // u 插入 v 前
        int operator_v = block[action.v];
        int operator_u = block[action.u];
        int operator_u_pre = sc.graph.reverse_operation_edges[operator_u];
        if (sc.time_info[operator_v].forward_path_length < sc.time_info[operator_u_pre].forward_path_length) {
            return false;
        }
    }
    return true;
}

Action tabusearch::find_move(Schedule& sc,int& _id) {
    int best_makespan = INT_MAX;
    std::vector<int> block_id;
    std::vector<Action> best_move;
    for (int i = 0; i<sc.critical_blocks.size(); ++i) {
        std::vector<int> block = sc.critical_blocks[i];
        std::vector<Action> actions;
        std::vector<Action> move_set = N7(block);
        for (int j = 0; j < move_set.size(); ++j) {
            Graph graph = sc.graph;
            int pre = graph.reverse_machine_edges[block[0]];
            int suc = graph.machine_edges[block[block.size()-1]];
            Action action = move_set[j];
            //判断序列是否符合合法
            int start;
            int end;
            if (!action_is_legal(action,block,sc)) {
                continue;
            }

            start = std::min(action.v, action.u);
            end = std::max(action.v, action.u);
            if (action.back_insert == 1) {
                //后插 u 插入 v 后
                if (action.v == block.size()-1) {
                    block.push_back(block[action.u]);
                }else {
                    block.emplace(block.begin()+action.v+1,block[action.u]);
                }

                if (action.u < action.v) {
                    block.erase(block.begin()+action.u);
                }else {
                    block.erase(block.begin()+action.u+1);
                }

            }else {
                //前插 u 插入 v 前
                block.emplace(block.begin()+action.v,block[action.u]);
                if (action.u < action.v) {
                    block.erase(block.begin()+action.u);
                }else {
                    block.erase(block.begin()+action.u+1);
                }
            }
            /*
             *
             *判断禁忌
             *.......
             *.......
             */
            std::vector<int> temp = block;
            int tem = block[end];
            block.assign(block.begin()+start,block.begin()+end);
            block.push_back(tem);
            std::vector<int> R(block.size(),0);
            std::vector<int> Q(block.size(),0);

            int n = block.size();
            for (int k = 0; k < block.size(); ++k) {
                //Action action = actions[k];

                int operator_rid = block[k];
                int job_pre = graph.reverse_operation_edges[operator_rid];
                if (k==0) {
                    int machine_pre = pre;
                    if (machine_pre != -1) {
                        R[0] = std::max(sc.time_info[job_pre].end_time,sc.time_info[machine_pre].end_time);
                    }else {
                        R[0] = sc.time_info[job_pre].end_time;
                    }
                }else {
                    int machine_pre_id = block[k-1];
                    R[k] = std::max(sc.time_info[job_pre].end_time,R[k-1] + sc.operation_list[machine_pre_id].time);
                }

                int operator_qid = block[n-1-k];

                int job_suc = graph.operation_edges[operator_qid];
                if (k == 0){
                    int machine_suc = suc;
                    if (machine_suc != -1) {
                        Q[n-1] = std::max(sc.time_info[job_suc].backward_path_length,sc.time_info[machine_suc].backward_path_length) + sc.operation_list[operator_qid].time;
                    }else {
                        Q[n-1] = sc.time_info[job_suc].backward_path_length + sc.operation_list[operator_qid].time;
                    }
                }else {
                    //int machine_suc = move_set[j][n-k];
                    Q[n-k-1] = std::max(sc.time_info[job_suc].backward_path_length,Q[n-k]) + sc.operation_list[operator_qid].time;
                }
            }
            block = temp;
            int makespan = R[0] + Q[0];
            for (int k = 1; k < block.size(); ++k) {
                if (R[k] + Q[k] > makespan) makespan = R[k] + Q[k];
            }
            if (best_makespan > makespan) {
                best_makespan = makespan;
                best_move.clear();
                block_id.clear();
                best_move.push_back(action);
                block_id.push_back(i);
            }else if (best_makespan == makespan) {
                best_move.push_back(action);
                block_id.push_back(i);
            }

        }
    }

    int random = RAND_INT(best_move.size());
    _id = block_id[random];
    return best_move[random];

}


void tabusearch::update(Schedule& sc,Action ac,int block_id) {


    std::vector<int> block = sc.critical_blocks[block_id];
    int operator_u = block[ac.u];
    int operator_v = block[ac.v];

    int operator_u_pre = sc.graph.reverse_machine_edges[operator_u];
    int operator_v_pre = sc.graph.reverse_machine_edges[operator_v];
    int operator_u_suc = sc.graph.machine_edges[operator_u];
    int operator_v_suc = sc.graph.machine_edges[operator_v];

    if (ac.back_insert == 1) {
        // u in v back
        if (operator_u_pre != -1)
            sc.graph.machine_edges[operator_u_pre] = operator_u_suc;

        if (operator_u_suc != -1)
            sc.graph.reverse_machine_edges[operator_u_suc] = operator_u_pre;

        sc.graph.machine_edges[operator_u] = operator_v_suc;

        if (operator_v_suc != -1)
            sc.graph.reverse_machine_edges[operator_v_suc] = operator_u;

        sc.graph.machine_edges[operator_v] = operator_u;
        sc.graph.reverse_machine_edges[operator_u] = operator_v;
    }else {
        if (operator_u_pre!=-1)
            sc.graph.machine_edges[operator_u_pre] = operator_u_suc;
        if (operator_u_suc != -1)
            sc.graph.reverse_machine_edges[operator_u_suc] = operator_u_pre;

        sc.graph.machine_edges[operator_u] = operator_v;

        sc.graph.reverse_machine_edges[operator_v] = operator_u;
        if (operator_v_pre != -1)
            sc.graph.machine_edges[operator_v_pre] = operator_u;

        sc.graph.reverse_machine_edges[operator_u] = operator_v_pre;
    }
    sc.calculate_time_info();
    sc.update_critical_blocks();
    //sc.export_to_csv("../../output/jssp_schedule.csv");

}