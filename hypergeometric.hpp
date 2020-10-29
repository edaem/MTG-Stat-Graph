#include <cstdio>
using namespace std;

/* calculates value of n! */
 long double factorial(int n){
     long double ret = 1;
     if(n < 0)
        return -1;
    
     for(n; n > 0; n--){
        ret *= n;
     }

    return ret; 
 }

/* Calculates the binomial coeffiecient C(n, k) */
long double binom_coeff(int n, int k){
     return factorial(n)/ (factorial(k) * factorial(n-k));
 }

/* Calculates the probability of k successes when drawing n times (without 
replacement) from a population of size N that contains K successes */
long double hypergeometric_distribution(int N, int K, int n, int k){
    return (binom_coeff(K, k) * binom_coeff(N - K, n - k)) / binom_coeff(N, n);
}