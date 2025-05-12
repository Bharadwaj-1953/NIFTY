// #include <iostream>
// #include <thread>
// #include <chrono>
// #include <unordered_map>
// #include <vector>
// #include <mutex>
// #include <cstdlib>
// #include <ctime>

// using namespace std;

// class Node {
// private:
//     string node_id;
//     vector<string> peers;
//     unordered_map<string, bool> peer_status;
//     unordered_map<string, int> missed_heartbeats;
//     mutex mtx;
//     const int max_missed_heartbeats = 3;  // Threshold for detecting node failure

// public:
//     Node(const string& id, const vector<string>& peer_list)
//         : node_id(id), peers(peer_list) {
//         for (const auto& peer : peers) {
//             peer_status[peer] = true;  // Assume all peers are initially alive
//             missed_heartbeats[peer] = 0; // Initialize missed heartbeats
//         }
//     }

//     void send_heartbeat() {
//         while (true) {  // Infinite loop to keep sending heartbeats
//             {
//                 lock_guard<mutex> lock(mtx);
//                 cout << "Node " << node_id << ": Sending heartbeat to peers..." << endl;
//             }
//             for (const auto& peer : peers) {
//                 this_thread::sleep_for(chrono::milliseconds(500));  // Simulate network delay
//                 // Simulate random failures
//                 if (rand() % 5 == 0) {
//                     lock_guard<mutex> lock(mtx);
//                     peer_status[peer] = false;
//                     missed_heartbeats[peer]++;
//                     cout << "Node " << node_id << ": Peer " << peer << " did not respond." << endl;
//                 } else {
//                     lock_guard<mutex> lock(mtx);
//                     peer_status[peer] = true;
//                     missed_heartbeats[peer] = 0;  // Reset missed heartbeats
//                     cout << "Node " << node_id << ": Received heartbeat response from " << peer << endl;
//                 }
//             }
//             this_thread::sleep_for(chrono::seconds(2));
//         }
//     }

//     void monitor_peers() {
//         while (true) {  // Infinite loop to keep monitoring peers
//             this_thread::sleep_for(chrono::seconds(2));
//             lock_guard<mutex> lock(mtx);
//             for (const auto& [peer, status] : peer_status) {
//                 if (!status) {
//                     if (missed_heartbeats[peer] >= max_missed_heartbeats) {
//                         cout << "ALERT: Node " << peer << " is not sending heartbeats!" << endl;
//                     }
//                 }
//             }
//         }
//     }
// };

// int main(int argc, char* argv[]) {
//     if (argc < 2) {
//         cerr << "Usage: " << argv[0] << " <node_id> [peer1 peer2 ...]" << endl;
//         return 1;
//     }

//     string node_id = argv[1];
//     vector<string> peers;

//     for (int i = 2; i < argc; ++i) {
//         peers.push_back(argv[i]);
//     }

//     srand(time(0));

//     Node node(node_id, peers);

//     thread sender(&Node::send_heartbeat, &node);
//     thread monitor(&Node::monitor_peers, &node);

//     sender.detach();
//     monitor.detach();

//     while (true) {
//         this_thread::sleep_for(chrono::hours(24));  // Keeps main alive indefinitely
//     }

//     return 0;
// }

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
    unordered_map<string, int> missed_heartbeats;
    mutex mtx;
    const int max_missed_heartbeats = 3;  // Threshold for detecting node failure

public:
    HeartbeatManager(const string& ip_file) {
        load_ips(ip_file);
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
            missed_heartbeats[ip] = 0; // Initialize missed heartbeats
        }
        file.close();

        cout << "Loaded IPs from " << ip_file << ":" << endl;
        for (const auto& node : nodes) {
            cout << "  - " << node << endl;
        }
    }

    void send_heartbeats() {
        while (true) {
            {
                lock_guard<mutex> lock(mtx);
                cout << "Sending heartbeat messages to all nodes..." << endl;
            }

            for (const auto& node : nodes) {
                this_thread::sleep_for(chrono::milliseconds(500));

                if (!send_heartbeat_to_node(node)) {
                    lock_guard<mutex> lock(mtx);
                    node_status[node] = false;
                    missed_heartbeats[node]++;
                    cout << "ALERT: Node " << node << " is not reachable!" << endl;
                } else {
                    lock_guard<mutex> lock(mtx);
                    node_status[node] = true;
                    missed_heartbeats[node] = 0; // Reset missed heartbeats
                    cout << "Node " << node << " responded successfully." << endl;
                }
            }

            this_thread::sleep_for(chrono::seconds(2));
        }
    }

    void monitor_nodes() {
        while (true) {
            this_thread::sleep_for(chrono::seconds(2));
            lock_guard<mutex> lock(mtx);
            for (const auto& [node, status] : node_status) {
                if (!status && missed_heartbeats[node] >= max_missed_heartbeats) {
                    cout << "CRITICAL ALERT: Node " << node << " is unreachable after multiple attempts!" << endl;
                }
            }
        }
    }

    bool send_heartbeat_to_node(const string& node) {
        // Actual network reachability logic using ping
        string command = "ping -c 1 -W 1 " + node + " > /dev/null 2>&1"; // Send 1 ping with a timeout of 1 second
        int result = system(command.c_str());
        if (result == 0) {
            return true; // Node is reachable
        } else {
            return false; // Node is not reachable
        }
    }
};

int main() {
    const string ip_file = "../NIFTY/src/all_ips.txt"; // Replace with the actual absolute path

    srand(time(0));

    HeartbeatManager manager(ip_file);

    thread sender(&HeartbeatManager::send_heartbeats, &manager);
    thread monitor(&HeartbeatManager::monitor_nodes, &manager);

    sender.detach();
    monitor.detach();

    while (true) {
        this_thread::sleep_for(chrono::hours(24));  // Keeps the main thread alive indefinitely
    }

    return 0;
}
