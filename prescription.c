#include "prescription.h"
#include "medicine.h"    // Để sử dụng timThuocTheoMaBangBam và inDanhSachThuocConsole
#include "csv_handler.h" // Cho docDongAnToan
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Hàm tiện ích nội bộ để xóa ký tự xuống dòng
static void removeNewlinePresc(char* str) {
    if (str == NULL) return;
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

// CẬP NHẬT: docDanhSachDonThuocChiTiet
DonThuocChiTiet* docDanhSachDonThuocChiTiet(const char* tenFile, int* soLuongDongConTro) {
    FILE* f = fopen(tenFile, "r");
    if (f == NULL) {
        *soLuongDongConTro = 0;
        return NULL;
    }

    DonThuocChiTiet* ds = NULL;
    int capacity = 0;
    *soLuongDongConTro = 0;
    char line[512]; // Tăng buffer
    int lineNumber = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        lineNumber++;
        removeNewlinePresc(line);
        if (strlen(line) < 5) continue;

        if (*soLuongDongConTro >= capacity) {
            capacity = (capacity == 0) ? 20 : capacity * 2; // Tăng capacity ban đầu
            DonThuocChiTiet* temp = realloc(ds, capacity * sizeof(DonThuocChiTiet));
            if (temp == NULL) {
                perror("Loi realloc (docDanhSachDonThuocChiTiet)");
                free(ds); fclose(f); *soLuongDongConTro = 0; return NULL;
            }
            ds = temp;
        }

        DonThuocChiTiet dtct;
        // Định dạng CSV: maDonThuocChiTiet,maThuoc,soLuong,lieuDung,cachDung
        char* token;

        token = strtok(line, ",");
        if (token && strlen(token) > 0) dtct.maDonThuocChiTiet = atoi(token);
        else {
            fprintf(stderr, "CANH BAO (docDanhSachDonThuocChiTiet Dong %d): Ma don thuoc chi tiet khong hop le. Bo qua dong.\n", lineNumber);
            continue;
        }

        token = strtok(NULL, ",");
        if (token && strlen(token) > 0) { // maThuoc không được rỗng
            strncpy(dtct.maThuoc, token, sizeof(dtct.maThuoc) - 1);
            dtct.maThuoc[sizeof(dtct.maThuoc) - 1] = '\0';
        }
        else {
            fprintf(stderr, "CANH BAO (docDanhSachDonThuocChiTiet Dong %d): Ma thuoc rong cho MaDon %d. Bo qua dong.\n", lineNumber, dtct.maDonThuocChiTiet);
            continue;
        }

        token = strtok(NULL, ",");
        if (token && atoi(token) > 0) { // soLuong phải > 0
            dtct.soLuong = atoi(token);
        }
        else {
            fprintf(stderr, "CANH BAO (docDanhSachDonThuocChiTiet Dong %d): So luong khong hop le cho MaDon %d, MaThuoc %s. Bo qua dong.\n", lineNumber, dtct.maDonThuocChiTiet, dtct.maThuoc);
            continue;
        }

        token = strtok(NULL, ",");
        if (token && strlen(token) > 0) { // lieuDung không được rỗng
            strncpy(dtct.lieuDung, token, sizeof(dtct.lieuDung) - 1);
            dtct.lieuDung[sizeof(dtct.lieuDung) - 1] = '\0';
        }
        else {
            // Gán giá trị mặc định hoặc báo lỗi nếu muốn chặt chẽ hơn
            fprintf(stderr, "CANH BAO (docDanhSachDonThuocChiTiet Dong %d): Lieu dung rong cho MaDon %d, MaThuoc %s. Dat mac dinh 'Chua co'.\n", lineNumber, dtct.maDonThuocChiTiet, dtct.maThuoc);
            strcpy(dtct.lieuDung, "Chưa có");
        }

        token = strtok(NULL, "\n");
        if (token && strlen(token) > 0) { // cachDung không được rỗng
            strncpy(dtct.cachDung, token, sizeof(dtct.cachDung) - 1);
            dtct.cachDung[sizeof(dtct.cachDung) - 1] = '\0';
        }
        else {
            fprintf(stderr, "CANH BAO (docDanhSachDonThuocChiTiet Dong %d): Cach dung rong cho MaDon %d, MaThuoc %s. Dat mac dinh 'Chua co'.\n", lineNumber, dtct.maDonThuocChiTiet, dtct.maThuoc);
            strcpy(dtct.cachDung, "Chưa có");
        }

        ds[*soLuongDongConTro] = dtct;
        (*soLuongDongConTro)++;
    }
    fclose(f);
    return ds;
}

