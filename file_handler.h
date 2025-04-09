#pragma once
#include <string>
#include <fstream>
#include <expected>
#include <iostream>

class FileHandler {
public:
	struct InputData {
		double t;
		double p1;
		double T1;
		double kap;
		double R;
		double m0;
		double m1;
		double h0;
		double h1;
		double F0;
		double eps;
	};

	FileHandler(const char* filename);
	~FileHandler();
	bool Read(InputData& data);

private:
	std::fstream file;
	double* IndexToDataPtr_(InputData& data, int index);
};