#pragma once

#include <filesystem>
#include <windows.h>
#include <list>
#include <string>

using namespace std;
using namespace std::filesystem;

class HostDirectory
{
private:
	list<pair <string, SYSTEMTIME>> pairs;

public:
	// Konstruktor
	HostDirectory( string,  string);

	// Destruktor
	~HostDirectory() {};

	// Ergebnis abholen
	list<pair <string, SYSTEMTIME>> GetResult();
};
