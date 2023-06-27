#pragma once
#include <chrono>
#include <unordered_map>
#include <string>

using namespace std;

class Stopwatch
{
public:
	void start(string name);
	long stop(string name);
	long measure(string name);
	string toString();
private:
	unordered_map<string, chrono::steady_clock::time_point> metrics;
	string result = "";

};

