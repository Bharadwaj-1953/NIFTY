#pragma once

#include <string>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <condition_variable>

class Node {
private:
    std::string node_id;
    std::vector<std::string> peers;
    std::unordered_map<std::string, bool> peer_status;
    std::mutex mtx;
    std::condition_variable cv;
    bool is_alive;

public:
    Node(const std::string& id, const std::vector<std::string>& peer_list);

    void send_heartbeat();
    void monitor_peers();
    void shutdown();
};
