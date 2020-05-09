/*
 * MODELOWANIE I SYMULACJE KOMPUTEROWE
 *
 * AUTHOR: Piotr Kosek (student - Faculty of Physics and Applied Informatics, University of Lodz)
 * CONTACT: piotr-kosek@tlen.pl
 * LICENSE: X11 License (MIT)
 * VERSION: 0.1 (2015/12/17)
 * __________________________________________________________________________________________________
 *
 * CONSTRAINS:
 *		o Single file
 *		o Mersenne-twister random algorithm
 *		o Chooseable excersize
 *		o All variables editable from within running application
 *		o Compileable with Dev-Cpp 4.9.9.2
 * __________________________________________________________________________________________________
 *
 * LIMITATION:
 *		o Doesn't use C++ '11 (limitation of Dev-Cpp 4.9.9.2)
 *		o Requires Windows to compile (using Windows-specific api for moving cursor in console)  
 * __________________________________________________________________________________________________
 *
 * SECTIONS:
 *		o Standard includes
 *		o Mersenne-twister random class
 *		o Helper random functions
 *		o Global draw functions
 *		o Histogram draw functions
 *		o Pages
 *		o Main function
 * __________________________________________________________________________________________________
 *
 * Copyright (c) 2015 Piotr Kosek
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 * A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 * __________________________________________________________________________________________________
 *
 * CHANGELOG:
 *		o 2015/12/17 - initial version
 * __________________________________________________________________________________________________
 *
 */
 
/////////////////////////////////////////////////////////////////////////////////////////////////////
// Standard includes
 
#include <conio.h>
#include <ctime>
#include <cstdarg>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <iostream> 
#include <string>
#include <vector>    
#include <windows.h> // For SetConsoleCursorPosition and so on
#include <fcntl.h>
#include <io.h>

using namespace std;
 
/////////////////////////////////////////////////////////////////////////////////////////////////////
// Mersenne-twister random class
        
// Following is direct implementation of http://pl.wikipedia.org/wiki/Mersenne_Twister

class Random
{

public:

	Random()
	{
		index = 0;
		Seed((unsigned int)time(NULL));
	}
	Random(unsigned int seed)
	{
		index = 0;
		Seed(seed);
	}
	~Random()
	{
	
	}

	void Seed(unsigned int seed)
	{
		MT[0] = seed;
		for(unsigned int i = 1; i < 624; i++)
		{
			MT[i] = 0xFFFFFFFFul & (1812433253ul * (MT[i-1] ^ ((MT[i-1] >> 30))) + i);
		}
	}

	unsigned int NextInt()
	{
		if(index == 0)
		{
			generateNumbers();
		}
	
		unsigned int y = MT[index];
		y = y ^ (y >> 11);
		y = y ^ ((y << 7) & (2636928640ul)); // 0x9d2c5680
		y = y ^ ((y << 15) & (4022730752ul)); // 0xefc60000
		y = y ^ (y >> 18);
	
		index = (index + 1) % 624;
		return y;
	}
	double NextDouble()
	{
		return ((double)((unsigned int)NextInt()%0x7fffffff) / (double)((unsigned int)0x7fffffff));
	}

private:

	unsigned int MT[624];
	unsigned int index;

	void generateNumbers()
	{
		for(unsigned int i = 0; i < 624; i++)
		{
			unsigned int y = (0xFFFFFFFFul & MT[i]) + (0x7FFFFFFFul & (MT[(i+1) % 624]));
			MT[i] = MT[(i + 397) % 624] ^ (y >> 1);
			if ((y % 2) == 1) // y is odd
			{
				MT[i] = MT[i] ^ (2567483615ul); // 0x9908b0df
			}
		}
	}

};

Random Rand; // for global access

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Helper random functions

typedef float (*MonteCarloFunc)(float);

float Zad306(float x)
{
	if(x >= 0.0f && x < 1.0f) return exp(x);
	if(x >= 1.0f && x < 3.0f) return 2.0f*x-1.0f;
	if(x >= 3.0f && x < 4.0f) return -0.5f * x * x + 3.0f * x;
	return 0.0f;
}

inline int Roll(int min, int max) { return ((Rand.NextInt() % (max - min + 1)) + min); }
inline float RollF(float min, float max) { return Rand.NextDouble() * (max - min) + min; }
inline float RollF() { return Rand.NextDouble(); }
inline int d(int x) { return Roll(1, x); }
inline int fact(int x) { int ret = 1; for(int i = 2; i < x; i++) { ret *= i; } return ret; }
inline float MonteCarlo(MonteCarloFunc func, float x1, float x2, float y1, float y2)
{
	for(;;)
	{
		float f = RollF(x1, x2);
		if(RollF(y1, y2) <= func(f))
			return f;
	}
}
inline float ExpRand(float av) { return -av * log(RollF()); }
inline float Gauss(float av, float std)
{
	return sqrt(-2*log(RollF()))*cos(2*M_PI*RollF()) * std + av;
}
inline int Poisson(float av)
{
	long double L = exp(-av);
	int k = 0;
	long double p = 1.0;
	do 
	{
		k++;
		long double u = RollF();
		p = p * u;
	} while (p > L);
	return k - 1;
}
inline int GaltonBox(int c)
{
	int ret = 0;
	for(int i = 0; i < c; i++)
		ret += d(2) - 1;
	return ret;
}
#define d2 d(2)
#define d3 d(3)
#define d4 d(4)
#define d6 d(6)
#define d8 d(8)
#define d10 d(10)
#define d12 d(12)
#define d20 d(20)
#define d30 d(30)
#define d100 d(100)         

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Global draw functions

HANDLE hBuffer; 
HANDLE hBufferPrev;

