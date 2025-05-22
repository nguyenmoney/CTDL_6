#include "medicine.h"
#include "csv_handler.h" // Cho docDongAnToan
#include "hash_table.h"  // Đã bao gồm medicine.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Cho toupper

// Hàm tiện ích nội bộ (static)
static void removeNewlineMed(char* str) {
    if (str == NULL) return;
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}
static Thuoc* timThuocTheoTen(const Thuoc dsThuoc[], int soLuong, const char* tenThuoc);
// CẬP NHẬT: docDanhSachThuoc - Yêu cầu các trường không rỗng
Thuoc* docDanhSachThuoc(const char* tenFile, int* soLuong) {
    FILE* f = fopen(tenFile, "r");
    if (f == NULL) {
        *soLuong = 0;
        return NULL;
    }

    Thuoc* ds = NULL;
    int capacity = 0;
    *soLuong = 0;
    char line[256]; // Buffer cho mot dong CSV thuoc (maThuoc[30] + tenThuoc[150] + ...)
    int lineNumber = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        lineNumber++;
        removeNewlineMed(line);
        if (strlen(line) < 2) continue;

        if (*soLuong >= capacity) {
            capacity = (capacity == 0) ? 10 : capacity * 2;
            Thuoc* temp = realloc(ds, capacity * sizeof(Thuoc));
            if (temp == NULL) {
                perror("Loi cap phat bo nho (realloc docDanhSachThuoc)");
                free(ds);
                fclose(f);
                *soLuong = 0;
                return NULL;
            }
            ds = temp;
        }

        Thuoc t;
        // Định dạng CSV: maThuoc,tenThuoc
        char* token;

        token = strtok(line, ",");
        if (token && strlen(token) > 0) { // maThuoc không được rỗng
            strncpy(t.maThuoc, token, sizeof(t.maThuoc) - 1);
            t.maThuoc[sizeof(t.maThuoc) - 1] = '\0';
        }
        else {
            fprintf(stderr, "CANH BAO (docDanhSachThuoc Dong %d): Ma thuoc rong hoac khong hop le. Bo qua dong.\n", lineNumber);
            continue;
        }

        token = strtok(NULL, "\n");
        if (token && strlen(token) > 0) { // tenThuoc không được rỗng
            strncpy(t.tenThuoc, token, sizeof(t.tenThuoc) - 1);
            t.tenThuoc[sizeof(t.tenThuoc) - 1] = '\0';
        }
        else {
            fprintf(stderr, "CANH BAO (docDanhSachThuoc Dong %d): Ten thuoc rong hoac khong hop le cho MaThuoc '%s'. Bo qua dong.\n", lineNumber, t.maThuoc);
            continue;
        }

        ds[*soLuong] = t;
        (*soLuong)++;
    }

    fclose(f);
    return ds;
}

// CẬP NHẬT: ghiDanhSachThuocRaFile - Đảm bảo không ghi trường rỗng
void ghiDanhSachThuocRaFile(const char* tenFile, const Thuoc dsThuoc[], int soLuong) {
    FILE* f = fopen(tenFile, "w");
    if (f == NULL) {
        perror("Loi mo file de ghi (ghiDanhSachThuocRaFile)");
        return;
    }
    // Không ghi dòng tiêu đề nếu file CSV không có
    for (int i = 0; i < soLuong; ++i) {
        // Các hàm nhập và sửa phải đảm bảo các trường này không rỗng
        // Tuy nhiên, thêm một lớp bảo vệ ở đây
        const char* maThuocOut = (strlen(dsThuoc[i].maThuoc) > 0) ? dsThuoc[i].maThuoc : "MA_INVALID";
        const char* tenThuocOut = (strlen(dsThuoc[i].tenThuoc) > 0) ? dsThuoc[i].tenThuoc : "TEN_INVALID";
        fprintf(f, "%s,%s\n", maThuocOut, tenThuocOut);
    }
    fclose(f);
}


// Hàm tìm thuốc theo mã (tuyến tính)
Thuoc* timThuocTheoMa(const Thuoc dsThuoc[], int soLuong, const char* maThuoc) {
    if (maThuoc == NULL) return NULL;
    char maThuocUpper[MAX_LEN_MA_THUOC]; // Sử dụng define từ structs.h
    strncpy(maThuocUpper, maThuoc, sizeof(maThuocUpper) - 1);
    maThuocUpper[sizeof(maThuocUpper) - 1] = '\0';
    for (int i = 0; maThuocUpper[i]; i++) maThuocUpper[i] = toupper(maThuocUpper[i]);

    for (int i = 0; i < soLuong; ++i) {
        // Giả sử dsThuoc[i].maThuoc đã được chuẩn hóa
        if (strcmp(dsThuoc[i].maThuoc, maThuocUpper) == 0) {
            return (Thuoc*)&dsThuoc[i];
        }
    }
    return NULL;
}

