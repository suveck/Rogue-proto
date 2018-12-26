#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sstream>
#include <ctime>

/*
#include <conio.h>//for arrow key stuff
#define KB_UP 72
#define KB_DOWN 80
#define KB_LEFT 75
#define KB_RIGHT 77
#define KB_ESCAPE 27
*/
using namespace std;

class room
{
public:
	string layout[5][5]{ { " ? " } };
	string parentLocation = "none";
	int trap = 0;
	int treasure = 0;
	bool roomRendered = false;
	bool childUp = false;
	bool childDown = false;
	bool childLeft = false;
	bool childRight = false;
	int secretPathUp = 0;
	int secretPathDown = 0;
	int secretPathLeft = 0;
	int secretPathRight = 0;
};
class player
{
public:
	bool ded = false;
	int treasure = 0;
	string icon = " @ ";
};
void newRoomCatalyst(player* plr, room map[11][11], int* mapY, int* mapX, int* roomY, int* roomX);
void newRoom(room map[11][11], string parentDir, int mapY, int mapX, room lastroom); //room builder
void randomizeChildren(room* newRoom);
bool areChildSame(room room1, room room2);
bool areRoomSame(room room1, room room2);
void buildRoom(room* room);
void drawWorld(room map[11][11]); //render world
void blankWorld(room map[11][11]);//to draw the background so that the drawworld works when not full map
void drawRoom(room room);//render room
void printRoomProperties(room test);//for validating room properties
void printEnd (bool victory);
char arrowToWASD();

int main()
{
	int MAP_SIZE = 11;
	int ROOM_SIZE = 5;
	int mapY = (MAP_SIZE / 2);// x/y coord for plr in whole map
	int mapX = (MAP_SIZE / 2);
	int roomY = ROOM_SIZE / 2;// x/y coord for plr inside room
	int roomX = ROOM_SIZE / 2;

	room map[11][11];
	blankWorld(map);
	room parentOrigin;
	randomizeChildren(&parentOrigin);
	while ((parentOrigin.childUp == false) &&
		(parentOrigin.childDown == false) &&
		(parentOrigin.childLeft == false) &&
		(parentOrigin.childRight == false))
		randomizeChildren(&parentOrigin);//ensures no single room maps
	parentOrigin.trap = 0;
	map[mapY][mapX] = parentOrigin; //<^ is making the base room and assigning it to the center of the map
	buildRoom(&map[mapY][mapX]);
	player plr;

	while ((plr.ded == false) && (plr.treasure < 3))
	{
		newRoomCatalyst(&plr, map, &mapY, &mapX, &roomY, &roomX);
	};
	if(plr.ded == true)printEnd(false);
	else printEnd(true);
}


