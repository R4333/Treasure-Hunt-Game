#include <iostream>
#include <string>
#include <fstream>
#include <conio.h>

using namespace std;

#include "Stack.h"




enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN, none };
eDirection dir;


bool gameOver = 0;
bool portal_flag = 0;
bool first = 1;


Stack <string> path; // stack to store the path taken by the player

Stack <int> rooms; // stack to store the rooms visited by the player




//Maze Game
// X indicates => a forbidden cell (Wall),
// 0 indicates => an allowed cell (user can move to this cell),
// D=> Devil, G=> Gasper, T=> Treasure,
// P= > Portal teleports the user randomly to another room to a random cell.
// The user can move to any of the adjacent cells (up, down, left, right) if the cell is allowed.


int numMaps = 0;
//number of maps/rooms

int room_idx = 0;
//room index

int rows[100], cols[100];
// rows[room_idx] and cols[room_idx] are arrays that store the number of rows and columns of each maze

// int userX[100], userY[100];
// // userX[room_idx] and userY[room_idx] are arrays that store the user's current position in each maze



int initRow[100], initCol[100];
// userRow[room_idx] and userCol[room_idx] are arrays that store the user's initial default row and column in each maze



//function reads a file hauntedhouse.txt which contains the number of maps in the first line and contains the names of map files in the next lines.
//functions reads the map files and stores the map in a 2D array.
//map files contain the number of rows and columns in the first line and the starting position of the user in the second line. the next lines contain the map.
//function returns the number of maps in the file and the 2D array of maps.

void readMapFile(string mapFiles[], char maps[][100][100]) 
{
    numMaps = 0;
    string line;

    ifstream file("hauntedhouse.txt");

    //read the number of maps and store it in numMaps and store the names of map files in mapFiles array
    if (file.is_open()) {
        getline(file, line);
        numMaps = stoi(line);
        for (int i = 0; i < numMaps; i++) {
            getline(file, line);
            mapFiles[i] = line;
        }
        file.close();
    }
    for (int i = 0; i < numMaps; i++) {
        ifstream file(mapFiles[i]);
        if (file.is_open()) {

            getline(file, line);
            //reads comma separated values of rows and columns
            rows[i] = stoi(line.substr(0, line.find(",")));
            cols[i] = stoi(line.substr(line.find(",") + 1, line.length()));

            
            getline(file, line);
            //reads comma separated values of starting position of the user
            initRow[i] = stoi(line.substr(0, line.find(",")));
            initCol[i] = stoi(line.substr(line.find(",") + 1, line.length()));


            //reads the map with each position separated by space
            for (int j = 0; j < rows[i]; j++) {
                getline(file, line);
                for (int k = 0; k < cols[i]; k++) {
                    maps[i][j][k] = line[k * 2]; //each position is separated by space. so we need to read every other character.
                }
            }

            maps[i][initRow[i]][initCol[i]] = 'U'; // starting position of the user denoted by "U". when the user moves, the previous position is replaced by "0"
                                            // and the new position is replaced by "U". when loading a new map, search for "U" and it will be the starting position of the user.
            file.close();

        }

    }

// ******************************************
//                NOTE
// ******************************************
    // logical layout of the map:

    // map[Room idx.][Row][Col]
}


//function displays the map
void displayMap(char map[100][100][100]) 
{
    
    cout << "Room: " << room_idx + 1 << endl;
    cout << "******************************************" << endl;
    for (int i = 0; i < rows[room_idx]; i++) 
    {
        cout << "* ";
        for (int j = 0; j < cols[room_idx]; j++) 
        {

            if(map[room_idx][i][j] != '0')
            {
                cout << map[room_idx][i][j] << " ";

            }
            else
            {
                cout << "  ";
            }

            if(j == cols[room_idx] - 1)
            {
                cout << "*";
            }
            
        }
        cout << endl;
    }

    cout << "******************************************" << endl;
}

