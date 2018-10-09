all: training

training: training.cc
	g++ -O0 -std=c++14 training.cc -o training
	#g++ -O3 -no-pie -g -std=c++14 training.cc -o training

clean:
	rm training *.csv
