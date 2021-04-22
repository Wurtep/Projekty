#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <cstring>

using namespace std;

// write headers to stderr if code is 2XX
void write_OKheaders_to_std_err(char *buff) {
	string message = buff;
	int head_X_body = message.find("\r\n\r\n");
	int first_line = message.find("\n");
	int how_far = head_X_body - first_line;
	cerr << message.substr(first_line+1, how_far-1);
	cout << message.substr(head_X_body+4);
}

// write headers to stderr if code is 4XX
void write_headers_to_stderr(char *buff) {
	string message = buff;
	int first_line = message.find("\n");
	cerr << message.substr(first_line+1);
}