// HÀM MỚI: timThuocTheoMaBangBam
Thuoc* timThuocTheoMaBangBam(BangBam* bbThuoc, const char* maThuoc) {
    if (bbThuoc == NULL || maThuoc == NULL) {
        return NULL;
    }
    char maThuocTimKiem[MAX_LEN_MA_THUOC];
    strncpy(maThuocTimKiem, maThuoc, sizeof(maThuocTimKiem) - 1);
    maThuocTimKiem[sizeof(maThuocTimKiem) - 1] = '\0';
    for (int k = 0; maThuocTimKiem[k]; k++) {
        maThuocTimKiem[k] = toupper(maThuocTimKiem[k]);
    }
    return (Thuoc*)timKiemTrongBangBam(bbThuoc, maThuocTimKiem);
}

// CẬP NHẬT: nhapThongTinThuocTuBanPhim
int nhapThongTinThuocTuBanPhim(Thuoc* tConTro,
    BangBam* bbThuocHienCo,
    const Thuoc dsThuocHienCo[], int soLuongThuocHienCo) {
    char bufferMa[MAX_LEN_MA_THUOC];
    printf("Nhap Ma Thuoc (VD: PARA500, toi da %zu ky tu, IN HOA, KHONG DUOC BO TRONG): ", sizeof(tConTro->maThuoc) - 1);
    if (docDongAnToan(bufferMa, sizeof(bufferMa), stdin) == NULL || strlen(bufferMa) == 0) {
        printf("LOI: Ma Thuoc khong duoc de trong. Huy them thuoc.\n");
        return 0;
    }
    for (int i = 0; bufferMa[i]; i++) {
        bufferMa[i] = toupper(bufferMa[i]);
    }
    if (strlen(bufferMa) == 0) { // Kiểm tra lại sau chuẩn hóa
        printf("LOI: Ma Thuoc khong duoc de trong sau chuan hoa. Huy them thuoc.\n");
        return 0;
    }

    if (bbThuocHienCo != NULL) {
        if (timKiemTrongBangBam(bbThuocHienCo, bufferMa) != NULL) {
            printf("LOI: Ma Thuoc '%s' da ton tai (kiem tra bang bam). Huy them thuoc.\n", bufferMa);
            return 0;
        }
    }
    else {
        fprintf(stderr, "LOI HE THONG: Bang bam thuoc khong kha dung. Khong the kiem tra trung lap MaThuoc. Huy them thuoc.\n");
        return 0;
    }
    strncpy(tConTro->maThuoc, bufferMa, sizeof(tConTro->maThuoc) - 1);
    tConTro->maThuoc[sizeof(tConTro->maThuoc) - 1] = '\0';

    printf("Nhap Ten Thuoc (KHONG DUOC BO TRONG, toi da %zu ky tu): ", sizeof(tConTro->tenThuoc) - 1);
    if (docDongAnToan(tConTro->tenThuoc, sizeof(tConTro->tenThuoc), stdin) == NULL || strlen(tConTro->tenThuoc) == 0) {
        printf("LOI: Ten Thuoc khong duoc de trong. Huy them thuoc.\n");
        return 0;
    }

    // Kiểm tra trùng tên thuốc (tuyến tính)
    const Thuoc* thuocTrungTen = timThuocTheoTen(dsThuocHienCo, soLuongThuocHienCo, tConTro->tenThuoc);
    if (thuocTrungTen != NULL) {
        printf("LOI: Ten Thuoc '%s' da ton tai voi Ma Thuoc khac ('%s'). Huy them thuoc.\n", tConTro->tenThuoc, thuocTrungTen->maThuoc);
        return 0;
    }

    return 1;
}

