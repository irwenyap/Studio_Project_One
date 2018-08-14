// This is the main file for the game logic and function
//
//
#include "game.h"
#include "Framework\console.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>

using namespace std;

double  g_dElapsedTime;
double  g_dDeltaTime;
bool    g_abKeyPressed[K_COUNT];
char level1[125][125];
double startgame = 3.0;
char level2[125][125];
int level = 0;

// Game specific variables here
SGameChar   g_sChar;
SGameChar   g_sLevel2Char;
SGameChar   g_sLevel1GuardCells;
SGameChar   g_sLevel1GuardCafe;
SGameChar   g_sLevel1GuardField1;
SGameChar   g_sLevel1GuardField2;
EGAMESTATES g_eGameState = S_SPLASHSCREEN;
double  g_dBounceTime; // this is to prevent key bouncing, so we won't trigger keypresses more than once

					   // Console object
Console g_Console(300, 75, "SP1 Framework");

//--------------------------------------------------------------
// Purpose  : Initialisation function
//            Initialize variables, allocate memory, load data from file, etc. 
//            This is called once before entering into your main loop
// Input    : void
// Output   : void
//--------------------------------------------------------------
void init(void)
{
	// Set precision for floating point output
	g_dElapsedTime = 0.0;
	g_dBounceTime = 0.0;

	// sets the initial state for the game
	g_eGameState = S_SPLASHSCREEN;

	g_sChar.m_cLocation.X = 6; //g_Console.getConsoleSize().X / 2;
	g_sChar.m_cLocation.Y = 4; //g_Console.getConsoleSize().Y / 2;
	g_sLevel2Char.m_cLocation.X = 46; //g_Console.getConsoleSize().X / 2;
	g_sLevel2Char.m_cLocation.Y = 3; //g_Console.getConsoleSize().Y / 2;
	g_sLevel1GuardCells.m_cLocation.X = 3; //g_Console.getConsoleSize().X / 2;
	g_sLevel1GuardCells.m_cLocation.Y = 12; //g_Console.getConsoleSize().Y / 2;
	g_sLevel1GuardCafe.m_cLocation.X = 40; //g_Console.getConsoleSize().X / 2;
	g_sLevel1GuardCafe.m_cLocation.Y = 12; //g_Console.getConsoleSize().Y / 2;
	g_sLevel1GuardField1.m_cLocation.X = 105; //g_Console.getConsoleSize().X / 2;
	g_sLevel1GuardField1.m_cLocation.Y = 15; //g_Console.getConsoleSize().Y / 2;
	g_sLevel1GuardField2.m_cLocation.X = 115; //g_Console.getConsoleSize().X / 2;
	g_sLevel1GuardField2.m_cLocation.Y = 5; //g_Console.getConsoleSize().Y / 2;


	// sets the width, height and the font name to use in the console
	g_Console.setConsoleFont(0, 16, L"Consolas");
}

