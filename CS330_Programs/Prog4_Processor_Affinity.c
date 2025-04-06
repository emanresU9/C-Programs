#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <wait.h>
#include <sys/time.h>
#include <time.h>

//////////////// SYNTAX ////////////////
// The Syntax for executing this program
// is: ./a.out <int> 
// where <int> is the number of processes
// you whish to run. 
///////////// DESCRIPTION ///////////////
// This program tests the use of multiple
// processors by running quickSort & shuffle,
// while using [1-n] processors, where n is the
// total number of processors on the system.
// After each test it prints the elapsed time 
// to run m processes using [1-n] processors.

void quickSort(char *arr[]);
void reQuickSort(char* arr[], int left, int right);
void shuffle(char **arr);
void swap(char* arr[], int i, int j);
int compare_string(char* s1, char* s2);
int parseInt(char * str);
void display(char **arr);
double calc_elapsed(struct timeval stop, struct timeval start);

char *animals[] = {"Sheep", "Ape", "Louse", "Elk", "Kangaroo", "Leapard", "Human", "Chicken", "Mallard", "Kookabura", "Ape", "Mantis", "Dugong", "Newt", "Conda", "Barracuda", "Lapwing", "Penguin", "Narwhal", "Jackal", "Gerbil", "Heron", "Goldfish", "Otter", "Cod", "Chicken", "Cobra", "Koala", "Hornet", "Ferret", "Dove", "Eland", "Cow", "Cormorant", "Caterpillar", "Dolphin", "Chough", "Jellyfish", "Zebra", "Frog", "Hippopotamus", "Cheetah", "Cassowary", "Gorilla", "Buffalo", "Lyrebird", "Crocodile", "Octopus", "Chinchilla", "Caribou", "Eel", "Seagull", "Lark", "Duck", "Alpaca", "Mule", "Pig", "Owl", "Jaguar", "Boar", "Gnat", "Dog", "Echidna", "Marten", "Okapi", "Meerkat", "Dragonfly", "Goat", "Boxelder", "Manatee", "Mosquito", "Mantis", "Lion", "Dotterel", "Beaver", "Mantis", "Baboon", "Antelope", "Turtle", "Armadillo", "Alligator", "Dunlin", "Chamois", "Partridge", "Linx", "Goldfinch", "Cattle", "Wolf", "Finch", "Guanaco", "Camel", "Crow", "Moose", "Kudu", "Goose", "Chimpanzee", "Gaur", "Grasshopper", "Bat", "Ant", "Gnu", "Opossum", "Albatross", "Mandrill", "Ladybug", "Fox", "Mouse", "Elephant", "Hamster", "Giraffe", "Mink", "Dove", "Goose", "Magpie", "Fish", "Ant", "Crane", "Grouse", "Jay", "Firefly", "Bat", "Llama", "Loris", "Aardvark", "Ibis", "Butterfly", "Pelican", "Bee", "Bear", "Dogfish", "Beetle", "Hare", "Lemur", "Worm", "Badger", "Bee", "Cattapillar", "Cat", "Hawk", "Clam", "Capybara", "Donkey", "Pigeon", "Spider", "Emu", "Peafowl", "Gazelle", "Pheasant", "Lobster", "Dinosaur", "Goshawk", "Penguin", "Eagle", "Horse", "Panther", "Peggasis", "Leopard", "Hippo", "Mongoose", "Hedgehog", "Hyena", "Ostrich", "Locust", "Kouprey", "Parrot", "Gull", "Elephant", "Kingfisher", "Hummingbird", "Nightingale", "Flamingo", "Parot", "Deer", "Monkey", "Ibex", "Curlew", "Crab", "Frog", "Horse", "Fly", "Oryx", "Herring", "Cockroach", "Mole", "Pig", "Falcon", "Oyster", "Coyote", "Anteater", "Bison", "Deer"};

