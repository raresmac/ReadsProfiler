#pragma once

#include "XML_commands.h"

int password_change(void *arg,char *username){
	struct thData tdL;
	tdL = *((struct thData*)arg);
    char password[NORMAL_LEN];
    char filename[FILENAME_LEN];
    if(read(tdL.cl,&password,sizeof(password)) <= 0){ //reading new password
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    fflush(stdout);

    if(password[strlen(password)-1] == '\n'){
        password[strlen(password)-1] = '\0';
    }
    strcpy(filename,"./users/");strcat(filename,username);strcat(filename,".xml");
    return xml_modifier(filename,"<password>\n",password);
}

int check_username(void *arg,char *username,char *password,int nr_com){
    FILE* file;
    char filename[FILENAME_LEN];
    char password_xml[NORMAL_LEN];
    struct thData tdL; 
	tdL= *((struct thData*)arg);
    strcpy(filename,"./users/");strcat(filename,username);strcat(filename,".xml");

    if((file = fopen(filename,"r")) != NULL){
        if(nr_com == 2) return 0;
        fclose(file);
        char user_status[3];
        if(xml_retriever(filename,"<status>",user_status) <= 0){ //getting the user status from xml
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEARCH_ERROR);
            return -1;
        }
        if(xml_retriever(filename,"<password>",password_xml) <= 0){ //getting the password from xml
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEARCH_ERROR);
            return -1;
        }
        if(strcmp(password,password_xml) == 0){
            int ans = user_status[0]-'0';
            return ans;
        }
        else{
            return 0;
        }
    }
    else if(nr_com == 1){
        return 0;
    }
    else{
        if(xml_user_creator(filename,password) != -1){
            update_server();
        }
        return 1;
    }
}

int login_register(void *arg,int nr_com,char username[NORMAL_LEN]){
    int i = 0;
    char password[NORMAL_LEN];
    int user_status;
	struct thData tdL;
	tdL = *((struct thData*)arg);

    if(read(tdL.cl,username,NORMAL_LEN) <= 0){
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);

    if(read(tdL.cl,&password,sizeof(password)) <= 0){
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}

	if((user_status = check_username(arg,username,password,nr_com)) >= 0){
        if(send(tdL.cl, &user_status, sizeof(user_status),MSG_NOSIGNAL) <= 0){
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            return -1;
        }
        else{
            printf("\n[Thread %d]Message sent succesfully!\n",tdL.idThread);
        }
    }
}
