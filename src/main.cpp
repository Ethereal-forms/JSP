//
// Created by qiming on 25-3-31.
//
#include "RandomGenerator.h"
#include "graph.h"
#include "instance.h"
#include "operation.h"
#include "schedule.h"
#include "tabusearch.h"

#include <deque>
#include <iostream>

// 测试代码
int main() {
    Instance instance = load_instance(R"(../../instance/ft/ft10.txt)");
    //RandomGenerator::instance().setSeed(12345);
    auto schedule = Schedule(instance);
    //schedule.export_to_csv("../../output/jssp_schedule1.csv");
    //std::cout<<schedule.makespan<<std::endl;




    tabusearch ts;
    ts.search(schedule);
    //ts.find_move(schedule);
    schedule.export_to_csv("../../output/jssp_schedule.csv");
    //std::cout<<schedule.makespan;
    return 0;
}
