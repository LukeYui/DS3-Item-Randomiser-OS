#include <Windows.h>
#include <iostream>
#include <random>
#include <chrono>
#include <fstream>
#include <algorithm>

class CAoBRandomiser {
public:
	virtual VOID Generate();
	virtual DWORD GetSeed();
	virtual VOID ShuffleList();
	virtual VOID OutputList();

};