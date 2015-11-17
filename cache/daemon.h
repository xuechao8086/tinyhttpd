/*************************************************************************
	> File Name: daemon.h
	> Author: 
	> Mail: 
	> Created Time: Tue 17 Nov 2015 12:15:32 AM PST
 ************************************************************************/

#ifndef _DAEMON_H
#define _DAEMON_H
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int daemonize(int nochdir, int noclose);

#endif
