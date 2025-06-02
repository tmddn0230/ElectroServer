#pragma once

#include <iostream>
#include <sstream>
#include <string>

struct Signal;

class Serializer
{

public:
	std::string SerializeToJson(const Signal& sig);


};

