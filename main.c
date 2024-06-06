#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef struct Directory
{
  int parent;
  char *name;
  int file_size;
  int childrens[25];
  int childrens_count;
} Directory;

typedef struct Directories
{
  size_t count;
  size_t capacity;
  Directory *items;
} Directories;

const size_t DIR_SIZE = 100;

void directories_append(Directories *da, Directory d)
{
  if (da->count >= da->capacity)
  {
    da->capacity = da->capacity == 0 ? DIR_SIZE :  da->capacity*2;
    da->items = realloc(da->items, da->capacity*sizeof(*da->items));
  }

  da->items[da->count++] = d;
}

Directory *create_directory(char *name, int parent)
{
  Directory *d = calloc(1, sizeof(Directory));
  d->name = name;
  d->parent = parent;
  return d;
}

enum COMMAND
{
  LS,
  CD,
};

typedef struct command
{
  enum COMMAND type;
  char *data;
} command;

void tokenize_ls(Directories *dirs, size_t dir_idx, char *data)
{
  size_t dir_size = 0;
  Directory dir = dirs->items[dir_idx];

  char *delim = "\n";
  char *line = strtok(data, delim);
  while (line != NULL)
  {
    if (strlen(line) == 0) {
      line = strtok(NULL, delim);
      continue;
    }

    char *end;
    char *tok1 = strtok_r(line, " ", &end);
    char *tok2 = strtok_r(end, " ", &end);

    char *temp;
    long size = strtol(tok1, &temp, 10);

    if (temp == tok1)
    {
      // no chars consumed. ie first thing is not a file size
      // ie it is a directory type.
      Directory *d = create_directory(tok2, dir_idx); // TODO: directory parent.
      d->parent = dir_idx;
      dir.childrens[dir.childrens_count++] = dirs->count - 1;
      directories_append(dirs, *d);
    }
    else
    {
      // recieved file size
      dir_size += size;
    }

    line = strtok(NULL, delim);
  }
}

size_t find_dir_by_name(Directories dirs, int* idx, char *name)
{
if (strncmp(name, "..", 2) == 0) {
    return dirs.items[idx].parent;
  }

  for (int i = 0; i < dirs.count; i++)
  {
    Directory d = dirs.items[i];
    printf(" '%s' - '%s' -  Does it match: %s\n", name, d.name, strcmp(name, d.name) == 0 ? "YES" : "NO");
    printf("==============\n");

    if (strcmp(name, d.name) == 0)
    {
      return i;
    }
  }

  return -1;
}

command parse_cmd(char *data)
{
  if (strncmp(data, " cd ", 4) == 0)
  {
    data += 4;
    char *tok = strtok(data, "\n");
    command cmd = {.type = CD, .data = tok};
    return cmd;
  }
  else if (strncmp(data, " ls", 3) == 0)
  {
    data += 3;
    command cmd = {.type = LS, .data = data};
    return cmd;
  }
  else
  {
    assert(false && "Unknown command");
  }
}

int main()
{
  char buf[1024];

  char c;
  size_t i = 0;

  // directories state
  int dir_idx = 0;
  Directories dirs = {0};

  Directory *d = create_directory("/", -1);
  directories_append(&dirs, *d);

  c = getchar();
  while (c != EOF)
  {
    c = getchar();

    if (c == '$')
    {
      buf[i++] = '\0';
      command cmd = parse_cmd(buf);
      if (cmd.type == LS)
      {
        tokenize_ls(&dirs, dir_idx, cmd.data);
      }
      else if (cmd.type == CD)
      {
        int idx = find_dir_by_name(dirs, &dir_idx, cmd.data);
        dir_idx = idx;
        printf("updating dir_idx: %d\n", dir_idx);
      }

      // reset to point at start
      i = 0;
      continue;
    }

    buf[i++] = c;
    assert(i < 1024 && "BUFFER OVERFLOW...");
  }

  for (int i = 0; i < dirs.count; i++)
  {
    Directory d = dirs.items[i];
    printf("directory: %s - %d (Parent Idx - %d) (Children count - %d)\n", d.name, d.file_size, d.parent, d.childrens_count);
  }

  return 0;
}
