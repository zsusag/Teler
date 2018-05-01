#include <argp.h>  // for argp_parse()
#include <stdbool.h> // for while(true)
#include <stdio.h>   // for puts()
#include <stdlib.h>  // for exit()
#include <string.h>  // for strncmp()
#include "teler.h"

const char *argp_program_version = "teler 0.0";

const char *argp_program_bug_address = "<lukerhad@grinnell.edu> and/or <susagzac@grinnell.edu>";

static char doc[] = "teler: a streamlined version control system";

static struct argp_option pull_options[] =
  {
    {"files", 'x', "files", 0, "Pull specific files"},
    {"from",  'f', "repo",  0, "Pull from a specific repo"},
    {0}
  };

static struct argp_option push_options[] =
  {
    {"files", 'x', "files", 0, "Push specific files"},
    {"to",    't', "repo",  0, "Push to a specific repo"},
    {0}
  };

struct arguments {
  char *args[2];
  char *repo;
  char *files; // TODO: array
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;
  switch (key) {
    case 'x':
      arguments->files = arg;
      puts(arguments->files);
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
      break;
    } else if (!strncmp(argv[1], "push", 4)) {
      argp_parse(&push_argp, argc-1, argv+1, 0, 0, &arguments);
      break;
    } else {
      printf("%s is not a command.\n", argv[1]);
      print_basic_usage();
      break;
    }
  }

  /* TODO: Use the information contained in "arguments" to send commands to
  backend. */
  teler_init();
  return EXIT_SUCCESS;
}

void print_basic_usage() {
  puts("Usage: teler <command>");
  puts("teler: a streamlined version control system");
  puts("");
  puts("Commands");
  puts("\tpull\tPull all your data from your default repo");
  puts("\t\t-f, --from=repo\t\tPull from a specific repo");
  puts("\t\t-x, --files=files\tPull specific files");
  puts("\tpush\tPush all your data of your default repo");
  puts("\t\t-t, --to=repo\t\tPush to a specific repo");
  puts("\t\t-x, --files=files\tPush specific files");
}
