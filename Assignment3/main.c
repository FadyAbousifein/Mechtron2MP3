#include "vaderSentiment.c"
int main() {
    WordData * data = readFile("vader_lexicon.txt"); 
    // Array of sentences to analyze
    char *sentences[] = {
        "VADER is smart, handsome, and funny.",
        "VADER is smart, handsome, and funny!",
        "VADER is very smart, handsome, and funny.",
        "VADER is VERY SMART, handsome, and FUNNY.",
        "VADER is VERY SMART, handsome, and FUNNY!!!",
        "VADER is VERY SMART, uber handsome, and FRIGGIN FUNNY!!!",
        "VADER is not smart, handsome, nor funny.",
        "At least it isn’t a horrible book.",
        "The plot was good, but the characters are uncompelling and the dialog is not great.",
        "Make sure you :) or :D today!",
        "Not bad at all"
    };

    // Calculate and print compound score for each sentence
    int numSentences = sizeof(sentences) / sizeof(sentences[0]);
    for (int i = 0; i < numSentences; i++) {
        float compound = sentimentScore(data, sentences[i]);
        printf("The sentence is:\n%s\nThe compound score: %f\n\n", sentences[i], compound);
    }

    // Free the lexicon data
    free(data);
}
