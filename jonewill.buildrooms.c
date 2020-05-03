

#include <sys/stat.h>
#include <unistd.h>  //pid
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h> //for bools
#include <time.h>  //random


char dir[20];
int* pid; 


struct Room 
{
    char* name; 
    char* type; 
    int numOutbound;  
    struct Room* outboundCon[6]; 
};



void shuffleArray(char **array, int n); 
void createDir(int pid);
bool sameRoom(struct Room* x, struct Room* y);
bool canAddConnectionFrom(struct Room* x);
void connectRoom(struct Room* x,struct  Room* y);
void addRandomConnection(struct Room* room);
void createFiles(int pid,struct  Room* roomArray);
bool connectionExists(struct Room* x, struct Room* y); 
bool isGraphFull(struct Room* rooms); 
void generateGraph(struct Room* rooms); 

//Hard coded file names
const char *fileNames[7] =
{
    "First_File", 
    "Second_File", 
    "Third_File", 
    "Fourth_File", 
    "Fifth_File", 
    "Sixth_File", 
    "Seventh_File"
};

//List of 10 rooms names
char *roomNames[10] = 
{
    "Living_Room", 
    "Master_Bedroom", 
    "Office", 
    "Kitchen", 
    "Bathroom_1", 
    "Guest_Bedroom", 
    "Bathroom_2", 
    "Front_Porch", 
    "Garage", 
    "Basement"
};

//--------------------------------------------------------
//Name: shuffleArray
//Parameters: The array and number of items in array
//Output: Returns the shuffled array
//Description: Used to shuffle array to randomize
//--------------------------------------------------------

void shuffleArray(char **array, int n)
{
    size_t i; 
    srand(time(NULL)); 
    int t = 0; 
    for (i = 0; i < n -1; i++)
    {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1); 
        array[t] = array[j]; 
        array[j] = array[i]; 
        array[i] = array[t]; 
    }
}

//--------------------------------------------------------
//Name: CreateDir
//Parameters: Takes in the PID 
//Output: Returns the new directory
//Description: Creates a new directory using the jonewill.rooms.<PID>
//--------------------------------------------------------
void createDir(int pid)
{
    sprintf(dir, "jonewill.rooms.%d", pid); 
    int result = mkdir(dir, 0755);
    printf("Result of mkdir(): %d\n", result); 
    
}


//--------------------------------------------------------
//Name: sameRoom
//Parameters: Two room structs
//Output: Returns true if they're the same, false if they're different
//Description: Compares two room names. Returns true if they're the same
//--------------------------------------------------------
bool sameRoom(struct Room* x, struct Room* y)
{
    //compares the name
    if (x->name == y->name)
    {
        return true; 
    }
    else
    {
        return false; 
    }
    
}

//--------------------------------------------------------
//Name: canAddOutboundFrom
//Parameters: single room 
//Output: Returns true if outbound connections are less than 6
//Description: Looks at number of outbound connections. True is < 6. False if more than 6
//--------------------------------------------------------
bool canAddConnectionFrom(struct Room* x)
{

    if (x->numOutbound < 6)
    {
        return true; 
    }
    else
    {
        return false; 
    }   
}

//--------------------------------------------------------
//Name: connectionExists
//Parameters: two room structs
//Output: Returns true they are already connected, false if not
//Description: compares the two rooms and checks if there is already a connection. True if one is found, false if not
//--------------------------------------------------------
bool connectionExists(struct Room* x, struct Room* y)
{
    int i = 0; 
    while (i < x->numOutbound)
    {
        //each connection for x
        if (x->outboundCon[i] == y) 
        {
            return true; //connection found 
        }
        i++; 
    }

    //if not found
    return false; 

}


//--------------------------------------------------------
//Name: connectRoom
//Parameters: two room structs
//Output: Room x points to room y
//Description: Used to point room x to room y. Used while creating the graph
//--------------------------------------------------------
void connectRoom(struct Room* x, struct Room* y)
{
    int num = x->numOutbound; //gets num
    if (canAddConnectionFrom(x) )
    {
        x->outboundCon[num] = y; //creates link
        x->numOutbound += 1; //increments int by one
    }
    
}


