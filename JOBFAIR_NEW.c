/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* --- Thiết lập Export cho Python --- */
#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif

/* Define --------------------------------------------------------------------*/
#define MAX_ROW 2
#define MAX_COLUMN 15
#define MAX_ENTERPRISE 30

/* Enum ----------------------------------------------------------------------*/
typedef struct Enterprise
{
    int booth_index;
    char name[50];
    char abbre[10];
    int itemValue;
    int itemWeight;
} Enterprise;
typedef enum Status
{
    Empty,
    Registered
} Status;

typedef enum CommandType
{
    REGISTER,
    ALTER,
    SHOW,
    DELETE,
    QUIT,
    INVALID
} CommandType;

/* --------------------------------------------------------------------------- */
/*                               Begin: Function                               */
/* --------------------------------------------------------------------------- */
void getAbbreviation(char *name, char *abbre)
{
    int c = 0;
    int length = strlen(name);
    abbre[c++] = name[0];
    for (int i = 1; i < length; i++)
    {
        if (i == 50)
            break;
        if (name[i - 1] == ' ')
        {
            abbre[c++] = name[i];
        }
    }
    abbre[c] = '\0';
}
CommandType getCommandType(char *command)
{
    char w[10];
    int i = 0;
    while (command[i] != ' ' && command[i] != '\0' && i < 9)
    {
        if (command[i] > 96)
        {
            w[i] = command[i] - 32;
        }
        else
        {
            w[i] = command[i];
        }
        i++;
    }
    w[i] = '\0';
    if (strcmp(w, "REGISTER") == 0)
    {
        return REGISTER;
    }
    else if (strcmp(w, "ALTER") == 0)
    {
        return ALTER;
    }
    else if (strcmp(w, "SHOW") == 0)
    {
        return SHOW;
    }
    else if (strcmp(w, "DELETE") == 0)
    {
        return DELETE;
    }
    else if (strcmp(w, "QUIT") == 0)
    {
        return QUIT;
    }
    else
    {
        return INVALID;
    }
}

