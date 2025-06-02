#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

// ���� ��ȣ �߻� ��ġ

struct Signal
{
public:
    // �ʿ��� ���� ��ȣ 
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