//--------------------------------------------------------
//Name: addRandomConnection
//Parameters: the room array 
//Output: Two rooms linked
//Description: Used to while creating the graph. 
//finds two random rooms by index. 
//Sends two rooms to the connect function. 
//--------------------------------------------------------
void addRandomConnection(struct Room* room)
{
  
    struct Room* x; 
    struct Room* y; 

    srand(time(NULL)); 
    int ind1; 
    int ind2;

    do
    {
        ind1 = rand() % 7; //random ind between 0 and 6
        x = &room[ind1]; 
    }while (canAddConnectionFrom(x) == false); 

    do 
    {
        ind2 = rand() % 7; //random ind between 0 and 6
        y = &room[ind2]; 
        //validates a connection can be made, not the same room, and connection doesn't exist
    }while (canAddConnectionFrom(y) == false || sameRoom(x, y) == true || connectionExists(x, y) == true );

    connectRoom(x,y); 
    connectRoom(y,x);
}

//--------------------------------------------------------
//Name: isGraphFull
//Parameters: the room array 
//Output: True if full, false if not
//Description: Checks if each room index has between 3 and 6 connections
//--------------------------------------------------------
bool isGraphFull(struct Room* rooms)
{
    int i = 0;
    int con;

    while (i < 7)
    {
        con = rooms[i].numOutbound;  //num for each room
        if (con < 3)                 //less than 3
        {
            return false; 
        }
        else if (con > 6)           //greater than 3
        {
            return false; 
        }
        i++; 
    }

    return true;                 //full returns true
}


//--------------------------------------------------------
//Name: createFiles
//Parameters: the pid, and roomArray
//Output: All the contents of the roomArray into each file
//Description: Creates the files and outputs the name, connections, and type of room to the files. 
//--------------------------------------------------------
void createFiles(int pid, struct Room* rooms)
{
    int i, j, r; 
  
    char file[100];
     
    sprintf(dir, "jonewill.rooms.%d", pid); //dir to moves files into 

    for (i = 0; i < 7; i++)   //loops through each ind of the array 
    {
        
        char file[100];
        for (j = 0; j < 100; j++) file[j] = '\0'; //initalizes each filename
        {
       
        //creates string for file name with dir
        strcat(file, dir); 
        strcat(file, "/"); 
        strcat(file, fileNames[i]); 

        FILE* fileDescriptor = fopen(file, "w"); //opens
      
        //appends to each file
        fprintf(fileDescriptor, "NAME: %s\n", roomNames[i]); 
        fprintf(fileDescriptor, "Room Type: %s\n", rooms[i].type); 
      
         for (r = 0; r < rooms[i].numOutbound; r++)
        {
            fprintf(fileDescriptor, "Connection %d: %s\n", r+1, rooms[i].outboundCon[r]->name);
        }

        fclose(fileDescriptor); //close file
        }
    }
}

//--------------------------------------------------------
//Name: generateGraph
//Parameters:  roomArray
//Output: Creates the graph, assignes a name, and type of room
//Description: Shuffles roomname array, assigns the first 7 ind to room name. 
//Generates two random numbers which are used to create indexes and assign the type of room. 
//passes array to random connection function to link other rooms. 
//--------------------------------------------------------
void generateGraph(struct Room* rooms)
{

    int i, j; 
    int randomNumber1 = 0;  // random num for index to assign type of room
    int randomNumber2 = 0;

    srand(time(NULL)); 

    //makes sure random nums are not the same
    while (randomNumber1 == randomNumber2)
    {
        randomNumber1 = (rand() % 7); 
        randomNumber2 = (rand() % 7); 
    }

    shuffleArray(roomNames, 10);  //shuffles array

    //first 7 ind assigned room names
    for (i = 0; i < 7; i++)
    {
        rooms[i].name = roomNames[i];  //room name
        if (i == randomNumber1)       //random number
        {
            rooms[i].type = "Start Room"; 
        } 
        else if(i == randomNumber2)     //random number
        {
            rooms[i].type = "End Room"; 
        }
        else
        {
            rooms[i].type = "Mid Room";  //rest of rooms
        }

        //Sets the initial number to zero 
        rooms[i].numOutbound = 0; 

        //Initalizes the outbound connection so the graph can be generated
        for (j = 0; j < 6; j++) 
        {
            rooms[i].outboundCon[j] = NULL;  //used to initalize
        }
        
    }

    //takes the array, while graph is incomplete, a random connection will be created
    while(isGraphFull(rooms) == false )
    {
        addRandomConnection(rooms);
    }
}



int main()
{
    struct Room roomArray[7]; 
    int pid = getpid(); 
    generateGraph(roomArray); 
    createDir(pid); 
    
    createFiles(pid, roomArray); 
  
    return 0; 
}