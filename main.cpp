#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <sstream>
#include <ctime>
//#include "win.txt"
//#include "ded.txt"

#define PI 3.14159265

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
	int trap = 0;//int determines wher or if trap is
	int treasure = 0;//same as trap
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
    int directionFacing = 1;//1=up, 2=left, 3=down, 4=right
	bool ded = false;
	int treasure = 0;
	string icon = " ^ ";
    bool viewport = false;
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
char arrowToWASD();// not wrtitten yet
void updatePlayerDirection(player *plr);
void roomPerspective(room map[11][11],room room,int mapX, int mapY, int roomX, int roomY, player plr);
void nextRoomPerspective(room map[11][11], room room,int mapX, int mapY, int roomX, int roomY, player plr);
void roomPerspectiveNoDraw(room map[11][11], room room,int mapX, int mapY, int roomX, int roomY, player plr);

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
	else if (plr.treasure == 3)printEnd(true);
}


void newRoomCatalyst(player* plr, room map[11][11], int* mapY, int* mapX, int* roomY, int* roomX)//needs to be changed when plr move is implemented
{

	char command = '?';
	bool inNewRoom = false;// so that you dont add extra movement going to new rooms
	map[*mapY][*mapX].layout[*roomY][*roomX] = plr->icon;// overlay player icon


    drawWorld(map);
    if (plr->viewport == true)
    {
        roomPerspective(map, map[*mapY][*mapX],*mapX,*mapY, *roomX, *roomY, *plr);//fix input data types
    }

	//printRoomProperties(map[*mapY][*mapX]);
	//cout << "roomX is \t:"<<*roomX <<"\t\troomY is \t:"<< *roomY<<endl;
	//cout << "mapX is \t:"<<*mapX <<"\t\tmapY is \t:"<< *mapY<<endl;

	cout << "Treasure collected:\t" << plr->treasure << endl;
	cout << "Direction?? (WASD) -> ENTER:" << endl;
	string tempToFixMultiInput;
	cin >> tempToFixMultiInput;
	command = tempToFixMultiInput.at(0);

    if (command == 'p'){plr->viewport = !plr->viewport;}
    else if (command == 'q')plr->ded = true;
    else if (command == 'z')
    {
        plr->directionFacing +=1;
        if(plr->directionFacing == 5)plr->directionFacing=1;
    }
    else if (command == 'x')
    {
        plr->directionFacing -=1;
        if(plr->directionFacing == 0)plr->directionFacing=4;
    }
	else if ((command == 'w') && (*roomY == 0))// handles moving to new room that is not rendered
	{
        plr->directionFacing = 1;
		map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
		*mapY -= 1;
		newRoom(map, "Down", *mapY, *mapX, map[*mapY + 1][*mapX]);
		*roomY = 4;
		inNewRoom = true;
	}
    else if ((command == 's') && (*roomY == 4))// handles moving to new room that is not rendered
	{
        plr->directionFacing = 3;
		map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
		*mapY += 1;
		newRoom(map, "Up", *mapY, *mapX, map[*mapY - 1][*mapX]);
		*roomY = 0;
		inNewRoom = true;
	}
    else if ((command == 'a') && (*roomX == 0))// handles moving to new room that is not rendered
	{
        plr->directionFacing = 2;
		map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
		*mapX -= 1;
		newRoom(map, "Right", *mapY, *mapX, map[*mapY][*mapX + 1]);
		*roomX = 4;
		inNewRoom = true;
	}
    else if ((command == 'd') && (*roomX == 4))// handles moving to new room that is not rendered
	{
        plr->directionFacing = 4;
		map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
		*mapX += 1;
		newRoom(map, "Left", *mapY, *mapX, map[*mapY][*mapX - 1]);
		*roomX = 0;
		inNewRoom = true;
	}
	updatePlayerDirection(plr);


    // handles all of the moving within rooms

    if (command == 'w') // handles all of the moving within rooms
    {
        plr->directionFacing = 1;
        updatePlayerDirection(plr);
        if (map[*mapY][*mapX].layout[*roomY-1][*roomX] == " # ")
        {
            cout << " Cant move here\n";sleep(2);
        }
        else
        {
            if (!inNewRoom)
            {
                map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                *roomY -= 1;
            }
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
        plr->directionFacing = 2;
        updatePlayerDirection(plr);
        if (map[*mapY][*mapX].layout[*roomY][*roomX-1] == " # ")
        {
            cout << " Cant move here\n";sleep(2);
        }
        else
        {
            if (!inNewRoom)
            {
                map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                *roomX -= 1;
            }
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
        plr->directionFacing = 3;
        updatePlayerDirection(plr);
        if (map[*mapY][*mapX].layout[*roomY + 1][*roomX] == " # ")
        {
            cout << " Cant move here\n";sleep(2);
        }
        else
        {
            if (!inNewRoom)
            {
                map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                *roomY += 1;
            }
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
        plr->directionFacing = 4;
        updatePlayerDirection(plr);
        if (map[*mapY][*mapX].layout[*roomY][*roomX + 1] == " # ")
        {
            cout << " Cant move here\n";sleep(2);
        }
        else
        {
            if (!inNewRoom)
            {
                map[*mapY][*mapX].layout[*roomY][*roomX] = " - ";
                *roomX += 1;
            }
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


    if (plr->viewport == true)
    {
        roomPerspectiveNoDraw(map, map[*mapY][*mapX],*mapX,*mapY, *roomX, *roomY, *plr);//fix input data types
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
	numChild = ((numChild % 2)+2);//mod will give either 0, 1, 2, 3
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
void printRoomProperties(room test)//for validating room properties (mostly for trouble shooting regeneration)
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
    room->roomRendered = true;
}
void drawWorld(room map[11][11]) //render world
{

//created size of box to print
    int drawMinX = 0;
    int drawMaxX = 10;
    int drawMinY = 0;
    int drawMaxY = 10;

/* not working properly
    for (int i = 0 ; i < 5 ; i++)
    {
        for (int j = 0 ; j < 11 ; j++)
        {
            if (!(map[i][j].roomRendered)) drawMinY = i;
        }
    }
    for (int i = 10 ; i > 5 ; i--)
    {
        for (int j = 0 ; j < 11 ; j++)
        {
            if (!(map[i][j].roomRendered)) drawMaxY = i;
        }
    }
    */



//worldmap printer function
	for (int Y = drawMinY; Y < drawMaxY; Y++)//col
	{
		for (int i = 0; i < 5; i++)//col lil
		{
			for (int X = drawMinX; X < drawMaxX; X++)//row
			{
				for (int j = 0; j < 5; j++)//row lil
				{
					cout << map[Y][X].layout[i][j];
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
        ifstream endTxt("win.txt");
        string line;
        while (getline(endTxt, line))
        {
            cout <<line << endl;
        }
    }
    if(victory == false)
    {
        ifstream endTxt("ded.txt");
        string line;
        while (getline(endTxt, line))
        {
            cout <<line << endl;
        }
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

void updatePlayerDirection(player *plr)
{
    if      (plr->directionFacing == 1)plr->icon = " ^ ";
    else if (plr->directionFacing == 2)plr->icon = " < ";
    else if (plr->directionFacing == 3)plr->icon = " v ";
    else if (plr->directionFacing == 4)plr->icon = " > ";
    return;
};


//Below is experimental and not implemented

void roomPerspective(room map[11][11], room room,int mapX, int mapY, int roomX, int roomY, player plr)
{
    int PSizeX = 40;// this is 4:3
    int PSizeY = 32;
    string perspectiveCanvas[100][100];//"__ROOM_PERSPECTIVE_GOES_HERE__";
    for (int i = 0 ; i < PSizeY ; i++)//building blank perspective
        {
            for (int j = 0 ; j < PSizeX ; j++)
            {
                perspectiveCanvas[i][j] = " . ";
            }
        }
    for (int i = 0 ; i < PSizeY ; i++)
        {
            for (int j = 0 ; j < PSizeX ; j++)
            {
                cout << perspectiveCanvas[i][j];
            }
            cout << endl;
        }

    {//for generating and displaying next room
        if(plr.directionFacing == 1 && room.childUp)
        {
            newRoom(map, "Down", mapY-1, mapX, room);
            //nextRoomPerspective(map, map[(mapY-1)][mapX], mapX, mapY, roomX, roomY, plr);
        };
        if(plr.directionFacing == 2 && room.childLeft)
        {
            newRoom(map, "Right", mapY, mapX-1, room);
            //nextRoomPerspective(map, map[(mapY)][mapX-1], mapX, mapY, roomX, roomY, plr);
        };
        if(plr.directionFacing == 3 && room.childDown)
        {
            newRoom(map, "Up", mapY+1, mapX, room);
            //nextRoomPerspective(map, map[(mapY+1)][mapX], mapX, mapY, roomX, roomY, plr);
        };
        if(plr.directionFacing == 4 && room.childRight)
        {
            newRoom(map, "Left", mapY, mapX+1, room);
            //nextRoomPerspective(map, map[(mapY)][mapX+1], mapX, mapY, roomX, roomY, plr);
        };



        //viewport rendering maths
        //finding angles for each ray
        float angle[PSizeX];
        float focalLength = 90;
        for (int column = 1; column <= PSizeX ; column+=1)
        {
            angle[column] = ((focalLength/PSizeX)*column);
            //cout << angle[column] <<"\t"<< column<< endl;//to make sure that the atan is working
        }

/*
    tracing each ray
        function ray(origin) {
        var stepX = step(sin, cos, origin.x, origin.y);
        var stepY = step(cos, sin, origin.y, origin.x, true);
        var nextStep = stepX.length2 < stepY.length2
        ? inspect(stepX, 1, 0, origin.distance, stepX.y)
        : inspect(stepY, 0, 1, origin.distance, stepY.x);

        if (nextStep.distance > range) return [origin];
        return [origin].concat(ray(nextStep));
        }*/

    //drawing col's


    }


    //IF dir == something && room XY.isrendered && there is a door in that direction THEN nextRoomPerspective
    //ELIF dir == somthing && room XY.isNotRendered && there is door
    //      {newRoom THEN next room perspective}
};
void nextRoomPerspective(room map[11][11], room room,int mapX, int mapY, int roomX, int roomY, player plr)
{
    if (mapX == -1 || mapY == -1 || mapX == 11 || mapY == 11)
    {
        cout << "attempt to render out of bounds in nextRoomPer"<< endl;// SEG FAULTING BEFORE THIS LINE
        return;// should stop out of bound recursion
    }
    else
    {//for generating and displaying next room
        if(plr.directionFacing == 1 && room.childUp)
        {
            newRoom(map, "Down", mapY-1, mapX, room);
            nextRoomPerspective(map, map[(mapY-1)][mapX], mapX, mapY, roomX, roomY, plr);
        };
        if(plr.directionFacing == 2 && room.childLeft)
        {
            newRoom(map, "Right", mapY, mapX-1, room);
            nextRoomPerspective(map, map[(mapY)][mapX-1], mapX, mapY, roomX, roomY, plr);
        };
        if(plr.directionFacing == 3 && room.childDown)
        {
            newRoom(map, "Up", mapY+1, mapX, room);
            nextRoomPerspective(map, map[(mapY+1)][mapX], mapX, mapY, roomX, roomY, plr);
        };
        if(plr.directionFacing == 4 && room.childRight)
        {
            newRoom(map, "Left", mapY, mapX+1, room);
            nextRoomPerspective(map, map[(mapY)][mapX+1], mapX, mapY, roomX, roomY, plr);
        };
    }


};
void roomPerspectiveNoDraw(room map[11][11], room room,int mapX, int mapY, int roomX, int roomY, player plr)
{
// this function is for force updating nextroom when player faces new room

    {//for generating and displaying next room
        if(plr.directionFacing == 1 && room.childUp)
        {
            newRoom(map, "Down", mapY-1, mapX, room);
            //nextRoomPerspective(map, map[(mapY-1)][mapX], mapX, mapY, roomX, roomY, plr);
        };
        if(plr.directionFacing == 2 && room.childLeft)
        {
            newRoom(map, "Right", mapY, mapX-1, room);
            //nextRoomPerspective(map, map[(mapY)][mapX-1], mapX, mapY, roomX, roomY, plr);
        };
        if(plr.directionFacing == 3 && room.childDown)
        {
            newRoom(map, "Up", mapY+1, mapX, room);
            //nextRoomPerspective(map, map[(mapY+1)][mapX], mapX, mapY, roomX, roomY, plr);
        };
        if(plr.directionFacing == 4 && room.childRight)
        {
            newRoom(map, "Left", mapY, mapX+1, room);
            //nextRoomPerspective(map, map[(mapY)][mapX+1], mapX, mapY, roomX, roomY, plr);
        };
    }


    //IF dir == something && room XY.isrendered && there is a door in that direction THEN nextRoomPerspective
    //ELIF dir == somthing && room XY.isNotRendered && there is door
    //      {newRoom THEN next room perspective}
};

/* THINGS TO FIX
    fix arrow to WASD
    win.txt ratio
    only render visible map data
    perspectives
    make perspective to negative space after implementation
*/