void Init()
{	
	srand(time(NULL));
	setlocale(LC_ALL, "C");
	
	HANDLE hConsoleOutput;
	CONSOLE_CURSOR_INFO structCursorInfo;
	
	hConsoleOutput = GetStdHandle( STD_OUTPUT_HANDLE );
	structCursorInfo.dwSize = 1;
	structCursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo( hConsoleOutput, &structCursorInfo );   
    /*                       		
    SetConsoleCP(437);       		// PK: What the fuck with these!?
	SetConsoleOutputCP(437);        // ... does not work, when uncommented o.O
	system("CHCP 437");
	*/     
	SetConsoleTitle(TEXT("Piotr Kosek app for MiS"));
	
	hBuffer = CreateConsoleScreenBuffer( GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ |  FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);  
	hBufferPrev = CreateConsoleScreenBuffer( GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ |  FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);  
	SetConsoleActiveScreenBuffer(hBuffer);  
	
    SetConsoleCursorInfo( hBuffer, &structCursorInfo );   
    SetConsoleCursorInfo( hBufferPrev, &structCursorInfo ); 
}    

COORD GetConsoleSize()
{
	/*
	COORD ret = { 80, 25 };
	return ret;
	*/
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( GetStdHandle(STD_OUTPUT_HANDLE), &csbi );
	COORD ret = { csbi.srWindow.Right - csbi.srWindow.Left + 1, csbi.srWindow.Bottom - csbi.srWindow.Top + 1 };
	return ret;
}

#define KEY_ESCAPE  0
#define KEY_ENTER   1
 
#define KEY_INSERT  2
#define KEY_HOME    3
#define KEY_PGUP    4
#define KEY_DELETE  5
#define KEY_END     6
#define KEY_PGDOWN  7
 
#define KEY_UP      14
#define KEY_DOWN    15
#define KEY_LEFT    16
#define KEY_RIGHT   17
 
#define KEY_F1      18
#define KEY_F2      19
#define KEY_F3      20
#define KEY_F4      21
#define KEY_F5      22
#define KEY_F6      23
#define KEY_F7      24
#define KEY_F8      25
#define KEY_F9      26
#define KEY_F10     27
#define KEY_F11     28
#define KEY_F12     29
 
#define KEY_NUMDEL  30
#define KEY_NUMPAD0 31
#define KEY_NUMPAD1 127
#define KEY_NUMPAD2 128
#define KEY_NUMPAD3 129
#define KEY_NUMPAD4 130
#define KEY_NUMPAD5 131
#define KEY_NUMPAD6 132
#define KEY_NUMPAD7 133
#define KEY_NUMPAD8 134
#define KEY_NUMPAD9 135
 
unsigned int GetCh(void)
{
    unsigned int f[12]={KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_F11,KEY_F12};
    unsigned int key;
 
    key=getch();
    while(GetKeyState(VK_LCONTROL)&0x8000 && GetKeyState(VK_RCONTROL)&0x8000) continue;
 
    switch(key)
    {
        case 0:
        {
            int k;
 
            switch((k=getch()))
            {
                case 71: return KEY_NUMPAD7;
                case 72: return KEY_NUMPAD8;
                case 73: return KEY_NUMPAD9;
                case 75: return KEY_NUMPAD4;
                case 77: return KEY_NUMPAD6;
                case 79: return KEY_NUMPAD1;
                case 80: return KEY_NUMPAD4;
                case 81: return KEY_NUMPAD3;
                case 82: return KEY_NUMPAD0;
                case 83: return KEY_NUMDEL;
                default: return f[k-59];
            }
        }
        case 224:
        {
            int k;
 
            switch((k=getch()))
            {
                case 71: return KEY_HOME;
                case 72: return KEY_UP;
                case 73: return KEY_PGUP;
                case 75: return KEY_LEFT;
                case 77: return KEY_RIGHT;
                case 79: return KEY_END;
                case 80: return KEY_DOWN;
                case 81: return KEY_PGDOWN;
                case 82: return KEY_INSERT;
                case 83: return KEY_DELETE;
                default: return f[k-123];
            }
        }
        case 13: return KEY_ENTER;
        case 27: return KEY_ESCAPE;
        default: return key;
    }
}

std::string GetStr()
{
	COORD cConSize = GetConsoleSize();          
	DWORD dwConSize = cConSize.X * cConSize.Y;  
    
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
	     
	char * buffer = new char[dwConSize + 1];                     
	gets(buffer);  	
	std::string ret = buffer;
    delete[] buffer;
    
    return ret;
}    

std::string GetStr(int y, int x, int size, int color)
{
	COORD cConSize = GetConsoleSize();          
	DWORD dwConSize = cConSize.X * cConSize.Y;  
	DWORD dwColor = color & (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY); 
	HANDLE hConsole = hBufferPrev;  
	COORD coordScreen = { x, y };   
	DWORD cCharsWritten;
    FillConsoleOutputAttribute( hConsole, dwColor, size, coordScreen, &cCharsWritten );
    
    HANDLE hStdin = hConsole; 
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode | ENABLE_ECHO_INPUT);   
    
    SetConsoleCursorPosition( hConsole, coordScreen );
	     
	char * buffer = new char[dwConSize + 1];                     
	gets(buffer);  	
	std::string ret = buffer;
    delete[] buffer;
    
    return ret;
}

void ClearScreen(int backgroundColor = 0) // Direct implementation of https://support.microsoft.com/en-us/kb/99261
{
	HANDLE hConsole = hBuffer; // GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = { 0, 0 };
	COORD cConSize = GetConsoleSize();
	DWORD dwConSize = cConSize.X * cConSize.Y;
	DWORD cCharsWritten;
	FillConsoleOutputCharacter( hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten );
	FillConsoleOutputAttribute( hConsole, backgroundColor & (BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY), dwConSize, coordScreen, &cCharsWritten );
}

