#include <iostream>

#include "CurlEasyHandle.h"
#include "ManageDirectories.h"

#ifdef WITH_MEM_LEAK_TESTING
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
#endif

using namespace std;


void exitfun( const char * errstr, int doexit )
{
	fprintf( stderr, "%s\n\n", errstr );

	if( doexit != 0 ) exit( -1 );
}

// callback wird benutzt von: RemoteDirectory, RemoteFile
size_t write_callback(void* buffer, size_t size, size_t nmemb, void* userp)
{
	// Anzahl der neuen Bytes
	size_t realsize = size * nmemb;

	// Zieldatenspeicher holen und neue Inhalte anfügen
	GrowableBuffer* b = (GrowableBuffer*)userp;
	b->AddContent(realsize, buffer);

	return realsize;
}

// Benutzung:	FileTransfer
// 1. Argument:	Remote-Verzeichnis
// 2. Argument:	Host-Verzeichnis
// 3. Argument: File-Signatur
// 
// Remote-Verzeichnis ist relativ zu: ftp://womser-schuetz.diskstation.me/homes/Juergen/
// Host-Verzeichnis ist absolut
// File-Namenskonvention: Signatur-dd.mm.yyyy.csv
//
// Beispiele:
// SensorValues: FileTransfer SensorValues/ D:\Projekte\HouseTemperature\SensorValues\ SensorValues
// BitCoinPrice: FileTransfer BitCoinPrice/ D:\Projekte\BitCoinValues\ BitCoinPrice

int main(int argc, const char *argv[])
{
	if (argc != 4) exitfun( "Wrong parameter count!", 1 );

	// LibCurl initialisieren
	curl_global_init(CURL_GLOBAL_DEFAULT);

	int ret = 0;

	try
	{
		ManageDirectories *manager = new ManageDirectories( argv[1], argv[2], argv[3] );
		manager->DownloadIfNeccessary();

		delete manager;
	}
	catch (exception& e)
	{
		ret = -2;

		cerr << "Caught: " << e.what() << endl;
		cerr << "Type: " << typeid(e).name() << endl;
	};

	curl_global_cleanup();

#ifdef WITH_MEM_LEAK_TESTING
	bool leak = _CrtDumpMemoryLeaks();
#endif

	return ret;
}