#pragma once

#include<iostream>
#include<cstring>
#include<string>
#include<vector>
#include<map>
#include<memory>
#include<chrono>
#include<thread>
#include"nlohmann/json.hpp"

#define MAX_FLOOR 3

namespace CPT106_Group
{

enum class TRANSPORTATION
{
    ERROR = 0,
    CAR = 1,
    TRUNK = 2,
    VAN = 3,
    MOTORCYCLE = 4,
    OTHERS = 5,
};

struct Ticket
{
    int floor;
    int id;
    std::string License_plate_number;
    TRANSPORTATION transportation;
    std::chrono::system_clock::time_point get_time;
};



}