#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <iostream>

#include "blueChannel.h"

#define DEBUG 1

constexpr char BTChannel::zaddr[];

BTChannel::BTChannel()
{
	omsg.data.resize(this->chunkSize, 0);
	imsg.data.resize(this->chunkSize, 0);
	this->addr.rc_family = AF_BLUETOOTH;
    this->addr.rc_channel = 0;
    str2ba( &this->zaddr[0], &this->addr.rc_bdaddr );
    this->clientAddr.rc_family = AF_BLUETOOTH;
    this->clientAddr.rc_channel = 0;
    str2ba( &this->zaddr[0], &this->clientAddr.rc_bdaddr );
}


void BTChannel::salloc()
{
    this->sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (this->sock == -1)
        cout << "Channel Error: Cannot allocate socket. " << errno << endl;
}

void BTChannel::connect()
{
	int status = ::connect(this->sock, reinterpret_cast<struct sockaddr*>(&this->addr), sizeof(this->addr));
    if (status == -1)
        cout << "Channel Error: Cannot connect to socket. " << errno << endl;
}

void BTChannel::write()
{
	int status = ::write(this->sock, this->omsg.data.data(), this->omsg.size);
    if( status == -1 )
        cout << "sendRequest Error: Write error. " << errno << endl;
}

void BTChannel::read()
{
	int bytesRead = ::read(this->sock, this->imsg.data.data(), this->chunkSize);
    if( bytesRead == -1 )
        cout << "sendRequest Error: Failed to read message. " << errno << endl;
    this->imsg.size = bytesRead;
}

void BTChannel::bind()
{
	int status = ::bind(this->sock, reinterpret_cast<struct sockaddr*>(&this->addr), sizeof(this->addr));
    if (status == -1)
        cout << "createServer Error: Cannot bind name to socket. " << errno << endl;
}

void BTChannel::listen()
{
	int status = ::listen(this->sock, 1);
    if (status == -1)
        cout << "Listen Error: Cannot listen for connections on socket. " << errno << endl;
}

void BTChannel::accept()
{
	socklen_t size = sizeof(this->clientAddr);
	int client = ::accept(this->sock, reinterpret_cast<struct sockaddr*>(&this->clientAddr), &size);
    if (client == -1)
        cout << "Listen Error: Failed to accept message. " << errno << endl;
}


#if DEBUG == 1
int main()
{
	return 0;
}
#endif
