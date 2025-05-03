#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 4096

// Fungsi untuk membalik string
void reverse(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmp;
    }
}

// Fungsi untuk mengubah biner menjadi string heksadesimal
void bytes_to_hex(const unsigned char *data, size_t len, char *hex_out) {
    for (size_t i = 0; i < len; i++) {
        sprintf(hex_out + (i * 2), "%02x", data[i]);
    }
    hex_out[len * 2] = '\0';
}

void upload_file() {
    char filename[256];
    printf("Masukkan nama file TXT: ");
    scanf("%s", filename);

    char path[300];
    snprintf(path, sizeof(path), "secrets/%s", filename);

    FILE *f = fopen(path, "rb");
    if (!f) {
        perror("Gagal membuka file dari folder secrets/");
        return;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char *binary_data = malloc(fsize);
    if (!binary_data) {
        perror("Gagal alokasi memori");
        fclose(f);
        return;
    }

    fread(binary_data, 1, fsize, f);
    fclose(f);

    // Convert to hex
    char *hex_data = malloc(fsize * 2 + 1);
    if (!hex_data) {
        perror("Gagal alokasi memori untuk hex");
        free(binary_data);
        return;
    }
    bytes_to_hex(binary_data, fsize, hex_data);
    free(binary_data);

    // Reverse hex string
    reverse(hex_data);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket error");
        free(hex_data);
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Gagal connect ke server");
        free(hex_data);
        close(sock);
        return;
    }

    // Compose request
    size_t req_len = 7 + strlen(filename) + 1 + strlen(hex_data) + 1;
    char *request = malloc(req_len);
    if (!request) {
        perror("Gagal alokasi memori untuk request");
        free(hex_data);
        close(sock);
        return;
    }
    snprintf(request, req_len, "UPLOAD %s\n%s", filename, hex_data);
    free(hex_data);

    send(sock, request, strlen(request), 0);
    free(request);

    char response[BUFFER_SIZE];
    int r = recv(sock, response, sizeof(response) - 1, 0);
    if (r > 0) {
        response[r] = '\0';
        printf("Respon server: %s\n", response);
    } else if (r == 0) {
        printf("Koneksi server ditutup.\n");
    } else {
        perror("Gagal menerima respon dari server");
    }

    close(sock);
}

void download_file() {
    char filename[256];
    printf("Masukkan nama file JPEG: ");
    scanf("%s", filename);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket error");
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Gagal connect ke server");
        close(sock);
        return;
    }

    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request), "DOWNLOAD %s\n", filename);
    send(sock, request, strlen(request), 0);

    char initial_response[16];
    int ir = recv(sock, initial_response, sizeof(initial_response) - 1, 0);
    if (ir <= 0) {
        perror("Gagal menerima respon dari server");
        close(sock);
        return;
    }
    initial_response[ir] = '\0';

    if (strncmp(initial_response, "SUCCESS", 7) != 0) {
        printf("Respon server: %s\n", initial_response);
        close(sock);
        return;
    }

    FILE *out = fopen(filename, "wb");
    if (!out) {
        perror("Gagal menyimpan file");
        close(sock);
        return;
    }

    unsigned char data[BUFFER_SIZE];
    int total_received = 0;
    int bytes_received;

    while ((bytes_received = recv(sock, data, sizeof(data), 0)) > 0) {
        fwrite(data, 1, bytes_received, out);
        total_received += bytes_received;
    }
    fclose(out);

    if (bytes_received < 0) {
        perror("Gagal menerima data dari server");
        remove(filename);
    } else if (total_received == 0) {
        printf("Tidak ada data file diterima (mungkin file tidak ditemukan di server).\n");
        remove(filename);
    } else {
        printf("File berhasil disimpan di: %s (%d bytes diterima)\n", filename, total_received);
    }

    close(sock);
}

int main() {
    while (1) {
        printf("=============================\n");
        printf("| Welcome to Pencari Gambar |\n");
        printf("=============================\n");
        printf("1. Upload file rahasia\n");
        printf("2. Download file hasil dekripsi\n");
        printf("3. Keluar\n");
        printf("Pilihan: ");

        int pilihan;
        if (scanf("%d", &pilihan) != 1) {
            while (getchar() != '\n');
            printf("Input tidak valid. Masukkan angka.\n");
            continue;
        }

        switch (pilihan) {
            case 1:
                upload_file();
                break;
            case 2:
                download_file();
                break;
            case 3:
                printf("Keluar dari program.\n");
                exit(0);
            default:
                printf("Pilihan tidak valid!\n");
        }
    }

    return 0;
}
