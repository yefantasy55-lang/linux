libmy.so:a.o b.o
	gcc -o $@ $^ -shared

%.o:%.c
	gcc -fPIC -c $<

.PHONY:output
output:
	mkdir -p ./lib64/include ./lib64/lib 
	cp *.h ./lib64/include 
	cp *.so ./lib64/lib 

.PHONY:clean 
clean:
	rm -rf *.o

