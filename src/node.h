#include <vector>
#include <atomic>
#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <thread>
#include "deviceDescriptor.h"
#include "rrpacket.h"
#include "utils.h"
#include "eventSync.h"

using namespace std;

class Node
{
public:
	inline static const string createTorCmd = "-ct";    // create torrent
	inline static const string listNearbyTorsCmd = "-lnt"; // list nearby torrents
	inline static const string quitCmd = "-q";

	class WorkerThread
	{
	public:
		enum Status
		{
			ACTIVE,
			PAUSE,
			KILL
		};

		template <class Fn, class... Args> WorkerThread(Fn&& fn, Args&&... args);
		WorkerThread(unique_ptr<thread> t, shared_ptr<atomic<Status>> status);
		WorkerThread(unique_ptr<thread> t, shared_ptr<atomic<Status>> status, shared_ptr<SyncEvent> event);

		void activate();
		void pause();
		void kill();
		void close();

	protected:
		unique_ptr<thread> t = nullptr;
		shared_ptr<atomic<Status>> status = nullptr;
		shared_ptr<SyncEvent> event = nullptr;

		void setStatus(Status status);
	};

	Node();

	static void printcli();

	void findLocalDevs();
	void scanForDevs();

	/*void requestTorrentList(const DeviceDescriptor& client, const DeviceDescriptor& server, Message& rsp);
	void requestTorrentFile(const DeviceDescriptor& client, const DeviceDescriptor& server, 
		const string& torrentName, Message& rsp);
	void requestChunk(const DeviceDescriptor& client, const DeviceDescriptor& server, 
		const string& torrentName, const int chunkNum, Message& rsp);*/

	void createServers();
	void createJobManager();

	void activateWorkerThreads();
	void pauseWorkerThreads();
	void killWorkerThreads();

	bool createTorrent(const string& name, const vector<string>& files);
	int listNearbyTorrents(const vector<string>& addrs);

private:
	inline static const string cli = "bp2p> ";

	enum DevStatus
	{
		READY,
		BUSY,
		INACTIVE
	};

	vector<DeviceDescriptor> localDevs;	
	map<DeviceDescriptor, vector<DeviceDescriptor>> local2remote;
	map<DeviceDescriptor, vector<DeviceDescriptor>> remote2local;
	map<DeviceDescriptor, DevStatus> remoteStatus;

	map<DeviceDescriptor, unique_ptr<WorkerThread>> servers;
	unique_ptr<WorkerThread> jobManager = nullptr;
	list<shared_ptr<RRPacket>> jobs;


	// request/response
	void carryOutRequest(RRPacket& req);
	void sendRequestWait4Response(const Message& req, Message& rsp, 
		const DeviceDescriptor& clientDes, const DeviceDescriptor& serverDes);
	void processRequest(const Message& req, Message& rsp);

	// server/client init
	unique_ptr<WorkerThread> createServerThread(const DeviceDescriptor& servDev);
	void serverThread(DeviceDescriptor devDes, 
		shared_ptr<atomic<Node::WorkerThread::Status>> status, shared_ptr<SyncEvent> event);
	unique_ptr<WorkerThread> createJobManagerThread();
	void jobManagerThread(shared_ptr<atomic<WorkerThread::Status>> status, shared_ptr<SyncEvent> event);

	// server/client control
	void activateServerThreads();
	void activateJobManager();
	void pauseServerThreads();
	void pauseJobManager();
	void killServers();
	void killJobManager();

};

class ArgParser
{
public:
	ArgParser()
	{
	}

	ArgParser(const string& cmd, vector<string>& args)
	{
		parse(cmd, args);
	}

	void parse(const string& cmd, vector<string>& args)
	{
		Utils::tokenize(cmd, " ", args);
		args.erase(remove(args.begin(), args.end(), ""), args.end());
		args.erase(remove(args.begin(), args.end(), "\n"), args.end());
		this->cmd  = cmd;
		this->args = args;
	}

	string cmd;
	vector <string> args;
};