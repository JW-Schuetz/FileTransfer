#pragma once


constexpr auto FTPSERVER = "ftp://womser-schuetz.diskstation.me/homes/Juergen/";
constexpr auto FTPCREDENTIALS = "";

// Parser-Token
constexpr auto MODYFY = "modify=";
constexpr auto TYPE = "type=";

// Debugging und Memory-Leaks-Erkennung
const bool VERBOSE = false;
#define WITH_MEM_LEAK_TESTING

// Externe Funktionen
size_t write_callback(void *, size_t, size_t, void *);
void exitfun( const char *, int );