#ifndef hashtableH
#define hashtableH
/** HashTable implements a hashtable with Cstring keys and pointer elements
*   The first part of the element MUST contain the Cstring key
*   Collisions are handled by separate chaining
*/

//---------------------------------------------------------------------------
typedef struct  tableItem {char * element; tableItem *next;} tableItem;

class HashTable {
public:
   HashTable(int n=257);
   char * findElement(char * key);
   void insertElement(char * element);
   void deleteElement(char * element);
private:
   int hash(char * key);
   void expand();

   int size;
   int count;
   tableItem **table;
};

#endif
