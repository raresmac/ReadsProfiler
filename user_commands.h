#pragma once

#include "XML_commands.h"

int download_book(void *arg,char *filename){
    char buffer[NORMAL_LEN];
    char file_download[FILENAME_LEN];
	struct thData tdL;
	tdL = *((struct thData*)arg);
    if(read(tdL.cl,&buffer,sizeof(buffer)) <= 0){
        printf("[Thread %d] ",tdL.idThread);
        printf(READ_ERROR);
        return -1;
    }
    if(buffer[0] == 'Y' || buffer[0] == 'y'){
        struct passwd *pw = getpwuid(getuid());
        char *homedir = pw->pw_dir;
        strcat(homedir,"/Downloads/");
        strcpy(file_download,filename);
        strcpy(file_download,file_download + 8);
        file_download[strlen(file_download) - 3] = 't';
        file_download[strlen(file_download) - 2] = 'x';
        file_download[strlen(file_download) - 1] = 't';
        filename[strlen(filename) - 3] = 't';
        filename[strlen(filename) - 2] = 'x';
        filename[strlen(filename) - 1] = 't';
        strcat(homedir,file_download);

        FILE *book,*download;
        book = fopen(filename,"r");
        download = fopen(homedir,"w");

        while(fgets(buffer,NORMAL_LEN,book) != NULL){
            fprintf(download,"%s",buffer);
        }
        fclose(book);
        fclose(download);
        return 1;
    }
    return 2;
}

