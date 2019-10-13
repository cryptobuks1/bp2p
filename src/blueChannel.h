#include <vector>
#include <string>
#include "channel.h"

using namespace std;

struct Message
{
	int size = 0;
	vector<char> data;
};


class BlueChannel : public Channel
{
private:
	static const int chunkSize = 32768;
	static constexpr char zaddr[] = "00:00:00:00:00:00";

	Message omsg;  // output
	Message imsg;  // input
	int sock = -1;
	struct sockaddr_rc addr;
	struct sockaddr_rc clientAddr;

public:
	BlueChannel();
	
	void salloc();
	void connect();
	
	void write();
	void read();

	void bind();
	void listen();
	void accept();
};