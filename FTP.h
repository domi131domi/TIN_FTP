#include <string>

class FTP{
static const int REPLYSIZE = 1024;

public:
static const int SHA256_LENGTH = 64;

static bool Send(const int socket, std::string message);
static std::string Read(const int socket);
static std::string SHA256(const char* const filepath);
};