#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define GENERIC_ERROR "There has been an error!\n"
#define INPUT_ERROR "Please enter a valid command!\n"
#define SOCKET_ERROR "socket() error!\n"
#define SEND_ERROR "send() error!\n"
#define READ_ERROR "read() error!\n"
#define CONNECT_ERROR "connect() error!\n"

#define LOGIN_REGISTER_TEXT "1. Login\n2. Register\n"

#define BUFFER_SIZE 256
#define SLEEP_TIME 0.5

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
//#define SEARCH_TAG 10 -> Nu am avut timp sa implementez tagurile, dar am lasat codul

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

#define COMMAND_EXIT 1000 //exit program

int login_register(int);
void main_menu();
void main_menu_admin();
int create_connection(char *argv[]);
int central_hub(int,int);
void answer_interpreter(int,int);

extern int errno; //error code returned by some functions
int port; //port used by server
int user_status = 0; //user type: 0 - not connected, 1 - default, 2 - admin

int main(int argc,char *argv[])
{
  int sd;
  int command = -1;
  char buffer[BUFFER_SIZE];

  if(argc != 3){ //checking arguments in command line
    printf("Syntax is: %s <server_address> <port>\n", argv[0]);
    return -1;
  }

  port = atoi(argv[2]); //getting port
  if((sd = create_connection(argv)) == errno) return 0;

  if(login_register(sd) != 1){ //login/register
    printf(GENERIC_ERROR);
    return errno;
  }

  while(command != COMMAND_EXIT){
    int answer;
    if(user_status == 1){
      main_menu();
    }
    else if(user_status == 2){
      main_menu_admin();
    }
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){
      printf(READ_ERROR);
      return errno;
    }
    command = atoi(buffer);
    if(command > 0 && command < 13){
      if(user_status == 2){
        command += 100;
      }
      else{
        command += 2;
      }
      central_hub(sd,command);
    }
    else if(command != COMMAND_EXIT){
      printf("Please enter a valid command!\n");
    }
  }
  int nr = COMMAND_EXIT;
  if(send(sd,&nr,sizeof(nr),MSG_NOSIGNAL) <= 0){
    printf(SEND_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  close(sd); //closing connection
}

int create_connection(char *argv[]){
  int sd;//socket descriptor
  struct sockaddr_in server;//structure used for connecting

  if((sd = socket(AF_INET,SOCK_STREAM,0)) == -1){ //creating socket
    perror(SOCKET_ERROR);
    return errno;
  }

  server.sin_family = AF_INET; //socket family
  server.sin_addr.s_addr = inet_addr(argv[1]); //ip address of server
  server.sin_port = htons(port); //port

  if(connect(sd,(struct sockaddr *) &server,sizeof(struct sockaddr)) == -1){ //connecting to server
    perror(CONNECT_ERROR);
    return errno;
  }
  return sd;
}

int book_profile(int sd){
  char buffer[BUFFER_SIZE];
  int nr_genres,nr_users;

  if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving title from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("\n-----BOOK PROFILE-----\n");
  printf("----%s----\n\n",buffer);
  //reading book profile
  if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving isbn from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("ISBN is %s\n",buffer);

  if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving author from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("Author is %s\n",buffer);

  if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving publisher from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("Publisher is %s\n",buffer);

  if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving average from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("Average rating is %s\n",buffer);

  if(read(sd,&nr_genres,sizeof(nr_genres)) < 0){ //receiving number of genres from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("It has %d genre(s)",nr_genres);
  if(nr_genres > 0) printf(":\n");
  else printf("!\n");

  for(int i = 0; i < nr_genres; i++){
    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each genre from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    printf("Genre number %d is %s\n",i+1,buffer);
  }

  /*if(read(sd,&nr_tags,sizeof(nr_tags)) < 0){ //receiving number of tags from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("It has %d tag(s)",nr_tags);
  if(nr_tags > 0) printf(":\n");
  else printf("!\n");

  for(int i = 0; i < nr_tags; i++){
    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each tag from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    printf("Tag number %d is %s\n",i+1,buffer);
  }*/

  if(read(sd,&nr_users,sizeof(nr_users)) < 0){ //receiving number of users from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("It has %d user(s)",nr_users);
  if(nr_users > 0) printf(":\n");
  else printf("!\n");

  for(int i = 0; i < nr_users; i++){
    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each user from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    printf("User number %d is %s, ",i+1,buffer);

    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving status/review from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    if(buffer[0] > '0' && buffer[0] < '6'){
      printf("giving it %s stars\n",buffer);
    }
    else{
      printf("with the status %s\n",buffer);
    }
  }

  printf("Do you want to download the book? Y/N\n");
  buffer[0] = '0';
  while(buffer[0] != 'Y' && buffer[0] != 'y' && buffer[0] != 'N' && buffer[0] != 'n' && buffer[1] != '\n'){
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving search result from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    if(buffer[0] != 'Y' && buffer[0] != 'y' && buffer[0] != 'N' && buffer[0] != 'n' && buffer[1] != '\n'){
      printf("Please insert a valid answer! Y/N\n");
    }
  }

  if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending answer to server
    printf(SEND_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  return 1;
}

int author_profile(int sd){
  char buffer[BUFFER_SIZE];
  int nr_books;

  if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving author from server
    printf(READ_ERROR);
    return errno;
  }
  printf("\n-----AUTHOR PROFILE-----\n");
  printf("----%s----\n\n",buffer);
  //reading author profile
  if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving birth place from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("Birth place is %s\n",buffer);

  if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving birth date from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("Birth date is %s\n",buffer);

  if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving death date from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  if(buffer[0] == '-'){
    printf("The author is still alive!\n");
  }
  else{
    printf("Death date is %s\n",buffer);
  }

  if(read(sd,&nr_books,sizeof(nr_books)) < 0){ //receiving number of books from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("They have written %d book(s):\n",nr_books);

  for(int i = 0; i < nr_books; i++){
    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each book from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    printf("Book number %d is %s\n",i+1,buffer);
  }
  return 1;
}

int publisher_profile(int sd){
  char buffer[BUFFER_SIZE];
  int nr_books;

  if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving publisher from server
    printf(READ_ERROR);
    return errno;
  }
  printf("\n-----PUBLISHER PROFILE-----\n");
  printf("----%s----\n\n",buffer);
  //reading publisher profile
  if(read(sd,&nr_books,sizeof(nr_books)) < 0){ //receiving number of books from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("They have published %d book(s):\n",nr_books);

  for(int i = 0; i < nr_books; i++){
    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each book from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    printf("Book number %d is %s\n",i+1,buffer);
  }
  return 1;
}

int genre_profile(int sd){
  char buffer[BUFFER_SIZE];
  int nr_books;

  if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving genre from server
    printf(READ_ERROR);
    return errno;
  }
  printf("\n-----GENRE PROFILE-----\n");
  printf("----%s----\n\n",buffer);
  //reading genre profile
  if(read(sd,&nr_books,sizeof(nr_books)) < 0){ //receiving number of books from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("There are %d books with this genre:\n",nr_books);

  for(int i = 0; i < nr_books; i++){
    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each book from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    printf("Book number %d is %s\n",i+1,buffer);
  }
  return 1;
}

/*int tag_profile(int sd){
  char buffer[BUFFER_SIZE];
  int nr_books;
  printf("\n------TAG PROFILE------\n\n");
  //reading tag profile
  if(read(sd,&nr_books,sizeof(nr_books)) < 0){ //receiving number of books from server
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  printf("There are %d books with this tag:\n",nr_books);

  for(int i = 0; i < nr_books; i++){
    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each book from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    printf("Book number %d is %s\n",i+1,buffer);
  }
  return 1;
}*/

int login_register(int sd){
  char buf1[BUFFER_SIZE],buf2[BUFFER_SIZE];
  memset(buf1,0,BUFFER_SIZE);
  memset(buf2,0,BUFFER_SIZE);
  printf(LOGIN_REGISTER_TEXT);
  int nr,nr2;
  if(fgets(buf2,100,stdin) == NULL){
    printf(READ_ERROR);
    return errno;
  }
  sleep(SLEEP_TIME);
  fflush(stdout);
  buf2[strlen(buf2)-1]='\0';
  nr=atoi(buf2);
  while(user_status == 0){
    while(nr != 1 && nr != 2){
        printf(INPUT_ERROR);
        if(fgets(buf2,BUFFER_SIZE,stdin) == NULL){
          printf(READ_ERROR);
          return errno;
        }
        sleep(SLEEP_TIME);
        fflush(stdout);
        buf2[strlen(buf2)-1]='\0';
        nr = atoi(buf2);
    }
    nr2 = nr;
    printf("Please enter your username: ");
    if(fgets(buf1,BUFFER_SIZE,stdin) == NULL){
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    buf1[strlen(buf1)-1]='\0';
    printf("Please enter your password: ");
    if(fgets(buf2,BUFFER_SIZE,stdin) == NULL){
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    buf2[strlen(buf2)-1]='\0';
    printf("%d -- %s -- %s\n",nr2,buf1,buf2);

    if(send(sd,&nr2,sizeof(int),MSG_NOSIGNAL) <= 0){//sending messages to server
        printf(SEND_ERROR);
        return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buf1,sizeof(buf1),MSG_NOSIGNAL) <= 0){
        printf(SEND_ERROR);
        return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buf2,sizeof(buf2),MSG_NOSIGNAL) <= 0){
        printf(SEND_ERROR);
        return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    if(read(sd,&nr,sizeof(int)) < 0){//receiving answer from server
        printf(READ_ERROR);
        return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    if(nr == 0 && nr2 == 1){
        printf("The username and password are incorrect! Please enter again!\n\n");
        printf(LOGIN_REGISTER_TEXT);
        if(fgets(buf2,BUFFER_SIZE,stdin) == NULL){
          printf(READ_ERROR);
          return errno;
        }
        sleep(SLEEP_TIME);
        fflush(stdout);
        buf2[strlen(buf2)-1]='\0';
        nr=atoi(buf2);
    }
    else if(nr == 0 && nr2 == 2){
        printf("The username already exists! Please enter a new one!\n\n");
        printf(LOGIN_REGISTER_TEXT);
        if(fgets(buf2,BUFFER_SIZE,stdin) == NULL){
          printf(READ_ERROR);
          return errno;
        }
        sleep(SLEEP_TIME);
        fflush(stdout);
        buf2[strlen(buf2)-1]='\0';
        nr=atoi(buf2);
    }
    else if(nr2 == 1){
        user_status = nr;
        printf("You have been logged in");
        if(user_status == 1) printf("!\n");
        else if(user_status == 2) printf(" as an admin!\n\n");
        return 1;
    }
    else{
        user_status = 1;
        printf("Your account has been created!\n\n");
        return 1;
    }
  }
}

void main_menu(){
  printf("\nThis is the main menu:\n");
  printf("%d. See your profile\n",SEE_PROFILE-2);
  printf("%d. Add a book to your account\n",ADD_BOOK-2);
  printf("%d. Remove a book from your account\n",REMOVE_BOOK-2);
  printf("%d. Search for a book\n",SEARCH_BOOK-2);
  printf("%d. Search for an author\n",SEARCH_AUTHOR-2);
  printf("%d. Search for a publisher\n",SEARCH_PUBLISHER-2);
  printf("%d. Search for a genre\n",SEARCH_GENRE-2);
  printf("%d. Search after ISBN\n",SEARCH_ISBN-2);
  printf("%d. Get recommended books\n",GET_RECOMMENDATIONS-2);
  printf("%d. Change your password\n",CHANGE_PASSWORD-2);
  printf("%d. Exit\n",COMMAND_EXIT);
  //printf("%d. Search for a tag\n",SEARCH_TAG-2);
  fflush(stdout);
}

void main_menu_admin(){
  printf("\nThis is the main menu for the admin:\n");
  printf("%d. Add a book to the database\n",ADD_BOOK_ADMIN-100);
  printf("%d. Add an author to the database\n",ADD_AUTHOR_ADMIN-100);
  printf("%d. Add a publisher to the database\n",ADD_PUBLISHER_ADMIN-100);
  printf("%d. Add a genre to the database\n",ADD_GENRE_ADMIN-100);
  printf("%d. Edit a book\n",EDIT_BOOK-100);
  printf("%d. Edit an author\n",EDIT_AUTHOR-100);
  //printf("%d. Add a tag to the database\n",ADD_TAG_ADMIN-100);
  /*printf("%d. Delete a book\n",DELETE_BOOK-100);
  printf("%d. Delete an author\n",DELETE_AUTHOR-100);
  printf("%d. Delete a publisher\n",DELETE_PUBLISHER-100);
  printf("%d. Delete genre\n",DELETE_GENRE-100);
  printf("%d. Delete a tag\n",DELETE_TAG-100);*/
  printf("%d. Exit\n",COMMAND_EXIT);
  fflush(stdout);
}

void answer_interpreter(int command,int answer){
  if(answer == 0){
    printf("There was a problem with the command! Make sure all the details are correct!\n");
  }
  else if(command == SEE_PROFILE){
  }
  else if(command == ADD_BOOK){
    if(answer == 1){
      printf("The book has been added to your profile!\n");
    }
    else if(answer == -1){
      printf("The book already is in your profile!\n");
    }
  }
  else if(command == REMOVE_BOOK){
    if(answer == 1){
      printf("The book has been removed from your profile\n");
    }
    else if(answer == -1){
      printf("The book isn't in your profile!\n");
    }
  }
  else if(command == SEARCH_BOOK){
    if(answer == 1){
      printf("The book has been downloaded to /Downloads/!\n");
    }
  }
  else if(command == CHANGE_PASSWORD){
    if(answer == 1){
      printf("The password has been changed!\n");
    }
    else{
      printf("There was a problem with changing the password!\n");
    }
  }
  else if(command == ADD_BOOK_ADMIN){
    if(answer == 1){
      printf("The book has been added to the database!\n");
    }
    else if(answer == -1){
      printf("The book already exists in the database!\n");
    }
  }
  else if(command == ADD_AUTHOR_ADMIN){
    if(answer == 1){
      printf("The author has been added to the database!\n");
    }
    else if(answer == -1){
      printf("The author already exists in the database!\n");
    }
  }
  else if(command == ADD_PUBLISHER_ADMIN){
    if(answer == 1){
      printf("The publisher has been added to the database!\n");
    }
    else if(answer == -1){
      printf("The publisher already exists in the database!\n");
    }
  }
  else if(command == ADD_GENRE_ADMIN){
    if(answer == 1){
      printf("The genre has been added to the database!\n");
    }
    else if(answer == -1){
      printf("The genre already exists in the database!\n");
    }
  }
  /*else if(command == ADD_TAG_ADMIN){
    if(answer == 1){
      printf("The tag has been added to the database!\n");
    }
    else if(answer == -1){
      printf("The tag already exists in the database!\n");
    }
  }*/
  else if(command == DELETE_BOOK){
    if(answer == 1){
      printf("The book has been deleted from the database!\n");
    }
    else if(answer == -1){
      printf("The book doesn't exist in the database!\n");
    }
  }
  else if(command == DELETE_AUTHOR){
    if(answer == 1){
      printf("The author has been deleted from the database!\n");
    }
    else if(answer == -1){
      printf("The author doesn't exist in the database!\n");
    }
  }
  else if(command == DELETE_PUBLISHER){
    if(answer == 1){
      printf("The publisher has been deleted from the database!\n");
    }
    else if(answer == -1){
      printf("The publisher doesn't exist in the database!\n");
    }
  }
  else if(command == DELETE_GENRE){
    if(answer == 1){
      printf("The genre has been deleted from the database!\n");
    }
    else if(answer == -1){
      printf("The genre doesn't exist in the database!\n");
    }
  }
  /*else if(command == DELETE_TAG){
    if(answer == 1){
      printf("The tag has been deleted from the database!\n");
    }
    else if(answer == -1){
      printf("The tag doesn't exist in the database!\n");
    }
  }*/
  else if(command > DELETE_GENRE){
    if(answer == 1){
      printf("The information has been modified!\n");
    }
    else if(answer == -1){
      printf("There has been a problem modifying the information! (it could mean that the file doesn't exist)\n");
    }
  }
  else{
    printf("The command has been processed!\n");
  }
  fflush(stdout);
}

int central_hub(int sd,int command){
  char buffer[BUFFER_SIZE];
  int answer;
  if(command == SEE_PROFILE){
    printf("------------------------------\n");
    printf("    This is your profile!\n\n");
    int nr_books;
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    if(read(sd,&nr_books,sizeof(nr_books)) < 0){ //receiving number of "reading" books from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    printf("You have %d book(s) in the \"reading\" shelf",nr_books);
    if(nr_books > 0) printf(":");
    else printf("!");
    printf("\n");
    for(int i = 0; i < nr_books; i++){
      if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving "reading" books from server
        printf(READ_ERROR);
        return errno;
      }
      if(buffer[strlen(buffer) - 1] == '\n'){
        buffer[strlen(buffer) - 1] = '\0';
      }
      sleep(SLEEP_TIME);
      fflush(stdout);
      printf("\"%s\"\n",buffer);
    }

    if(read(sd,&nr_books,sizeof(nr_books)) < 0){ //receiving number of "read" books from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    printf("You have %d book(s) in the \"read\" shelf",nr_books);
    if(nr_books > 0) printf(":");
    else printf("!");
    printf("\n");
    for(int i = 0; i < nr_books; i++){
      if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving "read" books from server
        printf(READ_ERROR);
        return errno;
      }
      char review = buffer[strlen(buffer)-2];
      buffer[strlen(buffer)-3]='\0';
      sleep(SLEEP_TIME);
      fflush(stdout);
      printf("\"%s\", rated %c stars\n",buffer,review);
    }

    if(read(sd,&nr_books,sizeof(nr_books)) < 0){ //receiving number of "reading" books from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    printf("You have %d book(s) in the \"dropped\" shelf",nr_books);
    if(nr_books > 0) printf(":");
    else printf("!");
    printf("\n");
    for(int i = 0; i < nr_books; i++){
      if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving "dropped" books from server
        printf(READ_ERROR);
        return errno;
      }
      if(buffer[strlen(buffer) - 1] == '\n'){
        buffer[strlen(buffer) - 1] = '\0';
      }
      sleep(SLEEP_TIME);
      fflush(stdout);
      printf("\"%s\"\n",buffer);
    }

    sleep(SLEEP_TIME);
    fflush(stdout);
    if(read(sd,&nr_books,sizeof(nr_books)) < 0){ //receiving number of "planning" books from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    printf("You have %d book(s) in the \"planning to read\" shelf",nr_books);
    if(nr_books > 0) printf(":");
    else printf("!");
    printf("\n");
    for(int i = 0; i < nr_books; i++){
      if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving "planning" books from server
        printf(READ_ERROR);
        return errno;
      }
      if(buffer[strlen(buffer) - 1] == '\n'){
        buffer[strlen(buffer) - 1] = '\0';
      }
      sleep(SLEEP_TIME);
      fflush(stdout);
      printf("\"%s\"\n",buffer);
    }

    if(read(sd,&answer,sizeof(answer)) < 0){ //receiving answer from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    answer_interpreter(command,answer);
  }
  else if(command == ADD_BOOK){
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter book's name: ");
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving book's name from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending book's name to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter book status:\n");
    printf("1. Reading\n");
    printf("2. Read\n");
    printf("3. Dropped\n");
    printf("4. Planning to read\n");
    fflush(stdout);
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving status from input
      printf(READ_ERROR);
      return errno;
    }
    int status = atoi(buffer);
    while(status < 1 || status > 4){
      printf("Please enter a valiid book status!\n");
      printf("1. Reading\n");
      printf("2. Read\n");
      printf("3. Dropped\n");
      printf("4. Planning to read\n");
      fflush(stdout);
      if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving status from input
        printf(READ_ERROR);
        return errno;
      }
      status = atoi(buffer);
    }
    sleep(SLEEP_TIME);
    if(send(sd,&status,sizeof(status),MSG_NOSIGNAL) <= 0){ //sending status to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(status == 2){
      int review = 0;
      while(review < 1 || review > 5){
        printf("Please enter the number of stars (max. 5):\n");
        if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving review from input
          printf(READ_ERROR);
          return errno;
        }
        review = atoi(buffer);
        if(review < 1 || review > 5){
          printf("This isn't a valid number of stars!\n");
        }
      }

      if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending book's review to server
        printf(SEND_ERROR);
        return errno;
      }
      sleep(SLEEP_TIME);
      fflush(stdout);
    }

    if(read(sd,&answer,sizeof(answer)) < 0){ //receiving answer from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    answer_interpreter(command,answer);
  }
  else if(command == REMOVE_BOOK){
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter book's name: ");
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving book's name from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending book's name to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    if(read(sd,&answer,sizeof(answer)) < 0){ //receiving answer from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    answer_interpreter(command,answer);
  }
  else if(command == SEARCH_BOOK){
    int nr_results,choose = 0;
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(read(sd,&nr_results,sizeof(nr_results)) < 0){ //receiving number of results from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    printf("There are %d result(s):\n",nr_results);
    for(int i = 0; i < nr_results; i++){
      if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each result from server
        printf(READ_ERROR);
        return errno;
      }
      sleep(SLEEP_TIME);
      printf("%d. %s",i+1,buffer);
    }
    while(choose < 1 || choose > nr_results + 1){
      printf("\nChoose a book or type %d to exit: ",nr_results+1);
      if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving search result from input
        printf(READ_ERROR);
        return errno;
      }
      choose = atoi(buffer);
    }

    if(send(sd,&choose,sizeof(choose),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(choose < nr_results + 1){
      book_profile(sd);
    }
    return 1;
  }
  else if(command == SEARCH_AUTHOR){
    int nr_results,choose = 0;
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(read(sd,&nr_results,sizeof(nr_results)) < 0){ //receiving number of results from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    printf("There are %d result(s):\n",nr_results);
    for(int i = 0; i < nr_results; i++){
      if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each result from server
        printf(READ_ERROR);
        return errno;
      }
      sleep(SLEEP_TIME);
      printf("%d. %s",i+1,buffer);
    }
    while(choose < 1 || choose > nr_results + 1){
      printf("\nChoose an author or type %d to exit: ",nr_results+1);
      if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving search result from input
        printf(READ_ERROR);
        return errno;
      }
      choose = atoi(buffer);
    }

    if(send(sd,&choose,sizeof(choose),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(choose < nr_results + 1){
      author_profile(sd);
    }
  }
  else if(command == SEARCH_PUBLISHER){
    int nr_results,choose = 0;
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(read(sd,&nr_results,sizeof(nr_results)) < 0){ //receiving number of results from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    printf("There are %d result(s):\n",nr_results);
    for(int i = 0; i < nr_results; i++){
      if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each result from server
        printf(READ_ERROR);
        return errno;
      }
      sleep(SLEEP_TIME);
      printf("%d. %s",i+1,buffer);
    }
    while(choose < 1 || choose > nr_results + 1){
      printf("\nChoose a publisher or type %d to exit: ",nr_results+1);
      if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving search result from input
        printf(READ_ERROR);
        return errno;
      }
      choose = atoi(buffer);
    }

    if(send(sd,&choose,sizeof(choose),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(choose < nr_results + 1){
      publisher_profile(sd);
    }
  }
  else if(command == SEARCH_GENRE){
    int nr_results,choose = 0;
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(read(sd,&nr_results,sizeof(nr_results)) < 0){ //receiving number of results from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    printf("There are %d result(s):\n",nr_results);
    for(int i = 0; i < nr_results; i++){
      if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each result from server
        printf(READ_ERROR);
        return errno;
      }
      sleep(SLEEP_TIME);
      printf("%d. %s",i+1,buffer);
    }
    while(choose < 1 || choose > nr_results + 1){
      printf("\nChoose a genre or type %d to exit: ",nr_results+1);
      if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving search result from input
        printf(READ_ERROR);
        return errno;
      }
      choose = atoi(buffer);
    }

    if(send(sd,&choose,sizeof(choose),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    
    if(choose < nr_results + 1){
      genre_profile(sd);
    }
  }
  /*else if(command == SEARCH_TAG){
    int nr_results,choose = 0;
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(read(sd,&nr_results,sizeof(nr_results)) < 0){ //receiving number of results from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    printf("There are %d result(s):\n",nr_results);
    for(int i = 0; i < nr_results; i++){
      if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving each result from server
        printf(READ_ERROR);
        return errno;
      }
      sleep(SLEEP_TIME);
      printf("%d. %s",i+1,buffer);
    }
    while(choose < 1 || choose > nr_results + 1){
      printf("\nChoose a tag or type %d to exit: ",nr_results+1);
      if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving search result from input
        printf(READ_ERROR);
        return errno;
      }
      choose = atoi(buffer);
    }

    if(send(sd,&choose,sizeof(choose),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    tag_profile(sd);
  }*/
  else if(command == SEARCH_ISBN){
    int answer;
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    printf("Please enter the isbn: ");
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving isbn from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending isbn to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(read(sd,&answer,sizeof(answer)) < 0){ //receiving answer from server
      printf(READ_ERROR);
      return errno;
    }

    if(answer == 1){
      book_profile(sd);
    }
    else{
      printf("The ISBN isn't in the database!\n");
    }
  }
  else if(command == GET_RECOMMENDATIONS){
    int ok, i = 1;
    char buffer[BUFFER_SIZE];
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(read(sd,&ok,sizeof(ok)) < 0){ //receiving answer from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    if(ok == 0){
      printf("You don't have enough genres to generate recommendations!\n");
      return 1;
    }
    else{
      printf("These are the recommended books for you:\n");
    }
    
    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving book 1 from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    if(buffer[0] != '-'){
      printf("%d. %s\n",i++,buffer);
    }

    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving book 2 from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    if(buffer[0] != '-'){
      printf("%d. %s\n",i++,buffer);
    }

    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving book 3 from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    if(buffer[0] != '-'){
      printf("%d. %s\n",i++,buffer);
    }

    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving book 4 from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    if(buffer[0] != '-'){
      printf("%d. %s\n",i++,buffer);
    }

    if(read(sd,&buffer,sizeof(buffer)) < 0){ //receiving book 5 from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    if(buffer[0] != '-'){
      printf("%d. %s\n",i++,buffer);
    }
    return 1;
  }
  else if(command == CHANGE_PASSWORD){
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter new password: ");
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving new password from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending new password to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    if(read(sd,&answer,sizeof(answer)) < 0){ //receiving answer from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    answer_interpreter(command,answer);
  }
  else if(command == ADD_BOOK_ADMIN){
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter book's name: ");
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving book's name from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending book's name to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter book's ISBN: ");
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving book's isbn from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending book's isbn to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter book's author: ");
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving book's author from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending book's author to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter book's publisher: ");
    fflush(stdout);
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving publisher from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending publisher to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    printf("Please enter book's number of genres: ");
    fflush(stdout);
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving number of genres from input
      printf(READ_ERROR);
      return errno;
    }
    int nr_genres = atoi(buffer);
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&nr_genres,sizeof(nr_genres),MSG_NOSIGNAL) <= 0){ //sending number of genres to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    for(int i = 1; i <= nr_genres; i++){
      printf("Please enter genre number %d: ",i);
      fflush(stdout);
      if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving genre from input
        printf(READ_ERROR);
        return errno;
      }
      sleep(SLEEP_TIME);
      buffer[strlen(buffer)-1]='\0';
      if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending genre to server
        printf(SEND_ERROR);
        return errno;
      }
      sleep(SLEEP_TIME);
    }

    if(read(sd,&answer,sizeof(answer)) < 0){ //receiving answer from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    answer_interpreter(command,answer);
  }
  else if(command == ADD_AUTHOR_ADMIN){
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter author's name: ");
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving name from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending author's name to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter author's place of birth: ");
    fflush(stdout);
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving place of birth from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending place of birth to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    printf("Please enter author's date of birth: ");
    fflush(stdout);
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving date of birth from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending date of birth to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);

    printf("Please enter author's date of death: ");
    fflush(stdout);
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving date of death from input
      printf(READ_ERROR);
      return errno;
    }
    printf("%s\n",buffer);
    sleep(SLEEP_TIME);
    buffer[strlen(buffer)-1]='\0';
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending date of death to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    if(read(sd,&answer,sizeof(answer)) < 0){ //receiving answer from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    answer_interpreter(command,answer);
  }
  else if(command == ADD_PUBLISHER_ADMIN){
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter publisher's name: ");
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving name from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending publisher's name to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    if(read(sd,&answer,sizeof(answer)) < 0){ //receiving answer from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    answer_interpreter(command,answer);
  }
  else if(command == ADD_GENRE_ADMIN){
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){ //sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter genre: ");
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving name from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending genre to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    if(read(sd,&answer,sizeof(answer)) < 0){ //receiving answer from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    answer_interpreter(command,answer);
  }
  /*else if(command == ADD_TAG_ADMIN){

  }*/
  else if(command == EDIT_BOOK){
    int nr;
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){//sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter for which book you will change information: ");
    if(fgets(buffer,sizeof(buffer),stdin) == NULL){//receiving name from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){//sending name to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("The categories are:\n1. Author\n2. Publisher\n3. Genres\n4.Please enter the category to be changed: ");
    if(fgets(buffer,sizeof(buffer),stdin) == NULL){//receiving name from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    nr = atoi(buffer);
    while(nr < 1 || nr > 3){
      printf("Please choose a valid category!\n");
      printf("The categories are:\n1. Author\n2. Publisher\n3. Genres\nPlease enter the category to be changed: ");
      if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){
        printf(READ_ERROR);
        return errno;
      }
      sleep(SLEEP_TIME);
      fflush(stdout);
      nr = atoi(buffer);
    }
    if(nr == 1){
      strcpy(buffer,"<author>\n");
    }
    else if(nr == 2){
      strcpy(buffer,"<publisher>\n");
    }
    else if(nr == 3){
      strcpy(buffer,"<genres>\n");
    }
    /*else{
      strcpy(buffer,"<tags>\n");
    }*/
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){//sending category to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter the new information for this category: ");
    fflush(stdout);
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving the new info from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending the new info to server
      printf(SEND_ERROR);
      return errno;
    }
    if(nr == 3 || nr == 4){
      int nr_genres = atoi(buffer);
      for(int i = 0; i < nr_genres; i++){
        if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving the new info from input
          printf(READ_ERROR);
          return errno;
        }
        buffer[strlen(buffer)-1]='\0';
        sleep(SLEEP_TIME);
        if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending the new info to server
          printf(SEND_ERROR);
          return errno;
        }
      }
    }
    sleep(SLEEP_TIME);
    if(read(sd,&answer,sizeof(answer)) < 0){ //receiving answer from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    answer_interpreter(command,answer);
  }
  else if(command == EDIT_AUTHOR){
    int nr;
    if(send(sd,&command,sizeof(command),MSG_NOSIGNAL) <= 0){//sending command to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter for which author you will change information: ");
    if(fgets(buffer,sizeof(buffer),stdin) == NULL){//receiving name from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){//sending name to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("The categories are:\n1. Birth place\n2. Birth date\n3. Death date\nPlease enter the category to be changed: ");
    if(fgets(buffer,sizeof(buffer),stdin) == NULL){//receiving name from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    nr = atoi(buffer);
    while(nr < 1 || nr > 3){
      printf("Please choose a valid category!\n");
      printf("The categories are:\n1. Birth place\n2. Birth date\n3. Death date\nPlease enter the category to be changed: ");
      if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){
        printf(READ_ERROR);
        return errno;
      }
      sleep(SLEEP_TIME);
      fflush(stdout);
      nr = atoi(buffer);
    }
    if(nr == 1){
      strcpy(buffer,"<birth_place>\n");
    }
    else if(nr == 2){
      strcpy(buffer,"<birth_date>\n");
    }
    else{
      strcpy(buffer,"<death_date>\n");
    }
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){//sending category to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);

    printf("Please enter the new information for this category: ");
    fflush(stdout);
    if(fgets(buffer,BUFFER_SIZE,stdin) == NULL){ //receiving the new info from input
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    if(send(sd,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending the new info to server
      printf(SEND_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    if(read(sd,&answer,sizeof(answer)) < 0){//receiving answer from server
      printf(READ_ERROR);
      return errno;
    }
    sleep(SLEEP_TIME);
    answer_interpreter(command,answer);
  }
  /*else if(command == DELETE_BOOK){

  }
  else if(command == DELETE_AUTHOR){
    
  }
  else if(command == DELETE_PUBLISHER){
    
  }
  else if(command == DELETE_GENRE){
    
  }
  else if(command == DELETE_TAG){
    
  }*/
}
