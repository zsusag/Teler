#include <argp.h>  // for argp_parse()
#include <stdbool.h> // for while(true)
#include <stdio.h>   // for puts()
#include <stdlib.h>  // for exit()
#include <string.h>  // for strncmp()
#include <time.h>

#include "teler.h"
#include "util.h"
const char *argp_program_version = "teler 0.0";

const char *argp_program_bug_address = "<lukerhad@grinnell.edu> and/or <susagzac@grinnell.edu>";

static char doc[] = "teler: a streamlined version control system";

static struct argp_option pull_options[] =
  {
    {"commit",'c', "commit",0, "Pull a specific commit"},
    {0}
  };

static struct argp_option push_options[] =
  {
    {0}
  };

struct arguments {
  char *args[2];
  char *repo;
  char *commit;
  char *files; // TODO: array
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;
  switch (key) {
  case 'x':
    arguments->files = arg;
    puts(arguments->files);
    break;
  case 'c':
    arguments->commit = arg;
    break;
  case 'f': case 't':
    arguments->repo = arg;
    puts(arguments->repo);
    break;
  case ARGP_KEY_ARG:
    puts("too many args");
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp pull_argp = { pull_options, parse_opt, 0, doc };
static struct argp push_argp = { push_options, parse_opt, 0, doc };

void print_basic_usage();

int main (int argc, char **argv) {
  struct arguments arguments;
  arguments.repo = "-";
  arguments.files = "-";

  while (true) {
    if (argv[1] == NULL) {
      print_basic_usage();
      break;
    } else if (!strncmp(argv[1], "pull", 4)) {
      argp_parse(&pull_argp, argc-1, argv+1, 0, 0, &arguments);
      pull(arguments.commit);
      break;
    } else if (!strncmp(argv[1], "push", 4)) {
      argp_parse(&push_argp, argc-1, argv+1, 0, 0, &arguments);
      push();
      break;
    } else if (!strncmp(argv[1], "init", 4)) {
      teler_init();
      break;
    } else if (!strncmp(argv[1], "history", 7)) {
      history();
      break;
    } else {
      printf("%s is not a command.\n", argv[1]);
      print_basic_usage();
      return EXIT_FAILURE;
      break;
    }
  }  
  /* TODO: Use the information contained in "arguments" to send commands to
  backend. */
  return EXIT_SUCCESS;
}

void print_basic_usage() {
  puts("Usage: teler <command>");
  puts("teler: a streamlined version control system");
  puts("");
  puts("Commands:");
  puts("\tinit\tInitialize a teler repository");
  puts("\tpull\tPull all your data from your default repo");
  puts("\t\t-c\tPull a specific commit");
  puts("\tpush\tPush all your data of your default repo");
  puts("\thistory\tPrint out the full commit history");
}