// CẬP NHẬT: ghiDanhSachDonThuocChiTietRaFile
void ghiDanhSachDonThuocChiTietRaFile(const char* tenFile, const DonThuocChiTiet dsDonThuocChiTiet[], int soLuongDong) {
    FILE* f = fopen(tenFile, "w");
    if (f == NULL) {
        perror("Loi mo file (ghiDanhSachDonThuocChiTietRaFile)");
        return;
    }
    // Không ghi dòng tiêu đề nếu file CSV không có

    for (int i = 0; i < soLuongDong; ++i) {
        // Đảm bảo các trường không rỗng khi ghi, mặc dù hàm nhập/sửa nên đảm bảo điều này
        const char* maThuocOut = (strlen(dsDonThuocChiTiet[i].maThuoc) > 0) ? dsDonThuocChiTiet[i].maThuoc : "MA_THUOC_INVALID";
        int soLuongOut = (dsDonThuocChiTiet[i].soLuong > 0) ? dsDonThuocChiTiet[i].soLuong : 1; // Hoặc một giá trị mặc định khác
        const char* lieuDungOut = (strlen(dsDonThuocChiTiet[i].lieuDung) > 0) ? dsDonThuocChiTiet[i].lieuDung : "Chưa có";
        const char* cachDungOut = (strlen(dsDonThuocChiTiet[i].cachDung) > 0) ? dsDonThuocChiTiet[i].cachDung : "Chưa có";

        fprintf(f, "%d,%s,%d,%s,%s\n",
            dsDonThuocChiTiet[i].maDonThuocChiTiet,
            maThuocOut,
            soLuongOut,
            lieuDungOut,
            cachDungOut);
    }
    fclose(f);
}

// CẬP NHẬT: themDongDonThuocChiTiet
int themDongDonThuocChiTiet(DonThuocChiTiet** dsDTCTConTro, int* soLuongDongConTro, DonThuocChiTiet dtctMoi, const char* tenFile) {
    // Kiểm tra các trường của dtctMoi không được rỗng (ngoại trừ maDonThuocChiTiet tự gán)
    if (strlen(dtctMoi.maThuoc) == 0 || dtctMoi.soLuong <= 0 ||
        strlen(dtctMoi.lieuDung) == 0 || strlen(dtctMoi.cachDung) == 0) {
        fprintf(stderr, "LOI (themDongDonThuocChiTiet): Thong tin chi tiet don thuoc khong hop le (truong rong hoac so luong <=0).\n");
        return 0;
    }

    // Kiểm tra xem thuốc đã có trong đơn thuốc này chưa
    for (int i = 0; i < *soLuongDongConTro; ++i) {
        if ((*dsDTCTConTro)[i].maDonThuocChiTiet == dtctMoi.maDonThuocChiTiet &&
            strcmp((*dsDTCTConTro)[i].maThuoc, dtctMoi.maThuoc) == 0) {
            // Không in lỗi ở đây, để hàm gọi (quanLyCapNhatChiTietDonThuoc) xử lý thông báo
            return 0; // Thuoc da ton tai
        }
    }

    DonThuocChiTiet* temp = realloc(*dsDTCTConTro, (*soLuongDongConTro + 1) * sizeof(DonThuocChiTiet));
    if (temp == NULL) {
        perror("Loi realloc (themDongDonThuocChiTiet)");
        return 0;
    }
    *dsDTCTConTro = temp;
    (*dsDTCTConTro)[*soLuongDongConTro] = dtctMoi;
    (*soLuongDongConTro)++;
    ghiDanhSachDonThuocChiTietRaFile(tenFile, *dsDTCTConTro, *soLuongDongConTro);
    return 1;
}

