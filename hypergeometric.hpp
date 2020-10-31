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
long double C(int n, int k){
     return factorial(n)/ (factorial(k) * factorial(n-k));
 }

/* Calculates the probability (hypergeometric distribution) of k 
successes when drawing n times (without replacement) from a population 
of size N that contains K successes */
long double hd(int N, int K, int n, int k){
    return (C(K, k) * C(N - K, n - k)) / C(N, n);
}