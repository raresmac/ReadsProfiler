#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <stdarg.h>
#include <pwd.h>

#define GENERIC_ERROR "There has been an error!\n"
#define INPUT_ERROR "Please enter a valid command!\n"
#define SOCKET_ERROR "socket() error!\n"
#define SEND_ERROR "send() error!\n"
#define READ_ERROR "read() error!\n"
#define BIND_ERROR "bind() error!\n"
#define LISTEN_ERROR "listen() error!\n"
#define ACCEPT_ERROR "accept() error!\n"
#define SEARCH_ERROR "xml_search() error!\n"
#define THREAD_ERROR "pthread_create() error!\n"
#define SERVER_ERROR "There has been an error within the server!!\n"

#define DOWNLOAD_LOCATION "./Downloads/"

#define PORT 1430 //used port
#define NR_THREADS 1001 //max number of threads
#define NORMAL_LEN 256 //max len of most char*
#define FILENAME_LEN 512 //max len of file names
#define MAX_GENRES 16 //max number of genres per book
#define MAX_BOOKS 128 //max number of books in a shelf
#define SLEEP_TIME 0.5 //amount of time between reading from/writing to server

//login/register commands
#define LOGIN 1
#define REGISTER 2

//user commands
#define SEE_PROFILE 3
#define ADD_BOOK 4
#define REMOVE_BOOK 5
#define SEARCH_BOOK 6
#define SEARCH_AUTHOR 7
#define SEARCH_PUBLISHER 8
#define SEARCH_GENRE 9
#define SEARCH_ISBN 10
#define GET_RECOMMENDATIONS 11
#define CHANGE_PASSWORD 12
//#define SEARCH_TAG 10

//admin commands
#define ADD_BOOK_ADMIN 101
#define ADD_AUTHOR_ADMIN 102
#define ADD_PUBLISHER_ADMIN 103
#define ADD_GENRE_ADMIN 104
#define EDIT_BOOK 105
#define EDIT_AUTHOR 106
#define DELETE_BOOK 107
#define DELETE_AUTHOR 108
#define DELETE_PUBLISHER 109
#define DELETE_GENRE 110
//#define ADD_TAG_ADMIN 111
//#define DELETE_TAG 112

#define COMMAND_EXIT 1000 //command for closing a client

typedef struct thData{
	int idThread; //thread id
	int cl; //descriptor
}thData;

int number_ids[5]; //id_data.txt
//0 - users
//1 - books
//2 - authors
//3 - publishers
//4 - genres
//5 was tags