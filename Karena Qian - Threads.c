/* Karena Qian
 * Threads Assignment
 * CSC 3350
 * May 30, 2023*/

#define _MULTI_THREADED //must be included in any module that uses thread data types/definitions: (IBM, 2021c)

#include <stdio.h>
#include <unistd.h> //for file management: (GeeksforGeeks, 2023)
#include <fcntl.h> //same as above
#include <string.h> //for string manipulation
#include <stdlib.h> //for exit(), rand()
#include <time.h> //for random generator seed: (UPENDRA BARTWAL,, 2022)
#include "my_threads.c" //for POSIX wrapper funcs

void* averageData(void *threadID){
	double start = Time_GetSeconds(); //single-thread timer
	
	//system call to open Data.txt file for read-only
	int data = open("Data.txt", O_RDONLY);
	assert(data != -1);
	
	//create Data_Thread_X.txt: X == A, B, or C, depending on which thread created the file.
	char letter = 65 + *(int*)threadID; //reference: (CSstudent, 2019)
	char filenameBuf[100];
	int filenameSize = sprintf(filenameBuf, "Data_Thread_%c.txt", letter);
	int newFile = open(filenameBuf, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	assert(newFile != -1);
	
      	//copy data from Data.txt to Data_Thread_X.txt
	char curr; //current char being read
	int digit = 0; //a single digit
	double sum = 0; //sum of all 1,000,000 numbers
	int isEnd = read(data, &curr, 1);
	assert(isEnd != -1);
	
	while(isEnd > 0){
		char charBuf[2];
		int charLen = sprintf(charBuf, "%c", curr);
		int charSize = write(newFile, charBuf, charLen);
		if(curr == ','){ //add digit to sum
			sum += (double)digit;
			digit = 0;
		}
		else{ //build digit
			digit = (digit * 10) + (curr - 48);	
		}
		isEnd = read(data, &curr, 1);
		assert(isEnd != -1);
	}

      	//calculate the average of the numbers in the Data.txt
      	//two places past the decimal point
	printf("Average of thread %c is: %.2f\n\n", letter, (sum / 1000000));	

      	//close Data.txt and Data_Thread_X.txt
	assert(close(data) != -1 && close(newFile) != -1);
	
	double end = Time_GetSeconds(); //single-thread timer stop
	
	//compute each threads’ execution time and output in milliseconds (ms)
	double time = (end - start) * 1e3;
	printf("Thread %c Time: %.5f ms\n\n", letter, time);
	
	return NULL;
}


int main(){
	//master (main thread) timer start
	double masterStart = Time_GetSeconds();
	
	//initialize everything
	int numThreads = 3;
	pthread_t thread[numThreads]; //for creating any number of threads (almost)
	int ids[numThreads]; //thread IDs
	//initialize thread IDs
	for(int i = 0; i < numThreads; i++){
		ids[i] = i;
	}
	
	//creates a file with one million random numbers between 0 and 100
      		
	//system call to open Data.txt file for write-only, create if not existant, and empty if exists
	int file = open("Data.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	//O_TRUNC: empties file if exists - (IBM, 2021a)
	//0644: allows user/owner to read/write, groups and others to read, no executing - (CHMOD Calculator, n.d.)
	
	//check for error
	if(file < 0){
		perror("r1");
		exit(1);
	}
	
      	//write 1,000,000 random numbers [0..100] to file
	srand(time(0));
	
	for(int i = 0; i < 1000000; i++){
		//reference: (UPENDRA BARTWAL,, 2022)
		int random = (rand() % (99 - 0 + 1)) + 1;
		
		//reference: (Sales, 2019)
		char buf[100];
		int len = sprintf(buf, "%d,", random);
		int size = write(file, buf, len);
		if(size < 0){
			perror("w1");
			exit(1);
		}     	
	}
	

	//close the file
	int closed = close(file);
	if(closed < 0){
		perror("c1");
		exit(1);
	}
	
	//3 threads total timer start
	double threadsStart = Time_GetSeconds();	

	//create 3 threads (fandyushin, 2016)
	for(int i = 0; i < numThreads; i++){
		Pthread_create(&thread[i], NULL, averageData, &ids[i]); 
	}
	
	//wait for 3 threads (fandyushin, 2016)
	for(int i = 0; i < numThreads; i++){
		Pthread_join(thread[i], NULL);
	}

	//measure and output time that each thread, and main routine, take to run
	
	//3 threads total timer stop
	double threadsEnd = Time_GetSeconds();
      	
        double totalThread = (threadsEnd - threadsStart) * 1e3;
        printf("%d Threads Time: %.5f ms\n", numThreads, totalThread);	

	//master (main) timer stop
	double masterEnd = Time_GetSeconds();
	
	double totalMaster = (masterEnd - masterStart) * 1e3;
	printf("Master Time: %.5f ms\n", totalMaster);

	return 0;
}
