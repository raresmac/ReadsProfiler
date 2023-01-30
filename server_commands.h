#pragma once

#include "basic_data.h"

void initialize_server(){
    FILE* id_data;
    char* buffer = NULL;
    size_t len = 0;
    if((id_data = fopen("id_data.txt","r")) != NULL){
        for(int i = 0; i < 5; i++){
            if(getline(&buffer,&len,id_data) != -1){
                number_ids[i] = atoi(buffer);
            }
        }
    }
    fclose(id_data);
    printf("The server has been initialized!\n");
}

void update_server(){
    FILE* id_data;
    if((id_data = fopen("id_data.txt","w")) != NULL){
        for(int i = 0; i < 5; i++){
            fprintf(id_data,"%d\n",number_ids[i]);
        }
    }
    printf("The server has been updated!\n");
    fclose(id_data);
}
