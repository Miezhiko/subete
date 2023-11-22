#define Console

#include <sqlite3.h>

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

///General properties
int retval, x, ind = 0;
char* dest = NULL;
char** out = NULL;

///Pointers
FILE* f = NULL;
time_t timefile;
sqlite3_stmt* stmt = NULL;

///Maximum sqlite query
const int q_size = 150;
char* home = NULL;
const int q_cnt = 30;

char** queries  = NULL;
sqlite3* handle = NULL;

char* todo_version() {
  return "  CTODO v0.0.5\n";
}

char* todo_help() {
  dest = calloc(4000, sizeof(char));
  strcpy(dest, todo_version()); // NOLINT
  strcat(dest,                  // NOLINT
"     todo list management lib\n");
  return &dest[0];
}

void shitHappended() {
  #ifdef Console
  printf("Shit happened\n");
  #endif
}

void timeUpdate(time_t t) {
  sprintf(queries[ind++], "INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (1,'%d')", (int)t); // NOLINT
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  if (retval) {
#ifdef Console
    printf("Failed to update db time\n");
#endif
    return;
  }
}

void sql(char* command) {
  sprintf(queries[ind++], "%s", command); // NOLINT
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
}

char* rtrim(char* str) {
  char* ptr;
  int len;
  len = strlen(str);
  for (ptr = str + len - 1; ptr >= str && isspace((int)*ptr); --ptr);
  ptr[1] = '\0';
  return str;
}

int prelude() {
  timefile = 0;
  f = NULL;
  char* temp = calloc(200, sizeof(char));
  queries = malloc(sizeof(char*)* q_cnt);
  for (x = 0; x < q_cnt; ++x)
    queries[x] = malloc(sizeof(char)* q_size);
  home = (char*)getenv("HOME");
  strcpy(temp, home); // NOLINT
  retval = sqlite3_open(strcat(temp, "/.todo.db3"), &handle); // NOLINT
  free(temp);
  if (retval) {
#ifdef Console
    printf("Database connection failed\n\r");
#endif
    return -1;
  }
  return 0;
}

int prelude_custom(char* db) {
  timefile = 0;
  f = NULL;
  queries = malloc(sizeof(char*)* q_cnt);
  for (x = 0; x < q_cnt; ++x)
    queries[x] = malloc(sizeof(char)* q_size);
  retval = sqlite3_open(db, &handle);
  if (retval) {
#ifdef Console
    printf("Database connection failed\n\r");
#endif
    return -1;
  }
  return 0;
}

void todo_close() {
  if (queries)  free(queries);
  if (dest)     free(dest);
  if (out)      free(out);
  if (stmt)     sqlite3_finalize(stmt);
  if (handle)   sqlite3_close(handle);
}

int todo_initdb_meta() {
  sql("CREATE TABLE IF NOT EXISTS TODO (id INTEGER PRIMARY KEY,text TEXT NOT NULL, list INTEGER NOT NULL)");
  if (retval) {
#ifdef Console
    printf("Init DB Failed, Shit happens?\n\r");
#endif
    return -1;
  }
  sql("CREATE TABLE IF NOT EXISTS OPTIONS (option INTEGER PRIMARY KEY,text TEXT NOT NULL)");
  sql("CREATE TABLE IF NOT EXISTS NAMELIST (option INTEGER PRIMARY KEY,name TEXT NOT NULL)");
  ///<Option>
  ///Sync file for tex serialization
  ///</Option>
  sql("INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (15,'readme.md')");
  ///<Option>
  ///Time of last synchronization
  ///</Option>
  sprintf(queries[ind++], "INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (1,'%d')", 0); // NOLINT
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  ///<Option>
  ///Use git for synchronization
  ///</Option>
  sql("INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (2,'1')");
  ///<Option>
  ///Use mercurial (hg) for synchronization
  ///</Option>
  sql("INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (3,'0')");
  ///<Option>
  ///Use subversion (svn) for synchronization
  ///</Option>
  sql("INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (4,'0')");
  ///<Option>
  ///Use darcs (darcs) for synchronization
  ///</Option>
  sql("INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (5'0')");
  ///<Option>
  ///Add ending word to each todo row
  ///</Option>
  sql("INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (12,'0')");
  ///<Option>
  ///Ending word (--motivate)
  ///</Option>
  sql("INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (13,'БЛЯДЬ')");
  ///<Option>
  ///Synchronization directory
  ///</Option>
  sql("INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (0,'~/todo')");
  ///<Option>
  ///Path for HOME (only for linux)
  ///</Option>
  sprintf(queries[ind++], "INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (20,'%s')", home); // NOLINT
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  ///<Option>
  ///Color scheme (only for linux)
  ///</Option>
  sql("INSERT OR REPLACE INTO OPTIONS (option,text) VALUES (21,'black')");
  if (retval) {
#ifdef Console
    printf("Instert deafaults options Failed, Shit happens?\n\r");
#endif
    return -1;
  }
  return 0;
}

