#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 100


void sort(int n, char** keys,char** data){
    for(int i=1;i<n;i++){
        char* key=keys[i];
        char* dat=data[i];
        int j=i-1;
        while(j>=0 && strcmp(keys[j],key)>0){
            keys[j+1]=keys[j];
            data[j+1]=data[j];
            j--;
        }
        keys[j+1]=key;
        data[j+1]=dat;
    }
}

void print(int n,char** keys,char** data){
    for(int i=0;i<n;i++){
        printf("| -18%s | -18%s |\n",keys[i],data[i]);
    }
    printf("\n");
}

int binpoisk(int n,char** keys,char* key){
    int left=0;
    int right=n-1;
    while(left<=right){
        int middle=left+(right-left)/2;
        int cmp=strcmp(keys[middle],key);
        if(cmp>0){
            right=middle-1;
        }
        else if(cmp<0){
            left=middle+1;
        }
        else{
            return middle;
        }
    }
    return -1;

}

int main(){
    FILE* fp=fopen("text.txt","r");
    int n;
    fscanf(fp,"%d",&n);
    char** keys=(char**)malloc(sizeof(char[SIZE])*n);
    char** data=(char**)malloc(sizeof(char[SIZE])*n);
    for(int i=0;i<n;i++){
        fscanf(fp,"%s %s",&(*keys[i]),&(*data[i]));
    }
    fclose(fp);
    print(n,keys,data);
    sort(n,keys,data);
    print(n,keys,data);
    return 0;
}