#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include "utils.h"
#include "torrentFileReq.h"
#include "torrentListReq.h"
#include "chunkReq.h"
#include "rrfactory.h"

// include c libs
extern "C" {
#include "blue.h"
}

#include "node.h"

using namespace std;

const string Peer::commString = "bp2p";
const string Peer::commSeparator = "||";

Peer::Peer(){

}

void Peer::findNearbyDevices(){
	int status = 0;
	devInf* devs = NULL;
	int numDevs;
	vector<Device> devices;

	status = ::findDevices(&devs, &numDevs);

	if (status == 0 && devs && numDevs > 0){
		devices.reserve(numDevs);

		int i;
		for (i = 0; i < numDevs; i++){
			Device dev{devs[i].addr, devs[i].name};
			devices.push_back(dev);
		}

		free(devs);
	}

	nodes.insert( nodes.end(), devices.begin(), devices.end() );
}

void Peer::findLocalDevices(){
	int status = 0;
	devInf* devs = NULL;
	int numDevs;
	vector<Device> devices;

	status = ::findLocalDevices(&devs, &numDevs);

	if (status == 0 && devs && numDevs > 0){
		devices.reserve(numDevs);

		int i;
		for (i = 0; i < numDevs; i++){
			Device dev{devs[i].devId, devs[i].addr, devs[i].name};
			devices.push_back(dev);
		}

		free(devs);
	}

	localDevices = devices;
}

int Peer::connect2Node(Peer::Device& dev){
	int err = 0;
	int sock = -1;

	sock = ::connect2Server(1, dev.getAddr().c_str(), &err);

	if (err > 0){
		cout << "CreateSock2Client Error: " << err << endl;
		return err;
	}

	dev.setSendSock(sock);

	return 0;
}

int Peer::sendReqWait4Resp(const Peer::Device& dev, const string req, string& resp){
	int err = 0;
	int respSize = 0;
	auto bResp = unique_ptr<char[]>{new char[CHUNK]};
	
	err = ::sendReqWait4Resp(dev.getSendSock(), req.c_str(), req.length(), bResp.get(), &respSize);

	if (err > 0){
		cout << "sendReqWait4Resp Error: " << err << endl;
		return err;
	}

	resp = bResp.get();
	return 0;
}

int Peer::initServer(Peer::Device& dev){
	int err = 0;
	int sock = -1;

	sock = ::initServer(&err);

	if (err > 0){
		cout << "initServer Error: " << err << endl;
		return err;
	}
	cout << "initServer Socket: " << sock << endl;

	dev.setRecSock(sock);
	return 0;
}

int Peer::listen4Req(Peer::Device& dev, Peer::Device& client){
	int err = 0;
	int clientSock = 0;
	char clientAddr[ADDR_SIZE];

	clientSock = ::listen4Req(dev.getRecSock(), clientAddr, &err);

	if (err > 0){
		cout << "listen4Req Error: " << err << endl;
		return err;
	}

	Device caller{clientAddr, ""};
	client = caller;
	client.setSendSock(clientSock);

	return 0;
}

int Peer::fetchRequestData(const Peer::Device& dev, string& req){
	int err = 0;
	char* data = NULL;
	int size = 0;

	err = ::fetchRequestData(dev.getSendSock(), &data, &size);

	if (err > 0){
		cout << "fetchRequestData Error: " << err << endl;
		return err;
	}

	req = data;

	if (data) free(data);

	return 0;
}
int Peer::sendResponse(Peer::Device& dev, const string data){
	int err = 0;

	err = ::sendResponse(dev.getSendSock(), data.c_str(), data.size());

	if (err > 0){
		cout << "sendResponse Error: " << err << endl;
		return err;
	}

	return 0;
}

void Peer::endComm(Peer::Device& dev){
	int err = 0;

	if (dev.getSendSock() >= 0)
		err = closeSocket(dev.getSendSock());

	if (err > 0){
		cout << "endComm Send Error: " << err << endl;
	}

	if (dev.getRecSock() >= 0)
		err = closeSocket(dev.getRecSock());

	if (err > 0){
		cout << "endComm Rec Error: " << err << endl;
	}
}

void Peer::processRequest(const vector<char>& req, vector<char>& resp){
	unique_ptr<RRPacket> packet = RRFactory::create(req);

	if (packet){
		packet->processRequest();
		resp = packet->getResp();
	}
}


/*void Peer::parseTorrentList(const string& resp, vector<string>& torrentList){
	Utils::tokenize(resp, commSeparator, torrentList);
}

int Peer::requestTorrentList(Peer::Device& dev){
	int err = 0;
	string req, resp;
	int reqType = 0;// torrentList;  actualy request type
	vector<string> torrentNames;

	req = commString + commSeparator + to_string(reqType);

	err = this->sendReqWait4Resp(nodes[0], req, resp);
	if (err > 0){
		cout << "Client Error: sendReqWait4Resp Failed with " << err << endl;
		return err;
	}

	parseTorrentList(resp, torrentNames);
	dev.addTorrentNames(torrentNames);

	return 0;
}*/

