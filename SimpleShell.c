//
//  SimpleShell.c
//  
//
//  Created by Hardik Patel on 2/10/14.
//
// Credit to sources used for this project:
// Linked list code reused from: http://www.thegeekstuff.com/2012/08/c-linked-list-example/
// Some shellInput code reused from: http://nsl.cs.sfu.ca/teaching/11/300/prj2_shell.html

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINE 80 //Maximum number of characters that can be entered on commandline

/*
    Represents a node in the list
 */
struct test_struct
{
    char *str;
    struct test_struct *next;
};

struct test_struct *head = NULL;
struct test_struct *curr = NULL;

/*
    Creates the linked list and initializes the first node
 */
struct test_struct* create_list(char * str)
{
    struct test_struct *ptr = (struct test_struct*)malloc(sizeof(struct test_struct));
    if(NULL == ptr)
    {
        return NULL;
    }
    ptr->str = str;
    ptr->next = NULL;
    
    head = curr = ptr;
    return ptr;
}

/*
    Adds a node into the linked list. In this program, the boolean value will be false so that the commands are entered to the beginning of the list. In this way, recent history can be printed easier than if the commands were entered at the end of the list .
 */
struct test_struct* add_to_list(char * str, bool add_to_end)
{
    // Creates the list if there is nothing in the list yet
    if(NULL == head)
    {
        return (create_list(str));
    }
    
    // Allocates memory for a new node
    struct test_struct *ptr = (struct test_struct*)malloc(sizeof(struct test_struct));
    
    if(NULL == ptr)
    {
        return NULL;
    }
    
    ptr->str = str;
    ptr->next = NULL;
    
    // Adds to end of list
    if(add_to_end)
    {
        curr->next = ptr;
        curr = ptr;
    }
    // Adds to beggining of list
    else
    {
        ptr->next = head;
        head = ptr;
    }
    
    return ptr;
}

/*
    Counts the number of nodes in the list
 */
int count_list(void)
{
    struct test_struct *ptr = head;
    int i = 0;
    while(ptr != NULL)
    {
        i++;
        ptr = ptr->next;
    }
    
    return i;
}

/*
    Prints a history of the 10 most recent commands entered
 */
void print_list(void)
{
    int i = 1;
    int listCount = 0;
    struct test_struct *ptr = head;
    
    listCount = count_list();
    printf("\n -------END OF HISTORY------- \n");
    
    // Loops 10 times to print the 10 most recent commands
    while(ptr != NULL && i < 11)
    {
        printf("\n %i: %s", listCount, ptr->str);
        ptr = ptr->next;
        i++;
        listCount--; //Decrements so that the correct number for each command is shown
    }
    printf("\n -------BEGINGING OF HISTORY------- \n");
    
    return;
}

/*
    Returns the most recent command entered.
 */
char* get_recent_command(void)
{
    int listCount = 0;
    int i = 0;
    listCount = count_list();
    
    // Makes sure there is already a command in the list and then gets the most recent command
    if(listCount > 1)
    {
        struct test_struct *ptr = head;
        while(ptr != NULL && i < 1)
        {
            i++;
            ptr= ptr->next;
        }
        return ptr->str;
    }
    // If there are no commands entered yet, returns NULL
    else
    {
        return NULL;
    }
}

/*
    Obtains the nth command from the list.
 */
char* get_nth_command(int n, struct test_struct **prev)
{
    int listCount = 0;
    int tempCount = 0;
    listCount = count_list();
    tempCount = count_list();
    
    // If the number of nodes in the list is larger than n, then the nth command exists
    if(listCount >= n)
    {
        struct test_struct *ptr = head;
        struct test_struct *tmp = NULL;
        bool found = false;
    
        //Decrements the tempCount variable until it equals n
        while(tempCount != n)
        {
            ptr = ptr->next;
            tempCount--;
        }
        
        // Now the ptr is pointing at the nth command
        return ptr->str;
    }
    // If the number of items in the list is less than n, then the nth command does not exist
    else
    {
        return NULL;
    }
}