//--------------------------------------------------------------
// Purpose  : Reset before exiting the program
//            Do your clean up of memory here
//            This is called once just before the game exits
// Input    : Void
// Output   : void
//--------------------------------------------------------------
void shutdown(void)
{
	// Reset to white text on black background
	colour(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	g_Console.clearBuffer();
}

//--------------------------------------------------------------
// Purpose  : Getting all the key press states
//            This function checks if any key had been pressed since the last time we checked
//            If a key is pressed, the value for that particular key will be true
//
//            Add more keys to the enum in game.h if you need to detect more keys
//            To get other VK key defines, right click on the VK define (e.g. VK_UP) and choose "Go To Definition" 
//            For Alphanumeric keys, the values are their ascii values (uppercase).
// Input    : Void
// Output   : void
//--------------------------------------------------------------
void getInput(void)
{
	g_abKeyPressed[K_UP] = isKeyPressed(VK_UP);
	g_abKeyPressed[W] = isKeyPressed(0x57);
	g_abKeyPressed[K_DOWN] = isKeyPressed(VK_DOWN);
	g_abKeyPressed[S] = isKeyPressed(0x53);
	g_abKeyPressed[K_LEFT] = isKeyPressed(VK_LEFT);
	g_abKeyPressed[A] = isKeyPressed(0x41);
	g_abKeyPressed[K_RIGHT] = isKeyPressed(VK_RIGHT);
	g_abKeyPressed[D] = isKeyPressed(0x44);
	g_abKeyPressed[K_SPACE] = isKeyPressed(VK_SPACE);
	g_abKeyPressed[K_ESCAPE] = isKeyPressed(VK_ESCAPE);
}

//--------------------------------------------------------------
// Purpose  : Update function
//            This is the update function
//            double dt - This is the amount of time in seconds since the previous call was made
//
//            Game logic should be done here.
//            Such as collision checks, determining the position of your game characters, status updates, etc
//            If there are any calls to write to the console here, then you are doing it wrong.
//
//            If your game has multiple states, you should determine the current state, and call the relevant function here.
//
// Input    : dt = deltatime
// Output   : void
//--------------------------------------------------------------
void update(double dt)
{
	// get the delta time
	g_dElapsedTime += dt;
	g_dDeltaTime = dt;

	switch (g_eGameState)
	{
	case S_SPLASHSCREEN: splashScreenWait(); // game logic for the splash screen
		break;
	case S_GAME: gameplay(); // gameplay logic when we are in the game
		break;
	}
}
//--------------------------------------------------------------
// Purpose  : Render function is to update the console screen
//            At this point, you should know exactly what to draw onto the screen.
//            Just draw it!
//            To get an idea of the values for colours, look at console.h and the URL listed there
// Input    : void
// Output   : void
//--------------------------------------------------------------
void render()
{
	clearScreen();      // clears the current screen and draw from scratch 
	switch (g_eGameState)
	{
	case S_SPLASHSCREEN: renderSplashScreen();
		break;
	case S_GAME: renderGame();
		break;
	}
	renderFramerate();  // renders debug information, frame rate, elapsed time, etc
	renderToScreen();   // dump the contents of the buffer to the screen, one frame worth of game
}

void splashScreenWait()    // waits for time to pass in splash screen
{
	if (g_dElapsedTime > startgame) // wait for 3 seconds to switch to game mode, else do nothing
		g_eGameState = S_GAME;
}

void gameplay()            // gameplay logic
{
	processUserInput(); // checks if you should change states or do something else with the game, e.g. pause, exit
	moveCharacter();    // moves the character, collision detection, physics, etc
						// sound can be played here too.
}

void moveCharacter()
{
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime)
		return;

	// Updating the location of the character based on the key press
	// providing a beep sound whenver we shift the character
	//Level 1
	if ((g_abKeyPressed[K_UP] || g_abKeyPressed[W]) && level1[g_sChar.m_cLocation.Y - 1][g_sChar.m_cLocation.X ] == 'x') //To move up checking
	{
		//Beep(1440, 30);
		g_sChar.m_cLocation.Y--;
		bSomethingHappened = true;

	}
	if ((g_abKeyPressed[K_LEFT] || g_abKeyPressed[A]) && g_sChar.m_cLocation.X > 0 && level1[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X - 1] == 'x')
	{
		//Beep(1440, 30);
		g_sChar.m_cLocation.X--;
		bSomethingHappened = true;
	}
	if ((g_abKeyPressed[K_DOWN] || g_abKeyPressed[S]) && g_sChar.m_cLocation.Y < g_Console.getConsoleSize().Y - 1 && g_sChar.m_cLocation.X > 0 && level1[g_sChar.m_cLocation.Y + 1][g_sChar.m_cLocation.X] == 'x')
	{
		//Beep(1440, 30);
		g_sChar.m_cLocation.Y++;
		bSomethingHappened = true;
	}
	if ((g_abKeyPressed[K_RIGHT] || g_abKeyPressed[D]) && g_sChar.m_cLocation.X < g_Console.getConsoleSize().X - 1 && g_sChar.m_cLocation.X > 0 && level1[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X + 1] == 'x')
	{
		//Beep(1440, 30);
		g_sChar.m_cLocation.X++;
		bSomethingHappened = true;
	}

	//Level 2

	if ((g_abKeyPressed[K_UP] || g_abKeyPressed[W]) && level1[g_sLevel2Char.m_cLocation.Y - 1][g_sLevel2Char.m_cLocation.X] == 'x') //To move up checking
	{
		//Beep(1440, 30);
		g_sLevel2Char.m_cLocation.Y--;
		bSomethingHappened = true;

	}
	if ((g_abKeyPressed[K_LEFT] || g_abKeyPressed[A]) && level1[g_sLevel2Char.m_cLocation.Y][g_sLevel2Char.m_cLocation.X - 1] == 'x')
	{
		//Beep(1440, 30);
		g_sLevel2Char.m_cLocation.X--;
		bSomethingHappened = true;
	}
	if ((g_abKeyPressed[K_DOWN] || g_abKeyPressed[S]) && level1[g_sLevel2Char.m_cLocation.Y + 1][g_sLevel2Char.m_cLocation.X] == 'x')
	{
		//Beep(1440, 30);
		g_sLevel2Char.m_cLocation.Y++;
		bSomethingHappened = true;
	}
	if ((g_abKeyPressed[K_RIGHT] || g_abKeyPressed[D]) && level1[g_sLevel2Char.m_cLocation.Y][g_sLevel2Char.m_cLocation.X + 1] == 'x')
	{
		//Beep(1440, 30);
		g_sLevel2Char.m_cLocation.X++;
		bSomethingHappened = true;
	}


	if (g_abKeyPressed[K_SPACE])
	{
		bSomethingHappened = true;
	}

	if (bSomethingHappened)
	{
		// set the bounce time to some time in the future to prevent accidental triggers
		g_dBounceTime = g_dElapsedTime + 0.125; // 125ms should be enough
	}
}

void processUserInput()
{
	// quits the game if player hits the escape key
	if (g_abKeyPressed[K_ESCAPE])
		g_bQuitGame = true;
}

