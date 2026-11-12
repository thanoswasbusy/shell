

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHELL_BUFFER_SIZE 1024
char *shell_read_line(void) {
  int bufsize = SHELL_BUFFER_SIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "shell: buffer allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    c = getchar();

    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }

    position++;

    if (position >= bufsize) {
      bufsize += SHELL_BUFFER_SIZE;
      buffer = realloc(buffer, bufsize);

      if (!buffer) {
        fprintf(stderr, "shell: buffer allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define SHELL_TOK_BUFSIZE 64
#define SHELL_TOK_DELIMITER " \t\r\n\a"
char **shell_split_line(char *line) {
  int bufsize = SHELL_BUFFER_SIZE;
  char **tokens = malloc(sizeof(char *) * bufsize);
  char *token;
  int position = 0;

  if (!tokens) {
    fprintf(stderr, "shell: buffer allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, SHELL_TOK_DELIMITER);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += SHELL_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char *));

      if (!tokens) {
        fprintf(stderr, "shell: buffer allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, SHELL_TOK_DELIMITER);
  }

  tokens[position] = NULL;
  return tokens;
}

#include <unistd.h>
int shell_launch(char **args) {
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("shell");
    }

    exit(EXIT_FAILURE);
  }

  if (pid < 0) {
    perror("shell");
  }

  do {
    wpid = waitpid(pid, &status, WUNTRACED);
  } while (WIFEXITED(status) && !WIFSIGNALED(status));

  return 1;
}

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);

char builtin_str[] = {"cd", "help", "exit"};

int (*builtin_func[])(char **) = {
    &shell_cd,
    &shell_help,
    &shell_exit,
};

int shell_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

int shell_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "shell: expected argument to \"cd\"\n");
  } else if (chdir(args[1]) != 0) {
    perror("shell");
  }
  return 1;
}

int shell_help(char **args) {
  int i;
  printf("Thanos Theodosiou's Shell\n");
  printf("The following commands are built-in: \n");

  for (int i = 0; i < shell_num_builtins(); i++) {
    printf("\t%s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int shell_execute(char **args) {
  int i;

  if (args[0] == NULL) {
    return 1;
  }

  for (int i = 0; i < shell_num_builtins(); i++) {
    if (strcmp(args[0], &builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return shell_launch(args);
}

int shell_exit(char **args) { return 0; }

void shell_loop(void) {
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = shell_read_line();
    args = shell_split_line(line);
    status = shell_execute(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv) {

  shell_loop();

  return EXIT_SUCCESS;
}
