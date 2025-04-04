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
#define THREADS 100         //Each thread runs a test function.
                             // More threads => longer user time
#define VERBOSE false       //Not to be used when program is being timed

void test_function(int thread_i);

// This program scheduels a time-taking 
// test_function to run on a single Core
// or on multiple depending on the boolean
// value of the macro, MULTITRHEADING. 
// The program uses the same number of threads
// independent of the use of multiple cores.
// Thread number can be set with the macro, 
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
    char *names[] = {"Zebra","Goose","Turtle","Penguin","Hippo","Frog","Leapard","Deer","Ape","Seagull","Bee", "Cow", "Horse", "Pig", "Chicken", "Bat", "Parot", "Mantis", "Dove", "Elephant", "Mantis", "Sheep", "Wolf", "Peggasis", "Conda", "Firefly", "Beetle", "Worm", "Spider", "Boxelder", "Ant", "Linx", "Cattapillar", "Ladybug", "Aardvark","Albatross","Alligator","Alpaca","Ant","Anteater","Antelope","Ape","Armadillo","Donkey","Baboon","Badger","Barracuda","Bat","Bear","Beaver","Bee","Bison","Boar","Buffalo","Butterfly","Camel","Capybara","Caribou","Cassowary","Cat","Caterpillar","Cattle","Chamois","Cheetah","Chicken","Chimpanzee","Chinchilla","Chough","Clam","Cobra","Cockroach","Cod","Cormorant","Coyote","Crab","Crane","Crocodile","Crow","Curlew","Deer","Dinosaur","Dog","Dogfish","Dolphin","Dotterel","Dove","Dragonfly","Duck","Dugong","Dunlin","Eagle","Echidna","Eel","Eland","Elephant","Elk","Emu","Falcon","Ferret","Finch","Fish","Flamingo","Fly","Fox","Frog","Gaur","Gazelle","Gerbil","Giraffe","Gnat","Gnu","Goat","Goldfinch","Goldfish","Goose","Gorilla","Goshawk","Grasshopper","Grouse","Guanaco","Gull","Hamster","Hare","Hawk","Hedgehog","Heron","Herring","Hippopotamus","Hornet","Horse","Human","Hummingbird","Hyena","Ibex","Ibis","Jackal","Jaguar","Jay","Jellyfish","Kangaroo","Kingfisher","Koala","Kookabura","Kouprey","Kudu","Lapwing","Lark","Lemur","Leopard","Lion","Llama","Lobster","Locust","Loris","Louse","Lyrebird","Magpie","Mallard","Manatee","Mandrill","Mantis","Marten","Meerkat","Mink","Mole","Mongoose","Monkey","Moose","Mosquito","Mouse","Mule","Narwhal","Newt","Nightingale","Octopus","Okapi","Opossum","Oryx","Ostrich","Otter","Owl","Oyster","Panther","Parrot","Partridge","Peafowl","Pelican","Penguin","Pheasant","Pig","Pigeon"};

    //Determin size of set
    int size = sizeof(names)/sizeof(names[0]);

    //Select a number of the elements to sort in this process
    // by choosing a start and end index for the sorting array 
    int portion = size/sysconf(_SC_NPROCESSORS_ONLN);
    int start = thread_i*portion%size, end;
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