#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>

#define SERVER_FIFO "/tmp/seqnum_sv"
#define KEY 1901042629
#define KEY2 344

#define MAX_CLIENTS 100


void execute_archServer(char *dirname, char *filename) {
    int pid = fork();
    if (pid == 0) { 
        char tar_path[] = "/bin/tar";
        char cvf_arg[] = "-cvf";
        char tarfile[1024];
        snprintf(tarfile, sizeof(tarfile), "%s", filename);
        char *tar_args[] = {tar_path, cvf_arg, tarfile, ".", NULL};

    
        int devnull = open("/dev/null", O_WRONLY);
        dup2(devnull, STDOUT_FILENO);
        dup2(devnull, STDERR_FILENO);
        close(devnull);

        execv(tar_path, tar_args);
        perror("exec failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) { 
        int status;
        waitpid(pid, &status, 0); 
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            struct stat file_stat;
            if (stat(filename, &file_stat) == -1) {
                perror("stat");
                char response[256];
                sprintf(response, "Archiving failed: Unable to determine file size\n");
                write(1, response, strlen(response));
            } else {
                char response[256];
                sprintf(response, "Archiving succeeded for %s, size: %ld bytes\n", filename, file_stat.st_size);
                write(1, response, strlen(response));
            }
        } else {
            char response[256];
            sprintf(response, "Archiving failed for %s\n", filename);
            write(1, response, strlen(response));
        }
    } else {
        perror("Failed to fork");
        char response[256];
        sprintf(response, "Failed to fork and archive %s\n", filename);
        write(1, response, strlen(response));
    }
}



void errExit(char * errMessage){
    perror(errMessage);
    exit(EXIT_FAILURE);
}

