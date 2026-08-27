#include <stdlib.h>

void free_string_array(char **array)
{
  if (!array)
    return;

  for (size_t i = 0; array[i]; ++i)
    free(array[i]);

  free(array);
}


int array_count(const char **arr)
{
  if (!arr)
    return 0;
    
  int count = 0;
  while (arr[count])
    count++;
  return count;
}
