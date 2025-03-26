# operating-systems
# CMPE382-HW1_AS  

## Description  
This project was created to **practice interprocess communication using pipes** in C. It creates two child processes that count prime numbers in two files, communicating through pipes to determine the winner.  

## How It Works  
1. The **parent process**:  
   - Asks for a number **N**.  
   - Creates two files (`File1` and `File2`) with **N random numbers**.  
   - Uses **pipes** to send **N** to two child processes (**P2** and **P3**).  
   - Receives prime counts from both children through pipes.  
   - Sends the counts back so they can determine the winner.  

2. Each **child process**:  
   - Reads **N** from the parent through a pipe.  
   - Counts prime numbers in its assigned file.  
   - Sends the prime count back to the parent using pipes.  
   - Receives the other child’s count and announces the winner.  

## How to Run  
1. **Compile**:  
   ```sh
   gcc CMPE382-HW1_AS.c -o CMPE382-HW1_AS
