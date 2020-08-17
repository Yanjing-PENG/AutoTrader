all: ./src/*.cc
	g++ -g -o ./bin/main ./src/*.cc -I ./src -L ./lib -l thostmduserapi_se -l thosttraderapi_se
clean: 
	rm -f ./bin/main