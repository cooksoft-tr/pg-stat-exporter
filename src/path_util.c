#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "path_util.h"
#include "errors.h"

// Reads the contents of a file at 'path' into a buffer, returns the buffer
char *path_to_buffer(const char *path)
{
  if (!path)
    return NULL;

  FILE *tplf = fopen(path, "r");
  if (!tplf)
  {
    fprintf(stderr, "Info: file doesn't exists: %s\n", path);
    return NULL;
  }

  fseek(tplf, 0, SEEK_END);
  long tpl_size = ftell(tplf);
  fseek(tplf, 0, SEEK_SET);

  char *tpl_buf = malloc(tpl_size + 1);
  if (!tpl_buf)
  {
    fclose(tplf);
    return NULL;
  }

  size_t read_size = fread(tpl_buf, 1, tpl_size, tplf);
  tpl_buf[read_size] = '\0';
  fclose(tplf);

  return tpl_buf;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 * Loads a file into a dynamically allocated buffer from either a custom
 * base directory or a default base directory.
 *
 * Parameters:
 *   custom_base_dir  - optional custom base directory (checked first if provided)
 *   base_dir         - default base directory used as fallback
 *   subdir           - subdirectory name (e.g., "database" or "cluster")
 *   postgres_version - PostgreSQL version (must be greater than 0)
 *   metric_name      - name of the metric / file (without extension)
 *   ext              - file extension (e.g., ".tpl", ".sql")
 *   out_buf          - pointer to the output buffer; must be NULL on input
 *
 * Returns:
 *   ERR_OK            - file successfully loaded into buffer
 *   ERR_INVALID_ARG   - invalid arguments or *out_buf is not NULL
 *   ERR_PATH_TOO_LONG - constructed file path exceeds buffer limits
 *   ERR_NOT_FOUND     - file not found in either custom or default directory
 *
 * Notes:
 *   - Memory for the buffer is allocated within the function; caller must free it
 *   - The function does not modify *out_buf if it is not NULL
 */
ErrorCode load_file_to_buffer(const char *custom_base_dir,
                              const char *base_dir,
                              const char *subdir,
                              int postgres_version,
                              const char *metric_name,
                              const char *ext,
                              char **out_buf)
{
  char path[512];
  int ret;

  if (postgres_version <= 0 || !base_dir || !subdir || !metric_name || !ext || !out_buf)
  {
    fprintf(stderr, "load_file_to_buffer: invalid argument\n");
    return ERR_INVALID_ARG;
  }

  if (*out_buf != NULL)
  {
    fprintf(stderr, "load_file_to_buffer: output buffer is not NULL\n");
    return ERR_INVALID_ARG;
  }

  // Try loading from custom directory first if provided
  if (custom_base_dir)
  {
    ret = snprintf(path, sizeof(path), "%s/%s/%d/%s%s",
                   custom_base_dir,
                   subdir,
                   postgres_version,
                   metric_name,
                   ext);

    if (ret < 0 || ret >= (int)sizeof(path))
    {
      fprintf(stderr, "load_file_to_buffer: path truncated or encoding error (custom)\n");
      return ERR_PATH_TOO_LONG;
    }

    *out_buf = path_to_buffer(path);
  }

  // Fallback to default directory if custom failed or not provided
  if (*out_buf == NULL)
  {
    ret = snprintf(path, sizeof(path), "%s/%s/%d/%s%s",
                   base_dir,
                   subdir,
                   postgres_version,
                   metric_name,
                   ext);

    if (ret < 0 || ret >= (int)sizeof(path))
    {
      fprintf(stderr, "load_file_to_buffer: path truncated or encoding error (default)\n");
      return ERR_PATH_TOO_LONG;
    }

    *out_buf = path_to_buffer(path);
  }

  // Final check
  if (*out_buf == NULL)
  {
    fprintf(stderr,
            "load_file_to_buffer: failed to load file for metric '%s' (version %d)\n",
            metric_name,
            postgres_version);
    return ERR_NOT_FOUND;
  }

  return ERR_OK;
}
