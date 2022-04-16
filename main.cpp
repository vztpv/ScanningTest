
#define USE_SIMD

#include <iostream>


#include "mimalloc-new-delete.h"

//#include "parser.h"

#include "parser2.h"


int main(void)
{
	//clau::LoadData::LoadDataFromFile("citylots.json", 0, 0);

	clau_test::LoadData::LoadDataFromFile("citylots.json", 0, 0);

	return 0;
}
