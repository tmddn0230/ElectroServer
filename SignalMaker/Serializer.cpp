#include "Serializer.h"
#include "Generator.h"



// JSON
std::string Serializer::SerializeToJson(const Signal& sig)
{
    std::ostringstream oss;
    oss << "{";
    oss << "\"device_id\":\"" << sig.device_id << "\",";
    oss << "\"timestamp\":\"" << sig.timestamp << "\",";
    oss << "\"voltage\":" << sig.voltage << ",";
    oss << "\"current\":" << sig.current << ",";
    //oss << "\"power\":" << sig.power << ",";
    oss << "\"power_factor\":" << sig.power_factor << ",";
    oss << "\"frequency\":" << sig.frequency << ",";
    oss << "\"status\":\"" << sig.status << "\"";
    oss << "}";
    return oss.str();
}
