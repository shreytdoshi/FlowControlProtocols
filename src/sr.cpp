

#include "../include/simulator.h"

#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <exception>


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
char sentData [1500][20];
char recvDataBuffer [1500][20];
float timeoutList[1000];
float reTimeoutList[1000];
int t = 0;
int re = 0;
int timeout;
int recvBase;
int prevSequenceNum;
int retransmittedSequenceNum;
int x;
int z = 0;
struct pkt storedPacket;
struct pkt storedAckPacket;
int preAckSeqNum;
int lastcounter;
std::map<std::string, int> X;
std::map<int, std::string> Y;
std::map<std::string, int> bufferSeq;

int retransmitFlags[1000];
int ret = 0;
int nextRet;
int N;
int maxSeq;
int count;
std::string temp;

void A_output(struct msg message)
{
	{
		struct pkt packet;

		while(i<=1500){
			memcpy(buffer[i], message.data, 20);
			bufferSeq[message.data] = i;
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

			//new
			temp = buffer[nextSequenceNumberA];
			std::cout << "In A output || temp:"<< temp << std::endl;
			X[temp]= lastcounter;
			std::cout << "In A output || X[temp]: "<< X[temp] << std::endl;
			Y[lastcounter] = temp;
			std::cout << "In A output || Y[lastcounter]: "<< Y[lastcounter] << std::endl;
			lastcounter++;
			//

			std::cout << "In A output || Message Sent"<< std::endl;

			std::cout << "In A output || Timer Started"<< std::endl;
			timeoutList[t] = get_sim_time();

			if(t==0){
				starttimer(0,timeout+timeoutList[t]-get_sim_time());
			}

			std::cout << "In A output || Message Sent at: "<<get_sim_time()<< std::endl;

			std::cout << "In A output || Logical Ret Num: "<<nextRet<< std::endl;
			t++;

			memcpy(sentData[nextSequenceNumberA], buffer[nextSequenceNumberA], 20);

			nextSequenceNumberA++;
			//nextRet++;

			std::cout << "In A output || Count: "<< count <<std::endl;
		}
	}
}

 //called from layer 3, when a packet arrives for layer 4
void A_input(struct pkt packet)
{

	std::cout << "In A Input ||||||||||||||||||||||||||||||||||||||||||||||||||||||||"<< std::endl;
	int checksum = 0;
	int sum = 0;
	char payload[20];
	memset(payload, '\0', 20);
	memcpy(payload,packet.payload, 20);
	std::cout << "In A Input || Message:" << payload << "\n";

	for(int i=0; i<20; i++){
		sum += payload[i];
	}

	sum = sum + packet.acknum + packet.seqnum;
	std::cout << "In A_Input || sum: "<< sum <<std::endl;
	checksum =~ sum;
	std::cout << "In A Input || Checksum:"<< checksum << std::endl;
	std::cout << "In A Input || SeqNum:"<< packet.seqnum << std::endl;
	std::cout << "In A Input || AckNum:"<< packet.acknum << std::endl;
	std::cout << "In A Input || Packet Checksum:"<< packet.checksum << std::endl;
	std::cout << "In A_Input || Receiving ACK from B\n";

	//retransmittedSequenceNum = preAckSeqNum;

	if(checksum == packet.checksum){
		stoptimer(0);
		////new
		int nextLogicalSeq = lastcounter-1;

		int seq;
		char ytemp[20];
		int iter = 0;
		strcpy(ytemp, Y[nextLogicalSeq].c_str());
		seq = bufferSeq[ytemp];

		if(timeoutList[packet.acknum] + timeout  > get_sim_time() && timeoutList[packet.acknum+1]!=NULL){
			std::cout << "In A_INPUT || ACK Received at: "<<get_sim_time()<< std::endl;
			//nextRet++;
			std::cout << "In A A_input || startting timer for ytemp: "<< ytemp << std::endl;
			std::cout << "In A A_input || Seq: "<< seq << std::endl;
			float T = timeout + timeoutList[seq] - get_sim_time();
			starttimer(0, T);
		}

		std::cout << "In A Input || Packet Acknum:"<< packet.acknum << std::endl;
		std::cout << "In A Input || SendBasea: "<< sendBaseA << std::endl;
		std::cout << "In A Input || Logical Ret Num: "<<nextRet<< std::endl;

		if(packet.acknum >= sendBaseA && packet.acknum <=sendBaseA+N){
			std::cout << "In A Input || Brackets((((((((((("<< std::endl;
			sendBaseA = packet.acknum+1;
		}
	}
}

 //called when A's timer goes off
