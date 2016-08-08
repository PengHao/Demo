#pragma once
//#include "unitiy.h"
#include <fstream>
#include <string>
using namespace std;


typedef enum{
	OpenFileRW	= 1,
	OpenFileR	= 2,
    OpenFileW	= 3,
    OpenFileAPP	= 4,
    OpenFileCleanW	= 5,
} OpenFileType;

class MyFstream
{
public:
	MyFstream(string _filename, OpenFileType type);
	~MyFstream(void);
public:
	string filename;
	size_t filesize;
	fstream *fileStreams;
	OpenFileType openType;
	bool hasopened;

	size_t read(char *data, size_t dataSize);
    size_t get_line(char *data, size_t dataSize);
    
    std::fstream::pos_type position();
    void jump(const std::fstream::pos_type &pos);
    
	void write(const void *data, size_t len);
	char dataBuffer[];
	bool fail(void);
    
    void flush();
};

