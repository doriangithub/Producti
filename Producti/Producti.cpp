﻿// Producti.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Producti.h"
#include <iostream>
#include "sqlite3.h"
#include <vector>
#include <set>
#include "atlstr.h"
#include <strsafe.h>

#define MAXSTRINGLEN 150

#ifdef _DEBUG
#define new DEBUG_NEW

#endif

/* Standard error macro for reporting API errors */
#define PERR(bSuccess, api){if(!(bSuccess)) printf("%s:Error %d from %s \n line %d\n", __FILE__, GetLastError(), api, __LINE__);}

// The one and only application object

CWinApp theApp;

using namespace std;

static const char DBFILENAME[] = "Producti.db";

/***********************************************************************************************/
/*	Utility function to check if the file is exists                                       */
/***********************************************************************************************/
bool fileExists(LPCWSTR fileName_in)
{
	LPCWSTR lpFileName = TEXT("C:\\DEVELOPMENT\\Producti\\x64\\Debug\\Producti.db");
	LPCWSTR	path = L"C:\\DEVELOPMENT\\Producti\\x64\\Debug\\Producti.db";

	DWORD ftyp = GetFileAttributesW(lpFileName);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
	{
		cout << "Error: NO SUCH FILE!!!" << endl;
		return false;  //something is wrong with your path!
	}

	if (ftyp & FILE_ATTRIBUTE_ARCHIVE)
		return true;   // file exists!

	return false;    // file not exists!
}


COORD getBufSize(HANDLE hConsole)
{
	// get current console size
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	COORD bufSize;

	if (!GetConsoleScreenBufferInfo(hConsole, &csbiInfo))
	{
		printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
	}

	bufSize = csbiInfo.dwSize;
	return bufSize;
}

void drawSingelCharacter( HANDLE hConsole, LPCTSTR character, COORD coordScreen)
{
	DWORD dwConLengt = wcslen(character);
	DWORD cCharsWritten;
	BOOL bSuccess = WriteConsoleOutputCharacter(hConsole, character, dwConLengt, coordScreen, &cCharsWritten);
	PERR(bSuccess, "WriteConsoleOutputCharacter");
}

void drawManyCharacters(HANDLE hConsole, LPCTSTR characters, COORD coordScreen)
{
	DWORD dwConLengt = wcslen(characters);
	DWORD cCharsWritten;
	BOOL bSuccess = WriteConsoleOutputCharacter(hConsole, characters, dwConLengt, coordScreen, &cCharsWritten);
	PERR(bSuccess, "WriteConsoleOutputCharacter");
}

