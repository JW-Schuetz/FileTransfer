#include <iterator>
#include <utility>	// wg. pair

#include "FileTransfer.h"
#include "RemoteDirectory.h"

using namespace std;

// Konstruktor
RemoteDirectory::RemoteDirectory( string dir,  string signatur,  string credential, long int verbose)
{
	this->signatur = signatur;

	// Buffer konstruieren
	buffer = new GrowableStringBuffer();

	// set verbose mode on/off
	CURLcode res = curl_easy_setopt(GetHandle(), CURLOPT_VERBOSE, verbose);
	if( res != CURLE_OK ) exitfun( "CURL error!", 1 );

	// custom string for request
	res = curl_easy_setopt(GetHandle(), CURLOPT_CUSTOMREQUEST, "MLSD");
	if (res != CURLE_OK) exitfun("CURL error!", 1);

	// provide the URL to use in the request
	res = curl_easy_setopt(GetHandle(), CURLOPT_URL, dir.c_str());
	if (res != CURLE_OK) exitfun("CURL error!", 1);

	// user name and password to use in authentication
	res = curl_easy_setopt(GetHandle(), CURLOPT_USERPWD, credential.c_str());
	if (res != CURLE_OK) exitfun("CURL error!", 1);

	// custom pointer passed to the write callback
	res = curl_easy_setopt(GetHandle(), CURLOPT_WRITEDATA, buffer);
	if (res != CURLE_OK) exitfun("CURL error!", 1);

	// set callback for writing received data
	res = curl_easy_setopt(GetHandle(), CURLOPT_WRITEFUNCTION, write_callback);
	if (res != CURLE_OK) exitfun("CURL error!", 1);

	// Directory holen
	// perform a blocking file transfer
	res = curl_easy_perform(GetHandle());
	if (res != CURLE_OK) exitfun("CURL error!", 1);

	// buffer enth�lt keinen String, d.h. die 0-Terminierung fehlt, also 0-Terminierung anf�gen
	buffer->Terminate();
	content = (char*)buffer->GetContent();

	// Parsen
	ParseContent();
}

// Destruktor
RemoteDirectory::~RemoteDirectory()
{
	delete buffer;
}

void RemoteDirectory::ParseContent()
{
	// in directoryContent sind nun zeilenweise die Inhalte des Verzeichnisses abgelegt, z:B.:
	// modify = 20191005153937; type = file; unique = 902UE84; size = 4030; UNIX.mode = 0777;
	// UNIX.owner = J�rgen; UNIX.group = users; BitCoinPrice - 01.09.2019.csv

	// Liste der Directory-Entries f�llen
	typedef list<char *> CharPtrList;
	CharPtrList entries;
	CharPtrList::iterator iter;

	char* p = content;
	while (char* q = strstr(p, "\r\n"))
	{
		size_t length = q - p;

		char* r = new char[length+1];
		memcpy(r, p, length);
		r[length] = 0;

		// in Liste eintragen
		entries.push_back(r);

		// n�chster Teilstring
		p = q + 2;
	}

	for (iter = entries.begin(); iter != entries.end(); ++iter)
	{
		p = strstr( *iter, signatur.c_str() );
		if (p != NULL)
		{
			// Dateiname gefunden
			string fileName(p);

			// Zeitstempel parsen, Struktur st mit 0 initialisieren
			SYSTEMTIME st = {};

			char *q = strstr(*iter, MODYFY);
			if (q != NULL)
			{
				// "modify = " gefunden, Zeitstring 14 Zeichen "YYYYMMDDHHMMSS"

				// Jahr
				p = q + strlen(MODYFY);
				size_t size = 5;
				char *year = (char *)malloc(size);
				if (year == NULL) exitfun("No more memory available!", 1);
				memcpy(year, p, size - 1);
				year[size - 1] = 0;
				st.wYear = (WORD)atoi(year);
				free(year);

				// Monat
				p = p + size - 1;
				size = 3;
				char *month = (char *)malloc(size);
				if (month == NULL) exitfun("No more memory available!", 1);
				memcpy(month, p, size - 1);
				month[size - 1] = 0;
				st.wMonth = (WORD)atoi(month);
				free(month);

				// Tag
				p = p + size - 1;
				size = 3;
				char *day = (char *)malloc(size);
				if (day == NULL) exitfun("No more memory available!", 1);
				memcpy(day, p, size - 1);
				day[size - 1] = 0;
				st.wDay = (WORD)atoi(day);
				free(day);

				// Stunde
				p = p + size - 1;
				size = 3;
				char *hour = (char *)malloc(size);
				if (hour == NULL) exitfun("No more memory available!", 1);
				memcpy(hour, p, size - 1);
				hour[size - 1] = 0;
				st.wHour = (WORD)atoi(hour);
				free(hour);

				// Minute
				p = p + size - 1;
				size = 3;
				char *minute = (char *)malloc(size);
				if (minute == NULL) exitfun("No more memory available!", 1);
				memcpy(minute, p, size - 1);
				minute[size - 1] = 0;
				st.wMinute = (WORD)atoi(minute);
				free(minute);

				// Sekunde
				p = p + size - 1;
				size = 3;
				char *second = (char *)malloc(size);
				if (second == NULL) exitfun("No more memory available!", 1);
				memcpy(second, p, size - 1);
				second[size - 1] = 0;
				st.wSecond = (WORD)atoi(second);
				free(second);
			}

			// File/Directory-Kennung parsen
			q = strstr(*iter, TYPE);
			if (q != NULL)
			{
				q += strlen(TYPE);
				// falls File: Listeneintrag
				if (*q == 'f') {
					pairs.push_back(pair <string, SYSTEMTIME>(fileName, st));
				}
			}
		}

		free(*iter);
	}
}

list<pair <string, SYSTEMTIME> > RemoteDirectory::GetResult()
{
	return pairs;
}