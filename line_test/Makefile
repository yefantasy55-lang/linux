code:test.c
	gcc -o $@ $^
	ln -s $@ code_s 
	ln $@ code_h

.PHONY:clean
clean:
	rm -rf code code_s code_h
