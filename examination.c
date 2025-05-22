#include "examination.h"
#include "patient.h"        
#include "csv_handler.h"    
#include "doctor.h"         
#include "department.h"     
#include "priority_queue.h" // Cho các hàm hàng đợi mới theo khoa
#include "hash_table.h"     // Cho BangBam
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#define TEN_BN_LK_CONSOLE_WIDTH // Su dung ten define khac de tranh xung dot neu da co TEN_BN_LK_WIDTH
#define TEN_BN_LK_CONSOLE_WIDTH 25      // Do rong hien thi Ten Benh Nhan
#define LY_DO_KHAM_LK_CONSOLE_WIDTH 20  // Do rong hien thi Ly Do Kham
#define CHAN_DOAN_LK_CONSOLE_WIDTH 15   // Do rong hien thi Chan Doan
#define GHI_CHU_BS_LK_CONSOLE_WIDTH 15  // Do rong hien thi Ghi Chu Bac Si
#define MA_BAC_SI_LK_CONSOLE_WIDTH 15   // Do rong hien thi Ma Bac Si
#define MA_LAN_KHAM_STR_LEN 12 // Đủ để chứa số nguyên lớn + null

// Hàm tiện ích chuyển int sang chuỗi cho mã lần khám
static void intMaLKToString(int maLK, char* maLKStr, size_t bufferSize) {
    snprintf(maLKStr, bufferSize, "%d", maLK);
}

// --- Các hàm chuyển đổi Enum <-> String --- 
// Đảm bảo các hàm này đã được định nghĩa đầy đủ
int my_strcasecmp(const char* s1, const char* s2) { // Thêm hàm này nếu chưa có
    int c1, c2;
    do {
        c1 = tolower((unsigned char)*s1++);
        c2 = tolower((unsigned char)*s2++);
    } while (c1 == c2 && c1 != 0);
    return c1 - c2;
}

const char* trangThaiLanKhamToString(TrangThaiLanKham tt) {
    switch (tt) {
    case LK_DANG_CHO: return "DangCho";
    case LK_DANG_KHAM: return "DangKham";
    case LK_DA_HOAN_THANH: return "DaHoanThanh";
    case LK_DA_HUY: return "DaHuy";
    case LK_DA_LO: return "DaLo";
    default: return "KhongXacDinh";
    }
}

TrangThaiLanKham stringToTrangThaiLanKham(const char* str) {
    if (str == NULL || strlen(str) == 0) return LK_DANG_CHO;
    char lowerStr[50];
    strncpy(lowerStr, str, sizeof(lowerStr) - 1); lowerStr[sizeof(lowerStr) - 1] = '\0';
    for (int i = 0; lowerStr[i]; i++) lowerStr[i] = tolower(lowerStr[i]);

    if (my_strcasecmp(lowerStr, "dangcho") == 0) return LK_DANG_CHO;
    if (my_strcasecmp(lowerStr, "dangkham") == 0) return LK_DANG_KHAM;
    if (my_strcasecmp(lowerStr, "dahoanthanh") == 0) return LK_DA_HOAN_THANH;
    if (my_strcasecmp(lowerStr, "dahuy") == 0) return LK_DA_HUY;
    if (my_strcasecmp(lowerStr, "dalo") == 0) return LK_DA_LO;
    return LK_DANG_CHO; // Mặc định nếu không khớp
}

const char* mucDoUuTienToString(MucDoUuTien md) {
    switch (md) {
    case CAP_CUU: return "CapCuu";
    case KHAN_CAP: return "KhanCap";
    case THONG_THUONG: return "ThongThuong";
    case TAI_KHAM: return "TaiKham";
    case UU_TIEN_KHAC: return "UuTienKhac";
    default: return "KhongXacDinh";
    }
}

MucDoUuTien stringToMucDoUuTien(const char* str) {
    if (str == NULL || strlen(str) == 0) return THONG_THUONG;
    char lowerStr[50];
    strncpy(lowerStr, str, sizeof(lowerStr) - 1); lowerStr[sizeof(lowerStr) - 1] = '\0';
    for (int i = 0; lowerStr[i]; i++) lowerStr[i] = tolower(lowerStr[i]);

    if (my_strcasecmp(lowerStr, "capcuu") == 0) return CAP_CUU;
    if (my_strcasecmp(lowerStr, "khancap") == 0) return KHAN_CAP;
    if (my_strcasecmp(lowerStr, "thongthuong") == 0) return THONG_THUONG;
    if (my_strcasecmp(lowerStr, "taikham") == 0) return TAI_KHAM;
    if (my_strcasecmp(lowerStr, "uutienkhac") == 0) return UU_TIEN_KHAC;
    return THONG_THUONG; // Mặc định
}