void Printf(int y, int x, int w, int color, std::string format, ...)
{      
	HANDLE hConsole = hBuffer; // GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwColor = color & (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY);
	COORD cConSize = GetConsoleSize();
	DWORD dwConSize = cConSize.X * cConSize.Y;
	DWORD cCharsWritten;
	char * buffer = new char[dwConSize + 1];
	
	va_list args;
    va_start( args, format );
    vsnprintf (buffer, dwConSize + 1, format.c_str(), args);
    va_end( args );
    
    
    int len = strlen(buffer);
    int posX = x + (w - len + 1) / 2; 
	COORD coordScreen = { x, y };      
	COORD coordPosScreen = { posX, y };
            
    SetConsoleCursorPosition( hConsole, coordPosScreen );
    WriteFile(hConsole, buffer, strlen(buffer), &cCharsWritten, NULL);
    FillConsoleOutputAttribute( hConsole, dwColor, w, coordScreen, &cCharsWritten );
    
    delete[] buffer;
}    
   
void Printf(int y, int x, int color, std::string format, ...)
{       
	COORD cConSize = GetConsoleSize();
	DWORD dwConSize = cConSize.X * cConSize.Y;
	char * buffer = new char[dwConSize + 1];
	
	va_list args;
    va_start( args, format );
    vsnprintf (buffer, dwConSize + 1, format.c_str(), args);
    va_end( args ); 
    
    Printf(y, x, strlen(buffer), color, buffer);
    
    delete[] buffer;
}  

void DrawWindow(int y, int x, int w, int h, int color, std::string heading = "")
{
	Printf(y, x, color, "%c", 218); 
	Printf(y, x + w - 1, color, "%c", 191);  
	Printf(y + h - 1, x, color, "%c", 192);   
	Printf(y + h - 1, x + w - 1, color, "%c", 217);
	for(int i = y + 1; i < y + h - 1; i++)
	{
		COORD coordScreen = { x, i };       
		DWORD cCharsWritten;  
		FillConsoleOutputCharacter( hBuffer, (TCHAR) ' ', w, coordScreen, &cCharsWritten );
    	FillConsoleOutputAttribute( hBuffer, color, w, coordScreen, &cCharsWritten ); 
    	
		Printf(i, x, color, "%c", 179);	 
		Printf(i, x + w - 1, color, "%c", 179); 
	}  
	for(int i = x + 1; i < x + w - 1; i++)
	{
		Printf(y, i, color, "%c", 196);	 
		Printf(y + h - 1, i, color, "%c", 196);	 
	}  
	
	if(!heading.empty())
	{
		Printf(y, x, w, color, "[%s]", heading.c_str()); 
	}
}

std::string FormatInt(int i, bool isFactor = false)
{
	char buff[64];
	
	int in = i;
	char min[2] = "";
	if(in < 0)
	{
		in = -in;
		strcpy(min, "-");
	}

	if(isFactor)
	{
		if(in == 0) return "0.0";
		else if(in < 10) sprintf(buff, "%s0.0%d", min, in);
		else if(in < 100) sprintf(buff, "%s0.%d", min, in);
		else sprintf(buff, "%s%d.%02d", min, in / 100, in % 100);
	}
	else
	{
		if(in == 0) return "0";
		else if(in < 100) sprintf(buff, "%s%d", min, in);
		else if(in < 1000) sprintf(buff, "%s%d", min, in - (in % 10));

		else if(in < 10000) sprintf(buff, "%s%d.%dk", min, in / 1000, (in / 100) % 10);
		else if(in < 100000) sprintf(buff, "%s%dk", min, in / 1000);
		else if(in < 1000000) sprintf(buff, "%s%d0k", min, in / 10000);

		else if(in < 10000000) sprintf(buff, "%s%d.%dm", min, in / 1000000, (in / 100000) % 10);
		else if(in < 100000000) sprintf(buff, "%s%dm", min, in / 1000000);
		else if(in < 1000000000) sprintf(buff, "%s%d0m", min, in / 10000000);

		else sprintf(buff, "%s1G+", min);
	}

	return buff;
}     

std::string FormatFloat(double f)
{
	return FormatInt((int)(f * 100.0), true);	
}  

void DrawCopyright()
{
	COORD size = GetConsoleSize();

	std::string author = "App for MiS ver 1.0 by Piotr Kosek";
	Printf(size.Y - 2, size.X - author.length(), FOREGROUND_GREEN, author.c_str());
}

void Publish()
{
	COORD pos = { 0, 0 } ;	 
	SetConsoleActiveScreenBuffer(hBuffer); 
	SetConsoleCursorPosition( hBuffer, pos );
	                          
    COORD coordBufSize = GetConsoleSize();    
    CHAR_INFO * chiBuffer = new CHAR_INFO[coordBufSize.X * coordBufSize.Y];        
	SMALL_RECT srctRect;
	srctRect.Top = 0;
    srctRect.Left = 0; 
    srctRect.Bottom = coordBufSize.Y - 1; 
    srctRect.Right = coordBufSize.X - 1; 
    COORD coordBufCoord = { 0, 0 }; 
    
    ReadConsoleOutput(hBuffer, chiBuffer, coordBufSize, coordBufCoord, &srctRect);
    WriteConsoleOutput(hBufferPrev, chiBuffer, coordBufSize, coordBufCoord, &srctRect);
                        
    delete[] chiBuffer;  
	std::swap(hBuffer, hBufferPrev);
}      