int todo_initdb() {
  if (prelude() == -1) return -1;
  return todo_initdb_meta();
}

int todo_initdb_custom(char* db) {
  if (prelude_custom(db) == -1) return -1;
  return todo_initdb_meta();
}

int todo_set_meta(char** argv, int argc) {
  int opt = 0;
  if (argc < 4) {
#ifdef Console
    printf("set what?\n\r");
#endif
  } else if (strcmp(argv[2], "syncfile") == 0)    opt = 15;
  else if (strcmp(argv[2], "syncdir") == 0)       opt = 0;
  else if (strcmp(argv[2], "ending") == 0)        opt = 13;
  else if (strcmp(argv[2], "color") == 0)         opt = 21;
  else if (strcmp(argv[2], "end") == 0)
  if ((strcmp(argv[3], "1") == 0) || (strcmp(argv[3], "0") == 0))
    opt = 12;
  else {
#ifdef Console
    printf("Use 1 or 0 for this option\n\r");
#endif
  } else if ((strcmp(argv[2], "git") == 0)
    || (strcmp(argv[2], "hg") == 0)
    || (strcmp(argv[2], "svn") == 0)
    || (strcmp(argv[2], "darcs") == 0)) {
    if ((strcmp(argv[3], "1") == 0) || (strcmp(argv[3], "0") == 0)) {
      if (strcmp(argv[2], "git") == 0)            opt = 2;
      else if (strcmp(argv[2], "hg") == 0)        opt = 3;
      else if (strcmp(argv[2], "svn") == 0)       opt = 4;
      else if (strcmp(argv[2], "darcs") == 0)     opt = 5;
    } else {
#ifdef Console
      printf("Use 1 or 0 for this option\n\r");
#endif
    }
  }
  else if (strcmp(argv[2], "home") == 0) {
    sprintf(queries[ind++], "UPDATE OPTIONS SET text='%s' WHERE option = 20", argv[3]); // NOLINT
    retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
    if (retval) {
#ifdef Console
      printf("Option home is not changed! (shit happens)\n\r");
#endif
      return -1;
    }
  } else {
#ifdef Console
    printf("There is no such option\n\r");
#endif
    return 0;
  }
  sprintf(queries[ind++], "UPDATE OPTIONS SET text='%s' WHERE option = %d", argv[3], opt); // NOLINT
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  if (retval) {
#ifdef Console
    printf("Option is not changed! (shit happens)\n\r");
#endif
    return -1;
  }
  return 0;
}

int todo_set(char** argv, int argc) {
  if (prelude() == -1) return -1;
  else return todo_set_meta(argv, argc);
}

int todo_set_custom(char** argv, int argc, char* db) {
  if (prelude_custom(db) == -1) return -1;
  else return todo_set_meta(argv, argc);
}