/*
    Initiates reading commands from the terminal and stores the individual arguments into a character array.
 */
void shellInput(char userInput[], char *args[], int *ampersand);

/*
    Main function that will fork a child process and execute the command specified by the user.
 */
int main(void)
{
    char *str; //Temporary char array
    char *str2; //AnotherTemporary char array
    char *token; //The result of tokenizing the command
    
    char *args[MAX_LINE/2 + 1]; // Holds individual arguments
    int shouldRun = 1; // While loop continues to run if this equals 1
    char userInput[MAX_LINE]; //Stores each character from user input
    int ampersand; // Represents ampersand from commandline
    pid_t pid; // Represent the pid number of the child process
    int n; // Represents the number entered by the user for the nth command
    
    printf("\nMessage: In this custom shell, user must enter '! n' and not'!n' (without quotes) where n is a number representing nth command\n");
    printf("\nLegend:\n\n 'history' -prints history of 10 most recent commands entered\n '!!' -Executes the most recent command entered.\n '! n' Executes the nth command in history.\n");
    
    while(shouldRun)
    {
        //If ampersand equals 1 at any point, the user entered an ampersand, so the parent will run concurrently
        ampersand = 0;
        printf("\nHardik's Shell>");
        fflush(stdout);
        
        // Obtins user input from command line
        shellInput(userInput, args, &ampersand);
        
        //Forks a child process
        pid = fork();
        
        //The child process will run the following since the child's pid is zero
        if(pid == 0)
        {
            //If the second argument is a number, converts number into an integer
            if (args[1])
            {
                n = atoi(args[1]);
            }
            
            //Prints history if user enters "history" command
            if(!(strncmp(args[0], "history", MAX_LINE)))
            {
                
                print_list();
            }
            
            //Executes the most recent command if user enters "!!" command
            else if(!(strncmp(args[0], "!!", MAX_LINE)))
            {
                //Gets the most recent command
                str = get_recent_command();
                
                // Tokenizes the most recent command
                token = strtok(str, "\n, " "");
                
                //Tells the user what command  will be run
                if(str != NULL)
                {
                    printf("\nRunning the '%s' command...\n\n", token);
                    fflush(stdout);
                }
                
                //Prints if no commands have been entered yet
                else
                {
                    printf("\nNo commands in history yet\n");
                    fflush(stdout);
                }
                
                //If result of execvp is negative, then execution will fail. For now, only executes the first argument of the most recent command instead of the entire command.
                if(execvp(&token[0], &token) < 0)
                {
                    printf("\nSorry. The command failed to execute\n");
                    exit(1);
                }
                
                //Executes function successfully
                else
                {
                    execvp(&token[0], &token);
                }
            }
            
            //Executes the nth command if user enters "! n" command, where n is a number
            else if(!(strncmp(args[0], "!", MAX_LINE)))
            {
                //Similar code to the above else-if code block
                str = get_nth_command(n,NULL);
                token = strtok(str, "\n, " "");
                if(str != NULL)
                {
                    printf("\nRunning the '%s' command...\n\n", str);
                    fflush(stdout);
                }
                else
                {
                    printf("\nNo such command in history\n");
                    fflush(stdout);
                }
            
                if(execvp(&token[0], &token) < 0)
                {
                    printf("\nSorry. The command failed to execute\n\n");
                    exit(1);
                }
                else
                {
                    execvp(&token[0], &token);
                }
            }
            
            //Executes a regular command from the commandline
            else
            {
                execvp(args[0], args);
                
            }
            
            exit(0);
        }
        
        //Parent process. If user entered an ampersand, then parent will execute concurrently with child. If not ampersand value will stay as zero and parent will wait for child to finish executing
        else if(ampersand == 0)
        {
            wait(&pid);
            
        }
        
        //User entered an ampersand following the command so parent will run concurrently with child and then exit out of this custom shell and into main terminal shell
        else if(ampersand == 1)
        {
            exit(0);
        }
        
    }
    exit(0);
    return 0;
}