void ErrorBox(std::string text, std::string line2 = "")
{
	COORD size = GetConsoleSize();  
	int color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_INTENSITY;
	DrawWindow(size.Y / 4, size.X / 4, size.X / 2, size.Y / 2, color, "ERROR");
	Printf(size.Y / 2 - 3, size.X / 4 + 1, size.X / 2 - 2, color, text.c_str());  
	Printf(size.Y / 2 - 2, size.X / 4 + 1, size.X / 2 - 2, color, line2.c_str()); 
	Printf(size.Y / 2, size.X / 4 + 1, size.X / 2 - 2, color, "Press any key to try again");
	Publish();
	GetCh();
}                 

int OptionBox(std::string heading, const char * options, int defOption = 0)
{
	std::vector<std::string> opts;
	
    for(const char* p=options; *p; p+=opts.back().size()+1)
        opts.push_back(p);
        
	int longest = heading.size() + 2;
	for(int i = 0; i < opts.size(); i++)
	{
		int len = opts[i].size();
		if(len > longest)
			longest = len;
	}
	
	int option = defOption;
	
	for(;;)
	{   
		COORD size = GetConsoleSize();
		DrawWindow((size.Y - 4 - opts.size()) / 2, (size.X - 4 - longest) / 2, longest + 4, 4 + opts.size(), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY, heading.c_str());
		for(int i = 0; i < opts.size(); i++)
		{
			if(option == i)
				Printf((size.Y - 4 - opts.size()) / 2 + 2 + i, (size.X - 4 - longest) / 2 + 2, longest, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY, opts[i].c_str());
			else    
				Printf((size.Y - 4 - opts.size()) / 2 + 2 + i, (size.X - 4 - longest) / 2 + 2, longest, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED, opts[i].c_str());
		} 
		Publish();
		int c = GetCh();
		if(c == KEY_DOWN)
			option = (option + 1) % opts.size();
		else if(c == KEY_UP)  
			option = (option + opts.size() - 1) % opts.size();
		else if(c == KEY_ENTER)
			return option;
	}  
}

