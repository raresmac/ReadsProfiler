#include "login_register.h"
#include "admin_commands.h"
#include "user_commands.h"

extern int errno; //error code

int central_hub(void*,thData,int);
static void *treat(void*);
void raspunde(void *);

pthread_t th[NR_THREADS]; //thread identifiers
char usernames[NR_THREADS][NORMAL_LEN]; //usernames

int main()
{
  struct sockaddr_in server;	//server structure
  struct sockaddr_in from;
  int sd; //socket descriptor
  int pid;
  int i = 0;
  if((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1){ //creating socket
    printf(SOCKET_ERROR);
    return errno;
  }
  int on=1;
  setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
  bzero(&server,sizeof(server));
  bzero(&from,sizeof(from));
  initialize_server();
  server.sin_family = AF_INET; //server family
  server.sin_addr.s_addr = htonl(INADDR_ANY); //accepting any address
  server.sin_port = htons(PORT); //using the port specified by PORT
  if(bind(sd,(struct sockaddr *) &server,sizeof(struct sockaddr)) == -1){ //binding socket
    printf(BIND_ERROR);
    perror("\n");
    return errno;
  }
  if(listen(sd,2) == -1){
    printf(LISTEN_ERROR);
    return errno;
  }

  while(1){
    int client;
    thData* td;
    int length = sizeof(from);
    printf("[server]Waiting at port %d...\n",PORT);
    fflush(stdout);
    if((client = accept(sd,(struct sockaddr*)&from,&length)) < 0){ //accepting a client
      printf(ACCEPT_ERROR);
      continue;
    }

	  td=(struct thData*)malloc(sizeof(struct thData));	
	  td->idThread=i++;
	  td->cl=client;
    if(pthread_create(&th[i], NULL, &treat, td) != 0){
      printf(THREAD_ERROR);
    }	
    printf("\nFinished job\n");
  }
};

int central_hub(void *arg,thData tdL,int nr){
  int answer;
  if(nr == LOGIN || nr == REGISTER){
    return login_register(arg,nr,usernames[tdL.cl]);
  }
  else if(nr == SEE_PROFILE){
    answer = see_profile(arg,usernames[tdL.cl]);
    if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
      printf("\n[Thread %d] ",tdL.idThread);
      printf(SEND_ERROR);
      return -1;
    }
    return 1;
  }
  else if(nr == ADD_BOOK){
    answer = add_book(arg,usernames[tdL.cl]);
    if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
      printf("\n[Thread %d] ",tdL.idThread);
      printf(SEND_ERROR);
      return -1;
    }
    return 1;
  }
  else if(nr == REMOVE_BOOK){
    answer = remove_book(arg,usernames[tdL.cl]);
    if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
      printf("\n[Thread %d] ",tdL.idThread);
      printf(SEND_ERROR);
      return -1;
    }
    return 1;
  }
  else if(nr == SEARCH_BOOK){
    answer = search(arg,1);
    return 1;
  }
  else if(nr == SEARCH_AUTHOR){
    answer = search(arg,2);
    return 1;
  }
  else if(nr == SEARCH_PUBLISHER){
    answer = search(arg,3);
    return 1;
  }
  else if(nr == SEARCH_GENRE){
    answer = search(arg,4);
    return 1;
  }
  /*else if(nr == SEARCH_TAG){
    answer = search(arg,5);
    return 1;
  }*/
  else if(nr == SEARCH_ISBN){
    answer = search_isbn(arg);
    return 1;
  }
  else if(nr == GET_RECOMMENDATIONS){
    answer = recommendation(arg,usernames[tdL.cl]);
    return 1;
  }
  else if(nr == CHANGE_PASSWORD){
    answer = password_change(arg,usernames[tdL.cl]);
    if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
      printf("\n[Thread %d] ",tdL.idThread);
      printf(SEND_ERROR);
      return -1;
    }
    return 1;
  }
  else if(nr == ADD_BOOK_ADMIN){
    answer = add_book_admin(arg);
    if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
      printf("\n[Thread %d] ",tdL.idThread);
      printf(SEND_ERROR);
      return -1;
    }
    return 1;
  }
  else if(nr == ADD_AUTHOR_ADMIN){
    answer = add_author(arg);
    if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
      printf("\n[Thread %d] ",tdL.idThread);
      printf(SEND_ERROR);
      return -1;
    }
    return 1;
  }
  else if(nr == ADD_PUBLISHER_ADMIN){
    answer = add_publisher(arg);
    if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
      printf("\n[Thread %d] ",tdL.idThread);
      printf(SEND_ERROR);
      return -1;
    }
    return 1;
  }
  else if(nr == ADD_GENRE_ADMIN){
    answer = add_genre(arg);
    if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
      printf("\n[Thread %d] ",tdL.idThread);
      printf(SEND_ERROR);
      return -1;
    }
    return 1;
  }
  /*else if(nr == ADD_TAG_ADMIN){
    answer = add_tag(arg);
    if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
      printf("\n[Thread %d] ",tdL.idThread);
      printf(SEND_ERROR);
      return -1;
    }
    return 1;
  }*/
  else if(nr == EDIT_BOOK){
    answer = edit_book(arg);
    if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
      printf("\n[Thread %d] ",tdL.idThread);
      printf(SEND_ERROR);
      return -1;
    }
    return 1;
  }
  else if(nr == EDIT_AUTHOR){
    answer = edit_author(arg);
    if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
      printf("\n[Thread %d] ",tdL.idThread);
      printf(SEND_ERROR);
      return -1;
    }
    return 1;
  }
  answer = 0;
  if(send(tdL.cl,&answer,sizeof(answer),MSG_NOSIGNAL) <= 0){
    printf("\n[Thread %d] ",tdL.idThread);
    printf(SEND_ERROR);
    return -1;
  }
  return 0;
}

static void *treat(void* arg){
    int nr_com = -1;
    struct thData tdL; 
    tdL=*((struct thData*)arg);	
    while(nr_com != COMMAND_EXIT){
      printf("[Thread %d] - Waiting for message...\n", tdL.idThread);
      pthread_detach(pthread_self());
      if(read(tdL.cl,&nr_com,sizeof(int)) <= 0){
        printf("[Thread %d] ",tdL.idThread);
        printf(READ_ERROR);
        break;
      }
      if(nr_com == 1000 || nr_com == -1) nr_com = COMMAND_EXIT;
      int answer = central_hub((struct thData*)arg,tdL,nr_com);
      if(answer < 0){
        printf("[Thread %d] Fatal error!\n",tdL.idThread);
        break;
      }
      else if(answer == 0){
        printf("Unknown command!\n");
      }
    }
    close((long)arg); //closing connection
    printf("[Thread %d] - Closed connection...\n", tdL.idThread);
    pthread_exit(NULL);
		return(NULL);	
};
