#ifndef PATIENT_RECORD_H
#define PATIENT_RECORD_H

struct PatientRecord {
    char id[5];
    char name[50];
    char gender[10];           // Gi?i tính ('M' cho nam, 'F' cho n?)
    int birth_year;
    int age;
    char address[100];
    char diagnosis[500];
};

#endif // PATIENT_RECORD_H