const int SIZE = sizeof(animals)/sizeof(char*);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Argument-Exception: Please provide one argument.\n");
        printf("\t Syntax: /a.out <int>\n");
        exit(1);
    }
    int proc_num = sysconf(_SC_NPROCESSORS_ONLN);
    struct timeval start, stop;
    cpu_set_t mask;
    CPU_ZERO(&mask);
    for (int i = 1; i <= proc_num; i++) {
        gettimeofday(&start, NULL);
        for (int j = 0; j < i; j++)
            CPU_SET(i, &mask);
        sched_setaffinity(0, sizeof(cpu_set_t), &mask);
        for (int j = 0; j < parseInt(argv[1]); j++) {
            if (fork() == 0) {
                shuffle(animals);
                quickSort(animals);
                exit(0);
            }
        }
        int status;
        for (int j = 0; j < parseInt(argv[1]); j++) {
            wait(&status);
            if (WIFEXITED(status)) {
                if (WEXITSTATUS(status) == EXIT_FAILURE) {
                    printf("Error occured in test_program\n");
                    printf("Exit code: %d\n", (WEXITSTATUS(status)));
                    exit(2);
                }
            }
        }
        gettimeofday(&stop, NULL);
        double elapsed = calc_elapsed(stop, start);
        printf("Running %d processes using %d proccessors: elapsed time was: %f\n", parseInt(argv[1]), i, elapsed);
    }
}

///////////////// FUNCTIONS //////////////////
void display(char **arr) {
    putchar('{');
    for (int i = 0; i < SIZE; i++) {
        if (i % 9 == 0)
            putchar('\n');
        if (i < SIZE -1)
            printf("\"%s\", ", arr[i]);
    }
    printf("\"%s\"}\n", arr[SIZE-1]);
}
void quickSort(char *arr[]) {
    reQuickSort(arr, 0, SIZE-1);
}
void reQuickSort(char* arr[], int left, int right) {
    if (left >= right)
        return;
    int curr_left = left;
    int curr_right = right;
    char* val_mid = arr[(curr_left + curr_right)/2];
    swap(arr, left, (curr_left + curr_right)/2);
    while(curr_left < curr_right) {
        while (curr_left < curr_right && compare_string(arr[curr_left], val_mid) <= 0)
            curr_left++;
        while (compare_string(arr[curr_right], val_mid) > 0)
            curr_right--;
        if (curr_left < curr_right)
            swap(arr, curr_left, curr_right);
    }
    swap(arr, left, curr_right);
    reQuickSort(arr, left, curr_right-1);
    reQuickSort(arr, curr_right+1, right);
}
void shuffle(char **arr) {
    srand(time(NULL));
    for (int i=0; i < SIZE; i++){
        swap(arr, i, rand()%SIZE);
    }
    return;
}
void swap(char* arr[], int i, int j) {
    char* temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}
int compare_string(char* s1, char* s2) {
    // printf("compare_string(%s, %s)\n", s1, s2);
    if (s1 == NULL || s2 == NULL) {
        printf("Can't compare a NULL string. Sorry bro\n");
        exit(1);
    }
    while(*s1 == *s2){
        if (*s1 == '\0')
            return 0;
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}
int parseInt(char * str) {
    int result = 0;
    int sign = 1;
    int i = 0;
    if (str[i] == '-') {
        sign = -1;
        i++;
    } 
    else if(str[i] == '+')
        i++;
    for (; str[i] != '\0' && str[i] >= '0' && str[i] <= '9'; i++) {
        result = result*10 + str[i] - '0';
    }
    if (str[i] != '\0')
        printf("Error parsing integer");
    result *= sign;
    return result;
}
double calc_elapsed(struct timeval stop, struct timeval start) {
    return start.tv_usec > stop.tv_usec ?
    (double)(stop.tv_sec - start.tv_sec) - 1 + 
    (double)(1000000 +stop.tv_usec -start.tv_usec) /1000000 : 
    (double)(stop.tv_sec - start.tv_sec) + 
    (double)(stop.tv_usec -start.tv_usec) /1000000;
}
