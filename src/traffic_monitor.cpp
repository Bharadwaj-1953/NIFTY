#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>
#include <string>
#include <cstdlib>

using namespace std;

class TrafficMonitor {
private:
    unordered_map<string, pair<int, int>> traffic_counters; // {node_ip: {incoming, outgoing}}
    unordered_map<string, bool> overload_status;
    const int traffic_threshold;
    const string log_file;
    vector<string> nodes;
    mutex mtx;

public:
    TrafficMonitor(int threshold, const string& log_file_path)
        : traffic_threshold(threshold), log_file(log_file_path) {}

    void load_nodes(const string& ip_file) {
        ifstream file(ip_file);
        if (!file) {
            cerr << "Error: Could not open " << ip_file << endl;
            exit(1);
        }

        string ip;
        while (file >> ip) {
            nodes.push_back(ip);
            traffic_counters[ip] = {0, 0};
            overload_status[ip] = false;
        }
        file.close();

        cout << "Loaded IPs from " << ip_file << ":" << endl;
        for (const auto& node : nodes) {
            cout << "  - " << node << endl;
        }
    }

    void record_incoming(const string& node_ip) {
        lock_guard<mutex> lock(mtx);
        traffic_counters[node_ip].first++;
    }

    void record_outgoing(const string& node_ip) {
        lock_guard<mutex> lock(mtx);
        traffic_counters[node_ip].second++;
    }

    void log_traffic() {
        lock_guard<mutex> lock(mtx);
        ofstream log(log_file, ios::trunc); // Clear and overwrite each time
        if (!log.is_open()) {
            cerr << "Error: Could not open log file!" << endl;
            return;
        }
        for (const auto& [node, counters] : traffic_counters) {
            log << "Node: " << node
                << ", Incoming: " << counters.first
                << ", Outgoing: " << counters.second
                << ", Overloaded: " << (overload_status[node] ? "Yes" : "No")
                << endl;
        }
        log.close();
        cout << "Traffic log updated successfully." << endl;
    }

    void monitor_traffic() {
        while (true) {
            {
                lock_guard<mutex> lock(mtx);
                cout << "Monitoring traffic..." << endl;
            }

            for (auto& [node, counters] : traffic_counters) {
                int total_traffic = counters.first + counters.second;
                if (total_traffic > traffic_threshold) {
                    overload_status[node] = true;
                    cout << "ALERT: Node " << node << " is overloaded!" << endl;
                } else {
                    overload_status[node] = false;
                }
            }
            log_traffic(); // Update the log file
            reset_counters(); // Reset traffic counters for the next interval
            this_thread::sleep_for(chrono::seconds(3)); // Update every 3 seconds
        }
    }

    void reset_counters() {
        lock_guard<mutex> lock(mtx);
        for (auto& [node, counters] : traffic_counters) {
            counters = {0, 0};
        }
    }

    void simulate_traffic() {
        while (true) {
            for (const auto& node : nodes) {
                record_incoming(node);
                record_outgoing(node);
            }
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }
};

int main() {
    const string ip_file = "../Desktop/NIFTY/src/all_ips.txt";  // Path to the file with node IPs
    const int traffic_threshold = 50;            // Threshold for traffic overload
    const string traffic_log = "../Desktop/NIFTY/log_files/traffic_log.txt"; // Traffic log file path

    TrafficMonitor traffic_monitor(traffic_threshold, traffic_log);

    traffic_monitor.load_nodes(ip_file);

    thread traffic_sim_thread(&TrafficMonitor::simulate_traffic, &traffic_monitor);
    thread traffic_monitor_thread(&TrafficMonitor::monitor_traffic, &traffic_monitor);

    traffic_sim_thread.detach();
    traffic_monitor_thread.detach();

    while (true) {
        this_thread::sleep_for(chrono::hours(24)); // Keep the program running indefinitely
    }

    return 0;
}
