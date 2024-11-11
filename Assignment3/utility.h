#ifndef UTILITY_H
#define UTILITY_H

// constant values 
#define ARRAY_SIZE 10 // for struct
#define MAX_STRING_LENGTH 100 

#define INTENSIFIER 0.293 // boost values  
#define EXCLAMATION 0.292 
#define CAPS 1.5 
#define NEGATION -0.5

// defining the WordData Struct 
typedef struct {
    char word[MAX_STRING_LENGTH];
    float value1; 
    float value2; 
    int intArray[ARRAY_SIZE]; 
} WordData; 

// function prototypes 
WordData * readFile(char * fileName); 
float sentimentScore(WordData * data, char * setence); 
WordData findData(WordData * data, char * word); 

// positive intesifiers 
#define POS_INT_SIZE 11 
static char * posInt[] = {"absolutely", "completely", "extremely", "really", "so", "totally", "very", "particularly", "exceptionally", "incredibly", "remarkably"}; 

// negative intesifiers
#define NEG_INT_SIZE 9 
static char * negInt[] = {"barely", "hardly", "scarcely", "somewhat", "mildly", "slightly", "partially", "fairly", "pretty much"};

// negations 
#define NEGAT_SIZE 13 
static char * negations[] = {"not", "isn’t", "doesn’t", "wasn’t", "shouldn’t", "won’t", "cannot", "can’t", "nor", "neither", "without", "lack", "missing"}; 
#endif // !UTILITY_H
