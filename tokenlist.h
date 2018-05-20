#ifndef tokenlistH

#define tokenlistH

//---------------------------------------------------------------------------

#include "movecmd.h"

#include "hashtable.h"



class TokenList {

public:

  TokenList(TOKENELEMENT  *pTokenList);

  TOKENELEMENT * FindToken(char * tstring);

  TOKENELEMENT * addToken(TOKENELEMENT * param);

  int removeToken(TOKENELEMENT * param);

  TokenList * parentList;

private:

  HashTable symboltable;

  TOKENELEMENT  *token;

  unsigned int NrTokens;

};



#endif