// CẬP NHẬT: docDanhSachLanKham
LanKham* docDanhSachLanKham(const char* tenFile, int* soLuong) {
    FILE* f = fopen(tenFile, "r");
    if (f == NULL) {
        *soLuong = 0;
        return NULL;
    }

    LanKham* ds = NULL;
    int capacity = 0;
    *soLuong = 0;
    char line[2048]; // Buffer lớn hơn
    int lineNumber = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        lineNumber++;
        char originalLineForLogging[2048]; // Để log lỗi nếu có
        strncpy(originalLineForLogging, line, sizeof(originalLineForLogging) - 1);
        originalLineForLogging[sizeof(originalLineForLogging) - 1] = '\0';
        xoaKyTuXuongDong(originalLineForLogging); // Từ csv_handler.h

        xoaKyTuXuongDong(line);
        if (strlen(line) < 10) { // Kiểm tra cơ bản cho dòng hợp lệ
            if (strlen(line) > 0) { // Tránh log dòng hoàn toàn trống
                //fprintf(stderr, "CANH BAO (docDSLK Dong %d): Dong qua ngan, bo qua: \"%s\"\n", lineNumber, originalLineForLogging);
            }
            continue;
        }

        if (*soLuong >= capacity) {
            capacity = (capacity == 0) ? 20 : capacity * 2;
            LanKham* temp = realloc(ds, capacity * sizeof(LanKham));
            if (temp == NULL) {
                perror("Loi cap phat bo nho (realloc docDanhSachLanKham)");
                free(ds); fclose(f); *soLuong = 0; return NULL;
            }
            ds = temp;
        }

        LanKham lk;
        // Khởi tạo các trường về giá trị mặc định an toàn
        lk.maLanKham = 0;                lk.maDinhDanh[0] = '\0';        lk.tenBenhNhan[0] = '\0';
        lk.maBacSi[0] = '\0';            lk.maKhoaPhong[0] = '\0';       lk.ngayGioKham = 0;
        lk.lyDoKham[0] = '\0';           lk.chanDoan[0] = '\0';          lk.phuongPhapDieuTri[0] = '\0';
        lk.ghiChuBacSi[0] = '\0';        lk.ngayTaiKham.ngay = -1;       lk.ngayTaiKham.thang = -1;
        lk.ngayTaiKham.nam = -1;         lk.coLichTaiKham = 0;           lk.mucDoUuTien = THONG_THUONG;
        lk.gioDangKyThanhCong = 0;       lk.coDonThuoc = 0;              lk.trangThai = LK_DANG_CHO;
        lk.maDonThuocChiTiet = -1;

        char mucDoUuTienStr[50] = "";    char trangThaiStr[50] = "";
        long tempTimestamp;              char* token;
        char* lineCopyForStrtok = strdup(line); // strtok thay đổi chuỗi gốc
        if (!lineCopyForStrtok) { perror("Loi strdup lineCopyForStrtok"); continue; }
        char* p = lineCopyForStrtok;

        // Thứ tự các trường trong CSV:
        // maLanKham,maDinhDanh,tenBenhNhan,maBacSi,maKhoaPhong,ngayGioKham,lyDoKham,chanDoan,
        // phuongPhapDieuTri,ghiChuBacSi,ngayTaiKham.ngay,ngayTaiKham.thang,ngayTaiKham.nam,
        // coLichTaiKham,mucDoUuTien,gioDangKyThanhCong,coDonThuoc,trangThai,maDonThuoc(ChiTiet)

        token = strtok(p, ",");
        if (token && strlen(token) > 0 && strspn(token, "0123456789") == strlen(token)) { lk.maLanKham = atoi(token); }
        else { fprintf(stderr, "LOI DOC CSV (LanKham dong %d): MaLanKham ('%s') khong hop le. Bo qua dong: \"%s\"\n", lineNumber, token ? token : "NULL", originalLineForLogging); free(lineCopyForStrtok); continue; }

        token = strtok(NULL, ",");
        if (token && strlen(token) > 0) { strncpy(lk.maDinhDanh, token, sizeof(lk.maDinhDanh) - 1); lk.maDinhDanh[sizeof(lk.maDinhDanh) - 1] = '\0'; }
        else { fprintf(stderr, "LOI DOC CSV (LanKham dong %d): Thieu maDinhDanh cho MaLK %d. Bo qua dong.\n", lineNumber, lk.maLanKham); free(lineCopyForStrtok); continue; }

        token = strtok(NULL, ","); if (token) { strncpy(lk.tenBenhNhan, token, sizeof(lk.tenBenhNhan) - 1); lk.tenBenhNhan[sizeof(lk.tenBenhNhan) - 1] = '\0'; }
        else { strcpy(lk.tenBenhNhan, "Khong Biet"); } // Mặc định nếu trống

        token = strtok(NULL, ","); if (token && strlen(token) > 0) { strncpy(lk.maBacSi, token, sizeof(lk.maBacSi) - 1); lk.maBacSi[sizeof(lk.maBacSi) - 1] = '\0'; }
        else { strcpy(lk.maBacSi, "CHUA_GAN"); }

        token = strtok(NULL, ","); // maKhoaPhong
        if (token && strlen(token) > 0) { strncpy(lk.maKhoaPhong, token, sizeof(lk.maKhoaPhong) - 1); lk.maKhoaPhong[sizeof(lk.maKhoaPhong) - 1] = '\0'; }
        else { fprintf(stderr, "LOI DOC CSV (LanKham dong %d): Thieu maKhoaPhong cho MaLK %d. Bo qua dong.\n", lineNumber, lk.maLanKham); free(lineCopyForStrtok); continue; }


        token = strtok(NULL, ","); // ngayGioKham
        if (token && strlen(token) > 0 && strspn(token, "0123456789") == strlen(token)) { tempTimestamp = atol(token); if (tempTimestamp >= 0) lk.ngayGioKham = (time_t)tempTimestamp; }

        token = strtok(NULL, ","); // lyDoKham
        if (token && strlen(token) > 0) { strncpy(lk.lyDoKham, token, sizeof(lk.lyDoKham) - 1); lk.lyDoKham[sizeof(lk.lyDoKham) - 1] = '\0'; }
        else { strcpy(lk.lyDoKham, "Khong ro ly do"); }

        token = strtok(NULL, ","); // chanDoan
        if (token && strlen(token) > 0) { strncpy(lk.chanDoan, token, sizeof(lk.chanDoan) - 1); lk.chanDoan[sizeof(lk.chanDoan) - 1] = '\0'; }
        else { strcpy(lk.chanDoan, "CHUA_KHAM"); }

        token = strtok(NULL, ","); // phuongPhapDieuTri
        if (token && strlen(token) > 0) { strncpy(lk.phuongPhapDieuTri, token, sizeof(lk.phuongPhapDieuTri) - 1); lk.phuongPhapDieuTri[sizeof(lk.phuongPhapDieuTri) - 1] = '\0'; }
        else { strcpy(lk.phuongPhapDieuTri, "CHUA_KHAM"); }

        token = strtok(NULL, ","); // ghiChuBacSi
        if (token) { strncpy(lk.ghiChuBacSi, token, sizeof(lk.ghiChuBacSi) - 1); lk.ghiChuBacSi[sizeof(lk.ghiChuBacSi) - 1] = '\0'; }
        else { strcpy(lk.ghiChuBacSi, "-1"); } // Mặc định là -1 nếu trống

        token = strtok(NULL, ","); if (token && strlen(token) > 0 && strspn(token, "-0123456789") == strlen(token)) lk.ngayTaiKham.ngay = atoi(token); else lk.ngayTaiKham.ngay = -1;
        token = strtok(NULL, ","); if (token && strlen(token) > 0 && strspn(token, "-0123456789") == strlen(token)) lk.ngayTaiKham.thang = atoi(token); else lk.ngayTaiKham.thang = -1;
        token = strtok(NULL, ","); if (token && strlen(token) > 0 && strspn(token, "-0123456789") == strlen(token)) lk.ngayTaiKham.nam = atoi(token); else lk.ngayTaiKham.nam = -1;
        token = strtok(NULL, ","); if (token && strlen(token) > 0 && strspn(token, "01") == strlen(token)) lk.coLichTaiKham = atoi(token); else lk.coLichTaiKham = 0;

        token = strtok(NULL, ","); // mucDoUuTien
        if (token && strlen(token) > 0) { lk.mucDoUuTien = stringToMucDoUuTien(token); }

        token = strtok(NULL, ","); // gioDangKyThanhCong
        if (token && strlen(token) > 0 && strspn(token, "0123456789") == strlen(token)) { tempTimestamp = atol(token); if (tempTimestamp >= 0) lk.gioDangKyThanhCong = (time_t)tempTimestamp; }

        token = strtok(NULL, ","); if (token && strlen(token) > 0 && strspn(token, "01") == strlen(token)) lk.coDonThuoc = atoi(token); else lk.coDonThuoc = 0;

        token = strtok(NULL, ","); // trangThai (phải là token cuối cùng trước maDonThuoc)
        if (token && strlen(token) > 0) { lk.trangThai = stringToTrangThaiLanKham(token); }

        token = strtok(NULL, "\n"); // maDonThuocChiTiet
        if (token && strlen(token) > 0 && strspn(token, "-0123456789") == strlen(token)) lk.maDonThuocChiTiet = atoi(token); else lk.maDonThuocChiTiet = -1;

        // Kiểm tra bắt buộc sau khi đọc trạng thái
        if (lk.trangThai == LK_DA_HOAN_THANH) {
            if (strlen(lk.chanDoan) == 0 || strcmp(lk.chanDoan, "CHUA_KHAM") == 0) {
                fprintf(stderr, "CANH BAO (docDSLK Dong %d): ChanDoan rong/mac dinh cho MaLK %d (DaHoanThanh). Dat 'CHUA_NHAP_CD_DOC'.\n", lineNumber, lk.maLanKham);
                strcpy(lk.chanDoan, "CHUA_NHAP_CD_DOC");
            }
            if (strlen(lk.phuongPhapDieuTri) == 0 || strcmp(lk.phuongPhapDieuTri, "CHUA_KHAM") == 0) {
                fprintf(stderr, "CANH BAO (docDSLK Dong %d): PPDieuTri rong/mac dinh cho MaLK %d (DaHoanThanh). Dat 'CHUA_NHAP_PPTT_DOC'.\n", lineNumber, lk.maLanKham);
                strcpy(lk.phuongPhapDieuTri, "CHUA_NHAP_PPTT_DOC");
            }
        }

        free(lineCopyForStrtok);
        ds[*soLuong] = lk;
        (*soLuong)++;
    }
    fclose(f);
    return ds;
}

// CẬP NHẬT: ghiDanhSachLanKhamRaFile
void ghiDanhSachLanKhamRaFile(const char* tenFile, const LanKham dsLanKham[], int soLuong) {
    FILE* f = fopen(tenFile, "w");
    if (f == NULL) {
        perror("Loi mo file de ghi (ghiDanhSachLanKhamRaFile)");
        return;
    }
    // Không ghi header nếu file CSV không có
    for (int i = 0; i < soLuong; ++i) {
        const char* maKhoaPhongOut = (strlen(dsLanKham[i].maKhoaPhong) > 0) ? dsLanKham[i].maKhoaPhong : "KP_INVALID";
        const char* lyDoKhamOut = (strlen(dsLanKham[i].lyDoKham) > 0) ? dsLanKham[i].lyDoKham : "Khong ro";
        const char* chanDoanOut = (strlen(dsLanKham[i].chanDoan) > 0) ? dsLanKham[i].chanDoan : "CHUA_KHAM";
        const char* ppdtOut = (strlen(dsLanKham[i].phuongPhapDieuTri) > 0) ? dsLanKham[i].phuongPhapDieuTri : "CHUA_KHAM";

        if (dsLanKham[i].trangThai == LK_DA_HOAN_THANH) {
            if (strcmp(chanDoanOut, "CHUA_KHAM") == 0 || strlen(chanDoanOut) == 0) chanDoanOut = "CHUA_NHAP_CD_GHI";
            if (strcmp(ppdtOut, "CHUA_KHAM") == 0 || strlen(ppdtOut) == 0) ppdtOut = "CHUA_NHAP_PPTT_GHI";
        }

        fprintf(f, "%d,%s,%s,%s,%s,%ld,%s,%s,%s,%s,%d,%d,%d,%d,%s,%ld,%d,%s,%d\n",
            dsLanKham[i].maLanKham,
            (strlen(dsLanKham[i].maDinhDanh) > 0 ? dsLanKham[i].maDinhDanh : "BN_INVALID"),
            (strlen(dsLanKham[i].tenBenhNhan) > 0 ? dsLanKham[i].tenBenhNhan : "Ten Khong Biet"),
            (strlen(dsLanKham[i].maBacSi) == 0 ? "CHUA_GAN" : dsLanKham[i].maBacSi),
            maKhoaPhongOut,
            (long)dsLanKham[i].ngayGioKham,
            lyDoKhamOut,
            chanDoanOut,
            ppdtOut,
            (strlen(dsLanKham[i].ghiChuBacSi) == 0 ? "-1" : dsLanKham[i].ghiChuBacSi),
            dsLanKham[i].ngayTaiKham.ngay, dsLanKham[i].ngayTaiKham.thang, dsLanKham[i].ngayTaiKham.nam,
            dsLanKham[i].coLichTaiKham,
            mucDoUuTienToString(dsLanKham[i].mucDoUuTien),
            (long)dsLanKham[i].gioDangKyThanhCong,
            dsLanKham[i].coDonThuoc,
            trangThaiLanKhamToString(dsLanKham[i].trangThai),
            dsLanKham[i].maDonThuocChiTiet
        );
    }
    fclose(f);
}


int taoMaLanKhamMoi(const LanKham dsLanKham[], int soLuong) {
    if (soLuong == 0) return 1;
    int maxId = 0;
    for (int i = 0; i < soLuong; ++i) {
        if (dsLanKham[i].maLanKham > maxId) maxId = dsLanKham[i].maLanKham;
    }
    return maxId + 1;
}

