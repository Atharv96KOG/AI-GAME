#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <queue>
#include <vector>
#include <cmath>
#include <algorithm> 

using namespace std;

#define MAX 10

struct Position
{
    int X;
    int Y;
    Position(int x = 0, int y = 0) : X(x), Y(y) {}
}PlayerPos, WumpusPos, GoldPos, PitPos;

char Grid[MAX][MAX];
bool Visited[MAX][MAX];

int gridSize;

// A* algorithm node structure
struct Node
{
    Position pos;
    int gCost; // Cost from the start node
    int hCost; // Heuristic cost (Manhattan distance to the goal)
    int fCost; // Total cost (gCost + hCost)
    Node* parent;

    Node(Position p, int g, int h, Node* par = nullptr)
        : pos(p), gCost(g), hCost(h), fCost(g + h), parent(par) {}

    bool operator>(const Node& other) const { return fCost > other.fCost; }
};

// Helper functions for A*
int manhattanDistance(Position a, Position b)
{
    return abs(a.X - b.X) + abs(a.Y - b.Y);
}

bool isValid(Position pos)
{
    return pos.X >= 0 && pos.X < gridSize && pos.Y >= 0 && pos.Y < gridSize;
}

bool isObstacle(Position pos)
{
    return Grid[pos.X][pos.Y] == 'W' || Grid[pos.X][pos.Y] == 'P';
}

// A* pathfinding function
vector<Position> findPath(Position start, Position goal)
{
    priority_queue<Node, vector<Node>, greater<Node>> openList;
    bool closedList[MAX][MAX] = {false};

    openList.push(Node(start, 0, manhattanDistance(start, goal)));

    while (!openList.empty())
    {
        Node current = openList.top();
        openList.pop();

        // If the goal is reached
        if (current.pos.X == goal.X && current.pos.Y == goal.Y)
        {
            vector<Position> path;
            Node* temp = &current;

            while (temp != nullptr)
            {
                path.push_back(temp->pos);
                temp = temp->parent;
            }

            reverse(path.begin(), path.end());
            return path;
        }

        closedList[current.pos.X][current.pos.Y] = true;

        // Possible moves: up, down, left, right
        vector<Position> moves = {
            Position(current.pos.X - 1, current.pos.Y),
            Position(current.pos.X + 1, current.pos.Y),
            Position(current.pos.X, current.pos.Y - 1),
            Position(current.pos.X, current.pos.Y + 1)
        };

        for (auto& move : moves)
        {
            if (isValid(move) && !isObstacle(move) && !closedList[move.X][move.Y])
            {
                int newG = current.gCost + 1;
                int newH = manhattanDistance(move, goal);

                openList.push(Node(move, newG, newH, new Node(current)));
            }
        }
    }

    return {}; // Return empty path if no path found
}

void printArt(string filename);
void printWorld();
void initialise(int hardnessLevel);
int getHardnessLevel();
void randomizeGameElements(int hardnessLevel);
void game();
void visit(int X , int Y , string *Message , bool *flag , int *score);
void devMode();
bool exiting();

int main()
{
    game();
}

void printArt(string filename)
{
    filename += ".txt";
    
    fstream newfile;

    newfile.open( filename , ios::in);

    if (newfile.is_open())
    {
        string tp;
        while (getline(newfile, tp))
        {
            cout << tp << "\n";
        }
        newfile.close();
    }

    char ch;
    getchar();
}