// xoaDongThuocTrongDon - logic không thay đổi nhiều
int xoaDongThuocTrongDon(DonThuocChiTiet** dsDTCTConTro, int* soLuongDongConTro, int maDonThuocCanTim, const char* maThuocCanXoa, const char* tenFile) {
    int viTriCanXoa = -1;
    char maThuocXoaUpper[MAX_LEN_MA_THUOC];
    strncpy(maThuocXoaUpper, maThuocCanXoa, sizeof(maThuocXoaUpper) - 1);
    maThuocXoaUpper[sizeof(maThuocXoaUpper) - 1] = '\0';
    for (int i = 0; maThuocXoaUpper[i]; i++) maThuocXoaUpper[i] = toupper(maThuocXoaUpper[i]);


    for (int i = 0; i < *soLuongDongConTro; ++i) {
        if ((*dsDTCTConTro)[i].maDonThuocChiTiet == maDonThuocCanTim &&
            strcmp((*dsDTCTConTro)[i].maThuoc, maThuocXoaUpper) == 0) { // So sánh với mã đã chuẩn hóa
            viTriCanXoa = i;
            break;
        }
    }

    if (viTriCanXoa != -1) {
        for (int i = viTriCanXoa; i < (*soLuongDongConTro - 1); ++i) {
            (*dsDTCTConTro)[i] = (*dsDTCTConTro)[i + 1];
        }
        (*soLuongDongConTro)--;
        if (*soLuongDongConTro == 0) {
            free(*dsDTCTConTro);
            *dsDTCTConTro = NULL;
        }
        else {
            DonThuocChiTiet* temp = realloc(*dsDTCTConTro, (*soLuongDongConTro) * sizeof(DonThuocChiTiet));
            if (temp == NULL && *soLuongDongConTro > 0) {
                perror("Loi realloc (xoaDongThuocTrongDon)");
            }
            else if (*soLuongDongConTro > 0) {
                *dsDTCTConTro = temp;
            }
            // Nếu temp NULL và *soLuongDongConTro == 0, thì free ở trên đã xử lý
        }
        ghiDanhSachDonThuocChiTietRaFile(tenFile, *dsDTCTConTro, *soLuongDongConTro);
        return 1;
    }
    // printf("Khong tim thay thuoc '%s' trong don thuoc %d de xoa.\n", maThuocXoaUpper, maDonThuocCanTim); // UI sẽ thông báo
    return 0;
}

