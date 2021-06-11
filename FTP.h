#include <string>

class FTP{
static const int REPLYSIZE = 1024;

public:
static const int SHA256_LENGTH = 64;

static const int FILE_OPEN_ERR = -1;
static const int FILE_LENGTH_RECV_ERR = -2;
static const int FILE_RECEIVE_ERR = -3;
static const int FILE_LENGTH_READ_ERR = -4;
static const int FILE_SEND_ERR = -5;


static int sendBuffer(int sock_fd, const char* buffer, int buffereSize, int chunkSize = 4 * 1024);
static long sendFile(int sock_fd, const std::string& fileName, int chunkSize = 64 * 1024);
static int receiveBuffer(int sock_fd, char* buffer, int bufferSize, int chunkSize = 4 * 1024);
static long receiveFile(int sock_fd, const std::string& fileName, int chunkSize = 64 * 1024);
static bool Send(const int socket, std::string message);
static std::string Read(const int socket);
static std::string SHA256(const char* const filepath);
static long getFileSize(const std::string& fileName);
};