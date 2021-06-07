#include "ServerPIModule.h"

int main(int argc, char const *argv[])
{
    ServerPIModule server(8080, "");
	server.Start();
	server.Stop();
    return 0;
}
