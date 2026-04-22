.PHONY:all
all:test mykill

mykill:mykill.cc
	g++ -o $@ $^ -std=c++11
test:test.cc
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -rf mykill test