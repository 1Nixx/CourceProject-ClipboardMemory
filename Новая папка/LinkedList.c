#include "LinkedList.h"
#include <stdlib.h>

void Push(Node** head, CLIPBOARDDATA data)
{
    Node* tmp = (Node*)malloc(sizeof(Node));
    tmp->value = data;
    tmp->next = (*head);
    (*head) = tmp;
}

CLIPBOARDDATA *Pop(Node** head)
{
    Node* prev = NULL;
    CLIPBOARDDATA val;
    if (head == NULL) 
    {
        return NULL;
    }

    prev = (*head);
    val = prev->value;
    (*head) = (*head)->next;
    free(prev);

    return &val;
}

Node* GetNth(Node* head, int n) 
{
    int counter = 1;
    while (counter < n && head) 
    {
        head = head->next;
        counter++;
    }
    return head;
}

Node* GetLast(Node* head) 
{
    if (head == NULL) 
    {
        return NULL;
    }
    while (head->next)
    {
        head = head->next;
    }
    return head;
}

void PushBack(Node* head, CLIPBOARDDATA value)
{
    Node* last = GetLast(head);
    Node* tmp = (Node*)malloc(sizeof(Node));
    tmp->value = value;
    tmp->next = NULL;
    last->next = tmp;
}

CLIPBOARDDATA *DeleteNth(Node** head, int n)
{
    if (n == 1) 
    {
        return Pop(head);
    }
    else 
    {
        Node* prev = GetNth(*head, n - 1);
        Node* elm = prev->next;
        CLIPBOARDDATA val = elm->value;

        prev->next = elm->next;
        free(elm);
        return &val;
    }
}

void DeleteList(Node** head) 
{
    Node* prev = NULL;
    while ((*head)->next) 
    {
        prev = (*head);
        (*head) = (*head)->next;
        free(prev);
    }
    free(*head);
}