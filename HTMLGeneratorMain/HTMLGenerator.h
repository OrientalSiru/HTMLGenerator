#pragma once
//#include "MT.h"
#include "BufferManager.h"
#include <Windows.h>
#include <random>
#include <unordered_map>
#include "MT.h"

#define MAXIMUM_LINKS 120
#define MINIMUM_LINKS 0

class HTMLGenerator {
private:
	static const char characters[64];//static array of characters
	MersenneTwister MT; 
	void randString(char* bm);
	void makeURL(char* ptr); //all URLS 29 characters + '\0'
	
	std::default_random_engine generator;
	std::uniform_int_distribution<int> *distribution;

	std::unordered_map<int, LONGLONG> stats;
public:
	HTMLGenerator(LONG seed); //seed for mersenne twister
	~HTMLGenerator();

	void makeHeadResponse(BufferManager* bm, int& bytes);
	void makeGetResponse(BufferManager* bm, int& bytes);
};