/* main -- entry code for Rhizome
 *
 * Copyright (C) 2015 	 
 *
 * This code is part of Rhizome.	
 */
#include "RhiServer.h"

int main(int argc, char** argv) {
	RhiInitManager *mgr = new RhiInitManager();
	mgr->readParamAll();

	RhiServer* rhi = new RhiServer(mgr);
	rhi->startMediaServer();
	return 0;
}


