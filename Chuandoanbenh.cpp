#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "PatientRecord.h"
#define MAX_SYMPTOMS 10
#define MAX_DISEASES 100
#define MAX_PATIENTS 100
#define MAX_LINE_LENGTH 200
#define MAX_PATIENTS 100
struct PatientRecord patients[MAX_PATIENTS]; // M?ng ch?a h? so b?nh nhân
int patient_count = 0;

struct Treatment {
    char disease_name[50];
    char medicines[200];
    char advice[200];
};

// C?u trúc b?nh
struct Disease {
    char name[50];
    char symptoms[MAX_SYMPTOMS][50];
    int symptom_count;
};


// C?u trúc k?t qu? ch?n doán
struct DiagnosisResult {
    char disease_name[50];
    int match_count;
    float confidence;
};

// Hàm chu?n hóa chu?i
void normalize(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void trim(char *str) {
    int i = 0, j = strlen(str) - 1;
    while (isspace(str[i])) i++;
    while (isspace(str[j])) j--;
    for (int k = 0; k <= j - i; k++) {
        str[k] = str[i + k];
    }
    str[j - i + 1] = '\0';
}

void normalize_and_trim(char *str) {
    normalize(str);
    trim(str);
}
int load_treatments_from_file(const char *filename, struct Treatment treatments[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Loi mo file");
        return 0;
    }

    int treatment_count = 0;
    char line[MAX_LINE_LENGTH];
    struct Treatment current_treatment = { "", "", "" };

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) <= 1) continue;

        line[strcspn(line, "\n")] = '\0';
        normalize_and_trim(line);

        if (strncmp(line, "ten benh:", 9) == 0) {
            if (strlen(current_treatment.disease_name) > 0) {
                treatments[treatment_count++] = current_treatment;
                current_treatment = (struct Treatment) { "", "", "" };
            }
            strcpy(current_treatment.disease_name, line + 10);
        } else if (strncmp(line, "thuoc tri:", 10) == 0) {
            strcpy(current_treatment.medicines, line + 11);
        } else if (strncmp(line, "loi khuyen:", 11) == 0) {
            strcpy(current_treatment.advice, line + 12);
        }
    }

    if (strlen(current_treatment.disease_name) > 0) {
        treatments[treatment_count++] = current_treatment;
    }

    fclose(file);
    return treatment_count;
}

// Hàm d?c danh sách b?nh t? file
int load_diseases_from_file(const char *filename, struct Disease diseases[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Loi mo file");
        return 0;
    }

    int disease_count = 0;
    char line[MAX_LINE_LENGTH];
    struct Disease current_disease = { "", {}, 0 };

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) <= 1) continue;

        line[strcspn(line, "\n")] = '\0';
        normalize_and_trim(line);

        if (strncmp(line, "ten benh:", 9) == 0) {
            if (current_disease.symptom_count > 0) {
                diseases[disease_count++] = current_disease;
                current_disease = (struct Disease) { "", {}, 0 };
            }
            strcpy(current_disease.name, line + 10);
            normalize_and_trim(current_disease.name);
        } else if (strncmp(line, "trieu chung:", 12) == 0) {
            continue;
        } else if (isdigit(line[0])) {
            char *symptom = strchr(line, '.');
            if (symptom != NULL) {
                symptom++;
                normalize_and_trim(symptom);
                strcpy(current_disease.symptoms[current_disease.symptom_count++], symptom);
            }
        }
    }

    if (current_disease.symptom_count > 0) {
        diseases[disease_count++] = current_disease;
    }

    fclose(file);
    return disease_count;
}
void find_treatment(const char *disease_name, struct Treatment treatments[], int treatment_count) {
    for (int i = 0; i < treatment_count; i++) {
        if (strcmp(disease_name, treatments[i].disease_name) == 0) {
            printf("%-25s: %s\n", "Thuoc tri", treatments[i].medicines);
            printf("%-25s: %s\n", "Loi khuyen", treatments[i].advice);
            return;
        }
    }
    printf("%-25s: Khong co thong tin\n", "Thuoc tri");
    printf("%-25s: Khong co thong tin\n", "Loi khuyen");
}