// CẬP NHẬT: themThuoc
int themThuoc(Thuoc** dsThuocConTro, int* soLuongConTro,
    BangBam** bbThuoc_ptr,
    Thuoc thuocMoi, const char* tenFile) {
    BangBam* bbHienTai = *bbThuoc_ptr;

    // Kiểm tra lại trùng mã (thuocMoi.maThuoc đã được chuẩn hóa và kiểm tra trong hàm nhập)
    if (bbHienTai != NULL) {
        if (timKiemTrongBangBam(bbHienTai, thuocMoi.maThuoc) != NULL) {
            fprintf(stderr, "LOI NGHIEM TRONG: Ma Thuoc '%s' da ton tai trong bang bam (logic themThuoc)!\n", thuocMoi.maThuoc);
            return 0;
        }
    }
    else {
        fprintf(stderr, "LOI HE THONG: Bang bam thuoc khong kha dung (NULL) khi them thuoc '%s'.\n", thuocMoi.maThuoc);
        return 0;
    }
    // Kiểm tra các trường của thuocMoi không được rỗng (đã được đảm bảo bởi nhapThongTinThuocTuBanPhim)
    if (strlen(thuocMoi.maThuoc) == 0 || strlen(thuocMoi.tenThuoc) == 0) {
        fprintf(stderr, "LOI: Thong tin thuoc moi khong hop le (ma hoac ten rong) khi them.\n");
        return 0;
    }


    Thuoc* temp = realloc(*dsThuocConTro, (*soLuongConTro + 1) * sizeof(Thuoc));
    if (temp == NULL) {
        perror("Loi cap phat bo nho (realloc themThuoc)");
        return 0;
    }
    *dsThuocConTro = temp;
    (*dsThuocConTro)[*soLuongConTro] = thuocMoi;
    Thuoc* conTroToiThuocTrongMang = &((*dsThuocConTro)[*soLuongConTro]);

    if (!chenVaoBangBam(bbHienTai, thuocMoi.maThuoc, conTroToiThuocTrongMang)) {
        fprintf(stderr, "LOI NGHIEM TRONG: Khong the chen Thuoc '%s' vao bang bam.\n", thuocMoi.maThuoc);
        return 0;
    }

    (*soLuongConTro)++;

    if ((*bbThuoc_ptr)->soLuongPhanTu / (double)(*bbThuoc_ptr)->dungLuong > BANG_BAM_NGUONG_TAI_TOI_DA) {
        printf("Thong bao: He so tai cua bang bam THUOC vuot nguong (%d/%d). Dang chuan bi rehash...\n",
            (*bbThuoc_ptr)->soLuongPhanTu, (*bbThuoc_ptr)->dungLuong);
        if (!rehashBangBam(bbThuoc_ptr)) {
            fprintf(stderr, "LOI: Rehash bang bam THUOC that bai. Chuong trinh co the tiep tuc nhung hieu suat se giam.\n");
        }
    }

    ghiDanhSachThuocRaFile(tenFile, *dsThuocConTro, *soLuongConTro);
    return 1;
}

// CẬP NHẬT: suaThongTinThuoc
int suaThongTinThuoc(Thuoc dsThuoc[], int soLuong, const char* maThuocCanSua, Thuoc thongTinMoi, const char* tenFile) {
    char maThuocCanSuaUpper[MAX_LEN_MA_THUOC];
    strncpy(maThuocCanSuaUpper, maThuocCanSua, sizeof(maThuocCanSuaUpper) - 1);
    maThuocCanSuaUpper[sizeof(maThuocCanSuaUpper) - 1] = '\0';
    for (int i = 0; maThuocCanSuaUpper[i]; i++) maThuocCanSuaUpper[i] = toupper(maThuocCanSuaUpper[i]);

    int indexThuocCanSua = -1;
    for (int i = 0; i < soLuong; ++i) {
        if (strcmp(dsThuoc[i].maThuoc, maThuocCanSuaUpper) == 0) {
            indexThuocCanSua = i;
            break;
        }
    }

    if (indexThuocCanSua == -1) {
        printf("Khong tim thay thuoc voi ma '%s' de sua.\n", maThuocCanSua);
        return 0;
    }

    // Tên thuốc mới không được rỗng
    if (strlen(thongTinMoi.tenThuoc) == 0) {
        printf("LOI: Ten thuoc moi khong duoc de trong. Sua that bai.\n");
        return 0;
    }

    // Kiểm tra trùng tên thuốc mới với các thuốc KHÁC
    for (int i = 0; i < soLuong; ++i) {
        if (i == indexThuocCanSua) continue; // Bỏ qua chính thuốc đang sửa

        char tenThuocTrongDSLower[sizeof(dsThuoc[i].tenThuoc)];
        char tenThuocMoiLower[sizeof(thongTinMoi.tenThuoc)];

        strncpy(tenThuocTrongDSLower, dsThuoc[i].tenThuoc, sizeof(tenThuocTrongDSLower) - 1);
        tenThuocTrongDSLower[sizeof(tenThuocTrongDSLower) - 1] = '\0';
        for (int k = 0; tenThuocTrongDSLower[k]; k++) tenThuocTrongDSLower[k] = tolower(tenThuocTrongDSLower[k]);

        strncpy(tenThuocMoiLower, thongTinMoi.tenThuoc, sizeof(tenThuocMoiLower) - 1);
        tenThuocMoiLower[sizeof(tenThuocMoiLower) - 1] = '\0';
        for (int k = 0; tenThuocMoiLower[k]; k++) tenThuocMoiLower[k] = tolower(tenThuocMoiLower[k]);

        if (strcmp(tenThuocMoiLower, tenThuocTrongDSLower) == 0) {
            printf("LOI: Ten thuoc moi '%s' da ton tai voi Ma Thuoc '%s'. Khong the sua.\n", thongTinMoi.tenThuoc, dsThuoc[i].maThuoc);
            return 0;
        }
    }

    // Ma thuoc không đổi, chỉ sửa tên thuốc
    strncpy(dsThuoc[indexThuocCanSua].tenThuoc, thongTinMoi.tenThuoc, sizeof(dsThuoc[indexThuocCanSua].tenThuoc) - 1);
    dsThuoc[indexThuocCanSua].tenThuoc[sizeof(dsThuoc[indexThuocCanSua].tenThuoc) - 1] = '\0';

    ghiDanhSachThuocRaFile(tenFile, dsThuoc, soLuong);
    return 1;
}

