#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define MAX_DATA 2048

#include <stdio.h>

// ANSI color codes
#define RESET   "\x1b[0m"
#define WHITE   "\x1b[97m"
#define CYAN    "\x1b[96m"
#define YELLOW  "\x1b[93m"
#define GREEN   "\x1b[92m"
#define MAGENTA "\x1b[95m"
#define RED     "\x1b[91m"

void print_main_menu() {
    printf("\n");
    printf(WHITE "╔══════════════════════════════════════╗\n" RESET);
    printf(WHITE "║" CYAN "      🕯️  DUNGEON MAIN MENU   🕯️      " WHITE "║\n" RESET);
    printf(WHITE "╠══════════════════════════════════════╣\n" RESET);
    printf(WHITE "║" YELLOW " 1. 📖 Examine Hero Status            " WHITE "║\n");
    printf(WHITE "║" GREEN  " 2. 🪙 Visit the Blacksmith           " WHITE "║\n");
    printf(WHITE "║" CYAN   " 3. 🎒 Manage Inventory & Gear        " WHITE "║\n");
    printf(WHITE "║" RED    " 4. ⚔️  Enter Battle Arena            " WHITE "║\n");
    printf(WHITE "║" MAGENTA " 5. 🚪 Leave the Dungeon              " WHITE "║\n");
    printf(WHITE "╚══════════════════════════════════════╝\n" RESET);
    printf(CYAN ">> Choose your fate, adventurer: " RESET);
}


int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_DATA];
    int choice;

    while (1) {
        print_main_menu();
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1: {
                // Send command
                const char* cmd = "SHOW_STATS";
                send(sock, cmd, strlen(cmd), 0);

                // Receive response
                memset(buffer, 0, MAX_DATA);
                recv(sock, buffer, MAX_DATA, 0);
                printf("\n%s\n", buffer);
                break;
            }

            case 2: {
                // Send command to enter shop
                const char* cmd = "SHOP";
                send(sock, cmd, strlen(cmd), 0);

                // Receive shop display
                memset(buffer, 0, MAX_DATA);
                recv(sock, buffer, MAX_DATA, 0);
                printf("\n%s", buffer);

                // Read weapon ID input
                int weapon_id;
                scanf("%d", &weapon_id);
                getchar();

                // Send weapon ID
                char id_str[16];
                snprintf(id_str, sizeof(id_str), "%d", weapon_id);
                send(sock, id_str, strlen(id_str), 0);

                // Receive confirmation
                memset(buffer, 0, MAX_DATA);
                recv(sock, buffer, MAX_DATA, 0);
                printf("%s\n", buffer);
                break;
            }

            case 3: {
                // Send command to view inventory
                const char* cmd = "INVENTORY";
                send(sock, cmd, strlen(cmd), 0);

                // Receive inventory list
                memset(buffer, 0, MAX_DATA);
                recv(sock, buffer, MAX_DATA, 0);
                printf("\n%s", buffer);

                // Read weapon index input
                int weapon_idx;
                printf("Enter weapon index to equip (or invalid number to cancel): ");
                if (scanf("%d", &weapon_idx) == 1) {
                    getchar(); // consume newline
                    char idx_str[8];
                    snprintf(idx_str, sizeof(idx_str), "%d", weapon_idx);
                    send(sock, idx_str, strlen(idx_str), 0);

                    // Receive equip confirmation
                    memset(buffer, 0, MAX_DATA);
                    recv(sock, buffer, MAX_DATA, 0);
                    printf("%s\n", buffer);
                } else {
                    getchar(); // consume invalid input
                    printf("Invalid input.\n");
                }
                break;
            }

            case 4: {
                const char* cmd = "BATTLE";
                send(sock, cmd, strlen(cmd), 0);
            
                while (1) {
                    // tunggu battle message server
                    memset(buffer, 0, MAX_DATA);
                    int valread = recv(sock, buffer, MAX_DATA - 1, 0);
                    if (valread <= 0) {
                        printf("Connection lost!\n");
                        break;
                    }
                    buffer[valread] = '\0'; // Null-terminate
            
                    printf("\n%s\n", buffer);
            
                    // Wait for input
                    printf("> ");
                    char battle_cmd[MAX_DATA];
                    fgets(battle_cmd, sizeof(battle_cmd), stdin);
                    battle_cmd[strcspn(battle_cmd, "\n")] = '\0'; // Remove newline
            
                    // Send input to server
                    send(sock, battle_cmd, strlen(battle_cmd), 0);
            
                    // Exit battle loop kalau ketik 'exit'
                    if (strcmp(battle_cmd, "exit") == 0) {
                        break;
                    }
                }
            
                break;
            }

            case 5:
                printf("Goodbye !\n");
                close(sock);
                exit(0);

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}