void printEnterpriseDetails(Enterprise e)
{
    printf("||%d||%s||%s||%d||%d||", e.booth_index, e.name, e.abbre, e.itemValue, e.itemWeight);
    return;
}
void createEnterprise(Enterprise *e, int booth_index, int itemValue, int itemWeight, char *name, char *abbre)
{
    if (e == NULL)
        e = (Enterprise *)malloc(sizeof(Enterprise));
    e->booth_index = booth_index;
    e->itemValue = itemValue;
    e->itemWeight = itemWeight;
    strcpy(e->name, name);
    strcpy(e->abbre, abbre);
    return;
}
void registerEnterprise(int map[MAX_ROW][MAX_COLUMN], Enterprise enterpriseArray[MAX_ENTERPRISE],
                        char *name, int booth, int itemValue, int itemWeight, int *out_booth, char *out_abbre)
{
    char abbre[10];
    getAbbreviation(name, abbre);
    int num_abbre = strlen(abbre);
    int index = booth;
    if (booth == -1)
        index = (num_abbre * 30) % 26;

    for (int j = index; j < 15; j++)
    {
        if (map[0][j] != 1)
        {
            index = j;
            map[0][index] = 1;
            booth = index;
            strcpy(out_abbre, abbre);
            int count = 0;
            while (count < MAX_ENTERPRISE && enterpriseArray[count].booth_index != -1)
                count++;
            Enterprise *enterpriseArray_ptr = &enterpriseArray[count];
            createEnterprise(enterpriseArray_ptr, index, itemValue, itemWeight, name, out_abbre);
            *out_booth = index + 200;
            return;
        }
    }
    for (int j = index; j < 30; j++)
    {
        if (map[1][j - 15] != 1)
        {
            index = j;
            map[1][index - 15] = 1;
            booth = index;
            strcpy(out_abbre, abbre);
            int count = 0;
            while (count < MAX_ENTERPRISE && enterpriseArray[count].booth_index != -1)
                count++;
            Enterprise *enterpriseArray_ptr = &enterpriseArray[count];
            createEnterprise(enterpriseArray_ptr, index, itemValue, itemWeight, name, out_abbre);
            *out_booth = index + 200;
            return;
        }
    }

    for (int j = index; j >= 15; j--)
    {
        if (map[1][j - 15] != 1)
        {
            index = j;
            map[0][index - 15] = 1;
            booth = index;
            strcpy(out_abbre, abbre);
            int count = 0;
            while (count < MAX_ENTERPRISE && enterpriseArray[count].booth_index != -1)
                count++;
            Enterprise *enterpriseArray_ptr = &enterpriseArray[count];
            createEnterprise(enterpriseArray_ptr, index, itemValue, itemWeight, name, out_abbre);
            *out_booth = index + 200;
            return;
        }
    }
    for (int j = index; j >= 0; j--)
    {
        if (map[0][j] != 1)
        {
            index = j;
            map[1][index] = 1;
            booth = index;
            strcpy(out_abbre, abbre);
            int count = 0;
            while (count < MAX_ENTERPRISE && enterpriseArray[count].booth_index != -1)
                count++;
            Enterprise *enterpriseArray_ptr = &enterpriseArray[count];
            createEnterprise(enterpriseArray_ptr, index, itemValue, itemWeight, name, out_abbre);
            *out_booth = index + 200;
            return;
        }
    }
    *out_booth = index + 100;
    return;
}
void showMap(int map[MAX_ROW][MAX_COLUMN], Enterprise enterpriseArray[MAX_ENTERPRISE])
{
    for (int i = 0; i < 15; i++)
        printf("|%d", i);
    printf("|\n");
    for (int i = 0; i < 15; i++)
        printf("|%d", map[0][i]);
    printf("|\n");
    for (int i = 15; i < 30; i++)
        printf("|%d", map[1][i - 15]);
    printf("|");
    return;
}
void showIndexOfStatus(int map[MAX_ROW][MAX_COLUMN], Status status)
{
    if (status == 0)
    {
        printf("Index[");
        int dauphay = 0;
        for (int i = 0; i < 15; i++)
        {
            if (map[0][i] == 0)
            {
                if (dauphay > 0)
                    printf(", ");
                printf("%d", i);
                dauphay++;
            }
        }
        for (int i = 15; i < 30; i++)
        {
            if (map[1][i - 15] == 0)
            {
                if (dauphay > 0)
                    printf(", ");
                printf("%d", i);
                dauphay++;
            }
        }
        printf("]");
    }

    if (status == 1)
    {
        printf("Index[");
        int dauphay = 0;
        for (int i = 0; i < 15; i++)
        {
            if (map[0][i] == 1)
            {
                if (dauphay > 0)
                    printf(", ");
                printf("%d", i);
                dauphay++;
            }
        }
        for (int i = 15; i < 30; i++)
        {
            if (map[1][i - 15] == 1)
            {
                if (dauphay > 0)
                    printf(", ");
                printf("%d", i);
                dauphay++;
            }
        }
        printf("]");
    }
    return;
}
void showTotalOfStatus(Enterprise enterpriseArray[MAX_ENTERPRISE], Status status)
{
    if (status == 0)
    {
        int dem = 0;
        for (int i = 0; i < 15; i++)
        {
            if (enterpriseArray[i].booth_index == 0)
                dem++;
        }
        for (int i = 15; i < 30; i++)
        {
            if (enterpriseArray[i].booth_index == 0)
                dem++;
        }
        printf("Total: %d", dem);
    }
    if (status == 1)
    {
        int dem = 0;
        for (int i = 0; i < 15; i++)
        {
            if (enterpriseArray[i].booth_index == 1)
                dem++;
        }
        for (int i = 0; i < 30; i++)
        {
            if (enterpriseArray[i].booth_index == 1)
                dem++;
        }
        printf("Total: %d", dem);
    }
}
void showIndexBracket(int map[MAX_ROW][MAX_COLUMN], Enterprise enterpriseArray[MAX_ENTERPRISE], int start, int end)
{
    if (start == end)
    {
        int x = start / 15;
        int y = start % 15;
        if (map[x][y])
        {
            int c = 0;
            while (c < MAX_ENTERPRISE && enterpriseArray[c].booth_index != start)
                c++;
            if (c == MAX_ENTERPRISE || enterpriseArray[c].booth_index != start)
            {
                printf("NONE\n");
                return;
            }
            printEnterpriseDetails(enterpriseArray[c]);
            printf("\n");
            return;
        }
        printf("NONE\n");
        return;
    }
    for (int i = start; i <= end; i++)
    {
        int x = i / 15;
        int y = i % 15;
        printf("Index %d: ", i);
        if (map[x][y])
        {
            int c = 0;
            while (c < MAX_ENTERPRISE && enterpriseArray[c].booth_index != i)
                c++;
            if (c == MAX_ENTERPRISE || enterpriseArray[c].booth_index != i)
            {
                printf("NONE\n");
                return;
            }
            printEnterpriseDetails(enterpriseArray[c]);
            printf("\n");
        }
        else
            printf("NONE\n");
    }
}
void handleShowCommand(int map[MAX_ROW][MAX_COLUMN], Enterprise enterpriseArray[MAX_ENTERPRISE], char *command)
{
    if (strcmp(command, "Show map") == 0)
    {
        showMap(map, enterpriseArray);
        return;
    }
    if (strcmp(command, "Show &0") == 0)
    {
        showIndexOfStatus(map, 0);
        return;
    }
    if (strcmp(command, "Show &1") == 0)
    {
        showIndexOfStatus(map, 1);
        return;
    }
    if (strcmp(command, "Show #0") == 0)
    {
        showTotalOfStatus(enterpriseArray, 0);
        return;
    }
    if (strcmp(command, "Show #1") == 0)
    {
        showTotalOfStatus(enterpriseArray, 1);
        return;
    }

    for (int i = 0; i < 30; i++)
    {
        char buffer[20];

        // Format for "Show [i]"
        sprintf(buffer, "Show [%d]", i);
        if (strcmp(command, buffer) == 0)
        {
            showIndexBracket(map, enterpriseArray, i, i);
            return;
        }

        // Format for "Show [i:-1]"
        sprintf(buffer, "Show [%d:-1]", i);
        if (strcmp(command, buffer) == 0)
        {
            showIndexBracket(map, enterpriseArray, i, i);
            return;
        }

        // Loop for range commands "Show [i:j]"
        bool found = false;
        for (int j = i; j < 30; j++)
        {
            sprintf(buffer, "Show [%d:%d]", i, j);
            if (strcmp(command, buffer) == 0)
            {
                showIndexBracket(map, enterpriseArray, i, j);
                found = true;
                break;
            }
        }
        if (found)
            return;
    }
}
void alterEnterprise(int map[MAX_ROW][MAX_COLUMN], Enterprise enterpriseArray[MAX_ENTERPRISE],
                     char *abbre, int registerBooth, int newBooth, int *out_booth, char *out_abbre)
{
    int eIndex = 0;
    while (enterpriseArray[eIndex].booth_index != registerBooth && eIndex < 30)
        eIndex++;
    if (newBooth == -1)
    {
        int x = registerBooth / 15;
        int y;
        if (registerBooth < 15)
            y = registerBooth;
        else
            y = registerBooth - 15;
        int tx = x, ty = y;
        while (map[x][y] == 1)
        {
            y++;
            if (y == 15)
            {
                x++;
                y = 0;
            }
            if (x == 2)
                break;
        }
        if (x != 2)
        {
            int index = x * 15 + y;
            map[x][y] = 1;
            map[tx][ty] = 0;
            enterpriseArray[eIndex].booth_index = index;
            *out_booth = enterpriseArray[eIndex].booth_index + 200;
            *out_abbre = *abbre;
            return;
        }
        x = tx;
        y = ty;
        while (map[x][y] == 1)
        {
            y--;
            if (y == -1)
            {
                x--;
                y = 14;
            }
            if (x == -1)
                break;
        }
        if (x != -1)
        {
            int index = x * 15 + y;
            map[x][y] = 1;
            map[tx][ty] = 0;
            enterpriseArray[eIndex].booth_index = index;
            *out_booth = enterpriseArray[eIndex].booth_index + 200;
            *out_abbre = *abbre;
            return;
        }
        *out_booth = enterpriseArray[eIndex].booth_index + 200;
        return;
    }
    if (newBooth != -1)
    {
        int x = registerBooth / 15;
        int y;
        if (registerBooth < 15)
            y = registerBooth;
        else
            y = registerBooth - 15;
        int tx = x, ty = y;
        x = newBooth / 15;
        if (newBooth < 15)
            y = newBooth;
        else
            y = newBooth - 15;
        if (map[x][y] == 1)
        {
            *out_booth = enterpriseArray[eIndex].booth_index + 100;
            return;
        }
        int index = x * 15 + y;
        map[x][y] = 1;
        map[tx][ty] = 0;
        enterpriseArray[eIndex].booth_index = index;
        *out_booth = enterpriseArray[eIndex].booth_index + 200;
        *out_abbre = *abbre;
        return;
    }
}
void deleteEnterprise(int map[MAX_ROW][MAX_COLUMN], Enterprise enterpriseArray[MAX_ENTERPRISE], char *abbre, int booth, int *totalEmpty)
{
    int x = booth / 15;
    int y;
    if (booth < 15)
        y = booth;
    else
        y = booth - 15;
    map[x][y] = 0;
    for (int i = 0; i < 30; i++)
    {
        if (strcmp(enterpriseArray[i].abbre, abbre) == 0 && enterpriseArray[i].booth_index == booth)
        {
            enterpriseArray[i].booth_index = 0;
            enterpriseArray[i].itemValue = 0;
            enterpriseArray[i].itemWeight = 0;
            strcpy(enterpriseArray[i].name, "");
            strcpy(enterpriseArray[i].abbre, "");
            break;
        }
    }
    *totalEmpty = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (map[i][j] == 0)
            {
                (*totalEmpty)++;
            }
        }
    }
    return;
}
void handleCommand(char *command, int map[MAX_ROW][MAX_COLUMN], Enterprise enterpriseArray[MAX_ENTERPRISE], CommandType *commandType)
{
    getCommandType(command);
    if (getCommandType(command) == REGISTER)
    {
        int booth = 0;
        int itemValue = 0;
        int itemWeight = 0;
        char name[50] = "";
        int out_booth = 0;
        char out_abbre[10] = "";
        int dem = 0;
        char booth_num[5] = "";
        char value_num[5] = "";
        char weight_num[5] = "";
        int j = 0, k = 0, l = 0, m = 0;
        while (dem < strlen(command))
        {
            if (command[dem] == '[')
            {
                dem++;
                if (strcmp(name, "") == 0)
                {
                    while (command[dem] != ']')
                    {
                        name[j] = command[dem];
                        j++;
                        dem++;
                    }
                    name[j] = '\0';
                    continue;
                }
                if (strcmp(booth_num, "") == 0)
                {
                    while (command[dem] != ']')
                    {
                        booth_num[k] = command[dem];
                        k++;
                        dem++;
                    }
                    booth_num[k] = '\0';
                    continue;
                }
                if (strcmp(value_num, "") == 0)
                {
                    while (command[dem] != ']')
                    {
                        value_num[l] = command[dem];
                        l++;
                        dem++;
                    }
                    booth_num[k] = '\0';
                    continue;
                }
                if (strcmp(weight_num, "") == 0)
                {
                    while (command[dem] != ']')
                    {
                        weight_num[m] = command[dem];
                        m++;
                        dem++;
                    }
                    booth_num[k] = '\0';
                    continue;
                }
            }
            dem++;
        }
        if (booth == 0)
        {
            if (strcmp(weight_num, "") == 0)
                booth = -1;
            else
                booth = atoi(booth_num);
        }
        if (itemValue == 0)
        {
            if (strcmp(weight_num, "") == 0)
                itemValue = atoi(booth_num);
            else
                itemValue = atoi(value_num);
        }
        if (itemWeight == 0)
        {
            if (strcmp(weight_num, "") == 0)
                itemWeight = atoi(value_num);
            else
                itemWeight = atoi(weight_num);
        }
        registerEnterprise(map, enterpriseArray,
                           name, booth, itemValue, itemWeight, &out_booth, out_abbre);
        *commandType = REGISTER;
        return;
    }
    if (getCommandType(command) == ALTER)
    {
        char abbre[10] = "";
        int registerBooth = 0;
        int newBooth = 0;
        int out_booth = 0;
        char out_abbre[10] = "";
        int dem = 0;
        int j = 0, k = 0, l = 0;
        char register_num[5] = "";
        char newBooth_num[5] = "";
        while (dem < strlen(command))
        {
            if (command[dem] == '[')
            {
                dem++;
                if (strcmp(abbre, "") == 0)
                {
                    while (command[dem] != ']')
                    {
                        abbre[j] = command[dem];
                        j++;
                        dem++;
                    }
                    abbre[j] = '\0';
                    continue;
                }
                if (strcmp(register_num, "") == 0)
                {
                    while (command[dem] != ']')
                    {
                        register_num[k] = command[dem];
                        k++;
                        dem++;
                    }
                    register_num[k] = '\0';
                    continue;
                }
                if (strcmp(newBooth_num, "") == 0)
                {
                    while (command[dem] != ']')
                    {
                        newBooth_num[l] = command[dem];
                        l++;
                        dem++;
                    }
                    newBooth_num[l] = '\0';
                    continue;
                }
            }
            dem++;
        }
        if (registerBooth == 0)
        {
            registerBooth = atoi(register_num);
        }
        if (newBooth == 0)
        {
            if (strcmp(newBooth_num, "") == 0)
                newBooth = -1;
            else
                newBooth = atoi(newBooth_num);
        }
        alterEnterprise(map, enterpriseArray,
                        abbre, registerBooth, newBooth, &out_booth, out_abbre);
        *commandType = ALTER;
        return;
    }
    if (getCommandType(command) == SHOW)
    {
        handleShowCommand(map, enterpriseArray, command);
        *commandType = SHOW;
        return;
    }
    if (getCommandType(command) == DELETE)
    {
        char abbre[10] = "";
        int booth = 0;
        int totalEmpty = 0;
        int dem = 0;
        int j = 0, k = 0;
        char num[2] = "";
        while (dem < strlen(command))
        {
            if (command[dem] == '[')
            {
                dem++;
                if (strcmp(abbre, "") == 0)
                {
                    while (command[dem] != ']')
                    {
                        abbre[j] = command[dem];
                        j++;
                        dem++;
                    }
                    continue;
                }
                while (command[dem] != ']')
                {
                    num[k] = command[dem];
                    k++;
                    dem++;
                }
            }
            dem++;
        }
        if (booth == 0)
        {
            booth = atoi(num);
        }
        deleteEnterprise(map, enterpriseArray, abbre, booth, &totalEmpty);
        *commandType = DELETE;
        return;
    }
    if (getCommandType(command) == QUIT)
    {
        *commandType = QUIT;
        return;
    }
    if (getCommandType(command) == INVALID)
    {
        *commandType = INVALID;
        return;
    }
    return;
}
int knapsack(int map[MAX_ROW][MAX_COLUMN], Enterprise enterpriseArray[MAX_ENTERPRISE],
			 int maxWeight, int numOfEnterprises, int index)
{
    // Điều kiện dừng: Hết gian hàng hoặc hết khối lượng chứa
    if (index == numOfEnterprises || maxWeight <= 0)
    {
        return 0;
    }

    // Trường hợp không chọn vật phẩm từ doanh nghiệp hiện tại
    int valueWithoutCurrent = knapsack(map, enterpriseArray, maxWeight, numOfEnterprises, index + 1);

    // Trường hợp chọn vật phẩm từ doanh nghiệp hiện tại (nếu khối lượng cho phép)
    int valueWithCurrent = 0;
    if (enterpriseArray[index].itemWeight <= maxWeight)
    {
        valueWithCurrent = enterpriseArray[index].itemValue +
                           knapsack(map, enterpriseArray, maxWeight - enterpriseArray[index].itemWeight, numOfEnterprises, index + 1);
    }

    // So sánh hai kết quả:
    // Ưu tiên giá trị cao hơn; nếu giá trị bằng nhau, ưu tiên khối lượng thấp hơn.
    if (valueWithCurrent > valueWithoutCurrent)
    {
        return valueWithCurrent;
    }
    else if (valueWithCurrent == valueWithoutCurrent)
    {
        // Tính khối lượng của hai trường hợp nếu giá trị bằng nhau
        int weightWithoutCurrent = 0;
        int weightWithCurrent = enterpriseArray[index].itemWeight;

        for (int i = index + 1; i < numOfEnterprises; i++)
        {
            if (enterpriseArray[i].itemWeight <= maxWeight - enterpriseArray[index].itemWeight)
            {
                weightWithCurrent += enterpriseArray[i].itemWeight;
            }
            if (enterpriseArray[i].itemWeight <= maxWeight)
            {
                weightWithoutCurrent += enterpriseArray[i].itemWeight;
            }
        }

        // Ưu tiên khối lượng thấp hơn
        if (weightWithCurrent < weightWithoutCurrent)
        {
            return valueWithCurrent;
        }
    }

    return valueWithoutCurrent;
}