int main(int argc, char * argv[]){
    if(argc < 3)
        errExit("Unsufficient argc");

    int numberOfClients = atoi(argv[2]);

    char * dirName = argv[1];

    char log[100];

    char buffer[1024];

    char secondBuffer[1024];

    char pid[2048];

    int fdx[2];

    int child;
    
    int flag=getpid();

    int pid_array[numberOfClients];

    int current = 0;


    char server_fifo[100];

    struct sembuf sembuf_wait = {0, -1, 0};
    struct sembuf sembuf_signal = {0, 1, 0};

    mkdir(dirName, 0777);

    sprintf(server_fifo, "/tmp/%d", flag);
    
    mkfifo(server_fifo, 0666);

    int fd = open(server_fifo, 0666);

    printf("Server started pid %d\n", flag);

    printf("Server waiting for clients\n");

            while(1){
                     memset(buffer, 0, sizeof(buffer));
                    if (read(fd, buffer, sizeof(buffer)) > 0) {
                    sprintf(pid, "%s", buffer);
                    strtok(pid, " ");
                    int ppid = atoi(strtok(strcpy(buffer, &buffer[5]), " "));
                    char *connect = strtok(NULL, " ");

                    bool slotFound = false;
                    for (int i = 0; i < numberOfClients; ++i) {
                        if (pid_array[i] == 0) {
                            pid_array[i] = ppid;
                            current = i;
                            slotFound = true;
                            break;
                        }
                    }

                    if (!slotFound) {
                        printf("Connection request pid %d... Que FULL\n", ppid);
                    }    
                        
            child = fork();
            
            if(child == 0){

                int semid = semget(ppid, 1, 0666);
                if(semid < 0)
                    errExit("semget");
                int semid2 = semget(ppid + 1, 1, 0666);
                if(semid2 < 0)
                    errExit("semget");

                int client_fd;

                if(pid_array[current] == ppid){
                    if(mkfifo(pid, 0666) == -1)
                        errExit("mkfifo");
                    client_fd = open(pid, 0666);
                    if(client_fd < 0){
                        errExit("open");
                    }
                }                 
                if(strcmp(connect,"connect")!=0){
                    kill(ppid,SIGTERM);
                    continue;
                }
                if(semop(semid, &sembuf_signal, 1) < 0)
                    errExit("semop");

                if(pid_array[current] != ppid){
                    continue;
                }

                sprintf(log, "%s/%d", dirName, ppid);

                int xd = open(log, O_WRONLY | O_CREAT | O_APPEND, 0644);

                printf("Client pid %d connected as client%d\n", ppid, current + 1);

                while(1){  
                    if(semop(semid2, &sembuf_wait, 1) == -1)
                        errExit("semop");
                    memset(secondBuffer, 0, sizeof(secondBuffer));
                    if(read(client_fd, secondBuffer, sizeof(secondBuffer)) > 0){


                        write(xd, secondBuffer, strlen(secondBuffer));

                        write(xd, "\n", 1);

                        pipe(fdx);

                        int new_child = fork();

                        if(new_child == 0){

                            dup2(fdx[1], STDOUT_FILENO);
                        
                               if (strncmp(secondBuffer, "list", 4) == 0) {
                                    DIR *d;
                                    struct dirent *dir;
                                    d = opendir(".");  
                                    if (d) {
                                        while ((dir = readdir(d)) != NULL) {
                                           
                                            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
                                                continue;
                                            }
                                            if (dir->d_type == DT_REG || dir->d_type == DT_DIR) {  
                                                printf("%s\n", dir->d_name);
                                            }
                                        }
                                        closedir(d); 
                                    } else {
                                        perror("Failed to open directory");
                                        exit(EXIT_FAILURE);
                                    }
                                }
                                else if(strncmp(secondBuffer, "readF", 5) == 0){
                                    int fd;
                                    char *filename;
                                    int targetLine=-1;
                                    char buffer[1024];
                                    int currentLine = 1;
                                    ssize_t nread;

                                    strtok(secondBuffer, " ");  
                                    filename = strtok(NULL, " ");  
                                    char *lineStr = strtok(NULL, " ");  
                                    if (!filename ) {
                                        write(STDOUT_FILENO, "Error: Missing filename or line number\n", 40);
                                        exit(EXIT_FAILURE);
                                    }
                                    if (lineStr != NULL) {
                                        targetLine = atoi(lineStr); 
                                    }

                                    fd = open(filename, O_RDONLY); 
                                    if (fd == -1) {
                                        perror("File cant open");
                                        exit(EXIT_FAILURE);
                                    }

                                    while ((nread = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
                                        buffer[nread] = '\0';  
                                    char *ptr = buffer;
                                    if (targetLine == -1) { 
                                        write(STDOUT_FILENO, buffer, nread);
                                    } else {
                                        while (*ptr) {
                                            if (currentLine == targetLine) {
                                                while (*ptr && *ptr != '\n') {
                                                    write(STDOUT_FILENO, ptr, 1);  
                                                    ptr++;
                                                }
                                                write(STDOUT_FILENO, "\n", 1);  
                                                close(fd);  
                                                return 0;  
                                            }
                                            if (*ptr == '\n') currentLine++;  
                                            ptr++;
                                        }
                                    }
                                    }



                                    close(fd);  
                                }

                                         else if (strncmp(secondBuffer, "archServer", 10) == 0) {
                                                strtok(secondBuffer, " ");
                                                char *filename = strtok(NULL, " ");
                                                if (filename != NULL) {
                                                    if (strstr(filename, ".tar") == NULL) {
                                                        write(1,"Error: Filename must end with '.tar'\n", strlen("Error: Filename must end with '.tar'\n"));
                                                    } else {
                                                        execute_archServer("server", filename);
                                                    }
                                                } else {
                                                    char response[256];
                                                    sprintf(response, "Error: Filename not specified for archiving\n");
                                                    write(client_fd, response, strlen(response));
                                                }
                                            }


                                                 else if (strncmp(secondBuffer, "writeT", 6) == 0) {
                                                    char *filename;
                                                    char *lineStr;
                                                    int lineNumber = -1;  
                                                    char *text;

                                                    strtok(secondBuffer, " ");  
                                                    filename = strtok(NULL, " ");  
                                                    lineStr = strtok(NULL, " ");  
                                                    text = strtok(NULL, "\0");  

                                                    if (!filename) {
                                                        errExit("Error: Missing filename");
                                                    }

                                                    if (lineStr && isdigit(lineStr[0])) {
                                                        lineNumber = atoi(lineStr);  
                                                        if (!text) {  
                                                            errExit("Error: Missing text to write");
                                                        }
                                                    } else {
                                                        text = lineStr; 
                                                    }

                                                    int fd = open(filename, O_RDWR | O_CREAT, 0666);
                                                    if (fd == -1) {
                                                        errExit("File opening failed");
                                                    }

                                                    
                                                    struct stat sb;
                                                    if (fstat(fd, &sb) == -1) {
                                                        close(fd);
                                                        errExit("Failed to get file size");
                                                    }

                                                    char *fileContent = malloc(sb.st_size + 1);
                                                    if (fileContent == NULL) {
                                                        close(fd);
                                                        errExit("Memory allocation failed");
                                                    }

                                                    if (read(fd, fileContent, sb.st_size) != sb.st_size) {
                                                        close(fd);
                                                        free(fileContent);
                                                        errExit("Failed to read file");
                                                    }
                                                    fileContent[sb.st_size] = '\0'; 

                                                    
                                                    char **lines = NULL;
                                                    int totalLines = 0;
                                                    char *line = strtok(fileContent, "\n");
                                                    while (line != NULL) {
                                                        lines = realloc(lines, sizeof(char*) * (totalLines + 1));
                                                        lines[totalLines++] = line;
                                                        line = strtok(NULL, "\n");
                                                    }

                                                    
                                                    if (lseek(fd, 0, SEEK_SET) == -1) {
                                                        free(fileContent);
                                                        free(lines);
                                                        close(fd);
                                                        errExit("lseek failed");
                                                    }

                                                    if (lineNumber > 0) {
                                                        for (int i = 0; i < totalLines; i++) {
                                                            if (i == lineNumber - 1) {
                                                                dprintf(fd, "%s\n", text);  
                                                            } else {
                                                                dprintf(fd, "%s\n", lines[i]);
                                                            }
                                                        }
                                                       
                                                        if (lineNumber > totalLines) {
                                                            for (int i = totalLines; i < lineNumber - 1; i++) {
                                                                dprintf(fd, "\n");
                                                            }
                                                            dprintf(fd, "%s\n", text);
                                                        }
                                                    } else {
                                                      
                                                        for (int i = 0; i < totalLines; i++) {
                                                            dprintf(fd, "%s\n", lines[i]);
                                                        }
                                                        dprintf(fd, "%s\n", text);
                                                    }

                                                    free(fileContent);
                                                    free(lines);
                                                    close(fd);
                                                }



                                    else if(strncmp(secondBuffer, "download", 8) == 0){
                                       
                                         strtok(secondBuffer, " ");
                                        char* fileName = strtok(NULL, " ");
                                        
                                        if (fileName == NULL) {
                                            printf("Cant change file.\n");
                                            return 0;
                                        }
                                        char cwd[1024];
                                        if (getcwd(cwd, sizeof(cwd)) == NULL) {
                                            perror("working directory not detected");
                                            return 0;
                                        }
                                        char sourcePath[512], destinationPath[512];
                                        snprintf(destinationPath, sizeof(sourcePath), "../client/%s", fileName);
                                        snprintf(sourcePath, sizeof(destinationPath), "./%s",fileName);

                                        int sourceFd = open(sourcePath, O_RDONLY);
                                        if (sourceFd < 0) {
                                            perror("Cant open source file");
                                            return 0;
                                        }

                                        int destFd = open(destinationPath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                                        if (destFd < 0) {
                                            perror("Could not open target file");
                                            close(sourceFd);
                                            return 0;
                                        }
                                        char buffer[1024];
                                        int bytesRead;
                                        int totalBytes=0;
                                        while ((bytesRead = read(sourceFd, buffer, sizeof(buffer))) > 0) {
                                            if (write(destFd, buffer, bytesRead) != bytesRead) {
                                                close(sourceFd);
                                                close(destFd);
                                                break;
                                            }
                                            totalBytes += bytesRead;
                                        }
                                        char ar[256];
                                        snprintf(ar,sizeof(ar),"total bytes %d \n",totalBytes);
                                        write(STDOUT_FILENO,ar,strlen(ar));
                                      
                                        
                                        
                                        close(sourceFd);
                                        close(destFd);
                                    
                            }

                           else if (strncmp(secondBuffer, "upload", 6) == 0) {
                                strtok(secondBuffer, " ");    
                                char *filename = strtok(NULL, " "); 

                                if (filename == NULL) {
                                    write(client_fd, "Error: No file specified.\n", 26);
                                    return 0; 
                                }

                                char sourcePath[256], destPath[256];
                                snprintf(sourcePath, sizeof(sourcePath), "../client/%s", filename);
                                snprintf(destPath, sizeof(destPath), "./%s", filename);

                                struct stat file_stat;
                                if (stat(sourcePath, &file_stat) == -1) {
                                    perror("Failed to get file status");
                                    write(client_fd, "Error getting file size.\n", 25);
                                    return 0; 
                                }

                                int source_fd = open(sourcePath, O_RDONLY);
                                if (source_fd < 0) {
                                    perror("Failed to open source file");
                                    write(client_fd, "Error opening source file.\n", 27);
                                    return 0; 
                                }

                                char file_size_msg[256];
                                int dest_fd = open(destPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                                if (dest_fd < 0) {
                                    perror("Failed to open or create destination file");
                                    write(client_fd, "Error opening or creating destination file.\n", 44);
                                    close(source_fd);
                                    return 0; 
                                }

                                char buffer[1024];
                                ssize_t readBytes;
                                int totalBytes=0;
                                while ((readBytes = read(source_fd, buffer, sizeof(buffer))) > 0) {
                                    if (write(dest_fd, buffer, readBytes) != readBytes) {
                                        close(dest_fd);
                                        close(source_fd);
                                        break;

                                    }
                                    totalBytes += readBytes;
                                }
                                char ar[2048];
                                sprintf(ar,"total bytes %d \n",totalBytes);
                                write(1, ar, strlen(ar));
                                close(dest_fd);
                                close(source_fd);
                            }
                            
                            exit(1);    
                        
                        }
                        else{

                            waitpid(new_child, NULL, 0);

                            char out[10240];

                            memset(out, 0, sizeof(out));
                                    if(strncmp(secondBuffer, "help", 4) == 0){
                                strtok(secondBuffer, " ");
                                char * helpAbout = strtok(NULL, "\n");
                                if(helpAbout == NULL){
                                    strcpy(out, "Available commands are:\nhelp, list, readF, writeT, upload, download, quit, killServer\n");
                                }else if(strncmp(helpAbout, "readF", 5) == 0){
                                    strcpy(out, "readF <file> <line #>\n\tdisplay the #th line of the <file>, returns with an error if <file> does not exist\n");
                                }else if(strncmp(helpAbout, "writeT", 6) == 0){
                                    strcpy(out, "writeT <file> <line #> <string>\n\trequest to write the content of “string” to the #th line the <file>, if the line # is not given writes to the end of file. If the file does not exists in Servers directory creates and edits the file at the same time\n");
                                }else if(strncmp(helpAbout, "download", 8) == 0){
                                    strcpy(out, "download <file>\n\trequest to receive <file> from servers directory to client side\n");
                                }else if(strncmp(helpAbout, "upload", 6) == 0){
                                    strcpy(out, "upload <file>\n\tupload the file from the current working directory of client to the servers directory\n");
                                }else if(strncmp(helpAbout, "list", 4) == 0){
                                    strcpy(out, "list\n\tdisplay the list of files in servers directory\n");
                                }else if(strncmp(helpAbout, "quit", 4) == 0){
                                    strcpy(out, "quit\n\tquit\n");
                                }else if(strncmp(helpAbout, "killServer", 10) == 0){
                                    strcpy(out, "killServer\n\tkill the server\n");
                                }else{
                                    strcpy(out, "NO RESPONSE\n");
                                }
                                
                            }
                            else if(strncmp(secondBuffer, "writeT", 6) == 0 || strncmp(secondBuffer, "download", 8) == 0 || strncmp(secondBuffer, "upload", 6) == 0){
                                strcpy(out, "Mission accomplished.\n");
                            }else if(strncmp(secondBuffer, "killServer", 10) == 0){
                                printf("kill signal from client%d... terminating...\n", current + 1);
                                printf("bye\n");
                                fflush(stdout);
                                for(int i = 0; i < numberOfClients; i++){
                                    kill(pid_array[i], SIGTERM);
                                }
                                kill(flag,SIGTERM);
                                
                            }else if(strncmp(secondBuffer, "quit", 4) == 0){
                                printf("client%d disconnect\n",current + 1);
                                strcpy(out, "Sending write request to server log file\nwaiting for log file...\n");
                                fflush(stdout);
                                current--;
                                if(current==-1){
                                    kill(flag,SIGTERM);
                                }
                            }else{
                                read(fdx[0], out, sizeof(out));
                            }

                            write(client_fd, out, sizeof(out));
                            if(semop(semid2, &sembuf_signal, 1) < 0)
                                errExit("semop");
                        
                        }
                    }
                }
            }
        }
    }

    return 0;
}