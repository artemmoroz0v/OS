files: client server

client: ClientProgram.cpp
	g++ -fsanitize=address ClientProgram.cpp -lzmq -o client -w

server: ServerProgram.cpp
	g++ -fsanitize=address ServerProgram.cpp -lzmq -o server -w

clean:
	rm  -rf client  server