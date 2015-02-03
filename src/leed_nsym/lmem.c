
#include <malloc.h>
#include <assert.h>
#include <leed.h>

int leed_crystal_bulk_init(leed_crystal *bulk)
{
  if (bulk == NULL) 
  {
    if ((bulk = (leed_crystal*) malloc(sizeof(leed_crystal))) == NULL)
    {
      assert(bulk == NULL);
    }
  }
  
  /* initialise variables */
  bulk->comments = NULL;
  
  return(0);
}