/* --------------------------------------------------------------------------- */
/*                             End: Function                                   */
/* --------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------- */
/*                           Example Test Cases for Main                        */
/* --------------------------------------------------------------------------- */

// Initializes booth map and enterprise array to default values and then
// runs a series of test cases to verify enterprise registration, booth
// assignments, alterations, and knapsack optimization functionality.

// Initialize the booth map with default status Empty
void initMap(int map[MAX_ROW][MAX_COLUMN])
{
    for (int i = 0; i < MAX_ROW; i++)
    {
        for (int j = 0; j < MAX_COLUMN; j++)
        {
            map[i][j] = Empty;
        }
    }
}

// Initialize the enterprise array with default values
void initEnterpriseArray(Enterprise enterpriseArray[MAX_ENTERPRISE])
{
    for (int i = 0; i < MAX_ENTERPRISE; i++)
    {
        enterpriseArray[i].booth_index = -1;
        strcpy(enterpriseArray[i].name, "");
        strcpy(enterpriseArray[i].abbre, "");
        enterpriseArray[i].itemValue = 0;
        enterpriseArray[i].itemWeight = 0;
    }
}

/*-Bổ sung-*/
int map[MAX_ROW][MAX_COLUMN];
    Enterprise enterpriseArray[MAX_ENTERPRISE];