void drawFrame(HANDLE hConsole)
{
	// get current console size
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	COORD bufSize = getBufSize(hConsole);
		
	LPCTSTR character;		// draw corners
	COORD coordScreen;

	character = L"╔";		// draw ╔
	coordScreen = { 0,0 };
	drawSingelCharacter(hConsole, character, coordScreen);

	character = L"╗";		// draw ╗
	coordScreen = { bufSize.X - 1, 0 };
	drawSingelCharacter(hConsole, character, coordScreen);

	character = L"╚";		// draw ╚
	coordScreen = { 0, bufSize.Y - 1};
	drawSingelCharacter(hConsole, character, coordScreen);

	character = L"╝";		// draw ╝
	coordScreen = { bufSize.X - 1 , bufSize.Y - 1 };
	drawSingelCharacter(hConsole, character, coordScreen);
	
	character = L"═";		// draw ═
	for (SHORT i = 1; i < bufSize.X - 1; i++)
	{
		coordScreen = { i , 0 };
		drawSingelCharacter(hConsole, character, coordScreen);
	}

	character = L"═";		// draw ═
	for (SHORT i = 1; i < bufSize.X - 1; i++)
	{
		coordScreen = { i ,  bufSize.Y - 1 };
		drawSingelCharacter(hConsole, character, coordScreen);
	}

	character = L"║";		// draw ═
	for (SHORT i = 1; i < bufSize.Y - 1; i++)
	{
		coordScreen = { bufSize.X - 1 ,  i };
		drawSingelCharacter(hConsole, character, coordScreen);
	}

	character = L"║";		// draw ═
	for (SHORT i = 1; i < bufSize.Y - 1; i++)
	{
		coordScreen = { 0 ,  i };
		drawSingelCharacter(hConsole, character, coordScreen);
	}

	// ╠ ═ ╣

	character = L"╠";	// draw ╠
	coordScreen = { 0, bufSize.Y - 3 };
	drawSingelCharacter(hConsole, character, coordScreen);

	character = L"╣";	// draw ╣
	coordScreen = { bufSize.X - 1, bufSize.Y - 3 };
	drawSingelCharacter(hConsole, character, coordScreen);

	character = L"═";	// draw ═
	for (SHORT i = 1; i < bufSize.X - 1; i++)
	{
		coordScreen = { i , bufSize.Y - 3 };
		drawSingelCharacter(hConsole, character, coordScreen);
	}

	// ╠ ═ ╣

	character = L"╠";	// draw ╠
	coordScreen = { 0, 2 };
	drawSingelCharacter(hConsole, character, coordScreen);

	character = L"╣";	// draw ╣
	coordScreen = { bufSize.X - 1,  2 };
	drawSingelCharacter(hConsole, character, coordScreen);

	character = L"═";	// draw ═
	for (SHORT i = 1; i < bufSize.X - 1; i++)
	{
		coordScreen = { i , 2 };
		drawSingelCharacter(hConsole, character, coordScreen);
	}



}


void printString(HANDLE hConsole, std::vector<wchar_t*> arrayToPrint)
{
	// get current console size
	COORD bufSize = getBufSize(hConsole);

	// calculate width of column if 3 columns
	//              = (width - frame - space) / 3
	int columnWidth = (bufSize.X - 2 - 2) / 3;
	int columnHight = (bufSize.Y - 3 - 3);

	int lengthArray = arrayToPrint.size();

	// start coordinates
	int xStart = 2;
	int yStart = 4;
	int x = xStart;
	int y = yStart;
	int columnIndex = 1;

	for (int i = 0; i < lengthArray; i++)
	{

		size_t n = wcslen(arrayToPrint[i]);

		wchar_t buffer[_MAX_U64TOSTR_BASE10_COUNT];
		wchar_t * number = _itow((i + 1), buffer, 10);

		wchar_t numberString[4];

		if ((i + 1) < 10)
		{
			// add space in front
			wcscpy(numberString, L" ");
			wcsncat(numberString, number, 1);
		}
		else
		{
			wcscpy(numberString, number);
		}

		wcsncat(numberString, L".", 1);

		wchar_t string[MAXSTRINGLEN + 1];
		wcscpy(string, numberString);


		//wcsncat(string, arrayToPrint[i], __min(n, MAXSTRINGLEN - wcslen(arrayToPrint[i])));

		if ((wcslen(arrayToPrint[i]) + 3) > columnWidth)
		{
			// cut and add ".."
			wcsncat(string, arrayToPrint[i], columnWidth - 5);
			wcsncat(string, L"..", 2);
		}
		else
		{
			wcsncat(string, arrayToPrint[i], columnWidth - 3);
		}


		COORD coordScreen = { x, y };
		drawManyCharacters(hConsole, string, coordScreen);
		y++;
		if (y > columnHight)
		{
			x = xStart + 1 + columnWidth * columnIndex;
			y = yStart;
			columnIndex++;
		}
	}


}

