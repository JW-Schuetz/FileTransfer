#pragma once

#include <list>
#include <string>

#include "CurlEasyHandle.h"
#include "GrowableStringBuffer.h"

using namespace std;

class RemoteDirectory : public CurlEasyHandle
{
private:
	string signatur;
	char *content = NULL;
	GrowableStringBuffer *buffer = NULL;
	list<pair <string, SYSTEMTIME>> pairs;

	// Contentparser
	void ParseContent();

public:
	// Konstruktor/ Destruktor
	RemoteDirectory( string, string, string, long int );
	~RemoteDirectory();

	// Ergebnis abholen
	list<pair <string, SYSTEMTIME>> GetResult();
};