//recursive function to display the map
void displayMapRecursive(char map[100][100][100], int i, int j) 
{
    if (i == rows[room_idx]) 
    {
        return;
    }
    if (j == cols[room_idx]) 
    {
        cout << endl;
        displayMapRecursive(map, i + 1, 0);
        return;
    }
    if (j == 0) 
    {
        cout << "* ";
    }
    if (map[room_idx][i][j] != '0') 
    {
        cout << map[room_idx][i][j] << " ";
    }
    else 
    {
        cout << "  ";
    }
    if (j == cols[room_idx] - 1) 
    {
        cout << "*";
    }
    displayMapRecursive(map, i, j + 1);
}


int find_treasure_room_idx(char maps[][100][100]) {
    for (int i = 0; i < numMaps; i++) {
        for (int j = 0; j < rows[i]; j++) {
            for (int k = 0; k < cols[i]; k++) {
                if (maps[i][j][k] == 'T') {
                    return i;
                }
            }
        }
    }
    return -1;
}

//recursive function to find the room with the treasure
int recursive_find_treasure_room_idx(char maps[][100][100], int room_idx) {
    if (room_idx == numMaps) {
        return -1;
    }
    for (int j = 0; j < rows[room_idx]; j++) {
        for (int k = 0; k < cols[room_idx]; k++) {
            if (maps[room_idx][j][k] == 'T') {
                return room_idx;
            }
        }
    }
    return recursive_find_treasure_room_idx(maps, room_idx + 1);
}

//takes a room index and returns the row and column of the cell next to the portal
void find_portal_cell(char maps[][100][100], int room_idx, int& row, int& col) {
    for (int i = 0; i < rows[room_idx]; i++) {
        for (int j = 0; j < cols[room_idx]; j++) {
            if (maps[room_idx][i][j] == 'P') {
                if (i - 1 >= 0 && maps[room_idx][i - 1][j] == '0') {
                    row = i - 1;
                    col = j;
                    return;
                }
                else if (i + 1 < rows[room_idx] && maps[room_idx][i + 1][j] == '0') {
                    row = i + 1;
                    col = j;
                    return;
                }
                else if (j - 1 >= 0 && maps[room_idx][i][j - 1] == '0') {
                    row = i;
                    col = j - 1;
                    return;
                }
                else if (j + 1 < cols[room_idx] && maps[room_idx][i][j + 1] == '0') {
                    row = i;
                    col = j + 1;
                    return;
                }
            }
        }
    }
}

//recursive function to find the row and column of the cell next to the portal in a given room. function takes a room index and returns the row and column of the cell next to the portal

void recursive_find_portal_cell(char maps[][100][100], int room_idx, int& row, int& col) {
    if (room_idx == numMaps) {
        return;
    }
    for (int i = 0; i < rows[room_idx]; i++) {
        for (int j = 0; j < cols[room_idx]; j++) {
            if (maps[room_idx][i][j] == 'P') {
                if (i - 1 >= 0 && maps[room_idx][i - 1][j] == '0') {
                    row = i - 1;
                    col = j;
                    return;
                }
                else if (i + 1 < rows[room_idx] && maps[room_idx][i + 1][j] == '0') {
                    row = i + 1;
                    col = j;
                    return;
                }
                else if (j - 1 >= 0 && maps[room_idx][i][j - 1] == '0') {
                    row = i;
                    col = j - 1;
                    return;
                }
                else if (j + 1 < cols[room_idx] && maps[room_idx][i][j + 1] == '0') {
                    row = i;
                    col = j + 1;
                    return;
                }
            }
        }
    }
    recursive_find_portal_cell(maps, room_idx + 1, row, col);
}








void Input()
{
    dir = none;
    char ch;
    ch = _getch();

    switch (ch)
    {
        case 'a':
            dir = LEFT;
            break;
        case 'd':
            dir = RIGHT;
            break;
        case 'w':
            dir = UP;
            break;
        case 's':
            dir = DOWN;
            break;
        case 'x':
            gameOver = true;
            break;
    }
}

//recursively find the user's current position in the map and store it in x and y.
void find_user(char map[100][100][100], int& x, int& y, int i, int j) {
    if (i == rows[room_idx]) {
        return;
    }
    if (j == cols[room_idx]) {
        find_user(map, x, y, i + 1, 0);
        return;
    }
    if (map[room_idx][i][j] == 'U') {
        x = i;
        y = j;
        return;
    }
    find_user(map, x, y, i, j + 1);
}




