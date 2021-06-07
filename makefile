CXX = g++
CXXFLAGS = -lpthread

all: server client

server: ServerMain.cpp ServerPIModule.cpp
	$(CXX) ServerMain.cpp ServerPIModule.cpp $(CXXFLAGS) -o ServerProgram

client: ClientMain.cpp
	$(CXX) ClientMain.cpp ClientPIModule.cpp $(CXXFLAGS) -o ClientProgram

clean:
	rm ServerProgram
	rm ClientProgram
