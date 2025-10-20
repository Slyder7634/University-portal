#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

void pause_console()
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
    getchar();
}

struct Record
{
    int student_id;
    char date[20];
    char activity[100];
    float amount;
    float credits_after;
};

struct Student
{
    int student_id;
    char name[50];
    float credits;
    char password[20];
};

struct Teacher
{
    int teacher_id;
    char name[50];
    char password[20];
};

struct Staff
{
    int id;
    char name[50];
    char password[20];
};

struct Student *students = NULL;
int student_num = 0;
int student_capacity = 0;

struct Teacher *teachers = NULL;
int teacher_num = 0;
int teacher_capacity = 0;

struct Record *records = NULL;
int record_num = 0;
int record_capacity = 0;

int check_password(struct Student *user)
{
    char entered_pass[20];
    printf("Enter the password for %s: ", user->name);
    scanf("%19s", entered_pass);
    if (strcmp(user->password, entered_pass) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int check_admin(struct Staff *workers)
{
    char entered_pass[20];
    printf("\nNOTE : Password should be at least 8 characters and 19 maximum characters long and contain a mix of letters, numbers, and symbols.\n");
    printf("Enter the password for %s: ", workers->name);
    scanf("%19s", entered_pass);
    if (strcmp(workers->password, entered_pass) == 0)
    {
        return 1;
    }
    else
    {
        printf("Incorrect password.\n");
        return 0;
    }
}

void add_record_simple(int student_id, const char *type, float amount, float credits_after)
{
    if (record_num == record_capacity)
    {
        record_capacity = (record_capacity == 0) ? 10 : record_capacity * 2;
        records = realloc(records, record_capacity * sizeof(struct Record));
        if (!records)
        {
            printf("Memory allocation error in add_record.\n");
            exit(1);
        }
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    snprintf(records[record_num].date, sizeof(records[record_num].date),
             "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

    // Format activity like: AddedCredits 5.00
    snprintf(records[record_num].activity, sizeof(records[record_num].activity),
             "%s %.2f", type, amount);

    records[record_num].student_id = student_id;
    records[record_num].amount = amount;
    records[record_num].credits_after = credits_after;
    record_num++;
}

void delete_student_by_id(int student_id)
{
    int found = 0;
    for (int i = 0; i < student_num; i++)
    {
        if (students[i].student_id == student_id)
        {
            found = 1;
            for (int j = i; j < student_num - 1; j++)
                students[j] = students[j + 1];
            student_num--;
            if (student_num < student_capacity / 2 && student_capacity > 2)
            {
                student_capacity = student_capacity / 2;
                students = realloc(students, student_capacity * sizeof(struct Student));
            }
            printf("Student ID %d deleted successfully.\n", student_id);
            break;
        }
    }
    if (!found)
    {
        printf("No student found with ID: %d\n", student_id);
    }
}

int main()
{
    // Load students from file (optional)
    FILE *fp = fopen("..\\students.txt", "r");
    if (fp)
    {
        while (1)
        {
            if (student_num == student_capacity)
            {
                student_capacity = (student_capacity == 0) ? 10 : student_capacity * 2;
                students = realloc(students, student_capacity * sizeof(struct Student));
                if (!students)
                {
                    printf("Memory allocation error.\n");
                    exit(1);
                }
            }

            int id;
            char name[50], pass[20];
            float creds;

            int read = fscanf(fp, "%d %49s %f %19s", &id, name, &creds, pass);
            if (read != 4)
                break;

            students[student_num].student_id = id;
            strncpy(students[student_num].name, name, sizeof(students[student_num].name) - 1);
            students[student_num].name[sizeof(students[student_num].name) - 1] = '\0';
            students[student_num].credits = creds;
            strncpy(students[student_num].password, pass, sizeof(students[student_num].password) - 1);
            students[student_num].password[sizeof(students[student_num].password) - 1] = '\0';
            student_num++;
        }
        fclose(fp);
    }

    // Load records (optional)
    FILE *fp_rec = fopen("..\\records.txt", "r");
    if (fp_rec)
    {
        char line[256];
        while (fgets(line, sizeof(line), fp_rec))
        {
            if (record_num == record_capacity)
            {
                record_capacity = (record_capacity == 0) ? 10 : record_capacity * 2;
                records = realloc(records, record_capacity * sizeof(struct Record));
                if (!records)
                {
                    printf("Memory allocation error.\n");
                    exit(1);
                }
            }

            int id;
            char date[20], activity[100];
            float amount, credits_after;

            int parsed = sscanf(line, "%d %19s \"%99[^\"]\" %f %f", &id, date, activity, &amount, &credits_after);
            if (parsed != 5)
            {
                continue; // skip malformed
            }

            records[record_num].student_id = id;
            strncpy(records[record_num].date, date, sizeof(records[record_num].date) - 1);
            records[record_num].date[sizeof(records[record_num].date) - 1] = '\0';
            strncpy(records[record_num].activity, activity, sizeof(records[record_num].activity) - 1);
            records[record_num].activity[sizeof(records[record_num].activity) - 1] = '\0';
            records[record_num].amount = amount;
            records[record_num].credits_after = credits_after;
            record_num++;
        }
        fclose(fp_rec);
    }

    // staff list (hard-coded)
    struct Staff workers[5] = {
        {1001, "Ankit", "7634"},
        {1002, "Slyder", "7634"},
        {1003, "Ankur", "7634"},
        {1004, "Jazzy", "7634"},
        {1005, "Abhijeet", "7634"},
    };

    char login_type[16];
    int user_id, option, admin_id;

start:
    printf("==================================\n");
    printf("         UNIVERSITY MANAGEMENT SYSTEM\n");
    printf("==================================\n\n");

    printf("Login as (Admin or Student): ");
    fgets(login_type, sizeof(login_type), stdin);
    login_type[strcspn(login_type, "\n")] = '\0';

    if (strcasecmp(login_type, "Student") == 0)
    {
        printf("Enter student ID: ");
        if (scanf("%d", &user_id) != 1) { getchar(); goto start; }
        getchar(); // consume newline

        while (1)
        {
            int found = 0;
            for (int i = 0; i < student_num; i++)
            {
                if (students[i].student_id == user_id)
                {
                    found = 1;
                    if ((check_password(&students[i])) == 1)
                    {
                    Menu:
                        system("cls");
                        printf("\nWelcome, %s!\n\n", students[i].name);
                        printf("=== Student Menu ===\n\n");
                        printf("1. Check credits\n");
                        printf("2. Enroll new student\n");
                        printf("3. Add credits\n");
                        printf("4. Use credits\n");
                        printf("5. Transfer credits\n");
                        printf("6. View records\n");
                        printf("7. Check student information\n");
                        printf("8. Update information\n");
                        printf("9. Exit to login\n");

                        printf("\nEnter option: ");
                        if (scanf("%d", &option) != 1) { getchar(); option = -1; }
                        getchar();
                        if (option == 9)
                        {
                            system("cls");
                            goto start;
                        }

                        if (option == 1)
                        {
                            system("cls");
                            printf("==================================\n");
                            printf("         CHECK CREDITS           \n");
                            printf("==================================\n\n");
                            printf("Your credits: %.2f\n", students[i].credits);
                        }

                        if (option == 3)
                        {
                            system("cls");
                            printf("==================================\n");
                            printf("         ADD CREDITS           \n");
                            printf("==================================\n\n");
                            int b = 0;
                            printf("Enter credits to add: ");
                            if (scanf("%d", &b) == 1)
                            {
                                students[i].credits += b;
                                printf("%d credits added successfully!\n", b);
                                printf("Updated credits: %.2f\n", students[i].credits);
                                add_record_simple(students[i].student_id, "AddedCredits", b, students[i].credits);
                            }
                            getchar();
                        }

                        if (option == 4)
                        {
                            system("cls");
                            printf("==================================\n");
                            printf("         USE CREDITS           \n");
                            printf("==================================\n\n");
                            int b = 0;
                            printf("Enter credits to use: ");
                            if (scanf("%d", &b) == 1)
                            {
                                if (b <= students[i].credits)
                                {
                                    students[i].credits -= b;
                                    printf("%d credits used successfully!\n", b);
                                    printf("Updated credits: %.2f\n", students[i].credits);
                                    add_record_simple(students[i].student_id, "UsedCredits", b, students[i].credits);
                                }
                                else
                                {
                                    printf("Insufficient credits.\n");
                                }
                            }
                            getchar();
                        }

                        if (option == 5)
                        {
                            system("cls");
                            printf("==================================\n");
                            printf("         TRANSFER CREDITS           \n");
                            printf("==================================\n\n");
                            int b = 0, id_to = 0, target_found = 0;
                            printf("Enter student ID to transfer to: ");
                            scanf("%d", &id_to);
                            printf("Enter credits to Transfer: ");
                            scanf("%d", &b);
                            if (b <= students[i].credits)
                            {
                                for (int j = 0; j < student_num; j++)
                                {
                                    if (students[j].student_id == id_to)
                                    {
                                        students[i].credits -= b;
                                        students[j].credits += b;
                                        target_found = 1;
                                        printf("%d credits transferred successfully to %s.\n", b, students[j].name);
                                        printf("Your updated credits: %.2f\n", students[i].credits);
                                        add_record_simple(students[i].student_id, "Transferred", b, students[i].credits);
                                        add_record_simple(students[j].student_id, "Received", b, students[j].credits);
                                        break;
                                    }
                                }
                                if (!target_found)
                                {
                                    printf("Recipient student not found.\n");
                                }
                            }
                            else
                            {
                                printf("Insufficient credits.\n");
                            }
                            getchar();
                        }

                        if (option == 2)
                        {
                            system("cls");
                            char confirm[2], nam[50], pass[20];
                            float new_creds;

                            printf("==================================\n");
                            printf("         NEW STUDENT           \n");
                            printf("==================================\n\n");

                            printf("Type Y to continue or N to cancel: ");
                            scanf("%1s", confirm);

                            if (strcasecmp(confirm, "Y") == 0)
                            {
                                if (student_num == student_capacity)
                                {
                                    student_capacity = (student_capacity == 0) ? 2 : student_capacity * 2;
                                    students = realloc(students, student_capacity * sizeof(struct Student));
                                }

                                printf("Enter name: ");
                                scanf("%49s", nam);
                                printf("initial credits: ");
                                scanf("%f", &new_creds);
                                printf("Set password: ");
                                scanf("%19s", pass);

                                students[student_num].student_id = 1000 + student_num;
                                strncpy(students[student_num].name, nam, sizeof(students[student_num].name) - 1);
                                students[student_num].name[sizeof(students[student_num].name) - 1] = '\0';
                                students[student_num].credits = new_creds;
                                strncpy(students[student_num].password, pass, sizeof(students[student_num].password) - 1);
                                students[student_num].password[sizeof(students[student_num].password) - 1] = '\0';
                                student_num++;

                                printf("Student enrolled successfully!\n");
                            }
                            else
                            {
                                printf("Cancelled.\n");
                            }
                            getchar();
                        }

                        if (option == 7)
                        {
                            system("cls");
                            printf("==================================\n");
                            printf("         STUDENT INFO           \n");
                            printf("==================================\n\n");

                            printf("STUDENT ID                  NAME                  CREDITS\n");
                            printf("%d                              %s                           %.2f\n", students[i].student_id, students[i].name, students[i].credits);
                        }

                        if (option == 8)
                        {
                            int y;
                            char z[50];
                            system("cls");

                            printf("==================================\n");
                            printf("         UPDATE INFO           \n");
                            printf("==================================\n\n");

                            printf("What do you want to update?\n\n1. Student name\n2. Password\n");
                            printf("Enter option: ");
                            if (scanf("%d", &y) != 1) { getchar(); continue; }
                            getchar();

                            if (y == 1)
                            {
                                system("cls");
                                printf("==================================\n");
                                printf("         UPDATE NAME           \n");
                                printf("==================================\n\n");

                                printf("\nCurrent name: %s\n", students[i].name);
                                printf("Enter new name: ");
                                fgets(z, sizeof(z), stdin);
                                z[strcspn(z, "\n")] = 0;

                                if (check_password(&students[i]))
                                {
                                    strncpy(students[i].name, z, sizeof(students[i].name) - 1);
                                    students[i].name[sizeof(students[i].name) - 1] = '\0';
                                    printf("\nYour name has been successfully updated to: %s\n", students[i].name);
                                }
                                else
                                {
                                    printf("Password verification failed.\n");
                                }
                            }

                            if (y == 2)
                            {
                                char xy[20];
                                printf("==================================\n");
                                printf("         CHANGE PASSWORD           \n");
                                printf("==================================\n\n");

                                printf("Please verify yourself!\n");

                                if (check_password(&students[i]))
                                {
                                    printf("Enter new password: ");
                                    fgets(xy, sizeof(xy), stdin);
                                    xy[strcspn(xy, "\n")] = 0;

                                    strncpy(students[i].password, xy, sizeof(students[i].password) - 1);
                                    students[i].password[sizeof(students[i].password) - 1] = '\0';
                                    system("cls");
                                    printf("PASSWORD UPDATED SUCCESSFULLY!!\n\nPlease Login Again...");
                                    goto start;
                                }
                                else
                                {
                                    printf("Password verification failed.\n");
                                }
                            }
                        }

                        if (option == 6)
                        {
                            system("cls");
                            printf("==================================\n");
                            printf("         RECORDS           \n");
                            printf("==================================\n\n");

                            printf("Showing records for student ID: %d\n", students[i].student_id);

                            int found = 0;
                            for (int k = 0; k < record_num; k++)
                            {
                                if (records[k].student_id == students[i].student_id)
                                {
                                    printf("%s | %s | %.2f | Credits after: %.2f\n",
                                           records[k].date,
                                           records[k].activity,
                                           records[k].amount,
                                           records[k].credits_after);
                                    found = 1;
                                }
                            }
                            if (!found)
                            {
                                printf("No records found for this student.\n");
                            }
                        }

                        break;
                    }
                    else
                    {
                        printf("Incorrect password.\n");
                        goto end;
                    }
                }
            }

            if (!found)
            {
                printf("\nStudent not found!\n");
                break;
            }

            printf("\n\nPress Enter to go back to menu !!");
            if (option == 1)
            {
                getchar();
            }
            else
            {
                pause_console();
            }

            goto Menu;
        }
    }
    else if (strcasecmp(login_type, "Admin") == 0)
    {
        while (1)
        {
            printf("Enter Admin ID: ");
            if (scanf("%d", &admin_id) != 1) { getchar(); goto start; }
            getchar();

            int found = 0;
            for (int i = 0; i < 5; i++)
            {
                if (workers[i].id == admin_id)
                {
                    found = 1;
                    if ((check_admin(&workers[i])) == 1)
                    {
                        while (1)
                        {
                        options:
                            printf("\nWelcome, %s!\n\n", workers[i].name);
                            printf("=== Admin Menu ===\n\n");
                            printf("1. View all students\n");
                            printf("2. Search for a student\n");
                            printf("3. Enroll new student\n");
                            printf("4. Remove a student\n");
                            printf("5. View all records\n");
                            printf("6. Generate detailed student report\n");
                            printf("7. Modify student credits\n");
                            printf("8. Update student information\n");
                            printf("9. Manage admin users\n");
                            printf("10. System logs & security checks\n");
                            printf("11. Exit\n");

                            printf("\nEnter option: ");
                            if (scanf("%d", &option) != 1) { getchar(); option = -1; }
                            getchar(); // flush newline
                            if (option >= 1 && option <= 11)
                            {
                                if (option == 1)
                                {
                                    system("cls");
                                    printf("STUDENT ID                  NAME                  CREDITS\n");
                                    for (int i = 0; i < student_num; i++)
                                    {
                                        printf("%d                              %s                           %.2f\n", students[i].student_id, students[i].name, students[i].credits);
                                    }
                                }

                                if (option == 2)
                                {
                                    system("cls");
                                    printf("Choose search type\n");
                                    printf("1. Search by student ID\n2. Search by name\n3. Search by credits\n4.Go back\n");
                                    int choice, num1 = 0, acc = -1;
                                    if (scanf("%d", &choice) != 1) { getchar(); goto options; }

                                    if (choice == 1)
                                    {
                                        printf("Enter student ID: ");
                                        scanf("%d", &user_id);
                                        for (int i = 0; i < student_num; i++)
                                        {
                                            if (students[i].student_id == user_id)
                                            {
                                                num1++;
                                                acc = i;
                                            }
                                        }

                                        if (acc != -1)
                                        {
                                            printf("STUDENT ID                  NAME                  CREDITS\n");
                                            printf("%d                              %s                           %.2f\n", students[acc].student_id, students[acc].name, students[acc].credits);
                                        }
                                        else
                                        {
                                            printf("No student found.\n\n");
                                        }
                                        printf("\n\n%d student(s) found", num1);
                                    }
                                    else if (choice == 3)
                                    {
                                        int user_cred;
                                        printf("Enter credits : ");
                                        scanf("%d", &user_cred);
                                        printf("STUDENT ID                  NAME                  CREDITS\n");
                                        for (int i = 0; i < student_num; i++)
                                        {
                                            if ((int)students[i].credits == user_cred)
                                            {
                                                acc = i;
                                                printf("%d                              %s                           %.2f\n", students[acc].student_id, students[acc].name, students[acc].credits);
                                                num1++;
                                            }
                                        }
                                        printf("\n\n%d student(s) found", num1);
                                    }

                                    else if (choice == 2)
                                    {
                                        char entered_name[50];
                                        printf("Enter the name: ");
                                        scanf("%49s", entered_name);
                                        printf("STUDENT ID                  NAME                  CREDITS\n");
                                        for (int i = 0; i < student_num; i++)
                                        {
                                            if (strcasecmp(students[i].name, entered_name) == 0)
                                            {
                                                acc = i;
                                                printf("%d                              %s                           %.2f\n", students[acc].student_id, students[acc].name, students[acc].credits);
                                                num1++;
                                            }
                                        }
                                        if (num1 == 0)
                                        {
                                            printf("No student found.\n");
                                        }
                                        printf("\n\n%d student(s) found", num1);
                                    }
                                    else if (choice == 4)
                                    {
                                        system("cls");
                                        goto options;
                                    }
                                }

                                if (option == 3)
                                {
                                    system("cls");
                                    char confirm[2], nam[50], pass[20];
                                    float new_creds;

                                    printf("==================================\n");
                                    printf("         NEW STUDENT           \n");
                                    printf("==================================\n\n");

                                    printf("Type Y to continue or N to cancel: ");
                                    scanf("%1s", confirm);

                                    if (strcasecmp(confirm, "Y") == 0)
                                    {
                                        if (student_num == student_capacity)
                                        {
                                            student_capacity = (student_capacity == 0) ? 2 : student_capacity * 2;
                                            students = realloc(students, student_capacity * sizeof(struct Student));
                                        }

                                        printf("Enter name: ");
                                        scanf("%49s", nam);
                                        printf("initial credits: ");
                                        scanf("%f", &new_creds);
                                        printf("Set password: ");
                                        scanf("%19s", pass);
                                        students[student_num].student_id = 1000 + student_num;
                                        strncpy(students[student_num].name, nam, sizeof(students[student_num].name) - 1);
                                        students[student_num].name[sizeof(students[student_num].name) - 1] = '\0';
                                        students[student_num].credits = new_creds;
                                        strncpy(students[student_num].password, pass, sizeof(students[student_num].password) - 1);
                                        students[student_num].password[sizeof(students[student_num].password) - 1] = '\0';
                                        student_num++;

                                        printf("Student enrolled successfully!\n");
                                    }
                                    else
                                    {
                                        printf("Cancelled.\n");
                                    }
                                }

                                if (option == 6)
                                {
                                    system("cls");
                                    printf("==================================\n");
                                    printf("         STUDENT DETAILS           \n");
                                    printf("==================================\n\n");

                                    printf("Showing Details for student ID: %d\n", (student_num>0?students[0].student_id:0));

                                    int found = 0;
                                    for (int k = 0; k < record_num; k++)
                                    {
                                        if (records[k].student_id == students[0].student_id)
                                        {
                                            printf("%s | %s | %.2f | Credits after: %.2f\n",
                                                   records[k].date,
                                                   records[k].activity,
                                                   records[k].amount,
                                                   records[k].credits_after);
                                            found = 1;
                                        }
                                    }
                                    if (!found)
                                    {
                                        printf("No records found for this student.\n");
                                    }
                                }

                                if (option == 5)
                                {
                                    system("cls");
                                    printf("==================================\n");
                                    printf("         ALL RECORDS           \n");
                                    printf("==================================\n\n");

                                    for (int k = 0; k < record_num; k++)
                                    {
                                        printf("%s | %s | %.2f | Credits after: %.2f\n",
                                               records[k].date,
                                               records[k].activity,
                                               records[k].amount,
                                               records[k].credits_after);
                                    }
                                }

                                if (option == 8)
                                {
                                    int y;
                                    char z[50];
                                    system("cls");

                                    printf("==================================\n");
                                    printf("         UPDATE INFO           \n");
                                    printf("==================================\n\n");

                                    printf("What do you want to update?\n\n1. Student name\n2. Password\n");
                                    printf("Enter option: ");
                                    if (scanf("%d", &y) != 1) { getchar(); continue; }
                                    getchar();
                                    if (y == 1)
                                    {
                                        system("cls");
                                        printf("==================================\n");
                                        printf("         UPDATE NAME           \n");
                                        printf("==================================\n\n");

                                        printf("\nCurrent name: %s\n", students[0].name);

                                        printf("Enter new name: ");
                                        fgets(z, sizeof(z), stdin);
                                        z[strcspn(z, "\n")] = 0;

                                        strncpy(students[0].name, z, sizeof(students[0].name)-1);
                                        students[0].name[sizeof(students[0].name)-1] = '\0';
                                        printf("\nStudent name updated to: %s\n", students[0].name);
                                    }

                                    if (y == 2)
                                    {
                                        char xy[20];
                                        printf("==================================\n");
                                        printf("         CHANGE PASSWORD           \n");
                                        printf("==================================\n\n");

                                        printf("Please Enter user's previous password !\n");

                                        if (check_password(&students[0]))
                                        {
                                            printf("Enter new password: ");
                                            fgets(xy, sizeof(xy), stdin);
                                            xy[strcspn(xy, "\n")] = 0;

                                            strncpy(students[0].password, xy, sizeof(students[0].password)-1);
                                            students[0].password[sizeof(students[0].password)-1] = '\0';
                                            system("cls");
                                            printf("PASSWORD UPDATED SUCCESSFULLY!!\n\n");
                                            goto start;
                                        }
                                        else
                                        {
                                            printf("Password verification failed.\n");
                                        }
                                    }
                                }

                                if (option == 11)
                                {
                                    system("cls");
                                    goto start;
                                }
                            }
                            else
                            {
                                printf("Choose correct option mentioned above !!\n");
                            }

                            printf("\nPress Enter to continue...");
                            getchar();
                            system("cls");
                        }
                    }
                    else
                    {
                        printf("Incorrect password.\n");
                        goto end;
                    }
                    goto end;
                }
            }

            if (!found)
            {
                system("cls");
                printf("Admin ID not found.\n");
                pause_console();
                goto start;
            }
        }
    }
    else
    {
        printf("Invalid login type. Please enter 'Admin' or 'Student'.\n");
    }

end:
    {
        // save students
        FILE *out = fopen("..\\students.txt", "w");
        if (out)
        {
            for (int i = 0; i < student_num; i++)
            {
                fprintf(out, "%d %s %.2f %s\n", students[i].student_id, students[i].name, students[i].credits, students[i].password);
            }
            fclose(out);
        }

        // append new records
        FILE *save_fp = fopen("..\\records.txt", "a");
        if (save_fp)
        {
            for (int i = 0; i < record_num; i++)
            {
                fprintf(save_fp, "%d %s \"%s\" %.2f %.2f\n",
                        records[i].student_id,
                        records[i].date,
                        records[i].activity,
                        records[i].amount,
                        records[i].credits_after);
            }
            fclose(save_fp);
        }

        free(students);
        free(records);
        printf("\nThank you for using the University Management System!\n");
        return 0;
    }
}