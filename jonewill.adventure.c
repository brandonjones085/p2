#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>
#include <unistd.h>  //pid
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> //for bools
#include <time.h>  //random

//Room struct from buildrooms.c
struct Room
{
    char* fileName; 
    char* name; 
    char* type; 
    int numOutbound;  
    struct Room* outboundCon[6]; 
};

//file constants
char *fileNames[7] =
{
    "First_File", 
    "Second_File", 
    "Third_File", 
    "Fourth_File", 
    "Fifth_File", 
    "Sixth_File", 
    "Seventh_File"
};


//globals
char newestDirName[256];   //holds newest dir
struct Room* rooms[7];     //room array
pthread_mutex_t thread;    //thread variable
char* timefile = "currentTime.txt"; 



void open(char* dir); 
void getRooms( char* dir); 
void roomArrayInit(); 
void readFile(char* file, int i); 
int printArray(char* r); 
int FindRoomByName(char* name);
int getRoom(char* r); 
void connectRoom(struct Room* x, struct Room* y);
void makeConnections(char* file, int i); 
void start(); 
int getStart();
void printPath(); 
void* getFile(); 



int getStart()
{
    int i=0; 
    int num = 100; 
    for(i=0; i < 7; i++)
    {
        if(strstr(rooms[i]->type, "Start_Room\n"))
        {
            num = i; 
        }

    }

    //printf("INDEX FOUND AT %d\n", num); 
    return num; 
}


//--------------------------------------------------------
//Name: open
//Parameters: Takes in the current directory
//Output: Fills the rooms array based on the input from files in the most recent directory
//Description: Takes in the current directory. 
//Searches the directory for the most recently created directory with the prefix jonewill.rooms.
//Moves into that directory and sends each file name into read function
//Loops through each file and adds the info into the rooms array. 
//--------------------------------------------------------
void open(char* dir)
{
    
    int newestDirTime=-1; 
    char targetDirPrefix[32] = "jonewill.rooms."; // Prefix we're looking for
   

    char line[256]; 
    

    DIR* dirToCheck; // Holds the directory we're starting in
    struct dirent *fileInDir; // Holds the current subdir of the starting dir
    struct stat dirAttributes;

    dirToCheck = opendir("."); // Open up the directory this program was run in

    if (dirToCheck > 0) // Make sure the current directory could be opened
    {
        while ((fileInDir = readdir(dirToCheck)) != NULL) //loop through each dir
        {
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL)  //compares to jonewill.rooms.
            {
                stat(fileInDir->d_name, &dirAttributes);         //gets stat struct 
               
                if((int)dirAttributes.st_mtime > newestDirTime)      //if greater than -1
                {
                    newestDirTime = (int)dirAttributes.st_mtime;
                 
                    strcpy(newestDirName, fileInDir->d_name);
                   

                    
                }
            }
        }
    }
    //once inside the newest dir, the files are read from that dir in a similar manner
    int i = 0; 
    int j = 0; 
    int q=0; 
    if((dirToCheck=opendir(newestDirName)) != NULL)
    {
        while ((fileInDir = readdir(dirToCheck)) != NULL)
        {
            if(strlen(fileInDir->d_name) > 2)
            {

                rooms[i]->fileName=fileInDir->d_name; //loads nme into file name of struct array
            
                int m;
                for (m = 0; m < 7; m++)
                {
                readFile(fileInDir->d_name, i);         //sends to read file function
                }
                
                i++; 
            }

        }
    }

}


//--------------------------------------------------------
//Name: readFile
//Parameters: Takes in char file and index of file
//Output: Fills the rooms array based on the input from files in the most recent directory
//Description: takes the file name given, 
//Reads each line of the file. 
//Uses strtok to parse through  the file input. 
//Filters each input and places data into rooms array. 
//--------------------------------------------------------

