#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

/* Print the prompt ">>> " and read a line of characters
   from stdin. */
int getcmd(char *buf, int nbuf) {
  write(2, ">>> ", 4);
  
  for (int count=0; count < nbuf-1; count++){
    int read_val = read(0, &buf[count], 1); 
    if (read_val == 1){
      if (buf[count] == '\n'){
        buf[count] = '\0';
        return count;
      }}
    
    else if (read_val == -1){
        return -1;
    }

    else if (read_val == 0){
        buf[count] = '\0';
        return count;
    }
  } 
    buf[nbuf-1] = '\0';
    return nbuf-1;
}

/*
  A recursive function which parses the command
  at *buf and executes it.
*/
__attribute__((noreturn))
void run_command(char *buf, int nbuf, int *pcp) {
  
  /* Useful data structures and flags. */
  char *arguments[10];
  int numargs = 0;
  /* Flags to mark word start/end */
  int ws = 1;

  /* Flags to mark redirection direction */
  int redirection_left = 0;
  int redirection_right = 0;

  /* File names supplied in the command */
  char *file_name_l = 0;
  char *file_name_r = 0;

  int p[2];
  int pipe_cmd = 0;

  /* Flag to mark sequence command */
  int sequence_cmd = 0;

  int i = 0;
  /* Parse the command character by character. */
   for (; i < nbuf && buf[i] != '\0'; i++) {

    /* Parse the current character and set-up various flags:
       sequence_cmd, redirection, pipe_cmd and similar. */

    if (buf[i] == '<'){
      redirection_left = 1;
      if (!ws) {
        buf[i] = '\0';
        numargs++;}
      ws=1;
    }
    else if (buf[i] == '>'){
      redirection_right = 1;
      if (!ws) {
        buf[i] = '\0';
        numargs++;}
      ws=1;
    }
    else if (buf[i] == '|'){
      pipe_cmd = 1;
      if (!ws) {
        buf[i] = '\0';
        numargs++;}
      ws=1;
      break;
    }
    else if (buf[i] == ';'){
      sequence_cmd = 1;
      if (!ws) {
        buf[i] = '\0';
        numargs++;}
      ws=1;
      break;
    }  
    else if (buf[i] != ' ') {
      if (ws) {
        arguments[numargs] = &buf[i];
        ws = 0;
      }
      }    
    else {
      if (!ws) {
        buf[i] = '\0';
        numargs++;
        ws = 1;
      }
    }

    if (!(redirection_left || redirection_right)) {
      /* No redirection, continue parsing command. */
    
      continue;  
    } else {
      /* Redirection command. Capture the file names. */

      if (redirection_left){
        for (int j =i+1; j < nbuf; j++){
          if (buf[j] == ' '){
            continue;
          }
          else{
            file_name_l = &buf[j];
            for (int k=j; k < nbuf; k++){
              if (buf[k] == '\n' || buf[k] == ' ' || buf[k] == '>' || buf[k] == '<' || buf[k] == '|' || buf [k] == ';'){
                buf[k] = '\0';
                i=k-1;
                break;
              }
            }
            break;
            }
          }
      }

      if (redirection_right){
        for (int j=i+1; j < nbuf; j++){
          if (buf[j] == ' '){
            continue;
          }
          else{
            file_name_r = &buf[j];
            for (int k=j; k < nbuf; k++){
              if (buf[k] == '\n' || buf[k] == ' ' || buf[k] == '>' || buf[k] == '<' || buf[k] == '|' || buf [k] == ';'){
                buf[k] = '\0';
                i=k-1;
                break;
              }
            }
            break;
          }
        }
      }
    }}
    if (ws == 0){
      buf[i] = '\0';
      numargs++;
    }
    

  /*
    Sequence command. Continue this command in a new process.
    Wait for it to complete and execute the command following ';'.
  */
 
  if (sequence_cmd) {
    sequence_cmd = 0;
    if (fork() != 0) {
      wait(0)
      // Call run_command recursively
      run_command(&buf[i+1], nbuf-i-1, pcp);

    }
  }

  /*
    If this is a redirection command,
    tie the specified files to std in/out.
  */
  if (redirection_left) {
    // ##### Place your code here.
    int fd = open(file_name_l, O_RDONLY);
    close(0);
    dup(fd);
    close(fd);
  }
  if (redirection_right) {
    int fd = open(file_name_r, O_WRONLY | O_CREATE | O_TRUNC);
    close(1);
    dup(fd);
    close(fd);
  }

  /* Parsing done. Execute the command. */
  arguments[numargs] = 0;

  /*
    If this command is a CD command, write the arguments to the pcp pipe
    and exit with '2' to tell the parent process about this.
  */


  if (strcmp(arguments[0], "cd") == 0) 
    write(pcp[1], arguments[1], strlen(arguments[1])+1);
    exit(2);
  } else {
    /*
      Pipe command: fork twice. Execute the left hand side directly.
      Call run_command recursion for the right side of the pipe.
    */

    if (pipe_cmd) {
            pipe(p);

      if (fork()==0){
        close(1);
        dup(p[1]);
        close(p[0]);
        close(p[1]);
        exec(arguments[0], arguments);
      }

      if (fork()==0){
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);
        run_command(&buf[i+1], nbuf - i - 1, pcp);
      }

      close(p[0]);
      close(p[1]);
      wait(0);
      wait(0);

    } else 
      // Simple command; call exec()
      exec(arguments[0], arguments);
    }
  }
  exit(0);
}

int main(void) {

  static char buf[100];

  int pcp[2];
  pipe(pcp);


  /* Read and run input commands. */
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(fork() == 0)
      run_command(buf, 100, pcp);

    /*
      Check if run_command found this is
      a CD command and run it if required.
    */
   

    int child_status;
    wait(&child_status);
    if (child_status == 2){
      read(pcp[0], buf, sizeof(buf));
      chdir(buf);
    }
    
  }
  exit(0);
}
