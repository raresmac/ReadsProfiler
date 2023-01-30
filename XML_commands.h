#pragma once

#include "server_commands.h"

int xml_user_creator(char* filename,char* password){
  FILE *file;
  if((file = fopen(filename,"w")) != NULL){
    fprintf(file,"<user>\n<id_user>\n%d\n</id_user>\n<status>\n1\n</status>\n<password>\n%s\n</password>\n<books>\n<reading>\n0\n</reading>\n<read>\n0\n</read>\n<dropped>\n0\n</dropped>\n<planning>\n0\n</planning>\n</books>\n<genres>\n0\n</genres>\n</user>",++number_ids[0],password);
  }
  else{
    return -1;
  }
  fclose(file);
  return 1;
}

int xml_author_creator(char *name,char *birth_place,char *birth_date,char *death_date){
  FILE *file;
  char filename[FILENAME_LEN];
  strcpy(filename,"./authors/");strcat(filename,name);strcat(filename,".xml");

  if((file = fopen(filename,"w")) != NULL){
    fprintf(file,"<author>\n<id_author>\n%d\n</id_author>\n<name>\n%s\n</name>\n<birth_place>\n%s\n</birth_place>\n<birth_date>\n%s\n</birth_date>\n<death_date>\n%s\n</death_date>\n<books>\n0\n</books>\n<genres>\n0\n</genres>\n</author>\n",++number_ids[2],name,birth_place,birth_date,death_date);
  }
  else{
    return -1;
  }
  fclose(file);
  return 1;
}

int xml_publisher_creator(char *name){
  FILE *file;
  char filename[FILENAME_LEN];
  strcpy(filename,"./publishers/");strcat(filename,name);strcat(filename,".xml");

  if((file = fopen(filename,"w")) != NULL){
    fprintf(file,"<publisher>\n<id_publisher>\n%d\n</id_publisher>\n<name>\n%s\n</name>\n<books>\n0\n</books>\n</publisher>",++number_ids[3],name);
  }
  else{
    return -1;
  }
  fclose(file);
  return 1;
}

int xml_genre_creator(char *name){
  FILE *file;
  char filename[FILENAME_LEN];
  strcpy(filename,"./genres/");strcat(filename,name);strcat(filename,".xml");

  if((file = fopen(filename,"w")) != NULL){
    fprintf(file,"<genre>\n<id_genre>\n%d\n</id_genre>\n<name>\n%s\n</name>\n<books>\n0\n</books>\n</genre>",++number_ids[4],name);
  }
  else{
    return -1;
  }
  fclose(file);
  return 1;
}

/*int xml_tag_creator(char *name){
  FILE *file;
  char filename[FILENAME_LEN];
  strcpy(filename,"./tags/");strcat(filename,name);strcat(filename,".xml");

  if((file = fopen(filename,"w")) != NULL){
    fprintf(file,"<tag>\n<id_tag>\n%d\n</id_tag>\n<name>\n%s\n</name>\n<books>\n0\n</books>\n</tag>",++number_ids[5],name);
  }
  else{
    return -1;
  }
  fclose(file);
  return 1;
}*/

int xml_book_creator(char *name,char *isbn,char *author,char *publisher,int nr_genres,char genres[MAX_GENRES][NORMAL_LEN]){
  FILE *file;
  char filename[FILENAME_LEN];
  strcpy(filename,"./books/");strcat(filename,name);strcat(filename,".xml");

  if((file = fopen(filename,"w")) != NULL){
    fprintf(file,"<book>\n<id_book>\n%d\n</id_book>\n<name>\n%s\n</name>\n<isbn>\n%s\n</isbn>\n<author>\n%s\n</author>\n<publisher>\n%s\n</publisher>\n<average>\n0\n</average>\n<genres>\n%d\n",++number_ids[1],name,isbn,author,publisher,nr_genres);
    for(int i = 0; i < nr_genres; i++){
      fprintf(file,"%s\n",genres[i]);
    }
    fprintf(file,"</genres>\n<users>\n0\n</users>\n</book>\n");
  }
  else{
    return -1;
  }
  fclose(file);
  return 1;
}

int xml_modifier(char *filename,char *finder,char *modifier){
  int ok = 0;
  FILE *file,*tmp;
  tmp = fopen("xml.tmp","w");
  if((file = fopen(filename,"r")) == NULL){
    return -1;
  }
  else{
    char *buffer = NULL;
    size_t len_buffer = NORMAL_LEN;

    //parsing the xml until discovering the category
    getline(&buffer,&len_buffer,file);
    fprintf(tmp,"%s",buffer);
    while(getline(&buffer,&len_buffer,file) != -1){
      fprintf(tmp,"%s",buffer);
      if(strcmp(buffer,finder) == 0){ //found the requested category
          ok = 1;
          getline(&buffer,&len_buffer,file);
          fprintf(tmp,"%s\n",modifier); //printing modifier instead of the buffer
          while(getline(&buffer,&len_buffer,file) != -1){
            fprintf(tmp,"%s",buffer);
          }
          break;
      }
    }
    fclose(file);
    fclose(tmp);
    if(ok == 0) return 0;
    
    file = fopen(filename,"w");
    tmp = fopen("xml.tmp","r");
    while(getline(&buffer,&len_buffer,tmp) != -1){
      fprintf(file,"%s",buffer);
    }
    fclose(file);
    fclose(tmp);
    return 1;
  }
}

