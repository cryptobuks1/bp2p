

using namespace std;

class Peer{
	private:
		class Device {
		private:
			int channel = -1;
			int sendSock = -1;
			int recSock = -1;
			int id = -1;
			string addr;
			string name;
			vector<string> torrentNames;

		public:
			Device ();
			Device (string addr, string name); // this will be used for local devices
			Device (int id, string addr, string name); // this will be used for remote devices

			string getAddr() const;
			string getName() const;
			int getChannel() const;
			int getSendSock() const;
			int getRecSock() const;

			void setSendSock(int sock);
			void setRecSock(int sock);

			void addTorrentNames(vector<string> torrentNames);



		};

	private:
		static const string commString;
		static const string commSeparator;

		vector<Device> nodes;
		vector<Device> localDevices;

	public:
		Peer();

		void findNearbyDevices();
		void findLocalDevices();

		int connect2Node(Peer::Device& dev);
		int sendReqWait4Resp(const Peer::Device& dev, const string req, string& resp);
		int initServer(Peer::Device& dev);
		int listen4Req(Peer::Device& dev, Peer::Device& client);
		int fetchRequestData(const Peer::Device& dev, string& req);
		int sendResponse(Peer::Device& dev, string data);
		void endComm(Peer::Device& dev);

		void processRequest(const vector<char>& req, vector<char>& resp);
		void createRequest();

		/*void parseTorrentList(const string& resp, vector<string>& torrentList);
		int requestTorrentList(Peer::Device& dev);*/
		
		static const string applicationDir;
		string getApplicationPath();


		// testing functions
		void Client();
		void Server();

};

