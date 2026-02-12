 # include <stdio.h>
 # include <unistd.h>
 # include <sys/wait.h>
 #include <stdlib.h>

 int main () {
for(int i = 0; i < 3; i++){
 pid_t pid = fork ();

 if ( pid < 0) { // Erro
    fprintf (stderr , " Fork falhou !\n");
    return 1;
 } else if (pid == 0) { // Processo Filho
    printf ("Eu sou o filho ! Meu PID e %d, meu pai e %d.\n", getpid () , getppid ());
 } else { // Processo Pai
    printf ("Eu sou o pai! Meu PID e %d, criei o filho %d.\n", getpid () , pid);
    exit(0);
    
 }
}
 return 0;
}