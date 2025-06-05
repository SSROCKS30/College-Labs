# 1. **Write a C program to:**
   1. Create a child process.
   2. The child should execute an interpreter file by passing a few arguments.
   3. Create an interpreter file that has the path of `echoall.c` file and pass one argument.
   4. Create `echoall.c` file which prints the arguments received from both the child process and the interpreter file.

## Note
In the interpreter file, have the path of your current directory using the `pwd` command. So, it should be something like:

"#! /PATH(from pwd command)/echoall my2"
For example: "#! /home/cs6c70/week9/echoall my2"

## Execution Steps

   ```bash
   gcc -o echoall echoall.c
   chmod 777 textinterpreter
   ./inter
   ```

# 2. **Write a C program to perform the following operations:**
   1. To create a child and parent process with the use of an `echoall` file.
   2. The child should execute a process that prints the user-defined values of environment variables.
   3. The parent should execute a process that prints default values for the environment variables.

## Note
Remember to change the path in the first argument of both execle() and execlp(), set path according to the location of your echoall file using pwd command similar to question 1.

## Execution Steps

   ```bash
   gcc -o echoall echoall.c
   gcc -o main main.c
   ./a.out
   ```

# 3. Write a C program to simulate system function.

## Execution Steps

   ```bash
   gcc -o program_name program_name.c
   ./program_name
   ```
