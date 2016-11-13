struct histnode{

  char command[MAX_LENGTH];
  struct histnode *next;
};

void print(struct histnode *head)
{
  printf("HISTORY:\n");
  struct histnode *traverse = head;

  while(traverse != NULL)
  {
    printf("    %s", traverse->command);
    traverse = traverse->next;
  }
}