/*
    Before the command is added into the list, it must go through a copying and concatenation process so that the final command added fully represents everything that the user entered. Then, the command that the user entered will be separated into individual arguments and stored in the args array.
 */

void shellInput(char userInput[], char *args[], int *ampersand)
{
    int commLength; // Length of command
    int i; // Index for loop before switch statement
    int inputIndex = -5; // Represents the first index of each command parameter
    int ParamCount = 0; // Number of parameters on command line (number of arguments)
    char str [MAX_LINE]; // Temporary char array that will be concatenated later
    char str2 [MAX_LINE]; // Another temporary char array
    
    // Another temporary char array. WARNING: Do not move where this line of code is (line 284). For an unknown reason, displaying, "Running the nth command" will not display properly if this line is moved to another spot.
    char testing [MAX_LINE]; 
    char *command; // Represents the final, complete command after extraction and concatenation is completed
    
    // Null terminates the first index. Must be done so that output is properly displayed.
    testing [0] = '\0';
    
    // Makes a copy and null terminates the copied char array. Again, must be done so that content is properly displayed
    strncpy(str2, &testing[0], 1);
    
    //Reads user input from command line
    commLength = read(0, userInput, MAX_LINE);

    for(int k = 0; k < commLength; k++)
    {
         // Copies characters from commandline input into str
        strncpy(str, &userInput[k], 1);
        
        // Continuously concatenates characters until str2 represents the entire command that was entered
        strcat(str2, str); 
    }
    
    // Allocates memory
    command = (char *)malloc(MAX_LINE);
    
    // Copies the final, command to the strTest variable. Now the command is finally ready to be entered into the list
    strcpy(command, str2);
    
    //Adds the command into the list
    add_to_list(command, false);
    
    //If user presses ctrl + d (thus the commLength will be 0), the shell will exit into the main terminal shell
    if (commLength == 0)
    {
        exit(0);
    }
    
    //Loops through every character in the userInput array
    for(i = 0; i < commLength; i++)
    {
        //Checks a character from userInput array
        switch(userInput[i])
        {
                //If there is a space character, the command parameter upto that space will be stored into args
                case ' ':
                if(inputIndex != -5)
                {
                    args[ParamCount] = &userInput[inputIndex]; //Stores parameter into args array
                    ParamCount++; //Increments number of arguments
                    inputIndex = -5; //Resets index for the next command
                    userInput[i] = '\0';// Null terminates the end of the command parameter
                }
                
                break;
                
                //If there is a new line character no more parameters will be stored
                case '\n':
                if(inputIndex != -5)
                {
                    args[ParamCount] = &userInput[inputIndex]; //Last parameter is stored
                    ParamCount++;
                    userInput[i] = '\0'; // Null terminates end of the entire command
                    args[ParamCount] = NULL; //Since this was the last command, everything after should be null
                }
                break;
            
             //Default case. If there is a character besides space or newline (usually should be)
            default:
                if(inputIndex == -5)
                {
                    //Everytime another command parameter has been found, the index will be set to the first index of that new parameter
                    inputIndex = i; 
                }
                
                if(userInput[i] == '&')
                {
                    *ampersand = 1;
                    userInput[i-1] = '\0'; //Makes the parameter before the ampersand into a C string and thus null terminates it
                }
                //*note*: This default case will not carry out a function inbetween when the first command parameter has been found and until the next is found b/c there is no need to.
        }
    }
    
    /*
    //Prints the arguments
    for(i = 0; i <= ParamCount; i++)
    {
        printf("args [%i]: %s\n", i, args[i]);  <--Not needed anymore but individual arguments can be printed if desired
    }*/
}