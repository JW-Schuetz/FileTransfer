#include <regex>

#include "ManageDirectories.h"
#include "RemoteFile.h"


// Konstruktor
ManageDirectories::ManageDirectories( string remotedir, string hostdir, string signatur )
{
	if( *(remotedir.end() - 1) != '/' ) remotedir += string( "/" );
	if( *(hostdir.end() - 1) != '\\' ) hostdir = hostdir + string( "\\" );

	remotedirectory = string(FTPSERVER) + remotedir;
	hostdirectory = hostdir;

	signatur += string( "-" );

	remote = new RemoteDirectory( remotedirectory, signatur, FTPCREDENTIALS, 1L );
	host = new HostDirectory( hostdirectory, signatur );

	remotePairs = remote->GetResult();
	hostPairs = host->GetResult();
}

// Destruktor
ManageDirectories::~ManageDirectories()
{
	delete remote;
	delete host; 
}

bool ManageDirectories::TestRemoteFileOlder(decltype(remotePairs)::iterator s, decltype(hostPairs)::iterator h)
{
	bool remoteOlder = false;

	// compare modify times
	SYSTEMTIME remote = s->second;
	SYSTEMTIME host = h->second;

	FILETIME ft;
	ULARGE_INTEGER uliRemote = {};
	ULARGE_INTEGER uliHost = {};

	if (SystemTimeToFileTime(&remote, &ft))
	{
		uliRemote.HighPart = ft.dwHighDateTime;
		uliRemote.LowPart = ft.dwLowDateTime;

		if (SystemTimeToFileTime(&host, &ft))
		{
			uliHost.HighPart = ft.dwHighDateTime;
			uliHost.LowPart = ft.dwLowDateTime;
		}
	}

	// QuadParts grow with time-distance to a fixed time in the very past.
	// Newer files have a bigger QuadPart, older files a smaller one.
	// remote: 15:39:37   host: 15.17:39
	if (uliRemote.QuadPart <= uliHost.QuadPart) remoteOlder = true;

	return remoteOlder;
}

void ManageDirectories::DownloadIfNeccessary()
{
	// für jedes Remotefile
	for (auto r = remotePairs.begin(); r != remotePairs.end(); ++r)
	{
		bool hit = false;
		// für jedes Hostfile
		for (auto h = hostPairs.begin(); h != hostPairs.end(); ++h)
		{
			// Pfadanteil entfernen, nur Dateinamen übriglassen
			path hostPath = h->first;
			bool equal = (r->first == hostPath.filename());
			if (equal)
			{
				// compare modify times
				bool fileOlder = TestRemoteFileOlder(r, h);
				// nur downloaden wenn remoteFile neuer ist als hostFile
				if (!fileOlder)	DownloadIt(r->first);
				hit = true;
				break;
			}
		}

		// s nicht in h gefunden: downloaden
		if (!hit) DownloadIt(r->first);
	}
}

void ManageDirectories::DownloadIt(string fileName)
{
	// File downloaden
	string remoteFileName = remotedirectory + fileName;
	RemoteFile remoteFile(remoteFileName, string(FTPCREDENTIALS), VERBOSE);
	remoteFile.GetRemoteFile();

	// Verzeichnisnamen aus Dateinamen extrahieren
	regex regx2( "20[0-9][0-9]" );

	smatch mres;
	auto ret = regex_search(fileName, mres, regx2);
	if( ret == true )
	{
		string dir = mres[0];

		// File abspeichern
		string hostDir = hostdirectory + dir + "\\";
		remoteFile.SaveRemoteFileToHost( hostDir, fileName );
	}
}