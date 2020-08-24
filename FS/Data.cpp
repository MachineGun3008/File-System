#include "Data.h"

void Data::addRun(pair<int32_t, uint32_t> Run)
{
	DataRun next;
	Length += next.create(Run.first, Run.second);
	data.push_back(next);
}
void Data::getBlockData(list<DataRun> &ans, int &Size, int &pos)
{
	int s;
	for (pos = 0; pos < data.size(); ++pos)
	{
		s = data[pos].getSize();
		if (Size >= s)
		{
			Size -= s;
			ans.push_back(data[pos]);
		}
		else break;
	}
	if (pos == data.size())
		pos = -1;
}