void readFile(char* file, int i)
{

    char buf1[256]; 
    char buf2[256]; 
    char token[256]; 

    chdir(newestDirName); //moves into newestdir

    FILE* fileToOpen; 

    fileToOpen = fopen(file, "r");   //open for read
    if(fileToOpen == NULL) //if error
    {
        printf("ERROR\n"); 
    }
    else //successfully found
    {  
        while(fgets(buf1, sizeof(buf1), fileToOpen) != NULL)  //reads each line
        {


            strtok(buf1, ":");     //looks for : and filters input
           // printf("LINE: %s\n", buf1); 
            strcpy(buf2, strtok(NULL, "")); //second part of string

            //adds \0 to end of string for both buf1 and buf2
            if (buf1[strlen(buf1)-1] == '\n')
            {
                buf1[strlen(buf1)-1]='\0';
            }
            
            if (buf2[strlen(buf2)-1] == '\n')
            {
                buf2[strlen(buf2)-1]='\0';
            }

            //searches for room type, room is either End Room, Mid Room, or Start Room
            if (strstr(buf1, "Room Type") != NULL)
            {
               if (strstr(buf2, "End"))
               {
                   rooms[i]->type = "End_Room\n"; 
               }
               else if (strstr(buf2, "Mid"))
               {
                   rooms[i]->type = "Mid_Room\n"; 
               }
               else
               {
                   rooms[i]->type = "Start_Room\n"; 
               }
               
            }

            //If I have more time, I will find a more elegant way
            //To handle this. 
            //I spent a long time on this only to just get this working
            //Looks for name of room
            //This includes all ten rooms whcih can be randomly generated from buildrooms.c
            else if (strstr(buf1, "NAME") != NULL)
            {
                if (strstr(buf2, "Living_Room"))
               {
                   rooms[i]->name = "Living_Room\n"; 
               }
               else if (strstr(buf2, "Master_Bedroom"))
               {
                   rooms[i]->name= "Master_Bedroom\n"; 
               }
               else if (strstr(buf2, "Office"))
               {
                   rooms[i]->name = "Office\n"; 
               }
                else  if (strstr(buf2, "Kitchen"))
               {
                   rooms[i]->name = "Kitchen\n"; 
               }
               else if (strstr(buf2, "Bathroom_1"))
               {
                   rooms[i]->name = "Bathroom_1\n"; 
               }
               else if (strstr(buf2, "Guest_Bedroom"))
               {
                   rooms[i]->name = "Guest_Bedroom\n"; 
               }
                else if (strstr(buf2, "Bathroom_2"))
               {
                   rooms[i]->name = "Bathroom_2\n"; 
               }
               else if (strstr(buf2, "Front_Porch"))
               {
                   rooms[i]->name= "Front_Porch\n"; 
               }
               else if (strstr(buf2, "Garage"))
               {
                   rooms[i]->name = "Garage\n"; 
               }
               else
               {
                   rooms[i]->name = "Basement\n";
               }
            }

            
        }    
        
    }
    fclose(fileToOpen);  //close file
    
}


//--------------------------------------------------------
//Name: connectRoom
//Parameters: Two room structs
//Output: Returns the rooms linked
//Description: Similar to connection function in buildrooms. 
//Takes two room structs and points x to y
//--------------------------------------------------------

void connectRoom(struct Room* x, struct Room* y)
{
     	int num = x->numOutbound; //gets num

        x->outboundCon[num] = y; //creates link
        x->numOutbound += 1; //increments int by one

}


//--------------------------------------------------------
//Name: getRoom
//Parameters: char room name
//Output: returns index of room as num
//Description: Takes in room name and finds index of room in array
//--------------------------------------------------------

int getRoom(char* r)
{
    int i=0; 
    int num=100; 
    char* p;
    for (i = 0; i < 7; i++)
   {  
      p=strstr(r, rooms[i]->name);
        if  (p)
        { 
            num = i; 
        }
    }
    return num; 
}


//--------------------------------------------------------
//Name: roomArrayInit
//Parameters: NA
//Output: Initialized array
//Description: Used to initialize the room array at beginning of program
//--------------------------------------------------------

