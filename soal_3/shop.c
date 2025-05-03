// shop.c

#include <stdio.h>
#include <string.h>

#define MAX_PASSIVE 2
#define SHOP_SIZE 5
#define BUFFER_SIZE 2048

#define RESET   "\x1b[0m"
#define WHITE   "\x1b[97m"
#define YELLOW  "\x1b[93m"
#define GREEN   "\x1b[92m"
#define CYAN    "\x1b[96m"
#define MAGENTA "\x1b[95m"
#define RED     "\x1b[91m"

typedef struct {
    int id;
    char name[32];
    int price;
    int damage;
    char passive[MAX_PASSIVE][64];
} Weapon;

// List senjata
static Weapon weapon_shop[SHOP_SIZE];

// Inisialisasi toko
void init_shop() {
    strcpy(weapon_shop[0].name, "Rusty Sword");
    weapon_shop[0].id = 1;
    weapon_shop[0].price = 50;
    weapon_shop[0].damage = 10;
    strcpy(weapon_shop[0].passive[0], "Bleed chance");
    strcpy(weapon_shop[0].passive[1], "Low durability");

    strcpy(weapon_shop[1].name, "Battle Axe");
    weapon_shop[1].id = 2;
    weapon_shop[1].price = 100;
    weapon_shop[1].damage = 20;
    strcpy(weapon_shop[1].passive[0], "Bleed chance");
    strcpy(weapon_shop[1].passive[1], "Slow attack");

    strcpy(weapon_shop[2].name, "Fire Wand");
    weapon_shop[2].id = 3;
    weapon_shop[2].price = 120;
    weapon_shop[2].damage = 15;
    strcpy(weapon_shop[2].passive[0], "Burn effect");
    strcpy(weapon_shop[2].passive[1], "Mana drain");

    strcpy(weapon_shop[3].name, "Twin Daggers");
    weapon_shop[3].id = 4;
    weapon_shop[3].price = 80;
    weapon_shop[3].damage = 12;
    strcpy(weapon_shop[3].passive[0], "Double hit");
    strcpy(weapon_shop[3].passive[1], "Poison");

    strcpy(weapon_shop[4].name, "Staff of Homa");
    weapon_shop[4].id = 5;
    weapon_shop[4].price = 150;
    weapon_shop[4].damage = 25;
    strcpy(weapon_shop[4].passive[0], "Double hit");
    strcpy(weapon_shop[4].passive[1], "Burn effect");
}

// Fungsi untuk menampilkan senjata di shop
void display_weapon_shop(char* buffer) {
    strcpy(buffer, WHITE "╔═══════════════════════[ 🛡️ Weapon Shop 🛡️ ]═══════════════════════╗\n" RESET);
    strcat(buffer, WHITE "║  ID │ Weapon Name       │ Price     │ DMG │ Passives               ║\n" RESET);
    strcat(buffer, WHITE "╠═════╪═══════════════════╪═══════════╪═════╪═════════════════════════╣\n" RESET);

    char line[512];
    for (int i = 0; i < SHOP_SIZE; i++) {
        snprintf(line, sizeof(line),
                 WHITE "║ %2d  │ " YELLOW "%-17s" WHITE " │ " GREEN "%-9d" WHITE " │ " RED "%3d" WHITE " │ " CYAN "%-10s" MAGENTA ", %-10s" WHITE "║\n",
                 weapon_shop[i].id,
                 weapon_shop[i].name,
                 weapon_shop[i].price,
                 weapon_shop[i].damage,
                 weapon_shop[i].passive[0],
                 weapon_shop[i].passive[1]);
        strcat(buffer, line);
    }

    strcat(buffer, WHITE "╚═══════════════════════════════════════════════════════════════════╝\n" RESET);
}

void format_weapon_display(const Weapon* w, char* output, int with_passive) {
    if (with_passive) {
        snprintf(output, 256, "%s (Passive: %s, %s)", w->name, w->passive[0], w->passive[1]);
    } else {
        snprintf(output, 256, "%s", w->name);
    }
}

// Fungsi untuk beli senjata
int get_weapon_by_id(int id, Weapon* result) {
    for (int i = 0; i < SHOP_SIZE; i++) {
        if (weapon_shop[i].id == id) {
            *result = weapon_shop[i];
            return 1;
        }
    }
    return 0;
}