void Peer::createRequest(){
	/*TorrentListReq req1;
	req1.createRequest();
	req1.processRequest();
	req1.processResponse();*/

	/*string torrentName{"torrent"};
	TorrentFileReq req2;
	req2.createRequest(torrentName);
	req2.processRequest();
	req2.processResponse();*/

	string torrentName{"large"};
	for (int i = 0; i < 2; i++){
		ChunkReq req1;
		req1.createRequest(torrentName, i);
		req1.processRequest();
		req1.processResponse(req1.resp, req1.size);
	}
	

	/*string strreq1{req1.req.begin(), req1.req.end()};
	string strreq2{req2.req.begin(), req2.req.end()};
	string strreq3{req3.req.begin(), req3.req.end()};
	cout << strreq1 << endl << strreq2 << endl << strreq3 << endl;*/
}

///////////////////////////////////////////////////////////
// defintions for device class

Peer::Device::Device(){
	this->id = -1;
	this->addr = "";	
	this->name = "";
}

Peer::Device::Device(string addr, string name){
	this->addr = addr;
	this->name = name;
}

Peer::Device::Device(int id, string addr, string name){
	this->id = id;
	this->addr = addr;
	this->name = name;
}

string Peer::Device::getAddr() const{
	return this->addr;
}

string Peer::Device::getName() const{
	return this->name;
}

int Peer::Device::getChannel() const{
	return this->channel;
}

int Peer::Device::getSendSock() const{
	return this->sendSock;
}

int Peer::Device::getRecSock() const{
	return this->recSock;
}

void Peer::Device::setSendSock(int sock){
	this->sendSock = sock;
}

void Peer::Device::setRecSock(int sock){
	this->recSock = sock;
}

void Peer::Device::addTorrentNames(vector<string> torrentNames){
	this->torrentNames = torrentNames;
}

void Peer::Client(){
	int err = 0;

	this->findNearbyDevices();

	if (nodes.size() == 0){
		cout << "Client Error: No Nodes Available " << endl;
		return;
	}

	err = this->connect2Node(nodes[0]);

	if (err > 0){
		cout << "Client Error: connect2Node Failed with " << err << endl;
		return;
	}

	string resp;
	err = this->sendReqWait4Resp(nodes[0], "Send This Data\n", resp);
	if (err > 0){
		cout << "Client Error: sendReqWait4Resp Failed with " << err << endl;
		return;
	}

	cout << "Received " << resp << " from " << nodes[0].getAddr() << endl;

	err = this->sendReqWait4Resp(nodes[0], "New data to be sent\n", resp);
	if (err > 0){
		cout << "Client Error: sendReqWait4Resp Failed with " << err << endl;
		return;
	}

	cout << "Received " << resp << " from " << nodes[0].getAddr() << endl;

	this->endComm(nodes[0]);
}

void Peer::Server(){
	int err = 0;

	this->findLocalDevices();
	if (localDevices.size() == 0){
		cout << "Server Error: No Local Devices Available " << endl;
		return;
	}

	err = this->initServer(localDevices[0]);
	if (err > 0){
		cout << "Server Error: initServer Failed with " << err << endl;
		return;
	}
	cout << "Server Notice: Sock " << localDevices[0].getRecSock() << endl;

	Peer::Device client;
	err = this->listen4Req(localDevices[0], client);
	if (err > 0){
		cout << "Server Error: listen4Req Failed with " << err << endl;
		return;
	}

	string req;
	err = this->fetchRequestData(client, req);
	if (err > 0){
		cout << "Server Error: fetchRequestData Failed with " << err << endl;
		return;
	}

	cout << localDevices[0].getAddr() << " received " << req << " from " << client.getAddr() << " PORT " << client.getSendSock() << endl;

	err = this->sendResponse(client, "Rec Data\n");
	if (err > 0){
		cout << "Server Error: sendResponse Failed with " << err << endl;
		return;
	}

	err = this->fetchRequestData(client, req);
	if (err > 0){
		cout << "Server Error: fetchRequestData Failed with " << err << endl;
		return;
	}

	cout << localDevices[0].getAddr() << " received " << req << " from " << client.getAddr() << " PORT " << client.getSendSock() << endl;


	err = this->sendResponse(client, "new data\n");
	if (err > 0){
		cout << "Server Error: sendResponse Failed with " << err << endl;
		return;
	}

	this->endComm(localDevices[0]);
	this->endComm(client);
}

int main(int argc, char *argv[]){
	/*string torrentName {"large"};
	string file1 {"test/test1"};
	string file2 {"test/test2"};
	string file3 {"test/test3"};
	string file4 {"test/large"};
	vector<string> files{file1, file2, file3, file4};
	Torrent t {torrentName, files};*/


	/*RRPacket::requestType reqType = RRPacket::chunk;
	string req{"bp2p||"};
	req = req + to_string(static_cast<int>(reqType));
	req = req + "||Torrent||0";
	vector<char> vResp = {};
	vector<char> vReq(req.begin(), req.end());
	me.processRequest(vReq, vResp);
	string resp(vResp.begin(), vResp.end());
	cout << "serialized Torrent: " << req << endl;// << resp << endl;*/

	Peer me{};
	me.createRequest();

    return 0;
}