void roomArrayInit()
{
	int i, j;
	for (i = 0; i < 7; i++)
	{	
        rooms[i] = malloc(sizeof(struct Room)); 
		for (j = 0; j < 6; j++)
		{
			rooms[i]->outboundCon[j] = NULL;
		}
		rooms[i]->numOutbound = 0;
        rooms[i]->name='\0';

	}
}

//--------------------------------------------------------
//Name: makeConnections
//Parameters: The file name and index
//Output: Creates the connections for all rooms
//Description: UReads files similar to readfile function
//Specifically filters for Connection 
//When connection found, sends name of connection to connectRooms function
//--------------------------------------------------------

void makeConnections(char* file, int index)
{
    char buf1[256]; 
    char buf2[256]; 
    char buf3[20];
    DIR* dirToCheck; // Holds the directory we're starting in
    struct dirent *fileInDir; // Holds the current subdir of the starting dir
    struct stat *dirAttributes = malloc(sizeof(struct stat)); // Holds information we've gained about subdir
    
    FILE* fileToOpen; 
    int i; 
    int n = 0; 

    fileToOpen = fopen(file, "r"); 
   // printf("FILE: %s\n", rooms[i]->fileName); 
    if(fileToOpen == NULL)    //failes to open file
    {
        printf("ERROR"); 
    }
    else
    {
        //printf("SUCCESS"); 
        //reads and filters exactly the same as readfile function
         while(fgets(buf1, sizeof buf1, fileToOpen) != NULL)
         {

            memset(buf2, '\0', 256);
            strtok(buf1, ":");
            strcpy(buf2, strtok(NULL, "")); 

            //terminates end with \0
            if(buf1[strlen(buf1) -1] == '\n')
            {
                buf1[strlen(buf1)-1] = '\0';
            }

         
            buf2[strlen(buf2)-1] = '\n';
            
            strcpy(buf3, buf2);//copies to buf3
             if (strstr(buf1, "Connect") != NULL)
             {
                // printf("SIZE %d", sizeof(buf2)); 
                // printf("BUF2 %s\n", buf2); 

                int ind = getRoom(buf2);
                // printf("BUF: %s");  
                // printf("ind: %d\n", ind);
                
               connectRoom(rooms[index], rooms[ind]); //connectRoom function
                // memset(buf2, '\0', 256); 
         
                // printf("I: %s", rooms[i]->name);
                // printf("CONNEC %s", buf2); 
            }
        }
    
    }
    
}

//--------------------------------------------------------
//Name: getFile
//Parameters: NA
//Output: Returns formated date/time to the currentTime file
//Description: Used with threading
//opens the time file and prints date/time formatted
//--------------------------------------------------------

void* getFile()
{

	FILE* file;
	file = fopen(timefile, "w+"); //opens file
    char buf[256];

	struct tm *t;  //tm struct
	time_t cur = time(NULL); //sets to null
	t = gmtime(&cur);  //current time

	
	strftime (buf, 256, "%Y-%m-%d %H:%M:%S", t);  //formats 
	fputs(buf, file);       //writest to file
	

	fclose(file);

}



//--------------------------------------------------------
//Name: start
//Parameters: The index of start room
//Output: The game is ran
//Description: The main function for game play
//Shows current room player is in, 
//Shows possible connections
//Shows type of room currently in. 
//Allows user to run time command to print current time. 
//When finished, shows number of steps and path. 
//--------------------------------------------------------

