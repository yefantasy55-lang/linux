libmy.a:a.o b.o
	ar -rc $@ $^

%.o:%.c
	gcc -c $<

.PHONY:output
output:
	mkdir -p ./include ./lib 
	cp -f *.h ./include 
	cp -f *.a ./lib 
	tar -czf ./yzq.tgz ./

usercode:usercode.c
	gcc -o usercode usercode.c -I ./include -L ./lib -lmy

.PHONY:clean
clean:
	rm -rf *.o libmy.a usercode yzq.tgz 