int todo_show_meta(char** argv, int argc) {
  char* opt = calloc(3, sizeof(char));
  char* answer = calloc(200, sizeof(char));
  if (argc < 3) {
#ifdef Console
    printf("show what?\n\r");
#endif
  } else if (strcmp(argv[2], "syncfile") == 0) sprintf(opt, "15");  // NOLINT
  else if (strcmp(argv[2], "syncdir") == 0)    sprintf(opt, "0");   // NOLINT
  else if (strcmp(argv[2], "ending") == 0)     sprintf(opt, "13");  // NOLINT
  else if (strcmp(argv[2], "color") == 0)      sprintf(opt, "21");  // NOLINT
    sprintf(queries[ind++], "SELECT option, text FROM OPTIONS");    // NOLINT
  retval = sqlite3_prepare_v2(handle, queries[ind - 1], -1, &stmt, 0);
  if (retval) {
    shitHappended();
    goto todo_show_meta_exit;
  }
  while (sqlite3_step(stmt) == SQLITE_ROW)
  if (strcmp((const char*)sqlite3_column_text(stmt, 0), opt) == 0) {
    printf(": %s", sqlite3_column_text(stmt, 1));
  }
  todo_show_meta_exit:
  free(opt);
  free(answer);
  return 0;
}
int todo_show(char** argv, int argc) {
  if (prelude() == -1) return -1;
  else return todo_show_meta(argv, argc);
}
int todo_show_custom(char** argv, int argc, char* db) {
  if (prelude_custom(db) == -1) return -1;
  else return todo_show_meta(argv, argc);
}

int todo_history_meta() {
  char* syncdir;
  char* cmd = calloc(200, sizeof(char));
  int git = 0, hg = 0, svn = 0;
  sprintf(queries[ind++], "SELECT option, text FROM OPTIONS"); //NOLINT
  retval = sqlite3_prepare_v2(handle, queries[ind - 1], -1, &stmt, 0);
  if (retval) {
#ifdef Console
    printf("Reading DB data Failed, running re-init\n\r");
#endif
    free(cmd);
    if (todo_initdb() == 0) {
#ifdef Console
      printf("Done\n\r");
#endif
    } else {
      shitHappended();
      return -1;
    }
    return todo_history_meta();
  }
  syncdir = calloc(200, sizeof(char));
  while (sqlite3_step(stmt) == SQLITE_ROW)
  if (strcmp((const char*)sqlite3_column_text(stmt, 0), "0") == 0)
    sprintf(syncdir, "%s", sqlite3_column_text(stmt, 1)); //NOLINT
  else if (strcmp((const char*)sqlite3_column_text(stmt, 0), "2") == 0)
    git = atoi((const char*)sqlite3_column_text(stmt, 1));
  else if (strcmp((const char*)sqlite3_column_text(stmt, 0), "3") == 0)
    hg = atoi((const char*)sqlite3_column_text(stmt, 1));
  else if (strcmp((const char*)sqlite3_column_text(stmt, 0), "4") == 0)
    svn = atoi((const char*)sqlite3_column_text(stmt, 1));
  if (git == 1 || hg == 1 || svn == 1) {
    putenv(home);
    if (git == 1)
      sprintf(cmd, "cd %s;git log", syncdir); //NOLINT
    else if (hg == 1)
      sprintf(cmd, "cd %s;hg log", syncdir);  //NOLINT
    else if (svn == 1)
      sprintf(cmd, "cd %s;svn log", syncdir); //NOLINT
    if (system(cmd) == -1) shitHappended();
  }
  free(syncdir);
  free(cmd);
  return 0;
}

int todo_history() {
  if (prelude() == -1) return -1;
  else return todo_history_meta();
}

int todo_history_custom(char* db) {
  if (prelude_custom(db) == -1) return -1;
  else return todo_history_meta();
}

