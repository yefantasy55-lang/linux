.PHONY:all
all:ServerNetCald ClientNetcal 

ServerNetCald:TcpServer.cc
	g++ -o $@ $^ -std=c++17 -ljsoncpp -lpthread
ClientNetcal:TcpClient.cc
	g++ -o $@ $^ -std=c++17 -ljsoncpp -lpthread


.PHONY:output
output:
	@mkdir output
	@mkdir -p output/bin
	@mkdir -p output/log
	@cp ServerNetCald output/bin
	@cp ClientNetcal output/bin
	@cp install.sh output/
	@cp uninstall.sh output/


.PHONY:clean
clean:
	rm -rf ServerNetCald ClientNetcal output 