void start()
{

    //printf("INDEXFROM MAIN %d\n", start); 
    int steps = 0; 
    char input[100]; //user
    int i, j;
    int currentRoom = getStart(); 
    bool game = true; 
    bool error = false; //error found bool
    int ind;      //index
    int path[500]; //holds index of each step

    while (game)
    {

        path[steps]=currentRoom;         //sets current room index as index 0

        //checks for end room type
        if (strstr(rooms[currentRoom]->type, "End_Room\n"))
        {
            printf("YOU HAVE FOUND THE END ROOM. cONGATULATIONS!\n"); 
            printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n", steps);  //number of steps
            printPath(path, steps);  //shows the path 
            game = false; //ends game
            exit(0); 
       
        }
        

        printf("\nCurrent Room is: %s\n\n", rooms[currentRoom]->name);  //current room
        printf("Choose a connection\n");                                //prints connections
        for (j = 0; j < rooms[currentRoom]->numOutbound; j++)
        {
            printf("Connection %d %s\n", j+1, rooms[currentRoom]->outboundCon[j]->name);
        }
        memset(input, '\0', 100);  //clears for input

        printf("Where To? > "); 
        scanf("%s", input);                 //user input
        input[strlen(input)] = '\n';         //terminates end with '\n'
        printf("%s\n", input); 
        error = true;                       //sets error to true

        if (strcmp(input, "time\n") == 0)         //if time command ran
        {
            //all used for threading and creating a thread
            pthread_t time;
            pthread_mutex_init (&thread, NULL); //initalized
            pthread_mutex_lock(&thread); //lock thread
            pthread_create(&time, NULL, getFile, NULL); //calls get file
            pthread_join(time, NULL);  //joins thread
            pthread_mutex_unlock(&thread);
            pthread_mutex_destroy(&thread); //destroys
            error = false;                 //error is false


            printf("GETTING THE TIME\n"); 
            char buf[256];
	        
	        memset(buf, '\0', 256);     //clears space
	        FILE* file= fopen(timefile,"r");  //opens file

            if(file != NULL)  // current time file found
            {
                //printf("SUCCESS"); 
                fgets(buf, 256, file); //gets line from file
                printf("%s\n", buf);         //prints to terminal
            
                fclose(file);
            }

            else             //file not found
            {
                printf("ERROR OPENING TIME FILE");
                return;

            }
       
        }

        char* p;             //checks for match between user input and the connected rooms
        for (i = 0; i < rooms[currentRoom]->numOutbound; i++)
        {
            p=strstr(input, rooms[currentRoom]->outboundCon[i]->name);     
            if  (p)                       //match found
            { 
                int num = getRoom(input);   //gets index
                ///printf("INDEX %d\n", num); 
                currentRoom = num;         
                steps += 1;             //increments 

                error = false;   //sets to false to continue on loop
            }
        }

        if (error == true)      //error found
        {
            printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n"); 
        }
        
    }
    
}

//--------------------------------------------------------
//Name: printPath
//Parameters: path index array, number of steps
//Output: prints path of the user through each room
//Description: Used in the main function. 
//Prints user path once game is ended
//--------------------------------------------------------

void printPath(int path[], int step)
{
  int i; 
  for (i = 0; i < step+1; i++)
  { 
      printf("%s\n", rooms[path[i]]->name); 
  }  
}



//--------------------------------------------------------
//Name: main
//Parameters: The array and number of items in array
//Output: NA
//Description: main function where program is ran
//--------------------------------------------------------

int main()
{
    char dir[50];
    int startIndex; 
    
    memset(newestDirName, '\0', sizeof(newestDirName));
    roomArrayInit(); 
    open(dir); 
    printf("DIR: %s\n", newestDirName); 
    
   int i, j, n; 
 
    for (i = 0; i < 7; i++)
    {
     
      makeConnections(fileNames[i], i); 
    }

    // for (j = 0; j < 7; j++)
    // {
    //     printf("ROOM %s\n", rooms[j]->fileName); 
    //     printf("ROOM %s\n", rooms[j]->name); 
    //     for (n = 0; n < rooms[j]->numOutbound; n++)
    //     {
    //         printf("CONNECT %s\n", rooms[j]->outboundCon[n]->name);
    //     }
    //}

    start(); 

    //frees up space
    for (n = 0; n < 7; n++)
    {
      free(rooms[n]); 
    }

    return 0;  
}