int todo_sync_meta(char** argv) {
  char* filename;
  char* home;
  int git = 0, hg = 0, svn = 0, darcs = 0;
  int timeDB = 0;
  int i = 0;
  char line[150];
  char write = 1;
  char* token1;
  char* token2;
  const char* search = "|";
  char* syncdir;
  char* cmd = calloc(200, sizeof(char));
  home = calloc(200, sizeof(char));
  filename = calloc(200, sizeof(char));
  syncdir = calloc(200, sizeof(char));
  sprintf(queries[ind++], "SELECT option, text FROM OPTIONS"); //NOLINT
  retval = sqlite3_prepare_v2(handle, queries[ind - 1], -1, &stmt, 0);
  if (retval) {
#ifdef Console
    printf("Reading DB data Failed, running re-init\n\r");
#endif
    free (cmd);
    free (home);
    free (filename);
    free (syncdir);
    if (todo_initdb() == 0) {
#ifdef Console
      printf("Done\n\r");
#endif
    } else {
      shitHappended();
      return -1;
    }
    return todo_sync_meta(argv);
  }
  while (sqlite3_step(stmt) == SQLITE_ROW)
  if (strcmp((const char*)sqlite3_column_text(stmt, 0), "0") == 0)
    sprintf(syncdir, "%s", sqlite3_column_text(stmt, 1)); //NOLINT
  else if (strcmp((const char*)sqlite3_column_text(stmt, 0), "1") == 0)
    timeDB = atoi((const char*)sqlite3_column_text(stmt, 1));
  else if (strcmp((const char*)sqlite3_column_text(stmt, 0), "2") == 0)
    git = atoi((const char*)sqlite3_column_text(stmt, 1));
  else if (strcmp((const char*)sqlite3_column_text(stmt, 0), "3") == 0)
    hg = atoi((const char*)sqlite3_column_text(stmt, 1));
  else if (strcmp((const char*)sqlite3_column_text(stmt, 0), "4") == 0)
    svn = atoi((const char*)sqlite3_column_text(stmt, 1));
  else if (strcmp((const char*)sqlite3_column_text(stmt, 0), "5") == 0)
    darcs = atoi((const char*)sqlite3_column_text(stmt, 1));
  else if (strcmp((const char*)sqlite3_column_text(stmt, 0), "15") == 0)
    sprintf(filename, "%s/%s", syncdir, sqlite3_column_text(stmt, 1)); //NOLINT
  else if (strcmp((const char*)sqlite3_column_text(stmt, 0), "20") == 0)
    sprintf(home, "HOME=%s", sqlite3_column_text(stmt, 1)); //NOLINT
  if (git == 1 || hg == 1 || svn == 1 || darcs == 1) {
    putenv(home);
    if (git == 1)
      sprintf(cmd, "cd %s;git pull", syncdir);          //NOLINT
    else if (hg == 1)
      sprintf(cmd, "cd %s;hg pull --update", syncdir);  //NOLINT
    else if (svn == 1)
      sprintf(cmd, "cd %s;svn update", syncdir);        //NOLINT
    else if (darcs == 1)
      sprintf(cmd, "cd %s;darcs pull", syncdir);        //NOLINT
    if (system(cmd)) shitHappended();
  }
#ifdef Console
  printf("Sync file: %s\n\r", filename);
#endif
  f = fopen(filename, "a+");
  if (f == NULL) {
#ifdef Console
    printf("There is no such file and it's failed to create it\n\r");
#endif
    free (cmd);
    free (home);
    free (filename);
    free (syncdir);
    return -1;
  }
  while (fgets(line, 150, f)) {
    if (i == 0) {
      timefile = atoi(line);
#ifdef Console
      printf("Timefile: %s\n\r", ctime(&timefile));
#endif
      if (timeDB > (int)timefile) break;
      else if (timeDB == (int)timefile) {
#ifdef Console
        printf("Everything is up to date\n\r");
#endif
        fclose (f);
        free (cmd);
        free (home);
        free (filename);
        free (syncdir);
        return 0;
      } else write = 0;
    } else {
      if (i == 1) {
        queries[ind++] = "DELETE FROM TODO";
        retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
        if (retval) {
#ifdef Console
          printf("Reading DB data Failed, running re-init\n\r");
#endif
          fclose (f);
          free (cmd);
          free (home);
          free (filename);
          free (syncdir);
          if (todo_initdb() == 0) {
#ifdef Console
            printf("Done\n\r");
#endif
          } else {
            shitHappended();
            return -1;
          }
          return todo_sync_meta(argv);
        }
      } else {
        token1 = strtok(line, search);
        token2 = strtok(NULL, search);
        if (token1[1] == '-') token1 += 3;
        rtrim(token2);
        sprintf(queries[ind++], "INSERT INTO TODO VALUES(%s,'%s', 0)", token1, token2); //NOLINT
        retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
        if (retval) {
#ifdef Console
          printf("Task were not added! (shit happens)\n\r");
#endif
        }
      }
    }
    i++;
  }
  fclose(f);
  if (write) {
    time_t now = time(0);
    f = fopen(filename, "w+");
    rewind(f);
    queries[ind++] = "SELECT id, text from TODO";
    retval = sqlite3_prepare_v2(handle, queries[ind - 1], -1, &stmt, 0);
    fprintf(f, "%d\n", (int)now);
    fprintf(f, "\n");
    timeUpdate(now);
    while (sqlite3_step(stmt) == SQLITE_ROW)
      fprintf(f, " - %s|%s\n"
      , sqlite3_column_text(stmt, 0)
      , sqlite3_column_text(stmt, 1));
    fclose(f);
    if (git == 1 || hg == 1 || svn == 1 || darcs == 1) {
      putenv(home);
      if (git == 1)
        sprintf(cmd, "cd %s;git commit -am \"TODO LIST UPDATE\";git push", syncdir);      //NOLINT
      else if (hg == 1)
        sprintf(cmd, "cd %s;hg commit -m \"TODO LIST UPDATE\";hg push", syncdir);         //NOLINT
      else if (svn == 1)
        sprintf(cmd, "cd %s;svn commit  -m \"TODO LIST UPDATE\"", syncdir);               //NOLINT
      else if (darcs == 1)
        sprintf(cmd, "cd %s;darcs commit -m \"TODO LIST UPDATE\"; darcs push", syncdir);  //NOLINT
      if (system(cmd) == -1) shitHappended();
    }
#ifdef Console
    printf("synchronization complete, syncfile updated\n\r");
#endif
  } else {
    timeUpdate(timefile);
#ifdef Console
    printf("synchronization complete, local database updated\n\r");
#endif
  }
  free (cmd);
  free (home);
  free(syncdir);
  free(filename);
  return 0;
}

