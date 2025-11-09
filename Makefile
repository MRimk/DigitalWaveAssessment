test:
	g++ -c QuickHashMap.cpp
	g++ -c task1.cpp -DQUICK_TEST
	g++ -o task1 task1.o QuickHashMap.o -lcurl 

all:
	g++ -c QuickHashMap.cpp
	g++ -c task1.cpp
	g++ -o task1 task1.o QuickHashMap.o -lcurl

clean:
	rm -f task1.o QuickHashMap.o task1