CC = g++
CFLAGS = $(shell mysql_config --cflags)
LDFLAGS = $(shell mysql_config --libs)
TARGET = C_API_Mysql
SOURCE = C_API_Mysql.cc

all:
	$(CC) $(SOURCE) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean