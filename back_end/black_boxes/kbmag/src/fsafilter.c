/* fsafilter.c 27/10/94.
 * This simply reads an fsa and prints it again.
 * Used either for testing, or for converting table format.
 * Can either use stdin/stdout or files.
 *
 * SYNOPSIS:  fsafilter [-ip d/s[dr]] [-op d/s] [-silent] [-v] [filename]
 *
 * Input is from filename (or stdin if not specified) ,
 *   which should contain an fsa.
 * Output is to filename.new, or stdout.
 *
 * OPTIONS:
 * -ip d/s[dr]  input in dense or sparse format - dense is default
 * -op d/s      output in dense or sparse format - default is as in current
 *               value of table->printing_format, in the fsa.
 * -v           verbose
 * -silent      no diagnostics
 *
 */

#include <stdio.h>
#include "defs.h"
#include "fsa.h"
#include "definitions.h"

FILE *rfile,
     *wfile,
     *fopen();

void  badusage();

/* Functions defined in other files used in this file */
void  fsa_read();
void  fsa_print();
void  fsa_clear();

main(argc, argv)
        int             argc;
        char           *argv[];
{ int arg;
  fsa testfsa;
  char inf[100],outf[100],fsaname[100];
  storage_type ip_store = DENSE;
  int dr = 0;
  boolean op_format_set = FALSE;
  storage_type op_format = DENSE;

  setbuf(stdout,(char*)0);
  setbuf(stderr,(char*)0);

  inf[0] = '\0';
  arg = 1;
  while (argc > arg) {
    if (strcmp(argv[arg],"-ip")==0) {
      arg++;
      if (arg >= argc)
        badusage();
      if (strcmp(argv[arg],"d")==0)
        ip_store = DENSE;
      else if (argv[arg][0] == 's') {
        ip_store = SPARSE;
        if (strlen(argv[arg]) > 1)
          dr = atoi(argv[arg]+1);
      }
      else
        badusage();
    }
    else if (strcmp(argv[arg],"-op")==0) {
      arg++;
      op_format_set=TRUE;
      if (arg >= argc)
        badusage();
      if (strcmp(argv[arg],"d")==0)
        op_format = DENSE;
      else if (strcmp(argv[arg],"s")==0)
        op_format = SPARSE;
      else
        badusage();
    }
    else if (strcmp(argv[arg],"-silent")==0)
      print_level = 0;
    else if (strcmp(argv[arg],"-v")==0)
      print_level = 2;
    else if (strcmp(argv[arg],"-vv")==0)
      print_level = 3;
    else {
       if (argv[arg][0] == '-')
         badusage();
       if (strcmp(inf,""))
         badusage();
       strcpy(inf,argv[arg]);
    }
    arg++;
  }

  if (strlen(inf)!=0) {
    strcpy(outf,inf);
    strcat(outf,".new");

    if ((rfile = fopen(inf,"r")) == 0) {
      fprintf(stderr,"Cannot open file %s.\n",inf);
      exit(1);
    }
  }
  else
    rfile = stdin;

  fsa_read(rfile,&testfsa,ip_store,dr,0,TRUE,fsaname);

  if (strlen(inf)!=0)
    fclose(rfile);

  if (op_format_set)
    testfsa.table->printing_format = op_format;

  if (strlen(inf)!=0)
    wfile = fopen(outf,"w");
  else
    wfile = stdout;

  fsa_print(wfile,&testfsa,fsaname);

  if (strlen(inf)!=0)
    fclose(wfile);
  if (wfile!=stdout && print_level>0)
    printf("#fsa with %d states output.\n",testfsa.states->size);

  fsa_clear(&testfsa);
  exit(0);
}
 
void
badusage()
{
    fprintf(stderr,
      "Usage: fsafilter [-ip d/s[dr]] [-op d/s] [-silent] [-v] [filename]\n");
    exit(1);
}