// CẬP NHẬT: dangKyKhamMoi
int dangKyKhamMoi(LanKham** dsLanKhamConTro, int* soLuongLanKhamConTro,
    BenhNhan** dsBenhNhanConTro, int* soLuongBenhNhanConTro,
    BangBam* bbBenhNhan,
    KhoaPhong* dsKhoaPhong, int soLuongKhoaPhong,
    BangBam* bbKhoaPhong,
    BangBam** bbLanKham_ptr,
    BangBam* bbHangDoiTheoKhoa,
    const char* fileLanKhamCSV, const char* fileBenhNhanCSV) {

    // --- Phần tìm/tạo bệnh nhân ---
    char maBN_input[MAX_LEN_MA_DINH_DANH_BN];
    char maBN_upper[MAX_LEN_MA_DINH_DANH_BN];
    BenhNhan* bnTimThay = NULL;

    printf("Nhap Ma Dinh Danh Benh Nhan (KHONG DUOC BO TRONG): ");
    if (docDongAnToan(maBN_input, sizeof(maBN_input), stdin) == NULL || strlen(maBN_input) == 0) {
        printf("LOI: Ma Dinh Danh Benh Nhan khong duoc de trong. Huy dang ky.\n");
        return -1;
    }
    strncpy(maBN_upper, maBN_input, sizeof(maBN_upper) - 1);
    maBN_upper[sizeof(maBN_upper) - 1] = '\0';
    for (int k = 0; maBN_upper[k]; k++) maBN_upper[k] = toupper(maBN_upper[k]);

    if (bbBenhNhan != NULL) {
        bnTimThay = timBenhNhanTheoMaBangBam(bbBenhNhan, maBN_upper);
    }
    if (bnTimThay == NULL && dsBenhNhanConTro != NULL && *dsBenhNhanConTro != NULL) {
        bnTimThay = (BenhNhan*)timBenhNhanTheoMaChuoi(*dsBenhNhanConTro, *soLuongBenhNhanConTro, maBN_upper);
    }

    if (bnTimThay == NULL) {
        printf("Benh nhan voi Ma Dinh Danh %s khong ton tai.\n", maBN_upper);
        char choice_str[10];
        printf("Ban co muon tao moi ho so benh nhan khong? (c/k): ");
        docDongAnToan(choice_str, sizeof(choice_str), stdin);
        if (strlen(choice_str) > 0 && tolower(choice_str[0]) == 'c') {
            BenhNhan bnMoiTam;
            printf("--- Nhap Thong Tin Benh Nhan Moi ---\n");
            if (nhapThongTinBenhNhanTuBanPhim(&bnMoiTam, *dsBenhNhanConTro, *soLuongBenhNhanConTro, bbBenhNhan)) { //
                // Giả sử bbBenhNhan được truyền dưới dạng BangBam* và không thay đổi địa chỉ ở đây
                // Nếu themBenhNhan có thể rehash và thay đổi *bbBenhNhan, cần truyền &bbBenhNhan
                // Hiện tại, bbBenhNhan là BangBam*, giả sử nó ổn định hoặc được quản lý ở nơi khác.
                // Tuy nhiên, tiêu chuẩn của chúng ta là truyền BangBam** cho các hàm thêm có rehash.
                // Do đó, hàm themBenhNhan nên nhận BangBam** bbBenhNhan_ptr.
                // Giả sử main đã truyền &bbBenhNhan vào hàm gọi UI, và UI truyền tiếp vào đây.
                // Nếu bbBenhNhan ở đây chỉ là BangBam*, và themBenhNhan cần BangBam**, cần điều chỉnh.
                // Dựa trên patient.c, themBenhNhan nhận BangBam**.
                // Cần một con trỏ tạm BangBam* để truyền vào themBenhNhan nếu bbBenhNhan hiện tại là giá trị.
                // Hoặc là thay đổi chữ ký dangKyKhamMoi để nhận BangBam** bbBenhNhan_ptr.
                // Tạm thời, giả sử bbBenhNhan được cập nhật đúng cách ở nơi gọi cao hơn nếu rehash.
                if (!themBenhNhan(dsBenhNhanConTro, soLuongBenhNhanConTro, &bbBenhNhan, bnMoiTam, fileBenhNhanCSV)) { //
                    printf("Loi: Khong thể tao benh nhan moi.\n");
                    return -1;
                }
                bnTimThay = timBenhNhanTheoMaBangBam(bbBenhNhan, bnMoiTam.maDinhDanh);
                if (bnTimThay == NULL) {
                    bnTimThay = (BenhNhan*)timBenhNhanTheoMaChuoi(*dsBenhNhanConTro, *soLuongBenhNhanConTro, bnMoiTam.maDinhDanh);
                }
                if (bnTimThay == NULL) {
                    fprintf(stderr, "LOI NGHIEM TRONG: Khong tim thay benh nhan %s ngay sau khi tao.\n", bnMoiTam.maDinhDanh);
                    return -1;
                }
                printf("Da tao benh nhan moi voi Ma Dinh Danh: %s\n", bnTimThay->maDinhDanh);
                strncpy(maBN_upper, bnTimThay->maDinhDanh, sizeof(maBN_upper) - 1);
                maBN_upper[sizeof(maBN_upper) - 1] = '\0';

            }
            else {
                printf("Huy tao benh nhan moi. Dang ky kham bi huy.\n");
                return -1;
            }
        }
        else {
            printf("Dang ky kham bi huy do benh nhan khong ton tai va khong tao moi.\n");
            return -1;
        }
    }
    // --- Kết thúc phần tìm/tạo bệnh nhân ---

    // --- Phần chọn khoa phòng ---
    char maKP_input[MAX_LEN_MA_KHOA_PHONG];
    char maKP_upper_norm[MAX_LEN_MA_KHOA_PHONG];
    KhoaPhong* kpChon = NULL;

    do {
        printf("\nDanh sach Khoa Phong hien co:\n");
        if (dsKhoaPhong && soLuongKhoaPhong > 0) { //
            inDanhSachKhoaPhongConsole(dsKhoaPhong, soLuongKhoaPhong); //
        }
        else {
            printf("Khong co khoa phong nao trong he thong.\n");
            printf("Vui long them khoa phong truoc khi dang ky kham. Huy dang ky.\n");
            return -1;
        }

        printf("Nhap Ma Khoa Phong muon dang ky kham (KHONG DUOC BO TRONG): ");
        if (docDongAnToan(maKP_input, sizeof(maKP_input), stdin) == NULL || strlen(maKP_input) == 0) {
            printf("LOI: Ma Khoa Phong khong duoc de trong. Huy dang ky.\n");
            return -1;
        }
        strncpy(maKP_upper_norm, maKP_input, sizeof(maKP_upper_norm) - 1);
        maKP_upper_norm[sizeof(maKP_upper_norm) - 1] = '\0';
        for (int k = 0; maKP_upper_norm[k]; k++) maKP_upper_norm[k] = toupper(maKP_upper_norm[k]);

        if (bbKhoaPhong) {
            kpChon = timKhoaPhongTheoMaBangBam(bbKhoaPhong, maKP_upper_norm); //
        }
        else if (dsKhoaPhong) {
            kpChon = timKhoaPhongTheoMa(dsKhoaPhong, soLuongKhoaPhong, maKP_upper_norm); //
        }

        if (kpChon == NULL) {
            printf("LOI: Ma Khoa Phong '%s' khong ton tai. Vui long chon lai.\n", maKP_upper_norm);
        }
    } while (kpChon == NULL);
    // --- Kết thúc phần chọn khoa phòng ---

    LanKham lkMoi;
    lkMoi.maLanKham = taoMaLanKhamMoi(*dsLanKhamConTro, *soLuongLanKhamConTro); //

    strncpy(lkMoi.maDinhDanh, bnTimThay->maDinhDanh, sizeof(lkMoi.maDinhDanh) - 1);
    lkMoi.maDinhDanh[sizeof(lkMoi.maDinhDanh) - 1] = '\0';
    strncpy(lkMoi.tenBenhNhan, bnTimThay->ten, sizeof(lkMoi.tenBenhNhan) - 1);
    lkMoi.tenBenhNhan[sizeof(lkMoi.tenBenhNhan) - 1] = '\0';

    strncpy(lkMoi.maKhoaPhong, kpChon->maKhoaPhong, sizeof(lkMoi.maKhoaPhong) - 1);
    lkMoi.maKhoaPhong[sizeof(lkMoi.maKhoaPhong) - 1] = '\0';

    printf("Nhap Ly Do Kham (KHONG DUOC BO TRONG): ");
    if (docDongAnToan(lkMoi.lyDoKham, sizeof(lkMoi.lyDoKham), stdin) == NULL || strlen(lkMoi.lyDoKham) == 0) {
        printf("LOI: Ly do kham khong duoc de trong. Huy dang ky.\n");
        return -1;
    }

    char mucDoUTStr[30];
    printf("Nhap Muc Do Uu Tien (CapCuu, KhanCap, ThongThuong, TaiKham, UuTienKhac) [ThongThuong]: ");
    docDongAnToan(mucDoUTStr, sizeof(mucDoUTStr), stdin);
    lkMoi.mucDoUuTien = (strlen(mucDoUTStr) == 0) ? THONG_THUONG : stringToMucDoUuTien(mucDoUTStr); //

    lkMoi.gioDangKyThanhCong = time(NULL);
    lkMoi.trangThai = LK_DANG_CHO;
    strcpy(lkMoi.maBacSi, "CHUA_GAN");
    lkMoi.ngayGioKham = 0;
    strcpy(lkMoi.chanDoan, "CHUA_KHAM");
    strcpy(lkMoi.phuongPhapDieuTri, "CHUA_KHAM");
    strcpy(lkMoi.ghiChuBacSi, "-1");
    lkMoi.ngayTaiKham.ngay = -1; lkMoi.ngayTaiKham.thang = -1; lkMoi.ngayTaiKham.nam = -1;
    lkMoi.coLichTaiKham = 0;
    lkMoi.coDonThuoc = 0;
    lkMoi.maDonThuocChiTiet = -1;

    // Thêm vào danh sách lần khám và bảng băm lần khám
    // Giả sử themLanKham trả về 1 nếu thành công, 0 nếu thất bại
    if (themLanKham(dsLanKhamConTro, soLuongLanKhamConTro, bbLanKham_ptr, lkMoi, fileLanKhamCSV)) { //
        // Thêm vào hàng đợi của khoa phòng cụ thể
        if (bbHangDoiTheoKhoa) {
            if (!themVaoHangDoiTheoKhoa(bbHangDoiTheoKhoa, lkMoi.maKhoaPhong, lkMoi)) { //
                fprintf(stderr, "LOI: Khong them duoc Lan Kham %d vao hang doi khoa %s.\n", lkMoi.maLanKham, lkMoi.maKhoaPhong);
                fprintf(stderr, "Thuc hien rollback: Xoa Lan Kham %d vua them.\n", lkMoi.maLanKham);

                // Thực hiện rollback: Xóa lkMoi khỏi dsLanKham và bbLanKham
                // Lưu ý: xoaLanKham không ghi lại file ngay lập tức nếu chỉ xóa một phần tử để rollback,
                // hoặc nó có thể được điều chỉnh để có một cờ không ghi file.
                // Hiện tại, xoaLanKham sẽ ghi lại file.
                if (!xoaLanKham(dsLanKhamConTro, soLuongLanKhamConTro, *bbLanKham_ptr, lkMoi.maLanKham, fileLanKhamCSV)) { //
                    fprintf(stderr, "LOI ROLLBACK: Khong the xoa Lan Kham %d khoi dsLanKham/bbLanKham.\n", lkMoi.maLanKham);
                    // Đây là tình huống lỗi nghiêm trọng, dữ liệu có thể không nhất quán
                }
                else {
                    printf("Rollback thanh cong: Da xoa Lan Kham %d.\n", lkMoi.maLanKham);
                }
                return -1; // Trả về lỗi sau khi rollback (hoặc cố gắng rollback)
            }
        }
        else {
            fprintf(stderr, "CANH BAO: Bang bam hang doi theo khoa khong kha dung. Lan kham %d chua duoc them vao hang doi khoa phong.\n", lkMoi.maLanKham);
        }
        printf("Dang ky kham thanh cong!\n");
        printf("  Ma Lan Kham: %d\n", lkMoi.maLanKham);
        printf("  Benh Nhan: %s (Ma: %s)\n", lkMoi.tenBenhNhan, lkMoi.maDinhDanh);
        printf("  Khoa Phong: %s (Ma: %s)\n", kpChon->tenKhoaPhong, lkMoi.maKhoaPhong);
        printf("  Muc Do Uu Tien: %s\n", mucDoUuTienToString(lkMoi.mucDoUuTien));
        return lkMoi.maLanKham;
    }
    else {
        printf("Loi: Khong the them lan kham vao he thong.\n");
        return -1;
    }
}
// CẬP NHẬT: themLanKham
int themLanKham(LanKham** dsLanKhamConTro, int* soLuongConTro,
    BangBam** bbLanKham_ptr,
    LanKham lanKhamMoi, const char* tenFile) {
    if (bbLanKham_ptr == NULL || *bbLanKham_ptr == NULL) {
        fprintf(stderr, "LOI HE THONG: Bang bam Lan Kham khong kha dung khi them.\n");
        return 0;
    }
    if (strlen(lanKhamMoi.maDinhDanh) == 0 || strlen(lanKhamMoi.maKhoaPhong) == 0 || strlen(lanKhamMoi.lyDoKham) == 0) {
        fprintf(stderr, "LOI (themLanKham): Thong tin lan kham moi khong hop le (maBN/maKP/lyDoKham rong).\n");
        return 0;
    }

    char maLKStr[MA_LAN_KHAM_STR_LEN];
    intMaLKToString(lanKhamMoi.maLanKham, maLKStr, sizeof(maLKStr));

    if (timKiemTrongBangBam(*bbLanKham_ptr, maLKStr) != NULL) {
        fprintf(stderr, "LOI NGHIEM TRONG: Ma Lan Kham %d (%s) da ton tai trong bang bam!\n", lanKhamMoi.maLanKham, maLKStr);
        return 0;
    }

    LanKham* temp = realloc(*dsLanKhamConTro, (*soLuongConTro + 1) * sizeof(LanKham));
    if (temp == NULL) {
        perror("Loi realloc (themLanKham)");
        return 0;
    }
    *dsLanKhamConTro = temp;
    (*dsLanKhamConTro)[*soLuongConTro] = lanKhamMoi;
    LanKham* conTroToiLKTrongMang = &((*dsLanKhamConTro)[*soLuongConTro]);

    if (!chenVaoBangBam(*bbLanKham_ptr, maLKStr, conTroToiLKTrongMang)) {
        fprintf(stderr, "LOI NGHIEM TRONG: Khong the chen Lan Kham %d (%s) vao bang bam.\n", lanKhamMoi.maLanKham, maLKStr);
        // Không tăng soLuongConTro, để tránh dữ liệu không nhất quán
        return 0;
    }

    (*soLuongConTro)++;

    if ((*bbLanKham_ptr)->soLuongPhanTu / (double)(*bbLanKham_ptr)->dungLuong > BANG_BAM_NGUONG_TAI_TOI_DA) {
        if (!rehashBangBam(bbLanKham_ptr)) {
            fprintf(stderr, "LOI: Rehash bang bam LAN KHAM that bai.\n");
        }
    }

    ghiDanhSachLanKhamRaFile(tenFile, *dsLanKhamConTro, *soLuongConTro);
    return 1;
}