/* --- CÁC HÀM EXPORT CHO PYTHON --- */

EXPORT void init_system() {
    for (int i = 0; i < MAX_ROW; i++)
        for (int j = 0; j < MAX_COLUMN; j++)
            map[i][j] = 0;

    for (int i = 0; i < MAX_ENTERPRISE; i++) {
        enterpriseArray[i].booth_index = -1;
        strcpy(enterpriseArray[i].name, "");
    }
}

EXPORT int register_enterprise_gui(char* name, int booth, int val, int weight) {
    char abbre[10];
    getAbbreviation(name, abbre);
    
    // Nếu booth == -1, tính hash theo logic của bạn
    int target = (booth == -1) ? (strlen(abbre) * 30) % 26 : booth;
    
    // Tìm vị trí trống (đơn giản hóa để GUI dễ quản lý)
    for (int i = 0; i < 30; i++) {
        int idx = (target + i) % 30;
        int r = idx / 15;
        int c = idx % 15;
        
        if (map[r][c] == 0) {
            map[r][c] = 1;
            enterpriseArray[idx].booth_index = idx;
            enterpriseArray[idx].itemValue = val;
            enterpriseArray[idx].itemWeight = weight;
            strcpy(enterpriseArray[idx].name, name);
            strcpy(enterpriseArray[idx].abbre, abbre);
            return idx; // Trả về vị trí đã đăng ký thành công
        }
    }
    return -1; // Hết chỗ
}

