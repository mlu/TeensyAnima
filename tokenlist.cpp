#include "tokenlist.h"
#include <stdlib.h>

/** Changelog
030202 - Hashtable to store symbol table
*/
/** @todo Linked sequence of symbol tables
*/


TokenList::TokenList(TOKENELEMENT  *pTokenList):symboltable(1001){
   token = pTokenList;
   parentList = NULL;
   NrTokens = 0;
   while (token[NrTokens].name[0] != 0) {
      addToken(token+NrTokens);
      NrTokens++;
   }
};

TOKENELEMENT * TokenList::FindToken(char * tstring) {
/* Search tokenlist for token "tstring" */
   TOKENELEMENT * tp=(TOKENELEMENT *)symboltable.findElement(tstring);
   if ((tp == NULL) && parentList) return parentList->FindToken(tstring);
   return tp;
}

TOKENELEMENT * TokenList::addToken(TOKENELEMENT * param)  {
   TOKENELEMENT * te=new TOKENELEMENT;
   *te = *param;
   symboltable.insertElement((char *)te);
   return te;
}

int TokenList::removeToken(TOKENELEMENT * param)  {
   symboltable.deleteElement((char *)param);
return 0;
}