// CẬP NHẬT: xoaLanKham
int xoaLanKham(LanKham** dsLanKhamConTro, int* soLuongConTro,
    BangBam* bbLanKham,
    int maLanKhamCanXoa, const char* tenFile) {
    if (bbLanKham == NULL) {
        fprintf(stderr, "LOI HE THONG: Bang bam Lan Kham khong kha dung khi xoa.\n");
        return 0;
    }
    char maLKStr[MA_LAN_KHAM_STR_LEN];
    intMaLKToString(maLanKhamCanXoa, maLKStr, sizeof(maLKStr));
    int removedFromHashTable = 0;

    if (timKiemTrongBangBam(bbLanKham, maLKStr) != NULL) {
        if (xoaKhoiBangBam(bbLanKham, maLKStr)) {
            removedFromHashTable = 1;
        }
        else {
            fprintf(stderr, "LOI: Khong the xoa Lan Kham %d (%s) khoi bang bam.\n", maLanKhamCanXoa, maLKStr);
            return 0;
        }
    } // Nếu không tìm thấy trong bảng băm, vẫn tiếp tục thử xóa khỏi mảng (có thể là lỗi đồng bộ)

    int viTri = -1;
    for (int i = 0; i < *soLuongConTro; ++i) {
        if ((*dsLanKhamConTro)[i].maLanKham == maLanKhamCanXoa) {
            viTri = i;
            break;
        }
    }
    if (viTri != -1) {
        if (!removedFromHashTable && timKiemTrongBangBam(bbLanKham, maLKStr) == NULL) { // Kiểm tra lại
            // Đã bị xóa khỏi bảng băm ở lần gọi khác, hoặc chưa bao giờ được thêm vào bảng băm
            fprintf(stderr, "CANH BAO: Lan Kham %d tim thay trong mang nhung khong co trong bang bam truoc khi xoa (hoac da duoc xoa khoi bang bam).\n", maLanKhamCanXoa);
        }

        for (int i = viTri; i < (*soLuongConTro - 1); ++i) {
            (*dsLanKhamConTro)[i] = (*dsLanKhamConTro)[i + 1];
        }
        (*soLuongConTro)--;
        if (*soLuongConTro == 0) {
            free(*dsLanKhamConTro); *dsLanKhamConTro = NULL;
        }
        else {
            LanKham* temp_ds = realloc(*dsLanKhamConTro, (*soLuongConTro) * sizeof(LanKham));
            if (temp_ds == NULL && *soLuongConTro > 0) {
                perror("Loi realloc (xoaLanKham)");
            }
            else if (*soLuongConTro > 0) *dsLanKhamConTro = temp_ds;
            // else (*dsLanKhamConTro) đã là NULL nếu *soLuongConTro là 0
        }
        ghiDanhSachLanKhamRaFile(tenFile, *dsLanKhamConTro, *soLuongConTro);
        return 1;
    }
    if (removedFromHashTable && viTri == -1) { // Đã xóa khỏi hash table nhưng không thấy trong mảng
        fprintf(stderr, "LOI DONG BO: Da xoa Lan Kham %d khoi bang bam nhung khong tim thay trong mang.\n", maLanKhamCanXoa);
    }
    return 0; // Không tìm thấy trong mảng
}

