#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <thread>
#include <chrono>

int main() {
    int waitTime; //arg in sleep() for proccess
    int PID; //for forking

    //Allocate an array of char letters A-Z
    char charArr[26];
    for (int i = 0; i < 26; i++) {charArr[i] = (char)((int)'A' + i);}
    
    //Seed srand
    srand(time(0));

    // Create 26 forks with FOR loop. In each loop 
    // put the forked processes to sleep for 
    // between 133 and 333 milliseconds. Then
    // print a char from charArray[].
    for (int i = 0; i < 26; i++) {
        // fork
        PID = fork();
        // IF fork is a child process
        if (PID == 0) {
            //Assign waitTime
            waitTime = rand() % 200 + 133;
            //Sleep for x milliseconds
            std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));
            //Print a char from charArr
            std::cout << charArr[i];
            //EXIT
            exit(0);
        }
        else if (PID < 0) {
            perror("Fork Failed");
            exit(0);
        }
    }

    //Wait FOR all 26 child processes
    // to terminate
    for (int i = 0; i < 26; i++){
        PID = wait(NULL);
    }
    std::cout<<std::endl;

    // RETURN
    return 0;
}
