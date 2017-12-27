


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <exception>
#include "../include/simulator.h"



/*
 ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
*********************************************************************

******** STUDENTS WRITE THE NEXT SEVEN ROUTINES ********

 called from layer 5, passed the data to be sent to other side
*/



int i = 0;
int j = 0;
char buffer [1500][20];
int nextSequenceNumberA = 0;
int ackNumberB = 0;
int sendBaseA = 0;
char prevRecvData[20];
char sentData [1000][20];
struct pkt storedAckPacket;
int startTimeout;
int timeout;

int N;
int maxSeq;
int count;

void A_output(struct msg message)
{
	struct pkt packet;

	while(i<=1000){
		memcpy(buffer[i], message.data, 20);
		i++;
		break;
	}

	std::cout << "In A output || Payload:"<< buffer[nextSequenceNumberA] << std::endl;

	std::cout << "In A output || Window:"<< N << std::endl;
	std::cout << "In A output || Base:"<< sendBaseA << std::endl;
	std::cout << "In A output || nextSeq:"<< nextSequenceNumberA << std::endl;

	if(nextSequenceNumberA < sendBaseA+N){
		std::cout << "In A output || Window:"<< N << std::endl;
		std::cout << "In A output || Base:"<< sendBaseA << std::endl;
		std::cout << "In A output || nextSeq:"<< nextSequenceNumberA << std::endl;
		std::cout << "In A output || Payload:"<< buffer[nextSequenceNumberA] << std::endl;

		packet.seqnum = nextSequenceNumberA;
		packet.acknum = nextSequenceNumberA;
		int checksum = 0;
		int sum = 0;
		for(int i=0; i<20; i++){
			sum += buffer[nextSequenceNumberA][i];
		}

		sum = sum + packet.seqnum + packet.acknum;
		std::cout << "In A output || sum: "<< sum <<std::endl;
		checksum = ~sum;
		std::cout << "In A output || Checksum: "<< checksum << std::endl;
		std::cout << "In A output || Sequence Number: "<< packet.seqnum << std::endl;
		std::cout << "In A output || AckNum: "<< packet.acknum << std::endl;

		packet.checksum = checksum;
		memset(packet.payload, '\0', 20);
		memcpy(packet.payload, buffer[nextSequenceNumberA], 20);

		std::cout << "In A output || Payload:"<< packet.payload << std::endl;

		tolayer3(0, packet);

		std::cout << "In A output || Message Sent"<< std::endl;

		if(sendBaseA == nextSequenceNumberA && startTimeout == 1){
			std::cout << "In A output || Timer Started"<< std::endl;
			starttimer(0, timeout);
			startTimeout = 0;
		}
		memcpy(sentData[nextSequenceNumberA], buffer[nextSequenceNumberA], 20);

		nextSequenceNumberA++;

		std::cout << "In A output || Count: "<< count <<std::endl;
	}
	else {
		//discard packets
	}

}

// called from layer 3, when a packet arrives for layer 4
void A_input(struct pkt packet)
{
	std::cout << "In A_Input || Message: " << packet.payload << "\n";
	std::cout << "In A_Input || Packet Seq: " << packet.seqnum << "\n";
	std::cout << "In A_Input || Packet Ack: " << packet.acknum << "\n";
	int checksum = 0;
	int sum = 0;
	for(int i=0; i<20; i++){
		sum += packet.payload[i];
	}
	sum = sum + packet.seqnum + packet.acknum;
	std::cout << "In A_Input || sum: "<< sum <<std::endl;
	checksum =~ sum;
	std::cout << "In A_Input || Payload checksum: "<< packet.checksum <<std::endl;
	std::cout << "In A_Input || checksum: "<< checksum <<std::endl;

	std::cout << "In A_Input || Base: "<< sendBaseA << std::endl;
	if(checksum == packet.checksum){
		sendBaseA = packet.acknum+1;
		std::cout << "In A_Input || Base: "<< sendBaseA << std::endl;
		std::cout << "In A_Input || nextSeqNum: "<< nextSequenceNumberA << std::endl;
		if(sendBaseA  == nextSequenceNumberA && startTimeout == 1){
			stoptimer(0);
			startTimeout = 1;
		}
		else {
			starttimer(0, timeout);
			startTimeout = 0;
		}
	}
}

