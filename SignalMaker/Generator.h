#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

// 전기 신호 발생 장치

struct Signal
{
public:
    // 필요한 전기 신호 
    std::string device_id;
    std::string timestamp;
    float voltage;
    float current;
    //float power;
    float power_factor;
    float frequency;
    std::string status;
};

class Generator
{
public:
    float randf(float min, float max);
    std::string GetCurrentTime();
    Signal GenerateSignal(std::string devName, int myMode);
};