void initialise(int hardnessLevel)
{
    gridSize = 3 + hardnessLevel*2;

    for(int i = 0 ; i < gridSize ; i++)
    {
        for(int j = 0 ; j < gridSize ; j++)
        {
            Grid[i][j] = ' ';
        }
    }

    PlayerPos.X = gridSize-1;
    PlayerPos.Y = 0;

    randomizeGameElements(hardnessLevel);

    for(int i = 0 ; i < gridSize ; i++)
    {
        for( int j = 0 ; j < gridSize ; j++)
        {
            if(Grid[i][j] == 'W')
            {
                if( i < gridSize-1 )
                { Grid[ i + 1 ][j] = 'S'; }
                if( i > 0 )
                { Grid[ i - 1 ][j] = 'S'; }
                if( j < gridSize-1 )
                { Grid[i][ j + 1 ] = 'S'; }
                if( j > 0 )
                { Grid[i][ j - 1 ] = 'S'; }
            }
        }

        for( int j = 0 ; j < gridSize ; j++)
        {
            if(Grid[i][j] == 'P')
            {
                if( Grid[i+1][j] == 'S' )
                {
                    Grid[i+1][j] = '?';
                }
                else
                {
                    if( i < gridSize-1 )
                    { Grid[ i + 1 ][j] = 'B'; }
                }

                if( Grid[i-1][j] == 'S' )
                {
                    Grid[i-1][j] = '?';
                }
                else
                {
                    if( i > 0 )
                    { Grid[ i - 1 ][j] = 'B'; }
                }

                if( Grid[i][j+1] == 'S' )
                {
                    Grid[i][j+1] = '?';
                }
                else
                {
                    if( j < gridSize-1 )
                    { Grid[i][ j + 1 ] = 'B'; }
                }

                if( Grid[i][j-1] == 'S' )
                {
                    Grid[i][j-1] = '?';
                }
                else
                {
                    if( j > 0 )
                    { Grid[i][ j - 1 ] = 'B'; }
                }
            }
        }
    }

    for(int i = 0 ; i < gridSize ; i++)
    {
        for(int j = 0 ; j < gridSize ; j++)
        {
            Visited[i][j] = false;
        }
    }
    Visited[PlayerPos.X][PlayerPos.Y] = true;
}

int getHardnessLevel()
{
    cout << "\tChoose the hardness level:\n";
    cout << "\t1. Easy\n";
    cout << "\t2. Medium\n";
    cout << "\t3. Hard\n";
    
    int choice;
    cin >> choice;
    
    switch (choice)
    {
        case 1:
            cout << "\tYou selected Easy. In this mode, the game will be relatively easier.\n";
            return 1;
            
        case 2:
            cout << "\tYou selected Medium. In this mode, the game will have moderate difficulty.\n";
            return 2;
            
        case 3:
            cout << "\tYou selected Hard. In this mode, the game will be quite challenging.\n";
            return 3;
            
        default:
            cout << "\tInvalid choice. Please select a valid hardness level.\n";
            return getHardnessLevel();
    }
}

void randomizeGameElements(int hardnessLevel)
{
    srand(time(NULL));

    for (int i = 0; i < hardnessLevel; i++)
    {
        int x = rand() % gridSize-1;
        int y = rand() % gridSize-1;

        while (Grid[x][y] != ' ')
        {
            x = rand() % gridSize-1;
            y = rand() % gridSize-1;
        }

        PitPos.X = x;
        PitPos.Y = y;

        Grid[x][y] = 'P';
    }

    for (int i = 0; i < hardnessLevel; i++)
    {
        int x = rand() % gridSize-1;
        int y = rand() % gridSize-1;

        while (Grid[x][y] != ' ')
        {
            x = rand() % gridSize-1;
            y = rand() % gridSize-1;
        }
        
        WumpusPos.X = x;
        WumpusPos.Y = y;

        Grid[x][y] = 'W';
    }

    for (int i = 0; i < hardnessLevel; i++)
    {
        int x = rand() % gridSize-1;
        int y = rand() % gridSize-1;

        while (Grid[x][y] != ' ')
        {
            x = rand() % gridSize-1;
            y = rand() % gridSize-1;
        }

        GoldPos.X = x;
        GoldPos.Y = y;

        Grid[x][y] = 'G';
    }
}

