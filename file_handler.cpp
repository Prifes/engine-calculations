#include "file_handler.h"

FileHandler::FileHandler(const char* filename){
	file.open(filename);
}

FileHandler::~FileHandler() {
	file.close();
}

bool FileHandler::Read(InputData& data) {
	bool value_flag = false;
	char c = ' ';
	int row = 0;
	std::string value;
	double* num;
	bool reading = true;
	while (reading) {
		switch (c) {
		case '=':
			value_flag = true;
			break;
		case EOF:
			reading = false;
		case '\n':
			num = IndexToDataPtr_(data, row++);
			if (num) {
				*num = atof(value.c_str());
				value_flag = false;
				value.clear();
			}
			else {
				return false;
			}
			break;
		default:
			if (value_flag) {
				value.push_back(c);
			}
		}
		c = file.get();
	}
	return true;
}

double* FileHandler::IndexToDataPtr_(InputData& data, int index) {
	switch (index) {
	case 0:
		return &data.t;
	case 1:
		return &data.p1;
	case 2:
		return &data.T1;
	case 3:
		return &data.kap;
	case 4:
		return &data.R;
	case 5:
		return &data.m0;
	case 6:
		return &data.m1;
	case 7:
		return &data.h0;
	case 8:
		return &data.h1;
	case 9:
		return &data.F0;
	case 10:
		return &data.eps;
	default:
		return nullptr;
	}
}