void A_timerinterrupt()
{
	//new
	int nextLogicalSeq = lastcounter-1;

	int rseq;
	char yt[20];
	int itr = 0;
	strcpy(yt, Y[nextRet].c_str());
	rseq = bufferSeq[yt];

	std::cout << "In A A_timerinterrupt || Before Logical Ret Num: "<<nextRet<< std::endl;
	retransmittedSequenceNum = rseq;

	std::cout << "In A A_timerinterrupt || Ret Num: "<<retransmittedSequenceNum<< std::endl;
	std::cout << "In A A_timerinterrupt || Logical Ret Num: "<<nextRet<< std::endl;

	struct pkt packet;
	packet.seqnum = retransmittedSequenceNum;
	packet.acknum = retransmittedSequenceNum;
	int checksum = 0;
	int sum = 0;

	for(int i=0; i<20; i++){
		sum += buffer[retransmittedSequenceNum][i];
	}

	sum = sum + packet.seqnum + packet.acknum;
	std::cout << "In A A_timerinterrupt || sum: "<< sum <<std::endl;
	checksum = ~sum;
	std::cout << "In A A_timerinterrupt || Checksum: "<< checksum << std::endl;
	std::cout << "In A A_timerinterrupt || Sequence Number: "<< packet.seqnum << std::endl;
	std::cout << "In A A_timerinterrupt || AckNum: "<< packet.acknum << std::endl;

	packet.checksum = checksum;
	memset(packet.payload, '\0', 20);
	memcpy(packet.payload, buffer[retransmittedSequenceNum], 20);

	std::cout << "In A A_timerinterrupt || Payload:"<< packet.payload << std::endl;

	tolayer3(0, packet);

	//new
	int seq;
	char ytemp[20];
	int iter = 0;
	strcpy(ytemp, Y[nextLogicalSeq].c_str());
	seq = bufferSeq[ytemp];
	std::cout << "In A A_timerinterrupt || ytemp: "<< ytemp << std::endl;
	std::cout << "In A A_timerinterrupt || seq: "<< seq << std::endl;


	std::cout << "In A A_timerinterrupt || nextLogicalSeq: "<< nextLogicalSeq << std::endl;
	float Z = timeout + timeoutList[seq] - get_sim_time();
	std::cout << "In A A_timerinterrupt || Z: "<< Z << std::endl;
	//

	//float T = timeout + timeoutList[packet.acknum+1] - get_sim_time();
	//std::cout << "In A A_timerinterrupt || T: "<< T << std::endl;
	//starttimer(0, T);
	starttimer(0, Z);

	//nextRet = lastcounter;
	//new
	char tempArray[20];
	//strcpy(tempArray, temp.c_str());
	strcpy(tempArray, Y[nextRet].c_str());
	nextRet++;
	std::cout << "In A A_timerinterrupt || temp:"<< tempArray << std::endl;
	X[tempArray] = lastcounter;
	std::cout << "In A A_timerinterrupt || X[temp]: "<< X[tempArray] << std::endl;
	Y[lastcounter] = tempArray;
	std::cout << "In A A_timerinterrupt || Y[lastcounter]: "<< Y[lastcounter] << std::endl;
	lastcounter++;
	//

	timeoutList[retransmittedSequenceNum] = get_sim_time();
	//retransmittedSequenceNum++;
	//starttimer(0,timeout+timeoutList[t]-get_sim_time());

	std::cout << "In A A_timerinterrupt || Message Sent"<< std::endl;

}  

 //the following routine will be called once (only) before any other
 //entity A routines are called. You can use it to do any initialization
void A_init()
{
	sendBaseA = 0;
	nextSequenceNumberA = 0;
	retransmittedSequenceNum = 0;
	memset(buffer, '\0', sizeof(buffer[0][0]) * 1500 * 20);
	memset(sentData, '\0', sizeof(sentData[0][0]) * 1500 * 20);
	N = getwinsize();
	maxSeq = (N-1)/2;
	count = 0;
	nextRet = 0;
	timeout = 30;
	preAckSeqNum = 0;
	memset(retransmitFlags, 0, sizeof(retransmitFlags[0])*1000);
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

	newPacket.seqnum = packet.seqnum;
	newPacket.acknum = packet.seqnum;
	memset(newPacket.payload, '\0', 20);
	memcpy(newPacket.payload, "ack", 20);

	std::cout << "In B || RecvBase: "<< recvBase <<std::endl;
	std::cout << "In B || Before Loop " <<std::endl;


//	if(packet.seqnum >= recvBase && packet.seqnum <=recvBase+N-1){

		if(checksum == packet.checksum){
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

			memcpy(recvDataBuffer[packet.seqnum], packet.payload, 20);

			while(recvDataBuffer[x][0]!= NULL){
				std::cout << "To layer 5: "<< recvDataBuffer[x] <<std::endl;
				tolayer5(1, recvDataBuffer[x]);
				x++;
				std::cout << "In B || x: "<< x <<std::endl;
			}

			//for(int i=0, )

			if(packet.seqnum == 0 || packet.seqnum == prevSequenceNum){
				std::cout << "In B || In order loop, prev: "<< prevSequenceNum <<std::endl;
				recvBase = x;
			}
			std::cout << "In B || prev: "<< prevSequenceNum <<std::endl;
			prevSequenceNum++;

			std::cout << "In B || RecvBase "<< recvBase <<std::endl;
		}
//	}
	else if(packet.seqnum >= recvBase-N && packet.seqnum <= recvBase-1){

		if(checksum == packet.checksum){
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

			if(packet.seqnum == 0 || packet.seqnum == prevSequenceNum){
				recvBase = x;
			}
			prevSequenceNum++;
		}
	}
	else{
		//do nothing
	}
}

// the following routine will be called once (only) before any other
// entity B routines are called. You can use it to do any initialization
void B_init()
{
	x = 0;
	recvBase = 0;
	N = getwinsize();
	prevSequenceNum = 0;
	memset(recvDataBuffer, '\0', sizeof(recvDataBuffer[0][0]) * 1500 * 20);
	ackNumberB = 0;
}