// CẬP NHẬT: capNhatThongTinLanKham
int capNhatThongTinLanKham(LanKham dsLanKham[], int soLuongLanKham, int maLanKhamCanCapNhat,
    const BacSi dsBacSi[], int soLuongBacSi,
    const KhoaPhong dsKhoaPhong[], int soLuongKhoaPhong,
    BangBam* bbBacSi,
    BangBam* bbKhoaPhong,
    const char* fileLanKhamCSV) {
    int indexLK = -1;
    for (int i = 0; i < soLuongLanKham; ++i) {
        if (dsLanKham[i].maLanKham == maLanKhamCanCapNhat) {
            indexLK = i;
            break;
        }
    }

    if (indexLK == -1) {
        printf("Khong tim thay Lan Kham voi Ma: %d\n", maLanKhamCanCapNhat);
        return 0;
    }

    LanKham* lk = &dsLanKham[indexLK];
    TrangThaiLanKham trangThaiCu = lk->trangThai; // Lưu trạng thái cũ để rollback nếu cần
    char chanDoanCu[sizeof(lk->chanDoan)];
    strcpy(chanDoanCu, lk->chanDoan);

    if (lk->trangThai == LK_DA_HOAN_THANH || lk->trangThai == LK_DA_HUY || lk->trangThai == LK_DA_LO) {
        printf("Lan kham (Ma: %d) da o trang thai '%s'. Khong the cap nhat theo quy trinh nay.\n",
            lk->maLanKham, trangThaiLanKhamToString(lk->trangThai));
        return 0;
    }
    if (lk->trangThai != LK_DANG_CHO && lk->trangThai != LK_DANG_KHAM) {
        printf("Lan kham (Ma: %d) khong o trang thai phu hop de cap nhat (hien tai: '%s').\n",
            lk->maLanKham, trangThaiLanKhamToString(lk->trangThai));
        return 0;
    }


    printf("\n--- CAP NHAT THONG TIN LAN KHAM MA: %d (Benh nhan: %s, Khoa: %s) ---\n", lk->maLanKham, lk->tenBenhNhan, lk->maKhoaPhong);

    char denKhamChoice_str[10];
    printf("Benh nhan co den kham khong? (c/k): ");
    docDongAnToan(denKhamChoice_str, sizeof(denKhamChoice_str), stdin);

    if (strlen(denKhamChoice_str) > 0 && tolower(denKhamChoice_str[0]) == 'k') {
        // ... (logic cho trường hợp không đến khám, như phiên bản trước)
        // Ghi chú: GhiChuBacSi có thể là "Vắng mặt" hoặc chi tiết hơn
        strcpy(lk->ghiChuBacSi, "Benh nhan khong den");
    }
    else if (strlen(denKhamChoice_str) > 0 && tolower(denKhamChoice_str[0]) == 'c') {
        lk->trangThai = LK_DA_HOAN_THANH;

        // Nhập Mã Bác Sĩ (bắt buộc)
        char maBacSiNhap[MAX_LEN_MA_BAC_SI];
        const BacSi* bacSiTimThay = NULL;
        do {
            printf("Nhap Ma Bac Si kham (KHONG DUOC BO TRONG): ");
            if (docDongAnToan(maBacSiNhap, sizeof(maBacSiNhap), stdin) == NULL || strlen(maBacSiNhap) == 0) {
                inThongDiepLoi("Ma Bac Si khong duoc de trong. Vui long nhap lai.");
                // Nếu bạn muốn hủy luôn thao tác hiện tại nếu người dùng nhập rỗng, bạn có thể return từ hàm chứa vòng lặp này.
                // Ví dụ: return 0; // Hoặc một mã lỗi phù hợp
                continue; // Yêu cầu nhập lại
            }
            for (int i = 0; maBacSiNhap[i]; i++) maBacSiNhap[i] = toupper(maBacSiNhap[i]);

            bacSiTimThay = NULL; // Reset trước mỗi lần tìm
            if (bbBacSi != NULL) { // Ưu tiên và chỉ sử dụng bảng băm nếu có
                bacSiTimThay = timBacSiTheoMaBangBam(bbBacSi, maBacSiNhap);
            }
            else {
                // Xử lý trường hợp nghiêm trọng: Bảng băm bác sĩ không khả dụng
                inThongDiepLoi("He thong du lieu Bac Si (Bang Bam) khong kha dung. Khong the xac thuc Ma Bac Si.");
                // Tùy theo logic, bạn có thể:
                // 1. Dừng hoàn toàn thao tác này: strcpy(lk->maBacSi, "LOI_BB_NULL"); break; hoặc return 0;
                // 2. Cho phép nhập mã mà không xác thực (không khuyến khích nếu mã BS là quan trọng)
                // 3. Nếu bạn có hàm timBacSiTheoMa (tuyến tính) và muốn dùng nó như một giải pháp cuối cùng tuyệt đối
                //    (dù đã thống nhất không fallback), bạn sẽ đặt nó ở đây, nhưng cần ghi rõ đây là ngoại lệ.
                // Theo yêu cầu "loại bỏ tìm kiếm tuyến tính", chúng ta sẽ không fallback ở đây.
                // Giả sử nếu bbBacSi là NULL, thì không thể xác thực và nên dừng.
                strcpy(lk->maBacSi, "CHUA_GAN_LOI_BB"); // Gán một giá trị báo lỗi
                inThongDiepLoi("Khong the gan Ma Bac Si do loi he thong. Vui long lien he quan tri.");
                // Cân nhắc việc thoát khỏi vòng lặp hoặc hàm chứa vòng lặp này
                break; // Thoát khỏi vòng lặp do-while nếu bảng băm không có
            }

            if (bacSiTimThay != NULL) {
                strncpy(lk->maBacSi, bacSiTimThay->maBacSi, sizeof(lk->maBacSi) - 1); // Sử dụng strncpy an toàn
                lk->maBacSi[sizeof(lk->maBacSi) - 1] = '\0';
                printf("Da chon Bac Si: %s (Ma: %s)\n", bacSiTimThay->tenBacSi, bacSiTimThay->maBacSi);
                break; // Thoát vòng lặp khi đã tìm thấy và gán bác sĩ hợp lệ
            }
            else {
                // Không tìm thấy trong bảng băm (hoặc bảng băm không có và đã xử lý ở trên)
                printf("CANH BAO: Ma Bac Si '%s' khong tim thay trong he thong.\n", maBacSiNhap);
                printf("Ban co chac chan muon su dung Ma Bac Si '%s' nay (co the khong ton tai)? (c/k): ", maBacSiNhap);
                char confirmBS[10];
                docDongAnToan(confirmBS, sizeof(confirmBS), stdin);
                if (strlen(confirmBS) > 0 && tolower(confirmBS[0]) == 'c') {
                    strncpy(lk->maBacSi, maBacSiNhap, sizeof(lk->maBacSi) - 1); // Người dùng chấp nhận mã không tìm thấy
                    lk->maBacSi[sizeof(lk->maBacSi) - 1] = '\0';
                    inThongDiepThanhCong("Da ghi nhan Ma Bac Si theo y nguoi dung (mac du khong tim thay trong danh sach).");
                    break; // Thoát vòng lặp
                }
                // Nếu người dùng không chọn 'c', vòng lặp sẽ tiếp tục để nhập lại
                inThongDiepLoi("Vui long nhap lai Ma Bac Si hoac chon mot Ma Bac Si ton tai.");
            }
        } while (1);

        printf("Nhap Chan Doan (KHONG DUOC BO TRONG): ");
        docDongAnToan(lk->chanDoan, sizeof(lk->chanDoan), stdin);
        if (strlen(lk->chanDoan) == 0) {
            printf("LOI: Chan Doan khong duoc de trong cho lan kham da hoan thanh. Cap nhat that bai.\n");
            lk->trangThai = trangThaiCu; // Rollback trạng thái
            return 0;
        }

        printf("Nhap Phuong Phap Dieu Tri (KHONG DUOC BO TRONG): ");
        docDongAnToan(lk->phuongPhapDieuTri, sizeof(lk->phuongPhapDieuTri), stdin);
        if (strlen(lk->phuongPhapDieuTri) == 0) {
            printf("LOI: Phuong Phap Dieu Tri khong duoc de trong cho lan kham da hoan thanh. Cap nhat that bai.\n");
            strcpy(lk->chanDoan, chanDoanCu); // Rollback chẩn đoán
            lk->trangThai = trangThaiCu;    // Rollback trạng thái
            return 0;
        }
        // ... (Nhập GhiChuBacSi, TaiKham, DonThuoc như trước) ...
        printf("Nhap Ghi Chu Bac Si (neu bo trong se la '-1'): ");
        docDongAnToan(lk->ghiChuBacSi, sizeof(lk->ghiChuBacSi), stdin);
        if (strlen(lk->ghiChuBacSi) == 0) {
            strcpy(lk->ghiChuBacSi, "-1");
        }
        // ... (logic nhập tái khám, đơn thuốc)
        char taiKhamChoice_str_capnhat[10];
        printf("Co lich tai kham khong? (c/k): ");
        docDongAnToan(taiKhamChoice_str_capnhat, sizeof(taiKhamChoice_str_capnhat), stdin);
        if (strlen(taiKhamChoice_str_capnhat) > 0 && tolower(taiKhamChoice_str_capnhat[0]) == 'c') {
            lk->coLichTaiKham = 1;
            // ... (nhập ngày tái khám)
        }
        else {
            lk->coLichTaiKham = 0; /* ... */
        }

        char donThuocChoice_str_capnhat[10];
        printf("Co ke don thuoc khong? (c/k): ");
        docDongAnToan(donThuocChoice_str_capnhat, sizeof(donThuocChoice_str_capnhat), stdin);
        if (strlen(donThuocChoice_str_capnhat) > 0 && tolower(donThuocChoice_str_capnhat[0]) == 'c') {
            lk->coDonThuoc = 1;
            lk->maDonThuocChiTiet = lk->maLanKham; // Gán mã đơn thuốc bằng mã lần khám
        }
        else {
            lk->coDonThuoc = 0; /* ... */
        }


        printf("Da cap nhat thong tin lan kham thanh cong (trang thai: Da Hoan Thanh).\n");
    }
    else {
        printf("Lua chon khong hop le. Khong thay doi.\n");
        return 0;
    }

    ghiDanhSachLanKhamRaFile(fileLanKhamCSV, dsLanKham, soLuongLanKham);
    return 1;
}


