#include "Data.h"

void Data::addRun(pair<int32_t, uint32_t> Run)
{
	DataRun next;
	Length += next.create(Run.first, Run.second);
	data.push_back(next);
}

