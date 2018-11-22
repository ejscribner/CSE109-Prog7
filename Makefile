All: Client

Client: Client.o
	g++ -g -Wall -Werror -lpthread -o Client Client.o

Client.o: Client.cpp Client.h
	g++ -g -Wall -Werror -lpthread -c Client.cpp

clean:
	rm *.o

.PHONY: clean *.o