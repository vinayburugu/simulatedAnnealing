#this target will compile all the files
parprog: simulatedAnnealing_Vinay.cpp
	g++ -o parprog simulatedAnnealing_Vinay.cpp
#here simulatedAnnealing_Vinay.cpp is the dependency for the target main.o
clean:
	rm -rf parprog
