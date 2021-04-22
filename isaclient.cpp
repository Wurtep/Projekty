#include "isaclient.h"

int main(int argc, char *argv[]) {
	int opt;
	string hostname;
	int port;
	bool H_flag = false;
	bool p_flag = false;
	// parsing switch arguments
	while ((opt = getopt(argc, argv, ":hp:H:")) != -1) {
		switch(opt) {
			case 'h':
				cout << "Launching this application with -h parameter will show this help." << endl;
				cout << "Launch this program like this: \"./isaclient -H <host> -p <port> <command>\", where <host> is name of host server and <port> is port of server. <command> is one of the following:" << endl;
				cout << "\"boards\" represents \"GET/BOARDS\" - will show all available boards" << endl;
				cout << "\"board add <name>\" represents \"POST /boards/<name>\" - creates new board <name>" << endl;
				cout << "\"board delete <name>\" represents \"DELETE /boards/<name>\" - deletes board <name>" << endl;
				cout << "\"board list <name>\" represents \"GET /board/<name>\" - lists content of board with <name>" << endl;
				cout << "\"item add <name> <content>\" represents \"POST /board/<name>\" - adds new post to board <name> with <content>" << endl;
				cout << "\"item delete <name> <id>\" represents \"DELETE /board/<name>/<id>\" - deletes post with <id> in board <name>" << endl;
				cout << "\"item update <name> <id> <content>\" represents \"PUT /board/<name>/<id>\" - updates post with <id> in board <name> with <content>" << endl;
				return 0;
			case 'H':
				H_flag = true;
				hostname = optarg;
				break;
			case 'p':{
				string s_port(optarg);
				stringstream ss(s_port);
				ss >> port;
				p_flag = true;
				break;
				}
			case '?':
				cerr << "Incorrect use of application. Try ./isaclient -h for more information.\n";
				return 1;
		}
	}

	// make sure that host and port are set
	if (H_flag == false || p_flag == false) { cerr << "-H, -p, or both are missing\n"; return 1;}
	
	int id, request_type = 0;
	string board_name, content;
	// parsing remaining arguments not parsed by getopt() above
	for (int i = 0; i < argc; i++) {
		// boards - GET /boards {
		if ("boards" == string(argv[i]) && argc == i + 1) {
			request_type = 1;
			break;
		}
		else if ("board" == string(argv[i])) {
			// board add <name> - POST /boards/<name> 
			if (i + 2 < argc && "add" == string(argv[i+1])) {
				board_name = argv[i+2];
				request_type = 2;
				break;
			}
			// board delete <name> - DELETE /boards/<name>
			else if (i + 2 < argc && "delete" == string(argv[i+1])) {
				board_name = argv[i+2];
				request_type = 3;
				break;
			}
			// board list <name> - GET /board/<name>
			else if (i + 2 < argc && "list" == string(argv[i+1])) {
				board_name = argv[i+2];
				request_type = 4;
				break;
			}			
		}
		else if ("item" == string(argv[i])) { 
			// item add <name> <content> - POST /board/<name>
			if (i + 3 < argc && "add" == string(argv[i+1])) {
				board_name = argv[i+2];
				content = argv[i+3];
				request_type = 5;
				break;
			}
			// item delete <name> <id> - DELETE /board/<name>/id
			else if (i + 3 < argc && "delete" == string(argv[i+1])) {
				board_name = argv[i+2];
				string s_id(argv[i+3]);
				stringstream ss(s_id);
				ss >> id;
				request_type = 6;
				break;
			}
			// item update <name> <id> <content> - PUT /board/<name>/id
			else if (i + 4 < argc && "update" == string(argv[i+1])) {
				board_name = argv[i+2];
				string s_id(argv[i+3]);
				stringstream ss(s_id);
				ss >> id;
				content = argv[i+4];
				request_type = 7;
				break;
			}
		}
	}

	// construct http request according to program arguments
	string http_request;
	switch (request_type) {
		case 0:
			cerr << "Wrong request. Try ./isaclient -h for more information.\n";
			return 1;
		case 1:
			http_request = "GET /boards HTTP/1.1\r\nHost: " + hostname + "\r\nAccept: text/plain\r\nContent-Length: 0\r\n\r\n";
			break;
		case 2:
			http_request = "POST /boards/" + board_name + " HTTP/1.1\r\nHost: " + hostname + "\r\nContent-Length: 0\r\nContent-type: text/plain\r\n\r\n";
			break;
		case 3:
			http_request = "DELETE /boards/" + board_name + " HTTP/1.1\r\nHost: " + hostname + "\r\nContent-Length: 0\r\n\r\n";
			break;
		case 4:
			http_request = "GET /board/" + board_name + " HTTP/1.1\r\nHost: " + hostname + "\r\nAccept: text/plain\r\nContent-Length: 0\r\n\r\n";
			break;
		case 5: {
			int content_length_int = int(content.length());
			string content_length = to_string(content_length_int);
			http_request = "POST /board/" + board_name + " HTTP/1.1\r\nHost: " + hostname + "\r\nContent-type: text/plain\r\nContent-Length: " + content_length + "\r\n\r\n" + content;
			}
			break;
		case 6: {
			string str_id = to_string(id);
			http_request = "DELETE /board/" + board_name + "/" + str_id +" HTTP/1.1\r\nHost: " + hostname + "\r\nContent-Length: 0\r\n\r\n";
			}
			break;
		case 7: {
			string str_id = to_string(id);
			int content_length_int = int(content.length());
			string content_length = to_string(content_length_int);
			http_request = "PUT /board/" + board_name + "/" + str_id +" HTTP/1.1\r\nHost: " + hostname + "\r\nContent-type: text/plain\r\nContent-Length: " + content_length + "\r\n\r\n" + content;
			}
			break;			
	}

	// creating socket, setting up various server variables, connecting
	int client_socket;
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		cerr << "Socket creation failed\n";
		return 1;
	}

	struct hostent *host;
	host = gethostbyname(hostname.c_str());
	char *IP;
	IP = inet_ntoa(*((struct in_addr*)host->h_addr_list[0]));
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = inet_addr(IP);
	server_address.sin_port = htons(port);

	if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
		cerr << "Unable to connect to server\n";
		return 1;
	}

	// fill buffer, write to server, receive response
	char buffer[32768];
	memset(buffer, 0, sizeof(buffer));
	strcpy(buffer, http_request.c_str());
	write(client_socket, buffer, sizeof(buffer));

	memset(buffer, 0, sizeof(buffer));
	read(client_socket, buffer, sizeof(buffer));
	
	// parse response
	switch (request_type) {
		case 1:
			if (strncmp(buffer, "HTTP/1.1 200 OK", 15) == 0) {
				write_OKheaders_to_std_err(buffer);
			}
			else if (strncmp(buffer, "HTTP/1.1 404 Not Found", 22) == 0) {
				write_headers_to_stderr(buffer);
			}
			break;
		case 2:
			write_headers_to_stderr(buffer);
			break;
		case 3:
			write_headers_to_stderr(buffer);
			break;
		case 4:
			if (strncmp(buffer, "HTTP/1.1 200 OK", 15) == 0) {
				write_OKheaders_to_std_err(buffer);
			}
			else if (strncmp(buffer, "HTTP/1.1 404 Not Found", 22) == 0) {
				write_headers_to_stderr(buffer);
			}
			break;
		case 5:
			write_headers_to_stderr(buffer);
			break;
		case 6:
			write_headers_to_stderr(buffer);
			break;
		case 7:
			write_headers_to_stderr(buffer);
			break;
	}

	close(client_socket);
	return 0;
}
