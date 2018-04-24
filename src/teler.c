#include <argp.h>  // for argp_parse()
#include <stdbool.h> // for while(true)
#include <stdio.h>   // for puts()
#include <stdlib.h>  // for exit()
#include <string.h>  // for strncmp()

static struct option pull_options[] =
  {
    {"files", required_argument, 0, 'x'},
    {"from",  required_argument, 0, 'f'},
    {0,       0,                 0, 0}
  };

static struct option push_options[] =
  {
    {"files", required_argument, 0, 'x'},
    {"to",    required_argument, 0, 't'},
    {0,       0,                 0, 0}
  };

int main (int argc, char **argv) {
  while (true) {
    if (argv[1] == NULL) {
      puts("teler requires at least one command!");
      break;
    } else if (!strncmp(argv[1], "pull", 4)) {
      puts("Hello dev. Pull stuff goes here!");
      break;
    } else if (!strncmp(argv[1], "push", 4)) {
      puts("Hello dev. Push stuff goes here!");
      break;
    } else {
      printf("%s is not a command.\n", argv[1]);
      break;
    }

    /*     int opt = getopt_long(argc, argv, "sg", options, &option_index); */

    /*     /\* Detect the end of the options. *\/ */
    /*     if (opt == -1) */
    /*       break; */

    /*     switch (opt) { */
    /*       case 0: */
    /*         /\* If this option set a flag, do nothing else now. *\/ */
    /*         if (options[option_index].flag != 0) */
    /*           break; */
    /*         printf("option %s", options[option_index].name); */
    /*         if (optarg) */
    /*           printf(" with arg %s", optarg); */
    /*         printf("\n"); */
    /*         break; */

    /*       case 'g': */
    /*         puts("option -g\n"); */
    /*         break; */

    /*       case 's': */
    /*         puts ("option -s\n"); */
    /*         break; */

    /*       case '?': */
    /*         /\* getopt_long already printed an error message. *\/ */
    /*         break; */

    /*       default: */
    /* 	  perror("getopt: This should never happen..."); */
    /*         exit(EXIT_FAILURE); */
    /*       } */
    /*   } */

    /* /\* Print any remaining command line arguments (not options). *\/ */
    /* if (optind < argc) { */
    /*     printf ("non-option ARGV-elements: "); */
    /*     while (optind < argc) */
    /*       printf("%s ", argv[optind++]); */
    /*     putchar('\n'); */
  }
  exit(0);
}