// Hàm t?o mã ng?u nhiên 4 ký t? s?
void generate_random_id(char *id) {
    srand(time(NULL));
    sprintf(id, "%04d", rand() % 10000);
}

// Hàm luu h? so b?nh nhân vào file
void save_patient_record() {
    if (patient_count >= MAX_PATIENTS) {
        printf("Danh sach ho so da day. Khong the them moi!\n");
        return;
    }

    struct PatientRecord record;

    // Nh?p thông tin h? so
    printf("Nhap ten benh nhan: ");
    fgets(record.name, sizeof(record.name), stdin);
    record.name[strcspn(record.name, "\n")] = '\0';
    
        // Nh?p gi?i tính
    printf("Nhap gioi tinh (Nam/Nu): ");
    fgets(record.gender, sizeof(record.gender), stdin);
    record.gender[strcspn(record.gender, "\n")] = '\0';  // X? lý newline còn l?i

    printf("Nhap nam sinh: ");
    scanf("%d", &record.birth_year);
    getchar();

    record.age = 2024 - record.birth_year;

    printf("Nhap noi o: ");
    fgets(record.address, sizeof(record.address), stdin);
    record.address[strcspn(record.address, "\n")] = '\0';

    // T?o mã ng?u nhiên
    generate_random_id(record.id);

    // Luu vào m?ng
    patients[patient_count++] = record;

    printf("Luu ho so thanh cong! Ma ho so cua ban la: %s\n", record.id);
}


void view_patient_records() {
    if (patient_count == 0) {
        printf("Khong co ho so nao!\n");
        return;
    }

    printf("\n================ DANH SACH HO SO ================\n");
    for (int i = 0; i < patient_count; i++) {
        printf("Ma ho so: %s\n", patients[i].id);
        printf("Ten: %s\n", patients[i].name);
        printf("Gioi tinh: %s\n", patients[i].gender);
        printf("Nam sinh: %d\n", patients[i].birth_year);
        printf("Tuoi: %d\n", patients[i].age);
        printf("Noi o: %s\n", patients[i].address);
        printf("Chuan doan: %s\n", strlen(patients[i].diagnosis) > 0 ? patients[i].diagnosis : "Chua co du lieu");
        printf("================================================\n");
    }
}



// Hàm luu k?t qu? ch?n doán vào h? so
void save_diagnosis_to_record(const char *patient_id, const char *diagnosis) {
    for (int i = 0; i < patient_count; i++) {
        if (strcmp(patient_id, patients[i].id) == 0) {
            strcpy(patients[i].diagnosis, diagnosis);
            printf("Luu chan doan thanh cong!\n");
            return;
        }
    }
    printf("Khong tim thay ho so voi ma: %s\n", patient_id);
}


// Hàm hi?n th? menu
void display_menu() {
    printf("\n================ MENU =================\n");
    printf("1. Xem danh sach benh\n");
    printf("2. Them ho so benh nhan\n");
    printf("3. Chuan doan trieu chung cua ban\n");
    printf("4. Xem danh sach ho so\n");
    printf("5. Thoat\n");
    printf("=======================================\n");
}


