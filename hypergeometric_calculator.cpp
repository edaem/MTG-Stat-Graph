#include "hypergeometric.hpp"
#include <cstdlib>
#include <cstdio>

int main(int argc, char* argv[]){
    if(argc != 5){
        printf("usage: hypergeometric PopulationSize SuccessesInPopulation Draws DrawnSuccesses\n");
        exit(1);
    }
    int pop = atoi(argv[1]);
    int pop_successes = atoi(argv[2]);
    int draws = atoi(argv[3]);
    int drawn_successes = atoi(argv[4]);
    // printf("population: %d\nsuccesses in population: %d\ndraws: %d\ndrawn successes: %d\n", pop, pop_successes, draws, drawn_successes);
    // printf("binomial coeffiecent test: %Lf\n", C(pop, draws));
    printf("%.8Lf\n", hd(pop, pop_successes, draws, drawn_successes));

    return 0;
}