/// void cls(HANDLE hConsole)
/// {
/// 	COORD coordScreen = { 0, 0 };    /* here's where we'll home the
/// 									 cursor */
/// 	COORD coordScreen2 = { 40, 0 };
/// 
/// 	BOOL bSuccess;
/// 	DWORD cCharsWritten;
/// 	CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
/// 	DWORD dwConSize;                 /* number of character cells in
/// 									 the current buffer */
/// 
/// 									 /* get the number of character cells in the current buffer */
/// 
/// 	DWORD dwConLengt;
/// 
/// 	// buffer size
/// 	// Get the current screen buffer size and window position. 
/// 	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
/// 	COORD bufSize;
/// 
/// 	if (!GetConsoleScreenBufferInfo(hConsole, &csbiInfo))
/// 	{
/// 		printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
/// 	}
/// 	else
/// 	{
/// 		bufSize = csbiInfo.dwSize;
/// 	}
/// 
/// 	if (bufSize.Y > 25)
/// 	{
/// 		COORD dwNewSize = { bufSize.X, 25 };
/// 		SetConsoleScreenBufferSize(hConsole, dwNewSize);
/// 	}
/// 
/// 	if (!GetConsoleScreenBufferInfo(hConsole, &csbiInfo))
/// 	{
/// 		printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
/// 	}
/// 	else
/// 	{
/// 		bufSize = csbiInfo.dwSize;
/// 	}
/// 
/// 	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
/// 	PERR(bSuccess, "GetConsoleScreenBufferInfo");
/// 	dwConSize = (csbi.dwSize.X * csbi.dwSize.Y);
/// 
/// 	/* fill the entire screen with blanks */
/// 
/// 	bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',
/// 		dwConSize, coordScreen, &cCharsWritten);
/// 	PERR(bSuccess, "FillConsoleOutputCharacter");
/// 
/// 	LPCTSTR character = L"-";
/// 
/// 	dwConLengt = wcslen(character);
/// 
/// 	bSuccess = WriteConsoleOutputCharacter(hConsole, character,
/// 		dwConLengt, coordScreen2, &cCharsWritten);
/// 	PERR(bSuccess, "WriteConsoleOutputCharacter");
/// 
/// 	/* get the current text attribute */
/// 
/// 	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
/// 	PERR(bSuccess, "ConsoleScreenBufferInfo");
/// 
/// 	/* now set the buffer's attributes accordingly */
/// 
/// 	bSuccess = FillConsoleOutputAttribute(hConsole, csbi.wAttributes,
/// 		dwConSize, coordScreen, &cCharsWritten);
/// 	PERR(bSuccess, "FillConsoleOutputAttribute");
/// 
/// 	/* put the cursor at (0, 0) */
/// 
/// 	bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
/// 	PERR(bSuccess, "SetConsoleCursorPosition");
/// 	return;
/// }




void cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    /* here's where we'll home the
									 cursor */
	COORD coordScreen2 = { 40, 0 };

	BOOL bSuccess;
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
	DWORD dwConSize;                 /* number of character cells in
									 the current buffer */

									 /* get the number of character cells in the current buffer */

	DWORD dwConLengt;

	// buffer size
	// Get the current screen buffer size and window position. 
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	COORD bufSize;
	
	if (!GetConsoleScreenBufferInfo(hConsole, &csbiInfo))
	{
		printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
	}
	else
	{
		bufSize = csbiInfo.dwSize;
	}
		
	//if (bufSize.Y > 25)
	//{
	//	COORD dwNewSize = { bufSize.X, 25 };
	//	if (!SetConsoleScreenBufferSize(hConsole, dwNewSize))
	//	{
	//		printf("SetConsoleScreenBufferSize (%d)\n", GetLastError());
	//	}
	//	bufSize = csbiInfo.dwSize;
	//}

	COORD dwNewSize = { bufSize.X, 25 };

	_SMALL_RECT Rect;
	Rect.Top = 0;
	Rect.Left = 0;
	Rect.Bottom = 25 - 1;
	Rect.Right = bufSize.X - 1;



    // Set Window Size 
	if (!SetConsoleWindowInfo(hConsole, TRUE, &Rect))
	{
		printf("SetConsoleWindowInfo (%d)\n", GetLastError());
	}

	// Set Buffer Size 
	if (!SetConsoleScreenBufferSize(hConsole, dwNewSize))
	{
		printf("SetConsoleScreenBufferSize (%d)\n", GetLastError());
	}

	if (!GetConsoleScreenBufferInfo(hConsole, &csbiInfo))
	{
		printf("GetConsoleScreenBufferInfo (%d)\n", GetLastError());
	}
	else
	{
		bufSize = csbiInfo.dwSize;
	}

	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	PERR(bSuccess, "GetConsoleScreenBufferInfo");
	dwConSize = (csbi.dwSize.X * csbi.dwSize.Y);

	/* fill the entire screen with blanks */

	bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR) ' ',
		dwConSize, coordScreen, &cCharsWritten);
	PERR(bSuccess, "FillConsoleOutputCharacter");

	return;
}


