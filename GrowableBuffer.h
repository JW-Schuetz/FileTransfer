#pragma once

#include <cstring>
#include <malloc.h>

class GrowableBuffer
{
protected:
	void *ptr;
	size_t size;

public:
	// Konstruktor
	GrowableBuffer();

	// Destruktor
	~GrowableBuffer();

	// neuen Inhalt anf�gen
	void AddContent(size_t additionalSize, void *src);

	// Inhalt liefern
	void *GetContent();

	// L�nge des Inhaltes liefern
	size_t GetSize();
};
