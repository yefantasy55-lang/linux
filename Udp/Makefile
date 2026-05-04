.PHONY:all
all:udpclient udpserver

udpclient:UdpClient.cc
	g++ -o $@ $^ -std=c++17
udpserver:UdpServer.cc
	g++ -o $@ $^ -std=c++17

.PHONY:clean
clean:
	rm -f udpclient udpserver