// CẬP NHẬT: suaDongThuocTrongDon
int suaDongThuocTrongDon(DonThuocChiTiet dsDonThuocChiTiet[], int soLuongDong, int maDonThuocCanTim, const char* maThuocCanSua, DonThuocChiTiet thongTinMoi, const char* tenFile) {
    // Kiểm tra các trường của thongTinMoi không được rỗng
    if (thongTinMoi.soLuong <= 0 || strlen(thongTinMoi.lieuDung) == 0 || strlen(thongTinMoi.cachDung) == 0) {
        fprintf(stderr, "LOI (suaDongThuocTrongDon): Thong tin thuoc moi khong hop le (so luong, lieu dung hoac cach dung rong/invalid).\n");
        return 0;
    }

    char maThuocSuaUpper[MAX_LEN_MA_THUOC];
    strncpy(maThuocSuaUpper, maThuocCanSua, sizeof(maThuocSuaUpper) - 1);
    maThuocSuaUpper[sizeof(maThuocSuaUpper) - 1] = '\0';
    for (int i = 0; maThuocSuaUpper[i]; i++) maThuocSuaUpper[i] = toupper(maThuocSuaUpper[i]);

    for (int i = 0; i < soLuongDong; ++i) {
        if (dsDonThuocChiTiet[i].maDonThuocChiTiet == maDonThuocCanTim &&
            strcmp(dsDonThuocChiTiet[i].maThuoc, maThuocSuaUpper) == 0) { // So sánh với mã đã chuẩn hóa

            dsDonThuocChiTiet[i].soLuong = thongTinMoi.soLuong;
            strncpy(dsDonThuocChiTiet[i].lieuDung, thongTinMoi.lieuDung, sizeof(dsDonThuocChiTiet[i].lieuDung) - 1);
            dsDonThuocChiTiet[i].lieuDung[sizeof(dsDonThuocChiTiet[i].lieuDung) - 1] = '\0';
            strncpy(dsDonThuocChiTiet[i].cachDung, thongTinMoi.cachDung, sizeof(dsDonThuocChiTiet[i].cachDung) - 1);
            dsDonThuocChiTiet[i].cachDung[sizeof(dsDonThuocChiTiet[i].cachDung) - 1] = '\0';

            ghiDanhSachDonThuocChiTietRaFile(tenFile, dsDonThuocChiTiet, soLuongDong);
            return 1;
        }
    }
    // printf("Khong tim thay thuoc '%s' trong don thuoc %d de sua.\n", maThuocSuaUpper, maDonThuocCanTim); // UI sẽ thông báo
    return 0;
}

// inChiTietMotDonThuocConsole - logic không thay đổi nhiều
void inChiTietMotDonThuocConsole(int maDonThuoc,
    const DonThuocChiTiet dsDonThuocChiTiet[], int soLuongDongTongCong,
    const Thuoc dsThuocToanBo[], int soLuongThuocTB) {
    printf("\n--- CHI TIET DON THUOC MA: %d ---\n", maDonThuoc);
    int stt = 0;
    int timThayDon = 0;

    for (int i = 0; i < soLuongDongTongCong; ++i) {
        if (dsDonThuocChiTiet[i].maDonThuocChiTiet == maDonThuoc) {
            if (!timThayDon) {
                printf("----------------------------------------------------------------------------------------------------\n");
                printf("| %-3s | %-15s | %-30s | %-7s | %-20s | %-20s |\n", "STT", "Ma Thuoc", "Ten Thuoc", "So Luong", "Lieu Dung", "Cach Dung");
                printf("----------------------------------------------------------------------------------------------------\n");
                timThayDon = 1;
            }
            stt++;
            // Sử dụng timThuocTheoMa (hoặc timThuocTheoMaBangBam nếu có bảng băm Thuốc ở đây)
            const Thuoc* thuocInfo = timThuocTheoMa(dsThuocToanBo, soLuongThuocTB, dsDonThuocChiTiet[i].maThuoc);
            char tenThuocHienThi[151] = "Khong tim thay ten";
            if (thuocInfo != NULL) {
                strncpy(tenThuocHienThi, thuocInfo->tenThuoc, 150);
                tenThuocHienThi[150] = '\0';
            }

            printf("| %-3d | %-15s | %-30.30s | %-7d | %-20.20s | %-20.20s |\n",
                stt,
                dsDonThuocChiTiet[i].maThuoc,
                tenThuocHienThi,
                dsDonThuocChiTiet[i].soLuong,
                dsDonThuocChiTiet[i].lieuDung,
                dsDonThuocChiTiet[i].cachDung);
        }
    }

    if (!timThayDon) {
        printf("Khong co chi tiet nao cho don thuoc ma %d hoac don thuoc khong ton tai.\n", maDonThuoc);
    }
    else {
        printf("----------------------------------------------------------------------------------------------------\n");
    }
}

