#pragma once
//#include "unitiy.h"
#include <fstream>
#include <string>
using namespace std;


typedef enum{
	OpenFileRW	= 1,
	OpenFileR	= 2,
    OpenFileW	= 3,
    OpenFileAPP	= 4
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

	void *read(void);
	void *read(size_t dataSize);
    char *get_line(void);
	void write(const void *data, size_t len);
	char dataBuffer[];
	bool fail(void);
    
    void flush();
};

