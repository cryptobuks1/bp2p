#include <iostream>
#include "btdevice.h"

#define DEBUG 1

BTDevice::BTDevice()
{

}

int BTDevice::connect2Device(const DeviceDescriptor& dev)
{
	int status = 0;
	status = channel.salloc();
	channel.setAdr(dev.addr);
	status = channel.connect();
	return status;
}

int BTDevice::sendReqWait4Resp(const Message& req, Message& resp)
{
	int status = 0;
	status = channel.writeToServer(req);
	status = channel.readFromServer(resp);
	return status;
}

int BTDevice::initServer()
{
	int status = 0;
	status = channel.salloc();
	status = channel.bind();
	return status;
}

int BTDevice::listen4Req(DeviceDescriptor& client)
{
	int status = 0;
	status = channel.listen();
	status = channel.accept(client);
	return status;
}

int BTDevice::fetchRequestData(Message& req)
{
	int status = 0;
	status = channel.readFromClient(req);
	return status;
}

int BTDevice::sendResponse(const Message& resp)
{
	int status = 0;
	status = channel.writeToClient(resp);
	return status;
}

int BTDevice::endClientComm()
{
	int status = 0;
	status = channel.closeClient();
	return status;
}

int BTDevice::endServerComm()
{
	int status = 0;
	status = channel.closeServer();
	return status;
}

int BTDevice::endComm()
{
	int status = 0;
	status = endClientComm();
    status = endServerComm();
    return status;
}

#if DEBUG == 1
int main ()
{
	string addr = "34:DE:1A:1D:F4:0B";
	DeviceDescriptor dev{addr};

	string data{"Send this message over\n"};
	Message req{data};
	Message resp;

	BTDevice myDev;
	myDev.connect2Device(dev);
	myDev.sendReqWait4Resp(req, resp);
	string strresp{resp.data.begin(), resp.data.end()};
	cout << "Message: " << strresp << endl;
	myDev.endComm();

	return 0;
}
#endif