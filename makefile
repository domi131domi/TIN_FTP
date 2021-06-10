CXX = g++
CXXFLAGS = -lpthread -lstdc++fs -lcrypto -std=c++17

all: server client

server: ServerMain.cpp ServerPIModule.cpp FTPServer.cpp ServerDTPModule.cpp
	$(CXX) FTP.cpp ServerMain.cpp FTPServer.cpp ServerPIModule.cpp ServerDTPModule.cpp $(CXXFLAGS) -o ServerProgram

client: ClientMain.cpp
	$(CXX) FTP.cpp ClientMain.cpp ClientPIModule.cpp ClientDTPModule.cpp $(CXXFLAGS) -o ClientProgram

clean:
	rm ServerProgram
	rm ClientProgram
