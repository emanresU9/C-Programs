#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include <stdbool.h>
#include <wait.h>
// #include <string.h>
// #include <math.h>

#define MULTITHREADING true //Program runs faster when true
#define THREADS 99          //Each thread runs a test function.
                            // More threads => longer user time
#define VERBOSE true       //Not to be used when program is being timed

void test_function(int thread_i);

// This program scheduels a time-taking 
// test_function to run on a single Core
// or on multiple depending on the boolean
// value of the macro, MULTITRHEADING. 
// Thread count can be set with the macro, 
// THREADS.

//// PSEUDO CODE ////
// 1. BEGIN Prog4_Processor_Affinity.c
// 2. FOR 4 iterations
// 3.   Fork() 
// 4.   IF in child process
// 5.     IF using multiple cores
//          set process' proccessor affinity to run on all CPUs
// 6.     ELSE
//          set process' processor affinity to use only one CPU
//        ENDIF using multiple cores
// 7.     Run a test program
// 8.     exit process
//      ENDIF in child process
//    ENDFOR 4 iterations
// 9. Wait for child processes to complete
// 10. exit process
// 11. DONE

int main() {
    int cpu_count;      // System dependent
    cpu_set_t mask;     // A datatype used by the setAffinity function

    // Initialization of mask
    CPU_ZERO(&mask);    

    //Determin cpu_count if using multithreading
    if (MULTITHREADING)
        cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
    else 
        cpu_count = 1;

    //For loop fork then set processor affinity 
    for (int thread_i = 0; thread_i < THREADS; ++thread_i)
        if (fork() == 0) {
            //Set the mask to use availble processors
            for (int i = 0; i < cpu_count; i++)
                CPU_SET(i, &mask);
            //set the processes processor affinity
            sched_setaffinity(0, sizeof(cpu_set_t), &mask);
            //Run test program. Parameter used by test_program to control its process.
            test_function(thread_i);
            exit(0);
        }
    
    // Wait for processes to finish
    int status;
    for (int i = 0; i < THREADS; i++) {
        wait(&status);
        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == EXIT_FAILURE) {
                printf("Error occured in test_program\n");
                printf("Exit code: %d\n", (WEXITSTATUS(status)));
                exit(2);
            }
        }
    }
    return 0;
}

void quickSortStrings(char *sArray[], int left, int right); //An easy sorting function 
void displayStrArray(char *sArray[], int size, int setNum);

void test_function(int thread_i){
    //names is a set of sortable elements
    char *names[] = 
        {"Oyster", "Gull", "Monkey", "Eagle", "Kangaroo", "Okapi", "Boxelder", "Owl", 
            "Fish", "Goose", "Pheasant", "Jaguar", "Duck", "Donkey", "Cow", "Albatross", "Hare", 
            "Jackal", "Crab", "Falcon", "Bat", "Lemur", "Chicken", "Gerbil", "Narwhal", "Koala", 
            "Bear", "Alpaca", "Linx", "Marten", "Cheetah", "Parot", "Goldfish", "Lobster", 
            "Leapard", "Hyena", "Conda", "Deer", "Boar", "Grasshopper", "Firefly", "Echidna", 
            "Mantis", "Penguin", "Heron", "Butterfly", "Bee", "Curlew", "Turtle", "Loris", 
            "Mongoose", "Chimpanzee", "Emu", "Gazelle", "Pelican", "Manatee", "Dunlin", "Mink", 
            "Mouse", "Goat", "Ibex", "Frog", "Cattapillar", "Dogfish", "Cat", "Eel", "Horse", 
            "Ape", "Goshawk", "Crow", "Dugong", "Jay", "Louse", "Deer", "Aardvark", 
            "Caterpillar", "Chough", "Dog", "Gaur", "Bee", "Lyrebird", "Dragonfly", "Wolf", 
            "Cockroach", "Leopard", "Human", "Gorilla", "Hornet", "Fly", "Horse", "Badger", 
            "Lapwing", "Antelope", "Flamingo", "Kudu", "Pig", "Hummingbird", "Seagull", 
            "Elephant", "Cormorant", "Guanaco", "Anteater", "Crane", "Peafowl", "Hedgehog", 
            "Herring", "Peggasis", "Ape", "Giraffe", "Alligator", "Jellyfish", "Spider", 
            "Sheep", "Opossum", "Ostrich", "Meerkat", "Coyote", "Elephant", "Octopus", "Gnat", 
            "Grouse", "Mosquito", "Moose", "Elk", "Nightingale", "Gnu", "Magpie", "Mole", "Cod",
             "Kookabura", "Newt", "Eland", "Otter", "Goose", "Kouprey", "Mantis", "Pig", 
             "Bison", "Dove", "Ant", "Oryx", "Partridge", "Crocodile", "Llama", "Lion", "Lark", 
             "Locust", "Ibis", "Dinosaur", "Camel", "Mandrill", "Hamster", "Kingfisher", 
             "Mantis", "Caribou", "Parrot", "Panther", "Finch", "Hippopotamus", "Worm", "Clam", 
             "Beetle", "Ant", "Baboon", "Mallard", "Chinchilla", "Ladybug", "Buffalo", 
             "Penguin", "Fox", "Barracuda", "Chamois", "Frog", "Cobra", "Zebra", "Ferret", 
             "Mule", "Cassowary", "Hawk", "Dolphin", "Chicken", "Dove", "Goldfinch", "Capybara", 
             "Beaver", "Armadillo", "Pigeon", "Bat", "Hippo", "Dotterel", "Cattle"};

    //Determin size of set
    int size = sizeof(names)/sizeof(names[0]);

    //Divide up the array of names evenly between the 
    // number of processors.
    int portion = size/sysconf(_SC_NPROCESSORS_ONLN);
    //Determin start point for each portion
    int start = thread_i*portion%size, end;
    // Determin endPoint, but don't go outside the names array
    if ((end = start + portion) >= size) 
        end = size-1;

    //Sort the elements
    quickSortStrings(names, start, end);

    //Dispaly the sorted portion if VERBOSE is set to true
    if (VERBOSE) 
        displayStrArray(names, size, thread_i);
    else 
        getpid();
}
void displayStrArray(char *sArray[], int size, int setNum) {
    for (int i = 0; i < size; i++) 
        printf("set #%d %s\n", setNum, sArray[i]);
}

void swap(char *sArray[], int i, int j);   //used in quicksort 
int comp(char *sArray[], int i, int j);   //comparison method used in quicksort

//Recusive function to sort elements
void quickSortStrings(char *sArray[], int left, int right) {
    if (left < right) {
        int min = left, max = right;
        swap (sArray, left, (left+right)/2);

        while (min < max) {
            while (min < max && comp(sArray,min,left) <= 0) {
                min++;
            }
            while (comp(sArray, max, left) > 0) {
                max--;
            }
        
            if (min < max) {
                swap(sArray, min, max);
            }
        }
        swap(sArray, left, max);
        quickSortStrings(sArray, left, right-1);
        quickSortStrings(sArray, max+1, right);
    }
}
void swap(char *sArray[], int i, int j) {
    char *temp = sArray[i];
    sArray[i] = sArray[j];
    sArray[j] = temp;
}
int comp(char *sArray[], int i, int j) {
    int result;
    int c_i = 0;
    while (sArray[i][c_i] == sArray[j][c_i]) {
        c_i++;
        if (sArray[i][c_i] == '\0')
            return 0;
    }
    result = (int)(sArray[i][c_i]) - (int)(sArray[j][c_i]);
    return result;
}