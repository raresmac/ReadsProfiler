#pragma once

#include "server_commands.h"
#include "search.h"

int add_book_admin(void *arg){
    FILE *book_file,*author_file,*publisher_file;
	struct thData tdL;
    char name[NORMAL_LEN];
    char isbn[NORMAL_LEN];
    char author[NORMAL_LEN];
    char publisher[NORMAL_LEN];
    int nr_genres;
    char genres[MAX_GENRES][NORMAL_LEN];
	tdL = *((struct thData*)arg);

    if(read(tdL.cl,&name,sizeof(name)) <= 0){ //getting book name
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    name[strlen(name)-1]='\0';

    if(read(tdL.cl,&isbn,sizeof(isbn)) <= 0){ //getting isbn
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    isbn[strlen(isbn)-1]='\0';

    if(read(tdL.cl,&author,sizeof(author)) <= 0){ //getting author name
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    author[strlen(author)-1]='\0';

    if(read(tdL.cl,&publisher,sizeof(publisher)) <= 0){ //getting publisher
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    publisher[strlen(publisher)-1]='\0';

    if(read(tdL.cl,&nr_genres,sizeof(nr_genres)) <= 0){ //getting number of genres
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);

    for(int i = 0; i < nr_genres; i++){ //getting each genre
        if(read(tdL.cl,&genres[i],sizeof(genres[i])) <= 0){
            printf("[Thread %d]\n",tdL.idThread);
            printf(READ_ERROR);
            return -1;
        }
        sleep(SLEEP_TIME);
    }

    //checking if files exist
    char filename_book[FILENAME_LEN];
    strcpy(filename_book,"./books/");strcat(filename_book,name);strcat(filename_book,".xml");
    if((book_file = fopen(filename_book,"r")) != NULL){
        fclose(book_file);
        return 0;
    }
    char filename_author[FILENAME_LEN];
    strcpy(filename_author,"./authors/");strcat(filename_author,author);strcat(filename_author,".xml");
    if((author_file = fopen(filename_author,"r")) != NULL){
        fclose(author_file);
    }
    else{
        return 0;
    }

    char filename_publisher[FILENAME_LEN];
    strcpy(filename_publisher,"./publishers/");strcat(filename_publisher,publisher);strcat(filename_publisher,".xml");
    if((publisher_file = fopen(filename_publisher,"r")) != NULL){
        fclose(publisher_file);
    }
    else{
        return 0;
    }
    for(int i = 0; i < nr_genres; i++){
        FILE *genre_file;
        char filename_genre[FILENAME_LEN];
        strcpy(filename_genre,"./genres/");strcat(filename_genre,genres[i]);strcat(filename_genre,".xml");
        if((genre_file = fopen(filename_genre,"r")) == NULL){
            return 0;
        }
        else{
            fclose(genre_file);
        }
    }

    int nr = xml_book_creator(name,isbn,author,publisher,nr_genres,genres);
    if(nr > 0){
        if(xml_incrementer(filename_author,"<books>\n",name) <= 0){
            return -1;
        }
        if(xml_incrementer(filename_publisher,"<books>\n",name) <= 0){
            return -1;
        }
        for(int i = 0; i < nr_genres; i++){
            xml_incrementer(filename_author,"<genres>\n",genres[i]);
            char filename_genre[FILENAME_LEN];
            strcpy(filename_genre,"./genres/");strcat(filename_genre,genres[i]);strcat(filename_genre,".xml");
            if(xml_incrementer(filename_genre,"<books>\n",name) <= 0){
                return -1;
            }
        }
        strcat(name,"\n");
        if(add_search("./search/books.txt",name) <= 0){
            return -1;
        }
        update_server();
    }
    return nr;
}

int add_author(void *arg){
    FILE *author_file;
	struct thData tdL;
    char name[NORMAL_LEN];
    char birth_place[NORMAL_LEN];
    char birth_date[NORMAL_LEN];
    char death_date[NORMAL_LEN];
	tdL = *((struct thData*)arg);

    if(read(tdL.cl,&name,sizeof(name)) <= 0){
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    name[strlen(name)-1]='\0';

    if(read(tdL.cl,&birth_place,sizeof(birth_place)) <= 0){
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    birth_place[strlen(birth_place)-1]='\0';

    if(read(tdL.cl,&birth_date,sizeof(birth_date)) <= 0){
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    birth_date[strlen(birth_date)-1]='\0';

    if(read(tdL.cl,&death_date,sizeof(death_date)) <= 0){
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    char filename[FILENAME_LEN];
    strcpy(filename,"./authors/");strcat(filename,name);strcat(filename,".xml");
    if((author_file = fopen(filename,"r")) != NULL){
        fclose(author_file);
        return 0;
    }
    int nr = xml_author_creator(name,birth_place,birth_date,death_date);
    if(nr > 0){
        strcat(name,"\n");
        add_search("./search/authors.txt",name);
        update_server();
    }
    return nr;
}

int add_publisher(void *arg){
    FILE* publisher_file;
	struct thData tdL;
    char name[NORMAL_LEN];
	tdL = *((struct thData*)arg);

    if(read(tdL.cl,&name,sizeof(name)) <= 0){
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    name[strlen(name)-1]='\0';

    char filename[FILENAME_LEN];
    strcpy(filename,"./publishers/");strcat(filename,name);strcat(filename,".xml");
    if((publisher_file = fopen(filename,"r")) != NULL){
        fclose(publisher_file);
        return 0;
    }
    int nr = xml_publisher_creator(name);
    if(nr > 0){
        strcat(name,"\n");
        add_search("./search/publishers.txt",name);
        update_server();
    }
    return nr;
}

int add_genre(void *arg){
    FILE* genre_file;
	struct thData tdL;
    char name[NORMAL_LEN];
	tdL = *((struct thData*)arg);

    if(read(tdL.cl,&name,sizeof(name)) <= 0){
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    name[strlen(name)-1]='\0';

    char filename[FILENAME_LEN];
    strcpy(filename,"./genres/");strcat(filename,name);strcat(filename,".xml");
    if((genre_file = fopen(filename,"r")) != NULL){
        fclose(genre_file);
        return 0;
    }
    int nr = xml_genre_creator(name);
    if(nr > 0){
        strcat(name,"\n");
        add_search("./search/genres.txt",name);
        update_server();
    }
    return nr;
}

/*int add_tag(void *arg){
    FILE* genre_file;
	struct thData tdL;
    char name[NORMAL_LEN];
	tdL = *((struct thData*)arg);

    if(read(tdL.cl,&name,sizeof(name)) <= 0){
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);
    name[strlen(name)-1]='\0';

    char filename[FILENAME_LEN];
    strcpy(filename,"./tags/");strcat(filename,name);strcat(filename,".xml");
    if((genre_file = fopen(filename,"r")) != NULL){
        fclose(genre_file);
        return 0;
    }
    int nr = xml_tag_creator(name);
    if(nr > 0){
        strcat(name,"\n");
        add_search("./search/tags.txt",name);
        update_server();
    }
    return nr;
}*/

int edit_book(void *arg){
	struct thData tdL;
    char name[NORMAL_LEN];
    char change[NORMAL_LEN];
    char new_info[NORMAL_LEN];
    char filename[FILENAME_LEN];
	tdL = *((struct thData*)arg);

    if(read(tdL.cl,&name,sizeof(name)) <= 0){ //reading book's name
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    name[strlen(name)-1]='\0';
    strcpy(filename,"./books/");strcat(filename,name);strcat(filename,".xml");

    if(read(tdL.cl,&change,sizeof(change)) <= 0){ //reading what category to be changed
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);

    if(read(tdL.cl,&new_info,sizeof(new_info)) <= 0){ //reading the new information
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    new_info[strlen(new_info)-1]='\0';

    int nr = xml_modifier(filename,change,new_info);
    return nr;
}

int edit_author(void *arg){
	struct thData tdL;
    char name[NORMAL_LEN];
    char change[NORMAL_LEN];
    char new_info[NORMAL_LEN];
    char filename[FILENAME_LEN];
	tdL = *((struct thData*)arg);

    if(read(tdL.cl,&name,sizeof(name)) <= 0){ //reading author's name
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    name[strlen(name)-1]='\0';
    strcpy(filename,"./authors/");strcat(filename,name);strcat(filename,".xml");

    if(read(tdL.cl,&change,sizeof(change)) <= 0){ //reading what category to be changed
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);

    if(read(tdL.cl,&new_info,sizeof(new_info)) <= 0){ //reading the new information
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    new_info[strlen(new_info)-1]='\0';

    return xml_modifier(filename,change,new_info);
}

int edit_publisher(void *arg){
	struct thData tdL;
    char name[NORMAL_LEN];
    char change[NORMAL_LEN];
    char new_info[NORMAL_LEN];
    char filename[FILENAME_LEN];
	tdL = *((struct thData*)arg);

    if(read(tdL.cl,&name,sizeof(name)) <= 0){ //reading publisher's name
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    name[strlen(name)-1]='\0';
    strcpy(filename,"./publishers/");strcat(filename,name);strcat(filename,".xml");

    if(read(tdL.cl,&change,sizeof(change)) <= 0){ //reading what category to be changed
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);

    if(read(tdL.cl,&new_info,sizeof(new_info)) <= 0){ //reading the new information
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    new_info[strlen(new_info)-1]='\0';

    return xml_modifier(filename,change,new_info);
}

int edit_genre(void *arg){
	struct thData tdL;
    char name[NORMAL_LEN];
    char change[NORMAL_LEN];
    char new_info[NORMAL_LEN];
    char filename[FILENAME_LEN];
	tdL = *((struct thData*)arg);

    if(read(tdL.cl,&name,sizeof(name)) <= 0){ //reading genre's name
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    name[strlen(name)-1]='\0';
    strcpy(filename,"./genres/");strcat(filename,name);strcat(filename,".xml");

    if(read(tdL.cl,&change,sizeof(change)) <= 0){ //reading what category to be changed
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);

    if(read(tdL.cl,&new_info,sizeof(new_info)) <= 0){ //reading the new information
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    new_info[strlen(new_info)-1]='\0';

    return xml_modifier(filename,change,new_info);
}

/*int edit_tag(void *arg){
	struct thData tdL;
    char name[NORMAL_LEN];
    char change[NORMAL_LEN];
    char new_info[NORMAL_LEN];
    char filename[FILENAME_LEN];
	tdL = *((struct thData*)arg);

    if(read(tdL.cl,&name,sizeof(name)) <= 0){ //reading genre's name
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    name[strlen(name)-1]='\0';
    strcpy(filename,"./tags/");strcat(filename,name);strcat(filename,".xml");

    if(read(tdL.cl,&change,sizeof(change)) <= 0){ //reading what category to be changed
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    sleep(SLEEP_TIME);

    if(read(tdL.cl,&new_info,sizeof(new_info)) <= 0){ //reading the new information
	    printf("[Thread %d]\n",tdL.idThread);
	    printf(READ_ERROR);
        return -1;
	}
    new_info[strlen(new_info)-1]='\0';

    return xml_modifier(filename,change,new_info);
}*/
