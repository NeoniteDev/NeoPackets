#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <IPv4Layer.h>
#include <Packet.h>
#include <PcapFileDevice.h>
#include <PcapLiveDevice.h>
#include "util.hpp"
#include <vector>
#include <map>
#include <PayloadLayer.h>

#define BUFLEN 1024
#define PORT 9001

struct sockaddr_in server, si_other;
WSADATA wsa;
SOCKET s;

pcpp::PcapFileReaderDevice reader("E:\\source\\repos\\NeoPackets\\bin\\solo.pcap");
pcpp::RawPacket rawPacket;
pcpp::PayloadLayer* payloadPacket = nullptr;
char* payload = nullptr;
int payloadLen = 0;

void packetLoop()
{

	pcpp::Packet parsedPacket(&rawPacket);
	pcpp::IPv4Address srcIP = parsedPacket.getLayerOfType<pcpp::IPv4Layer>()->getSrcIpAddress();
	if (!srcIP.toString().rfind("192", 0) == 0)
	{
		payloadPacket = parsedPacket.getLayerOfType<pcpp::PayloadLayer>();
	}
	else
	{
		reader.getNextPacket(rawPacket);
	}

	payload = (char*)payloadPacket->getPayload();

	payloadLen = payloadPacket->getPayloadLen();
	
}

void main(int argc, char* argv[])
{

	if (!reader.open())
	{
		printd("[x] Error opening the pcap file\n");
		return;
	}

	//printd("[+] Packets are ready to be casted!.\n");

	int slen = sizeof(si_other);

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printd("[x] Failed. Error Code : ", WSAGetLastError());
		return exit(EXIT_FAILURE);
	}

	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printd("[x] Could not create socket : ", WSAGetLastError());
		return exit(EXIT_FAILURE);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	if (bind(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printd("[x] Bind failed with error code : ", WSAGetLastError());
		return exit(EXIT_FAILURE);
	}

	printd("[=] Server was binded to port ", PORT, "\n");

	//TODO: make a thread.
	//TODO: chop and filter all payloads before casting it.

	char buf[BUFLEN];

	reader.getNextPacket(rawPacket);
	HANDLE packetsThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&packetsThread, NULL, NULL, NULL);
	
	while (true)
	{
		if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen) != SOCKET_ERROR && reader.getNextPacket(rawPacket))
		{
				if (sendto(s, payload, payloadLen, 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
				{
					printd("[x] Error while sending, error code : ", WSAGetLastError());
					return exit(EXIT_FAILURE);
				}
		}
		else
		{
			printd("[x] Error while receiving, error code : ", WSAGetLastError());
			return exit(EXIT_FAILURE);
		}
	}

	closesocket(s);
	WSACleanup();
	reader.close();
}
