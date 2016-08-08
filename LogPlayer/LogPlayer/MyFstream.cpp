#include "MyFstream.h"


MyFstream::MyFstream(string _filename,  OpenFileType type) :
fileStreams(nullptr),
hasopened(true),
filename(_filename),
openType(type)
{
	fileStreams = new(fstream);
	switch (type) {
		case OpenFileR:
			fileStreams->open(filename.c_str(), ios_base::in|ios_base::binary);
            break;
        case OpenFileW:
            fileStreams->open(filename.c_str(), ios_base::binary|ios_base::out);
            break;
        case OpenFileCleanW:
            fileStreams->open(filename.c_str(), ios_base::binary|ios_base::out|ios_base::trunc);
            break;
        case OpenFileAPP:
			fileStreams->open(filename.c_str(), ios_base::binary|ios_base::app);
			break;
		default:
			fileStreams->open(filename.c_str(), ios_base::in|ios_base::binary|ios_base::out);
			break;
	}
	
	if(fileStreams->fail()){
		printf("error:cannot open file %s\n", filename.c_str());
	}else {
		hasopened = true;
        fileStreams->seekg (0, ios::end);
        filesize = (size_t)fileStreams->tellg();
        if (type != OpenFileAPP) {
            fileStreams->seekg (0, ios::beg);
        }
	}
}

MyFstream::~MyFstream(void){
	if(!fileStreams->fail())
		fileStreams->close();
	delete(fileStreams);
}

//读取文件

size_t MyFstream::read(char *data, size_t dataSize){
    if (dataSize == 0) {
        dataSize = filesize;
    }
	fileStreams->read(data, dataSize);
	return dataSize;
}

size_t MyFstream::get_line(char *data, size_t dataSize){
    fileStreams->getline(data, dataSize);
    return dataSize;
}

std::fstream::pos_type MyFstream::position() {
    return fileStreams->tellg();
}

//写入文件
void MyFstream::write(const void *data, size_t len){
	fileStreams->write((const char *)data, len);
}

void MyFstream::jump(const std::fstream::pos_type &pos) {
    fileStreams->seekg(pos);
}


bool MyFstream::fail(void){
	return hasopened;
}

void MyFstream::flush() {
    fileStreams->flush();
}