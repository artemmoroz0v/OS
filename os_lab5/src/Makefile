files: main1 main2

main1: libd1.so compilation.cpp
	g++ compilation.cpp -L. -ld1 -o main1 -Wl,-rpath -Wl,.

main2: libd1.so libd2.so launch.cpp
	g++ launch.cpp -L. -ldl -o main2 -Wl,-rpath -Wl,.

libd1.so: d1.o 
	g++ -shared d1.o -o libd1.so 

libd2.so: d2.o 
	g++ -shared d2.o -o libd2.so 

d1.o: first.cpp
	g++ -fPIC -c first.cpp -o d1.o

d2.o: second.cpp
	g++ -fPIC -c second.cpp -o d2.o

clean:
	rm -r *.so *.o main1 main2