EXPORT int run_optimization(int maxWeight) {
    return knapsack(map, enterpriseArray, maxWeight, 30, 0);
}

EXPORT const char* get_abbre(int index) {
    return enterpriseArray[index].abbre;
}

int main()
{    
    initMap(map);
    char *command_name[] = {"REGISTER", "ALTER", "SHOW", "DELETE", "QUIT", "INVALID"};
    initEnterpriseArray(enterpriseArray);
    printf("----- Sample testcase 05 -----\n");
    printf("Test RegisterCommand:\n");
    int *out_booth = malloc(sizeof(int));
    char *out_abbre = malloc(10);
    registerEnterprise(map, enterpriseArray, "Tech Co", -1, 4, 2, out_booth, out_abbre);
    registerEnterprise(map, enterpriseArray, "Innovative Solutions Company", 10, 3, 4, out_booth, out_abbre);
    registerEnterprise(map, enterpriseArray, "EMLP Technology and Solutions Limted Company", -1, 1, 3, out_booth, out_abbre);
    registerEnterprise(map, enterpriseArray, "NextGen", 12, 3, 4, out_booth, out_abbre);
    char *command = "Register [Minh Entertainment] [3] [4] [2]";
    CommandType *commandType;
    handleCommand(command, map, enterpriseArray, commandType);
    showIndexBracket(map, enterpriseArray, 0, 30);
    char *command1 = "Show map";
    handleShowCommand(map, enterpriseArray, command1);
    printf("\n%d" ,knapsack(map, enterpriseArray, 10, 5, 0));
    return 0;
}