int todo_sync(char** argv) {
  if (prelude() == -1) return -1;
  return todo_sync_meta(argv);
}

int todo_sync_custom(char** argv, char* db) {
  if (prelude_custom(db) == -1) return -1;
  return todo_sync_meta(argv);
}

void todo_edit_meta(char** argv, int argc) {
  char* text = calloc(200, sizeof(char));
  for (int argi = 3; argi < argc; ++argi) {
    strcat(text, argv[argi]); //NOLINT
    strcat(text, " ");        //NOLINT
  }
  sprintf(queries[ind++], "UPDATE TODO SET text='%s' WHERE id = %s", text, argv[2]); //NOLINT
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  if (retval) {
#ifdef Console
    printf("Task were not edited! (shit happens)\n\r");
#endif
  }
  free(text);
  timeUpdate(time(0));
}

void todo_edit(char** argv, int argc) {
  if (prelude() != -1) todo_edit_meta(argv, argc);
}

void todo_edit_custom(char** argv, int argc, char* db) {
  if (prelude_custom(db) != -1) todo_edit_meta(argv, argc);
}

void todo_swap_meta(char** argv) {
  int val1 = atoi(argv[2]);
  int val2 = atoi(argv[3]);
  sprintf(queries[ind++], "UPDATE TODO SET id=%d WHERE id = %d", 9999, val1); //NOLINT
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  if (retval) {
#ifdef Console
    printf("Swap failed! (shit happens)\n\r");
#endif
  }
  sprintf(queries[ind++], "UPDATE TODO SET id=%d WHERE id = %d", val1, val2); //NOLINT
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  if (retval) {
#ifdef Console
    printf("Swap failed! (shit happens)\n\r");
#endif
  }
  sprintf(queries[ind++], "UPDATE TODO SET id=%d WHERE id = %d", val2, 9999); //NOLINT
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  if (retval) {
#ifdef Console
    printf("Swap failed! (shit happens)\n\r");
#endif
  }
  timeUpdate(time(0));
}

void todo_swap(char** argv) {
  if (prelude() != -1) todo_swap_meta(argv);
}

void todo_swap_custom(char** argv, char* db) {
  if (prelude_custom(db) != -1) todo_swap_meta(argv);
}

void todo_reindex() {
  if (prelude() != -1) {
    sql("UPDATE TODO SET id = id + 1000000000");
    sql("UPDATE TODO SET id = rowid");
    timeUpdate(time(0));
  }
}

