all: client server
client:
	g++ ClientProgram.cpp CommonMutex.cpp -o client -lrt -pthread
server:
	g++ ServerProgram.cpp CommonMutex.cpp -o server -lrt -pthread
clean:
	rm -rf client server Statistics.txt