// CẬP NHẬT: suaChiTietLanKham (tương tự capNhatThongTinLanKham về kiểm tra trường rỗng)
int suaChiTietLanKham(LanKham dsLanKham[], int soLuongLanKham, int maLanKhamCanSua,
    const BacSi dsBacSi[], int soLuongBacSi,
    const KhoaPhong dsKhoaPhong[], int soLuongKhoaPhong,
    BangBam* bbBacSi,
    BangBam* bbKhoaPhong,
    const char* fileLanKhamCSV) {
    // ... (Tìm indexLK như trước)
    // ... (Lấy lkCanSua và tạo lkTamSua)

    // Trong vòng lặp do...while, case 0 (Hoàn tất và Lưu):
    /*
        case 0: // Hoan tat va Luu
            if (lkTamSua.trangThai == LK_DA_HOAN_THANH) {
                if (strlen(lkTamSua.chanDoan) == 0 || strcmp(lkTamSua.chanDoan, "CHUA_KHAM") == 0 || strcmp(lkTamSua.chanDoan, "CHUA_NHAP_CD_GHI") == 0 ) {
                    printf("LOI: Chan Doan khong duoc de trong/mac dinh cho lan kham da hoan thanh. Vui long cap nhat.\n");
                    luaChonSuaNoiBo = -1; // Để vòng lặp tiếp tục
                    dungManHinhCho();
                    break;
                }
                if (strlen(lkTamSua.phuongPhapDieuTri) == 0 || strcmp(lkTamSua.phuongPhapDieuTri, "CHUA_KHAM") == 0 || strcmp(lkTamSua.phuongPhapDieuTri, "CHUA_NHAP_PPTT_GHI") == 0) {
                    printf("LOI: Phuong Phap Dieu Tri khong duoc de trong/mac dinh. Vui long cap nhat.\n");
                    luaChonSuaNoiBo = -1;
                    dungManHinhCho();
                    break;
                }
                 if (strlen(lkTamSua.maBacSi) == 0 || strcmp(lkTamSua.maBacSi, "CHUA_GAN") == 0) {
                    printf("LOI: Ma Bac Si khong duoc de trong/CHUA_GAN cho lan kham da hoan thanh. Vui long cap nhat.\n");
                    luaChonSuaNoiBo = -1;
                    dungManHinhCho();
                    break;
                }
            }
            // Đảm bảo các trường khác không rỗng nếu cần
            if (strlen(lkTamSua.maKhoaPhong) == 0 || strcmp(lkTamSua.maKhoaPhong, "CHUA_GAN") == 0) {
                 printf("LOI: Ma Khoa Phong khong duoc de trong/CHUA_GAN. Vui long cap nhat.\n");
                 luaChonSuaNoiBo = -1; dungManHinhCho(); break;
            }
             if (strlen(lkTamSua.lyDoKham) == 0) {
                 printf("LOI: Ly do kham khong duoc de trong. Vui long cap nhat.\n");
                 luaChonSuaNoiBo = -1; dungManHinhCho(); break;
            }


            *lkCanSua = lkTamSua;
            ghiDanhSachLanKhamRaFile(fileLanKhamCSV, dsLanKham, soLuongLanKham);
            // ... (thông báo thành công)
            return 1; // hoặc break để thoát vòng lặp do-while rồi return
    */
    // Logic chi tiết hơn cho case 0 trong suaChiTietLanKham:
    int indexLK = -1;
    for (int i = 0; i < soLuongLanKham; ++i) {
        if (dsLanKham[i].maLanKham == maLanKhamCanSua) {
            indexLK = i;
            break;
        }
    }

    if (indexLK == -1) {
        // ... (báo lỗi không tìm thấy)
        return 0;
    }
    LanKham* lkCanSua = &dsLanKham[indexLK];
    LanKham lkTamSua = *lkCanSua; // Tạo bản sao để sửa đổi tạm

    int luaChonSuaNoiBo;
    char bufferNhap[512]; // Buffer cho nhập liệu

    do {
        // Hiển thị thông tin hiện tại của lkTamSua và menu chọn trường sửa
        // ... (Code hiển thị menu như trong file ui.c gốc của bạn, nhưng lấy từ lkTamSua)
        xoaManHinhConsole(); // Giả sử có hàm này
        printf("\n--- SUA CHI TIET LAN KHAM (Ma LK: %d, Benh Nhan: %s, Khoa: %s, Trang Thai: %s) ---\n",
            lkTamSua.maLanKham, lkTamSua.tenBenhNhan, lkTamSua.maKhoaPhong, trangThaiLanKhamToString(lkTamSua.trangThai));
        printf("  1. Ma Bac Si        (Hien tai: %s)\n", (strlen(lkTamSua.maBacSi) > 0 && strcmp(lkTamSua.maBacSi, "CHUA_GAN") != 0) ? lkTamSua.maBacSi : "CHUA_GAN");
        printf("  2. Ma Khoa Phong    (Hien tai: %s)\n", (strlen(lkTamSua.maKhoaPhong) > 0 && strcmp(lkTamSua.maKhoaPhong, "CHUA_GAN") != 0) ? lkTamSua.maKhoaPhong : "CHUA_GAN");
        printf("  3. Ly Do Kham       (Hien tai: %s)\n", strlen(lkTamSua.lyDoKham) > 0 ? lkTamSua.lyDoKham : "Khong ro");
        printf("  4. Chan Doan        (Hien tai: %s)\n", (strlen(lkTamSua.chanDoan) > 0 && strcmp(lkTamSua.chanDoan, "CHUA_KHAM") != 0) ? lkTamSua.chanDoan : "CHUA_KHAM/CHUA_NHAP");
        printf("  5. Phuong Phap Dieu Tri (Hien tai: %s)\n", (strlen(lkTamSua.phuongPhapDieuTri) > 0 && strcmp(lkTamSua.phuongPhapDieuTri, "CHUA_KHAM") != 0) ? lkTamSua.phuongPhapDieuTri : "CHUA_KHAM/CHUA_NHAP");
        printf("  6. Ghi Chu Bac Si   (Hien tai: %s)\n", (strlen(lkTamSua.ghiChuBacSi) > 0 && strcmp(lkTamSua.ghiChuBacSi, "-1") != 0) ? lkTamSua.ghiChuBacSi : "Khong co");
        // ... các trường khác ...
        printf("  7. Thay doi Trang Thai\n");
        printf("-------------------------------------------------\n");
        printf("  0. HOAN TAT SUA DOI VA LUU\n");
        printf("  9. Huy bo tat ca thay doi va quay lai\n");
        // Giả sử có hàm nhapSoNguyenCoGioiHan
        luaChonSuaNoiBo = nhapSoNguyenCoGioiHan("Lua chon cua ban", 0, 9);
        if (luaChonSuaNoiBo < 0) { luaChonSuaNoiBo = 9; } // Xử lý lỗi nhập

        switch (luaChonSuaNoiBo) {
        case 1: // Sửa Mã Bác Sĩ
            printf("Nhap Ma Bac Si moi (KHONG DUOC BO TRONG): ");
            docDongAnToan(bufferNhap, sizeof(bufferNhap), stdin);
            if (strlen(bufferNhap) == 0) {
                inThongDiepLoi("Ma Bac Si moi khong duoc de trong. Thao tac huy.");
                break;
            }
            for (int k = 0; bufferNhap[k]; ++k) bufferNhap[k] = toupper(bufferNhap[k]);

        
            // Kiểm tra xem mã bác sĩ MỚI này đã tồn tại hay chưa bằng bảng băm
            const BacSi* bsKiemTra = NULL;
            if (bbBacSi != NULL) {
                bsKiemTra = timBacSiTheoMaBangBam(bbBacSi, bufferNhap);
            }
            else {
                inThongDiepLoi("He thong du lieu Bac Si (Bang Bam) khong kha dung. Khong the kiem tra MaBS moi.");
                // Không cho phép sửa mã nếu không kiểm tra được
                break;
            }
            if (bsKiemTra != NULL) {
                // Mã bác sĩ mới đã tồn tại cho một bác sĩ khác
                printf("LOI: Ma Bac Si moi '%s' da ton tai. Vui long chon ma khac.\n", bufferNhap);
            }
            else {
                // Mã bácSi mới là hợp lệ (chưa tồn tại)
                // Thực hiện việc thay đổi mã:
                // 1. Xóa bác sĩ với mã cũ khỏi bảng băm
                if (bbBacSi != NULL) {
                    xoaKhoiBangBam(bbBacSi, lkTamSua.maBacSi); // bsTamSua.maBacSi là mã BS cũ trước khi sửa
                }
                // 2. Cập nhật mã mới trong bsTamSua
                strncpy(lkTamSua.maBacSi, bufferNhap, sizeof(lkTamSua.maBacSi) - 1);
                lkTamSua.maBacSi[sizeof(lkTamSua.maBacSi) - 1] = '\0';
                // 3. Khi lưu (case 0 của menu sửa), hàm suaThongTinBacSi sẽ cập nhật mảng,
                //    và sau đó cần chèn lại bác sĩ này vào bảng băm với mã mới.
                //    Hoặc, hàm themBacSi (nếu dùng để "thêm lại" sau khi sửa mã) sẽ tự động thêm vào bảng băm.
                //    Đây là một quy trình phức tạp và dễ gây lỗi, đó là lý do không nên cho sửa mã chính.
                inThongDiepThanhCong("Ma Bac Si se duoc cap nhat thanh '%s' khi ban luu thay doi.", lkTamSua.maBacSi);
            }
            break;
        case 2: // Sửa Mã Khoa Phòng
            printf("Nhap Ma Khoa Phong moi (KHONG DUOC BO TRONG): ");
            docDongAnToan(bufferNhap, sizeof(bufferNhap), stdin);
            if (strlen(bufferNhap) > 0) {
                for (int k = 0; bufferNhap[k]; ++k) bufferNhap[k] = toupper(bufferNhap[k]);
                const KhoaPhong* kp = bbKhoaPhong ? timKhoaPhongTheoMaBangBam(bbKhoaPhong, bufferNhap) : timKhoaPhongTheoMa(dsKhoaPhong, soLuongKhoaPhong, bufferNhap);
                if (kp) strcpy(lkTamSua.maKhoaPhong, kp->maKhoaPhong);
                else printf("CANH BAO: Ma Khoa Phong '%s' khong tim thay. Giu nguyen ma cu.\n", bufferNhap);
            }
            else printf("LOI: Ma Khoa Phong khong duoc de trong. Giu nguyen ma cu.\n");
            break;
        case 3: // Sửa Lý Do Khám
            printf("Nhap Ly Do Kham moi (KHONG DUOC BO TRONG): ");
            docDongAnToan(lkTamSua.lyDoKham, sizeof(lkTamSua.lyDoKham), stdin);
            if (strlen(lkTamSua.lyDoKham) == 0) {
                printf("LOI: Ly Do Kham khong duoc de trong. Khoi phuc gia tri cu.\n");
                strcpy(lkTamSua.lyDoKham, lkCanSua->lyDoKham); // Phục hồi
            }
            break;
        case 4: // Sửa Chẩn Đoán
            printf("Nhap Chan Doan moi (KHONG DUOC BO TRONG neu trang thai la DaHoanThanh): ");
            docDongAnToan(lkTamSua.chanDoan, sizeof(lkTamSua.chanDoan), stdin);
            if (lkTamSua.trangThai == LK_DA_HOAN_THANH && strlen(lkTamSua.chanDoan) == 0) {
                printf("LOI: Chan Doan khong duoc de trong khi Da Hoan Thanh. Khoi phuc gia tri cu.\n");
                strcpy(lkTamSua.chanDoan, lkCanSua->chanDoan);
            }
            else if (strlen(lkTamSua.chanDoan) == 0) {
                strcpy(lkTamSua.chanDoan, "CHUA_KHAM"); // Mặc định nếu không phải đã hoàn thành
            }
            break;
        case 5: // Sửa Phương Pháp Điều Trị
            printf("Nhap Phuong Phap Dieu Tri moi (KHONG DUOC BO TRONG neu trang thai la DaHoanThanh): ");
            docDongAnToan(lkTamSua.phuongPhapDieuTri, sizeof(lkTamSua.phuongPhapDieuTri), stdin);
            if (lkTamSua.trangThai == LK_DA_HOAN_THANH && strlen(lkTamSua.phuongPhapDieuTri) == 0) {
                printf("LOI: Phuong Phap Dieu Tri khong duoc de trong khi Da Hoan Thanh. Khoi phuc gia tri cu.\n");
                strcpy(lkTamSua.phuongPhapDieuTri, lkCanSua->phuongPhapDieuTri);
            }
            else if (strlen(lkTamSua.phuongPhapDieuTri) == 0) {
                strcpy(lkTamSua.phuongPhapDieuTri, "CHUA_KHAM");
            }
            break;
        case 6: // Sửa Ghi Chú Bác Sĩ
            printf("Nhap Ghi Chu Bac Si moi (bo trong se la '-1'): ");
            docDongAnToan(lkTamSua.ghiChuBacSi, sizeof(lkTamSua.ghiChuBacSi), stdin);
            if (strlen(lkTamSua.ghiChuBacSi) == 0) strcpy(lkTamSua.ghiChuBacSi, "-1");
            break;
        case 7: // Sửa Trạng Thái
            printf("Chon Trang Thai moi (DangCho, DangKham, DaHoanThanh, DaHuy, DaLo): ");
            docDongAnToan(bufferNhap, sizeof(bufferNhap), stdin);
            if (strlen(bufferNhap) > 0) {
                TrangThaiLanKham ttMoi = stringToTrangThaiLanKham(bufferNhap);
                // Kiểm tra nếu stringToTrangThaiLanKham trả về mặc định do nhập sai
                if (ttMoi == LK_DANG_CHO && my_strcasecmp(bufferNhap, "DangCho") != 0 && strlen(bufferNhap) > 0) {
                    printf("CANH BAO: Trang thai nhap khong hop le. Giu nguyen trang thai cu.\n");
                }
                else {
                    lkTamSua.trangThai = ttMoi;
                    // Nếu chuyển sang DANG_CHO hoặc DANG_KHAM, có thể reset một số trường
                    if (ttMoi == LK_DANG_CHO || ttMoi == LK_DANG_KHAM) {
                        strcpy(lkTamSua.chanDoan, "CHUA_KHAM");
                        strcpy(lkTamSua.phuongPhapDieuTri, "CHUA_KHAM");
                        strcpy(lkTamSua.maBacSi, "CHUA_GAN");
                        // Xử lý đơn thuốc, tái khám nếu cần
                    }
                }
            }
            else {
                printf("LOI: Trang thai khong duoc de trong. Giu nguyen trang thai cu.\n");
            }
            break;
        case 0: // Hoàn tất và Lưu
            // Kiểm tra các trường bắt buộc dựa trên trạng thái cuối cùng của lkTamSua
            if (strlen(lkTamSua.maDinhDanh) == 0) { /* Lỗi, không nên xảy ra */ }
            if (strlen(lkTamSua.maKhoaPhong) == 0 || strcmp(lkTamSua.maKhoaPhong, "CHUA_GAN") == 0) {
                printf("LOI KHI LUU: Ma Khoa Phong khong duoc de trong/CHUA_GAN. Vui long sua lai.\n");
                luaChonSuaNoiBo = -1; dungManHinhCho(); break;
            }
            if (strlen(lkTamSua.lyDoKham) == 0) {
                printf("LOI KHI LUU: Ly Do Kham khong duoc de trong. Vui long sua lai.\n");
                luaChonSuaNoiBo = -1; dungManHinhCho(); break;
            }

            if (lkTamSua.trangThai == LK_DA_HOAN_THANH) {
                if (strlen(lkTamSua.chanDoan) == 0 || strcmp(lkTamSua.chanDoan, "CHUA_KHAM") == 0 || my_strcasecmp(lkTamSua.chanDoan, "CHUA_NHAP_CD_DOC") == 0 || my_strcasecmp(lkTamSua.chanDoan, "CHUA_NHAP_CD_GHI") == 0) {
                    printf("LOI KHI LUU: Chan Doan khong duoc de trong/mac dinh cho lan kham da hoan thanh. Vui long sua lai.\n");
                    luaChonSuaNoiBo = -1; dungManHinhCho(); break;
                }
                if (strlen(lkTamSua.phuongPhapDieuTri) == 0 || strcmp(lkTamSua.phuongPhapDieuTri, "CHUA_KHAM") == 0 || my_strcasecmp(lkTamSua.phuongPhapDieuTri, "CHUA_NHAP_PPTT_DOC") == 0 || my_strcasecmp(lkTamSua.phuongPhapDieuTri, "CHUA_NHAP_PPTT_GHI") == 0) {
                    printf("LOI KHI LUU: Phuong Phap Dieu Tri khong duoc de trong/mac dinh. Vui long sua lai.\n");
                    luaChonSuaNoiBo = -1; dungManHinhCho(); break;
                }
                if (strlen(lkTamSua.maBacSi) == 0 || strcmp(lkTamSua.maBacSi, "CHUA_GAN") == 0) {
                    printf("LOI KHI LUU: Ma Bac Si khong duoc de trong/CHUA_GAN cho lan kham da hoan thanh. Vui long sua lai.\n");
                    luaChonSuaNoiBo = -1; dungManHinhCho(); break;
                }
            }
            *lkCanSua = lkTamSua; // Áp dụng các thay đổi
            ghiDanhSachLanKhamRaFile(fileLanKhamCSV, dsLanKham, soLuongLanKham);
            printf("THANH CONG: Da luu cac thay doi cho Lan Kham Ma: %d.\n", maLanKhamCanSua);
            // luaChonSuaNoiBo đã là 0, vòng lặp sẽ tự thoát
            break;
        case 9: // Hủy bỏ
            printf("Da huy bo tat ca thay doi.\n");
            // luaChonSuaNoiBo đã là 9, sẽ thoát vòng lặp và hàm trả về 0 (hoặc giá trị khác)
            return 0;
        default:
            printf("LOI: Lua chon khong hop le.\n");
            dungManHinhCho();
            break;
        }
        if (luaChonSuaNoiBo >= 1 && luaChonSuaNoiBo <= 7 && luaChonSuaNoiBo != 0 && luaChonSuaNoiBo != 9) {
            printf("Thong tin da duoc cap nhat tam thoi. Chon truong khac de sua hoac Luu/Huy.\n");
            dungManHinhCho();
        }
    } while (luaChonSuaNoiBo != 0 && luaChonSuaNoiBo != 9);

    return (luaChonSuaNoiBo == 0); // Trả về 1 nếu lưu thành công (choice là 0), ngược lại là 0
}


