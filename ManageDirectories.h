#pragma once

#include <string>

#include "RemoteDirectory.h"
#include "HostDirectory.h"
#include "FileTransfer.h"

class ManageDirectories
{
private:
	HostDirectory *host;
	RemoteDirectory *remote;
	string hostdirectory;
	string remotedirectory;

	// Directory-Inhalte
	decltype(remote->GetResult()) remotePairs;
	decltype(host->GetResult()) hostPairs;

	// Funktionen
	bool TestRemoteFileOlder(decltype(remotePairs)::iterator, decltype(hostPairs)::iterator);
	void DownloadIt(string);

public:
	// Konstruktor/ Destruktor
	ManageDirectories( string, string, string );
	~ManageDirectories();

	// DoIt!
	void DownloadIfNeccessary();
};