#pragma once

#include "GrowableBuffer.h"

class GrowableStringBuffer : public GrowableBuffer
{
public:
	// Konstruktor
	GrowableStringBuffer() {};

	// Destruktor
	~GrowableStringBuffer() {};

	// String 0-Terminierung anf�gen
	void Terminate();
};