// HÀM MỚI: timLanKhamTheoMaBangBam
LanKham* timLanKhamTheoMaBangBam(BangBam* bbLanKham, int maLK) {
    if (bbLanKham == NULL) return NULL;
    char maLKStr[MA_LAN_KHAM_STR_LEN];
    intMaLKToString(maLK, maLKStr, sizeof(maLKStr));
    return (LanKham*)timKiemTrongBangBam(bbLanKham, maLKStr);
}

// CẬP NHẬT: inDanhSachLanKhamConsole
void inDanhSachLanKhamConsole(const LanKham dsLanKham[], int soLuongLanKham,
    const BenhNhan dsBenhNhan[], int soLuongBenhNhan, // Vẫn giữ để fallback
    BangBam* bbBenhNhan) {
    printf("\n--- DANH SACH LAN KHAM BENH ---\n");
    if (soLuongLanKham == 0) {
        printf("Chua co lan kham nao.\n");
        return;
    }
    // ... (Code in tiêu đề bảng như trước) ...
    printf("-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("| %-3s | %-7s | %-18s | %-25s | %-12s | %-15s | %-18s | %-20s | %-20s | %-10s | %-10s | %-15s | %-20s |\n",
        "STT", "Ma LK", "Ma BN", "Ten Benh Nhan", "Trang Thai", "Muc Do UT", "Ma Khoa Phong", "Ly Do Kham", "Chan Doan", "Tai Kham", "Don Thuoc", "Ma Bac Si", "Ghi Chu BS");
    printf("-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");


    for (int i = 0; i < soLuongLanKham; ++i) {
        const LanKham* lk = &dsLanKham[i];
        char tenBNHienThi[101] = "N/A";
        char tenBNRutGon[26];
        char lyDoKhamRutGon[21];
        char chanDoanRutGon[21];
        char ghiChuBSRutGon[21];
        char maBacSiDisplay[16];


        // Lấy tên bệnh nhân
        if (bbBenhNhan != NULL && strlen(lk->maDinhDanh) > 0) {
            BenhNhan* bn = timBenhNhanTheoMaBangBam(bbBenhNhan, lk->maDinhDanh);
            if (bn != NULL) {
                strncpy(tenBNHienThi, bn->ten, sizeof(tenBNHienThi) - 1);
                tenBNHienThi[sizeof(tenBNHienThi) - 1] = '\0';
            }
        }
        else if (dsBenhNhan != NULL && strlen(lk->maDinhDanh) > 0) { // Fallback
            const BenhNhan* bnTuyenTinh = timBenhNhanTheoMaChuoi(dsBenhNhan, soLuongBenhNhan, lk->maDinhDanh);
            if (bnTuyenTinh) {
                strncpy(tenBNHienThi, bnTuyenTinh->ten, sizeof(tenBNHienThi) - 1);
                tenBNHienThi[sizeof(tenBNHienThi) - 1] = '\0';
            }
        }
        else if (strlen(lk->tenBenhNhan) > 0 && strcmp(lk->tenBenhNhan, "Khong Biet") != 0) {
            strncpy(tenBNHienThi, lk->tenBenhNhan, sizeof(tenBNHienThi) - 1);
            tenBNHienThi[sizeof(tenBNHienThi) - 1] = '\0';
        }
        rutGonChuoiHienThi(tenBNRutGon, sizeof(tenBNRutGon), tenBNHienThi, 25);
        rutGonChuoiHienThi(lyDoKhamRutGon, sizeof(lyDoKhamRutGon), lk->lyDoKham, 20);


        char ngayTaiKhamStr[12] = "-";
        if (lk->coLichTaiKham && lk->ngayTaiKham.ngay > 0) {
            snprintf(ngayTaiKhamStr, sizeof(ngayTaiKhamStr), "%02d/%02d/%04d", lk->ngayTaiKham.ngay, lk->ngayTaiKham.thang, lk->ngayTaiKham.nam);
        }

        // Chuẩn bị các chuỗi hiển thị cho các trường có điều kiện
        const char* cdDisplay = "N/A";
        const char* ppdtDisplay = "N/A"; // Không có trong format hiện tại nhưng để đây nếu cần
        const char* bsDisplay = "N/A";
        const char* gcDisplay = "N/A";

        if (lk->trangThai == LK_DA_HOAN_THANH || lk->trangThai == LK_DA_HUY || lk->trangThai == LK_DA_LO) {
            cdDisplay = (strlen(lk->chanDoan) > 0 && strcmp(lk->chanDoan, "CHUA_KHAM") != 0) ? lk->chanDoan : "CHUA_NHAP";
            bsDisplay = (strlen(lk->maBacSi) > 0 && strcmp(lk->maBacSi, "CHUA_GAN") != 0) ? lk->maBacSi : "N/A";
            gcDisplay = (strlen(lk->ghiChuBacSi) > 0 && strcmp(lk->ghiChuBacSi, "-1") != 0) ? lk->ghiChuBacSi : "Khong";
        }
        rutGonChuoiHienThi(chanDoanRutGon, sizeof(chanDoanRutGon), cdDisplay, 20);
        rutGonChuoiHienThi(maBacSiDisplay, sizeof(maBacSiDisplay), bsDisplay, 15);
        rutGonChuoiHienThi(ghiChuBSRutGon, sizeof(ghiChuBSRutGon), gcDisplay, 20);


        printf("| %-3d | %-7d | %-18s | %-25s | %-12s | %-15s | %-18s | %-20s | %-20s | %-10s | %-10s | %-15s | %-20s |\n",
            i + 1,
            lk->maLanKham,
            lk->maDinhDanh,
            tenBNRutGon,
            trangThaiLanKhamToString(lk->trangThai),
            mucDoUuTienToString(lk->mucDoUuTien),
            lk->maKhoaPhong,
            lyDoKhamRutGon,
            chanDoanRutGon,
            ngayTaiKhamStr,
            lk->coDonThuoc ? "Co" : "Khong",
            maBacSiDisplay,
            ghiChuBSRutGon
        );
    }
    printf("-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}


// Tìm kiếm tuyến tính hiện có
void timKiemLanKham(const LanKham dsLanKham[], int soLuongLanKham,
    const char* tieuChi, const char* giaTriTimKiem,
    LanKham** ketQuaTimKiem, int* soLuongKetQua) {
    // Giữ nguyên logic tìm kiếm tuyến tính này cho các tiêu chí không phải mã lần khám
    // (hoặc có thể được gọi nếu bbLanKham là NULL)
    *ketQuaTimKiem = NULL;
    *soLuongKetQua = 0;
    if (soLuongLanKham == 0 || giaTriTimKiem == NULL || tieuChi == NULL || strlen(giaTriTimKiem) == 0) {
        return;
    }

    LanKham* dsKQTemp = (LanKham*)malloc(soLuongLanKham * sizeof(LanKham));
    if (dsKQTemp == NULL) {
        perror("Loi malloc trong timKiemLanKham (dsKQTemp)");
        return;
    }
    int count = 0;

    char tieuChiLower[50];
    strncpy(tieuChiLower, tieuChi, sizeof(tieuChiLower) - 1);
    tieuChiLower[sizeof(tieuChiLower) - 1] = '\0';
    for (int idx = 0; tieuChiLower[idx]; idx++) tieuChiLower[idx] = tolower(tieuChiLower[idx]);

    char giaTriTimKiemLower[100];
    strncpy(giaTriTimKiemLower, giaTriTimKiem, sizeof(giaTriTimKiemLower) - 1);
    giaTriTimKiemLower[sizeof(giaTriTimKiemLower) - 1] = '\0';
    if (strcmp(tieuChiLower, "malankham") != 0) { // Chỉ chuyển lowercase nếu không tìm theo mã (số)
        for (int idx = 0; giaTriTimKiemLower[idx]; idx++) giaTriTimKiemLower[idx] = tolower(giaTriTimKiemLower[idx]);
    }


    for (int i = 0; i < soLuongLanKham; ++i) {
        int timThayTrongLanKhamNay = 0;

        if (strcmp(tieuChiLower, "malankham") == 0) {
            if (dsLanKham[i].maLanKham == atoi(giaTriTimKiem)) { // atoi an toàn nếu chuỗi không phải số
                timThayTrongLanKhamNay = 1;
            }
        }
        else if (strcmp(tieuChiLower, "madinhdanhbn") == 0) {
            char currentMaDDBNLower[MAX_LEN_MA_DINH_DANH_BN + 1]; // Sử dụng define nếu có
            strncpy(currentMaDDBNLower, dsLanKham[i].maDinhDanh, sizeof(currentMaDDBNLower) - 1);
            currentMaDDBNLower[sizeof(currentMaDDBNLower) - 1] = '\0';
            for (int k = 0; currentMaDDBNLower[k]; k++) currentMaDDBNLower[k] = tolower(currentMaDDBNLower[k]);
            if (strcmp(currentMaDDBNLower, giaTriTimKiemLower) == 0) {
                timThayTrongLanKhamNay = 1;
            }
        }
        else if (strcmp(tieuChiLower, "trangthai") == 0) {
            char currentTrangThaiLower[50];
            strncpy(currentTrangThaiLower, trangThaiLanKhamToString(dsLanKham[i].trangThai), sizeof(currentTrangThaiLower) - 1);
            currentTrangThaiLower[sizeof(currentTrangThaiLower) - 1] = '\0';
            for (int k = 0; currentTrangThaiLower[k]; k++) {
                currentTrangThaiLower[k] = tolower(currentTrangThaiLower[k]);
            }
            if (strcmp(currentTrangThaiLower, giaTriTimKiemLower) == 0) {
                timThayTrongLanKhamNay = 1;
            }
        }
        else if (strcmp(tieuChiLower, "bacsi") == 0) { // Tìm theo mã bác sĩ
            char currentMaBacSiLower[MAX_LEN_MA_BAC_SI + 1]; // Sử dụng define
            strncpy(currentMaBacSiLower, dsLanKham[i].maBacSi, sizeof(currentMaBacSiLower) - 1);
            currentMaBacSiLower[sizeof(currentMaBacSiLower) - 1] = '\0';
            for (int k = 0; currentMaBacSiLower[k]; k++) {
                currentMaBacSiLower[k] = tolower(currentMaBacSiLower[k]);
            }
            if (strstr(currentMaBacSiLower, giaTriTimKiemLower) != NULL) { // Có thể dùng strcmp nếu muốn khớp chính xác
                timThayTrongLanKhamNay = 1;
            }
        }
        else if (strcmp(tieuChiLower, "makhoaphong") == 0) { // Tìm theo mã khoa phòng
            char currentMaKPLower[MAX_LEN_MA_KHOA_PHONG + 1];
            strncpy(currentMaKPLower, dsLanKham[i].maKhoaPhong, sizeof(currentMaKPLower) - 1);
            currentMaKPLower[sizeof(currentMaKPLower) - 1] = '\0';
            for (int k = 0; currentMaKPLower[k]; k++) {
                currentMaKPLower[k] = tolower(currentMaKPLower[k]);
            }
            if (strcmp(currentMaKPLower, giaTriTimKiemLower) == 0) {
                timThayTrongLanKhamNay = 1;
            }
        }


        if (timThayTrongLanKhamNay) {
            dsKQTemp[count++] = dsLanKham[i];
        }
    }

    if (count > 0) {
        *ketQuaTimKiem = (LanKham*)malloc(count * sizeof(LanKham));
        if (*ketQuaTimKiem != NULL) {
            memcpy(*ketQuaTimKiem, dsKQTemp, count * sizeof(LanKham));
            *soLuongKetQua = count;
        }
        else {
            perror("Loi malloc cho ketQuaTimKiem trong timKiemLanKham");
            *soLuongKetQua = 0;
        }
    }
    free(dsKQTemp);
}