// CẬP NHẬT: xoaThuoc
int xoaThuoc(Thuoc** dsThuocConTro, int* soLuongConTro,
    BangBam* bbThuoc,
    const char* maThuocCanXoa, const char* tenFile) {
    char maXoaUpper[MAX_LEN_MA_THUOC];
    strncpy(maXoaUpper, maThuocCanXoa, sizeof(maXoaUpper) - 1);
    maXoaUpper[sizeof(maXoaUpper) - 1] = '\0';
    for (int i = 0; maXoaUpper[i]; i++) maXoaUpper[i] = toupper(maXoaUpper[i]);

    int removedFromHashTable = 0;
    if (bbThuoc != NULL) {
        if (timKiemTrongBangBam(bbThuoc, maXoaUpper) != NULL) {
            if (xoaKhoiBangBam(bbThuoc, maXoaUpper)) {
                removedFromHashTable = 1;
            }
            else {
                fprintf(stderr, "LOI: Khong the xoa Thuoc '%s' khoi bang bam mac du da tim thay.\n", maXoaUpper);
                return 0;
            }
        }
    }
    else {
        fprintf(stderr, "LOI HE THONG: Bang bam thuoc khong kha dung khi xoa.\n");
        return 0;
    }

    int viTriCanXoa = -1;
    for (int i = 0; i < *soLuongConTro; ++i) {
        if (strcmp((*dsThuocConTro)[i].maThuoc, maXoaUpper) == 0) {
            viTriCanXoa = i;
            break;
        }
    }

    if (viTriCanXoa != -1) {
        if (!removedFromHashTable && timKiemTrongBangBam(bbThuoc, maXoaUpper) == NULL) {
            fprintf(stderr, "CANH BAO: Thuoc '%s' tim thay trong mang nhung khong co trong bang bam truoc khi xoa khoi mang.\n", maXoaUpper);
        }

        for (int i = viTriCanXoa; i < (*soLuongConTro - 1); ++i) {
            (*dsThuocConTro)[i] = (*dsThuocConTro)[i + 1];
        }
        (*soLuongConTro)--;

        if (*soLuongConTro == 0) {
            free(*dsThuocConTro);
            *dsThuocConTro = NULL;
        }
        else {
            Thuoc* temp = realloc(*dsThuocConTro, (*soLuongConTro) * sizeof(Thuoc));
            if (temp == NULL && *soLuongConTro > 0) {
                perror("Loi realloc khi xoa thuoc (thu nho mang)");
            }
            else if (*soLuongConTro > 0) {
                *dsThuocConTro = temp;
            }
        }
        ghiDanhSachThuocRaFile(tenFile, *dsThuocConTro, *soLuongConTro);
        // printf("Da xoa thuoc '%s'.\n", maXoaUpper); // Thông báo này nên ở UI
        return 1;
    }

    if (removedFromHashTable && viTriCanXoa == -1) {
        fprintf(stderr, "LOI DONG BO: Da xoa Thuoc '%s' khoi bang bam nhung khong tim thay trong mang.\n", maXoaUpper);
        return 0;
    }

    // printf("Khong tim thay thuoc voi ma '%s' de xoa.\n", maXoaUpper); // Thông báo này nên ở UI
    return 0;
}

