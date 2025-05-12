CC=g++
CFLAGS=-c -std=c++17 -lpthread -pthread
LDFLAGS= -lpthread
DEPS = dv.h

SRC_DIR := src
OBJ_DIR := obj

.PHONY: all clean nifty partitioner heartbeat traffic_monitor

all: nifty partitioner heartbeat traffic_monitor

debug: CFLAGS += -Wall -DDEBUG -g
debug: all

nifty: $(OBJ_DIR)/daemon.o $(OBJ_DIR)/nifty.o 
	$(CC) -o $@ $^ -pthread

partitioner: $(OBJ_DIR)/partitioner.o
	$(CC) -o $@ $^

heartbeat: $(OBJ_DIR)/heartbeat.o
	$(CC) -o $@ $^ $(LDFLAGS)

traffic_monitor: $(OBJ_DIR)/traffic_monitor.o
	$(CC) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/nifty.h | $(OBJ_DIR) 
	$(CC) -o $@ $< $(CFLAGS)

$(OBJ_DIR)/heartbeat.o: $(SRC_DIR)/heartbeat.cpp $(SRC_DIR)/heartbeat.h | $(OBJ_DIR) 
	$(CC) -o $@ $< $(CFLAGS)

$(OBJ_DIR)/traffic_monitor.o: $(SRC_DIR)/traffic_monitor.cpp $(SRC_DIR)/traffic_monitor.h | $(OBJ_DIR)
	$(CC) -o $@ $< $(CFLAGS)

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm  -r $(OBJ_DIR) nifty partitioner heartbeat traffic_monitor
