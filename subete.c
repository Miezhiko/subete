#ifdef _WIN32
#include <Windows.h>
#endif

#include "ctodo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* dest_help = NULL;
char* db        = NULL;

char* subete_version() {
  return "  subete v0.0.4\n";
}

char* subete_help() {
  dest_help = calloc(4000, sizeof(char));
  strcpy(dest_help, subete_version()); // NOLINT
  strcat(dest_help,                    // NOLINT
"  * usage:\n\
    subete <command> <arguments>\n\
  - --init - init empty database structure\n\
    (set default database options without data lose, useful if you or some update broke it)\n\
  - <without options> - to read all, --list / -l to read all from specified list\n\
      --list / -l - read specified list, by default reads all lists\n\
  - <msg> - just write todo <msg> to add new node to your todo list\n\
      --first or -1 to put task on top priority\n\
      --motivate - end todo note with additional word (see ending option)\n\
      --list / -l - write to specified list\n\
      - db <db3path> - use another database file (works for other commands too) \n\
  - --edit or -e <number> <msg> - edit task\n\
  - --mv <number1> <number2> - move task\n\
  - --rm <number1,number2> (or space separated) - delete tasks\n\
  - --clean / --clear - remove all tasks\n\
  - --swap or -s <number1> <number2> - swap elements\n\
  - --sync - text synchronization to avoid binaries in vcs\n\
  - --history - read sync repository todo history \n\
  - --set <option> <value> - todo options, available options (--show for read):\n\
      - syncdir - directory for vcs synchronization\n\
      - syncfile - file for text serialization for synchronization (default 'readme.md')\n\
        - git - execute git synchronization 1/0 for enable/disable (default 1)\n\
        - hg - execute mercurial synchronization 1/0 for enable/disable (default 0)\n\
        - svn - execute subversion synchronization 1/0 for enable/disable (default 0)\n\
        - darcs - execute darcs synchronization 1/0 for enable/disable (default 0)\n\
      - end - always end todo notes with additional word (default 0)\n\
      - ending - word, using for end feature (default 'be a man')\n");
#ifndef _WIN32
    strcat(dest_help,                    // NOLINT
"\
      - home - file for home path (request for synchronization)\n\
      - color - ctodo color scheme for posix (default 'red')\n\
        - schemas: red, blink, green, pink, black\n");
#endif
    return &dest_help[0];
}

