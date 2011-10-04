/*
 * test-read.c
 *
 *  Created on: Oct 3, 2011
 *      Author: ashokgelal
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

char * devices[] = {"/dev/booga", "/dev/booga0", "/dev/booga1", "/dev/booga3" };
int fail_count;
int total_tests;

static void fail_test(char *msg);
static void print_result();
static void read_test_device(char *device);

int main(){
	fail_count = 0;
	total_tests = 0;
	read_test_device(devices[0]);
	read_test_device(devices[1]);
	read_test_device(devices[2]);

	print_result();
	exit(EXIT_SUCCESS);
}

static void read_test_device(char *device){
	char *buf;
	int src;
	int out;
	int size = 10;
	total_tests++;
	src = open(device, O_RDONLY);
	if(src < 0){
		fail_test("Device failed to open");
		return;
	}
	buf = (char *) malloc(sizeof(char)* (size+1));
	out = read(src, buf, size);
	total_tests++;
	if(out != size){
		fail_test("Buffer wasn't written properly");
	}
	free(buf);
	close(src);
}

static void fail_test(char *msg){
	fail_count++;
	fprintf(stderr, "Error: %s\n", msg);
}

static void print_result(){
	if(fail_count == 0){
		fprintf(stdout, "Total Tests: %d\n", total_tests);
		fprintf(stdout, "All tests passed successfully!\n");
	}
	else{
		fprintf(stderr, "Total Tests: %d\n", total_tests);
		fprintf(stderr, "%d tests failed\n", fail_count);
	}
}
