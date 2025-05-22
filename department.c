#include "department.h"
#include "csv_handler.h" // Cho docDongAnToan
#include "hash_table.h"  // Đã bao gồm trong department.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Cho toupper

// Hàm tiện ích nội bộ (static) để xóa ký tự xuống dòng
static void removeNewlineDept(char* str) {
    if (str == NULL) return;
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

// Hàm đọc danh sách khoa phòng từ file CSV
// KHÔNG bỏ qua dòng tiêu đề.
KhoaPhong* docDanhSachKhoaPhong(const char* tenFile, int* soLuong) {
    FILE* f = fopen(tenFile, "r");
    if (f == NULL) {
        *soLuong = 0;
        return NULL;
    }

    KhoaPhong* ds = NULL;
    int capacity = 0;
    *soLuong = 0;
    char line[500];
    char* token;

    while (fgets(line, sizeof(line), f) != NULL) {
        removeNewlineDept(line);
        if (strlen(line) < 3) continue; // Bỏ qua các dòng quá ngắn, có thể là dòng trống

        if (*soLuong >= capacity) {
            capacity = (capacity == 0) ? 5 : capacity * 2;
            KhoaPhong* temp = realloc(ds, capacity * sizeof(KhoaPhong));
            if (temp == NULL) {
                perror("Loi cap phat bo nho (realloc docDanhSachKhoaPhong)");
                free(ds);
                fclose(f);
                *soLuong = 0;
                return NULL;
            }
            ds = temp;
        }

        KhoaPhong kp;
        // Định dạng CSV: maKhoaPhong,tenKhoaPhong,viTri,moTa
        token = strtok(line, ",");
        if (token && strlen(token) > 0) { // Mã khoa phòng không được rỗng từ file
            strncpy(kp.maKhoaPhong, token, sizeof(kp.maKhoaPhong) - 1); kp.maKhoaPhong[sizeof(kp.maKhoaPhong) - 1] = '\0';
        }
        else {
            fprintf(stderr, "CANH BAO (docDanhSachKhoaPhong): Dong khong co maKhoaPhong hop le. Bo qua dong.\n");
            continue; // Bỏ qua dòng nếu mã khoa phòng không hợp lệ
        }

        token = strtok(NULL, ",");
        if (token && strlen(token) > 0) { // Tên khoa phòng không được rỗng từ file
            strncpy(kp.tenKhoaPhong, token, sizeof(kp.tenKhoaPhong) - 1); kp.tenKhoaPhong[sizeof(kp.tenKhoaPhong) - 1] = '\0';
        }
        else {
            fprintf(stderr, "CANH BAO (docDanhSachKhoaPhong): Dong co maKhoaPhong '%s' khong co tenKhoaPhong hop le. Bo qua dong.\n", kp.maKhoaPhong);
            continue; // Bỏ qua dòng nếu tên khoa phòng không hợp lệ
        }

        token = strtok(NULL, ",");
        if (token && strlen(token) > 0) { // Vị trí không được rỗng từ file
            strncpy(kp.viTri, token, sizeof(kp.viTri) - 1); kp.viTri[sizeof(kp.viTri) - 1] = '\0';
        }
        else {
            fprintf(stderr, "CANH BAO (docDanhSachKhoaPhong): Dong co maKhoaPhong '%s' khong co viTri hop le. Bo qua dong.\n", kp.maKhoaPhong);
            continue; // Bỏ qua dòng nếu vị trí không hợp lệ
        }

        token = strtok(NULL, "\n");
        if (token && strlen(token) > 0) {
            strncpy(kp.moTa, token, sizeof(kp.moTa) - 1); kp.moTa[sizeof(kp.moTa) - 1] = '\0';
        }
        else {
            strcpy(kp.moTa, "Không có mô tả"); // Giá trị mặc định nếu mô tả trống từ file
        }

        ds[*soLuong] = kp;
        (*soLuong)++;
    }

    fclose(f);
    return ds;
}

// Hàm ghi danh sách khoa phòng ra file CSV
// KHÔNG ghi dòng tiêu đề.
void ghiDanhSachKhoaPhongRaFile(const char* tenFile, const KhoaPhong dsKhoaPhong[], int soLuong) {
    FILE* f = fopen(tenFile, "w");
    if (f == NULL) {
        perror("Loi mo file de ghi (ghiDanhSachKhoaPhongRaFile)");
        return;
    }

    for (int i = 0; i < soLuong; ++i) {
        fprintf(f, "%s,%s,%s,%s\n",
            dsKhoaPhong[i].maKhoaPhong,
            dsKhoaPhong[i].tenKhoaPhong,
            dsKhoaPhong[i].viTri,
            strlen(dsKhoaPhong[i].moTa) == 0 ? "Không có mô tả" : dsKhoaPhong[i].moTa); // Đảm bảo mô tả không bao giờ rỗng khi ghi
    }
    fclose(f);
}

// Hàm tìm khoa phòng theo mã (tìm kiếm tuyến tính)
KhoaPhong* timKhoaPhongTheoMa(const KhoaPhong dsKhoaPhong[], int soLuong, const char* maKhoaPhong) {
    if (maKhoaPhong == NULL) return NULL;
    char maKPTimKiemUpper[MAX_LEN_MA_KHOA_PHONG];
    strncpy(maKPTimKiemUpper, maKhoaPhong, sizeof(maKPTimKiemUpper) - 1);
    maKPTimKiemUpper[sizeof(maKPTimKiemUpper) - 1] = '\0';
    for (int i = 0; maKPTimKiemUpper[i]; i++) maKPTimKiemUpper[i] = toupper(maKPTimKiemUpper[i]);

    for (int i = 0; i < soLuong; ++i) {
        if (strcmp(dsKhoaPhong[i].maKhoaPhong, maKPTimKiemUpper) == 0) {
            return (KhoaPhong*)&dsKhoaPhong[i];
        }
    }
    return NULL;
}

// HÀM MỚI: Tìm khoa phòng theo mã sử dụng bảng băm
KhoaPhong* timKhoaPhongTheoMaBangBam(BangBam* bbKhoaPhong, const char* maKhoaPhong) {
    if (bbKhoaPhong == NULL || maKhoaPhong == NULL) {
        return NULL;
    }
    char maKPTimKiem[MAX_LEN_MA_KHOA_PHONG];
    strncpy(maKPTimKiem, maKhoaPhong, sizeof(maKPTimKiem) - 1);
    maKPTimKiem[sizeof(maKPTimKiem) - 1] = '\0';
    for (int k = 0; maKPTimKiem[k]; k++) {
        maKPTimKiem[k] = toupper(maKPTimKiem[k]);
    }
    return (KhoaPhong*)timKiemTrongBangBam(bbKhoaPhong, maKPTimKiem);
}

// CẬP NHẬT: nhapThongTinKhoaPhongTuBanPhim - Tất cả các trường đều bắt buộc, trừ mô tả.
int nhapThongTinKhoaPhongTuBanPhim(KhoaPhong* kp,
    BangBam* bbKhoaPhongHienCo,
    const KhoaPhong dsKhoaPhongHienCo[], int soLuongHienCo) {
    char bufferMa[MAX_LEN_MA_KHOA_PHONG];
    printf("Nhap Ma Khoa Phong (VD: KHOA_TIMMACH, toi da %zu ky tu, IN HOA, KHONG DUOC BO TRONG): ", sizeof(kp->maKhoaPhong) - 1);
    if (docDongAnToan(bufferMa, sizeof(bufferMa), stdin) == NULL || strlen(bufferMa) == 0) {
        printf("LOI: Ma Khoa Phong khong duoc de trong. Huy them khoa phong.\n");
        return 0;
    }
    for (int i = 0; bufferMa[i]; i++) {
        bufferMa[i] = toupper(bufferMa[i]);
    }

    if (strlen(bufferMa) == 0) { // Kiểm tra lại sau khi chuẩn hóa
        printf("LOI: Ma Khoa Phong khong duoc de trong sau khi chuan hoa. Huy them khoa phong.\n");
        return 0;
    }

    if (bbKhoaPhongHienCo != NULL) {
        if (timKiemTrongBangBam(bbKhoaPhongHienCo, bufferMa) != NULL) {
            printf("LOI: Ma Khoa Phong '%s' da ton tai (kiem tra bang bam). Huy them khoa phong.\n", bufferMa);
            return 0;
        }
    }
    else {
        fprintf(stderr, "LOI HE THONG: Bang bam khoa phong khong kha dung. Khong the kiem tra trung lap MaKhoaPhong. Huy them khoa phong.\n");
        return 0;
    }
    strncpy(kp->maKhoaPhong, bufferMa, sizeof(kp->maKhoaPhong) - 1);
    kp->maKhoaPhong[sizeof(kp->maKhoaPhong) - 1] = '\0';

    printf("Nhap Ten Khoa Phong (KHONG DUOC BO TRONG): ");
    if (docDongAnToan(kp->tenKhoaPhong, sizeof(kp->tenKhoaPhong), stdin) == NULL || strlen(kp->tenKhoaPhong) == 0) {
        printf("LOI: Ten Khoa Phong khong duoc de trong. Huy them khoa phong.\n");
        return 0;
    }
    // Kiểm tra trùng tên khoa phòng (tuyến tính) - Nếu muốn, có thể bỏ comment
    // for (int i = 0; i < soLuongHienCo; ++i) {
    //     if (strcmp(dsKhoaPhongHienCo[i].tenKhoaPhong, kp->tenKhoaPhong) == 0) {
    //         printf("LOI: Ten Khoa Phong '%s' da ton tai (MaKP: %s). Huy them khoa phong.\n", kp->tenKhoaPhong, dsKhoaPhongHienCo[i].maKhoaPhong);
    //         return 0; 
    //     }
    // }


    printf("Nhap Vi Tri (KHONG DUOC BO TRONG): ");
    if (docDongAnToan(kp->viTri, sizeof(kp->viTri), stdin) == NULL || strlen(kp->viTri) == 0) {
        printf("LOI: Vi tri khong duoc de trong. Huy them khoa phong.\n");
        return 0;
    }

    printf("Nhap Mo Ta (neu bo trong se la 'Không có mô tả'): ");
    if (docDongAnToan(kp->moTa, sizeof(kp->moTa), stdin) == NULL || strlen(kp->moTa) == 0) {
        strcpy(kp->moTa, "Không có mô tả");
    }
    return 1;
}
static void dept_toLowerString(char* dest, const char* src, size_t dest_size) {
    if (src == NULL || dest == NULL || dest_size == 0) return;
    size_t i = 0;
    for (i = 0; src[i] && i < dest_size - 1; ++i) {
        dest[i] = tolower((unsigned char)src[i]);
    }
    dest[i] = '\0';
}

// Hàm kiểm tra một chuỗi có chứa chuỗi con khác không (không phân biệt hoa thường)
static int kiemTraChuoiConKhongPhanBietHoaThuong(const char* chuoiGoc, const char* chuoiTimKiem) {
    if (chuoiGoc == NULL || chuoiTimKiem == NULL) return 0;

    // Kích thước buffer đủ lớn cho các trường có thể tìm kiếm
    char chuoiGocLower[201]; // Dựa trên kích thước lớn nhất của tenKhoaPhong, viTri, moTa
    char chuoiTimKiemLower[201];

    dept_toLowerString(chuoiGocLower, chuoiGoc, sizeof(chuoiGocLower));
    dept_toLowerString(chuoiTimKiemLower, chuoiTimKiem, sizeof(chuoiTimKiemLower));

    return (strstr(chuoiGocLower, chuoiTimKiemLower) != NULL);
}

void timKiemKhoaPhongTuyenTinh(const KhoaPhong dsKhoaPhong[], int soLuongKhoaPhong,
    const char* tieuChi, const char* giaTriTimKiem,
    KhoaPhong** ketQuaTimKiem, int* soLuongKetQua) {
    *ketQuaTimKiem = NULL;
    *soLuongKetQua = 0;
    if (soLuongKhoaPhong == 0 || giaTriTimKiem == NULL || tieuChi == NULL || strlen(giaTriTimKiem) == 0) {
        return;
    }

    KhoaPhong* dsKetQuaTamThoi = (KhoaPhong*)malloc(soLuongKhoaPhong * sizeof(KhoaPhong));
    if (dsKetQuaTamThoi == NULL) {
        perror("Loi cap phat bo nho (timKiemKhoaPhongTuyenTinh: dsKetQuaTamThoi)");
        return;
    }
    int count = 0;

    char tieuChiLower[50];
    dept_toLowerString(tieuChiLower, tieuChi, sizeof(tieuChiLower));
    // giaTriTimKiem sẽ được chuyển thành chữ thường bên trong kiemTraChuoiConKhongPhanBietHoaThuong

    for (int i = 0; i < soLuongKhoaPhong; ++i) {
        int timThayTrongKhoaPhongNay = 0;
        if (strcmp(tieuChiLower, "tenkhoaphong") == 0) {
            if (kiemTraChuoiConKhongPhanBietHoaThuong(dsKhoaPhong[i].tenKhoaPhong, giaTriTimKiem)) {
                timThayTrongKhoaPhongNay = 1;
            }
        }
        else if (strcmp(tieuChiLower, "mota") == 0) {
            if (kiemTraChuoiConKhongPhanBietHoaThuong(dsKhoaPhong[i].moTa, giaTriTimKiem)) {
                timThayTrongKhoaPhongNay = 1;
            }
        }
        else if (strcmp(tieuChiLower, "vitri") == 0) {
            if (kiemTraChuoiConKhongPhanBietHoaThuong(dsKhoaPhong[i].viTri, giaTriTimKiem)) {
                timThayTrongKhoaPhongNay = 1;
            }
        }

        if (timThayTrongKhoaPhongNay) {
            if (count < soLuongKhoaPhong) {
                dsKetQuaTamThoi[count++] = dsKhoaPhong[i];
            }
        }
    }

    if (count > 0) {
        *ketQuaTimKiem = (KhoaPhong*)malloc(count * sizeof(KhoaPhong));
        if (*ketQuaTimKiem == NULL) {
            perror("Loi cap phat bo nho (timKiemKhoaPhongTuyenTinh: ketQuaTimKiem)");
            free(dsKetQuaTamThoi);
            *soLuongKetQua = 0;
            return;
        }
        memcpy(*ketQuaTimKiem, dsKetQuaTamThoi, count * sizeof(KhoaPhong));
        *soLuongKetQua = count;
    }

    free(dsKetQuaTamThoi);
}
// CẬP NHẬT: themKhoaPhong (logic không thay đổi nhiều, dựa vào hàm nhập đã kiểm tra)
int themKhoaPhong(KhoaPhong** dsKhoaPhongConTro, int* soLuongConTro,
    BangBam** bbKhoaPhong_ptr,
    KhoaPhong khoaPhongMoi, const char* tenFile) {
    BangBam* bbHienTai = *bbKhoaPhong_ptr;

    if (bbHienTai != NULL) {
        if (timKiemTrongBangBam(bbHienTai, khoaPhongMoi.maKhoaPhong) != NULL) {
            fprintf(stderr, "LOI NGHIEM TRONG: Ma Khoa Phong '%s' da ton tai trong bang bam (logic themKhoaPhong)!\n", khoaPhongMoi.maKhoaPhong);
            return 0;
        }
    }
    else {
        fprintf(stderr, "LOI HE THONG: Bang bam khoa phong khong kha dung (NULL) khi them khoa phong '%s'.\n", khoaPhongMoi.maKhoaPhong);
        return 0;
    }

    KhoaPhong* temp = realloc(*dsKhoaPhongConTro, (*soLuongConTro + 1) * sizeof(KhoaPhong));
    if (temp == NULL) {
        perror("Loi cap phat bo nho (realloc themKhoaPhong)");
        return 0;
    }
    *dsKhoaPhongConTro = temp;
    (*dsKhoaPhongConTro)[*soLuongConTro] = khoaPhongMoi;
    KhoaPhong* conTroToiKPTrongMang = &((*dsKhoaPhongConTro)[*soLuongConTro]);

    if (!chenVaoBangBam(bbHienTai, khoaPhongMoi.maKhoaPhong, conTroToiKPTrongMang)) {
        fprintf(stderr, "LOI NGHIEM TRONG: Khong the chen Khoa Phong '%s' vao bang bam.\n", khoaPhongMoi.maKhoaPhong);
        return 0;
    }

    (*soLuongConTro)++;

    if ((*bbKhoaPhong_ptr)->soLuongPhanTu / (double)(*bbKhoaPhong_ptr)->dungLuong > BANG_BAM_NGUONG_TAI_TOI_DA) {
        printf("Thong bao: He so tai cua bang bam KHOA PHONG vuot nguong (%d/%d). Dang chuan bi rehash...\n",
            (*bbKhoaPhong_ptr)->soLuongPhanTu, (*bbKhoaPhong_ptr)->dungLuong);
        if (!rehashBangBam(bbKhoaPhong_ptr)) {
            fprintf(stderr, "LOI: Rehash bang bam KHOA PHONG that bai. Chuong trinh co the tiep tuc nhung hieu suat se giam.\n");
        }
    }

    ghiDanhSachKhoaPhongRaFile(tenFile, *dsKhoaPhongConTro, *soLuongConTro);
    return 1;
}

// CẬP NHẬT: suaThongTinKhoaPhong
// Ma Khoa Phong (khóa) là bất biến. Các trường khác không được rỗng.
int suaThongTinKhoaPhong(KhoaPhong dsKhoaPhong[], int soLuong, const char* maKhoaPhongCanSua, KhoaPhong thongTinMoi, const char* tenFile) {
    int indexCanSua = -1;
    char maCanSuaUpper[MAX_LEN_MA_KHOA_PHONG];
    strncpy(maCanSuaUpper, maKhoaPhongCanSua, sizeof(maCanSuaUpper) - 1);
    maCanSuaUpper[sizeof(maCanSuaUpper) - 1] = '\0';
    for (int i = 0; maCanSuaUpper[i]; i++) maCanSuaUpper[i] = toupper(maCanSuaUpper[i]);

    for (int i = 0; i < soLuong; ++i) {
        if (strcmp(dsKhoaPhong[i].maKhoaPhong, maCanSuaUpper) == 0) {
            indexCanSua = i;
            break;
        }
    }

    if (indexCanSua == -1) {
        printf("Khong tim thay khoa phong voi ma '%s' de sua.\n", maKhoaPhongCanSua);
        return 0;
    }

    // Kiểm tra các trường bắt buộc của thongTinMoi
    if (strlen(thongTinMoi.tenKhoaPhong) == 0) {
        printf("LOI: Ten Khoa Phong moi khong duoc de trong. Sua that bai.\n");
        return 0;
    }
    // Kiểm tra trùng tên khoa phòng khi sửa (nếu muốn)
    // for (int i = 0; i < soLuong; ++i) {
    //     if (i == indexCanSua) continue; // Bỏ qua chính khoa phòng đang sửa
    //     if (strcmp(dsKhoaPhong[i].tenKhoaPhong, thongTinMoi.tenKhoaPhong) == 0) {
    //         printf("LOI: Ten Khoa Phong moi '%s' da ton tai (MaKP: %s). Sua that bai.\n", thongTinMoi.tenKhoaPhong, dsKhoaPhong[i].maKhoaPhong);
    //         return 0;
    //     }
    // }


    if (strlen(thongTinMoi.viTri) == 0) {
        printf("LOI: Vi tri moi khong duoc de trong. Sua that bai.\n");
        return 0;
    }
    if (strlen(thongTinMoi.moTa) == 0) {
        strcpy(thongTinMoi.moTa, "Không có mô tả"); // Giá trị mặc định nếu mô tả bị xóa rỗng khi sửa
    }

    // Ma Khoa Phong không đổi, chỉ cập nhật các thông tin khác
    strcpy(dsKhoaPhong[indexCanSua].tenKhoaPhong, thongTinMoi.tenKhoaPhong);
    strcpy(dsKhoaPhong[indexCanSua].viTri, thongTinMoi.viTri);
    strcpy(dsKhoaPhong[indexCanSua].moTa, thongTinMoi.moTa);

    ghiDanhSachKhoaPhongRaFile(tenFile, dsKhoaPhong, soLuong);
    return 1;
}

// CẬP NHẬT: xoaKhoaPhong (logic không thay đổi nhiều)
int xoaKhoaPhong(KhoaPhong** dsKhoaPhongConTro, int* soLuongConTro,
    BangBam* bbKhoaPhong,
    const char* maKhoaPhongCanXoa, const char* tenFile) {
    char maXoaUpper[MAX_LEN_MA_KHOA_PHONG];
    strncpy(maXoaUpper, maKhoaPhongCanXoa, sizeof(maXoaUpper) - 1);
    maXoaUpper[sizeof(maXoaUpper) - 1] = '\0';
    for (int k = 0; maXoaUpper[k]; k++) maXoaUpper[k] = toupper(maXoaUpper[k]);

    int removedFromHashTable = 0;
    if (bbKhoaPhong != NULL) {
        if (timKiemTrongBangBam(bbKhoaPhong, maXoaUpper) != NULL) {
            if (xoaKhoiBangBam(bbKhoaPhong, maXoaUpper)) {
                removedFromHashTable = 1;
            }
            else {
                fprintf(stderr, "LOI: Khong the xoa Khoa Phong '%s' khoi bang bam mac du da tim thay.\n", maXoaUpper);
                return 0;
            }
        }
    }
    else {
        fprintf(stderr, "LOI HE THONG: Bang bam khoa phong khong kha dung khi xoa.\n");
        return 0;
    }

    int viTriCanXoa = -1;
    for (int i = 0; i < *soLuongConTro; ++i) {
        if (strcmp((*dsKhoaPhongConTro)[i].maKhoaPhong, maXoaUpper) == 0) {
            viTriCanXoa = i;
            break;
        }
    }

    if (viTriCanXoa != -1) {
        if (!removedFromHashTable && timKiemTrongBangBam(bbKhoaPhong, maXoaUpper) == NULL) {
            fprintf(stderr, "CANH BAO: Khoa Phong '%s' tim thay trong mang nhung khong co trong bang bam truoc khi xoa khoi mang.\n", maXoaUpper);
        }

        for (int i = viTriCanXoa; i < (*soLuongConTro - 1); ++i) {
            (*dsKhoaPhongConTro)[i] = (*dsKhoaPhongConTro)[i + 1];
        }
        (*soLuongConTro)--;

        if (*soLuongConTro == 0) {
            free(*dsKhoaPhongConTro);
            *dsKhoaPhongConTro = NULL;
        }
        else {
            KhoaPhong* temp = realloc(*dsKhoaPhongConTro, (*soLuongConTro) * sizeof(KhoaPhong));
            if (temp == NULL && *soLuongConTro > 0) {
                perror("Loi realloc khi xoa khoa phong (thu nho mang)");
            }
            else if (*soLuongConTro > 0) {
                *dsKhoaPhongConTro = temp;
            }
        }
        ghiDanhSachKhoaPhongRaFile(tenFile, *dsKhoaPhongConTro, *soLuongConTro);
        return 1;
    }

    if (removedFromHashTable && viTriCanXoa == -1) {
        fprintf(stderr, "LOI DONG BO: Da xoa Khoa Phong '%s' khoi bang bam nhung khong tim thay trong mang.\n", maXoaUpper);
        return 0;
    }

    printf("Khong tim thay khoa phong voi ma '%s' trong he thong de xoa.\n", maXoaUpper);
    return 0;
}

// Hàm in danh sách khoa phòng ra console
void inDanhSachKhoaPhongConsole(const KhoaPhong dsKhoaPhong[], int soLuong) {
    printf("\n--- DANH SACH KHOA PHONG ---\n");
    if (soLuong == 0) {
        printf("Khong co khoa phong nao trong danh sach.\n");
        return;
    }
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-5s | %-20s | %-30s | %-20s | %-30s |\n", "STT", "Ma Khoa Phong", "Ten Khoa Phong", "Vi Tri", "Mo Ta");
    printf("-------------------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < soLuong; ++i) {
        char moTaDisplay[31]; // 30 chars + null
        rutGonChuoiHienThi(moTaDisplay, sizeof(moTaDisplay), dsKhoaPhong[i].moTa, 30); // Sử dụng csv_handler
        printf("| %-5d | %-20s | %-30s | %-20s | %-30s |\n",
            i + 1,
            dsKhoaPhong[i].maKhoaPhong,
            dsKhoaPhong[i].tenKhoaPhong,
            dsKhoaPhong[i].viTri,
            moTaDisplay);
    }
    printf("-------------------------------------------------------------------------------------------------------------------------\n");
}