// inTatCaDonThuocChiTietConsole - logic không thay đổi nhiều
void inTatCaDonThuocChiTietConsole(const DonThuocChiTiet dsDonThuocChiTiet[], int soLuongDong,
    const Thuoc dsThuocToanBo[], int soLuongThuocTB) {
    printf("\n--- TAT CA CHI TIET CAC DON THUOC ---\n");
    if (soLuongDong == 0) {
        printf("Khong co chi tiet don thuoc nao.\n");
        return;
    }
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-7s | %-15s | %-30s | %-7s | %-20s | %-20s |\n", "Ma DT", "Ma Thuoc", "Ten Thuoc", "So Luong", "Lieu Dung", "Cach Dung");
    printf("----------------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < soLuongDong; ++i) {
        const Thuoc* thuocInfo = timThuocTheoMa(dsThuocToanBo, soLuongThuocTB, dsDonThuocChiTiet[i].maThuoc);
        char tenThuocHienThi[151] = "N/A";
        if (thuocInfo != NULL) {
            strncpy(tenThuocHienThi, thuocInfo->tenThuoc, 150);
            tenThuocHienThi[150] = '\0';
        }
        printf("| %-7d | %-15s | %-30.30s | %-7d | %-20.20s | %-20.20s |\n",
            dsDonThuocChiTiet[i].maDonThuocChiTiet,
            dsDonThuocChiTiet[i].maThuoc,
            tenThuocHienThi,
            dsDonThuocChiTiet[i].soLuong,
            dsDonThuocChiTiet[i].lieuDung,
            dsDonThuocChiTiet[i].cachDung);
    }
    printf("----------------------------------------------------------------------------------------------------------------------\n");
}

