#include "stdio.h"

// function declaration 
double sqrtUser(double number, int n);

int main() { 
   
    // declare variables to store user input
    int n; 
    double number; 

    // prompt user to enter the number they wish to square root and the # of digits they want 
    // values will be stored in their respective variables
    printf("Enter the number you would like to square root: ");
    scanf("%lf", &number); 
    printf("Enter the number of accurate decimals you would like: ");
    scanf("%d", &n);
    
    // print result
    printf("The square root of %lf accurate to %d decimals is: %.*lf\n",number,n,n,sqrtUser(number, n)); 

}

// calculates square root using newton's method solving the function x^2-number = 0 
double sqrtUser(double number, int n) {
    
    double x_n = number/2.0; // initial guess
    double x_n_plus_one;  
    double tolerance = 1.0; 

    // calculates degree to tolerance, i.e, if you wanted 3 decimal places your tolerance would be 0.001
    for (int i = 0; i < n; i++) {
        tolerance /= 10.0; 
    }

    // newton's method
    do { 
        double f_x = x_n*x_n - number;  
        double f_prime_x = 2* x_n; // d/dx (x^2-number) = 2*x
        
        x_n_plus_one = x_n - f_x/f_prime_x; // newtons method -> xn+1 = xn - f(xn)/f'(xn)
        
        // checks if we have enough accurate decimals, notice that we check two condictions
        // this is because the tolerance is actually defined as the |Xn-Xn+1| 
        if ((x_n - x_n_plus_one) < tolerance || (x_n_plus_one - x_n) < tolerance) {
            break; // if we have enough accurate decimals, newton's method no longer is needed
        }
        
        x_n = x_n_plus_one; // new iteration becomes our new "initial guess" 

    } while(1); 

    return x_n_plus_one; 
    

}
