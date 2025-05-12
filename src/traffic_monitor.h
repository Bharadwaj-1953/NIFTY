#pragma once

#include <unordered_map>
#include <vector>
#include <mutex>
#include <string>

class TrafficMonitor {
private:
    std::vector<std::string> nodes;  // List of nodes to monitor
    std::unordered_map<std::string, std::pair<int, int>> traffic_counters; // {node_ip: {incoming, outgoing}}
    std::unordered_map<std::string, bool> overload_status;  // {node_ip: overloaded or not}
    const int traffic_threshold;  // Threshold for considering traffic as overloaded
    const std::string log_file;  // Path to the log file for recording traffic details
    std::mutex mtx;  // Mutex for thread-safe operations

    void reset_counters();  // Reset traffic counters after logging

public:
    // Constructor: Initializes the TrafficMonitor with an IP file, threshold, and log file
    TrafficMonitor(const std::string& ip_file, int threshold, const std::string& log_file_path);

    // Loads the list of IPs to monitor from a file
    void load_ips(const std::string& ip_file);

    // Records an incoming traffic event for a given node
    void record_incoming(const std::string& node_ip);

    // Records an outgoing traffic event for a given node
    void record_outgoing(const std::string& node_ip);

    // Logs the current traffic status to the log file
    void log_traffic();

    // Monitors traffic continuously and checks for overload
    void monitor_traffic();

    // Simulates traffic events for testing purposes
    void simulate_traffic();
};
