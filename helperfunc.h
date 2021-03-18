struct Node
{
  char* key;
  char* data;
  struct Node *next;
};
struct Node* append(struct Node*, char*, char*);
char* find(struct Node*, char* );
