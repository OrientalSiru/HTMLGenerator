#pragma once
//piazza post: https://piazza.com/class/iy0nxnbxdsf6m9?cid=61
class BufferManager {
private:
	char* ptr;
public:
	BufferManager() {
		ptr = NULL;
	}
	~BufferManager() {
		if (ptr != NULL)
			delete ptr;
	}
	char* Allocate(int size) {
		if (ptr != NULL)
			delete ptr;

		ptr = new char[size];
		return ptr;
	}

	char* GetBuffer(void) {
		return ptr;
	}
};