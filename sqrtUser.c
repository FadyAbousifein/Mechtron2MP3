#include "stdio.h"

double sqrtUser(double number, int n);

int main() { 
    
    int n; 
    double number; 

    printf("Enter the number you would like to square root: ");
    scanf("%lf", &number); 
    printf("Enter the number of accurate decimals you would like: ");
    scanf("%d", &n);

    printf("The square root of %lf accurate to %d decimals is: %lf",number,n,sqrtUser(number, n)); 

}
// sqrt(3) x^2 -3 = 0
double sqrtUser(double number, int n) {
    
    double x_n = number;
    double f_x = x_n*x_n - number; 
    double f_prime_x = 2*x_n; 
    double x_n_plus_one = x_n - f_x/f_prime_x;
    double tolerance = 10*10^(-n); 
    
    while(x_n - x_n_plus_one < tolerance || x_n_plus_one - x_n < tolerance) {
        sqrtUser(x_n_plus_one, n); 
    }
    return x_n_plus_one; 
    

}