void todo_mv_meta(char** argv) {
  sprintf(queries[ind++], "UPDATE TODO SET id = %s WHERE id = %s", argv[3], argv[2]); //NOLINT
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  timeUpdate(time(0));
}

void todo_mv(char** argv) {
  if (prelude() != -1) todo_mv_meta(argv);
}

void todo_mv_custom(char** argv, char* db) {
  if (prelude_custom(db) != -1) todo_mv_meta(argv);
}

void todo_clean_meta() {
  queries[ind++] = "DELETE FROM TODO";
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  timeUpdate(time(0));
}

void todo_clean() {
  if (prelude() != -1) todo_clean_meta();
}

void todo_clean_custom(char* db) {
  if (prelude_custom(db) != -1) todo_clean_meta();
}

void todo_rm_meta(int argc, char** argv) {
  if (strstr(argv[2], ",") != NULL) {
    sprintf(queries[ind++], "DELETE FROM TODO WHERE id IN (%s)", argv[2]);  //NOLINT
  } else {
    for (int x = 2; x < argc; ++x) {
      sprintf(queries[ind++], "DELETE FROM TODO WHERE id = %s", argv[x]);   //NOLINT
    }
  }
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  timeUpdate(time(0));
}

void todo_rm(int argc, char** argv) {
  if (prelude() != -1) todo_rm_meta(argc, argv);
}

void todo_rm_custom(int argc, char** argv, char* db) {
  if (prelude_custom(db) != -1) todo_rm_meta(argc, argv);
}

