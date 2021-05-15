#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#define hentry_err_handle(_exp, _msg) \
  do { \
    if( _exp ) { \
      fprintf(stderr, "%s\n", _msg); \
      exit(EXIT_FAILURE); \
    } \
  } while(0)

static const char* d_entry =
  "# Host addresses\n"
  "127.0.0.1  localhost\n"
  "127.0.1.1  reshifr\n"
  "::1        localhost ip6-localhost ip6-loopback\n"
  "ff02::1    ip6-allnodes\n"
  "ff02::2    ip6-allrouters\n\n";

static inline int write_hosts(void* misc, int argc, char** argv, char** field) {
  fprintf(misc, "%s  %s\n", argv[0], argv[1]);
  return 0;
}

int main(int argc, char** argv) {
  hentry_err_handle(argc==1, "hentry SQLITE_DATABASE_FILE");
  FILE* buf;
  hentry_err_handle((buf=fopen("/etc/hosts", "w"))==NULL, "Update Failed");
  sqlite3* db;
  char* sql = "SELECT hosts, domain FROM entry ORDER BY domain ASC";
  char* msg;
  int ret = sqlite3_open(argv[1], &db);
  hentry_err_handle(ret!=SQLITE_OK, "DB Failed");
  fprintf(buf, "%s%s", d_entry, "# HEntry\n");
  sqlite3_exec(db, sql, &write_hosts, buf, &msg);
  fprintf(buf, "\n\n");
  sqlite3_close(db);
  fclose(buf);
  printf("Success...\n");
  return EXIT_SUCCESS;
}