// Hàm inDanhSachThuocConsole
void inDanhSachThuocConsole(const Thuoc dsThuoc[], int soLuong) {
    printf("\n--- DANH SACH THUOC TRONG HE THONG ---\n");
    if (soLuong == 0) {
        printf("Khong co loai thuoc nao trong danh sach.\n");
        return;
    }
    printf("---------------------------------------------------------------------------\n");
    printf("| %-5s | %-20s | %-40s |\n", "STT", "Ma Thuoc", "Ten Thuoc");
    printf("---------------------------------------------------------------------------\n");

    for (int i = 0; i < soLuong; ++i) {
        printf("| %-5d | %-20s | %-40s |\n",
            i + 1,
            dsThuoc[i].maThuoc,
            dsThuoc[i].tenThuoc);
    }
    printf("---------------------------------------------------------------------------\n");
}
static void med_toLowerString(char* dest, const char* src, size_t dest_size) {
    if (src == NULL || dest == NULL || dest_size == 0) return;
    size_t i = 0;
    for (i = 0; src[i] && i < dest_size - 1; ++i) {
        dest[i] = tolower((unsigned char)src[i]);
    }
    dest[i] = '\0';
}

// Hàm tìm thuốc theo tên (static, tuyến tính, không phân biệt hoa thường, tìm kiếm chứa)
// Hàm này được sử dụng bởi nhapThongTinThuocTuBanPhim để kiểm tra trùng tên.
// Giữ lại và có thể tái sử dụng logic của nó.

 static Thuoc* timThuocTheoTen(const Thuoc dsThuoc[], int soLuong, const char* tenThuoc) {  
        if (tenThuoc == NULL || strlen(tenThuoc) == 0) return NULL;  
        char tenThuocLower[sizeof(((Thuoc*)0)->tenThuoc)];  
        med_toLowerString(tenThuocLower, tenThuoc, sizeof(tenThuocLower));  

        for (int i = 0; i < soLuong; ++i) {  
            char currentTenThuocLower[sizeof(dsThuoc[i].tenThuoc)];  
            med_toLowerString(currentTenThuocLower, dsThuoc[i].tenThuoc, sizeof(currentTenThuocLower));  

            if (strcmp(currentTenThuocLower, tenThuocLower) == 0) {  
                return (Thuoc*)&dsThuoc[i];  
            }  
        }  
        return NULL;  
    }
   


void timKiemThuocTheoTenTuyenTinh(const Thuoc dsThuoc[], int soLuongThuoc,
    const char* tenThuocTimKiem,
    Thuoc** ketQuaTimKiem, int* soLuongKetQua) {
    *ketQuaTimKiem = NULL;
    *soLuongKetQua = 0;
    if (soLuongThuoc == 0 || tenThuocTimKiem == NULL || strlen(tenThuocTimKiem) == 0) {
        return;
    }

    Thuoc* dsKetQuaTamThoi = (Thuoc*)malloc(soLuongThuoc * sizeof(Thuoc));
    if (dsKetQuaTamThoi == NULL) {
        perror("Loi cap phat bo nho (timKiemThuocTheoTenTuyenTinh: dsKetQuaTamThoi)");
        return;
    }
    int count = 0;

    char tenTimKiemLower[sizeof(((Thuoc*)0)->tenThuoc)];
    med_toLowerString(tenTimKiemLower, tenThuocTimKiem, sizeof(tenTimKiemLower));

    for (int i = 0; i < soLuongThuoc; ++i) {
        char tenThuocTrongDSLower[sizeof(dsThuoc[i].tenThuoc)];
        med_toLowerString(tenThuocTrongDSLower, dsThuoc[i].tenThuoc, sizeof(tenThuocTrongDSLower));

        if (strstr(tenThuocTrongDSLower, tenTimKiemLower) != NULL) { // Tìm kiếm chứa
            if (count < soLuongThuoc) {
                dsKetQuaTamThoi[count++] = dsThuoc[i];
            }
        }
    }

    if (count > 0) {
        *ketQuaTimKiem = (Thuoc*)malloc(count * sizeof(Thuoc));
        if (*ketQuaTimKiem == NULL) {
            perror("Loi cap phat bo nho (timKiemThuocTheoTenTuyenTinh: ketQuaTimKiem)");
            free(dsKetQuaTamThoi);
            *soLuongKetQua = 0;
            return;
        }
        memcpy(*ketQuaTimKiem, dsKetQuaTamThoi, count * sizeof(Thuoc));
        *soLuongKetQua = count;
    }

    free(dsKetQuaTamThoi);
}
