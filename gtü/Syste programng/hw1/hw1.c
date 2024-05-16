#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAX_INPUT_LENGTH 100
#define MAX_NAME_LENGTH 50
#define MAX_GRADE_LENGTH 3

void gtuStudentGrades(char* fileName, char* out_file) {
    int pid = fork();
    if (pid == -1) {
        printf("Error: Fork failed\n");
        exit(1);
    }

    if (pid == 0) { // Child process
        int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        int xd = open(out_file, O_WRONLY | O_APPEND, 0644);
        char* message1 = "Gtustudentsgrades ";
        char* message2 = fileName;
        char* errorMessage = "Error: Unable to open file\n";
        char* successMessage = "The file was opened successfully.\n";

        if (fd == -1 || xd == -1) {
            write(xd, errorMessage, strlen(errorMessage));
            exit(1);
        }

        // Dosya tanımlayıcısını çocuk işlemde kapat
        write(xd, message1, strlen(message1));
        write(xd, " ", 1);
        write(xd, message2, strlen(message2));
        write(xd, "\n", 1);

        write(xd, successMessage, strlen(successMessage));
        close(fd);
        close(xd);
        exit(0);
    }
    else { // Parent process
        // wait end od the child 
        wait(NULL);
    }
}
void listGrades(char* fileName, char* out_file) {
    int pid = fork();
    if (pid == -1) {
        printf("Error: Fork failed\n");
        exit(1);
    }

    if (pid == 0) { // Child process
        int fd = open(fileName, O_RDONLY);
        int xd = open(out_file, O_WRONLY  | O_APPEND, 0644);
        char* message1 = "listGrades ";
        char* message2 = fileName;
        char* errorMessage = "Error: Unable to open file\n";
        char* successMessage = "Listgrades successfully .\n";

        if (fd == -1 || xd == -1) {
            write(xd, errorMessage, strlen(errorMessage));
            exit(1);
        }
        write(xd, message1, strlen(message1));
        write(xd, " ", 1);
        write(xd, message2, strlen(message2));
        write(xd, "\n", 1);

        char buffer[MAX_INPUT_LENGTH + 1];
        char studentName[MAX_NAME_LENGTH], grade[MAX_GRADE_LENGTH];
        int bytes_read;
        char ch;
        int i = 0;
        int count = 1;
        while ((bytes_read = read(fd, &ch, 1)) > 0 && count <= 5) {
            if (ch == '\n') { // Satır sonuna geldiğinde
                buffer[i] = '\0'; // Stringi sonlandır
                sscanf(buffer, "%[^,], %s", studentName, grade); // İsim ve notu al
                printf("%d.Grade: %s\n", count, grade);
                i = 0;
                memset(buffer, 0, sizeof(buffer));
                count++;
            }
            else {
                buffer[i++] = ch; // Karakteri tampona ekle
            }
        }

        // Başarı mesajını dosyaya yaz
        write(xd, successMessage, strlen(successMessage));
        close(xd);
        close(fd);
        exit(0);
    }
    else { // Parent process
        // Çocuk işleminin bitmesini bekle
        wait(NULL);
    }
}
void listSome(char* fileName, char* out_file, int x, int y) {
    int pid = fork();
    if (pid == -1) {
        printf("Error: Fork failed\n");
        exit(1);
    }

    if (pid == 0) { // Child process
        int fd = open(fileName, O_RDONLY);
        int xd = open(out_file, O_WRONLY  | O_APPEND, 0644);
        char* message1 = "listSome ";
        char* message2 = fileName;
        char* numberMessage = " number:";
        char* pageMessage = " page:";
        char* errorMessage = "Error: Unable to open file\n";
        char* successMessage = "Student successfully list.\n";

        if (fd == -1 || xd == -1) {
            write(xd, errorMessage, strlen(errorMessage));
            exit(1);
        }

        // Dosya tanımlayıcısını çocuk işlemde kapat
        write(xd, message1, strlen(message1));
        write(xd, " ", 1);
        write(xd, message2, strlen(message2));
        write(xd, numberMessage, strlen(numberMessage));
        dprintf(xd, "%d", x);
        write(xd, pageMessage, strlen(pageMessage));
        dprintf(xd, "%d\n", y);

        char buffer[MAX_INPUT_LENGTH + 1];
        char studentName[MAX_NAME_LENGTH], grade[MAX_GRADE_LENGTH];
        int bytes_read;
        char ch;
        int i = 0;
        int count = 0, a = 0;
        while ((bytes_read = read(fd, &ch, 1)) > 0 && count < x) {
            if (ch == '\n') { // Satır sonuna geldiğinde
                buffer[i] = '\0'; // Stringi sonlandır
                sscanf(buffer, "%[^,], %s", studentName, grade); // İsim ve notu al
                if (a >= x * (y - 1) && a < x * y) {
                    printf("Name: %s  Grade: %s\n", studentName, grade);
                    count++;
                }
                i = 0;
                a++;
                memset(buffer, 0, sizeof(buffer));
            }
            else {
                buffer[i++] = ch; // Karakteri tampona ekle
            }
        }

        // Başarı mesajını dosyaya yaz
        write(xd, successMessage, strlen(successMessage));
        close(fd);
        close(xd);
        exit(0);
    }
    else { // Parent process
        // Çocuk işleminin bitmesini bekle
        wait(NULL);
    }
}
void showAll(char* fileName,char*out_file) {
    int pid = fork();
    if (pid == -1) {
        printf("Error: Fork failed\n");
        exit(1);
    }

    if (pid == 0) { // Child process
        int fd = open(fileName, O_RDONLY);
        int xd = open(out_file, O_WRONLY  | O_APPEND, 0644);
        char* message1 = "showAll ";
        char* message2 = fileName;
        char* errorMessage = "Error: Unable to open file\n";
        char* successMessage = "Student successfully show.\n";

        if (fd == -1 || xd == -1) {
            write(xd, errorMessage, strlen(errorMessage));
            exit(1);
        }

        // Dosya tanımlayıcısını çocuk işlemde kapat
        write(xd, message1, strlen(message1));
        write(xd, " ", 1);
        write(xd, message2, strlen(message2));
        write(xd, "\n", 1);

        char buffer[MAX_INPUT_LENGTH + 1];
        char studentName[MAX_NAME_LENGTH], grade[MAX_GRADE_LENGTH];
        int bytes_read;
        char ch;
        int i = 0;
        while ((bytes_read = read(fd, &ch, 1)) > 0) {
            if (ch == '\n') { // Satır sonuna gelindiğinde
                buffer[i] = '\0'; // Stringi sonlandır
                sscanf(buffer, "%[^,], %s", studentName, grade); // İsim ve notu al
                printf("%s   %s\n", studentName, grade);
                i = 0;
                memset(buffer, 0, sizeof(buffer));
            }
            else {
                buffer[i++] = ch; // Karakteri tampona ekle
            }
        }

        // Başarı mesajını dosyaya yaz
        write(xd, successMessage, strlen(successMessage));
        close(fd);
        close(xd);
        exit(0);
    }
    else { // Parent process
        // Çocuk işleminin bitmesini bekle
        wait(NULL);
    }
}

void searchStudent(char* fileName, char* name, char* out_file) {
    int pid = fork();
    if (pid == -1) {
        printf("Error: Fork failed\n");
        exit(1);
    }

    if (pid == 0) { // Child process
        int fd = open(fileName, O_RDONLY);
        int xd = open(out_file, O_WRONLY  | O_APPEND, 0644);
        char* message1 = "searchStudent ";
        char* message2 = fileName;
        char* errorMessage1 = "Error: Unable to open file\n";
        char* errorMessage2 = "Student not found.\n";
        char* successMessage = "Student successfully found\n";

        if (fd == -1 || xd == -1) {
            write(xd, errorMessage1, strlen(errorMessage1));
            exit(1);
        }

        // Dosya tanımlayıcısını çocuk işlemde kapat
        write(xd, message1, strlen(message1));
        write(xd, " ", 1);
        write(xd, message2, strlen(message2));
        write(xd, "\n", 1);

        char buffer[MAX_INPUT_LENGTH + 1];
        char studentName[MAX_NAME_LENGTH], grade[MAX_GRADE_LENGTH];
        int found = 0;
        int bytes_read;
        char ch;
        int i = 0;
        while ((bytes_read = read(fd, &ch, 1)) > 0) {
            if (ch == '\n') { // Satır sonuna gelindiğinde
                buffer[i] = '\0'; // Stringi sonlandır
                sscanf(buffer, "%[^,], %s", studentName, grade); // İsim ve notu al
                if (strcmp(studentName, name) == 0) {
                    printf("Student found:\nName: %s\nGrade: %s\n", studentName, grade);
                    found = 1;
                    break; 
                }
                i = 0;
                memset(buffer, 0, sizeof(buffer));
            }
            else {
                buffer[i++] = ch; // Karakteri tampona ekle
            }
        }

        if (!found) {
            printf("Student not found.\n");
            write(xd, errorMessage2, strlen(errorMessage2));
            exit(1);
        }
        write(xd, successMessage, strlen(successMessage));
        close(fd);
        close(xd);
        exit(0);
    }
    else { // Parent process
        wait(NULL); // Child processin bitmesini bekle
    }
}

