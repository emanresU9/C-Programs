#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <wait.h>
#include <sys/time.h>
#include <time.h>

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

int main() {
    shuffle(animals);
    display(animals);
    quickSort(animals);
    display(animals);
}
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
