#!/bin/bash

set -x
# get NIFTY_HOME
. ../configuration

# If your ssh need more options, you can set them in the $sshOptions variable (you can set identify file, port, ...)
sshOptions=" -T "


ALL_IPS_FILE="../src/all_ips.txt"
TRAFFIC_LOG="../log_files/traffic_log.txt"
NODE_STATUS="../log_files/node_status.txt"
TRAFFIC_MONITOR_EXEC="../traffic_monitor"
HEARTBEAT_EXEC="../heartbeat"
> $ALL_IPS_FILE
> $NODE_STATUS
> $TRAFFIC_LOG
# Iterate through all the nodes, get their brdige IPs and MACs and save them (used to update nifty's nodes.conf)
ips="";
macs="";
nodesCount=0;
while IFS= read -r nodeIP
do
	if [ -z $nodeIP ]; then
		continue;
	fi
  	#ssh into the node and get its IP and MAC.
  	ip=$(ssh -n $sshOptions $nodeIP ip addr show br0 | grep 'inet ' | cut -f2 | awk '{print $2}' | rev | cut -c4- | rev)
  	mac=$(ssh -n $sshOptions $nodeIP cat /sys/class/net/br0/address)

	if [ -n $ip ]; then
		echo "$ip" >> $ALL_IPS_FILE
		echo "Discovered IP for node $nodeIP: $ip"
	else
		echo "Failed to retrieve IP for node $nodeIP"
	fi

	ips="${ips}${ip}\n"
	macs="${macs}${mac}\n"
	let nodesCount=nodesCount+1
done < ./nodes.conf

chmod 777 $ALL_IPS_FILE


while IFS= read -r nodeIP; do
	if [ -z $nodeIP ]; then
		continue;
	fi

	echo "copying all_ips.txt to node $nodeIP"
	scp $sshOptions $ALL_IPS_FILE $nodeIP:"${NIFTY_HOME}/src/all_ips.txt"
	ssh -n $sshOptions $nodeIP "chmod 777 ${NIFTY_HOME}/src/all_ips.txt"

	echo "copying traffic_monitor to node $nodeIP"
	scp $sshOptions $TRAFFIC_MONITOR_EXEC $nodeIP:"${NIFTY_HOME}/traffic_monitor"
	ssh -n $sshOptions $nodeIP "sudo chmod +x ${NIFTY_HOME}/traffic_monitor"

done < ./nodes.conf


printf "%d\n%b%b" $nodesCount $ips $macs > nifty_nodes.conf
# For each of the nodes in deployment, update nodes.conf & run nifty with the nodes IP.
while IFS= read -r nodeIP
do
	if [ -z $nodeIP ]; then
		continue;
	fi
  	#ssh into the node and get its IP and MAC.
  	ip=$(ssh -n $sshOptions $nodeIP ip addr show br0 | grep 'inet ' | cut -f2 | awk '{print $2}' | rev | cut -c4- | rev)
  	mac=$(ssh -n $sshOptions $nodeIP cat /sys/class/net/br0/address)
	scp $sshOptions ./nifty_nodes.conf $nodeIP:"${NIFTY_HOME}/nifty_nodes.conf"
	
	echo "Starting NIFTY on node $nodeIP (which has IP address: $ip, and MAC address: $mac)"
	# Could need to either run the script as sudo or add sudo here to be able to deploy rules. (or have OVS not require sudo)
	sudo ssh -n $sshOptions $nodeIP "cd $NIFTY_HOME && ./nifty -t 200 -i $ip -m $mac -c nifty_nodes.conf" &

	#start the heartbeat monitoring
	echo "Starting heartbeat on node $nodeIP"
	ssh -n $sshOptions $nodeIP "cd $NIFTY_HOME && ./heartbeat $ip" &
 
done < ./nodes.conf

echo "Starting heartbeat locally"
chmod +x $HEARTBEAT_EXEC
$HEARTBEAT_EXEC &

echo "Starting Traffic Monitoring locally"
chmod +x $TRAFFIC_MONITOR_EXEC
$TRAFFIC_MONITOR_EXEC $

rm ./nifty_nodes.conf

set +x