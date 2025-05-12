#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <string>
#include <cstdlib> // For system()

using namespace std;

class HeartbeatManager {
private:
    vector<string> nodes;
    unordered_map<string, bool> node_status;
    mutex mtx;
    const string status_file;
    const int max_missed_heartbeats = 3;  // Threshold for detecting node failure

public:
    HeartbeatManager(const string& ip_file, const string& output_file)
        : status_file(output_file) {
        load_ips(ip_file);
        initialize_status_file();
    }

    void load_ips(const string& ip_file) {
        ifstream file(ip_file);
        if (!file) {
            cerr << "Error: Could not open " << ip_file << endl;
            exit(1);
        }

        string ip;
        while (file >> ip) {
            nodes.push_back(ip);
            node_status[ip] = true;  // Assume all nodes are initially reachable
        }
        file.close();

        cout << "Loaded IPs from " << ip_file << ":" << endl;
        for (const auto& node : nodes) {
            cout << "  - " << node << endl;
        }
    }

    void initialize_status_file() {
        ofstream file(status_file, ios::trunc);
        if (!file) {
            cerr << "Error: Could not create " << status_file << endl;
            exit(1);
        }
        file.close();
    }

    void write_status_file() {
        lock_guard<mutex> lock(mtx);
        ofstream file(status_file, ios::trunc);
        if (!file) {
            cerr << "Error: Could not open " << status_file << endl;
            return;
        }

        for (const auto& [node, status] : node_status) {
            file << node << " " << (status ? "reachable" : "unreachable") << endl;
        }

        file.close();
        cout << "Node status file updated successfully." << endl;
    }

    bool ping_node(const string& node) {
        string command = "ping -c 1 -W 1 " + node + " > /dev/null 2>&1"; // Send 1 ping with a timeout of 1 second
        int result = system(command.c_str());
        return result == 0;
    }

    void send_heartbeats() {
        while (true) {
            {
                lock_guard<mutex> lock(mtx);
                cout << "Sending heartbeat signals to all nodes..." << endl;
            }

            for (const auto& node : nodes) {
                if (ping_node(node)) {
                    lock_guard<mutex> lock(mtx);
                    node_status[node] = true;
                    cout << "Node " << node << " is reachable." << endl;
                } else {
                    lock_guard<mutex> lock(mtx);
                    node_status[node] = false;
                    cout << "ALERT: Node " << node << " is unreachable!" << endl;
                }
            }

            write_status_file(); // Update node_status.txt after each heartbeat round
            this_thread::sleep_for(chrono::seconds(3)); // Update every 3 seconds
        }
    }
};

int main() {
    const string ip_file = "../Desktop/NIFTY/src/all_ips.txt";           // Path to the file with node IPs
    const string status_file = "../Desktop/NIFTY/log_files/node_status.txt";   // File to write node statuses

    srand(time(0));

    HeartbeatManager heartbeat_manager(ip_file, status_file);

    thread heartbeat_thread(&HeartbeatManager::send_heartbeats, &heartbeat_manager);

    heartbeat_thread.detach();

    // Keep the main thread alive indefinitely
    while (true) {
        this_thread::sleep_for(chrono::hours(24));
    }

    return 0;
}