char** todo_read_meta(int list, int parcount) {
  char* lineborder1;
  char* spaces1;
  char* spaces2;
  int maxl2 = 0, maxl1 = 0;
  int i, maxi1, maxi2;
  out = malloc(sizeof(char*) * 50);
  for (x = 0; x < 100; ++x)
    out[x] = malloc(sizeof(char) * 512);
  char* colorscheme;
  char* lineborder2;
  ///<Summary>
  ///Get color scheme (For linux only)
  ///</Summary>
  sprintf(queries[ind++], "SELECT option, text FROM OPTIONS WHERE option = 21"); //NOLINT
  retval = sqlite3_prepare_v2(handle, queries[ind - 1], -1, &stmt, 0);
  if (retval) {
#ifdef Console
    printf("Failed to get color scheme\n\r");
#endif
  }
  while (sqlite3_step(stmt) == SQLITE_ROW)
  if (strcmp((const char*)sqlite3_column_text(stmt, 0), "21") == 0) {
    colorscheme = calloc(50, sizeof(char));
    if (strcmp((const char*)sqlite3_column_text(stmt, 1), "black") == 0)
      sprintf(colorscheme, "%c[%d;%d;%dm", 0x1B, 1, 6, 66);     //NOLINT
    else if (strcmp((const char*)sqlite3_column_text(stmt, 1), "blink") == 0)
      sprintf(colorscheme, "%c[%d;%d;%dm", 0x1B, 1, 50, 5);     //NOLINT
    else if (strcmp((const char*)sqlite3_column_text(stmt, 1), "green") == 0)
      sprintf(colorscheme, "%c[%d;%d;%dm", 0x1B, 1, 68, 32);    //NOLINT
    else if (strcmp((const char*)sqlite3_column_text(stmt, 1), "pink") == 0)
      sprintf(colorscheme, "%c[%d;%d;%dm", 0x1B, 1, 35, 2);     //NOLINT
    else if (strcmp((const char*)sqlite3_column_text(stmt, 1), "red") == 0)
      sprintf(colorscheme, "%c[%d;%d;%dm", 0x1B, 1, 37, 41);    //NOLINT
    else sprintf(colorscheme, "%c[%d;%d;%dm", 0x1B, 1, 37, 41); //NOLINT
    break;
  }
  if (parcount > 0)
    sprintf(queries[ind++], "SELECT COALESCE(MAX(id),0) FROM TODO WHERE list = %d", list); //NOLINT
  else queries[ind++] = "SELECT COALESCE(MAX(id),0) from TODO";
  retval = sqlite3_prepare_v2(handle, queries[ind - 1], -1, &stmt, 0);
  if (retval) {
#ifdef Console
    printf("Reading DB data Failed, running re-init\n\r");
#endif
    if (todo_initdb() == 0) {
#ifdef Console
      printf("Done\n\r");
#endif
    } else {
      shitHappended();
      return NULL;
    }
    return todo_read_meta(list, parcount);
  }
  while (sqlite3_step(stmt) == SQLITE_ROW)
    maxl1 = strlen((const char*)sqlite3_column_text(stmt, 0));
  if (parcount > 0)
    sprintf(queries[ind++], "SELECT COALESCE(MAX(LENGTH(text)),0) FROM TODO WHERE list = %d", list); //NOLINT
  else queries[ind++] = "SELECT COALESCE(MAX(LENGTH(text)),0) from TODO";
  retval = sqlite3_prepare_v2(handle, queries[ind - 1], -1, &stmt, 0);
  if (retval) {
#ifdef Console
    printf("Reading DB data Failed, running re-init\n\r");
#endif
    if (todo_initdb() == 0) {
#ifdef Console
      printf("Done\n\r");
#endif
    } else {
      shitHappended();
      return NULL;
    }
    return todo_read_meta(list, parcount);
  }
  while (sqlite3_step(stmt) == SQLITE_ROW)
    maxl2 = atoi((const char*)sqlite3_column_text(stmt, 0));
  if (parcount > 0)
    sprintf(queries[ind++], "SELECT id, text, LENGTH(text) FROM TODO WHERE list = %d", list); //NOLINT
  else queries[ind++] = "SELECT id, text, LENGTH(text) from TODO";
  retval = sqlite3_prepare_v2(handle, queries[ind - 1], -1, &stmt, 0);
  if (retval) {
#ifdef Console
    printf("Reading DB data Failed, running re-init\n\r");
#endif
    if (todo_initdb() == 0) {
#ifdef Console
      printf("Done\n\r");
#endif
    } else {
      shitHappended();
      return NULL;
    }
    return todo_read_meta(list, parcount);
  }
  lineborder1 = calloc(512, sizeof(char));
  lineborder2 = calloc(512, sizeof(char));
  spaces1 = calloc(200, sizeof(char));
  spaces2 = calloc(200, sizeof(char));
  for (i = 0; i < ((maxl2 + maxl1) + 5); ++i)
  if (i == 2 + maxl1) {
    strcat(lineborder1, "╤"); //NOLINT
    strcat(lineborder2, "╧"); //NOLINT
  } else {
    strcat(lineborder1, "═"); //NOLINT
    strcat(lineborder2, "═"); //NOLINT
  }
  sprintf(out[1], "%s", colorscheme); //NOLINT
  sprintf(out[0], "%s", lineborder1); //NOLINT
  x = 4;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    maxi1 = maxl1 - strlen((const char*)sqlite3_column_text(stmt, 0));
    maxi2 = maxl2 - atoi((const char*)sqlite3_column_text(stmt, 2));
    strcpy(spaces1, ""); //NOLINT
    strcpy(spaces2, ""); //NOLINT
    for (i = 0; i < maxi1; ++i)
      strcat(spaces1, " "); //NOLINT
    for (i = 0; i < maxi2; ++i)
      strcat(spaces2, " "); //NOLINT
    sprintf(out[x], " %s %s", //NOLINT
      sqlite3_column_text(stmt, 0)
      , spaces1);
    sprintf(out[x + 1], " %s %s" //NOLINT
      , sqlite3_column_text(stmt, 1)
      , spaces2);
    x += 2;
  }
  memcpy(out[3], &x, sizeof(int));    //NOLINT
  sprintf(out[2], "%s", colorscheme); //NOLINT
  sprintf(out[1], "%s", lineborder2); //NOLINT
  free(lineborder1);
  free(spaces1);
  free(spaces2);
  free(lineborder2);
  free(colorscheme);
  return out;
}

char** todo_read(int list, int parcount) {
  if (prelude() == -1) return NULL;
  return todo_read_meta(list, parcount);
}

char** todo_read_custom(int list, int parcount, char* db) {
  if (prelude_custom(db) == -1) return NULL;
  return todo_read_meta(list, parcount);
}

