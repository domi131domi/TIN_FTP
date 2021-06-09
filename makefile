CXX = g++
CXXFLAGS = -lpthread -lstdc++fs

all: server client

server: ServerMain.cpp ServerPIModule.cpp FTPServer.cpp ServerDTPModule.cpp
	$(CXX) ServerMain.cpp FTPServer.cpp ServerPIModule.cpp ServerDTPModule.cpp $(CXXFLAGS) -o ServerProgram

client: ClientMain.cpp
	$(CXX) ClientMain.cpp ClientPIModule.cpp ClientDTPModule.cpp $(CXXFLAGS) -o ClientProgram

clean:
	rm ServerProgram
	rm ClientProgram
