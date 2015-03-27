/* main -- entry code for Rhizome, starting the server
 *
 * Copyright (C) 2015 	 
 *
 * This code is part of Rhizome.	
 */
#include "RhiServer.h"

int main(int argc, char** argv) {
	RhiServer* rhi = new RhiServer();
	rhi->serverInitialize();
	rhi->startMediaServer();
	return 0;
}