void printWorld()
{
        cout << "\t --- " ;
        for(int i = 1 ; i < gridSize ; i++)
        { cout << "--- " ; }
        cout << endl;

        cout << "\t| " ; if(Visited[0][0] && PlayerPos.X == 0 && PlayerPos.Y == 0) { cout << '@'; } else if(Visited[0][0]) { cout << Grid[0][0]; } else { cout << "#"; }cout << " |" ;
        for(int i = 1 ; i < gridSize ; i++)
        { cout << " " ; if(Visited[0][i] && PlayerPos.X == 0 && PlayerPos.Y == i) { cout << '@'; } else if(Visited[0][i]) { cout << Grid[0][i]; } else { cout << "#"; }cout << " |" ; }
        cout << endl;

        cout << "\t --- " ;
        for(int i = 1 ; i < gridSize ; i++)
        { cout << "--- " ; }
        cout << endl;

    for(int j = 1 ; j < gridSize ; j++)
    {
        cout << "\t| " ; if(Visited[j][0] && PlayerPos.X == j && PlayerPos.Y == 0) { cout << '@'; } else if(Visited[j][0]) { cout << Grid[j][0]; } else { cout << "#"; }cout << " |" ;
        for(int i = 1 ; i < gridSize ; i++)
        { cout << " " ; if(Visited[j][i] && PlayerPos.X == j && PlayerPos.Y == i) { cout << '@'; } else if(Visited[j][i]) { cout << Grid[j][i]; } else { cout << "#"; }cout << " |" ; }
        cout << endl;

        cout << "\t --- " ;
        for(int i = 1 ; i < gridSize ; i++)
        { cout << "--- " ; }
        cout << endl;
    }
}

void game()
{
Restart:;

    string Message;
    bool flag = true;
    int Score = 0;

    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
		
    printArt("TitleScreen");
    printArt("Instructions");

    int hardnessLevel = getHardnessLevel();

    initialise(hardnessLevel);

    // A* pathfinding
    vector<Position> path = findPath(PlayerPos, GoldPos);

    for (auto& step : path)
    {
        if (!flag) break;

        system("cls");
        PlayerPos = step; // Move player to next step
        printWorld();
        cout << "\n\tScore : " << Score << endl;
        Message.clear();
        visit(PlayerPos.X, PlayerPos.Y, &Message, &flag, &Score);

        // Delay to visualize the steps
        Sleep(500);
    }

    if( exiting() )
    { goto Restart ;} 
}

void visit(int X , int Y , string *Message , bool *flag , int *Score)
{
    if( X < 0 || X > gridSize-1 || Y < 0 || Y > gridSize-1)
    {
        return;
    }
    else if( Grid[X][Y] == 'W')
    {
        printArt("Wumpus");
        *flag = false;
    }
    else if( Grid[X][Y] == 'P')
    {
        printArt("Pit");
        *flag = false;
    }
    else if( Grid[X][Y] == 'S')
    {
        *Message = "You Sense a Stench in the air. The WUMPUS is nearby";
    }
    else if( Grid[X][Y] == 'B')
    {
        *Message = "You Sense a Cold Breeze. There is a PIT Nearby";
    }
    else if( Grid[X][Y] == '?')
    {
        *Message = "You Sense a Stench in the air. The WUMPUS is nearby\nYou Sense a Cold Breeze. There is a PIT Nearby";
    }
    else if( Grid[X][Y] == 'G')
    {
        *Score += 100;
        *flag = false;
        printArt("Gold");
    }
 
    PlayerPos.X = X ; PlayerPos.Y = Y ;
    Visited[X][Y] = true;
}

void devMode()
{
    for(int i = 0 ; i < gridSize ; i++)
    {
        for(int j = 0 ; j < gridSize ; j++)
        {
            Visited[i][j] = true;
        }
    }
}

bool exiting()
{
start:

    cout << "\n\n Thank you for Playing the Game \n" ;

    char answer;

    cout << "\nDo you want to Play Again (Y/N) : " ;
    cin >> answer ;

    switch (answer)
    {
    case 'y':
    case 'Y':
            return true;
            break;

    case 'n':
    case 'N':
            return false;
            break;

    default:
            cout << "\nYou did not respond correctly...\n" ;
            goto start;
            break;
    }
}
