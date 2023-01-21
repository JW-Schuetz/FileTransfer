#pragma warning(disable : 4996)

#include <errno.h>
#include <direct.h>
#include "RemoteFile.h"
#include "FileTransfer.h"

using namespace std;

// Konstruktor
RemoteFile::RemoteFile(const string file, const string credential, long int verbose)
{
	// Buffer konstruieren
	buffer = new GrowableStringBuffer();

	// set verbose mode on/off
	CURLcode res = curl_easy_setopt(GetHandle(), CURLOPT_VERBOSE, verbose);
	if (res != CURLE_OK) exitfun("CURL error!", 1);

	// custom string for request
	curl_easy_setopt(GetHandle(), CURLOPT_CUSTOMREQUEST, "MLSD");
	if (res != CURLE_OK) exitfun("CURL error!", 1);

	// provide the URL to use in the request
	res = curl_easy_setopt(GetHandle(), CURLOPT_URL, file.c_str());
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
}

// Destruktor
RemoteFile::~RemoteFile()
{
	delete buffer;
}

// File holen
void RemoteFile::GetRemoteFile()
{
	// perform a blocking file transfer
	CURLcode res = curl_easy_perform(GetHandle());
	if (res != CURLE_OK) exitfun("CURL error!", 1);

	// buffer enthält keinen String, d.h. die 0-Terminierung fehlt, also 0-Terminierung anfügen
	buffer->Terminate();
	content = buffer->GetContent();
}

// File remote abspeichern
void RemoteFile::SaveRemoteFileToHost( string hostDirectory, string filename )
{
	int ret = _mkdir( hostDirectory.c_str() );
	if( ret == -1 )
	{
		if( errno != EEXIST )
			exitfun( "Error writing host-directory!", 1 );
	}

	string hostFileName = hostDirectory + filename;
	FILE *f = fopen(hostFileName.c_str(), "w");

	if( f == NULL ) exitfun( "Error writing host-file!", 1 );

	fwrite(content, 1, strlen((char *)content), f);

	int err = fclose(f);

	if (err != 0) exitfun("Error closing host-file!", 1);
}