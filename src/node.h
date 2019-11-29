#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include "deviceDescriptor.h"
#include "rrpacket.h"
#include "utils.h"
#include "workerThread.h"
#include "torrent.h"

using namespace std;

class Node
{
public:
	inline static const string createTorCmd = "-tc";       // create torrent
	inline static const string listNearbyTorsCmd = "-lnt"; // list nearby torrents
	inline static const string requestTorCmd = "-tr";      // request torrent file command
	inline static const string requestTorDataCmd = "-td";  // request torrent data command
	inline static const string quitCmd = "-q";             // quit

	inline static const string requestChunkAvailCmd = "-rc"; // testing request

	Node();

	static void printcli();

	void findLocalDevs();
	void scanForDevs();

	void createServers();
	void createJobManager();

	void activateWorkerThreads();
	void pauseWorkerThreads();
	void killWorkerThreads();

	bool createTorrent(const string& name, const vector<string>& files);
	int listNearbyTorrents(const vector<string>& addrs);
	int requestTorrentFile(const string& name, const string& addr);
	int requestTorrentData(const string& name);

	int requestTorrentAvail(const string& name, const string& addr);  // testing

	void populateLocalTorrents();

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

	enum chunkStatus
	{
		HAVE,
		NEED,
		REQUESTING
	};
	map<DeviceDescriptor, vector<string>> dev2tor;                  // remote devices torrents
	map<string, vector<DeviceDescriptor>> torName2dev;              // torrents available from remote devices
	map<string, map<int, vector<DeviceDescriptor>>> torName2Avail;  // torrents available chunks
	//unordered_set<Torrent> torrents;
	map<string, Torrent> name2torrent;

	map<DeviceDescriptor, unique_ptr<WorkerThread>> servers;
	unique_ptr<WorkerThread> jobManager = nullptr;
	list<shared_ptr<RRPacket>> jobs;


	// request/response
	void carryOutRequest(RRPacket& req);
	void sendRequestWait4Response(const Message& req, Message& rsp, 
		const DeviceDescriptor& clientDes, const DeviceDescriptor& serverDes);
	void processRequest(const Message& req, Message& rsp);
	void processRequest(RRPacket* req);

	void getTorrentNameList(vector<string>& torrentList);
	void getSerializedTorrent(string& serializedTorrent, const string& name);
	void getTorrentAvailFromTorrent(vector<int>& torrentAvail, const string& name);
	void retrieveChunk(vector<char>& chunk, const string& name, const int& index);

	// server/client init
	unique_ptr<WorkerThread> createServerThread(const DeviceDescriptor& servDev);
	void serverThread(DeviceDescriptor devDes);
	unique_ptr<WorkerThread> createJobManagerThread();
	void jobManagerThread();

	int getMissingChunkIndex(const Torrent& torrent);
	void insertJob(const shared_ptr<RRPacket> job);

	void requestAllNearbyTorrents();
	void requestNearbyTorrents(const vector<DeviceDescriptor>& devs);
	int requestTorrentAvail(const string& name, const DeviceDescriptor& dev, vector<int>& avail);
	int requestTorrentFile(const string& name, const DeviceDescriptor& dev);
	int requestTorrentFileIfMissing(const string& name, Torrent& torrent);
	int requestChunk(const Torrent& torrent);
	int requestChunk(const string& name, int index);
	int requestChunk(const string& name, int index, const DeviceDescriptor& dev);
	shared_ptr<ChunkReq> createChunkRequest(const string& name, int index, 
		const DeviceDescriptor& remote, const DeviceDescriptor& local);

	// server/client control
	void activateServerThreads();
	void activateJobManager();
	void pauseServerThreads();
	void pauseJobManager();
	void killServers();
	void killJobManager();

	void getTorrentList(vector<Torrent>& torrentList);

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