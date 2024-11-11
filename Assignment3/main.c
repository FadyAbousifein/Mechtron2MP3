#include "utility.h"

int main() {
    WordData * data = readFile("vader_lexicon.txt"); 
    char * sentence = "VADER is smart, handsome, and funny."; 

    float compound = sentimentScore(data, sentence); 
    printf("The compound score is: %f\n", compound); 
    free(data); 
}
