#include "doctor.h"
#include "department.h" // Cho timKhoaPhongTheoMa
#include "csv_handler.h"  // Cho docDongAnToan
#include "hash_table.h" // Đã bao gồm trong doctor.h, nhưng thêm ở đây cho rõ ràng
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Cho toupper, tolower, isdigit

// Hàm tiện ích nội bộ (static) để xóa ký tự xuống dòng
static void removeNewlineDoc(char* str) {
    if (str == NULL) return;
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

// Hàm docDanhSachBacSi
// Phiên bản này KHÔNG bỏ qua dòng tiêu đề.
BacSi* docDanhSachBacSi(const char* tenFile, int* soLuong) {
    FILE* f = fopen(tenFile, "r");
    if (f == NULL) {
        *soLuong = 0;
        return NULL;
    }

    BacSi* ds = NULL;
    int capacity = 0;
    *soLuong = 0;
    char line[500];
    char* token;

    while (fgets(line, sizeof(line), f) != NULL) {
        removeNewlineDoc(line);
        if (strlen(line) < 5) continue;

        if (*soLuong >= capacity) {
            capacity = (capacity == 0) ? 10 : capacity * 2;
            BacSi* temp = realloc(ds, capacity * sizeof(BacSi));
            if (temp == NULL) {
                perror("Loi cap phat bo nho (realloc docDanhSachBacSi)");
                free(ds);
                fclose(f);
                *soLuong = 0;
                return NULL;
            }
            ds = temp;
        }

        BacSi bs;
        token = strtok(line, ",");
        if (token) { strncpy(bs.maBacSi, token, sizeof(bs.maBacSi) - 1); bs.maBacSi[sizeof(bs.maBacSi) - 1] = '\0'; }
        else continue;

        token = strtok(NULL, ",");
        if (token) { strncpy(bs.tenBacSi, token, sizeof(bs.tenBacSi) - 1); bs.tenBacSi[sizeof(bs.tenBacSi) - 1] = '\0'; }
        else bs.tenBacSi[0] = '\0';

        token = strtok(NULL, ",");
        if (token) { strncpy(bs.chuyenKhoa, token, sizeof(bs.chuyenKhoa) - 1); bs.chuyenKhoa[sizeof(bs.chuyenKhoa) - 1] = '\0'; }
        else bs.chuyenKhoa[0] = '\0';

        token = strtok(NULL, ",");
        if (token) { strncpy(bs.soDienThoai, token, sizeof(bs.soDienThoai) - 1); bs.soDienThoai[sizeof(bs.soDienThoai) - 1] = '\0'; }
        else bs.soDienThoai[0] = '\0';

        token = strtok(NULL, ",");
        if (token) { strncpy(bs.email, token, sizeof(bs.email) - 1); bs.email[sizeof(bs.email) - 1] = '\0'; }
        else bs.email[0] = '\0';

        token = strtok(NULL, "\n");
        if (token) { strncpy(bs.maKhoaPhong, token, sizeof(bs.maKhoaPhong) - 1); bs.maKhoaPhong[sizeof(bs.maKhoaPhong) - 1] = '\0'; }
        else bs.maKhoaPhong[0] = '\0';

        ds[*soLuong] = bs;
        (*soLuong)++;
    }

    fclose(f);
    return ds;
}

// Hàm ghiDanhSachBacSiRaFile
void ghiDanhSachBacSiRaFile(const char* tenFile, const BacSi dsBacSi[], int soLuong) {
    FILE* f = fopen(tenFile, "w");
    if (f == NULL) {
        perror("Loi mo file de ghi (ghiDanhSachBacSiRaFile)");
        return;
    }
    for (int i = 0; i < soLuong; ++i) {
        fprintf(f, "%s,%s,%s,%s,%s,%s\n",
            dsBacSi[i].maBacSi, dsBacSi[i].tenBacSi, dsBacSi[i].chuyenKhoa,
            dsBacSi[i].soDienThoai, dsBacSi[i].email, dsBacSi[i].maKhoaPhong);
    }
    fclose(f);
}

// --- Các hàm tìm kiếm nội bộ (static) theo các thuộc tính không phải khóa chính ---
// timBacSiTheoTen: Tên được phép trùng, nên hàm này vẫn giữ nguyên logic tìm bác sĩ đầu tiên khớp tên (nếu có).
// Trong ngữ cảnh kiểm tra trùng lặp khi thêm mới, việc tên trùng không còn là lỗi.
static BacSi* timBacSiTheoTen(const BacSi dsBacSi[], int soLuong, const char* tenBacSi) {
    if (tenBacSi == NULL || strlen(tenBacSi) == 0) return NULL;
    char tenBacSiLower[sizeof(dsBacSi[0].tenBacSi)];
    strncpy(tenBacSiLower, tenBacSi, sizeof(tenBacSiLower) - 1);
    tenBacSiLower[sizeof(tenBacSiLower) - 1] = '\0';
    for (int k = 0; tenBacSiLower[k]; k++) tenBacSiLower[k] = tolower(tenBacSiLower[k]);

    for (int i = 0; i < soLuong; ++i) {
        char currentTenBSLower[sizeof(dsBacSi[i].tenBacSi)];
        strncpy(currentTenBSLower, dsBacSi[i].tenBacSi, sizeof(currentTenBSLower) - 1);
        currentTenBSLower[sizeof(currentTenBSLower) - 1] = '\0';
        for (int k = 0; currentTenBSLower[k]; k++) currentTenBSLower[k] = tolower(currentTenBSLower[k]);

        if (strcmp(currentTenBSLower, tenBacSiLower) == 0) {
            return (BacSi*)&dsBacSi[i];
        }
    }
    return NULL;
}

static void toLowerString(char* dest, const char* src, size_t dest_size) {
    if (src == NULL || dest == NULL || dest_size == 0) return;
    size_t i = 0;
    for (i = 0; src[i] && i < dest_size - 1; ++i) {
        dest[i] = tolower((unsigned char)src[i]);
    }
    dest[i] = '\0';
}

// Hàm tìm bác sĩ theo tên (static, tuyến tính, tìm kiếm chứa)
// Hàm này trả về bác sĩ đầu tiên khớp, hoặc có thể được điều chỉnh để dùng trong timKiemBacSiTuyenTinh
static int kiemTraTenBacSi(const char* tenDayDu, const char* tenTimKiemLower) {
    if (tenDayDu == NULL || tenTimKiemLower == NULL) return 0;
    char tenDayDuLower[sizeof(((BacSi*)0)->tenBacSi)]; // Lấy kích thước từ struct
    toLowerString(tenDayDuLower, tenDayDu, sizeof(tenDayDuLower));
    return (strstr(tenDayDuLower, tenTimKiemLower) != NULL);
}

// Hàm tìm bác sĩ theo chuyên khoa (static, tuyến tính, tìm kiếm chứa)
static int kiemTraChuyenKhoaBacSi(const char* chuyenKhoaBS, const char* chuyenKhoaTimKiemLower) {
    if (chuyenKhoaBS == NULL || chuyenKhoaTimKiemLower == NULL) return 0;
    char chuyenKhoaBSLower[sizeof(((BacSi*)0)->chuyenKhoa)];
    toLowerString(chuyenKhoaBSLower, chuyenKhoaBS, sizeof(chuyenKhoaBSLower));
    return (strstr(chuyenKhoaBSLower, chuyenKhoaTimKiemLower) != NULL);
}

// Các hàm timBacSiTheoSDT và timBacSiTheoEmail đã được bỏ static
// Chúng thực hiện khớp chính xác (Email không phân biệt hoa thường)

BacSi* timBacSiTheoSDT(const BacSi dsBacSi[], int soLuong, const char* soDienThoai) { //
    if (soDienThoai == NULL || strlen(soDienThoai) == 0) return NULL; //
    for (int i = 0; i < soLuong; ++i) { //
        if (strcmp(dsBacSi[i].soDienThoai, soDienThoai) == 0) { //
            return (BacSi*)&dsBacSi[i]; //
        }
    }
    return NULL; //
}

BacSi* timBacSiTheoEmail(const BacSi dsBacSi[], int soLuong, const char* email) { //
    if (email == NULL || strlen(email) == 0) return NULL; //
    char emailLower[sizeof(((BacSi*)0)->email)]; //
    toLowerString(emailLower, email, sizeof(emailLower)); //

    for (int i = 0; i < soLuong; ++i) { //
        char currentEmailLower[sizeof(dsBacSi[i].email)]; //
        toLowerString(currentEmailLower, dsBacSi[i].email, sizeof(currentEmailLower)); //

        if (strcmp(currentEmailLower, emailLower) == 0) { //
            return (BacSi*)&dsBacSi[i]; //
        }
    }
    return NULL; //
}


void timKiemBacSiTuyenTinh(const BacSi dsBacSi[], int soLuongBacSi,
    const char* tieuChi, const char* giaTriTimKiem,
    BacSi** ketQuaTimKiem, int* soLuongKetQua) {
    *ketQuaTimKiem = NULL;
    *soLuongKetQua = 0;
    if (soLuongBacSi == 0 || giaTriTimKiem == NULL || tieuChi == NULL || strlen(giaTriTimKiem) == 0) {
        return;
    }

    // Cấp phát mảng tạm để lưu các con trỏ tới bác sĩ tìm thấy (hoặc bản sao)
    // Để đơn giản, chúng ta sẽ tạo một mảng bản sao các bác sĩ tìm thấy.
    BacSi* dsKetQuaTamThoi = (BacSi*)malloc(soLuongBacSi * sizeof(BacSi));
    if (dsKetQuaTamThoi == NULL) {
        perror("Loi cap phat bo nho (timKiemBacSiTuyenTinh: dsKetQuaTamThoi)");
        return;
    }
    int count = 0;

    char tieuChiLower[50];
    toLowerString(tieuChiLower, tieuChi, sizeof(tieuChiLower));

    char giaTriTimKiemLower[sizeof(((BacSi*)0)->tenBacSi)]; // Lấy kích thước lớn nhất có thể
    toLowerString(giaTriTimKiemLower, giaTriTimKiem, sizeof(giaTriTimKiemLower));

    for (int i = 0; i < soLuongBacSi; ++i) {
        int timThayTrongBacSiNay = 0;
        if (strcmp(tieuChiLower, "ten") == 0) {
            if (kiemTraTenBacSi(dsBacSi[i].tenBacSi, giaTriTimKiemLower)) {
                timThayTrongBacSiNay = 1;
            }
        }
        else if (strcmp(tieuChiLower, "chuyenkhoa") == 0) {
            if (kiemTraChuyenKhoaBacSi(dsBacSi[i].chuyenKhoa, giaTriTimKiemLower)) {
                timThayTrongBacSiNay = 1;
            }
        }
        else if (strcmp(tieuChiLower, "sodienthoai") == 0) {
            // SĐT thường là khớp chính xác và không cần lowercase
            if (strcmp(dsBacSi[i].soDienThoai, giaTriTimKiem) == 0) {
                timThayTrongBacSiNay = 1;
            }
        }
        else if (strcmp(tieuChiLower, "email") == 0) {
            char currentEmailLower[sizeof(dsBacSi[i].email)];
            toLowerString(currentEmailLower, dsBacSi[i].email, sizeof(currentEmailLower));
            if (strcmp(currentEmailLower, giaTriTimKiemLower) == 0) { // Email khớp chính xác, không phân biệt hoa thường
                timThayTrongBacSiNay = 1;
            }
        }
        // Có thể thêm tìm kiếm theo MaKhoaPhong nếu muốn

        if (timThayTrongBacSiNay) {
            if (count < soLuongBacSi) { // Đảm bảo không ghi quá mảng tạm
                dsKetQuaTamThoi[count++] = dsBacSi[i];
            }
        }
    }

    if (count > 0) {
        *ketQuaTimKiem = (BacSi*)malloc(count * sizeof(BacSi));
        if (*ketQuaTimKiem == NULL) {
            perror("Loi cap phat bo nho (timKiemBacSiTuyenTinh: ketQuaTimKiem)");
            free(dsKetQuaTamThoi);
            *soLuongKetQua = 0; // Đặt lại để tránh lỗi
            return;
        }
        memcpy(*ketQuaTimKiem, dsKetQuaTamThoi, count * sizeof(BacSi));
        *soLuongKetQua = count;
    }

    free(dsKetQuaTamThoi);
}


// CẬP NHẬT: nhapThongTinBacSiTuBanPhim
int nhapThongTinBacSiTuBanPhim(BacSi* bs,
    BangBam* bbBacSiHienCo,
    const BacSi dsBacSiHienCo[], int soLuongBSHienCo,
    const KhoaPhong dsKhoaPhong[], int soLuongKhoaPhong) {
    char bufferMaBS[MAX_LEN_MA_BAC_SI];
    char bufferMaKP[MAX_LEN_MA_KHOA_PHONG];
    BacSi* bsTimThay;

    printf("Nhap Ma Bac Si (VD: BS001, toi da %zu ky tu, se duoc chuyen IN HOA): ", sizeof(bs->maBacSi) - 1);
    if (docDongAnToan(bufferMaBS, sizeof(bufferMaBS), stdin) == NULL || strlen(bufferMaBS) == 0) {
        printf("LOI: Ma Bac Si khong duoc de trong. Huy them bac si.\n");
        return 0;
    }
    for (int i = 0; bufferMaBS[i]; i++) {
        bufferMaBS[i] = toupper(bufferMaBS[i]);
    }

    if (bbBacSiHienCo != NULL) {
        if (timKiemTrongBangBam(bbBacSiHienCo, bufferMaBS) != NULL) {
            printf("LOI: Ma Bac Si '%s' da ton tai (kiem tra bang bam). Huy them bac si.\n", bufferMaBS);
            return 0;
        }
    }
    else {
        fprintf(stderr, "LOI HE THONG: Bang bam bac si khong kha dung. Khong the kiem tra trung lap MaBS. Huy them bac si.\n");
        return 0;
    }
    strncpy(bs->maBacSi, bufferMaBS, sizeof(bs->maBacSi) - 1);
    bs->maBacSi[sizeof(bs->maBacSi) - 1] = '\0';

    // Nhập Tên Bác Sĩ (bắt buộc, được phép trùng)
    printf("Nhap Ten Bac Si (KHONG DUOC BO TRONG): ");
    if (docDongAnToan(bs->tenBacSi, sizeof(bs->tenBacSi), stdin) == NULL || strlen(bs->tenBacSi) == 0) {
        printf("LOI: Ten Bac Si khong duoc de trong. Huy them bac si.\n");
        return 0;
    }
    // Không còn kiểm tra trùng tên ở đây nữa

    printf("Nhap Chuyen Khoa (KHONG DUOC BO TRONG): ");
    if (docDongAnToan(bs->chuyenKhoa, sizeof(bs->chuyenKhoa), stdin) == NULL || strlen(bs->chuyenKhoa) == 0) {
        printf("LOI: Chuyen khoa khong duoc de trong. Huy them bac si.\n");
        return 0;
    }

    // Nhập và kiểm tra Số Điện Thoại (bắt buộc, KHÔNG TRÙNG, chỉ chứa số)
    printf("Nhap So Dien Thoai (KHONG DUOC BO TRONG, chi chua so, KHONG TRUNG): ");
    if (docDongAnToan(bs->soDienThoai, sizeof(bs->soDienThoai), stdin) == NULL || strlen(bs->soDienThoai) == 0) {
        printf("LOI: So Dien Thoai khong duoc de trong. Huy them bac si.\n");
        return 0;
    }
    for (int i = 0; bs->soDienThoai[i]; ++i) {
        if (!isdigit(bs->soDienThoai[i])) {
            printf("LOI: So Dien Thoai chi duoc chua cac ky tu so. Huy them bac si.\n");
            return 0;
        }
    }
    bsTimThay = timBacSiTheoSDT(dsBacSiHienCo, soLuongBSHienCo, bs->soDienThoai);
    if (bsTimThay != NULL) { // Nếu tìm thấy SDT này ở bác sĩ khác
        printf("LOI: So Dien Thoai '%s' da duoc su dung boi Bac Si MaBS: %s. Huy them bac si.\n", bs->soDienThoai, bsTimThay->maBacSi);
        return 0;
    }

    // Nhập và kiểm tra Email (bắt buộc, KHÔNG TRÙNG, định dạng cơ bản)
    printf("Nhap Email (KHONG DUOC BO TRONG, vd: ten@domain.com, KHONG TRUNG): ");
    if (docDongAnToan(bs->email, sizeof(bs->email), stdin) == NULL || strlen(bs->email) == 0) {
        printf("LOI: Email khong duoc de trong. Huy them bac si.\n");
        return 0;
    }
    if (strchr(bs->email, '@') == NULL || strchr(bs->email, '.') == NULL) {
        printf("LOI: Dinh dang Email khong hop le. Huy them bac si.\n");
        return 0;
    }
    bsTimThay = timBacSiTheoEmail(dsBacSiHienCo, soLuongBSHienCo, bs->email);
    if (bsTimThay != NULL) { // Nếu tìm thấy email này ở bác sĩ khác
        printf("LOI: Email '%s' da duoc su dung boi Bac Si MaBS: %s. Huy them bac si.\n", bs->email, bsTimThay->maBacSi);
        return 0;
    }

    printf("Nhap Ma Khoa Phong (phai ton tai, IN HOA, KHONG DUOC BO TRONG): ");
    if (docDongAnToan(bufferMaKP, sizeof(bufferMaKP), stdin) == NULL || strlen(bufferMaKP) == 0) {
        printf("LOI: Ma Khoa Phong khong duoc de trong. Huy them bac si.\n");
        return 0;
    }
    for (int i = 0; bufferMaKP[i]; i++) {
        bufferMaKP[i] = toupper(bufferMaKP[i]);
    }
    if (timKhoaPhongTheoMa(dsKhoaPhong, soLuongKhoaPhong, bufferMaKP) == NULL) {
        printf("LOI: Ma Khoa Phong '%s' khong ton tai! Huy them bac si.\n", bufferMaKP);
        return 0;
    }
    strncpy(bs->maKhoaPhong, bufferMaKP, sizeof(bs->maKhoaPhong) - 1);
    bs->maKhoaPhong[sizeof(bs->maKhoaPhong) - 1] = '\0';

    return 1;
}

// CẬP NHẬT: themBacSi (Logic không thay đổi nhiều, chủ yếu dựa vào nhapThongTinBacSiTuBanPhim đã kiểm tra)
int themBacSi(BacSi** dsBacSiConTro, int* soLuongConTro,
    BangBam** bbBacSi_ptr,
    BacSi bacSiMoi, const char* tenFile) {

    BangBam* bbHienTai = *bbBacSi_ptr;

    if (bbHienTai != NULL) {
        if (timKiemTrongBangBam(bbHienTai, bacSiMoi.maBacSi) != NULL) {
            fprintf(stderr, "LOI NGHIEM TRONG: Ma Bac Si '%s' da ton tai trong bang bam ngay truoc khi them vao mang (logic themBacSi)!\n", bacSiMoi.maBacSi);
            return 0;
        }
    }
    else {
        fprintf(stderr, "LOI HE THONG: Bang bam bac si khong kha dung (NULL) khi them bac si '%s'.\n", bacSiMoi.maBacSi);
        return 0;
    }

    BacSi* temp = realloc(*dsBacSiConTro, (*soLuongConTro + 1) * sizeof(BacSi));
    if (temp == NULL) {
        perror("Loi cap phat bo nho (realloc themBacSi)");
        return 0;
    }
    *dsBacSiConTro = temp;
    (*dsBacSiConTro)[*soLuongConTro] = bacSiMoi;
    BacSi* conTroToiBsTrongMang = &((*dsBacSiConTro)[*soLuongConTro]);

    if (!chenVaoBangBam(bbHienTai, bacSiMoi.maBacSi, conTroToiBsTrongMang)) {
        fprintf(stderr, "LOI NGHIEM TRONG: Khong the chen Bac Si '%s' vao bang bam.\n", bacSiMoi.maBacSi);
        return 0;
    }

    (*soLuongConTro)++;

    if ((*bbBacSi_ptr)->soLuongPhanTu / (double)(*bbBacSi_ptr)->dungLuong > BANG_BAM_NGUONG_TAI_TOI_DA) {
        printf("Thong bao: He so tai cua bang bam BAC SI vuot nguong (%d/%d). Dang chuan bi rehash...\n",
            (*bbBacSi_ptr)->soLuongPhanTu, (*bbBacSi_ptr)->dungLuong);
        if (!rehashBangBam(bbBacSi_ptr)) {
            fprintf(stderr, "LOI: Rehash bang bam BAC SI that bai. Chuong trinh co the tiep tuc nhung hieu suat se giam.\n");
        }
    }

    ghiDanhSachBacSiRaFile(tenFile, *dsBacSiConTro, *soLuongConTro);
    return 1;
}

// CẬP NHẬT: suaThongTinBacSi
int suaThongTinBacSi(BacSi dsBacSi[], int soLuong, const char* maBacSiCanSua, BacSi thongTinMoi,
    const KhoaPhong dsKhoaPhong[], int soLuongKhoaPhong,
    const char* tenFile) {
    int indexCanSua = -1;
    char maCanSuaUpper[MAX_LEN_MA_BAC_SI];
    strncpy(maCanSuaUpper, maBacSiCanSua, sizeof(maCanSuaUpper) - 1);
    maCanSuaUpper[sizeof(maCanSuaUpper) - 1] = '\0';
    for (int i = 0; maCanSuaUpper[i]; i++) maCanSuaUpper[i] = toupper(maCanSuaUpper[i]);

    for (int i = 0; i < soLuong; ++i) {
        if (strcmp(dsBacSi[i].maBacSi, maCanSuaUpper) == 0) {
            indexCanSua = i;
            break;
        }
    }

    if (indexCanSua == -1) {
        printf("Khong tim thay bac si voi ma '%s' de sua.\n", maBacSiCanSua);
        return 0;
    }

    BacSi* bsTimThay;

    // Xác thực Tên Bác Sĩ mới (được phép trùng, nhưng không được rỗng)
    if (strlen(thongTinMoi.tenBacSi) == 0) {
        printf("Ten Bac Si khong duoc de trong.\n"); return 0;
    }
    // Không cần kiểm tra trùng tên nữa

    // Xác thực Số Điện Thoại mới (KHÔNG TRÙNG với bác sĩ KHÁC)
    if (strlen(thongTinMoi.soDienThoai) == 0) {
        printf("So Dien Thoai khong duoc de trong.\n"); return 0;
    }
    for (int i = 0; thongTinMoi.soDienThoai[i]; ++i) {
        if (!isdigit(thongTinMoi.soDienThoai[i])) {
            printf("So Dien Thoai moi chi duoc chua cac ky tu so.\n"); return 0;
        }
    }
    bsTimThay = timBacSiTheoSDT(dsBacSi, soLuong, thongTinMoi.soDienThoai);
    if (bsTimThay != NULL && strcmp(bsTimThay->maBacSi, maCanSuaUpper) != 0) { // Nếu tìm thấy SDT và đó là của bác sĩ khác
        printf("Loi: So Dien Thoai '%s' da duoc su dung boi Bac Si MaBS: %s.\n", thongTinMoi.soDienThoai, bsTimThay->maBacSi);
        return 0;
    }

    // Xác thực Email mới (KHÔNG TRÙNG với bác sĩ KHÁC)
    if (strlen(thongTinMoi.email) == 0) {
        printf("Email khong duoc de trong.\n"); return 0;
    }
    if (strchr(thongTinMoi.email, '@') == NULL || strchr(thongTinMoi.email, '.') == NULL) {
        printf("Dinh dang Email moi khong hop le.\n"); return 0;
    }
    bsTimThay = timBacSiTheoEmail(dsBacSi, soLuong, thongTinMoi.email);
    if (bsTimThay != NULL && strcmp(bsTimThay->maBacSi, maCanSuaUpper) != 0) { // Nếu tìm thấy email và đó là của bác sĩ khác
        printf("Loi: Email '%s' da duoc su dung boi Bac Si MaBS: %s.\n", thongTinMoi.email, bsTimThay->maBacSi);
        return 0;
    }

    char maKPMoiUpper[MAX_LEN_MA_KHOA_PHONG];
    if (strlen(thongTinMoi.maKhoaPhong) == 0) {
        printf("Loi: Ma Khoa Phong khong duoc de trong.\n"); return 0;
    }
    strncpy(maKPMoiUpper, thongTinMoi.maKhoaPhong, sizeof(maKPMoiUpper) - 1);
    maKPMoiUpper[sizeof(maKPMoiUpper) - 1] = '\0';
    for (int i = 0; maKPMoiUpper[i]; i++) maKPMoiUpper[i] = toupper(maKPMoiUpper[i]);

    if (timKhoaPhongTheoMa(dsKhoaPhong, soLuongKhoaPhong, maKPMoiUpper) == NULL) {
        printf("Loi: Ma Khoa Phong moi '%s' khong ton tai!\n", maKPMoiUpper);
        return 0;
    }
    strcpy(thongTinMoi.maKhoaPhong, maKPMoiUpper);

    strcpy(dsBacSi[indexCanSua].tenBacSi, thongTinMoi.tenBacSi);
    strcpy(dsBacSi[indexCanSua].chuyenKhoa, thongTinMoi.chuyenKhoa);
    strcpy(dsBacSi[indexCanSua].soDienThoai, thongTinMoi.soDienThoai);
    strcpy(dsBacSi[indexCanSua].email, thongTinMoi.email);
    strcpy(dsBacSi[indexCanSua].maKhoaPhong, thongTinMoi.maKhoaPhong);

    ghiDanhSachBacSiRaFile(tenFile, dsBacSi, soLuong);
    return 1;
}

// CẬP NHẬT: xoaBacSi (Logic không thay đổi nhiều)
int xoaBacSi(BacSi** dsBacSiConTro, int* soLuongConTro,
    BangBam* bbBacSi,
    const char* maBacSiCanXoa, const char* tenFile) {
    char maXoaUpper[MAX_LEN_MA_BAC_SI];
    strncpy(maXoaUpper, maBacSiCanXoa, sizeof(maXoaUpper) - 1);
    maXoaUpper[sizeof(maXoaUpper) - 1] = '\0';
    for (int k = 0; maXoaUpper[k]; k++) maXoaUpper[k] = toupper(maXoaUpper[k]);

    int removedFromHashTable = 0;
    if (bbBacSi != NULL) {
        if (timKiemTrongBangBam(bbBacSi, maXoaUpper) != NULL) {
            if (xoaKhoiBangBam(bbBacSi, maXoaUpper)) {
                removedFromHashTable = 1;
            }
            else {
                fprintf(stderr, "LOI: Khong the xoa Bac Si '%s' khoi bang bam mac du da tim thay.\n", maXoaUpper);
                return 0;
            }
        }
    }
    else {
        fprintf(stderr, "LOI HE THONG: Bang bam bac si khong kha dung khi xoa.\n");
        return 0;
    }

    int viTriCanXoa = -1;
    for (int i = 0; i < *soLuongConTro; ++i) {
        if (strcmp((*dsBacSiConTro)[i].maBacSi, maXoaUpper) == 0) {
            viTriCanXoa = i;
            break;
        }
    }

    if (viTriCanXoa != -1) {
        if (!removedFromHashTable && timKiemTrongBangBam(bbBacSi, maXoaUpper) == NULL) {
            fprintf(stderr, "CANH BAO: Bac Si '%s' tim thay trong mang nhung khong co trong bang bam truoc khi xoa khoi mang.\n", maXoaUpper);
        }

        for (int i = viTriCanXoa; i < (*soLuongConTro - 1); ++i) {
            (*dsBacSiConTro)[i] = (*dsBacSiConTro)[i + 1];
        }
        (*soLuongConTro)--;

        if (*soLuongConTro == 0) {
            free(*dsBacSiConTro);
            *dsBacSiConTro = NULL;
        }
        else {
            BacSi* temp = realloc(*dsBacSiConTro, (*soLuongConTro) * sizeof(BacSi));
            if (temp == NULL && *soLuongConTro > 0) {
                perror("Loi realloc khi xoa bac si (thu nho mang)");
            }
            else if (*soLuongConTro > 0) {
                *dsBacSiConTro = temp;
            }
        }
        ghiDanhSachBacSiRaFile(tenFile, *dsBacSiConTro, *soLuongConTro);
        return 1;
    }

    if (removedFromHashTable && viTriCanXoa == -1) {
        fprintf(stderr, "LOI DONG BO: Da xoa Bac Si '%s' khoi bang bam nhung khong tim thay trong mang.\n", maXoaUpper);
        return 0;
    }

    printf("Khong tim thay bac si voi ma '%s' trong he thong de xoa.\n", maXoaUpper);
    return 0;
}

// Hàm inDanhSachBacSiConsole
void inDanhSachBacSiConsole(const BacSi dsBacSi[], int soLuong) {
    printf("\n--- DANH SACH BAC SI ---\n");
    if (soLuong == 0) {
        printf("Khong co bac si nao trong danh sach.\n");
        return;
    }
    printf("---------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-5s | %-10s | %-30s | %-20s | %-15s | %-25s | %-20s |\n", "STT", "Ma BS", "Ten Bac Si", "Chuyen Khoa", "So Dien Thoai", "Email", "Ma Khoa Phong");
    printf("---------------------------------------------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < soLuong; ++i) {
        printf("| %-5d | %-10s | %-30s | %-20s | %-15s | %-25s | %-20s |\n",
            i + 1, dsBacSi[i].maBacSi, dsBacSi[i].tenBacSi, dsBacSi[i].chuyenKhoa,
            dsBacSi[i].soDienThoai, dsBacSi[i].email, dsBacSi[i].maKhoaPhong);
    }
    printf("---------------------------------------------------------------------------------------------------------------------------------------------------\n");
}

// HÀM MỚI: timBacSiTheoMaBangBam
BacSi* timBacSiTheoMaBangBam(BangBam* bbBacSi, const char* maBacSi) {
    if (bbBacSi == NULL || maBacSi == NULL) {
        return NULL;
    }
    char maBSTimKiem[MAX_LEN_MA_BAC_SI];
    strncpy(maBSTimKiem, maBacSi, sizeof(maBSTimKiem) - 1);
    maBSTimKiem[sizeof(maBSTimKiem) - 1] = '\0';
    for (int k = 0; maBSTimKiem[k]; k++) {
        maBSTimKiem[k] = toupper(maBSTimKiem[k]);
    }
    return (BacSi*)timKiemTrongBangBam(bbBacSi, maBSTimKiem);
}