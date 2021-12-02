
#ifndef _ASQ_H
#define _ASQ_H

#include "common.h"

using namespace irr;
using namespace core;


class sequence
{
public:
	sequence();
	core::stringc name;
	int start;
	int end;
};


void ReadSeq(irr::IrrlichtDevice * dev,core::stringc path,sequence * sq, stringc name);



#endif