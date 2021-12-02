
#include "ASQ.h"


sequence::sequence()
{
	name = "";
	start = 0;
	end = 0;
}

void ReadSeq(irr::IrrlichtDevice * dev,core::stringc path,sequence * sq, stringc name)
{
	irr::io::IFileSystem * fs = dev->getFileSystem();
	irr::io::IXMLReader * reader;

	if (!fs->existFile(path))
		return;

	reader = fs->createXMLReader(path);
	bool done = false;

	while(reader->read() && !done)
	{
		if	(name == reader->getNodeName())
		{
			sq->name = name;
			sq->start = reader->getAttributeValueAsInt(L"Start");
			sq->end = reader->getAttributeValueAsInt(L"End");
			done = true;
		}
	}

	if (!done)
	{
		printf("Animation ");
		printf(name.c_str());
		printf(" not found! \n");
		sq = 0;
	}
	else
	{
		core::stringc num;
		num += sq->start;
		num += " - ";
		num += sq->end;
		printf("Loaded sequence: ");
		printf(name.c_str());
		printf(" Frames: ");
		printf(num.c_str());
		printf("\n");
	}
}