int screenHandle(void)
{
	HANDLE hStdout, hNewScreenBuffer;
	SMALL_RECT srctReadRect;
	SMALL_RECT srctWriteRect;
	CHAR_INFO chiBuffer[160]; // [2][80]; 
	COORD coordBufSize;
	COORD coordBufCoord;
	BOOL fSuccess;

	// Get a handle to the STDOUT screen buffer to copy from and 
	// create a new screen buffer to copy to. 

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hNewScreenBuffer = CreateConsoleScreenBuffer(
		GENERIC_READ |           // read/write access 
		GENERIC_WRITE,
		FILE_SHARE_READ |
		FILE_SHARE_WRITE,        // shared 
		NULL,                    // default security attributes 
		CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE 
		NULL);                   // reserved; must be NULL 

	if (hStdout == INVALID_HANDLE_VALUE ||
		hNewScreenBuffer == INVALID_HANDLE_VALUE)
	{
		printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
		return 1;
	}

	// Make the new screen buffer the active screen buffer. 

	if (!SetConsoleActiveScreenBuffer(hNewScreenBuffer))
	{
		printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
		return 1;
	}

	// Set the source rectangle. 

	srctReadRect.Top = 0;    // top left: row 0, col 0 
	srctReadRect.Left = 0;
	srctReadRect.Bottom = 1; // bot. right: row 1, col 79 
	srctReadRect.Right = 79;

	// The temporary buffer size is 2 rows x 80 columns. 

	coordBufSize.Y = 2;
	coordBufSize.X = 80;

	// The top left destination cell of the temporary buffer is 
	// row 0, col 0. 

	coordBufCoord.X = 0;
	coordBufCoord.Y = 0;

	// Copy the block from the screen buffer to the temp. buffer. 

	fSuccess = ReadConsoleOutput(
		hStdout,        // screen buffer to read from 
		chiBuffer,      // buffer to copy into 
		coordBufSize,   // col-row size of chiBuffer 
		coordBufCoord,  // top left dest. cell in chiBuffer 
		&srctReadRect); // screen buffer source rectangle 
	if (!fSuccess)
	{
		printf("ReadConsoleOutput failed - (%d)\n", GetLastError());
		return 1;
	}

	// Set the destination rectangle. 

	srctWriteRect.Top = 10;    // top lt: row 10, col 0 
	srctWriteRect.Left = 0;
	srctWriteRect.Bottom = 11; // bot. rt: row 11, col 79 
	srctWriteRect.Right = 79;

	// Copy from the temporary buffer to the new screen buffer. 

	fSuccess = WriteConsoleOutput(
		hNewScreenBuffer, // screen buffer to write to 
		chiBuffer,        // buffer to copy from 
		coordBufSize,     // col-row size of chiBuffer 
		coordBufCoord,    // top left src cell in chiBuffer 
		&srctWriteRect);  // dest. screen buffer rectangle 
	if (!fSuccess)
	{
		printf("WriteConsoleOutput failed - (%d)\n", GetLastError());
		return 1;
	}
	Sleep(5000);

	// Restore the original active screen buffer. 

	if (!SetConsoleActiveScreenBuffer(hStdout))
	{
		printf("SetConsoleActiveScreenBuffer failed - (%d)\n", GetLastError());
		return 1;
	}

	return 0;
}


