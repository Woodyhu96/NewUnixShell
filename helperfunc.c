#include "sh.h"
#include "helperfunc.h"

struct Node* append(struct Node* head, char* string, char* key){
    struct Node* cur = head;
    if(head != NULL){
        while(cur->next != NULL){
            cur = cur->next;
        }
    }
    struct Node* new = (struct Node*)malloc(sizeof(struct Node));
      new->data = (char*)malloc(strlen(string));
      if(key != NULL){
        new->key = (char*)malloc(strlen(key));
        strcpy(new->key, key);
      }
      else{
         new->key = NULL;
      }
      strcpy(new->data, string);
      new->next = NULL;

      if(head == NULL){
          head = new;
      }
      else{
          cur->next = new;
      }
      return head;
}
char* find(struct Node* head, char* string){
    if(head != NULL){
        struct Node* cur = head;
        while(cur != NULL){
            if(strcmp(cur->data, string) == 0){
                char* result = (char*)malloc(strlen(string));
                strcpy(result, cur->key);
                return result;
            }
            cur = cur->next;
        }
    }
    return NULL;
}
