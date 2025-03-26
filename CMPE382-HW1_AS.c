#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <stdlib.h>
#include <time.h>

//Alara Sermutlu 
//cmpe 382 hw1

int isPrime(int num) {
    if (num < 2) return 0; //no prime smaller than 2
    int i = 2;
    while(i * i <= num){ //if num is divisible by i, it's not a prime
        if(num % i == 0)
        return 0;
        i++;
    }
    return 1;
}


void createFile(const char *filename, int N, int range_max){ //this function generates a file with random integers inside
FILE *file= fopen(filename , "w"); //create file in writing mode
 if (file == NULL) { //if file can't be opened, show error
        perror("Error opening file");
        exit(1);
    }

for(int i=0; i < N; i++){ //generate N random numbers in range 1-range_max
    int random_num = (rand() % range_max) + 1 ; //+1 keeps the number in range
    fprintf(file, "%d\n" , random_num);
}
    fclose(file);
}

int countPrimes(const char *filename) {
    FILE *file = fopen(filename , "r"); //open file in read mode
    if (file == NULL) { //handle errors
        perror("Error opening file");
        exit(1);
    }
    int count = 0, num; //count starts from 0, num loops through numbers inside the file
    while (fscanf(file, "%d", &num) != EOF) {
        if(isPrime(num)) count++; //increase count if number is prime
    }
    fclose(file);
    return count;
}

    
int main() {
    pid_t pid1, pid2;
    int n, primes_p2, primes_p3;

    int pfd1to2[2], pfd2to1[2], pfd1to3[2], pfd3to1[2]; //create 4 pipes
    // Error handling for pipe creation
    if (pipe(pfd1to2) == -1 || pipe(pfd2to1) == -1 || pipe(pfd1to3) == -1 || pipe(pfd3to1) == -1) {
        perror("Pipe failed");
        exit(1);
    }

    srand(time(NULL)); //ensures random numbers change in each iteration
    
    pid1 = fork();  //create the first child p2

    if (pid1 == -1) {  //error handling
        perror("fork");
        return -1;
    } 
    
    if (pid1 == 0) {  // inside process p2 (child 1)
        close(pfd1to2[1]); //close write end of the pipe where p2 receives from p1
        close(pfd2to1[0]); //close read end of the pipe where p2 sends to p1
        close(pfd1to3[0]); //close other unused pipe ends
        close(pfd1to3[1]);
        close(pfd3to1[0]);
        close(pfd3to1[1]);
        
        read(pfd1to2[0], &n ,sizeof(int)); //receive N from parent
        primes_p2 = countPrimes("File1"); //count prime numbers in File1
        write(pfd2to1[1], &primes_p2, sizeof(int)); //send prime count to parent
        close(pfd2to1[1]); //done with the write end of the pipe
        read(pfd1to2[0], & primes_p3 ,sizeof(int)); //read second child's prime count
        close(pfd1to2[0]); //done with the read end of the pipe
        if(primes_p2 > primes_p3) {
            printf("I am the Child process P2: The winner is child process P2\n");
        }
        else printf ("I am the Child process P2: The winner is child process P3\n");
        
        return 0;  //ensure the first child exits properly
    } 
    
    if (pid1 > 0) {  // inside process p1
        pid2 = fork();  // Second fork for the second child p3
        if (pid2 == -1) {  // Error handling
            perror("fork");
            return -1;
        } 
        
        if (pid2 == 0) {  // inside process p3 (second child)
            close(pfd1to3[1]); //close write end of the pipe where p3 receives from p1
            close(pfd3to1[0]); //close read end of the pipe where p3 sends to p1
            close(pfd1to2[0]); //close other unused pipe ends
            close(pfd1to2[1]); 
            close(pfd2to1[0]);
            close(pfd2to1[1]); 
            
            read(pfd1to3[0], &n, sizeof(int)); //read n from parent
            primes_p3 = countPrimes("File2");
            write(pfd3to1[1], &primes_p3, sizeof(int)); //send prime count to parent
            close(pfd3to1[1]);
            read(pfd1to3[0], &primes_p2, sizeof(int)); //read first child's prime count
            close(pfd1to3[0]);
            if(primes_p3 > primes_p2) printf("I am the Child process P3: The winner is child process P3\n");
            else printf ("I am the Child process P3: The winner is child process P2\n");
            
            return 0;  //ensure the second child exits properly
        } 
        
        else {  //inside process p1(parent)
            close(pfd1to2[0]);
            close(pfd2to1[1]);
            close(pfd1to3[0]);
            close(pfd3to1[1]);
            
            printf("Type an integer N: \n");
            scanf("%d", &n);
            
            createFile("File1", n, 754);
            createFile("File2", n, 754);
            
            write(pfd1to2[1], &n, sizeof(int)); //send n to p2
            write(pfd1to3[1], &n, sizeof(int)); //send n to p3
            
            read(pfd2to1[0], &primes_p2, sizeof(int)); //take prime count from p2
            read(pfd3to1[0], &primes_p3, sizeof(int)); //take prime count from p3
        
            close(pfd2to1[0]); //done reading, close pipe
            close(pfd3to1[0]); //done reading, close pipe

            write(pfd1to2[1], &primes_p3, sizeof(int)); //send p3's count to p2
            write(pfd1to3[1], &primes_p2, sizeof(int)); //send p2's count to p3
            
            close(pfd1to2[1]); //done writing n, close pipe
            close(pfd1to3[1]); //done writing n, close pipe
            
            // Wait for both children to finish
            wait(NULL);  
            wait(NULL);

            printf("The number of positive integers in each file: %d\n", n); //print out the results
            printf("The number of prime numbers in File1: %d\n", primes_p2);
            printf("The number of prime numbers in File2: %d\n", primes_p3);
        }
    }
    
    return 0;
}