int InputInteger(std::string heading)
{
	COORD size = GetConsoleSize();
	for(;;)
	{
		DrawWindow((size.Y - 5) / 2, (size.X - 4 - heading.size()) / 2, heading.size() + 4, 5, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY, heading.c_str());
		Publish();
		std::string str = GetStr((size.Y - 5) / 2 + 2, (size.X - 4 - heading.size()) / 2 + 2, heading.size() + 2, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		if(str.find_first_not_of("-0123456789") == std::string::npos)
		{
			return atoi(str.c_str());	
		}
		
		ErrorBox("ERROR: Incorect variable", "Please input number");
	}
}  

double InputDouble(std::string heading)
{
	COORD size = GetConsoleSize();
	for(;;)
	{
		DrawWindow((size.Y - 5) / 2, (size.X - 4 - heading.size()) / 2, heading.size() + 4, 5, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY, heading.c_str());
		Publish();
		std::string str = GetStr((size.Y - 5) / 2 + 2, (size.X - 4 - heading.size()) / 2 + 2, heading.size() + 2, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		if(str.find_first_not_of("-0123456789.") == std::string::npos)
		{
			return atof(str.c_str());	
		}
			
		ErrorBox("ERROR: Incorect variable", "Please input floating point number");
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Histogram draw functions

class Histogram
{
public:
	
	Histogram()
	{
		this->heading = "Empty";
		this->min = 0;
		this->max = 1;
		this->cubes.clear();
		this->average = 0.5f;	
		this->standardDeviation = 0.0f;
	}
	
	enum FactorialType
	{
		FactorialNone,
		FactorialHidden,
		FactorialVisible	
	};
	
	Histogram(std::string heading, std::vector<int> & variables, int min, int max, FactorialType factorial)
	{
		Regen(heading, variables, min, max, factorial);	
	}
	
	void Regen(std::string heading, std::vector<int> & variables, int min, int max, FactorialType factorial)
	{
		int facInt = (factorial == FactorialHidden) ? 100 : 1;
		this->heading = heading;
		this->min = min / facInt;
		this->max = max / facInt;
		
		this->cubes.clear();
		this->cubes.resize(this->max - this->min + 3);
		long double sum = 0;
		long double fac = (factorial == FactorialHidden) ? 100.0 : 1.0;   
		long double fac2 = (factorial == FactorialNone) ? 1.0 : 100.0;
		for(int i = 0; i < variables.size(); i++)
		{
			int var = (int)floor(variables[i] / fac);
			sum += variables[i] / fac2;
			if(var < this->min)
				cubes[0]++;
			else if(var > this->max)
				cubes[cubes.size() - 1]++;
			else
				cubes[var - this->min + 1]++;
		}                                                               
		average = sum / (long double)variables.size();   
		long double sumQuad = 0;  
		for(int i = 0; i < variables.size(); i++)
		{         
			long double var = variables[i] / fac2;
			sumQuad += (var - average) * (var - average);
		}
		standardDeviation = sumQuad / (long double)(variables.size());
		standardDeviation = sqrt(standardDeviation);
	}
	
	void Draw(FactorialType factorial)
	{
		COORD size = GetConsoleSize();
	
		ClearScreen();
		Printf(0, 0, size.X, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, heading);
	
		int legendMax = 0;
		bool wasSet = false;
			
		for(int x = 0; x < cubes.size(); x++)
		{
			if(!wasSet || cubes[x] > legendMax)
			{
				wasSet = true;
				legendMax = cubes[x];
			}
		}
	
		std::vector<std::string> legend;
		int legendCount = size.Y - 5;
		int legendStart = 2;
		int legendLength = 0;   
	
		for(int i = 0; i < legendCount; i++)
		{
			int legendVal = (legendCount - i - 1) * legendMax / (legendCount - 1);
			legend.push_back(FormatInt(legendVal));
			if(i == 0 || legend.back().length() > legendLength)
				legendLength = legend.back().length();
		}
		for(int y = 0; y < legendCount; y+=2)
		{
			Printf(y + legendStart, legendLength - legend[y].length(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, legend[y].c_str());
		}
	
		int cubesLegendSize = size.X - legendLength - 1;
		int cubesLegendMaxSize = 2;
	
		for(int x = min; x <= max; x++)
		{
			int len = FormatInt(x, factorial == FactorialVisible).length() + 1;
			if(len > cubesLegendMaxSize) cubesLegendMaxSize = len;
		}
	
		double cubesLegendLength = ((double)cubesLegendSize) / (max - min + 3);
		int cubesLegendSkip = 1;
		while(cubesLegendLength * cubesLegendSkip < cubesLegendMaxSize)
		{
			cubesLegendSkip++;
		}
	
		for(int x = min - 1; x <= max + 1; x+=cubesLegendSkip)
		{
			std::string legend;
			if(x == min - 1) legend = "<";
			else if(x+cubesLegendSkip > max + 1) legend = ">";
			else legend = FormatInt(x, factorial == FactorialVisible);
	
			Printf(size.Y - 3, (int)(legendLength + (x - min + 1) * cubesLegendLength + 1 + (cubesLegendLength - (int)legend.length()) / 2), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, legend.c_str());
		}
	
		for(int x = 0; x < size.X - legendLength - 1; x++)
		{
			for(int y = 0; y < size.Y - 5; y++)
			{
				int v = cubes[(int)(x / cubesLegendLength)];
				if(v * (size.Y - 5) / legendMax > y)
					Printf(size.Y - 4 - y, x + legendLength, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY, " ");
			}
		}
	
		DrawCopyright();
		Printf(size.Y - 2, 0, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Av: %s   Std-Dev: %s", FormatFloat(average).c_str(), FormatFloat(standardDeviation).c_str());
	}
	
private:
	std::string heading;
	std::vector<int> cubes;
	int min;
	int max;
	
	long double average;
	long double standardDeviation;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Pages

class Page
{

public:

	static void ShowPage(int page)
	{
		if(page < 0 || page >= pages.size())
		{          
			COORD size = GetConsoleSize();
			ClearScreen();
			
			int color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_INTENSITY;
			                        
			ErrorBox("ERROR: Outside page bound. WTF?!");
            
			DrawCopyright();
			Publish();
			return;
		}
		else
		{          
			ClearScreen();
			
			pages[page]->Draw();
			
			DrawCopyright();
			DrawPageTurn(pages[page]->help);
			Publish();
		}
	}
	static void PageInput(int page, int ch)
	{
		if(page >= 0 && page < pages.size())
			pages[page]->UpdateInput(ch);
	}
	static int GetPagesCount()
	{
		return pages.size();
	}
	static void ReRollAll()
	{
		for(int i = 0; i < pages.size(); i++)
		{
			pages[i]->ReRoll();
		}
	}

protected:

	Page()
	{
		pages.push_back(this);
	}        
	static void DrawPageTurn(std::string help = "")
	{
		COORD size = GetConsoleSize();
		
		Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Navigation: LEFT/RIGHT arrows. Quit: Q. Re-roll: F5. %s", help.c_str());  
	}
	std::string help;

private:

	virtual void ReRoll() = 0;
	virtual void Draw() = 0;
	virtual void UpdateInput(int ch) { }

	static std::vector<Page*> pages;
};

std::vector<Page*> Page::pages;  

class PageRoll : public Page
{
public:
	
	PageRoll() : mode(INVALID)
	{
		help = "Change function/count: F3";
		DoMultiRandom(1, 6, 2, 10000);
	}
	
	void DoSingleRandom(int min, int max, int count)
	{
		this->mode = SINGLE_RANDOM;
		this->SingleRandom.min = min; 
		this->SingleRandom.max = max;
		this->count = count;
		ReRoll();
	}   
	void DoMultiRandom(int min, int max, int rcount, int count)
	{
		this->mode = MULTI_RANDOM;
		this->MultiRandom.min = min; 
		this->MultiRandom.max = max;  
		this->MultiRandom.count = rcount;
		this->count = count;
		ReRoll();
	}  
	void DoGauss(float av, float std, int count)
	{
		this->mode = GAUSS;
		this->Gauss.av = av; 
		this->Gauss.std = std;  
		this->count = count;
		ReRoll();
	}    
	void DoPoisson(float av, int count)
	{
		this->mode = POISSON;
		this->Poisson.av = av;
		this->count = count;
		ReRoll();
	}    
	void DoGalton(int c, int count)
	{
		this->mode = GALTON_BOX;
		this->Galton.c = c;
		this->count = count;
		ReRoll();
	}    
	void DoFloatRandom(float min, float max, int count)
	{
		this->mode = FLOAT_RANDOM;
		this->FloatRandom.min = min;  
		this->FloatRandom.max = max;
		this->count = count;
		ReRoll();
	}        
	void DoExpRandom(float av, int count)
	{
		this->mode = EXP_RANDOM;
		this->ExpRandom.av = av;  
		this->count = count;
		ReRoll();
	}    
	
	enum MonteCarloFunction
	{
		Sin,
		Zad306	
	};
	
	void DoMonteCarloRand(MonteCarloFunction func, float x1, float x2, float y1, float y2, int count)
	{
		this->mode = MONTE_CARLO_RANDOM;
		this->MonteCarloRand.func = func;   
		this->MonteCarloRand.x1 = x1;       
		this->MonteCarloRand.x2 = x2;       
		this->MonteCarloRand.y1 = y1;    
		this->MonteCarloRand.y2 = y2;  
		this->count = count;
		ReRoll();
	}
	void ChangeCount(int count)
	{
		this->count = count;
		ReRoll();
	}   
		
private: 
	
	enum Mode
	{
		INVALID,
		SINGLE_RANDOM,
		MULTI_RANDOM,
		GAUSS,
		POISSON,
		GALTON_BOX,
		FLOAT_RANDOM,
		EXP_RANDOM,
		MONTE_CARLO_RANDOM
	};
	   
	Mode mode;
	union
	{
		struct
		{
			int min;
			int max;	
		} SingleRandom;	   
		struct
		{
			int min;
			int max;
			int count;	
		} MultiRandom;	
		struct
		{
			float av;
			float std;	
		} Gauss;  
		struct
		{
			float av;	
		} Poisson;     
		struct
		{
			int c;	
		} Galton; 
		struct
		{
			float min;
			float max;	
		} FloatRandom;	 
		struct
		{
			float av;	
		} ExpRandom;
		struct
		{
			MonteCarloFunction func;	  
			float x1;
			float x2;  
			float y1;
			float y2;
		}MonteCarloRand;
	};
	int count;
	
	Histogram::FactorialType factorial;
	std::string pattern;
	std::vector<int> variables;
	int min;
	int max;        
	Histogram hist;
	
	void ReRoll() 
	{
		variables.clear();  
		for(int i = 0; i < count; i++)
		{
			switch(mode)
			{
				case SINGLE_RANDOM: variables.push_back(Roll(SingleRandom.min, SingleRandom.max)); break;   
				case MULTI_RANDOM: { int v = 0; for(int i = 0; i < MultiRandom.count; i++) { v += Roll(MultiRandom.min, MultiRandom.max); } variables.push_back(v); } break;   
				case GAUSS: variables.push_back((int)(100.0 * ::Gauss(Gauss.av, Gauss.std))); break;        
				case POISSON: variables.push_back((int)(100.0 * ::Poisson(Poisson.av))); break;            
				case GALTON_BOX: variables.push_back(::GaltonBox(Galton.c)); break;                
				case FLOAT_RANDOM: variables.push_back((int)(100.0 * RollF(FloatRandom.min, FloatRandom.max))); break;  
				case EXP_RANDOM: variables.push_back((int)(100.0 * ExpRand(ExpRandom.av))); break;               
				case MONTE_CARLO_RANDOM:
					switch(MonteCarloRand.func)
					{
						case Sin: variables.push_back((int)(100.0 * MonteCarlo(sin, MonteCarloRand.x1, MonteCarloRand.x2, MonteCarloRand.y1, MonteCarloRand.y2))); break;
						case Zad306: variables.push_back((int)(100.0 * MonteCarlo(::Zad306, MonteCarloRand.x1, MonteCarloRand.x2, MonteCarloRand.y1, MonteCarloRand.y2)));  break;	
					}
				break;
				default: variables.push_back(0);	
			}	
		}
		min = 0;
		max = 0;
		if(!variables.empty())
		{
			min = variables[0];
			max = variables[0];
		}
		for(int i = 1; i < count; i++)
		{
			if(variables[i] < min)
				min = variables[i];   
			if(variables[i] > max)
				max = variables[i];
		}
		char patternBuff[256];
		strcpy(patternBuff, "Empty");
		switch(mode)
		{
			case SINGLE_RANDOM: factorial = Histogram::FactorialNone; sprintf(patternBuff, "Roll(%d, %d)", SingleRandom.min, SingleRandom.max); break;     
			case MULTI_RANDOM: factorial = Histogram::FactorialNone; sprintf(patternBuff, "Roll(%d, %d) x %d", MultiRandom.min, MultiRandom.max, MultiRandom.count); break;   
			case GAUSS: factorial = Histogram::FactorialVisible; sprintf(patternBuff, "Gauss av: %s, std: %s", FormatFloat(Gauss.av).c_str(), FormatFloat(Gauss.std).c_str()); break;  
			case POISSON: factorial = Histogram::FactorialHidden; sprintf(patternBuff, "Poisson av: %s", FormatFloat(Poisson.av).c_str()); break;         
			case GALTON_BOX: factorial = Histogram::FactorialNone; sprintf(patternBuff, "GaltonBox(%d)", Galton.c); break;                             
			case FLOAT_RANDOM: factorial = Histogram::FactorialVisible; sprintf(patternBuff, "Float Roll(%s, %s)", FormatFloat(FloatRandom.min).c_str(), FormatFloat(FloatRandom.max).c_str()); break;  
			case EXP_RANDOM: factorial = Histogram::FactorialVisible; sprintf(patternBuff, "Exp roll av: %s", FormatFloat(ExpRandom.av).c_str()); break;       
			case MONTE_CARLO_RANDOM: factorial = Histogram::FactorialVisible;
			    char name[256]; 
			    strcpy(name, "Unknown?");
				switch(MonteCarloRand.func)
				{
					case Sin: strcpy(name, "Sine"); break;
					case Zad306: strcpy(name, "Zad306");  break;	
				}
			sprintf(patternBuff, "Monte carlo method for %s", name); break;
		}
		this->pattern = patternBuff;  
		char buff[32];
		sprintf(buff, "%d", count);
		
		std::string heading = "Result of ";
		heading += pattern;
		heading += " in ";
		heading += buff;
		heading += " rolls";
		
		hist.Regen(heading, variables, min, max, factorial);
	}
	
	void Draw()   
	{
		hist.Draw(factorial);
	}
	
	void UpdateInput(int ch)
	{
		if(ch == KEY_F3)
		{
			for(;;)
			{      
				COORD size = GetConsoleSize();      
				hist.Draw(factorial); 
				Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Up/Down arrow to select, Enter to pick"); 
				int func = OptionBox("Pick", "Pre-made function\0Custom function\0Change roll count\0Cancel\0");
				if(func == 0)
				{        
					hist.Draw(factorial); 
					Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Up/Down arrow to select, Enter to pick"); 
					int preMade = OptionBox("Pick pre-made", "d2\0d6\0d6+d6\0d12\0Roll(1, 1000)\0Gauss av: 0.0, std: 1.0\0Poisson av: 2.75\0GaltonBox(12)\0Float roll(1.0, 10.0)\0Exp rand av: 2.5\0MonteCarlo(Sine)\0MonteCarlo (zad 306)\0Back\0");
					switch(preMade)
					{
						case 0: DoSingleRandom(1, 2, count); return;  
						case 1: DoSingleRandom(1, 6, count); return;  
						case 2: DoMultiRandom(1, 6, 2, count); return;   
						case 3: DoSingleRandom(2, 12, count); return; 
						case 4: DoSingleRandom(1, 1000, count); return;   
						case 5: DoGauss(0.0f, 1.0f, count); return;     
						case 6: DoPoisson(2.75f, count); return;       
						case 7: DoGalton(12, count); return;             
						case 8: DoFloatRandom(1.0, 10.0, count); return;    
						case 9: DoExpRandom(2.5, count); return;           
						case 10: DoMonteCarloRand(Sin, 0.0f, 3.14f, 0.0f, 1.0f, count); return;  
						case 11: DoMonteCarloRand(Zad306, 0.0f, 4.0f, 0.0f, 10.0f, count); return;
					}	
				}
				else if(func == 1)
				{       
					hist.Draw(factorial); 
					Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Up/Down arrow to select, Enter to pick"); 
					int custom = OptionBox("Custom function", "Single random roll\0Multiple random rolls\0Gauss random\0Poisson random\0Galton box\0Float random roll\0Exp random\0Back\0");
					switch(custom)
					{
						case 0:
							for(;;)
							{
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input minimum for random, Enter to accept"); 
								int min = InputInteger("Specify minimum value");	
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input maximum for random, Enter to accept"); 
								int max = InputInteger("Specify maximum value");
								if(min >= max)
									ErrorBox("Min value must be lower then max!", "Isn't that obvious...?");
								else
								{  
									DoSingleRandom(min, max, count);
									return;
								}
							}
						break;	  
						case 1:
							for(;;)
							{
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input minimum for random, Enter to accept"); 
								int min = InputInteger("Specify minimum value");	
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input maximum for random, Enter to accept"); 
								int max = InputInteger("Specify maximum value"); 	
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input number of rolls per attempt, Enter to accept"); 
								int c = InputInteger("Specify roll count");
								if(min >= max)
									ErrorBox("Min value must be lower then max!", "Isn't that obvious...?");
								else if(c <= 0) 
									ErrorBox("Count must be greater then 0!", "Isn't that obvious...?");
								else
								{  
									DoMultiRandom(min, max, c, count);
									return;
								}
							}
						break;	     
						case 2:
							for(;;)
							{
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input average number, dot is the separator, Enter to accept"); 
								double av = InputDouble("Specify average value");    
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input standard deviation number, dot is the separator, Enter to accept"); 
								double std = InputDouble("Specify standard deviation value");
								if(std <= 0)
									ErrorBox("Standard deviation must be greater then 0!", "Isn't that obvious...?");
								else
								{  
									DoGauss(av, std, count);
									return;
								}
							}
						break;   	     
						case 3:
							for(;;)
							{
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input average number, dot is the separator, Enter to accept"); 
								double av = InputDouble("Specify average value");    
								if(av <= 0)
									ErrorBox("Average must be greater then 0!", "Isn't that obvious...?");
								else
								{  
									DoPoisson(av, count);
									return;
								}
							}
						break;     
						case 4:
							for(;;)
							{
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input galton box count, Enter to accept"); 
								int c = InputInteger("Specify count");
								if(c <= 1) 
									ErrorBox("Count must be greater then 1!", "Isn't that obvious...?");
								else
								{  
									DoGalton(c, count);
									return;
								}
							}
						break;	      	     
						case 5:
							for(;;)
							{
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input minimum number, dot is the separator, Enter to accept"); 
								double min = InputDouble("Specify minimum value");  
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input maximum number, dot is the separator, Enter to accept"); 
								double max = InputDouble("Specify maximum value");  
								if(min >= max)
									ErrorBox("Min value must be lower then max!", "Isn't that obvious...?");  
								else
								{  
									DoFloatRandom(min, max, count);
									return;
								}
							}
						break;    	     
						case 6:
							for(;;)
							{
								hist.Draw(factorial);  
								Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input average number, dot is the separator, Enter to accept"); 
								double av = InputDouble("Specify average value");    
								if(av <= 0)
									ErrorBox("Average must be greater then 0!", "Isn't that obvious...?");
								else
								{  
									DoExpRandom(av, count);
									return;
								}
							}
						break; 
					}
				}	
				else if(func == 2)
				{      
					hist.Draw(factorial); 
					Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Up/Down arrow to select, Enter to pick"); 
					int count = OptionBox("Roll count", "10""\0""100""\0""1 000""\0""10 000""\0""100 000""\0""1 000 000""\0""Custom""\0""Back\0");
					switch(count)
					{
						case 0: ChangeCount(10); return;
						case 1: ChangeCount(100); return;	
						case 2: ChangeCount(1000); return;  	
						case 3: ChangeCount(10000); return;   	
						case 4: ChangeCount(100000); return;  	
						case 5: ChangeCount(1000000); return;
						case 6:
							hist.Draw(factorial);  
							Printf(size.Y - 1, 0, size.X, FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Input number, Enter to accept"); 
							ChangeCount(InputInteger("Specify custom roll count"));
						return;
					}
				}
				else return;
			}
	    }
	}
};   

class PageGameOfLife : public Page
{

public:

	PageGameOfLife()
	{   
		help = "Update generation: SPACE";
		ReRoll();
	}

private:
	int board[20][80];
	int boardOld[20][80];
	int GetX(int x)
	{
		return (x + 80) % 80;
	}
	int GetY(int y)
	{
		return (y + 20) % 20;
	}
	int CountNeighboors(int x, int y)
	{
		int ret = 0;
		for(int yp = y - 1; yp <= y + 1; yp++)
		{
			for(int xp = x - 1; xp <= x + 1; xp++)
			{
				if(yp == y && xp == x) continue;
				if(boardOld[GetY(yp)][GetX(xp)])
					ret++;
			}
		}
		return ret;
	}
	void ReRoll()
	{
		for(int y = 0; y < 20; y++)
		{
			for(int x = 0; x < 80; x++)
			{
				if(d(4) == 1)
					board[y][x] = 1;
				else
					board[y][x] = 0;
			}
		}
	}
	void Draw()
	{
		COORD size = GetConsoleSize();
	
		Printf(0, 0, size.X, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Game of life");
	
	
		for(int y = 0; y < 20; y++)
		{
			for(int x = 0; x < 80; x++)
			{
				int yp = y+2;
				int xp = x;
	
				if(board[y][x] != 0)
				{
					if(boardOld[y][x] == 0)
						Printf(yp, xp, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "%c", 2);
					else
						Printf(yp, xp, FOREGROUND_BLUE | FOREGROUND_GREEN, "%c", 2);
				}
			}
		}
	
		DrawCopyright();
	}
	void UpdateInput(int ch)
	{
		if(ch == ' ')
		{
			for(int i = 0; i < 1; i++)
			{
				for(int y = 0; y < 20; y++)
				{
					for(int x = 0; x < 80; x++)
					{
						boardOld[y][x] = board[y][x];
					}
				}
				for(int y = 0; y < 20; y++)
				{
					for(int x = 0; x < 80; x++)
					{
						int n = CountNeighboors(x, y);
						if(board[y][x] == 0 && n == 3)
							board[y][x] = 1;
						else if(board[y][x] == 1 && n != 2 && n != 3)
							board[y][x] = 0;
					}
				}
				Draw();
			}
		}
		else if(ch == 'r')
		{
			ReRoll();
		}
	}
};


class PageMonteCarloPi : public Page
{

public:

	PageMonteCarloPi()
	{        
		help = "Calc more: SPACE";
		ReRoll();
	}

private:
	int board[20][20];
	int pointsAll;
	int pointsInCircle;
	void ReRoll()
	{
		for(int y = 0; y < 20; y++)
		{
			for(int x = 0; x < 20; x++)
			{
				board[y][x] = 0;
			}
		}
		pointsAll = 0;
		pointsInCircle = 0;
	}
	void Draw()
	{
		COORD size = GetConsoleSize();
	    Printf(0, 0, size.X, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Monte Carlo for PI generation");
	
		char buff[512];
		if(pointsAll == 0)
			sprintf(buff, "Calculated PI: ???");
		else
			sprintf(buff, "Calculated PI: %f", (double)(4.0L * pointsInCircle / pointsAll));
			
	    Printf(1, 0, size.X, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, buff);
	         
		Printf(10, 25, size.X - 30, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY, "Points: %d (%d in circle)", pointsAll, pointsInCircle);
	                 
		for(int y = 0; y < 20; y++)
		{
			for(int x = 0; x < 20; x++)
			{
				bool inCircle = ((x - 9.5f) * (x - 9.5f) + (y - 9.5f) * (y - 9.5f) <= 10 * 10);
	
				int color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				if(!inCircle)
					color = FOREGROUND_RED | FOREGROUND_INTENSITY;
	
				int yp = y+2;
				int xp = x+2;
	
				if(board[y][x] == 0)
				{
					// Do nothing
				}	
				else if(board[y][x] < 10)
					Printf(yp, xp, color, "%c", 176);
				else if(board[y][x] < 50)
					Printf(yp, xp, color, "%c", 177);
				else if(board[y][x] < 100)
					Printf(yp, xp, color, "%c", 178);
				else
					Printf(yp, xp,color, "%c", 219); 	
			}
		}
		
	}
	void UpdateInput(int ch)
	{
		if(ch == ' ')
		{
			for(int i = 0; i < 1000; i++)
			{
				double x = RollF(0.0f, 20.0f);
				double y = RollF(0.0f, 20.0f);
				board[(int)y][(int)x]++;
				pointsAll++;
				if((x - 10.0f) * (x - 10.0f) + (y - 10.0f) * (y - 10.0f) < 10.0f * 10.0f)
					pointsInCircle++; 
				
				if((i % 40) == 1)	
				{
					ClearScreen();
					Draw();      
					DrawCopyright();      
					DrawPageTurn(help);
					Publish();
				}
			}
		}
		else if(ch == 'r')
		{
			ReRoll();
		}
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Main function

PageRoll page1; 
PageGameOfLife page2;
PageMonteCarloPi page3;

int main(int argc, char *argv[])
{
	Init();

    if(Page::GetPagesCount() == 0)
	{        
		COORD size = GetConsoleSize();
		ClearScreen();
			
		int color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_INTENSITY;
			        
		DrawCopyright();                                                        
		ErrorBox("ERROR: No pages are defined!!!");
	}
	else
	{
		int page = 0;
		for(;;)
		{
			Page::ShowPage(page);
			int ch = GetCh();
			if(ch == KEY_RIGHT)
				page = (page + 1) % Page::GetPagesCount();
			else if(ch == KEY_LEFT)
				page = (page - 1 + Page::GetPagesCount()) % Page::GetPagesCount();
			else if(ch == 'q')
				break;
			else if(ch == KEY_F5)
				Page::ReRollAll();
			else
				Page::PageInput(page, ch);
		}
	}
    
    return EXIT_SUCCESS;
}
