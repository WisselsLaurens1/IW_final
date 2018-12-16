#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "util.h"

int e;

/*
 * Pre-process one given line of the input file by writing the desired output
 * to stdout (use printf), and any errors to stderr (use pr_error).
 *
 * TODO Current dummy implementation only handles whitespace.
 * Your job is to implement the desired preprocessing functionality, as
 * described in the assignment PDF.
 *
 */

int blockComments = 0;


void process_line(char *line, struct alias_list *list){


  char *lineCopie = malloc(strlen(line));
  strcpy(lineCopie,line);
  char *invalid = strtok(lineCopie, "#");
  char *invalid2 = strtok(invalid, " \t");
  char *token = strtok(line, " \t");

  while (token){



    // 1. single line commentaar
      if(token[0] == '/' && token[1] == '/'){
        printf("\n");
        break;
      }


    // # handeling

      if(strstr(line,"#") != NULL){

        //1. warning handeling

        if(strcmp(token,"#warning")==0){
            if(strncmp(token,"#warning",strlen("#warning"))==0){
            token = strtok(NULL, " \t");
            pr_error("warning: ");
              while(token){
                if (token[strlen(token)-1] == '\n') {
                  pr_error("%s",token);
                }else{
                  pr_error("%s ",token);
                }
                token = strtok(NULL, " \t");
              }
            break;
          }

        //2. define handeling

        }if(strcmp(token,"#define")==0){
          if(strncmp(token,"#define",strlen("#define"))==0){
              token = strtok(NULL, " \t");
              if(alias_list_lookup(list,token) == NULL){
                alias_list_append(list,token,token+strlen(token)+1);
                break;
              }else{
                if(strcmp(alias_list_lookup(list,token), token+strlen(token)+1)==0){
                  pr_error("warning: '%s", token);
                  pr_error("' redefined\n");
                  break;
                }else{
                  alias_list_remove(list,token);
                  alias_list_append(list,token,token+strlen(token)+1);
                  break;
                }
            }
          }

        //3. undefine handeling

        }if(strcmp(token,"#undef")==0){
          if(strncmp(token,"#undef",strlen("#undef"))==0){
              token = strtok(NULL, " \t");

              if(alias_list_lookup(list,token) != NULL){
                alias_list_remove(list,token);
                break;
              }else{
                  break;
              }

            }

        //4. error handeling

      }if(strcmp(token,"#error")==0){
        if(strncmp(token,"#error",strlen("#error"))==0){
          if(e == 0){
            token = strtok(NULL, " \t");
            pr_error("error: ");
              while(token){
                if (token[strlen(token)-1] == '\n') {
                  pr_error("%s",token);
                }else{
                  pr_error("%s ",token);
                }
                token = strtok(NULL, " \t");
              }
            break;
          }else{
            exit(0);
          }
        }

        //5. invalid handeling

      }else{
          pr_error("error: invalid preprocessing directive: #%s", invalid);
          pr_error("\n");
          break;
        }
      }

      // replacing keys

      if(alias_list_lookup(list,token) != NULL){
        char *value = alias_list_lookup(list,token);
        char *newValue = (char*) malloc(sizeof(value)-strlen("\n"));
        memcpy(newValue,value,strlen(value)-strlen("\n"));
        printf("%s",newValue);
        printf("%s"," ");
        token = strtok(NULL, " \t");
      }

    // multi-comments

    if(token[0] =='/' && token[1] == '*'){
      blockComments +=1;
    }

    if(token[0] == '*' && token[1] == '/'){
      token += sizeof("*/")-1;
      blockComments +=1;
    }

    // normal printing

    if(blockComments%2 ==0){
      if (token[strlen(token)-1] == '\n') {
      printf("%s", token);

    }else{
      printf("%s ", token);
    }
      token = strtok(NULL, " \t");
    }else{
      token = strtok(NULL, " \t");
    }

  }
}



/*
 * Process a single-char "-C" command line option.
 *
 * TODO add your own single-char options here
 *  (e.g., "-v" for printing author and version information)
 */
void option(char c)
{
    switch (c)
    {
        case 'h':
            printf("cpp-iw: The awesome IW C preprocessor\n");
            printf("\nUsage: cpp-iw [-h -e] file.c\n");
            printf("\nRecognized options:\n");
            printf("-h\tdisplay this help message\n");
            printf("-e\tterminate preprocessing on #error\n");
            exit(0);
            break;
        case 'e':
          e = 1;

        default:
            pr_error("Unrecognized option '%c'\n", c);
            exit(1);
    }
}

int main(int argc, char *argv[])
{
  struct alias_list *list = alias_list_create();


    int i;
    char *s;

    /* Process command line options */
	for (i = 1; i < argc && *argv[i] == '-'; i++)
        option(*(argv[i]+1));

    if (argv[i] == NULL)
    {
        pr_error("No input C file provided\n");
        return 1;
    }

    /* Open the input file */
    if ( infile_open(argv[i]) )
    {
        pr_error("Could not open input file '%s'\n", argv[i]);
        return 1;
    }

    /* First pass the list test suite (make sure to add your own tests!) */
	if (alias_list_test() == 0)
        return 1;

    /* Do the actual preprocessing work line by line */

    while ((s = infile_getline()))
    {
        process_line(s,list);
    }

    infile_close();
	return 0;
}