int ctodo_read_meta(int list, char** out) {
  int x, maxl;
#ifdef _WIN32
  memcpy(&maxl, out[1], sizeof(int));
#else
  memcpy(&maxl, out[3], sizeof(int)); // NOLINT
#endif
  if (out != NULL) {
#ifdef _WIN32
    printf("+%s+\n\r", out[0]);
#else
    printf(" %s", out[2]);
    printf("╔%s╗", out[0]);
    printf("%c[%dm\n\r", 0x1B, 0);
#endif
#ifdef _WIN32
    for (x = 2; x < maxl; x += 2) {
      printf("| %s| %s|\n\r", out[x], out[x + 1]);
#else
    for (x = 4; x < maxl; x += 2) {
      printf(" %s║", out[2]);
      printf("%c[%dm", 0x1B, 0);
      printf("%s", out[x]);
      printf("%s│", out[2]);
      printf("%c[%dm", 0x1B, 0);
      printf("%s", out[x + 1]);
      printf("%s║", out[2]);
      printf("%c[%dm\n", 0x1B, 0);
#endif
    }
#ifdef _WIN32
    printf("+%s+\n\r", out[0]);
#else
    printf(" %s", out[2]);
    printf("╚%s╝", out[1]);
    printf("%c[%dm\n\r", 0x1B, 0);
#endif
    return 0;
  } else {
    return -1;
  }
}

int ctodo_read(int list) {
  char** out;
  if (list == -1) out = todo_read(0, 0);
  else out = todo_read(list, 1);
  if (out == NULL) return -1;
  return ctodo_read_meta(list, out);
}

int ctodo_read_custom(int list, char* db) {
  char** out;
  if (list == -1) out = todo_read_custom(0, 0, db);
  else out = todo_read_custom(list, 1, db);
  if (out == NULL) return -1;
  return ctodo_read_meta(list, out);
}

char dbcheck(int argc, char* argv[]) {
  for (int argi = 1; argi < (argc - 1); argi++) {
    if ((strcmp(argv[argi], "--db") == 0)) {
      db = calloc(200, sizeof(char));
      sprintf(db, "%s", argv[argi + 1]); // NOLINT
      return 1;
    }
  }
  return 0;
}

int main(int argc, char* argv[]) {
  int return_value = 0;
  if (argc < 2) return_value = ctodo_read(-1);
  else {
    if ((strcmp(argv[1], "--version") == 0) || (strcmp(argv[1], "-v") == 0)) {
      printf("%s", subete_version());
      printf("%s", todo_version());
    } else if ((strcmp(argv[1], "--help") == 0) || (strcmp(argv[1], "-h") == 0)) {
      printf("%s", subete_help());
      if (dest_help) free(dest_help);
      printf("%s", todo_help());
    } else if ((strcmp(argv[1], "--list") == 0) || (strcmp(argv[1], "-l") == 0)) {
      if (argc < 3) {
        printf("you need to specify list alike --list 1\n\r");
        return_value = -1;
      } else return_value = ctodo_read(atoi(argv[2]));
    } else if (strcmp(argv[1], "--db") == 0) {
      if (argc < 3) {
        printf("you need to specify db file alike --db mydb.db3\n\r");
        return_value = -1;
      } else {
        db = calloc(200, sizeof(char));
        sprintf(db, "%s", argv[2]); // NOLINT
        if (argc < 4) return_value = ctodo_read_custom(-1, db);
        else {
          if (strcmp(argv[3], "--list") == 0) {
            if (argc < 5) {
              printf("you need to specify list alike --list 1\n\r");
              return_value = -1;
            } else return_value = ctodo_read_custom(atoi(argv[4]), db);
          } else return_value = ctodo_read_custom(-1, db);
        }
      }
    } else if (strcmp(argv[1], "--init") == 0 && argc == 2) {
      if (dbcheck(argc, argv) == 0) todo_initdb();
      else todo_initdb_custom(db);
      printf("done.\n\r");
    } else if (strcmp(argv[1], "--set") == 0) {
      if (argc < 4) {
        printf("set what?\n\r");
      } else if (dbcheck(argc, argv) == 0)
        return_value = todo_set(argv, argc);
      else return_value = todo_set_custom(argv, argc, db);
    } else if (strcmp(argv[1], "--show") == 0) {
      if (argc < 3) {
        printf("show what?\n\r");
      } else if (dbcheck(argc, argv) == 0) {
        return_value = todo_show(argv, argc);
      }
      else return_value = todo_show_custom(argv, argc, db);
    } else if (strcmp(argv[1], "--history") == 0 && argc == 2) {
      if (dbcheck(argc, argv) == 0) return_value = todo_history();
      else return_value = todo_history_custom(db);
    } else if (strcmp(argv[1], "--sync") == 0 && argc == 2) {
      if (dbcheck(argc, argv) == 0) return_value = todo_sync(argv);
      else return_value = todo_sync_custom(argv, db);
    } else if ((strcmp(argv[1], "--edit") == 0) || (strcmp(argv[1], "-e") == 0)) {
      if (argc < 3) printf("edit what?\n\r");
      else if (dbcheck(argc, argv) == 0)
        todo_edit(argv, argc);
      else todo_edit_custom(argv, argc, db);
    } else if ((strcmp(argv[1], "--swap") == 0) || (strcmp(argv[1], "-s") == 0)) {
      if (argc < 3) printf("swap what?\n\r");
      else if (dbcheck(argc, argv) == 0)
        todo_swap(argv);
      else todo_swap_custom(argv, db);
    } else if (strcmp(argv[1], "--reindex") == 0 && argc == 2) {
      char answer;
      printf("This feature is currently broken, do you want to test it? (y/n)");
      if (scanf("%c", &answer) > 0) // NOLINT
      if (answer == 'y') todo_reindex();
    } else if (strcmp(argv[1], "--mv") == 0) {
      if (argc < 4) printf("move what? you need to provide two indexes\n\r");
      else if (dbcheck(argc, argv) == 0)
        todo_mv(argv);
      else todo_mv_custom(argv, db);
    } else if ((strcmp(argv[1], "--clean") == 0
             || strcmp(argv[1], "--clear") == 0) && argc == 2) {
      char answer;
      printf("Are you sure that you want to clean all the tasks? (y/n) ");
      if (scanf("%c", &answer) > 0) // NOLINT
      if (answer == 'y') {
        if (dbcheck(argc, argv) == 0)
          todo_clean();
        else todo_clean_custom(db);
      }
    } else if (strcmp(argv[1], "--rm") == 0) {
      if (argc < 3) printf("remove what?\n\r");
      else if (dbcheck(argc, argv) == 0)
        todo_rm(argc, argv);
      else todo_rm_custom(argc, argv, db);
    } if (strncmp(argv[1], "--", 2) == 0) {
      printf("There is no such command\n\n");
      printf("%s", subete_help());
      if (dest_help) free(dest_help);
      printf("%s", todo_help());
    } else {
      int argi, list = 0;
      char custom = 0;
      for (argi = 1; argi < (argc - 1); argi++) {
        if ((strcmp(argv[argi], "--list") == 0)) {
          list = atoi(argv[argi + 1]);
          argv[argi] = "";
          argv[argi + 1] = "";
        }
        if ((strcmp(argv[argi], "--db") == 0)) {
          db = calloc(200, sizeof(char));
          sprintf(db, "%s", argv[argi + 1]); // NOLINT
          argv[argi] = "";
          argv[argi + 1] = "";
          custom = 1;
        }
      }
      if (custom == 0) return_value = todo_write(argv, argc, list);
      else return_value = todo_write_custom(argv, argc, list, db);
    }
  }
  todo_close();
  if (db) {
    free (db);
  }
  return return_value;
}
