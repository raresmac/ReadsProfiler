#pragma once

#include "basic_data.h"

int add_search(char *filename,char *name){
    int ok = 0;
    FILE *search_file = fopen(filename,"r");
    FILE *tmp = fopen("search.tmp","w");
    char *buffer = NULL;
    size_t len_buffer = NORMAL_LEN;

    //parsing the xml until the name is added alphabetically
    while(getline(&buffer,&len_buffer,search_file) != -1){
        if(strcmp(buffer,name) < 0){
            fprintf(tmp,"%s",buffer);
        }
        else{
            ok = 1;
            fprintf(tmp,"%s%s",name,buffer);
            while(getline(&buffer,&len_buffer,search_file) != -1){
                fprintf(tmp,"%s",buffer);
            }
            break;
        }
    }
    if(ok == 0){
        fprintf(tmp,"%s",name);
    }
    fclose(search_file);
    fclose(tmp);
    
    search_file = fopen(filename,"w");
    tmp = fopen("search.tmp","r");
    while(getline(&buffer,&len_buffer,tmp) != -1){
        fprintf(search_file,"%s",buffer);
    }
    fclose(search_file);
    fclose(tmp);
    return 1;
}

int remove_search(char *filename,char *name){
    FILE *search_file = fopen(filename,"r");
    FILE *tmp = fopen("search.tmp","w");
    char *buffer = NULL;
    size_t len_buffer = NORMAL_LEN;

    //parsing the xml until the name is added alphabetically
    while(getline(&buffer,&len_buffer,search_file) != -1){
        if(strcmp(buffer,name) == 0){
            fprintf(tmp,"%s",buffer);
            while(getline(&buffer,&len_buffer,search_file) != -1){
                fprintf(tmp,"%s",buffer);
            }
            break;
        }
        fprintf(tmp,"%s",buffer);
    }
    fclose(search_file);
    fclose(tmp);
    
    search_file = fopen(filename,"w");
    tmp = fopen("search.tmp","r");
    while(getline(&buffer,&len_buffer,tmp) != -1){
        fprintf(search_file,"%s",buffer);
    }
    fclose(search_file);
    fclose(tmp);
    return 1;
}

int nr_search(char *filename){
    int nr = 0;
    FILE *search_file = fopen(filename,"r");
    char *buffer = NULL;
    size_t len_buffer = NORMAL_LEN;

    while(getline(&buffer,&len_buffer,search_file) != -1){
        nr++;
    }
    fclose(search_file);
    return nr;
}