int todo_write_meta(char** argv, int argc, int list) {
  char first = 0;
  int last = 0;
  int argi;
  char* text;
  int useending = 0;
  unsigned int limit = 150;
  ///<Summary>
  ///Getting options from local database
  ///<Summary>
  sprintf(queries[ind++], "SELECT option, text FROM OPTIONS WHERE option = 12 or option = 13"); //NOLINT
  retval = sqlite3_prepare_v2(handle, queries[ind - 1], -1, &stmt, 0);
  if (retval) {
#ifdef Console
    printf("Reading DB data Failed, running re-init\n\r");
#endif
    if (todo_initdb() == 0) {
#ifdef Console
      printf("Done\n\r");
#endif
    } else {
      shitHappended();
      return -1;
    }
    return todo_write_meta(argv, argc, list);
  }
  char* ending = calloc(200, sizeof(char));
  while (sqlite3_step(stmt) == SQLITE_ROW)
  if (strcmp((const char*)sqlite3_column_text(stmt, 0), "13") == 0) {
    sprintf(ending, "%s", sqlite3_column_text(stmt, 1)); //NOLINT
  } else if (strcmp((const char*)sqlite3_column_text(stmt, 0), "12") == 0) {
    useending = atoi((const char*)sqlite3_column_text(stmt, 1));
  }
  ///<Summary>
  ///Writing to local database
  ///<Summary>
  sprintf(queries[ind++], "SELECT COALESCE(MAX(id),0) FROM TODO"); //NOLINT
  retval = sqlite3_prepare_v2(handle, queries[ind - 1], -1, &stmt, 0);
  if (retval) {
#ifdef Console
    printf("Reading DB data Failed, running re-init\n\r");
#endif
    free (ending);
    if (todo_initdb() == 0) {
#ifdef Console
      printf("Done\n\r");
#endif
    } else {
      shitHappended();
      return -1;
    }
    return todo_write_meta(argv, argc, list);
  }
  while (sqlite3_step(stmt) == SQLITE_ROW)
    last = atoi((const char*)sqlite3_column_text(stmt, 0));
  text = calloc(limit + 1, sizeof(char));
  if (useending == 1)
    limit = limit - strlen(ending);
  for (argi = 1; argi < argc; ++argi) {
    if (strlen(text) + strlen(argv[argi]) > limit) {
#ifdef Console
      printf("Part of message is out of limit: %d\n\r", limit);
#endif
      strcat(text, " ");        //NOLINT
      break;
    }
    else if ((strcmp(argv[argi], "--motivate") == 0))
      useending = 1;
    else if ((strcmp(argv[argi], "--first") == 0) || (strcmp(argv[argi], "-1") == 0))
      first = 1;
    else {
      strcat(text, argv[argi]); //NOLINT
      strcat(text, " ");        //NOLINT
    }
  }
  if (useending == 1) {
    unsigned long int t_len = strlen(text);
    for (int i = 0; i < t_len; ++i) {
      text[i] = toupper(text[i]);
    }
    strcat(text, ending); //NOLINT
  }
  if (first == 1) {
    sql("UPDATE TODO SET id = id + 1000000000");
    sql("UPDATE TODO SET id = id - (1000000000 - 1)");
    sprintf(queries[ind++], "INSERT INTO TODO VALUES(0,'%s',%d)", text, list); //NOLINT
  } else
    sprintf(queries[ind++], "INSERT INTO TODO VALUES(%d,'%s', %d)", last + 1, text, list); //NOLINT
  retval = sqlite3_exec(handle, queries[ind - 1], 0, 0, 0);
  if (retval) {
#ifdef Console
    printf("Task were not added! (shit happens)\n\r");
#endif
  }
  free(text);
  free(ending);
  timeUpdate(time(0));
  return 0;
}

int todo_write(char** argv, int argc, int list) {
  if (prelude() == -1) return -1;
  return todo_write_meta(argv, argc, list);
}

int todo_write_custom(char** argv, int argc, int list, char* db) {
  if (prelude_custom(db) == -1) return -1;
  return todo_write_meta(argv, argc, list);
}