void clearScreen()
{
	// Clears the buffer with this colour attribute
	g_Console.clearBuffer();
}

void renderSplashScreen()  // renders the splash screen
{
	COORD c = g_Console.getConsoleSize();
	string line;
	ifstream myfile("titlescreen.txt");

	c.X = 1;
	c.Y = 1;

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			for (int col = 0; col < line.size(); col++)
			{
				if (line[col] == '#')
				{
					line[col] = 219;
				}
				g_Console.writeToBuffer(c, line[col], 0x03);
				c.X++;
			}
			c.Y++;
			c.X = 1;
		}
	}
}

void renderGame()
{
	renderTutorialMap(); // renders the map to the buffer first
	renderCharacter();   // renders the character into the buffer
}

void renderTutorialMap()
{
	string line;
	COORD c;
	ifstream myfile("map_tutorial.txt");

	c.X = 1;
	c.Y = 1;

	int x = 1;
	int y = 1;
	level = 1;

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			for (int col = 0; col < line.size(); col++)
			{
				if (line[col] == '#')
				{
					line[col] = 205;
				}
				if (line[col] == '*')
				{
					line[col] = 186;
				}
				if (line[col] == 'H')
				{
					line[col] = 219;
				}
				if (line[col] == 'A')
				{
					line[col] = 185;
				}
				if (line[col] == 'B')
				{
					line[col] = 204;
				}
				if (line[col] == 'C')
				{
					line[col] = 201;
				}
				if (line[col] == 'D')
				{
					line[col] = 187;
				}
				if (line[col] == 'E')
				{
					line[col] = 203;
				}
				if (line[col] == 'F')
				{
					line[col] = 202;
				}
				if (line[col] == 'G')
				{
					line[col] = 200;
				}
				if (line[col] == 'I')
				{
					line[col] = 188;
				}
				if (line[col] == '!')
				{
					line[col] = 176;
				}
				if (line[col] == ' ')
				{
					level1[x][y] = 'x';
				}
				g_Console.writeToBuffer(c, line[col], 0x03);
				c.X++;
				y++;
			}
			x++;
			y = 1;
			c.Y++;
			c.X = 1;
		}
	}
}

void renderBronzeMap()
{
	string line;
	COORD c;
	ifstream myfile("map_bronze.txt");

	c.X = 1;
	c.Y = 1;

	int x = 1;
	int y = 1;
	level = 2;

	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			for (int col = 0; col < line.size(); col++)
			{
				if (line[col] == '#')
				{
					line[col] = 205;
				}
				if (line[col] == '*')
				{
					line[col] = 186;
				}
				if (line[col] == 'H')
				{
					line[col] = 219;
				}
				if (line[col] == 'A')
				{
					line[col] = 185;
				}
				if (line[col] == 'B')
				{
					line[col] = 204;
				}
				if (line[col] == 'C')
				{
					line[col] = 201;
				}
				if (line[col] == 'D')
				{
					line[col] = 187;
				}
				if (line[col] == 'E')
				{
					line[col] = 203;
				}
				if (line[col] == 'F')
				{
					line[col] = 202;
				}
				if (line[col] == 'G')
				{
					line[col] = 200;
				}
				if (line[col] == 'I')
				{
					line[col] = 188;
				}
				if (line[col] == '!')
				{
					line[col] = 176;
				}
				if (line[col] == '+')
				{
					line[col] = 206;
				}
				if (line[col] == ' ')
				{
					level1[x][y] = 'x';
				}
				g_Console.writeToBuffer(c, line[col], 0x03);
				c.X++;
				y++;
			}
			x++;
			y = 1;
			c.Y++;
			c.X = 1;
		}
	}
}

void renderCharacter()
{
	// Draw the location of the character
	if (level == 1)
	{
		g_Console.writeToBuffer(g_sChar.m_cLocation, (char)1, 0x0C);
		g_Console.writeToBuffer(g_sLevel1GuardCells.m_cLocation, (char)2, 0X0F);
		g_Console.writeToBuffer(g_sLevel1GuardCafe.m_cLocation, (char)2, 0X0F);
		g_Console.writeToBuffer(g_sLevel1GuardField1.m_cLocation, (char)2, 0X0F);
		g_Console.writeToBuffer(g_sLevel1GuardField2.m_cLocation, (char)2, 0X0F);
	}
	if (level == 2)
	{
		g_Console.writeToBuffer(g_sLevel2Char.m_cLocation, (char)1, 0x0C);
	}

}

void characterInteraction()
{
	
}

void renderFramerate()
{
	COORD c;
	// displays the framerate
	std::ostringstream ss;
	ss << std::fixed << std::setprecision(3);
	ss << 1.0 / g_dDeltaTime << "fps";
	c.X = 0;
	c.Y = 0;
	g_Console.writeToBuffer(c, ss.str());
}

void renderToScreen()
{
	// Writes the buffer to the console, hence you will see what you have written
	g_Console.flushBufferToConsole();
}