TCHAR* GetFolderPath(TCHAR *exePath)
{
	TCHAR *delim = _T("\\");
	TCHAR *context;
	TCHAR *name;

	std::vector<TCHAR*> myPath;

	name = _tcstok_s(exePath, delim, &context);
	myPath.push_back(name);
	
	while (name != NULL)
	{
		name = _tcstok_s(NULL, delim, &context);
		if (name != NULL)
			myPath.push_back(name);
	}

	TCHAR newPath[_MAX_PATH];
	myPath.pop_back(); // remove file name

	std::vector<int>::size_type sz = myPath.size();
	wcscpy(newPath, myPath[0]);
	wcscat(newPath, _T("\\"));

	for (unsigned i = 1; i < sz; i++)
	{
		wcscat(newPath, myPath[i]);
		wcscat(newPath, _T("\\"));
	}

	return newPath;
}


/*********************************************************************************************/
/*	 Convert STR to LPC									                                     */
/*********************************************************************************************/
LPCSTR makeLPCSTR(CString str)
{
	CStringA strA(str); // a helper string
	LPCSTR newString = strA;
	return newString;
}

//int createTable(const char* sqlStr)
//{
//	sqlite3 *db;
//	char *zErrMsg = 0;
//	int  rc;
//
//	/* Open database */
//	rc = sqlite3_open(DBFILENAME, &db);
//	if (rc) {
//		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
//		return(0);
//	}
//	else {
//		//fprintf(stdout, "Opened database successfully\n");
//	}
//
//	/* Execute SQL statement */
//	rc = sqlite3_exec(db, sqlStr, callback, 0, &zErrMsg);
//	if (rc != SQLITE_OK) {
//		fprintf(stderr, "SQL error: %s\n", zErrMsg);
//		system("pause");
//		sqlite3_free(zErrMsg);
//		//return(0);
//	}
//	else {
//		//fprintf(stdout, "Table created successfully\n");
//	}
//	sqlite3_close(db);
//	return 1;
//}