// CẬP NHẬT: quanLyCapNhatChiTietDonThuoc
void quanLyCapNhatChiTietDonThuoc(int maDonThuocDeCapNhat,
    DonThuocChiTiet** dsDTCTConTro, int* soLuongDongConTro,
    const Thuoc dsThuocToanBo[], int soLuongThuocTB,
    BangBam* bbThuoc, // <<<< THÊM BangBam* bbThuoc
    const char* tenFileCSV) {
    int choice;
    char buffer[128]; // Tăng buffer

    if (dsThuocToanBo == NULL || soLuongThuocTB == 0 || bbThuoc == NULL) { // Kiểm tra bbThuoc
        printf("LOI: Khong co du lieu thuoc hoac bang bam thuoc khong kha dung de ke don.\n");
        printf("Vui long cap nhat danh sach thuoc va khoi tao bang bam thuoc truoc.\n");
        return;
    }

    do {
        inChiTietMotDonThuocConsole(maDonThuocDeCapNhat, *dsDTCTConTro, *soLuongDongConTro, dsThuocToanBo, soLuongThuocTB);
        printf("\n--- QUAN LY DON THUOC MA: %d ---\n", maDonThuocDeCapNhat);
        printf("1. Them thuoc vao don\n");
        printf("2. Sua thong tin thuoc trong don\n");
        printf("3. Xoa thuoc khoi don\n");
        printf("0. Quay lai\n");
        printf("Lua chon cua ban: ");

        if (docDongAnToan(buffer, sizeof(buffer), stdin) == NULL) { // Sử dụng docDongAnToan
            choice = 0; // Thoát nếu có lỗi đọc hoặc EOF
        }
        else if (sscanf(buffer, "%d", &choice) != 1) {
            choice = -1;
        }

        switch (choice) {
        case 1: { // Them thuoc
            DonThuocChiTiet dtctMoi;
            dtctMoi.maDonThuocChiTiet = maDonThuocDeCapNhat;

            printf("\n--- Danh Sach Thuoc Hien Co (co the tim theo Ma hoac Ten) ---\n");
            inDanhSachThuocConsole(dsThuocToanBo, soLuongThuocTB);

            printf("Nhap Ma Thuoc can them (IN HOA, KHONG DUOC BO TRONG): ");
            if (docDongAnToan(dtctMoi.maThuoc, sizeof(dtctMoi.maThuoc), stdin) == NULL || strlen(dtctMoi.maThuoc) == 0) {
                printf("LOI: Ma Thuoc khong duoc de trong. Huy them.\n");
                break;
            }
            for (int i = 0; dtctMoi.maThuoc[i]; i++) dtctMoi.maThuoc[i] = toupper(dtctMoi.maThuoc[i]);

            // Sử dụng timThuocTheoMaBangBam để xác thực
            if (timThuocTheoMaBangBam(bbThuoc, dtctMoi.maThuoc) == NULL) {
                printf("Loi: Ma Thuoc '%s' khong ton tai trong danh muc thuoc! Huy them.\n", dtctMoi.maThuoc);
                break;
            }

            // Kiểm tra xem thuốc đã có trong đơn này chưa
            int thuocDaCoTrongDon = 0;
            for (int i = 0; i < *soLuongDongConTro; ++i) {
                if ((*dsDTCTConTro)[i].maDonThuocChiTiet == maDonThuocDeCapNhat &&
                    strcmp((*dsDTCTConTro)[i].maThuoc, dtctMoi.maThuoc) == 0) {
                    thuocDaCoTrongDon = 1;
                    break;
                }
            }
            if (thuocDaCoTrongDon) {
                printf("Loi: Thuoc '%s' da ton tai trong don thuoc %d. Hay sua so luong/thong tin neu can.\n", dtctMoi.maThuoc, maDonThuocDeCapNhat);
                break;
            }


            printf("Nhap So Luong (phai > 0): ");
            if (docDongAnToan(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &dtctMoi.soLuong) != 1 || dtctMoi.soLuong <= 0) {
                printf("LOI: So luong khong hop le hoac khong phai la so nguyen duong. Huy them.\n");
                break;
            }

            printf("Nhap Lieu Dung (KHONG DUOC BO TRONG): ");
            if (docDongAnToan(dtctMoi.lieuDung, sizeof(dtctMoi.lieuDung), stdin) == NULL || strlen(dtctMoi.lieuDung) == 0) {
                printf("LOI: Lieu dung khong duoc de trong. Huy them.\n");
                break;
            }
            printf("Nhap Cach Dung (KHONG DUOC BO TRONG): ");
            if (docDongAnToan(dtctMoi.cachDung, sizeof(dtctMoi.cachDung), stdin) == NULL || strlen(dtctMoi.cachDung) == 0) {
                printf("LOI: Cach dung khong duoc de trong. Huy them.\n");
                break;
            }

            if (themDongDonThuocChiTiet(dsDTCTConTro, soLuongDongConTro, dtctMoi, tenFileCSV)) {
                printf("Them thuoc vao don thanh cong!\n");
            }
            else {
                printf("Them thuoc vao don THAT BAI (co the do loi logic hoac du lieu khong hop le da duoc kiem tra).\n");
            }
            break;
        }
        case 2: { // Sua thuoc
            char maThuocSua[MAX_LEN_MA_THUOC];
            DonThuocChiTiet thongTinMoiSua; // Đổi tên biến để tránh nhầm lẫn
            printf("Nhap Ma Thuoc trong don can sua (IN HOA, KHONG DUOC BO TRONG): ");
            if (docDongAnToan(maThuocSua, sizeof(maThuocSua), stdin) == NULL || strlen(maThuocSua) == 0) {
                printf("LOI: Ma Thuoc can sua khong duoc de trong. Huy sua.\n");
                break;
            }
            for (int i = 0; maThuocSua[i]; i++) maThuocSua[i] = toupper(maThuocSua[i]);

            int timThayThuocTrongDon = 0;
            for (int i = 0; i < *soLuongDongConTro; ++i) {
                if ((*dsDTCTConTro)[i].maDonThuocChiTiet == maDonThuocDeCapNhat && strcmp((*dsDTCTConTro)[i].maThuoc, maThuocSua) == 0) {
                    timThayThuocTrongDon = 1;
                    break;
                }
            }
            if (!timThayThuocTrongDon) {
                printf("Thuoc '%s' khong co trong don thuoc %d. Huy sua.\n", maThuocSua, maDonThuocDeCapNhat);
                break;
            }

            printf("Nhap So Luong moi (phai > 0): ");
            if (docDongAnToan(buffer, sizeof(buffer), stdin) == NULL || sscanf(buffer, "%d", &thongTinMoiSua.soLuong) != 1 || thongTinMoiSua.soLuong <= 0) {
                printf("LOI: So luong moi khong hop le. Huy sua.\n");
                break;
            }

            printf("Nhap Lieu Dung moi (KHONG DUOC BO TRONG): ");
            if (docDongAnToan(thongTinMoiSua.lieuDung, sizeof(thongTinMoiSua.lieuDung), stdin) == NULL || strlen(thongTinMoiSua.lieuDung) == 0) {
                printf("LOI: Lieu dung moi khong duoc de trong. Huy sua.\n");
                break;
            }
            printf("Nhap Cach Dung moi (KHONG DUOC BO TRONG): ");
            if (docDongAnToan(thongTinMoiSua.cachDung, sizeof(thongTinMoiSua.cachDung), stdin) == NULL || strlen(thongTinMoiSua.cachDung) == 0) {
                printf("LOI: Cach dung moi khong duoc de trong. Huy sua.\n");
                break;
            }

            if (suaDongThuocTrongDon(*dsDTCTConTro, *soLuongDongConTro, maDonThuocDeCapNhat, maThuocSua, thongTinMoiSua, tenFileCSV)) {
                printf("Sua thong tin thuoc thanh cong!\n");
            }
            else {
                printf("Sua thong tin thuoc THAT BAI.\n");
            }
            break;
        }
        case 3: { // Xoa thuoc
            char maThuocXoa[MAX_LEN_MA_THUOC];
            printf("Nhap Ma Thuoc trong don can xoa (IN HOA, KHONG DUOC BO TRONG): ");
            if (docDongAnToan(maThuocXoa, sizeof(maThuocXoa), stdin) == NULL || strlen(maThuocXoa) == 0) {
                printf("LOI: Ma Thuoc can xoa khong duoc de trong. Huy xoa.\n");
                break;
            }
            for (int i = 0; maThuocXoa[i]; i++) maThuocXoa[i] = toupper(maThuocXoa[i]);

            if (xoaDongThuocTrongDon(dsDTCTConTro, soLuongDongConTro, maDonThuocDeCapNhat, maThuocXoa, tenFileCSV)) {
                printf("Xoa thuoc khoi don thanh cong!\n");
            }
            else {
                printf("Xoa thuoc khoi don THAT BAI (co the khong tim thay thuoc '%s' trong don %d).\n", maThuocXoa, maDonThuocDeCapNhat);
            }
            break;
        }
        case 0:
            printf("Quay lai menu truoc.\n");
            break;
        default:
            printf("Lua chon khong hop le. Vui long chon lai.\n");
            break;
        }
        if (choice != 0) {
            printf("\nNhan Enter de tiep tuc...");
            // Xóa bộ đệm đầu vào trước khi chờ Enter, phòng trường hợp còn sót ký tự
            int c_temp;
            while ((c_temp = getchar()) != '\n' && c_temp != EOF);
            if (c_temp == EOF && choice != 0) { // Nếu gặp EOF, thoát luôn để tránh vòng lặp vô hạn
                choice = 0;
            }
            else if (choice != 0) { // Nếu không phải EOF, chờ thêm 1 Enter nữa
                getchar();
            }
        }
    } while (choice != 0);
}