// called when A's timer goes off
void A_timerinterrupt()
{
	int x = sendBaseA;
	starttimer(0, timeout);

	while(x<nextSequenceNumberA){

		struct pkt packet;
		packet.seqnum = x;
		packet.acknum = x;
		int checksum = 0;
		int sum = 0;
		for(int i=0; i<20; i++){
			sum += buffer[x][i];
		}

		sum = sum + packet.seqnum + packet.acknum;
		std::cout << "In A_timerinterrupt || sum: "<< sum <<std::endl;
		checksum = ~sum;
		std::cout << "In A_timerinterrupt || Checksum: "<< checksum << std::endl;
		std::cout << "In A_timerinterrupt || Sequence Number: "<< packet.seqnum << std::endl;
		std::cout << "In A_timerinterrupt || AckNum: "<< packet.acknum << std::endl;

		packet.checksum = checksum;
		memset(packet.payload, '\0', 20);
		memcpy(packet.payload, buffer[x], 20);

		std::cout << "In A_timerinterrupt || Retransmitted Payload:"<< packet.payload << std::endl;

		tolayer3(0, packet);
		x++;
	}
}  

 //the following routine will be called once (only) before any other
 //entity A routines are called. You can use it to do any initialization
void A_init()
{
	sendBaseA = 0;
	nextSequenceNumberA = 0;
	memset(buffer, '\0', sizeof(buffer[0][0]) * 1500 * 20);
	memset(sentData, '\0', sizeof(sentData[0][0]) * 1000 * 20);
	N = getwinsize();
	maxSeq = (N-1)/2;
	count = 0;
	startTimeout = 1;
	timeout = 50;
}

 //Note that with simplex transfer from a-to-B, there is no B_output()

 //called from layer 3, when a packet arrives for layer 4 at B
void B_input(struct pkt packet)
{
	struct pkt newPacket;
	std::cout << "In B || Message: " << packet.payload << "\n";
	std::cout << "In B || Packet Seq: " << packet.seqnum << "\n";
	std::cout << "In B || Packet Ack: " << packet.acknum << "\n";
	int checksum = 0;
	int sum = 0;
	for(int i=0; i<20; i++){
		sum += packet.payload[i];
	}
	sum = sum + packet.seqnum + packet.acknum;
	std::cout << "In B || sum: "<< sum <<std::endl;
	checksum =~ sum;
	std::cout << "In B || Payload checksum: "<< packet.checksum <<std::endl;
	std::cout << "In B || checksum: "<< checksum <<std::endl;
	std::cout << "In B || AckNumberB: "<< ackNumberB <<std::endl;

	if(checksum == packet.checksum && ackNumberB == packet.seqnum){
		std::cout<<"In B || Received DATA"<< packet.payload<<std::endl;
		if(strncmp(prevRecvData, packet.payload, 20) != 0){
			std::cout<<"In B, Data Received by App Layer:"<< packet.payload<< std::endl;
			tolayer5(1, packet.payload);
		}
		memcpy(prevRecvData, packet.payload, 20);
		newPacket.seqnum = packet.seqnum;
		newPacket.acknum = packet.seqnum;
		memset(newPacket.payload, '\0', 20);
		memcpy(newPacket.payload, "ack", 20);
		int checksum = 0;
		int sum = 0;
		for(int i=0; i<20; i++){
			sum += newPacket.payload[i];
		}
		sum = sum + newPacket.seqnum + newPacket.acknum;
		checksum = ~sum;
		newPacket.checksum = checksum;
		storedAckPacket = newPacket;
		std::cout<<"Sending ACK" <<std::endl;
		tolayer3(1, newPacket);
		ackNumberB++;
	}
	else {
		std::cout << "In B || ELSE"<< std::endl;
		tolayer3(1, storedAckPacket);
	}

}

 //the following rouytine will be called once (only) before any other
 //entity B routines are called. You can use it to do any initialization
void B_init()
{
	ackNumberB = 0;
}