// Hàm ch?n doán b?nh (dã c?p nh?t k?t n?i luu h? so)
void diagnose(char input_symptoms[MAX_SYMPTOMS][50], int num_input_symptoms, struct Disease diseases[], int num_diseases, struct Treatment treatments[], int treatment_count) {
    char patient_id[5];
    printf("Nhap ma ho so cua ban: ");
    fgets(patient_id, sizeof(patient_id), stdin);
    patient_id[strcspn(patient_id, "\n")] = '\0';

    // Ki?m tra mã h? so
    int found = 0;
    for (int i = 0; i < patient_count; i++) {
        if (strcmp(patient_id, patients[i].id) == 0) {
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Khong tim thay ma ho so: %s\n", patient_id);
        return;
    }

    struct DiagnosisResult results[MAX_DISEASES];
    int result_count = 0;

    // Ch?n doán b?nh
    for (int i = 0; i < num_diseases; i++) {
        int match_count = 0;

        for (int j = 0; j < num_input_symptoms; j++) {
            normalize_and_trim(input_symptoms[j]);

            for (int k = 0; k < diseases[i].symptom_count; k++) {
                normalize_and_trim(diseases[i].symptoms[k]);

                if (strcmp(input_symptoms[j], diseases[i].symptoms[k]) == 0) {
                    match_count++;
                    break;
                }
            }
        }

        if (match_count > 2) {
            float confidence = match_count * 25.0f;
            if (confidence > 100.0f) confidence = 100.0f;

            strcpy(results[result_count].disease_name, diseases[i].name);
            results[result_count].match_count = match_count;
            results[result_count].confidence = confidence;
            result_count++;
        }
    }

    if (result_count > 0) {
        printf("\n================== KET QUA CHAN DOAN ==================\n");
        char diagnosis[500] = "";
        for (int i = 0; i < result_count; i++) {
            printf("\n%-25s: %s\n", "Benh", results[i].disease_name);
            printf("%-25s: %d\n", "So trieu chung trung khop", results[i].match_count);
            printf("%-25s: %.2f%%\n", "Do tin cay", results[i].confidence);

            find_treatment(results[i].disease_name, treatments, treatment_count);
            strcat(diagnosis, results[i].disease_name);
            strcat(diagnosis, "; ");
        }

        save_diagnosis_to_record(patient_id, diagnosis);
    } else {
        printf("Khong tim thay benh phu hop.\n");
    }
}



// Hàm chính
int main() {
    // Khai báo danh sách b?nh và danh sách thu?c tr?
    struct Disease diseases[MAX_DISEASES];
    struct Treatment treatments[MAX_DISEASES];
    
    // T?i d? li?u t? file
    int num_diseases = load_diseases_from_file("Tenbenh.txt", diseases);
    int num_treatments = load_treatments_from_file("Thuoctri.txt", treatments);

    // Ki?m tra d? li?u
    if (num_diseases == 0 || num_treatments == 0) {
        printf("Khong co du lieu benh hoac thuoc tri trong file.\n");
        return 1;
    }

    int choice; // L?a ch?n t? menu
    do {
        // Hi?n th? menu
        display_menu();

        // Nh?p l?a ch?n t? ngu?i dùng
        printf("Nhap lua chon: ");
        scanf("%d", &choice);
        getchar(); // Xóa ký t? newline còn l?i trong b? d?m

        // X? lý t?ng l?a ch?n
        switch (choice) {
            case 1: {
                // Xem danh sách b?nh
                printf("\nDanh sach cac benh pho bien:\n");
                for (int i = 0; i < num_diseases; i++) {
                    printf("%-5d %-50s\n", i + 1, diseases[i].name);
                }
                break;
            }
            case 2: {
                // Thêm h? so b?nh nhân
                save_patient_record();
                break;
            }
            case 3: {
                // Ch?n doán tri?u ch?ng
                char input_symptoms[MAX_SYMPTOMS][50];
                int num_input_symptoms;

                // Nh?p s? lu?ng tri?u ch?ng
                printf("Nhap so luong trieu chung cua ban (toi da %d): ", MAX_SYMPTOMS);
                scanf("%d", &num_input_symptoms);
                getchar();

                // Nh?p t?ng tri?u ch?ng
                for (int i = 0; i < num_input_symptoms; i++) {
                    printf("Trieu chung %d: ", i + 1);
                    fgets(input_symptoms[i], sizeof(input_symptoms[i]), stdin);
                    input_symptoms[i][strcspn(input_symptoms[i], "\n")] = '\0'; // Lo?i b? newline
                }

                // G?i hàm ch?n doán
                diagnose(input_symptoms, num_input_symptoms, diseases, num_diseases, treatments, num_treatments);
                break;
            }
            case 4: {
                // Xem danh sách h? so
                view_patient_records();
                break;
            }
            case 5: {
                // Thoát chuong trình
                printf("Thoat chuong trinh...\n");
                break;
            }
            default: {
                // X? lý l?a ch?n không h?p l?
                printf("Lua chon khong hop le. Vui long thu lai!\n");
            }
        }
    } while (choice != 5); // Thoát khi ngu?i dùng ch?n 5

    return 0;
}


