#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <unistd.h>
#include <netdb.h>

#include <cstring>

using namespace std;

// structure for posts
// order - order of post in board
// content - content of post
struct post {
	int order;
	string content;
};

// structure representing board
// name - name of board
// posts - all posts that are in this board
struct board {
	string name;
	vector<post> posts;
};

// boards is variable containing all boards
vector<board> boards;


// checks whether board exists or not
bool board_exists(string board_name) {
	bool exists = false;
	for (unsigned int i = 0; i < boards.size(); i++) {
		if (board_name == boards.at(i).name) {
			exists = true;
		}
	}
	return exists;
}

// parse http message, get board name
string get_board_name(char *buff, int n) {
	string cut = string(buff).substr(n);
	string board_name = "";
	for (unsigned int i = 0; i < cut.length(); i++) {
		if (cut.at(i) != ' ' && cut.at(i) != '/') {
			board_name.push_back(cut.at(i));
		}
		else {
			break;
		}
	}
	return board_name;
}

// get body of http message
string get_content(char *buff) {
	string http = buff;
	int pos1 = http.find("\r\n\r\n");
	string message_body = http.substr(pos1+4);
	return message_body;
}

// get id from http message
int get_post_id(char *buff, int n) {
	string cut = string(buff).substr(n);
	string board_name = "";
	for (unsigned int i = 0; i < cut.length(); i++) {
		if (cut.at(i) != ' ') {
			board_name.push_back(cut.at(i));
		}
		else {
			break;
		}
	}
	string delimiter = "/";
	size_t pos = board_name.find(delimiter);
	string s_id = board_name.substr(pos+1);
	int id;
	stringstream ss(s_id);
	ss >> id;
	return id;

}

// check whether post with post_id exists or not
bool post_exists(string board_name, int post_id) {
	bool postExists = false;
	for (unsigned int i = 0; i < boards.size(); i++) {
		if (board_name == boards.at(i).name) {
			for (unsigned int j = 0; j < boards.at(i).posts.size(); j++) {
				if (post_id == boards.at(i).posts.at(j).order) {
					postExists = true;
				}
			}
		}
	}
	return postExists;
}

// adds new board
void board_add(string new_board_name) {
    board new_board;
    new_board.name = new_board_name;
    boards.push_back(new_board);
}

// deletes board
void board_delete(string board_to_be_deleted) {
    for (int i = 0; i < int(boards.size()); i++) {
        if (boards[i].name == board_to_be_deleted) {
            boards.erase(boards.begin() + i);
        }
    }
}

// show content of board
string board_list(string board_to_list) {
	string board_posts = "[" + board_to_list + "]\n";
    for (int i = 0; i < int(boards.size()); i++) {
        if (boards[i].name == board_to_list) {
            for (int j = 0; j < int(boards[i].posts.size()); j++) {
                board_posts.append(to_string(j+1));
                board_posts.append(". ");
                board_posts.append(boards[i].posts[j].content);
                board_posts.append("\n");
            }
        }
    }
    string b_posts_replaced = ""; // a\nc
    for (unsigned int i = 0; i < board_posts.length(); i++) {
    	if (board_posts.at(i) == '\\' && board_posts.at(i+1) == 'n') {
    		b_posts_replaced.push_back('\n');
    		i++;
    	}
    	else {
    		b_posts_replaced.push_back(board_posts.at(i));
    	}
    }
    return b_posts_replaced;
}

// add new post to board
void item_add(string board_to_add_to, string content_to_add) {
    for (int i = 0; i < int(boards.size()); i++) {
        if (boards[i].name == board_to_add_to) {
            post new_post;
            new_post.content = content_to_add;
            new_post.order =  int(boards[i].posts.size() + 1);
            boards[i].posts.push_back(new_post);
        }
    }
}

// delete post from board specified by id
void item_delete(string board_to_delete_from, int id) {
    for (int i = 0; i < int(boards.size()); i++) {
        if (boards[i].name == board_to_delete_from) {
            for (int j = 0; j < int(boards[i].posts.size()); j++) {
                if (boards[i].posts[j].order == id) {
                    boards[i].posts.erase(boards[i].posts.begin() + j);
                    for (int k = 0; k < int(boards[i].posts.size()); k++) {
                        boards[i].posts[k].order = k + 1;
                    }
                }
            }
        }
    }
}

// update post in board specified by id
void item_update(string board_to_update_post_in, string new_content, int id) {
    for (int i = 0; i < int(boards.size()); i++) {
        if (boards[i].name == board_to_update_post_in) {
            for (int j = 0; j < int(boards[i].posts.size()); j++) {
                if (boards[i].posts[j].order == id) {
                    boards[i].posts[j].content = new_content;
                }
            }
        }
    }
}