#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void swap(char** names, int i, int j);
void display(char** names);

char *names[] = {"Zebra","Goose","Turtle","Penguin","Hippo","Frog","Leapard","Deer",
    "Ape","Seagull","Bee", "Cow", "Horse", "Pig", "Chicken", "Bat", "Parot", "Mantis", 
    "Dove", "Elephant", "Mantis", "Sheep", "Wolf", "Peggasis", "Conda", "Firefly", 
    "Beetle", "Worm", "Spider", "Boxelder", "Ant", "Linx", "Cattapillar", "Ladybug",
    "Aardvark","Albatross","Alligator","Alpaca","Ant","Anteater","Antelope","Ape",
    "Armadillo","Donkey","Baboon","Badger","Barracuda","Bat","Bear","Beaver","Bee",
    "Bison","Boar","Buffalo","Butterfly","Camel","Capybara","Caribou","Cassowary","Cat",
    "Caterpillar","Cattle","Chamois","Cheetah","Chicken","Chimpanzee","Chinchilla",
    "Chough","Clam","Cobra","Cockroach","Cod","Cormorant","Coyote","Crab","Crane",
    "Crocodile","Crow","Curlew","Deer","Dinosaur","Dog","Dogfish","Dolphin","Dotterel",
    "Dove","Dragonfly","Duck","Dugong","Dunlin","Eagle","Echidna","Eel","Eland",
    "Elephant","Elk","Emu","Falcon","Ferret","Finch","Fish","Flamingo","Fly","Fox",
    "Frog","Gaur","Gazelle","Gerbil","Giraffe","Gnat","Gnu","Goat","Goldfinch",
    "Goldfish","Goose","Gorilla","Goshawk","Grasshopper","Grouse","Guanaco","Gull",
    "Hamster","Hare","Hawk","Hedgehog","Heron","Herring","Hippopotamus","Hornet","Horse",
    "Human","Hummingbird","Hyena","Ibex","Ibis","Jackal","Jaguar","Jay","Jellyfish",
    "Kangaroo","Kingfisher","Koala","Kookabura","Kouprey","Kudu","Lapwing","Lark",
    "Lemur","Leopard","Lion","Llama","Lobster","Locust","Loris","Louse","Lyrebird",
    "Magpie","Mallard","Manatee","Mandrill","Mantis","Marten","Meerkat","Mink","Mole",
    "Mongoose","Monkey","Moose","Mosquito","Mouse","Mule","Narwhal","Newt","Nightingale",
    "Octopus","Okapi","Opossum","Oryx","Ostrich","Otter","Owl","Oyster","Panther",
    "Parrot","Partridge","Peafowl","Pelican","Penguin","Pheasant","Pig","Pigeon"};

long size;

int main() {
    srand(time(NULL));
    size = sizeof(names)/sizeof(*names);
    printf("Shuffling\n");
    for (int i=0; i<size ; i++){
        swap(names, i, rand()%size);
    }
    display(names);
    return 0;
}
void swap(char** names, int i, int j) {
    char* temp = names[i];
    names[i] = names[j];
    names[j] = temp;
}
void display(char **names) {
    int result_size = size*3+1;
    char** result = malloc(result_size*sizeof(char*));
    int i;
    int count = 0;

    result[0] = "{";
    for (i=1; i < size+1; i++) {
        result[i*3-2] = "\"";
        result[i*3-1] = names[i-1];
        if (i*3 < result_size)
            result[i*3] = "\", "; 
    }
    result[i*3-3] = "\"}\n";

    for(int i=0; i<result_size; i++)
        printf("%s",result[i]);
    putchar('\n');
}