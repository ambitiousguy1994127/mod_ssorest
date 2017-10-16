#Makefile

CXXFLAGS = -std=c++11

all: module install
	
module:
	scons

install:
	sudo apxs -i -n ssorest mod_ssorest.so
	
clean:
	scons -c
	
