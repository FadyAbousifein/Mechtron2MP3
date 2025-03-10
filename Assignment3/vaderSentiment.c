#include "utility.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

WordData * readFile(char * fileName) {
    FILE * file = fopen(fileName, "r"); // open file in read mode

    // handling potential error while opening file 
    if (file == NULL) {
        printf("Error occured while opening the file\n"); 
        return NULL; 
    }
    
    // allocating enough memory for one word 
    WordData * data = malloc(sizeof(WordData));
    
    // handling potential error while allocating memory
    if (data == NULL) {
        printf("Error occured while allocating memmory\n"); 
        fclose(file); 
        return NULL; 
    }

    // reading data from file 
    int i = 0; 
    while (true) {
        
        // reallocate memort for new item 
        data = realloc(data, (i+1) * sizeof(WordData)); 

        // handling potential error while allocating memory
        if (data  == NULL) {
            printf("Error occured while allocating memmory\n"); 
            free(data); 
            fclose(file); 
            return NULL; 
        }
        
        // read a string and two float values 
        fscanf(file, "%s %f %f", &data[i].word, &data[i].value1, &data[i].value2);
       
        // handle end of file 
        if (feof(file)) {
            data[i].word[0] = '\0'; // assign word as null character 
            break; 
        }
        
        // read the array 
        char line[2000]; 
        fgets(line, 2000, file); 

        char * tok = strtok(line, "[], \n\t\v\f\r"); // tokenize the array for integers 
        
        // store each tokenized integer into the integer array element of our struct 
        for (int j = 0; j < ARRAY_SIZE && tok != NULL; j++) {
            data[i].intArray[j] = atoi(tok); 
            tok = strtok(NULL, "[] \n\t\v\f\r"); // get next token 
        }
       
        // next word 
        i++; 
    }

    // close file and return the data array
    fclose(file); 
    return data; 
}

// find the data of a word 
WordData findData(WordData * data, char * word) {
    // iterate through elements in data 
    for (int i = 0; data[i].word[0] != '\0'; i++) {
        // if the word matches, then return it's data
        if (strcmp(data[i].word, word) == 0) {
            return data[i]; 
        }
    }

    // handling if word is not found in data 
    WordData nullData = {0}; 
    return nullData;
}

float sentimentScore(WordData *data, char *sentence) {
    // initialize variables 
    float scores[MAX_STRING_LENGTH] = {0.0}; // array for scores
    int i =  0; 

    int sentimentalWords = 0; // number of words with sentimental value

    float sumOfSentiment = 0.0; // sum of all the sentiment scores 

    char splitSent[MAX_STRING_LENGTH][MAX_STRING_LENGTH]; // stores the sentence's words individually 
    char copySent[MAX_STRING_LENGTH]; // copy of the sentence 
    
    float intesifier = 0.0; // current intesifer value 
    float negation = 0.0; // current negation value 

    // copy sentence into copySent and tokenize it removing all punctuation without sentiment effecting value 
    strcpy(copySent, sentence); 
    char *tok = strtok(copySent, " \n\t\v\f\r,.?"); 

    // Loop through the tokens
    for (; tok != NULL; i++) {
        bool allCaps = true;    // will store whether a token is all caps or not
        int exclamation = 0;    // will store the number of exclamation marks in a token

        // initialize a character array to store tokens in lowercase and copy token into it
        char unCapsTok[MAX_STRING_LENGTH];
        strcpy(unCapsTok, tok);

        // iterate through characters in unCapsTok
        for (int j = 0; unCapsTok[j] != '\0'; j++) {
            // check if word is all caps if not alter allCaps boolean to false ! must not be present to ensure unecessary intesficaiton does not occur
            if (islower(unCapsTok[j]) && unCapsTok[j] != '!') {
                allCaps = false;
            }

            unCapsTok[j] = tolower(unCapsTok[j]); // unCaps all characters in the token

            // check for exclamation marks if found increment exclamation
            if (unCapsTok[j] == '!') {
                exclamation++;
                unCapsTok[j] = '\0'; // add a null character in place

                // max exclamation mark significance is capped at 3
                if (exclamation > 3) {
                    exclamation = 3;
                }
            }
        }

        // copy token after being processed into splitSent
        strcpy(splitSent[i], unCapsTok);

        WordData wordData = findData(data, unCapsTok); // attempt to find data of the word

        // check if word is found in the data, if so then it has sentiment and thus contributes to the score
        if (wordData.word[0] != '\0') {
            sentimentalWords++;
            scores[i] = wordData.value1;

            // if the word is all capital, its score will be multiplied by the CAPS factor
            if (allCaps) {
                scores[i] *= CAPS;
            }

            // intensifier effect on score
            if (intesifier != 0) {
                scores[i] += intesifier * scores[i];
            }

            // negation effect on score
            if (negation != 0) {
                scores[i] *= negation;
            }

            // exclamation will amplify a positive score, and reduce an already negative score, capped at 3
            if (scores[i] > 0) {
                scores[i] += exclamation * EXCLAMATION;
            } else {
                scores[i] -= exclamation * EXCLAMATION;
            }

            // current sum of sentiment scores
            sumOfSentiment += scores[i];
        }

        // reset intensifier
        intesifier = 0;

        // check for positive intensifiers
        for (int j = 0; j < POS_INT_SIZE; j++) {
            if (strcmp(splitSent[i], posInt[j]) == 0) {
                intesifier = INTENSIFIER;
            }
        }

        // check for negative intensifiers
        for (int j = 0; j < NEG_INT_SIZE; j++) {
            if (strcmp(splitSent[i], negInt[j]) == 0) {
                intesifier = -INTENSIFIER;
            }
        }

        // handle capitalized intensifier
        if (allCaps && intesifier != 0) {
            intesifier *= CAPS;
        }

        // check for negations
        for (int j = 0; j < NEGAT_SIZE; j++) {
            if (strcmp(splitSent[i], negations[j]) == 0) {
                negation = NEGATION;
            }
        }

        // handle capitalized negation
        if (allCaps && negation != 0) {
            negation *= CAPS;
        }

        // move to the next word in the sentence
        tok = strtok(NULL, " \n\t\v\f\r,.?");
    }

    // print words and their scores 
    // for (int j = 0; j < i; j++) {
        // printf("Word: %s Score: %f\n", splitSent[j], scores[j]);
        // tok = strtok(NULL, " \n\t\v\f\r,.?"); 
    // }

    float compound = sumOfSentiment / sqrt(pow(sumOfSentiment, 2) + 15); 
    return compound;
}