typedef struct {
    char name[MAX_NAME_LENGTH];
    char grade[MAX_GRADE_LENGTH];
} StudentGrade;

int compare_grade(StudentGrade a, StudentGrade b) {
    return strcmp(a.grade, b.grade);
}

int compare_name(StudentGrade a, StudentGrade b) {
    return strcmp(a.name, b.name);
}

// Bubble Sort fonksiyonu
void bublesort(StudentGrade* array, int n,char*commend) {
    int i, j;
    StudentGrade temp;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (strcmp(commend, "name") == 0) {
                if (compare_name(array[j], array[j + 1]) > 0) {
                    temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                }
            }
            else if (strcmp(commend, "grade") == 0) {
                if (compare_grade(array[j], array[j + 1]) > 0) {
                    temp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = temp;
                }
            }
        }
    }
}
void sortAll(char* fileName, char* out_file) {

    int pid = fork();
    if (pid == -1) {
        printf("Error: Fork failed\n");
        exit(1);
    }
    if (pid == 0) {
         char* message1 = "sortAll ";
        char* message2 = fileName;
        char* errorMessage1 = "Error: Unable to open file\n";
        char* successMessage = "Student grades sorted and written back to the file.\n";
        int fd = open(fileName, O_RDWR);
        int xd = open(out_file, O_WRONLY  | O_APPEND, 0644);
        write(xd, message1, strlen(message1));
        write(xd, " ", 1);
        write(xd, message2, strlen(message2));
        write(xd, "\n", 1);
        if (fd == -1 || xd == -1) {
            printf("Error: Unable to open file\n");
             write(xd, errorMessage1, strlen(errorMessage1));
             exit(1);
        }

        // Count the number of lines in the file to determine the number of student grades
        int numGrades = 0;
        char ch;
        while (read(fd, &ch, 1) > 0) {
            if (ch == '\n') {
                numGrades++;
            }
        }
        lseek(fd, 0, SEEK_SET);

        // Allocate memory for student grades
        StudentGrade* grades = malloc(numGrades * sizeof(StudentGrade));
        if (grades == NULL) {
            printf("Error: Memory allocation failed\n");
             write(xd, "Error: Memory allocation failed\n", strlen("Error: Memory allocation failed\n"));
            close(fd);
            close(xd);
            exit(1);
        }

        // Read student grades from the file
        char line[MAX_NAME_LENGTH + MAX_GRADE_LENGTH + 2]; // +2 for comma and newline
        for (int i = 0; i < numGrades; i++) {
            int j = 0;
            while (read(fd, &ch, 1) > 0 && ch != '\n' && j < MAX_NAME_LENGTH + MAX_GRADE_LENGTH + 1) {
                line[j++] = ch;
            }
            line[j] = '\0';
            sscanf(line, "%[^,], %s", grades[i].name, grades[i].grade);
        }

        // Prompt user for sorting criteria
        printf("Sort by (name/grade): ");
        char sortBy[MAX_NAME_LENGTH];
        fgets(sortBy, sizeof(sortBy), stdin);
        sortBy[strcspn(sortBy, "\n")] = 0; // Removing trailing newline

        // Prompt user for sorting order
        printf("Sort in (asc/desc)ending order: ");
        char sortOrder[MAX_NAME_LENGTH];
        fgets(sortOrder, sizeof(sortOrder), stdin);
        sortOrder[strcspn(sortOrder, "\n")] = 0; // Removing trailing newline
        // Sort student grades based on user input
        if (strcmp(sortBy, "name") == 0) {
            if (strcmp(sortOrder, "asc") == 0) {
                bublesort(grades, numGrades, sortBy);
                dprintf(xd, "name ascending \n");
            }
            else if (strcmp(sortOrder, "desc") == 0) {
                bublesort(grades, numGrades, sortBy);
                dprintf(xd, " name descending \n");
                // Reverse the order
                for (int i = 0; i < numGrades / 2; i++) {
                    StudentGrade temp = grades[i];
                    grades[i] = grades[numGrades - i - 1];
                    grades[numGrades - i - 1] = temp;
                }
            }
            else {
                printf("Error: Invalid sorting order\n");
                 write(xd, "Error: Invalid sorting order\n", strlen("Error: Invalid sorting order\n"));
                close(fd);
                close(xd);
                free(grades);
                exit(1);
            }
        }
        else if (strcmp(sortBy, "grade") == 0) {
            if (strcmp(sortOrder, "asc") == 0) {
                bublesort(grades, numGrades, sortBy);
                write(xd, "grade ascending \n", strlen("grade ascending \n"));
                
            }
            else if (strcmp(sortOrder, "desc") == 0) {
                bublesort(grades, numGrades, sortBy);
                 write(xd, "grade descending \n", strlen("grade descending \n"));
                //reverse the order
                for (int i = 0; i < numGrades / 2; i++) {
                    StudentGrade temp = grades[i];
                    grades[i] = grades[numGrades - i - 1];
                    grades[numGrades - i - 1] = temp;
                }
            }
            else {
                printf("Error: Invalid sorting order\n");
                 write(xd, "Error: Invalid sorting order\n", strlen("Error: Invalid sorting order\n"));
                close(fd);
                close(xd);
                free(grades);
                exit(1);
            }
        }
        else {
            printf("Error: Invalid sorting criteria\n");
            write(xd, "Error: Invalid sorting criteria\n", strlen("Error: Invalid sorting criteria\n"));
            close(fd);
            close(xd);
            free(grades);
            exit(1);
        }

        // Write sorted student grades back to the file
        lseek(fd, 0, SEEK_SET);
        for (int i = 0; i < numGrades; i++) {
            dprintf(fd, "%s, %s\n", grades[i].name, grades[i].grade);

        }

        printf("Student grades sorted and written back to the file.\n");
       write(xd, "Student grades sorted and written back to the file.\n", strlen("Student grades sorted and written back to the file.\n"));

        // Free allocated memory and close the file
        close(fd);
        close(xd);
        free(grades);
        exit(0);
    }
    else {
        wait(NULL);
    }
}
void listCommend() {
     printf("gtuStudentGrades \"filename\" : Lists student grades from the specified 'filename' file.\n");
    printf("addStudentGrade \"Name Surname\" \"Grade\" \"filename\": Adds the specified 'Name Surname' and 'Grade' information to the file.\n");
    printf("searchStudent \"Name Surname\" \"filename\": Searches for the specified 'Name Surname' student in the file and displays their information if found.\n");
    printf("sortAll \"filename\" : Sorts the student grades in the file (by name or grade) and rewrites it to the file.\n");
    printf("showAll \"filename\" : Lists all student grades from the file.\n");
    printf("listGrades \"filename\" : Lists student grades from the file.\n");
    printf("listSome \"numofEntries\" \"pageNumber\" \"filename\" : Lists student grades from the file using the specified number of entries and page number.\n");
}
void addStudentGrade(char* fileName, char* name, char* grade, char* out_file) {
    int pid = fork();
    if (pid == -1) {
        printf("Error: Fork failed\n");
        exit(1);
    }

    if (pid == 0) { // Child process
        int fd = open(fileName, O_WRONLY  | O_APPEND, 0644);
        int xd = open(out_file, O_WRONLY  | O_APPEND, 0644);
        char* message1 = "addStudentGrade ";
        char* message2 = fileName;
        char* errorMessage = "Error: Unable to open file\n";
        char* successMessage = "Student grade added successfully.\n";
        write(xd, message1, strlen(message1));
        write(xd, " ", 1);
        write(xd, message2, strlen(message2));
        write(xd, " ", 1);
        char validGrades[][3] = { "AA", "BB", "FF","CC", "FF", "VF", "BA", "AC", "DC", "CB", "DD" };
        if (fd == -1 || xd==-1) {
            printf("Error: Unable to open file\n");
            write(xd, errorMessage, strlen(errorMessage));
            exit(1);
        }
         for (int i = 0; grade[i] != '\0'; i++) {
            grade[i] = toupper(grade[i]);
        }
         int isValid = 0;
         for (int i = 0; i < sizeof(validGrades) / sizeof(validGrades[0]); i++) {
             if (strcmp(grade, validGrades[i]) == 0) {
                 isValid = 1;
                 break;
             }
         }
         if (!isValid) {
             printf("Error: Invalid grade format\n");
             write(xd, "Error: Invalid grade format\n", strlen("Error: Invalid grade format\n"));
             exit(1);
         }
        char buffer[MAX_NAME_LENGTH + MAX_GRADE_LENGTH + 2]; // +2 for space and newline
        snprintf(buffer, sizeof(buffer), "%s,%s \n", name, grade);
        write(xd, buffer, strlen(buffer));
        if (write(fd, buffer, strlen(buffer)) == -1) {
            printf("Error: Unable to write to file\n");
           write(xd, "Error: Unable to write to file\n", strlen("Error: Unable to write to file\n"));
            close(fd);
            close(xd);
            exit(1);
        }
        write(xd, successMessage, strlen(successMessage));
        close(fd);
        close(xd);
        printf("Student grade added successfully.\n");
        exit(0);
    }
    else { // Parent process
        wait(NULL); // Wait for child process to finish
    }
}
void create_file(char*out_file) {
    int pid = fork();
    if (pid == -1) {
        printf("Error: Fork failed\n");
        exit(1);
    }
    char* message = "Error: Unable to open file\n";
    if (pid == 0) { // Child process
        int fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        char* message1 = "Creat_file ";
        write(fd, message1, strlen(message1));
        write(fd, out_file, strlen(out_file));
        write(fd, "\n ", 1);
        if (fd == -1) {
            printf("Error: Unable to open file\n");
            write(fd, message, strlen(message));

            exit(1);
        }

        // Close the file descriptor in the child process
        close(fd);
        exit(0);
    }
    else { // Parent process
        // Wait for the child process to finish
        wait(NULL);
    }

}
int main(int argc, char* argv[]) {

    char* fileName = (char*)malloc(MAX_INPUT_LENGTH * sizeof(char));
    char* out_file = "log_file.log";
    create_file(out_file);
    int flag = 0;
    int x = 0;
    int y = 0;
    char input[MAX_INPUT_LENGTH];
    printf("enter q for exit\n");
    int fd = open(out_file, O_WRONLY | O_APPEND, 0644);
    // Parse input
    while (input != "q") {
        int out=0;
        printf("Enter command and student information ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0; // Removing trailing newline
        char command[MAX_NAME_LENGTH], name[MAX_NAME_LENGTH], grade[MAX_GRADE_LENGTH];
        if (sscanf(input, "%s \"%[^\"]\" \"%[^\"]\"  \"%[^\"]\" ", command, name, grade, fileName) == 4) {
            if (strcmp(command, "listSome") == 0) {
                x = atoi(name);
                y = atoi(grade);
            }
            out=4;
            // addStudentGrade operation will be performed here
        }
        else if (sscanf(input, "%s \"%[^\"]\" \"%[^\"]\"", command, name, fileName) == 3) {
            // searchstudent operation will be performed here
            out=3;
        }
        else if (sscanf(input, "%s \"%[^\"]\"", command, fileName) == 2) {
            out=2;
        }
        else if (sscanf(input, "%s", command) == 1) {
            flag = 1;
        }
        else {
            printf("Invalid input. Please make sure your input is in the correct format.\n");
            return 0;
        }


        // Check if command is addStudentGrade
        if (strcmp(command, "addStudentGrade") == 0 && out==4) {
            addStudentGrade(fileName, name, grade, out_file);
        }
        else if (strcmp(command, "searchStudent") == 0 && out==3) {
            searchStudent(fileName, name, out_file);


        }
        else if (strcmp(command, "gtuStudentGrades") == 0) {
            if (flag == 1) {
                listCommend();
                flag = 0;
            }
            else {
                gtuStudentGrades(fileName,out_file);
            }
        }
        else if (strcmp(command, "sortAll" ) == 0 && out==2) {
            sortAll(fileName, out_file);
        }
        else if (strcmp(command, "showAll") == 0 && out==2) {
            showAll(fileName, out_file);
        }
        else if (strcmp(command, "listGrades") == 0 && out==2) {
            listGrades(fileName, out_file);
        }
        else if (strcmp(command, "listSome") == 0 && out==4) {
            listSome(fileName, out_file, x, y);
        }
        else if (strcmp(command, "q") == 0) {
            printf("exited terminal");
            return 1;
        }
        else {
            printf("Error: Invalid command\n");
            write(fd,command ,strlen(command));
            write(fd,"\n",1);
            write(fd, "Error: Invalid command or missing command part\n", strlen("Error: Invalid command or missing command part\n"));
        }
    }
    close(fd);
    return 0;
}
