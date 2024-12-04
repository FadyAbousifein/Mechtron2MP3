// CODE: include library(s)
#include "utility.h"
#include "OF_lib.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 

// Helper function to generate random numbers in a range
double random_double(double min, double max) {
    return min + (max - min) * ((double)rand() / RAND_MAX);
}

// CODE: implement other functions here if necessary

// Function which will initialize a particle and all its attributes as seen in the psuedo code 
void initializer(Particle * particles, int NUM_PARTICLES, int NUM_VARIABLES, Bound * bounds, ObjectiveFunction objectiveFunction) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        // declare a new particle 
        Particle particle; 
        
        // allocate  memory for a particles attributes defined in the Particle struct
        particle.position = malloc(NUM_VARIABLES * sizeof(double)); 
        particle.velocity = malloc(NUM_VARIABLES * sizeof(double)); 
        particle.bestPosition = malloc(NUM_VARIABLES * sizeof(double)); 

        // initialize values for the particles above attributes 
        for (int j = 0; j < NUM_VARIABLES; j++) {
            particle.position[j] = random_double(bounds[j].lowerBound, bounds[j].upperBound); 
            particle.velocity[j] = 0.0; 
            // TODO: Best position initilization 
        }

        // retrieve particles current position and initialize the best value as the current value
        particle.value = objectiveFunction(NUM_VARIABLES, particle.position);
        particle.bestValue = particle.value; 

        particles[i] = particle; // this particle has now been added to the list of particles
    }
}

// Function which will update a particles attributes throughout the algorithm 
void attributeUpdates(Particle * particles, int NUM_PARTICLES, int NUM_VARIABLES, double * bestPosition, double w, double c1, double c2, ObjectiveFunction objectiveFunction, Bound * bounds) {
    // declare and initialize the global best value 
    double globalBest = objectiveFunction(NUM_VARIABLES, bestPosition); 

    // update particle attributes 
    for (int i = 0; i < NUM_VARIABLES; i++) {
        for (int j = 0; j < NUM_VARIABLES; j++) {
            // velocity update 
            particles[i].velocity[j] = w * particles[i].velocity[j] + c1 * random_double(0, 1) * (particles[i].bestPosition[j] - particles[i].position[j]) + c2 * random_double(0, 1) * (bestPosition[j] - particles[i].position[j]); 
            
            // position update
            particles[i].position[j] += particles[i].velocity[j]; 

            // clamp the particles position within the bounds as defined in the PseudoCode 
            if (particles[i].position[j] < bounds[j].lowerBound) { 
                particles[i].position[j] = bounds[j].lowerBound;
            }
            else if (particles[i].position[j] > bounds[j].upperBound) {
                particles[i].position[j] = bounds[j].upperBound;
            }
        }
        
        // particle value update 
        particles[i].value = objectiveFunction(NUM_VARIABLES, particles[i].position); 

        // best value/position update if needed
        if (particles[i].value < particles[i].bestValue) {
            particles[i].bestValue = particles[i].value; 
            for (int k = 0; k < NUM_VARIABLES; k++) {
                particles[i].bestPosition[k] = particles[i].position[k]; 
            }
        }

        // global best value/position update if needed 
        if (particles[i].value < globalBest) {
            globalBest = particles[i].value; 
            for (int k = 0; k < NUM_VARIABLES; k++) {
                bestPosition[k] = particles[i].position[k]; 
            }
        }

    }

}

// pso implementation 
double pso(ObjectiveFunction objectiveFunction, int NUM_VARIABLES, Bound *bounds, int NUM_PARTICLES, int MAX_ITERATIONS, double *bestPosition) {
    // declare and initialize threshold to double precision
    double threshold = 1.0e-15; 

    // allocate memmory for all particles and initialize them with their attributes
    Particle * particles = malloc(NUM_PARTICLES * sizeof(Particle)); 
    initializer(particles, NUM_PARTICLES, NUM_VARIABLES, bounds, objectiveFunction); 

    // initialize the bestPosition to the first particles position (this is just a placeholder essentially) 
    for (int i = 0; i < NUM_VARIABLES ; i++) {
       bestPosition[i] = particles[0].position[i]; 
    }

    // update attributes of the particles 
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        attributeUpdates(particles, NUM_PARTICLES,  NUM_VARIABLES, bestPosition, 0.7, 1.5, 1.5, objectiveFunction, bounds); 
    }

    // free the allocated memory 
    for (int i = 0; i < NUM_PARTICLES; i++) {
        free(particles[i].position); 
        free(particles[i].velocity); 
        free(particles[i].bestPosition); 
    }
    free(particles); 

    return objectiveFunction(NUM_VARIABLES, bestPosition); 
}
