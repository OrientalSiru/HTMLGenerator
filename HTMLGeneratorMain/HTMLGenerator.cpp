#include "stdafx.h"
#include "HTMLGenerator.h"

const char HTMLGenerator::characters[64] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6',
'7', '8', '9', '0', '0' };

HTMLGenerator::HTMLGenerator(LONG seed) {
	printf("HTML Generator started...\n");
	//initialize MT
	MT.init_genrand(seed);

	//initial uniform integer distrbution
	distribution = new std::uniform_int_distribution<int>(MINIMUM_LINKS, MAXIMUM_LINKS);

	BufferManager bm;
	/*int bytes = 0;
	makeGetResponse(&bm, bytes);
	int a = 0;*/
	int bytes;
	for (int i = 0; i < 1e5; i++) {
		makeGetResponse(&bm, bytes);
		//printf("%s\n", bm.GetBuffer());
	}
}

HTMLGenerator::~HTMLGenerator() {

}

void HTMLGenerator::randString(char* ptr) {
	int u = MT.genrand_int32();
	for (int i = 0; i < 5; i++) {
		int index = (u >> (6 * i)) & ((1 << 6) - 1);
		ptr[i] = characters[index];
	}
}

void HTMLGenerator::makeURL(char* ptr) {
	int offset = 0;
	memcpy(ptr, "http://", 7);
	offset += 7;

	randString(ptr + offset);
	offset += 4;

	ptr[offset] = '.';
	offset++;

	randString(ptr + offset);
	offset += 4;

	memcpy(ptr + offset, ".com/", 5);
	offset += 5;

	randString(ptr + offset);
	offset += 4;

	ptr[offset] = '?';
	offset++;

	randString(ptr + offset);
	offset += 4;

	//printf("offset = %d\n", offset);
}

void HTMLGenerator::makeHeadResponse(BufferManager* bm, int& bytes) {
	bytes = 51;
	char* buf = bm->Allocate(52);
	strcpy(buf, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n");
}

void HTMLGenerator::makeGetResponse(BufferManager* bm, int& bytes) {
	int r = (*distribution)(generator); //number of links per page
	int count = stats.count(r);
	if (count == 0) {
		stats.insert({ r, 1 });
	}
	else {
		stats[r]++;
	}

	int htmlSize = 66 + 91 * r + 16 + 1; //<html>... + (<div>...)*r + </body>... '\0'
	char* html = new char[htmlSize];
	int offset = 0;
	memcpy(html + offset, "<html>\r\n<head>\r\n<title>Test Http Server</title>\r\n</head>\r\n<body>\r\n", 66); //"\r" is one byte, same as "\n"
	offset += 66;
	char* url = new char[31];
	for (int i = 0; i < r; i++) {
		memcpy(html + offset, "<div><a href='", 14);
		offset += 14;
		makeURL(url);
		memcpy(html + offset, url, 30); //ignore '\0'
		offset += 30;
		memcpy(html + offset, "'>", 2);
		offset += 2;
		memcpy(html + offset, url, 30);
		offset += 30;
		memcpy(html + offset, "</a><br/></div>", 15);
		offset += 15;
	}
	delete[] url;
	memcpy(html + offset, "</body>\r\n</html>", 16);
	offset += 16;
	html[offset] = '\0';

	//calculate how many bytes to assign for content-length in http header
	int contentLengthDigits = 0;
	int temp = htmlSize;
	while (temp > 0) {
		temp /= 10;
		contentLengthDigits++;
	}
	int responseSize = 33 + contentLengthDigits + 8 + htmlSize;

	bm->Allocate(responseSize);

	offset = 0;
	memcpy(bm->GetBuffer(), "HTTP/1.1 200 OK\r\nContent-Length: ", 33);
	offset += 33;
	char* contentLength = new char[contentLengthDigits + 1];
	_itoa(htmlSize, contentLength, 10); //base 10
	memcpy(bm->GetBuffer() + offset, contentLength, contentLengthDigits);
	offset += contentLengthDigits;
	memcpy(bm->GetBuffer() + offset, html, htmlSize); //already null-terminated

	bytes = responseSize;

	delete[] contentLength;
	delete[] html;
}