int see_profile(void *arg,char *username){
    int nr;
    char books[MAX_BOOKS][NORMAL_LEN];
    char username_aux[NORMAL_LEN];
    strcpy(username_aux,username);
    int status;
	struct thData tdL;
	tdL = *((struct thData*)arg);

    char filename[FILENAME_LEN];
    if(username_aux[strlen(username_aux)-1] == '\n'){
        username_aux[strlen(username_aux)-1] = '\0';
    }
    strcpy(filename,"./users/");strcat(filename,username_aux);strcat(filename,".xml");
    fflush(stdout);
    sleep(SLEEP_TIME);

    nr = xml_category(filename,"<reading>",books);
    if(send(tdL.cl,&nr,sizeof(nr),MSG_NOSIGNAL) <= 0){
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    
    for(int i = 0; i < nr; i++){
        if(send(tdL.cl,&books[i],sizeof(books[i]),MSG_NOSIGNAL) <= 0){ //sending each "reading" book
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            return -1;
        }
        sleep(SLEEP_TIME);
    }

    nr = xml_category(filename,"<read>",books);
    if(send(tdL.cl,&nr,sizeof(nr),MSG_NOSIGNAL) <= 0){
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    sleep(SLEEP_TIME);
    for(int i = 0; i < nr; i++){
        if(send(tdL.cl,&books[i],sizeof(books[i]),MSG_NOSIGNAL) <= 0){
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            return -1;
        }
        sleep(SLEEP_TIME);
    }

    nr = xml_category(filename,"<dropped>",books);
    sleep(SLEEP_TIME);
    if(send(tdL.cl,&nr,sizeof(nr),MSG_NOSIGNAL) <= 0){
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    sleep(SLEEP_TIME);
    for(int i = 0; i < nr; i++){
        if(send(tdL.cl,&books[i],NORMAL_LEN,MSG_NOSIGNAL) <= 0){
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            return -1;
        }
        sleep(SLEEP_TIME);
    }

    nr = xml_category(filename,"<planning>",books);
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(tdL.cl,&nr,sizeof(nr),MSG_NOSIGNAL) <= 0){
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    sleep(SLEEP_TIME);
    for(int i = 0; i < nr; i++){
        if(send(tdL.cl,&books[i],NORMAL_LEN,MSG_NOSIGNAL) <= 0){
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            return -1;
        }
        sleep(SLEEP_TIME);
    }
    return 1;
}

int book_profile(void *arg,char *filename){
	struct thData tdL;
	tdL = *((struct thData*)arg);
    FILE *search_file = fopen(filename,"r");
    char buffer[NORMAL_LEN]="";

    while(strcmp(buffer,"<isbn>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    if(buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1]='\0';
    }
    if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending isbn to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }
    sleep(SLEEP_TIME);

    while(strcmp(buffer,"<author>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    if(buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1]='\0';
    }
    if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending author to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }
    sleep(SLEEP_TIME);

    while(strcmp(buffer,"<publisher>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    if(buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1]='\0';
    }
    if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending publisher to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }
    sleep(SLEEP_TIME);

    while(strcmp(buffer,"<average>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    if(buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1] = '\0';
    }
    if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending average to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }
    sleep(SLEEP_TIME);

    while(strcmp(buffer,"<genres>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    int nr_genres = atoi(buffer);
    if(send(tdL.cl,&nr_genres,sizeof(nr_genres),MSG_NOSIGNAL) <= 0){ //sending number of genres to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }
    sleep(SLEEP_TIME);
    for(int i = 0; i < nr_genres; i++){
        fgets(buffer,NORMAL_LEN,search_file);
        if(buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1]='\0';
        }
        if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending each genre to client
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            fclose(search_file);
            return -1;
        }
        sleep(SLEEP_TIME);
    }

    /*while(strcmp(buffer,"<tags>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    int nr_tags = atoi(buffer);
    if(send(tdL.cl,&nr_tags,sizeof(nr_tags),MSG_NOSIGNAL) <= 0){ //sending number of tags to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }
    sleep(SLEEP_TIME);
    for(int i = 0; i < nr_tags; i++){
        fgets(buffer,NORMAL_LEN,search_file);
        if(buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1]='\0';
        }
        if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending each tag to client
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            fclose(search_file);
            return -1;
        }
        sleep(SLEEP_TIME);
    }*/

    while(strcmp(buffer,"<users>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    int nr_users = atoi(buffer);
    if(send(tdL.cl,&nr_users,sizeof(nr_users),MSG_NOSIGNAL) <= 0){ //sending number of users to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }
    sleep(SLEEP_TIME);
    for(int i = 0; i < nr_users; i++){
        fgets(buffer,NORMAL_LEN,search_file);
        if(buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1]='\0';
        }
        if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending each user to client
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            fclose(search_file);
            return -1;
        }
        sleep(SLEEP_TIME);
        fgets(buffer,NORMAL_LEN,search_file);
        if(buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1]='\0';
        }
        if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending each status/review to client
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            fclose(search_file);
            return -1;
        }
        sleep(SLEEP_TIME);
    }
    fclose(search_file);
    return download_book(arg,filename);
}

int author_profile(void *arg,char *filename){
	struct thData tdL;
	tdL = *((struct thData*)arg);
    FILE *search_file = fopen(filename,"r");
    char buffer[NORMAL_LEN]="";

    while(strcmp(buffer,"<birth_place>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    if(buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1]='\0';
    }
    if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending birth place to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }

    while(strcmp(buffer,"<birth_date>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    if(buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1]='\0';
    }
    if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending birth date to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }

    while(strcmp(buffer,"<death_date>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    if(buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1]='\0';
    }
    if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending death date to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }

    while(strcmp(buffer,"<books>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    int nr_books = atoi(buffer);
    if(send(tdL.cl,&nr_books,sizeof(nr_books),MSG_NOSIGNAL) <= 0){ //sending number of books to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }
    for(int i = 0; i < nr_books; i++){
        fgets(buffer,NORMAL_LEN,search_file);
        if(buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1]='\0';
        }
        if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending each book to client
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            fclose(search_file);
            return -1;
        }
    }
    fclose(search_file);
    return 1;
}

int publisher_profile(void *arg,char *filename){
	struct thData tdL;
	tdL = *((struct thData*)arg);
    FILE *search_file = fopen(filename,"r");
    char buffer[NORMAL_LEN]="";

    while(strcmp(buffer,"<books>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    int nr_books = atoi(buffer);
    if(send(tdL.cl,&nr_books,sizeof(nr_books),MSG_NOSIGNAL) <= 0){ //sending number of books to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }
    for(int i = 0; i < nr_books; i++){
        fgets(buffer,NORMAL_LEN,search_file);
        if(buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1]='\0';
        }
        if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending each book to client
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            fclose(search_file);
            return -1;
        }
    }
    fclose(search_file);
    return 1;
}

int genre_profile(void *arg,char *filename){
	struct thData tdL;
	tdL = *((struct thData*)arg);
    FILE *search_file = fopen(filename,"r");
    char buffer[NORMAL_LEN]="";

    while(strcmp(buffer,"<books>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    int nr_books = atoi(buffer);
    if(send(tdL.cl,&nr_books,sizeof(nr_books),MSG_NOSIGNAL) <= 0){ //sending number of books to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }
    for(int i = 0; i < nr_books; i++){
        fgets(buffer,NORMAL_LEN,search_file);
        if(buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1]='\0';
        }
        if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending each book to client
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            fclose(search_file);
            return -1;
        }
    }
    fclose(search_file);
    return 1;
}

/*int tag_profile(void *arg,char *filename){
	struct thData tdL;
	tdL = *((struct thData*)arg);
    FILE *search_file = fopen(filename,"r");
    char buffer[NORMAL_LEN]="";

    while(strcmp(buffer,"<books>\n")){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fgets(buffer,NORMAL_LEN,search_file);
    int nr_books = atoi(buffer);
    if(send(tdL.cl,&nr_books,sizeof(nr_books),MSG_NOSIGNAL) <= 0){ //sending number of books to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        fclose(search_file);
        return -1;
    }
    for(int i = 0; i < nr_books; i++){
        fgets(buffer,NORMAL_LEN,search_file);
        if(buffer[strlen(buffer)-1] == '\n'){
            buffer[strlen(buffer)-1]='\0';
        }
        if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending each book to client
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            fclose(search_file);
            return -1;
        }
    }
    fclose(search_file);
    return 1;
}*/

int add_book(void *arg,char *username){
    FILE *book_file;
    char name[NORMAL_LEN];
    char username_aux[NORMAL_LEN];
    strcpy(username_aux,username);
    char finder[NORMAL_LEN];
    char buffer[NORMAL_LEN],buffer_aux[NORMAL_LEN];
    int status,review,correct = 1;
	struct thData tdL;
	tdL = *((struct thData*)arg);

    char filename[FILENAME_LEN];
    if(username_aux[strlen(username_aux)-1] == '\n'){
        username_aux[strlen(username_aux)-1] = '\0';
    }
    strcpy(filename,"./users/");strcat(filename,username_aux);strcat(filename,".xml");

    if(read(tdL.cl,&name,sizeof(name)) <= 0){ //reading book name
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    name[strlen(name)-1]='\0';

    if(read(tdL.cl,&status,sizeof(status)) <= 0){ //reading book status
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);

    //checking if book is in profile
    int line_found = xml_retriever(filename,name,buffer_aux);
    if(line_found > 11){
        correct = 0;
    }

    if(status == 1){
        strcpy(finder,"<reading>\n");
        strcat(username_aux,"\n");
        strcat(username_aux,"reading");
    }
    else if(status == 2){
        strcpy(finder,"<read>\n");
        if(read(tdL.cl,&buffer,sizeof(buffer)) <= 0){ //reading book review
            printf("[Thread %d]\n",tdL.idThread);
            printf(READ_ERROR);
            return -1;
        }
        review = atoi(buffer);
        sleep(SLEEP_TIME);
        strcat(username_aux,"\n");
        strcat(username_aux,buffer);
        username_aux[strlen(username_aux)-1]='\0';
    }
    else if(status == 3){
        strcpy(finder,"<dropped>\n");
        strcat(username_aux,"\n");
        strcat(username_aux,"dropped");
    }
    else{
        strcpy(finder,"<planning>\n");
        strcat(username_aux,"\n");
        strcat(username_aux,"planning");
    }

    if(correct == 0) return 0;
    //checking if book is in database
    char filename_book[FILENAME_LEN];
    strcpy(filename_book,"./books/");strcat(filename_book,name);strcat(filename_book,".xml");
    if((book_file = fopen(filename_book,"r")) == NULL){
        return 0;
    }
    fclose(book_file);

    if(status == 2){
        strcat(name,"\n");
        strcat(name,buffer);
        name[strlen(name)-1]='\0';
    }

    if(xml_incrementer(filename,finder,name) <= 0){
        return -1;
    }
    if(xml_incrementer(filename_book,"<users>\n",username_aux) <= 0){
        return -1;
    }
    if(status == 2){
        xml_retriever(filename_book,"<average>",buffer);
        float avg = atof(buffer);
        xml_retriever(filename_book,"<users>",buffer);
        int nr_users = atoi(buffer);
        avg *= 1.*(nr_users - 1);
        avg += review;
        avg = avg*1./nr_users;
        sprintf(buffer,"%f",avg);
        xml_modifier(filename_book,"<average>\n",buffer);
        
        char genres[MAX_BOOKS][NORMAL_LEN];
        int nr1,nr2,ok,new_number = 0;
        FILE *file = fopen(filename,"r");
        FILE *tmp = fopen("xml.tmp","w");
        while(fgets(buffer,NORMAL_LEN,file) != NULL){
            fprintf(tmp,"%s",buffer);
            if(strstr(buffer,"<genres>") != NULL){
                book_file = fopen(filename_book,"r");
                int exit_while = 0;
                while(fgets(buffer,NORMAL_LEN,book_file) != NULL && exit_while == 0){
                    if(strstr(buffer,"<genres>") != NULL){
                        fgets(buffer,NORMAL_LEN,book_file);
                        nr2 = atoi(buffer);
                        for(int i = 0; i < nr2; i++){
                            fgets(genres[i],NORMAL_LEN,book_file);
                        }
                        exit_while = 1;
                    }
                }
                fclose(book_file);

                fgets(buffer,NORMAL_LEN,file); //<number>
                fprintf(tmp,"%s",buffer);
                fgets(buffer,NORMAL_LEN,file); //number of genres
                fprintf(tmp,"%s",buffer);
                nr1 = atoi(buffer);
                new_number = nr1;
                fgets(buffer,NORMAL_LEN,file); //</number>
                fprintf(tmp,"%s",buffer);
                for(int i = 0; i < nr1; i++){
                    fgets(buffer,NORMAL_LEN,file); //<name>
                    fprintf(tmp,"%s",buffer);
                    fgets(buffer,NORMAL_LEN,file); //name of genre
                    fprintf(tmp,"%s",buffer);
                    char genre_name[NORMAL_LEN];
                    strcpy(genre_name,buffer);
                    fgets(buffer,NORMAL_LEN,file); //</name>
                    fprintf(tmp,"%s",buffer);
                    fgets(buffer,NORMAL_LEN,file); //<average>
                    fprintf(tmp,"%s",buffer);
                    char avg[10],nr[10];
                    fgets(avg,10,file); //average rating for this genre
                    fgets(nr,10,file); //nr of reviews for this genre
                    float new_avg = atof(avg);
                    int new_nr = atoi(nr);

                    ok = 0;
                    for(int j = 0; j < nr2 && ok == 0; j++){
                        if(strcmp(genre_name,genres[j]) == 0){
                            ok = 1;
                            genres[j][0] = '!';
                            break;
                        }
                    }
                    if(ok == 0){
                        fprintf(tmp,"%s",avg);
                        fprintf(tmp,"%s",nr);
                        fgets(buffer,NORMAL_LEN,file); //</average>
                        fprintf(tmp,"%s",buffer);
                    }
                    else{
                        new_avg *= 1.*new_nr;
                        new_avg += review;
                        new_nr++;
                        new_avg /= 1.*new_nr;
                        fprintf(tmp,"%f\n",new_avg);
                        fprintf(tmp,"%d\n",new_nr);
                        fgets(buffer,NORMAL_LEN,file); //</average>
                        fprintf(tmp,"%s",buffer);
                    }
                }
                for(int i = 0; i < nr2; i++){
                    if(genres[i][0] != '!'){
                        new_number++;
                        fprintf(tmp,"<genre_name>\n%s</genre_name>\n<average>\n%d\n%d\n</average>\n",genres[i],review,1);
                    }
                }
            }
        }
        fclose(file);
        fclose(tmp);

        file = fopen(filename,"w");
        tmp = fopen("xml.tmp","r");

        while(fgets(buffer,NORMAL_LEN,tmp) != NULL){
            if(strstr(buffer,"<number>") != NULL){
                fprintf(file,"%s",buffer);
                fgets(buffer,NORMAL_LEN,tmp);
                fprintf(file,"%d\n",new_number);
                fgets(buffer,NORMAL_LEN,tmp);
            }
            fprintf(file,"%s",buffer);
        }
        fclose(file);
        fclose(tmp);
    }
    return 1;
}

int remove_book(void *arg,char *username){
    FILE *book_file;
    char name[NORMAL_LEN];
    char keeper[NORMAL_LEN];
    char username_aux[NORMAL_LEN];
    char keeper_aux[NORMAL_LEN];
    strcpy(username_aux,username);
    char buffer[NORMAL_LEN];
    int status,correct = 1;
	struct thData tdL;
	tdL = *((struct thData*)arg);

    char filename[FILENAME_LEN];
    strcpy(filename,"./users/");strcat(filename,username_aux);strcat(filename,".xml");

    if(read(tdL.cl,&name,sizeof(name)) <= 0){ //reading book name
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    name[strlen(name)-1]='\0';
    char filename_book[FILENAME_LEN];
    strcpy(filename_book,"./books/");strcat(filename_book,name);strcat(filename_book,".xml");

    //checking if book is in profile
    int line_found = xml_retriever(filename,name,keeper_aux);
    if(line_found <= 0){
        return 0;
    }
    xml_retriever(filename_book,username_aux,buffer);
    if(strcmp(buffer,"reading") && strcmp(buffer,"dropped") && strcmp(buffer,"planning")){
        int review = atoi(buffer);
        xml_retriever(filename_book,"<average>",buffer);
        float avg = atof(buffer);
        xml_retriever(filename_book,"<users>",buffer);
        int nr_users = atoi(buffer);
        avg *= 1.*nr_users;
        avg -= review;
        if(nr_users > 1) avg /= 1.*(nr_users - 1);
        else avg = 0;
        sprintf(buffer,"%f",avg);
        xml_modifier(filename_book,"<average>\n",buffer);
        strcpy(buffer,"read");
    }
    sprintf(keeper,"<%s>\n",buffer);
    strcat(username_aux,"\n");
    if(xml_decrementer(filename_book,"<users>\n",username_aux) <= 0){
        return -1;
    }
    strcat(name,"\n");
    if(xml_decrementer(filename,keeper,name) <= 0){
        return 0;
    }
    return 1;
}

int search(void *arg,int type){
    int nr,nr_result;
	struct thData tdL;
	tdL = *((struct thData*)arg);
    char filename[FILENAME_LEN];
    char filename_search[FILENAME_LEN];
    char buffer[NORMAL_LEN];

    strcpy(filename,"./search/");
    if(type == 1) strcat(filename,"books.txt");
    else if(type == 2) strcat(filename,"authors.txt");
    else if(type == 3) strcat(filename,"publishers.txt");
    else if(type == 4) strcat(filename,"genres.txt");
    //else if(type == 5) strcat(filename,"tags.txt");

    nr = nr_search(filename);
    if(send(tdL.cl,&nr,sizeof(nr),MSG_NOSIGNAL) <= 0){
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    sleep(SLEEP_TIME);
    FILE *search_file = fopen(filename,"r");
    for(int i = 0; i < nr; i++){
        fgets(buffer,NORMAL_LEN,search_file);
        if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            fclose(search_file);
            return -1;
        }
        sleep(SLEEP_TIME);
    }
    fclose(search_file);

    if(read(tdL.cl,&nr_result,sizeof(nr_result)) <= 0){
        printf("[Thread %d] ",tdL.idThread);
        printf(READ_ERROR);
        return -1;
    }
    sleep(SLEEP_TIME);
    if(nr_result > nr) return 1;

    search_file = fopen(filename,"r");
    for(int i = 0; i < nr_result; i++){
        fgets(buffer,NORMAL_LEN,search_file);
    }
    fclose(search_file);
    if(buffer[strlen(buffer)-1] == '\n'){
        buffer[strlen(buffer)-1]='\0';
    }

    if(send(tdL.cl,&buffer,sizeof(buffer),MSG_NOSIGNAL) <= 0){ //sending choice to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    
    if(type == 1){
        strcpy(filename_search,"./books/");strcat(filename_search,buffer);strcat(filename_search,".xml");
        book_profile(arg,filename_search);
    }
    else if(type == 2){
        strcpy(filename_search,"./authors/");strcat(filename_search,buffer);strcat(filename_search,".xml");
        author_profile(arg,filename_search);
    }
    else if(type == 3){
        strcpy(filename_search,"./publishers/");strcat(filename_search,buffer);strcat(filename_search,".xml");
        publisher_profile(arg,filename_search);
    }
    else if(type == 4){
        strcpy(filename_search,"./genres/");strcat(filename_search,buffer);strcat(filename_search,".xml");
        genre_profile(arg,filename_search);
    }
    /*else if(type == 5){
        strcpy(filename_search,"./tags/");strcat(filename_search,buffer);strcat(filename_search,".xml");
        tag_profile(arg,filename_search);
    }*/
    return 1;
}

int search_isbn(void *arg){
    FILE *search_file,*file;
    int nr,nr_result;
	struct thData tdL;
	tdL = *((struct thData*)arg);
    char filename[FILENAME_LEN],filename_search[FILENAME_LEN];
    char buffer[NORMAL_LEN],isbn[NORMAL_LEN],book[NORMAL_LEN];

    if(read(tdL.cl,&isbn,sizeof(isbn)) <= 0){ //reading isbn from client
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    fflush(stdout);

    strcpy(filename_search,"./search/books.txt");
    search_file = fopen(filename_search,"r");
    int ok = 0;
    while(ok == 0 && fgets(book,NORMAL_LEN,search_file) != NULL){
        sleep(SLEEP_TIME);
        fflush(stdout);
        if(book[strlen(book)-1] == '\n'){
            book[strlen(book)-1]='\0';
        }
        strcpy(filename,"./books/");strcat(filename,book);strcat(filename,".xml");
        file = fopen(filename,"r");
        while(fgets(buffer,NORMAL_LEN,file) && ok == 0){
            sleep(SLEEP_TIME);
            fflush(stdout);
            if(strcmp(buffer,"<isbn>\n") == 0){
                fgets(buffer,NORMAL_LEN,file);
                sleep(SLEEP_TIME);
                fflush(stdout);
                if(strcmp(buffer,isbn) == 0){
                    ok = 1;
                    break;
                }
                else{
                    break;
                }
            }
        }
        fclose(file);
    }
    fclose(search_file);
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(send(tdL.cl,&ok,sizeof(ok),MSG_NOSIGNAL) <= 0){ //sending answer to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    sleep(SLEEP_TIME);
    fflush(stdout);
    if(ok == 1){
        if(send(tdL.cl,&book,sizeof(book),MSG_NOSIGNAL) <= 0){ //sending answer to client
            printf("\n[Thread %d] ",tdL.idThread);
            printf(SEND_ERROR);
            return -1;
        }
        sleep(SLEEP_TIME);
        book_profile(arg,filename);
    }
    return 1;
}

int recommendation(void *arg,char *username){
    FILE *search_file,*file,*user_file;
    int nr,nr_result,ok;
	struct thData tdL;
	tdL = *((struct thData*)arg);
    char username_aux[NORMAL_LEN],filename[FILENAME_LEN];
    char username_search[FILENAME_LEN],filename_search[FILENAME_LEN];
    char buffer[NORMAL_LEN],buffer_aux[NORMAL_LEN];
    char book[NORMAL_LEN],genre[NORMAL_LEN];
    char books[5][NORMAL_LEN];
    books[0][0] = '-';books[1][0] = '-';books[2][0] = '-';books[3][0] = '-';books[4][0] = '-';
    char genres[3][NORMAL_LEN];
    float genre_averages[3]={-1,-1,-1};
    float averages[5]={-1,-1,-1,-1,-1};
    strcpy(username_aux,username);

    strcpy(username_search,"./users/");strcat(username_search,username_aux);strcat(username_search,".xml");
    user_file = fopen(username_search,"r");
    while(fgets(buffer,NORMAL_LEN,user_file) != NULL){
        sleep(SLEEP_TIME);
        if(strstr(buffer,"<genre_name>") != NULL){
            fgets(genre,NORMAL_LEN,user_file);
            while(fgets(buffer,NORMAL_LEN,user_file) != NULL){
                if(strstr(buffer,"<average>") != NULL){
                    fgets(buffer,NORMAL_LEN,user_file);
                    float new_avg = atof(buffer);
                    if(new_avg > genre_averages[0]){
                        strcpy(genres[2],genres[1]);
                        genre_averages[2] = genre_averages[1];
                        strcpy(genres[1],genres[0]);
                        genre_averages[1] = genre_averages[0];
                        strcpy(genres[0],genre);
                        genre_averages[0] = new_avg;
                    }
                    else if(new_avg > genre_averages[1]){
                        strcpy(genres[2],genres[1]);
                        genre_averages[2] = genre_averages[1];
                        strcpy(genres[1],genre);
                        genre_averages[1] = new_avg;
                    }
                    else if(new_avg > genre_averages[2]){
                        strcpy(genres[2],genre);
                        genre_averages[2] = new_avg;
                    }
                    break;
                }
            }
        }
    }
    fclose(user_file);
    sleep(SLEEP_TIME);
    if(genre_averages[2] == -1){
        ok = 0;
    }
    else ok = 1;
    if(send(tdL.cl,&ok,sizeof(ok),MSG_NOSIGNAL) <= 0){ //sending answer to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    if(ok == 0) return 1;
    strcpy(filename_search,"./search/books.txt");
    search_file = fopen(filename_search,"r");
    while(fgets(book,NORMAL_LEN,search_file) != NULL){
        //checking if book is in profile
        if(book[strlen(book)-1] == '\n'){
            book[strlen(book)-1]='\0';
        }
        int line_found = xml_retriever(username_search,book,buffer_aux);
        if(line_found <= 11){
            sleep(SLEEP_TIME);
            fflush(stdout);
            strcpy(filename,"./books/");strcat(filename,book);strcat(filename,".xml");
            file = fopen(filename,"r");
            while(fgets(buffer,NORMAL_LEN,file) != NULL){
                sleep(SLEEP_TIME);
                fflush(stdout);
                if(strstr(buffer,"<average>") != NULL){
                    fgets(buffer,NORMAL_LEN,file);
                    float avg_aux = atof(buffer);
                    sleep(SLEEP_TIME);
                    fflush(stdout);
                }
                if(strstr(buffer,genres[0]) != NULL){
                    fgets(buffer,NORMAL_LEN,file);
                    fgets(buffer,NORMAL_LEN,file);
                    fgets(buffer,NORMAL_LEN,file);
                    float next_avg = atof(buffer);
                    for(int i = 0; i < 3; i++){
                        if(next_avg > averages[i]){
                            for(int j = 2; j > i; j--){
                                strcpy(books[j],books[j-1]);
                                averages[j] = averages[j-1];
                            }
                            averages[i] = next_avg;
                            strcpy(books[i],book);
                            break;
                        }
                    }                
                }
            }
            fclose(file);
        }
    }
    fclose(search_file);
    sleep(SLEEP_TIME);

    search_file = fopen(filename_search,"r");
    while(fgets(book,NORMAL_LEN,search_file) != NULL){
        //checking if book is in profile
        if(book[strlen(book)-1] == '\n'){
            book[strlen(book)-1]='\0';
        }
        int line_found = xml_retriever(username_search,book,buffer_aux);
        if(line_found <= 11){
            sleep(SLEEP_TIME);
            fflush(stdout);
            strcpy(filename,"./books/");strcat(filename,book);strcat(filename,".xml");
            file = fopen(filename,"r");
            while(fgets(buffer,NORMAL_LEN,file) != NULL){
                sleep(SLEEP_TIME);
                fflush(stdout);
                if(strstr(buffer,"<average>") != NULL){
                    fgets(buffer,NORMAL_LEN,file);
                    float avg_aux = atof(buffer);
                    sleep(SLEEP_TIME);
                    fflush(stdout);
                }
                if(strstr(buffer,genres[1]) != NULL){
                    fgets(buffer,NORMAL_LEN,file);
                    fgets(buffer,NORMAL_LEN,file);
                    fgets(buffer,NORMAL_LEN,file);
                    float next_avg = atof(buffer);
                    if(next_avg > averages[3]){
                        averages[3] = next_avg;
                        strcpy(books[3],book);
                    }                
                }
            }
            fclose(file);
        }
    }
    fclose(search_file);
    sleep(SLEEP_TIME);

    search_file = fopen(filename_search,"r");
    while(fgets(book,NORMAL_LEN,search_file) != NULL){
        //checking if book is in profile
        if(book[strlen(book)-1] == '\n'){
            book[strlen(book)-1]='\0';
        }
        int line_found = xml_retriever(username_search,book,buffer_aux);
        if(line_found <= 11){
            sleep(SLEEP_TIME);
            fflush(stdout);
            strcpy(filename,"./books/");strcat(filename,book);strcat(filename,".xml");
            file = fopen(filename,"r");
            while(fgets(buffer,NORMAL_LEN,file) != NULL){
                sleep(SLEEP_TIME);
                fflush(stdout);
                if(strstr(buffer,"<average>") != NULL){
                    fgets(buffer,NORMAL_LEN,file);
                    float avg_aux = atof(buffer);
                    sleep(SLEEP_TIME);
                    fflush(stdout);
                }
                if(strstr(buffer,genres[2]) != NULL){
                    fgets(buffer,NORMAL_LEN,file);
                    fgets(buffer,NORMAL_LEN,file);
                    fgets(buffer,NORMAL_LEN,file);
                    float next_avg = atof(buffer);
                    if(next_avg > averages[4]){
                        averages[4] = next_avg;
                        strcpy(books[4],book);
                    }                
                }
            }
            fclose(file);
        }
    }
    fclose(search_file);
    sleep(SLEEP_TIME);

    if(send(tdL.cl,&books[0],sizeof(books[0]),MSG_NOSIGNAL) <= 0){ //sending book 0 to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    if(send(tdL.cl,&books[1],sizeof(books[1]),MSG_NOSIGNAL) <= 0){ //sending book 1 to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    if(send(tdL.cl,&books[2],sizeof(books[2]),MSG_NOSIGNAL) <= 0){ //sending book 2 to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    if(send(tdL.cl,&books[3],sizeof(books[3]),MSG_NOSIGNAL) <= 0){ //sending book 3 to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    if(send(tdL.cl,&books[4],sizeof(books[4]),MSG_NOSIGNAL) <= 0){ //sending book 4 to client
        printf("\n[Thread %d] ",tdL.idThread);
        printf(SEND_ERROR);
        return -1;
    }
    return 1;
}
