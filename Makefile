.DEFAULT_GOAL := all

%:
	@qmake -qt=5
#	g++ -std=c++11 -g -c -o leappd.o leappd.cpp -lasound -ljack -lpthread -Ileap/include/ -Ilibpd/pure-data/src -Ilibpd/libpd_wrapper -Ilibpd/cpp
#	g++ -std=c++11 -g -c -o rtaudio.o rtaudio.cpp -lasoun -ljack -lpthread
	@$(MAKE) -f qMakefile $@