void Logic(char map[100][100][100])
{
    int x, y;
    int prevroom;
    int prevX, prevY;

    string str;

    // //find the user's current position in the map and store it in x and y. if the user is not found then use the default starting position of the user.
    // for (int i = 0; i < rows[room_idx]; i++) {
    //     for (int j = 0; j < cols[room_idx]; j++) {
    //         if (map[room_idx][i][j] == 'U') {
    //             x = i;
    //             y = j;
    //             break;
    //         }
    //     }
    // }

    find_user(map, x, y, 0, 0); //recursive function to find the user's current position in the map and store it in x and y.


    //remove duplicate edge case
    for (int i = 0; i < rows[room_idx]; i++) 
    {
        for (int j = 0; j < cols[room_idx]; j++) 
        {
            if (map[room_idx][i][j] == 'U' && i != x && j != y) 
            {
                map[room_idx][i][j] = '0';
            }
        }
    }




    //store the previous position of the user

    prevX = x;
    prevY = y;
    prevroom = room_idx;


    switch (dir)
    {
        case LEFT:
            y--;
            break;
        case RIGHT:
            y++;
            break;
        case UP:
            x--;
            break;
        case DOWN:
            x++;
            break;    
        default:
            break;
    }

    //check if the user can move to the new position
    if (map[room_idx][x][y] == '0') 
    {
        map[room_idx][x][y] = 'U';
        map[room_idx][prevX][prevY] = '0';

        str = "Room: " + to_string(room_idx + 1) + "  Position: (" + to_string(x) + "," + to_string(y) + ")";
        path.push(str);



    }
    else if (map[room_idx][x][y] == 'T') 
    {
        cout << "You found the treasure!" << endl;

        str = "Room: " + to_string(room_idx + 1) + " Treasure " +"  Position: (" + to_string(x) + "," + to_string(y) + ")";
        path.push(str);

        //print postion of treasure and treasure room number
        cout << "Treasure room: " << room_idx + 1 << endl;
        cout << "Treasure Index position: " << x << ", " << y << endl;
        gameOver = true;
        system("pause");
    }
    else if (map[room_idx][x][y] == 'D') {

        str = "Room: " + to_string(room_idx + 1) + " Devil " +"  Position: (" + to_string(x) + "," + to_string(y) + ")";
        path.push(str);

        str =  "You were eaten by the devil!";
        path.push(str);

        cout << "You were eaten by the devil!" << endl;
        gameOver = true;
        system("pause");
    }
    else if (map[room_idx][x][y] == 'G') {

        str = "Room: " + to_string(room_idx + 1) + " Gasper " + "  Position: (" + to_string(x) + "," + to_string(y) + ")";
        path.push(str);

        rooms.push(room_idx);

        first = false;

       
        // room_idx = find_treasure_room_idx(map);  //iterative method

        room_idx = recursive_find_treasure_room_idx(map, 0); //recursive method

        cout << "You found the treasure room!" << endl;
        cout << "You are now in room " << room_idx + 1 << endl;
        map[room_idx][initRow[room_idx]][initCol[room_idx]] = 'U';

        str = "Room: " + to_string(room_idx + 1) + "  Position: (" + to_string(initRow[room_idx]) + "," + to_string(initCol[room_idx]) + ")";
        path.push(str);

        //replace the previous position of the user with "0"
        map[prevroom][prevX][prevY] = '0';

        //restore the starting position of the user in the previous room
        map[prevroom][initRow[prevroom]][initCol[prevroom]] = 'U';


        system("pause");
        
    }
    else if (map[room_idx][x][y] == 'P') 
    {
        //         P= > Portal teleports the user randomly to another room to a random cell.
        // Every time user reaches a portal. The portal flips a coin. If the head comes, he is transported randomly to some room in the
        // house. In case the tail occurs then he is taken back to the room from where he came (to the cell next to portal).
        // Note: For the first time, when the user enters the house and reaches a portal in a room, then on the tail, he is kicked out of the
        // house, and the game ends.

        // 0 = head, 1 = tail

        

        str = "Room: " + to_string(room_idx + 1) + " Portal " + "  Position: (" + to_string(x) + "," + to_string(y) + ")";
        path.push(str);

        int coin = rand() % 2;

        //kickout the user if he uses portal in the treasure room
        if (room_idx == recursive_find_treasure_room_idx(map, 0)) 
        {
            str = "You were kicked out of the house for using the portal in treasure room!";
            path.push(str);

            cout << "You were kicked out of the house for using the portal in treasure room!" << endl;
            gameOver = true;
            system("pause");
        }
        else if (first && coin != 0)
        {
            cout << "You were kicked out of the house!" << endl;
            gameOver = true;

            

            str = "You were kicked out of the house!";
            path.push(str);

            system("pause");

        }
        else
        {
            int random_room_idx = rand() % numMaps;
            //check if the random room is the same as the current room
            while (random_room_idx == room_idx) 
            {
                random_room_idx = rand() % numMaps;
            }
            int random_row = rand() % rows[random_room_idx];
            int random_col = rand() % cols[random_room_idx];
            //checked if the random position is a wall or not. if it is a wall, then generate a new random position.
            while (map[random_room_idx][random_row][random_col] != '0') 
            {
                random_room_idx = rand() % numMaps;
                random_row = rand() % rows[random_room_idx];
                random_col = rand() % cols[random_room_idx];
            }
            
            if (coin == 0) //heads
            {
                cout << "You were transported to new room " << random_room_idx + 1 << endl;
                rooms.push(room_idx);
                room_idx = random_room_idx;
                x = random_row;
                y = random_col;
                map[room_idx][random_row][random_col] = 'U'; //set the user in the random position
                map[prevroom][prevX][prevY] = '0'; //replace the previous position of the user with "0"

                str = "Room: " + to_string(room_idx + 1) + "  Position: (" + to_string(x) + "," + to_string(y) + ")";
                path.push(str);

                //remove the initial position of the user from the new room
                map[room_idx][initRow[room_idx]][initCol[room_idx]] = '0';

                //restore the initial position of the user in the previous room
                map[prevroom][initRow[prevroom]][initCol[prevroom]] = 'U';
            }
            else //tails
            {
                cout << "You were transported to the previous room " << prevroom + 1 << endl;
                room_idx = rooms.pop();
                
                //send the user to the previous room to the cell next to the portal
                // find_portal_cell(map, room_idx, x, y); //iterative method

                recursive_find_portal_cell(map, room_idx, x, y); //recursive method

                map[room_idx][x][y] = 'U';
                //remove the initial position of the user from the new room
                map[room_idx][initRow[room_idx]][initCol[room_idx]] = '0';
                //restore the initial position of the user in the previous room
                map[prevroom][initRow[prevroom]][initCol[prevroom]] = 'U';
                //replace the previous position of the user with "0"
                map[prevroom][prevX][prevY] = '0';




                str = "Room: " + to_string(room_idx + 1) + "  Position: (" + to_string(x) + "," + to_string(y) + ")";
                path.push(str);
                // room_idx = prevroom;
                // map[prevroom][prevX][prevY] = 'U';
                // map[prevroom][prevX][prevY] = '0';
            }
            system("pause");
        }


        first = false;
    }
} 




//recursive function for gameover
void game(char maps[100][100][100])
{
    if (!gameOver)
    {

        displayMapRecursive(maps, 0, 0); //recursive method
        Input();
        Logic(maps);

        system("cls");

        game(maps);
    }
    
}


        



int main()
{

    string mapFiles[100]; //stores the names of map files
    char maps[100][100][100]; //stores the maze maps
    readMapFile(mapFiles, maps); //reads the map files and stores the maps in maps array

    srand(time(NULL)); //seed the random number generator

    

    // while (!gameOver)
    // {
    //     displayMap(maps);
    //     Input();
    //     Logic(maps);

    //     system("cls");
    // }

    // cout << "Game Over!" << endl;

    // cout << "Path: " << endl;
    // path.print();

    // system("pause");

    game(maps); // recursive method

    cout << "Game Over!" << endl;

    cout << "Path: " << endl;
    path.print();

    system("pause");











    return 0;

}