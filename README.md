<h1 align="center">
NIFTY: Decentralized Fault Tolerance Framework for <br> Partial Network Partitioning in Distributed Systems
</h1>

---

## 📝 Abstract

<div align="justify">

NIFTY is a decentralized monitoring framework designed to enhance fault tolerance and resilience in distributed systems, especially under conditions of partial network partitioning. The project integrates three lightweight components — heartbeat-based failure detection, traffic monitoring, and node status logging — into a cohesive architecture. 

Unlike traditional centralized approaches, NIFTY distributes responsibilities across peer nodes, eliminating single points of failure and improving scalability. Through real-time local logging and proactive metrics correlation, system administrators gain actionable insights into network health, enabling early intervention and efficient performance optimization.

</div>

---

## 📁 Repository Structure

```bash
NIFTY/
├── deploy_nifty.sh          # Script to deploy binaries and config files across nodes
├── heartbeat                # Executable for heartbeat-based failure detection
├── traffic_monitor          # Executable for traffic monitoring
├── nodes.conf               # Static list of IPs for all participating nodes
├── nifty_nodes.conf         # Dynamic config with node bridge IPs and MACs
├── node_status.txt          # Local file logging node reachability status
├── traffic_log.txt          # Local file logging real-time traffic metrics
└── README.md                # Project documentation
```
---
## 🎯 Key Features

- Decentralized fault detection using heartbeat messages between neighboring nodes  
- Real-time traffic logging to identify data flow bottlenecks  
- Local log files (`node_status.txt`, `traffic_log.txt`) maintained per node  
- Scalable, lightweight peer-to-peer monitoring architecture  
- Secure communication using SSH and controlled file permissions  
- High detection accuracy with minimal bandwidth and resource overhead  

---

## ⚙️ Deployment Instructions

### 1. System Requirements

- OS: Linux-based system  
- Utilities: `ping`, `ssh`, `scp`, `chmod`  
- Password-less SSH enabled across all participating nodes  

### 2. Setup & Execution

From the master node:

```bash
chmod +x deploy_nifty.sh
./deploy_nifty.sh
```

This script will:

- Deploy binaries and config files to all nodes  
- Create the `nifty_nodes.conf` with MAC/IP details  
- Set proper permissions for monitoring logs  

---

## 📊 Monitoring Components

### Heartbeat Monitoring

- Sends lightweight `ping` messages to immediate neighbors  
- Uses a configurable threshold to reduce false positives during transient disruptions  
- Generates `node_status.txt` with real-time reachability status  

### Traffic Monitoring

- Logs inbound and outbound traffic data on each node  
- Filters unreachable peers using data from `node_status.txt`  
- Outputs metrics to `traffic_log.txt`  

### Log File Integration

Logs are stored locally and updated periodically:
```bash
├── node_status.txt      # Reachability of neighboring nodes  
├── traffic_log.txt      # Packet counts, traffic volume, and timestamps  
```
---

## 🧪 Testing & Results

Tested using a simulated 8-node environment with varying workloads and network failures:

| Nodes | Avg Latency | Detection Accuracy | CPU Usage | Bandwidth Overhead             |
|-------|-------------|--------------------|-----------|--------------------------------|
| 2     | 4 ms        | 99.5%              | 1.5%      | 0.2 KB/s (Heartbeat only)      |
| 8     | 10 ms       | 98.5%              | 3.0%      | 0.8 KB/s (HB), 2 KB/s (Traffic)|

> *HB = Heartbeat*

Performance includes tests under network partition scenarios and injected failures.

---

## 🔐 Security Measures

- All inter-node communication uses `scp` and password-less `ssh`  
- Log files are protected using `chmod` for restricted write access  
- Decentralized logging avoids single points of failure and reduces attack surface  

---

## 📊 Experimental Results and Analysis

- Extensive experiments were conducted on simulated distributed environments to evaluate the runtime performance, fault detection accuracy, bandwidth usage, and scalability of the NIFTY framework.  
- Testing scenarios included varying node counts, induced network partitions, and different heartbeat frequencies and thresholds.
- Detailed runtime logs, reachability trends, and bandwidth metrics were collected during each test phase.  
- Visualizations of log outputs, node failure detection timelines, and traffic patterns are available upon request.
> If you would like access to the complete experimental results, data visualizations, or wish to discuss the framework in more depth, please feel free to contact me.
---
🌐 **Contact:** 
- **Email**: manne.bharadwaj.1953@gmail.com
- **LinkedIn**: [Bharadwaj Manne](https://www.linkedin.com/in/bharadwaj-manne-711476249/)
