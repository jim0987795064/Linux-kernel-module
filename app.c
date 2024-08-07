#include"app.h"

int main()
{

    userinput();

    return 0;

}
void userinput()
{

    int startRow = 0, endRow = 0;

    while(1)
    {
        char userInput, end_of_line;
        printf("%s%s", "Which information do you want?\n",
               "Version(v),CPU(c),Memory(m),Time(t),All(a),Exit(e)?\n");
        scanf("%s", &userInput);
        scanf("%c", &end_of_line);
        if(userInput == 'v')
        {
            startRow = 1;
            endRow = 1;
            printf("%s", "\nVersion: ");
            information(startRow, endRow, 'V');
        }
        else if(userInput == 'c')
        {
            startRow = 4;
            endRow = 11;
            printf("%s", "\nCpu information:\n");
            information(startRow, endRow, 'C');
        }
        else if(userInput == 'm')
        {
            startRow = 15;
            endRow = 24;
            printf("%s", "\nMemory information:\n");
            information(startRow, endRow, 'M');
        }
        else if(userInput == 't')
        {
            startRow = 28;
            endRow = 29;
            printf("%s", "\nTime information:\n");
            information(startRow, endRow, 'T');
        }
        else if(userInput == 'a')
        {
            startRow = 0;
            endRow = 29;
            information(startRow, endRow, 'a');
        }
        else if(userInput == 'e')
            return;



        printf("%s", "\n--------------------------------------\n\n");

    }

}
void information(int startRow, int endRow, char userInput)
{
    int row = 0, buffer_length = 0, count = 0;
    char buffer[100];
    bool status = false;
    FILE *fp = fopen("/proc/my_info", "r");
    if(fp != NULL && userInput != 'a')
    {

        while(fgets(buffer,100,fp)!= NULL)
        {
            if(buffer[0] == '=' && buffer[10] == userInput)
                status = true;
            if(buffer[0] == '=' && buffer[10] != userInput)
                status = false;
            if(status == true && buffer[0] != '=')
                printf("%s", buffer);
        }

    }
    if(userInput == 'a')
    {
        while(fgets(buffer,100,fp)!= NULL)
            printf("%s", buffer);
    }
    fclose(fp);
    return;
}