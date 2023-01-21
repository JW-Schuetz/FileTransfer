#include <regex>

#include "HostDirectory.h"
#include "FileTransfer.h"

#pragma warning(disable : 4996)


// Konstruktor
HostDirectory::HostDirectory(string hostdirectory, const string signatur)
{
	path p = path(hostdirectory.c_str());

	// Regular Expression für den Match des Dateinamens
	wregex regx2( L"(^.*20[0-9][0-9]$)" );

	// Original BitCoinKurs-Directory iterieren
	for (directory_iterator next(p), end; next != end; ++next)
	{
		directory_entry d = *next;

		if( !d.is_directory() ) continue;					// Element ist kein Directory: uninteressant

		// REGEX match
		wsmatch cm;											// Element ist Directory: evtl. interessant
		wstring wname = d.path().wstring();
		if( !regex_match( wname, cm, regx2 ) ) continue;	// Directoryname nicht vom Typ regx2: uninteressant

		path q = path( d.path().c_str() );					// Directoryname vom richtigen Typ: darüber iterieren
		for( directory_iterator next( q ), end; next != end; ++next )
		{
			directory_entry e = *next;
			auto* name = e.path().c_str();

			file_time_type ftime = last_write_time( e );

			FILETIME ft;
			memcpy( &ft, &ftime, sizeof( FILETIME ) );

			// Struktur mit 0 initialisieren
			SYSTEMTIME st = {};

			if( FileTimeToSystemTime( &ft, &st ) )
			{
				// werden im Server nicht gefüllt
				st.wDayOfWeek = 0;
				st.wMilliseconds = 0;

				// Länge der Dateinamen: BitCoinPrice-dd.mm.yyyy
				// Anzahl benötigter Bytes bestimmen
				size_t size = wcstombs( NULL, name, 1 );

				// Platz allokieren
				char* fn = (char*)malloc( size + 1 );
				if (fn == NULL) exitfun("No more memory available!", 1);
				wcstombs( fn, name, size );
				fn[size] = 0;

				char* pfn = strstr( fn, signatur.c_str() );
				if( pfn != NULL )	// nur die "BitCoinPrice-" Dateien
					// Beispiel für fn: "D:\\Projekte\\BitCoinKurs\\2019\\BitCoinPrice-01.10.2019.csv"
					pairs.push_back( pair <string, SYSTEMTIME>( fn, st ) );

				free( fn );	// Platz wieder zur Verfügung stellen
			}
		}
	}
}

list<pair <string, SYSTEMTIME> > HostDirectory::GetResult()
{
	return pairs;
}