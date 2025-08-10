#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

/* ====================== */
/*  Prime Number Checker  */
/* ====================== */
/**
 * Checks if a number is prime.
 * @param num The number to check
 * @return true if prime, false otherwise
 * 
 * This function uses trial division up to sqrt(num) for efficiency.
 * It first checks small numbers and even numbers, then tests odd divisors.
 */
bool isPrime(long num) {
    if (num <= 1) return false;  // 0 and 1 are not prime
    if (num == 2) return true;   // 2 is the only even prime
    if (num % 2 == 0) return false; // Eliminate even numbers
    
    // Check odd divisors up to sqrt(num)
    for (long i = 3; i * i <= num; i += 2) {
        if (num % i == 0)
            return false;
    }
    return true;
}

/* ====================== */
/*  Greatest Common Divisor (GCD)  */
/* ====================== */
/**
 * Computes GCD using Euclidean algorithm.
 * @param a First number
 * @param b Second number
 * @return GCD of a and b
 * 
 * This is essential for finding a suitable 'e' value that is
 * coprime with phi(n).
 */
long gcd(long a, long b) {
    while (b != 0) {
        long temp = b;
        b = a % b;  // Remainder becomes new b
        a = temp;    // Old b becomes new a
    }
    return a;
}

/* ====================== */
/*  Modular Inverse Calculator  */
/* ====================== */
/**
 * Finds modular inverse using Extended Euclidean Algorithm.
 * @param e The number to find inverse for
 * @param phi Modulus value
 * @return d such that (e*d) % phi = 1
 * 
 * This computes the private key 'd' which is crucial for decryption.
 */
long modInverse(long e, long phi) {
    long m0 = phi;
    long y = 0, x = 1;
    
    if (phi == 1)
        return 0;  // Edge case

    while (e > 1) {
        long q = e / phi;  // Quotient
        long t = phi;
        
        phi = e % phi;  // Remainder
        e = t;
        t = y;
        
        // Update x and y
        y = x - q * y;
        x = t;
    }
    
    // Ensure x is positive
    if (x < 0)
        x += m0;
    
    return x;
}

/* ====================== */
/*  Modular Exponentiation  */
/* ====================== */
/**
 * Efficiently computes (base^exp) % mod using "fast exponentiation".
 * @param base The number to exponentiate
 * @param exp The exponent
 * @param mod The modulus
 * @return Result of the computation
 * 
 * This is used for both encryption and decryption operations.
 * The method reduces computation time from O(n) to O(log n).
 */
long modPow(long base, long exp, long mod) {
    long result = 1;
    base = base % mod;  // Ensure base is within mod range
    
    while (exp > 0) {
        if (exp % 2 == 1)  // If exponent is odd
            result = (result * base) % mod;
        
        base = (base * base) % mod;  // Square the base
        exp = exp / 2;  // Halve the exponent
    }
    return result;
}

/* ====================== */
/*  ASCII Value Printer  */
/* ====================== */
/**
 * Prints ASCII values of characters in a string.
 * @param str The string to analyze
 * 
 * Helper function to show the numeric representation of characters
 * before and after encryption.
 */
void printAsciiValues(const char *str) {
    printf("ASCII values: ");
    for (int i = 0; str[i] != '\0'; i++) {
        printf("%ld ", (long)str[i]);
    }
    printf("\n");
}

/* ====================== */
/*  Main Program  */
/* ====================== */
int main() {
    long p, q, n, phi, e, d;
    char message[100];

    /* ===== User Input ===== */
    printf("Enter a message (max 100 chars): ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0'; // Remove trailing newline

    /* ===== Prime Generation ===== */
    srand(time(NULL)); // Seed random number generator with current time

    // Generate first prime p (100-1000 range)
    do {
        p = rand() % (1000 - 100 + 1) + 100;
    } while (!isPrime(p));

    // Generate second prime q (different from p)
    do {
        q = rand() % (1000 - 100 + 1) + 100;
    } while (!isPrime(q) || q == p);

    /* ===== Key Calculation ===== */
    n = p * q;  // Modulus for both keys
    phi = (p - 1) * (q - 1);  // Euler's totient function

    // Security check: n must be > 255 for ASCII values
    if (n <= 255) {
        printf("Error: n = %ld is too small (must be > 255 for proper encryption).\n", n);
        printf("Generated primes p = %ld, q = %ld are too small.\n", p, q);
        return 1;
    }

    // Find e (public exponent) coprime with phi
    e = 3;  // Start from smallest reasonable prime
    while (e < phi) {
        if (gcd(e, phi) == 1)
            break;
        else
            e++;
    }

    // Compute d (private exponent)
    d = modInverse(e, phi);

    /* ===== Encryption ===== */
    int len = strlen(message);
    long encrypted[100];
    char decrypted[100];

    
    for (int i = 0; i < len; i++) {
        // Encrypt: c = m^e mod n
        encrypted[i] = modPow((long)message[i], e, n);        
    }

    /* ===== Decryption ===== */
    for (int i = 0; i < len; i++) {
        // Decrypt: m = c^d mod n
        decrypted[i] = (char)modPow(encrypted[i], d, n);
    }
    decrypted[len] = '\0';  // Null-terminate decrypted string

    /* ===== Results Output ===== */
    printf("\nGenerated Keys:\n");
    printf("Generated prime numbers:\n");
    printf("p = %ld\n", p);
    printf("q = %ld\n", q);
    printf("n = p * q = %ld\n", n);
    printf("phi = (p-1)*(q-1) = %ld\n", phi);
    printf("Public Key (e, n): (%ld, %ld)\n", e, n);
    printf("Private Key (d, n): (%ld, %ld)\n", d, n);

    printf("\nOriginal Message: %s\n", message);
    printAsciiValues(message);

    printf("\nEncrypted Values: ");
    for (int i = 0; i < len; i++) {
        printf("%ld ", encrypted[i]);
    }
    printf("\n");

    printf("\nDecrypted Message: %s\n", decrypted);
    printAsciiValues(decrypted);

    return 0;
}