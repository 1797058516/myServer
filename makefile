#OBJ = http_conn.o main.o http_conn.h

#server: $(OBJ) 
#	g++ -o server $(OBJ) -lpthread -std=c++11 -g
#http_conn.o: http_conn.cpp locker.h http_conn.h
#	g++ -c http_conn.cpp -g
#main.o: main.cpp locker.h threadpool.h http_conn.h
#	g++ -c main.cpp http_conn.cpp -g
#
server: main.cpp threadpool.h http_conn.cpp http_conn.h locker.h 
	g++ -o server main.cpp threadpool.h http_conn.cpp http_conn.h locker.h -lpthread -g

clean:
	rm  -r server

