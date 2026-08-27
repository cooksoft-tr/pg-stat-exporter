#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

/* Key-value çifti */
typedef struct
{
  const char *key;   /* Sütun adı */
  const char *value; /* Hücre değeri */
} db_column_value_t;

typedef struct
{
  db_column_value_t *values; /* Array pointer */
  size_t count;              /* Array boyutu (kaç çift var) */
} db_row_t;

typedef struct
{
  db_row_t **rows; /* her satır için db_row_t* */
  size_t count;    /* toplam satır sayısı */
} db_table_t;

int array_count(const char **arr);

// Frees a NULL-terminated array of strings
void free_string_array(char **array);
#endif
