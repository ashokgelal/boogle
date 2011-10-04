/*
 * test-sigterm.c
 *
 *  Created on: Oct 3, 2011
 *      Author: ashokgelal
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

char *buf;
int device;
void handle_sigterm(int sig);
static void write_test();
static void fail_test(char *msg);
static void cleanup();

int main(){
	(void) signal(SIGTERM, handle_sigterm);
	write_test();
	// we should never reach this point
	fail_test("Test fail!\n");
	exit(EXIT_FAILURE);
}

static void write_test(){
	device = open("/dev/booga3", O_WRONLY);
	if(device < 0){
		fail_test("Couldn't open /dev/booga3\n");
	}
	buf = (char *)malloc(sizeof(char)*10);
	write(device, buf, 10);
	fail_test("Test fail!");
}

void handle_sigterm(int sig){
	fprintf(stdout, "Test Success!\n");
	cleanup();
	exit(EXIT_SUCCESS);
}

static void fail_test(char *msg){
	perror(msg);
	cleanup();
	exit(EXIT_FAILURE);
}

static void cleanup(){
	if(buf)
		free(buf);
	close(device);
}