int xml_category(char *filename,char *finder,char keeper[MAX_BOOKS][NORMAL_LEN]){
  FILE *file;
  if((file = fopen(filename,"r")) == NULL){
    return -1;
  }
  char *buffer = NULL;
  size_t len = 0;
  while(getline(&buffer,&len,file) != -1){
    buffer[strlen(buffer)-1]='\0';
    if(strcmp(buffer,finder) == 0){
      if(getline(&buffer,&len,file) != -1){
        int nr = atoi(buffer);
        for(int i = 0; i < nr; i++){
            fgets(keeper[i],NORMAL_LEN,file);
            if(strcmp(finder,"<read>") == 0){
              getline(&buffer,&len,file);
              strcat(keeper[i],buffer);
            }
            else if(strcmp(finder,"<reading>") && strcmp(finder,"<dropped>") && strcmp(finder,"<planning>")){
              getline(&buffer,&len,file);
            }
        }
        fclose(file);
        return nr;
      }
    }
  }
  fclose(file);
  return 0;
}

int xml_retriever(char *filename,char *finder,char *keeper){
  int line_found = 1;
  FILE *file;
  if((file = fopen(filename,"r")) == NULL){
    return -1;
  }
  char *buffer = NULL;
  size_t len = 0;
  getline(&buffer,&len,file);
  while(getline(&buffer,&len,file) != -1){
    buffer[strlen(buffer)-1]='\0';
    if(strcmp(buffer,finder) == 0){
      if(getline(&keeper,&len,file) != -1){
        keeper[strlen(keeper)-1]='\0';
        fclose(file);
        return line_found;
      }
      else{
        fclose(file);
        return -1;
      }
    }
    line_found++;
  }
  fclose(file);
  return 0;
}

int xml_incrementer(char *filename,char *finder,char *new_info){
  FILE *file,*tmp;
  tmp = fopen("xml.tmp","w");
  if((file = fopen(filename,"r")) == NULL){
    return -1;
  }
  else{
    char *buffer = NULL;
    size_t len_buffer = NORMAL_LEN;

    //parsing the xml until discovering the category
    getline(&buffer,&len_buffer,file);
    fprintf(tmp,"%s",buffer);
    while(getline(&buffer,&len_buffer,file) != -1){
      fprintf(tmp,"%s",buffer);
      if(strcmp(buffer,finder) == 0){ //found the requested category
          getline(&buffer,&len_buffer,file);
          int nr = atoi(buffer);
          fprintf(tmp,"%d\n%s\n",nr+1,new_info);
          while(getline(&buffer,&len_buffer,file) != -1){
            fprintf(tmp,"%s",buffer);
          }
          break;
      }
    }
    fclose(file);
    fclose(tmp);
    
    file = fopen(filename,"w");
    tmp = fopen("xml.tmp","r");
    while(getline(&buffer,&len_buffer,tmp) != -1){
      fprintf(file,"%s",buffer);
    }
    fclose(file);
    fclose(tmp);
    return 1;
  }
}

int xml_decrementer(char *filename,char *finder,char *old_info){
  FILE *file,*tmp;
  tmp = fopen("xml.tmp","w");
  if((file = fopen(filename,"r")) == NULL){
    fclose(tmp);
    return -1;
  }
  else{
    char *buffer = NULL;
    size_t len_buffer = NORMAL_LEN;
    int ok = 0, i = 0;
    int nr;

    getline(&buffer,&len_buffer,file); //<user>
    fprintf(tmp,"%s",buffer);
    //parsing the xml until discovering the category
    while(getline(&buffer,&len_buffer,file) != -1){
      sleep(0.5);
      fflush(stdout);
      fprintf(tmp,"%s",buffer);
      if(strcmp(buffer,finder) == 0){ //found the requested category
        getline(&buffer,&len_buffer,file);
        nr = atoi(buffer);
        fprintf(tmp,"%d\n",nr-1); //printing decremented number
        while(getline(&buffer,&len_buffer,file) != -1){
          if(i < nr && strcmp(buffer,old_info) == 0 && ok == 0){
            ok = 1;
            getline(&buffer,&len_buffer,file);
            if(strcmp(finder,"<users>\n") == 0 || strcmp(finder,"<read>\n") == 0){
              getline(&buffer,&len_buffer,file);
            }
          }
          i++;
          fprintf(tmp,"%s",buffer);
        }
        break;
      }
    }
    fclose(file);
    fclose(tmp);    
    
    file = fopen(filename,"w");
    tmp = fopen("xml.tmp","r");
    while(getline(&buffer,&len_buffer,tmp) != -1){
      fprintf(file,"%s",buffer);
    }
    fclose(tmp);
    fclose(file);
    return 1;
  }
}