int main(int args, char * argv[])
{
	int nRetCode = 0;

	////////////////////////////////////////////

 	char * path = argv[0];

	wchar_t wtext[256];
	mbstowcs(wtext, path, strlen(path) + 1);//Plus null
	
	LPWSTR ptr = wtext;

	GetFileAttributes(ptr); // from winbase.h
	if (0xffffffff == GetFileAttributes(ptr))
	{
		//File not found
		int i = 0;
	}

	////////////////////////////////////////////
	

	// Check if database file is in the same folder
	TCHAR exePath[MAX_PATH];

	if (!GetModuleFileName(NULL, exePath, MAX_PATH))
	{
		printf("Cannot install service (%d)\n", GetLastError());
		return 0;
	}

	// remove exe file name from the path
	TCHAR *folderPath = GetFolderPath(ptr);
	
	TCHAR szPath[MAX_PATH];

	const int lengthName = size(DBFILENAME);

	TCHAR fileName[lengthName];

	TCHAR *pFileName = fileName;

	for (int i = 0; i < lengthName; i++)
	{
		fileName[i] = DBFILENAME[i];
	}

	PathAppend(folderPath, pFileName);

	bool rezultBoolFile;
	rezultBoolFile = fileExists(folderPath);	// bool dirExists(const std::string& dirName_in)
	if (!rezultBoolFile)
	{
		//	create default database file
	}


	////////////////////// SETUP DATABASE  /////////////////////////

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *sql;

	/* Open database */
	rc = sqlite3_open(DBFILENAME, &db);

	if (rc) {
		fprintf(stderr, "Can't open database: %s...*\n", sqlite3_errmsg(db));
		return -1;
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}
	sqlite3_close(db);

	//////////////////// END SETUP DATABASE //////////////////////// 

	HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // initialize MFC and print and error on failure
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: change error code to suit your needs
            wprintf(L"Fatal Error: MFC initialization failed\n");
            nRetCode = 1;
        }
		else
		{
			// Clean screen
			HANDLE hStdout, hNewScreenBuffer;

			hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
			hNewScreenBuffer = CreateConsoleScreenBuffer(
				GENERIC_READ |           // read/write access 
				GENERIC_WRITE,
				FILE_SHARE_READ |
				FILE_SHARE_WRITE,        // shared 
				NULL,                    // default security attributes 
				CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE 
				NULL);                   // reserved; must be NULL 

			if (hStdout == INVALID_HANDLE_VALUE ||
				hNewScreenBuffer == INVALID_HANDLE_VALUE)
			{
				printf("CreateConsoleScreenBuffer failed - (%d)\n", GetLastError());
				return 1;
			}

			if (hStdout != NULL)
			{
				cls(hStdout);
				drawFrame(hStdout);
			}


			// COORD  dwCursorPosition = { 0, 10 };
			// SetConsoleCursorPosition(hStdout, dwCursorPosition);
			// 
			// int age;
			// cin >> age;
			// 
			// dwCursorPosition = { 1, 11 };
			// SetConsoleCursorPosition(hStdout, dwCursorPosition);
			// 
			// const VOID    *lpBuffer = L"ddddd";
			// DWORD   nNumberOfCharsToWrite = 5;
			// LPDWORD lpNumberOfCharsWritten = NULL;
			// LPVOID  lpReserved = NULL;
			// 
			// WriteConsole(hStdout, lpBuffer, nNumberOfCharsToWrite, lpNumberOfCharsWritten, lpReserved);

			// ╔ ═ ╗
			// ╠ ═ ╣
			// ╚ ═ ╝

			std:vector<wchar_t*> prodNames;
			prodNames.push_back(L"Avocado qwertyuiopasdfghjklzxcvbnm");
			prodNames.push_back(L"Beef");
			prodNames.push_back(L"Trout");
			prodNames.push_back(L"Salmon");
			prodNames.push_back(L"Herring");
			prodNames.push_back(L"Mackerel");
			prodNames.push_back(L"Halibut");
			prodNames.push_back(L"Shrimp");
			prodNames.push_back(L"Flax oil");
			prodNames.push_back(L"Coconut oil");
			prodNames.push_back(L"Akadamia");
			prodNames.push_back(L"Pecan");
			prodNames.push_back(L"Brazilian nut");
			prodNames.push_back(L"Pistachios");
			prodNames.push_back(L"Almond");
			prodNames.push_back(L"Pine nut");
			prodNames.push_back(L"Walnut");
			prodNames.push_back(L"Broccoli");
			prodNames.push_back(L"Brussels sprouts");
			prodNames.push_back(L"Cauliflower");
			prodNames.push_back(L"Arugula");
			prodNames.push_back(L"Watercress");
			prodNames.push_back(L"Sauerkraut");
			prodNames.push_back(L"Kombucha");
			prodNames.push_back(L"Yogurt");
			prodNames.push_back(L"Kvas");
			prodNames.push_back(L"Kefir");
			prodNames.push_back(L"Butter");
			prodNames.push_back(L"Salo");
			prodNames.push_back(L"Eggs");
			prodNames.push_back(L"Blueberries");
			prodNames.push_back(L"Strawberry");
			prodNames.push_back(L"Raspberries");
			prodNames.push_back(L"Cranberry");
			prodNames.push_back(L"Black grapes");
			prodNames.push_back(L"Green apple");
			prodNames.push_back(L"Orange");
			prodNames.push_back(L"Pomegranate");
			prodNames.push_back(L"Mango");

			printString(hStdout, prodNames);




        }
    }
    else
    {
        // TODO: change error code to suit your needs
        wprintf(L"Fatal Error: GetModuleHandle failed\n");
        nRetCode = 1;
    }

    return nRetCode;
}

