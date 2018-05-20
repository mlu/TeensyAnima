#include "hashtable.h"
#include <string.h>

HashTable::HashTable(int n){
   size = n;
   count = 0;
   table = new tableItem *[n];
   for (int k=0;k<n;k++) table[k]=NULL;
};

char * HashTable::findElement(char * key){
   int kv=hash(key);
   tableItem * ti=table[kv];
   while (ti && (strcmp(ti->element,key)!=0))
      ti = ti->next;
   if (ti) return ti->element;
   return NULL;
};

void HashTable::insertElement(char * element){
   int kv=hash(element);
   tableItem ** tip=&(table[kv]);
   while ((*tip) && (strcmp((*tip)->element,element)!=0))
      tip = &((*tip)->next);
   if (*tip == NULL) {
      *tip = new tableItem;
      count++;
      (*tip)->next = NULL;
      }
   else
      delete (*tip)->element;
   (*tip)->element = element;
};

void HashTable::deleteElement(char * element){
   tableItem * ne;
   int kv=hash(element);
   tableItem ** tip=&table[kv];
   while ((*tip) && (strcmp((*tip)->element,element)!=0))
      tip = &((*tip)->next);
   if (*tip == NULL)
      return;
   ne = (*tip)->next;
   delete (*tip)->element;
   delete (*tip);
   count--;
   *tip = ne;
};


int HashTable::hash(char * key){
   unsigned int kv=0;
   int i=0;
   while (key[i] !=0)
      kv=3*kv+key[i++];
   kv = kv % size;
   return kv;
};

void HashTable::expand(){};

