#include "MyFstream.h"


MyFstream::MyFstream(string _filename,  OpenFileType type){
	hasopened = true;
	filename = _filename;
	fileStreams = new(fstream);
	openType = type;
	switch (type) {
		case OpenFileR:
			fileStreams->open(filename.c_str(), ios_base::in|ios_base::binary);
			break;
        case OpenFileW:
            fileStreams->open(filename.c_str(), ios_base::binary|ios_base::out);
            break;
        case OpenFileAPP:
			fileStreams->open(filename.c_str(), ios_base::binary|ios_base::app);
			break;
		default:
			fileStreams->open(filename.c_str(), ios_base::in|ios_base::binary|ios_base::out);
			break;
	}
	
	if(fileStreams->fail()){
		printf("error:cannot open file\n");
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
void *MyFstream::read(size_t dataSize){
    void *data = malloc(dataSize);
    memset(data, 0, dataSize);
	fileStreams->read((char *)data, dataSize);
	return data;
}

char *MyFstream::get_line(void){
    //can not be more tha 256
    char *str_line = (char *)malloc(256*sizeof(char));
    memset(str_line, 0, 256*sizeof(char));
    fileStreams->getline(str_line, 256*sizeof(char));
    //    DBG("%s",str_line);
    return str_line;
}

void *MyFstream::read(void){
	return this->read(filesize);
}

//写入文件
void MyFstream::write(const void *data, size_t len){
	fileStreams->write((const char *)data, len);
}



bool MyFstream::fail(void){
	return hasopened;
}

void MyFstream::flush() {
    fileStreams->flush();
}