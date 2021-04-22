#include "isaserver.h"

int main(int argc, char *argv[]) {
	int opt, port;
	bool p_flag = false;
	// parsing required arguments
	while ((opt = getopt(argc, argv, ":hp:")) != -1) {
		switch(opt) {
			case 'h':
				cout << "Launching this application with -h parameter will show this help." << endl;
				cout << "Launch this program like this: \"./isaserver -p <port>\", where <port> is port on which is the server running." << endl;
				return 0;
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

	// port param missing
	if (p_flag == 0) {
		cerr << "Missing -p. Try ./isaclient -h for more information.\n";
		return 1;
	}

	int server_socket;
	if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		cerr << "Socket creation failed\n";
		return 1;
	}

	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(port);


	if ((bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address))) != 0) {
		cerr << "Binding failed\n";
		return 1;
	}

	if ((listen(server_socket, 10)) != 0) {
		cerr << "Server does not listen()\n";
		return 1;
	}

	struct sockaddr_in client_address;
	unsigned int address_length = sizeof(client_address);

	char buffer[32768];
	string http_response = "";
	while (1) {
		int connecting_client = accept(server_socket, (struct sockaddr *)&server_address, &address_length);
		
		if (connecting_client < 0) {
			cerr << "Could not accept connection from client\n";
		}

		memset(buffer, 0, sizeof(buffer));
		read(connecting_client, buffer, sizeof(buffer));

		// boards
		if (strncmp(buffer, "GET /boards HTTP/1.1", 20) == 0) {
			if (boards.size() == 0) {
				http_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}
			else {
				string board_names;
				for (unsigned int i = 0; i < boards.size(); i++) {
				    board_names = board_names + boards.at(i).name + "\n";
				}
				http_response = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(board_names.length()) + "\r\nConnection: close\r\nContent-type: text/plain\r\n\r\n" + board_names;
			}
		}
		// board add <name>
		else if (strncmp(buffer, "POST /boards/", 13) == 0) {
			string board_name = get_board_name(buffer, 13);
			bool boardExists = board_exists(board_name);
			if (board_name.length() == 0){
				http_response = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}
			else if (boardExists == false) {
				board_add(board_name);
				http_response = "HTTP/1.1 201 Created\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}
			else {
				http_response = "HTTP/1.1 409 Conflict\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}

		}
		// board delete <name>
		else if (strncmp(buffer, "DELETE /boards/", 15) == 0) {
			string board_name = get_board_name(buffer, 15);
			bool boardExists = board_exists(board_name);
			if (boardExists == true) {
				board_delete(board_name);
				http_response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}
			else {
				http_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}

		}
		// board list <name>
		if (strncmp(buffer, "GET /board/", 11) == 0) {
			string board_name = get_board_name(buffer, 11);
			bool boardExists = board_exists(board_name);
			if (boardExists == true) {
				string posts = board_list(board_name);
				http_response = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(posts.length()) + "\r\nConnection: close\r\nContent-type: text/plain\r\n\r\n" + posts;
			}
			else {
				http_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}
		}
		// item add <name> <content>
		else if (strncmp(buffer, "POST /board/", 12) == 0)  { 
			string board_name = get_board_name(buffer, 12);
			string content = get_content(buffer);
			bool boardExists = board_exists(board_name);
			if (content.length() == 0) {
				http_response = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}
			else if (boardExists == true) {
				item_add(board_name, content);
				http_response = "HTTP/1.1 201 Created\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}
			else {
				http_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}
		}
		else if (strncmp(buffer, "DELETE /board/", 14) == 0)  {
			string board_name = get_board_name(buffer, 14);
			bool boardExists = board_exists(board_name);
			int id = get_post_id(buffer, 14);
			if (boardExists == true) {
				bool postExists = post_exists(board_name, id);
				if (postExists == true) {
					item_delete(board_name, id);
					http_response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
				}
				else {
					http_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
				}			
			}
			else {
				http_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}
		}
		else if (strncmp(buffer, "PUT /board/", 11) == 0)  { // dodelat PUT error s content len 0
			string board_name = get_board_name(buffer, 11);
			bool boardExists = board_exists(board_name);
			int id = get_post_id(buffer, 11);
			string content = get_content(buffer);
			if (content.length() == 0) {
				http_response = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}
			else if (boardExists == true) {
				bool postExists = post_exists(board_name, id);
				if (postExists == true) {
					item_update(board_name, content, id);
					http_response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
				}
				else {
					http_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
				}	
			}
			else {
				http_response = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			}
		}

		strcpy(buffer, http_response.c_str());
		write(connecting_client, buffer, sizeof(buffer));
		close(connecting_client);
	}

	close(server_socket);

	return 0;
}
