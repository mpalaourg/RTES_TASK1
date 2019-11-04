#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define NONADAPTIVE 1
#define ADAPTIVE 2
#define INTERRUPT 3
// Global Variables used for @timer_handler //
int iterrations, counter = 0;
double *Timestamps;
double last_time;
struct sigaction sa;
FILE *fp;

void handleNonAdaptive(int Time_Sec_Sleep, float Time_uSec_Sleep);
void handleAdaptive(int Time_Sec_Sleep, float Time_uSec_Sleep);
void handleInterrupt(int Time_Sec_Sleep, float Time_uSec_Sleep);

int main(int argc, char** argv) {
	// Check for right # of input arguments //
	if (argc != 4) {
		printf("\033[31m" "Wrong number of inputs!\n" "\033[0m");
		exit(EXIT_FAILURE);
	}
	// Initialize and process the input arguments //
	int TimeInterval = atoi(argv[2]); float SamplingStep = atof(argv[3]);
	int Time_Sec_Sleep = (int) SamplingStep;
	float Time_uSec_Sleep = SamplingStep - Time_Sec_Sleep;
	iterrations = TimeInterval / SamplingStep;
	double Samples[iterrations];
	// The first argument determines the desired mode //
	switch(atoi( argv[1] )) {
	case NONADAPTIVE:
		printf("Hello from NonAdaptive Sleep\n");
		fp = fopen( "Sleep_Non_Adaptive.csv", "w" ); // Open file for writing
		handleNonAdaptive(Time_Sec_Sleep, Time_uSec_Sleep);
		fclose(fp);
		break;
	case ADAPTIVE:
		printf("Hello from Adaptive Sleep\n");
		fp = fopen( "sleep_Adaptive.csv", "w" ); // Open file for writing
		handleAdaptive(Time_Sec_Sleep, Time_uSec_Sleep);
		fclose(fp);
		break;
	case INTERRUPT:
		printf("Hello from Interrupt\n");
		fp = fopen( "interrupt.csv", "w" ); // Open file for writing
		Timestamps = (double*) malloc(iterrations * sizeof(double) );
		handleInterrupt(Time_Sec_Sleep, Time_uSec_Sleep);
		fclose(fp);
		free(Timestamps);
		break;
	default:
		printf("\033[31m" "Wrong Input!\n" "\033[0m" "Use 1 for NonAdaptive Sleep, 2 for Adaptive Sleep or 3 for interrupts.\n");
		exit(EXIT_FAILURE);
	}

}

void handleNonAdaptive(int Time_Sec_Sleep, float Time_uSec_Sleep){
	// Initialize the variables //
	struct timeval current;
	int count = 0;
    double beginTime, currTime, prevTime = 0;
	double Samples[iterrations];
	gettimeofday(&current, NULL);
    currTime = current.tv_sec * 1000 + current.tv_usec/1000.0;
	beginTime = currTime;
	// For each iterration, sleep for the desired time, wake up and write the response time into a file //
	for (count = 0; count < iterrations; count++) {
		Samples[count] = currTime;
		if (Time_Sec_Sleep) sleep(Time_Sec_Sleep);
		if (Time_uSec_Sleep) usleep(Time_uSec_Sleep * 1000 * 1000);
		gettimeofday(&current, NULL);
		currTime = (current.tv_sec) * 1000 + current.tv_usec/1000.0;
		fprintf(fp, "%f\r\n", currTime - beginTime - prevTime);
		prevTime = currTime - beginTime;
		//if (!(count % 1000)) printf("%d\n",count);
	}
	// After the sampling is done, write the timestamps into a file //
	FILE* timestamps_sleep;
	timestamps_sleep = fopen( "Sleep_Non_Adaptive_Timestamps.csv", "w" ); // Open file for writing
	for (int i = 0; i < iterrations; i++){
		fprintf(timestamps_sleep, "%f\r\n", Samples[i]);
	}
	fclose(timestamps_sleep);	
}

void handleAdaptive(int Time_Sec_Sleep, float Time_uSec_Sleep){
	// Initialize the variables //
	struct timeval current;
	int count = 0;
    double beginTime, currTime, prevTime = 0, adaption = 0;;
	double Samples[iterrations];
	gettimeofday(&current, NULL);
    currTime = current.tv_sec * 1000 + current.tv_usec / 1000.0;
	beginTime = currTime;
	// For each iterration, sleep for the desired time, wake up, write the response time into a file and measure the deviation of the SamplingStep //
	for ( count = 0; count < iterrations; count++) {
		Samples[count] = currTime;
		if (Time_Sec_Sleep) sleep(Time_Sec_Sleep);														// Second sleep {propably 0}
		if (Time_uSec_Sleep - adaption > 0) usleep((Time_uSec_Sleep - adaption) * 1000 * 1000);			// uSecond sleep
		gettimeofday(&current, NULL);
		currTime = (current.tv_sec) * 1000 + current.tv_usec/1000.0;
		fprintf(fp, "%f\r\n", currTime - beginTime - prevTime);
		adaption = ( (currTime - beginTime - prevTime) / 1000.0) - (Time_Sec_Sleep + Time_uSec_Sleep);
		prevTime = currTime - beginTime;
		//if (!(count % 1000)) printf("%d\n",count);
	}
	// After the sampling is done, write the timestamps into a file //
	FILE* timestamps_adaptive;
	timestamps_adaptive = fopen( "Sleep_Adaptive_Timestamps.csv", "w" ); // Open file for writing
	for (int i = 0; i < iterrations; i++){
		fprintf(timestamps_adaptive, "%f\r\n", Samples[i]);
	}
	fclose(timestamps_adaptive);
}

void timer_handler(int signum){       
	// No input arguments can be given, so -almost- all the variables are global // 
    // Count the # of interrupts, when # of iterrations has been reached, write the timestamps into a file and exit the process // 
	if (counter > iterrations - 1){
		sigaction(SIGALRM, &sa, NULL);
		FILE* timestamps_interrupt;
		timestamps_interrupt = fopen( "Interrupt_Timestamps.csv", "w" ); // Open file for writing
		for (int i = 0; i < iterrations; i++){
			fprintf(timestamps_interrupt, "%f\r\n", Timestamps[i]);
		}
		fclose(timestamps_interrupt);
        exit(EXIT_SUCCESS);
    }
	// For each interrupt, Sampling must be continued //
    struct timeval t;
    gettimeofday(&t, NULL);
    double currTime = t.tv_sec * 1000 + t.tv_usec / 1000.0;
	// For each iterration besides the first one, response time must be writen into a file //
	if (last_time != 0){
        fprintf(fp, "%f\r\n", currTime - last_time);
		Timestamps[counter] = currTime;
		counter++;
    }
    last_time = currTime; 
}

void handleInterrupt(int Time_Sec_Sleep, float Time_uSec_Sleep){
	struct itimerval timer;
	// Set timer_handler as the signal handler for SIGALRM //
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &timer_handler;
	sigaction (SIGALRM, &sa, NULL);
	// Initialize itimer to count each time for (Time_Sec_Sleep + Time_uSec_Sleep) //
	timer.it_interval.tv_sec = Time_Sec_Sleep;
	timer.it_interval.tv_usec = Time_uSec_Sleep * 1000 * 1000; 
	timer.it_value = timer.it_interval;
	setitimer (ITIMER_REAL, &timer, NULL);
	// do other job //
	while (1){
		pause();
	}
}