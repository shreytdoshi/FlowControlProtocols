#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <exception>
#include "../include/simulator.h"

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional data transfer 
   protocols (from A to B). Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
 **********************************************************************/

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */

struct pkt storedPacket;
struct pkt storedAckPacket;
int sequenceNumber;
int packetInTransit = 0;
int ackReceived;
int ackNumber;
int i = 0;
int k = 0;
char prevRecvData[20];
char buffer[1000][20];

void A_output(struct msg message)
{
	try{

		struct pkt packet;
		char data[20];
		memset(data, '\0', 20);

		while(i<1000){
			memcpy(buffer[i], message.data, 20);
			i++;
			break;
		}

		if(ackReceived == 1){

			packet.acknum = ackNumber;
			packet.seqnum = sequenceNumber;
			packet.checksum = 0;

			while(k<1000){
				memcpy(data, buffer[k],20);
				k++;
				break;
			}

			std::cout << "In A Output || Message:" << data << "\n";

			int checksum = 0;
			int sum = 0;
			for(int i=0; i<20; i++){
				sum += data[i];
			}

			sum = sum + packet.seqnum + packet.acknum;

			std::cout << "In A output || sum: "<< sum <<std::endl;

			checksum = ~sum;

			std::cout << "In A output || Checksum: "<< checksum << std::endl;

			std::cout << "In A output || Sequence Number: "<< packet.seqnum << std::endl;

			std::cout << "In A output || AckNum: "<< packet.acknum << std::endl;

			packet.checksum = checksum;
			memset(packet.payload, '\0', 20);
			memcpy(packet.payload, data, 20);

			std::cout << "In A output || Payload:"<< packet.payload << std::endl;

			storedPacket = packet;
			std::cout << "In A output || Message Sent\n";
			tolayer3(0, packet);
			starttimer(0,20);
		}
		ackReceived = 0;
		std::cout<<"Count k: "<<k<<" i:"<<i<<std::endl;

	}
	catch(std::exception& e){
		std::cout << "Exception:" << e.what() << std::endl;
	}
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet)
{
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
	std::cout<<"In A_Input || Receiving ACK from B\n";

	if(checksum == packet.checksum && packet.acknum == 0){
		std::cout << "In A Input 0 || Stopping Timer"<< std::endl;
		stoptimer(0);
		std::cout << "In A Input 0 || Timer Stopped"<< std::endl;
		ackReceived = 1;
		std::cout << "In A Input 0 || Sequence Number Data: "<< sequenceNumber << std::endl;
		if(sequenceNumber == 0){
			std::cout<<"In A_Input 0 || Received ACK 0 from B\n";
			//ackReceived = 1;
			sequenceNumber = 1;
			ackNumber = 1;
		}
		//tolayer5(0, packet.payload);
	}
	else if(checksum == packet.checksum && packet.acknum == 1){
		std::cout << "In A Input 1 || Stopping Timer"<< std::endl;
		stoptimer(0);
		std::cout << "In A Input 1 || Timer Stopped"<< std::endl;
		ackReceived = 1;
		std::cout << "In A Input 1 || Sequence Number Data: "<< sequenceNumber << std::endl;
		if(sequenceNumber == 1){
			//ackReceived = 1;
			std::cout<<"In A_Input || Received ACK 1 from B\n";
			sequenceNumber = 0;
			ackNumber = 0;
		}
		//tolayer5(0, packet.payload);
	}//else do nothing, let the timer expire
}

/* called when A's timer goes off */
void A_timerinterrupt()
{
	tolayer3(0, storedPacket);
	starttimer(0, 20);
}  

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init()
{
	ackReceived = 1;
	sequenceNumber = 0;
	ackNumber = 0;
	memset(buffer, '\0', sizeof(buffer[0][0]) * 50 * 20);

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
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

	if(checksum == packet.checksum && packet.seqnum == 0){
		std::cout<<"In B || Received DATA Seq:0"<< packet.payload<<std::endl;
		if(strncmp(prevRecvData, packet.payload, 20) != 0){
			std::cout<<"In B, Data Received by App Layer0:"<< packet.payload<< std::endl;
			tolayer5(1, packet.payload);
		}
		memcpy(prevRecvData, packet.payload, 20);
		newPacket.seqnum = 0;
		newPacket.acknum = 0;
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
		std::cout<<"Sending ACK0" <<std::endl;
		tolayer3(1, newPacket);
	}
	else if(checksum == packet.checksum && packet.seqnum == 1){
		std::cout<<"In B || Received DATA Seq1:"<< packet.payload<<std::endl;
		if(strncmp(prevRecvData, packet.payload, 20) != 0){
			std::cout<<"In B, Data Received by App Layer1:"<< packet.payload<< std::endl;
			tolayer5(1, packet.payload);
		}
		memcpy(prevRecvData, packet.payload, 20);
		newPacket.seqnum = 1;
		newPacket.acknum = 1;
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
		std::cout<<"Sending ACK1" <<std::endl;
		tolayer3(1, newPacket);
	}
	//else tolayer3(1, storedAckPacket);
}

/* the following rouytine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init()
{
	storedAckPacket.acknum = 0;
}

