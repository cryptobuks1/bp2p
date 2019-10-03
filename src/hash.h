#include <openssl/sha.h>
#include <vector>
#include <string>
#include <memory>

using namespace std;

///////////////////////////////////////////////////////////
//
//  Hashes a file in chunks of size 32768 bytes into a
//  256 bit hash.
//
//  char* filename:  filename of file to hash
//  char* digest:    function allocated pointer to 2d array
//  int* length:     number of chucks the file splits into
//                    
int computeSha256FileChunks(const char* const path, char*** const digest, unsigned int* const length);


///////////////////////////////////////////////////////////
//
//  Hashes a file into  a 256 bit hash.
//
//  char* filename:  filename of file to hash
//  char digest[65]: function allocated pointer to 2d array
//  
int computeSha256File(const char * const path, char digest[65]);





class Hash
{
protected:
	vector<char> hash;
	string strHash;

public:
	Hash();

	virtual void init() = 0;
	virtual void update(const vector<char>& buff, const int size) = 0;
	virtual vector<char> final() = 0;
	virtual vector<char> computeHash(const vector<char>& buff, const int size) = 0;
};

class Sha256 : public Hash
{
private:
	SHA256_CTX ctx;

public:
	Sha256();

	void init();
	void update(const vector<char>& buff, const int size);
	vector<char> final();
	vector<char> computeHash(const vector<char>& buff, const int size);
};

class FileHasher
{
private:
	vector<char> hash;
	string strHash;

public:
	FileHasher();

	int computeSha256Hash();
};