void newRoomCatalyst(player* plr, room map[11][11], int* mapY, int* mapX, int* roomY, int* roomX)//needs to be changed when plr move is implemented
{
	char command = '?';
	map[*mapY][*mapX].layout[*roomY][*roomX] = plr->icon;// overlay player icon
	drawWorld(map);
	//printRoomProperties(map[*mapY][*mapX]);
	//cout << "roomX is \t:"<<*roomX <<"\t\troomY is \t:"<< *roomY<<endl;
	//cout << "mapX is \t:"<<*mapX <<"\t\tmapY is \t:"<< *mapY<<endl;
	cout << "Direction?? (WASD)"<<endl;
	cin >> command;
    if (command == 'q')plr->ded = true;
	if ((command == 'w') && (*roomY == 0))// handles moving to new room that is not rendered
	{
		map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
		*mapY -= 1;
		newRoom(map, "Down", *mapY, *mapX, map[*mapY + 1][*mapX]);
		*roomY = 4;
	}
    else if ((command == 's') && (*roomY == 4))// handles moving to new room that is not rendered
	{
		map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
		*mapY += 1;
		newRoom(map, "Up", *mapY, *mapX, map[*mapY - 1][*mapX]);
		*roomY = 0;
	}
    else if ((command == 'a') && (*roomX == 0))// handles moving to new room that is not rendered
	{
		map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
		*mapX -= 1;
		newRoom(map, "Right", *mapY, *mapX, map[*mapY][*mapX + 1]);
		*roomX = 4;
	}
    else if ((command == 'd') && (*roomX == 4))// handles moving to new room that is not rendered
	{
		map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
		*mapX += 1;
		newRoom(map, "Left", *mapY, *mapX, map[*mapY][*mapX - 1]);
		*roomX = 0;
	}


    else// handles all of the moving within rooms
    {
        if (command == 'w') // handles all of the moving within rooms
        {
            if (map[*mapY][*mapX].layout[*roomY-1][*roomX] == " # ")
            {
                cout << " Cant move here\n";
            }
            else
            {
                map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                *roomY -= 1;
                if (map[*mapY][*mapX].layout[*roomY][*roomX] == "~~~")
                {
                    map[*mapY][*mapX].layout[*roomY][*roomX] = "~X~";
                    plr->ded = true;
                    drawWorld(map);
                }
                if (map[*mapY][*mapX].layout[*roomY][*roomX] == " $ ")
                {
                    map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                    plr->treasure += 1;
                    drawWorld(map);
                }
            }
        }
        else if (command == 'a')
        {
            if (map[*mapY][*mapX].layout[*roomY][*roomX-1] == " # ")
            {
                cout << " Cant move here\n";
            }
            else
            {
                map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                *roomX -= 1;
                if (map[*mapY][*mapX].layout[*roomY][*roomX] == "~~~")
                {
                    map[*mapY][*mapX].layout[*roomY][*roomX] = "~X~";
                    plr->ded = true;
                    drawWorld(map);
                }
                if (map[*mapY][*mapX].layout[*roomY][*roomX] == " $ ")
                {
                    map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                    plr->treasure += 1;
                    drawWorld(map);
                }
            }
        }
        else if (command == 's')
        {
            if (map[*mapY][*mapX].layout[*roomY + 1][*roomX] == " # ")
            {
                cout << " Cant move here\n";
            }
            else
            {
                map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                *roomY += 1;
                if (map[*mapY][*mapX].layout[*roomY][*roomX] == "~~~")
                {
                    map[*mapY][*mapX].layout[*roomY][*roomX] = "~X~";
                    plr->ded = true;
                    drawWorld(map);
                }
                if (map[*mapY][*mapX].layout[*roomY][*roomX] == " $ ")
                {
                    map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                    plr->treasure += 1;
                    drawWorld(map);
                }
            }
        }
        else if (command == 'd')
        {
            if (map[*mapY][*mapX].layout[*roomY][*roomX + 1] == " # ")
            {
                cout << " Cant move here\n";
            }
            else
            {
                map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                *roomX += 1;
                if (map[*mapY][*mapX].layout[*roomY][*roomX] == "~~~")
                {
                    map[*mapY][*mapX].layout[*roomY][*roomX] = "~X~";
                    plr->ded = true;
                    drawWorld(map);
                }
                if (map[*mapY][*mapX].layout[*roomY][*roomX] == " $ ")
                {
                    map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                    plr->treasure += 1;
                    drawWorld(map);
                }
            }
        }
    }
};
void newRoom(room map[11][11], string parentDir, int mapY, int mapX, room lastRoom) //room builder
{
    if(map[mapY][mapX].roomRendered == false)
    {
        randomizeChildren(&map[mapY][mapX]);
        while (areChildSame(map[mapY][mapX], lastRoom))
        {
            cout <<"Randomizing Next Room\n";
            sleep(1);
            randomizeChildren(&map[mapY][mapX]);
        };
        if (mapY == 0) map[mapY][mapX].childUp = false;//ensures that boarder rooms do not have openings to nothing
        if (mapY == 10) map[mapY][mapX].childDown = false;
        if (mapX == 0) map[mapY][mapX].childLeft = false;
        if (mapX == 10) map[mapY][mapX].childRight = false;
        if (parentDir == "Up") map[mapY][mapX].childUp = true;//ensures the child always connects to parent
        if (parentDir == "Down") map[mapY][mapX].childDown = true;
        if (parentDir == "Left") map[mapY][mapX].childLeft = true;
        if (parentDir == "Right") map[mapY][mapX].childRight = true;
        if ((map[mapY-1][mapX].roomRendered == true)&&(map[mapY-1][mapX].childDown == false)&&(map[mapY][mapX].childUp == true))
        {
            map[mapY][mapX].secretPathUp = 1;
            map[mapY][mapX].childUp = false;
        }
        if ((map[mapY+1][mapX].roomRendered == true)&&(map[mapY+1][mapX].childUp == false)&&(map[mapY][mapX].childDown == true))
        {
            map[mapY][mapX].secretPathDown = 1;
            map[mapY][mapX].childDown = false;
        }
        if ((map[mapY][mapX-1].roomRendered == true)&&(map[mapY][mapX-1].childRight == false)&&(map[mapY][mapX].childLeft == true))
        {
            map[mapY][mapX].secretPathLeft = 1;
            map[mapY][mapX].childLeft = false;
        }
        if ((map[mapY][mapX+1].roomRendered == true)&&(map[mapY][mapX+1].childLeft == false)&&(map[mapY][mapX].childRight == true))
        {
            map[mapY][mapX].secretPathRight = 1;
            map[mapY][mapX].childRight = false;
        }
        buildRoom(&map[mapY][mapX]);
    }
}
void randomizeChildren(room* newRoom)
{
	newRoom->trap = ((((clock() % 150) % 21) % 13) % 7);
	newRoom->treasure = ((clock() % 25));

	newRoom->roomRendered = true;
	int numChild = clock();
	numChild = ((numChild % 4));//mod will give either 0, 1, 2, 3
	int whatChild = time(0) % 4;// for determining direction Child is allocated
	while (numChild != 0)
	{
		if (whatChild == 0)
			newRoom->childUp = true;
		if (whatChild == 1)
			newRoom->childDown = true;
		if (whatChild == 2)
			newRoom->childLeft = true;
		if (whatChild == 3)
			newRoom->childRight = true;
		numChild--;
		whatChild = (whatChild + (time(0) % 9)) % 4;//attempt to rand next child
	}
}
bool areChildSame(room room1, room room2)
{
	if ((room1.childUp == room2.childUp) &&
		(room1.childDown == room2.childDown) &&
		(room1.childLeft == room2.childLeft) &&
		(room1.childRight == room2.childRight))return true;
	else
		return false;
}
bool areRoomSame(room room1, room room2)
{
	if ((room1.childUp == room2.childUp) &&
		(room1.childDown == room2.childDown) &&
		(room1.childLeft == room2.childLeft) &&
		(room1.childRight == room2.childRight) &&
		(room1.trap == room2.trap) &&
		(room1.treasure == room2.treasure) &&
		(room1.parentLocation == room2.parentLocation))return true;
	else
		return false;
}
void printRoomProperties(room test)//for validating room properties
{
	cout << "parent location:\t" << test.parentLocation << endl;
	cout << "childUp:\t" << test.childUp << endl;
	cout << "childDown:\t" << test.childDown << endl;
	cout << "childLeft:\t" << test.childLeft << endl;
	cout << "childRight:\t" << test.childRight << endl;
}
void buildRoom(room* room)
{
	for (int i = 0; i <= 4; i++)//for populating parentOrigin blank map while functuon is not working
	{
		for (int j = 0; j <= 4; j++)
		{
			room->layout[i][j] = " - ";
		}
		//cout << endl;
	}
	int j = 0;//build walls
	for (int i = 0; i <= 4; i++)
	{
		room->layout[i][j] = " # ";
		if ((i == 4) && (j == 0)) { i = 0; j = 4; }
	}
	j = 0;
	for (int i = 0; i <= 4; i++)
	{
		room->layout[j][i] = " # ";
		if ((i == 4) && (j == 0)) { i = 0; j = 4; }
	}
	if (room->childUp == true)//build doors
	{
		room->layout[0][1] = " - ";
		room->layout[0][2] = " - ";
		room->layout[0][3] = " - ";
	};
	if (room->childDown == true)
	{
		room->layout[4][1] = " - ";
		room->layout[4][2] = " - ";
		room->layout[4][3] = " - ";
	};
	if (room->childLeft == true)
	{
		room->layout[1][0] = " - ";
		room->layout[2][0] = " - ";
		room->layout[3][0] = " - ";
	};
	if (room->childRight == true)
	{
		room->layout[1][4] = " - ";
		room->layout[2][4] = " - ";
		room->layout[3][4] = " - ";
	};
    if (room->secretPathUp == 1)
	{
		room->layout[0][1] = " * ";
		room->layout[0][2] = " * ";
		room->layout[0][3] = " * ";
	};
	if (room->secretPathDown == 1)
	{
		room->layout[4][1] = " * ";
		room->layout[4][2] = " * ";
		room->layout[4][3] = " * ";
	};
	if (room->secretPathLeft == 1)
	{
		room->layout[1][0] = " * ";
		room->layout[2][0] = " * ";
		room->layout[3][0] = " * ";
	};
	if (room->secretPathRight == 1)
	{
		room->layout[1][4] = " * ";
		room->layout[2][4] = " * ";
		room->layout[3][4] = " * ";
	};
	if (room->trap == 1) { room->layout[1][1] = "~~~"; room->layout[1][2] = "~~~"; room->layout[2][1] = "~~~"; room->layout[2][2] = "~~~"; };
	if (room->trap == 2) { room->layout[1][3] = "~~~"; room->layout[1][2] = "~~~"; room->layout[2][3] = "~~~"; room->layout[2][2] = "~~~"; };
	if (room->trap == 3) { room->layout[3][1] = "~~~"; room->layout[3][2] = "~~~"; room->layout[2][1] = "~~~"; room->layout[2][2] = "~~~"; };
	if (room->trap == 4) { room->layout[3][3] = "~~~"; room->layout[3][2] = "~~~"; room->layout[2][3] = "~~~"; room->layout[2][2] = "~~~"; };

    if (room->treasure == 0) room->layout[1][1] = " $ ";
    if (room->treasure == 1) room->layout[1][2] = " $ ";
    if (room->treasure == 2) room->layout[1][3] = " $ ";
    if (room->treasure == 3) room->layout[2][1] = " $ ";
    if (room->treasure == 4) room->layout[2][2] = " $ ";
    if (room->treasure == 5) room->layout[2][3] = " $ ";
    if (room->treasure == 6) room->layout[3][1] = " $ ";
    if (room->treasure == 7) room->layout[3][2] = " $ ";
    if (room->treasure == 8) room->layout[3][3] = " $ ";
}
void drawWorld(room map[11][11]) //render world
{
	for (int Y = 0; Y < 11; Y++)//col
	{
		for (int i = 0; i < 5; i++)//col lil
		{
			for (int X = 0; X < 11; X++)//row
			{
				for (int j = 0; j < 5; j++)//row lil
				{
					/*if (map[Y][X].roomRendered)*/cout << map[Y][X].layout[i][j];
				}
			}
			cout << endl;
		}
	}
}
void blankWorld(room map[11][11])
{
	for (int Y = 0; Y < 11; Y++)//col
	{
		for (int i = 0; i < 5; i++)//col lil
		{
			for (int X = 0; X < 11; X++)//row
			{
				for (int j = 0; j < 5; j++)//row lil
				{
					map[Y][X].layout[i][j] = "   ";
				}
			}
		}
	}
}
void drawRoom(room room)//render room
{
	for (int i = 0; i <= 4; i++)
	{
		for (int j = 0; j <= 4; j++)
		{
			if (room.layout[i][j] == "")
				cout << "missing room data @ " << i << j << endl;
			else
				cout << room.layout[i][j];//<<"\t" ;//print line in room layout array
		}
		cout << endl;
	}
}
void printEnd (bool victory)
{
    if(victory == true)
    {
        cout <<"win";
    }
    if(victory == false)
    {
        cout <<"!win";
    }
};
char arrowToWASD()
{
/*
   int KB_code=0;

   while(KB_code != KB_ESCAPE )
   {
     if (kbhit())
      {
            KB_code = getch();
            printf("KB_code = %i \n",KB_code);

            switch (KB_code)
            {
                case KB_LEFT:
                           //Do something
                break;

                case KB_RIGHT:
                           //Do something
                break;

                case KB_UP:
                           //Do something
                break;

                case KB_DOWN:
                           //Do something
                break;

            }

      }
  }
  */
}

/* THINGS TO FIX
    make ascii images for win / loss
    fix arrow to WASD
*/
