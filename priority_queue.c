#include "priority_queue.h"
#include "patient.h"     // Để sử dụng timBenhNhanTheoMaBangBam
#include "examination.h" // Cho mucDoUuTienToString và các hằng số
#include "csv_handler.h" // Cho rutGonChuoiHienThi
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>      // Cho toupper

// --- Hàm so sánh cho sắp xếp (giữ nguyên) ---
static int soSanhUuTienLanKham(const void* a, const void* b) {
    const LanKham* lk1 = (const LanKham*)a;
    const LanKham* lk2 = (const LanKham*)b;

    if (lk1->mucDoUuTien < lk2->mucDoUuTien) return -1;
    if (lk1->mucDoUuTien > lk2->mucDoUuTien) return 1;
    // Nếu mức độ ưu tiên bằng nhau, xét thời gian đăng ký
    if (lk1->gioDangKyThanhCong < lk2->gioDangKyThanhCong) return -1; // Đăng ký sớm hơn, ưu tiên hơn
    if (lk1->gioDangKyThanhCong > lk2->gioDangKyThanhCong) return 1;
    return 0; // Hoàn toàn giống nhau (hiếm khi xảy ra)
}

// --- Triển khai Merge Sort ---
static void merge(LanKham arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    LanKham* L_arr = (LanKham*)malloc(n1 * sizeof(LanKham));
    LanKham* R_arr = (LanKham*)malloc(n2 * sizeof(LanKham));

    if (L_arr == NULL || R_arr == NULL) {
        perror("Loi malloc trong ham merge (priority_queue)");
        if (L_arr) free(L_arr);
        if (R_arr) free(R_arr);
        // Trong một ứng dụng thực tế, cần có cơ chế xử lý lỗi tốt hơn
        // Ví dụ: trả về mã lỗi để hàm gọi biết và xử lý
        return;
    }

    for (i = 0; i < n1; i++) L_arr[i] = arr[l + i];
    for (j = 0; j < n2; j++) R_arr[j] = arr[m + 1 + j];

    i = 0; j = 0; k = l;
    while (i < n1 && j < n2) {
        if (soSanhUuTienLanKham(&L_arr[i], &R_arr[j]) <= 0) {
            arr[k] = L_arr[i];
            i++;
        }
        else {
            arr[k] = R_arr[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L_arr[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R_arr[j];
        j++;
        k++;
    }
    free(L_arr);
    free(R_arr);
}

static void mergeSort(LanKham arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2; // Tránh tràn số cho l+r lớn
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// Hàm sắp xếp hàng đợi của một khoa sử dụng Merge Sort
static void sapXepHangDoiMotKhoaBangMergeSort(HangDoiUuTienMotKhoa* hqmk) {
    if (hqmk == NULL || hqmk->soLuongHienTai <= 1) {
        return;
    }
    mergeSort(hqmk->dsChoKham, 0, hqmk->soLuongHienTai - 1);
}


// --- Các hàm public của module (đã sửa đổi) ---

void khoiTaoHangDoiMotKhoa(HangDoiUuTienMotKhoa* hqmk) {
    if (hqmk == NULL) return;
    hqmk->dsChoKham = NULL;
    hqmk->soLuongHienTai = 0;
    hqmk->dungLuong = 0;
}

int themVaoHangDoiTheoKhoa(BangBam* bbHangDoiTheoKhoa, const char* maKhoaPhong, LanKham lanKhamMoi) {
    if (bbHangDoiTheoKhoa == NULL || maKhoaPhong == NULL || strlen(maKhoaPhong) == 0) {
        fprintf(stderr, "LOI (themVaoHangDoiTheoKhoa): Bang bam hang doi hoac maKhoaPhong khong hop le.\n");
        return 0;
    }

    if (lanKhamMoi.trangThai != LK_DANG_CHO) {
        // Không thêm nếu không phải trạng thái ĐANG CHỜ
        return 1;
    }

    char maKPNorm[MAX_LEN_MA_KHOA_PHONG]; // Chuẩn hóa mã khoa phòng
    strncpy(maKPNorm, maKhoaPhong, sizeof(maKPNorm) - 1);
    maKPNorm[sizeof(maKPNorm) - 1] = '\0';
    for (int i = 0; maKPNorm[i]; ++i) maKPNorm[i] = toupper(maKPNorm[i]);

    HangDoiUuTienMotKhoa* hqmk = (HangDoiUuTienMotKhoa*)timKiemTrongBangBam(bbHangDoiTheoKhoa, maKPNorm);

    if (hqmk == NULL) {
        hqmk = (HangDoiUuTienMotKhoa*)malloc(sizeof(HangDoiUuTienMotKhoa));
        if (hqmk == NULL) {
            perror("Loi malloc cho HangDoiUuTienMotKhoa moi");
            return 0;
        }
        khoiTaoHangDoiMotKhoa(hqmk);

        char* khoaMaKPStored = strdup(maKPNorm);
        if (khoaMaKPStored == NULL) {
            perror("Loi strdup maKhoaPhong cho bang bam hang doi");
            free(hqmk);
            return 0;
        }
        // Chèn vào bảng băm bbHangDoiTheoKhoa. 
        // Lưu ý: bbHangDoiTheoKhoa cần được khởi tạo với kích thước đủ lớn ở main.
        // Việc rehash cho bbHangDoiTheoKhoa cần được xem xét nếu số lượng khoa phòng lớn.
        if (!chenVaoBangBam(bbHangDoiTheoKhoa, khoaMaKPStored, hqmk)) {
            fprintf(stderr, "LOI: Khong the chen hang doi cua khoa %s vao bang bam hang doi.\n", khoaMaKPStored);
            free(khoaMaKPStored); // Phải giải phóng key đã strdup nếu chèn thất bại
            free(hqmk);
            return 0;
        }
        // Sau khi chèn, kiểm tra và rehash bbHangDoiTheoKhoa nếu cần (logic tương tự như thêm bệnh nhân/bác sĩ)
        if (bbHangDoiTheoKhoa->soLuongPhanTu / (double)bbHangDoiTheoKhoa->dungLuong > BANG_BAM_NGUONG_TAI_TOI_DA) {
            // printf("Thong bao: He so tai cua BANG BAM HANG DOI THEO KHOA vuot nguong. Dang rehash...\n");
            // Việc rehash cho bbHangDoiTheoKhoa cần một hàm rehash tùy chỉnh nếu value (HangDoiUuTienMotKhoa*)
            // và key (strdup(maKhoaPhong)) cần được xử lý đặc biệt khi tạo bảng mới.
            // Hoặc, đảm bảo hàm rehashBangBam chung có thể xử lý con trỏ value và key đã strdup.
            // Hiện tại, giả sử rehashBangBam chuẩn có thể xử lý.
            // BangBam* tempBB = bbHangDoiTheoKhoa; // Lưu con trỏ cũ
            // if (!rehashBangBam(&tempBB)) { // Truyền địa chỉ của con trỏ
            //     fprintf(stderr, "LOI: Rehash bang bam HANG DOI KHOA PHONG that bai.\n");
            // }
            // bbHangDoiTheoKhoa = tempBB; // Cập nhật con trỏ (nếu rehash thay đổi nó)
            // => Việc rehash cho bbHangDoiTheoKhoa nên được quản lý ở nơi khởi tạo và truyền nó.
        }

    }

    for (int i = 0; i < hqmk->soLuongHienTai; ++i) {
        if (hqmk->dsChoKham[i].maLanKham == lanKhamMoi.maLanKham) {
            return 1;
        }
    }

    if (hqmk->soLuongHienTai >= hqmk->dungLuong) {
        int dungLuongMoi = (hqmk->dungLuong == 0) ? 10 : hqmk->dungLuong * 2;
        LanKham* temp_ds = realloc(hqmk->dsChoKham, dungLuongMoi * sizeof(LanKham));
        if (temp_ds == NULL) {
            perror("Loi realloc dsChoKham (themVaoHangDoiTheoKhoa)");
            return 0;
        }
        hqmk->dsChoKham = temp_ds;
        hqmk->dungLuong = dungLuongMoi;
    }

    hqmk->dsChoKham[hqmk->soLuongHienTai] = lanKhamMoi;
    hqmk->soLuongHienTai++;

    sapXepHangDoiMotKhoaBangMergeSort(hqmk);

    return 1;
}

int layBenhNhanTiepTheoTheoKhoa(BangBam* bbHangDoiTheoKhoa, const char* maKhoaPhong, LanKham* lanKhamLayRa) {
    if (bbHangDoiTheoKhoa == NULL || maKhoaPhong == NULL || strlen(maKhoaPhong) == 0 || lanKhamLayRa == NULL) return 0;

    char maKPNorm[MAX_LEN_MA_KHOA_PHONG];
    strncpy(maKPNorm, maKhoaPhong, sizeof(maKPNorm) - 1);
    maKPNorm[sizeof(maKPNorm) - 1] = '\0';
    for (int i = 0; maKPNorm[i]; ++i) maKPNorm[i] = toupper(maKPNorm[i]);

    HangDoiUuTienMotKhoa* hqmk = (HangDoiUuTienMotKhoa*)timKiemTrongBangBam(bbHangDoiTheoKhoa, maKPNorm);

    if (hqmk == NULL || hqmk->soLuongHienTai == 0) {
        return 0;
    }
    // Không cần sắp xếp lại ở đây vì đã sắp xếp khi thêm
    // sapXepHangDoiMotKhoaBangMergeSort(hqmk); 

    *lanKhamLayRa = hqmk->dsChoKham[0];

    for (int i = 0; i < hqmk->soLuongHienTai - 1; ++i) {
        hqmk->dsChoKham[i] = hqmk->dsChoKham[i + 1];
    }
    hqmk->soLuongHienTai--;

    // Cân nhắc thu nhỏ mảng dsChoKham nếu soLuongHienTai quá nhỏ so với dungLuong
    // if (hqmk->soLuongHienTai > 0 && hqmk->soLuongHienTai < hqmk->dungLuong / 4 && hqmk->dungLuong > 20) {
    //     int dungLuongMoi = hqmk->dungLuong / 2;
    //     LanKham* temp_ds = realloc(hqmk->dsChoKham, dungLuongMoi * sizeof(LanKham));
    //     if (temp_ds != NULL) {
    //         hqmk->dsChoKham = temp_ds;
    //         hqmk->dungLuong = dungLuongMoi;
    //     }
    // }

    return 1;
}

int laHangDoiTheoKhoaRong(BangBam* bbHangDoiTheoKhoa, const char* maKhoaPhong) {
    if (bbHangDoiTheoKhoa == NULL || maKhoaPhong == NULL || strlen(maKhoaPhong) == 0) return 1;

    char maKPNorm[MAX_LEN_MA_KHOA_PHONG];
    strncpy(maKPNorm, maKhoaPhong, sizeof(maKPNorm) - 1);
    maKPNorm[sizeof(maKPNorm) - 1] = '\0';
    for (int i = 0; maKPNorm[i]; ++i) maKPNorm[i] = toupper(maKPNorm[i]);

    HangDoiUuTienMotKhoa* hqmk = (HangDoiUuTienMotKhoa*)timKiemTrongBangBam(bbHangDoiTheoKhoa, maKPNorm);
    if (hqmk == NULL) return 1; // Khoa không có hàng đợi -> coi như rỗng
    return hqmk->soLuongHienTai == 0;
}

void inHangDoiMotKhoaConsole(BangBam* bbHangDoiTheoKhoa, const char* maKhoaPhong,
    BangBam* bbBenhNhan,
    const BenhNhan dsBenhNhanToanBo[], int soLuongBNTB) {
    if (bbHangDoiTheoKhoa == NULL || maKhoaPhong == NULL || strlen(maKhoaPhong) == 0) {
        printf("Loi: Khong tim thay bang bam hang doi hoac ma khoa phong khong hop le.\n");
        return;
    }

    char maKPHienThi[MAX_LEN_MA_KHOA_PHONG];
    strncpy(maKPHienThi, maKhoaPhong, sizeof(maKPHienThi) - 1);
    maKPHienThi[sizeof(maKPHienThi) - 1] = '\0';
    for (int i = 0; maKPHienThi[i]; ++i) maKPHienThi[i] = toupper(maKPHienThi[i]);

    HangDoiUuTienMotKhoa* hqmk = (HangDoiUuTienMotKhoa*)timKiemTrongBangBam(bbHangDoiTheoKhoa, maKPHienThi);

    printf("\n--- HANG DOI CHO KHAM CUA KHOA: %s ---\n", maKPHienThi);
    if (hqmk == NULL || hqmk->soLuongHienTai == 0) {
        printf("Hang doi cua khoa %s rong.\n", maKPHienThi);
        return;
    }

    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("| %-3s | %-7s | %-18s | %-25s | %-15s | %-20s |\n", "STT", "Ma LK", "Ma BN", "Ten Benh Nhan", "Muc Do Uu Tien", "Gio Dang Ky");
    printf("-------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < hqmk->soLuongHienTai; ++i) {
        const LanKham* lk = &hqmk->dsChoKham[i];
        char tenBNHienThi[101] = "N/A";

        if (strlen(lk->maDinhDanh) > 0) {
            if (bbBenhNhan) { // Ưu tiên bảng băm bệnh nhân
                BenhNhan* bn = timBenhNhanTheoMaBangBam(bbBenhNhan, lk->maDinhDanh);
                if (bn) {
                    strncpy(tenBNHienThi, bn->ten, sizeof(tenBNHienThi) - 1);
                    tenBNHienThi[sizeof(tenBNHienThi) - 1] = '\0';
                }
            }
            else if (dsBenhNhanToanBo != NULL) { // Fallback nếu không có bbBenhNhan
                for (int j = 0; j < soLuongBNTB; ++j) {
                    if (strcmp(dsBenhNhanToanBo[j].maDinhDanh, lk->maDinhDanh) == 0) {
                        strncpy(tenBNHienThi, dsBenhNhanToanBo[j].ten, sizeof(tenBNHienThi) - 1);
                        tenBNHienThi[sizeof(tenBNHienThi) - 1] = '\0';
                        break;
                    }
                }
            }
        }

        char gioDangKyStr[30];
        if (lk->gioDangKyThanhCong != 0) {
            struct tm* tm_info = localtime(&lk->gioDangKyThanhCong);
            if (tm_info != NULL) {
                strftime(gioDangKyStr, sizeof(gioDangKyStr), "%H:%M:%S %d/%m/%Y", tm_info);
            }
            else {
                strcpy(gioDangKyStr, "Loi TGian");
            }
        }
        else {
            strcpy(gioDangKyStr, "Chua DK");
        }

        char tenBNRutGon[26];
        rutGonChuoiHienThi(tenBNRutGon, sizeof(tenBNRutGon), tenBNHienThi, 25);


        printf("| %-3d | %-7d | %-18s | %-25s | %-15s | %-20s |\n",
            i + 1,
            lk->maLanKham,
            (lk->maDinhDanh[0] != '\0' ? lk->maDinhDanh : "CHUA_GAN"),
            tenBNRutGon,
            mucDoUuTienToString(lk->mucDoUuTien),
            gioDangKyStr);
    }
    printf("-------------------------------------------------------------------------------------------------------------\n");
    printf("Tong so benh nhan dang cho tai khoa %s: %d\n", maKPHienThi, hqmk->soLuongHienTai);
}

void giaiPhongDuLieuHangDoiMotKhoa(HangDoiUuTienMotKhoa* hqmk) {
    if (hqmk != NULL) {
        free(hqmk->dsChoKham);
        hqmk->dsChoKham = NULL;
        hqmk->soLuongHienTai = 0;
        hqmk->dungLuong = 0;
    }
}

void giaiPhongTatCaHangDoiTheoKhoa(BangBam* bbHangDoiTheoKhoa) {
    if (bbHangDoiTheoKhoa == NULL || bbHangDoiTheoKhoa->cacBucket == NULL) {
        return;
    }

    for (int i = 0; i < bbHangDoiTheoKhoa->dungLuong; ++i) {
        NutBam* nutHienTai = bbHangDoiTheoKhoa->cacBucket[i];
        while (nutHienTai != NULL) {
            HangDoiUuTienMotKhoa* hqmk = (HangDoiUuTienMotKhoa*)nutHienTai->giaTri;
            if (hqmk != NULL) {
                giaiPhongDuLieuHangDoiMotKhoa(hqmk); // Giải phóng mảng dsChoKham
                free(hqmk);                     // Giải phóng chính cấu trúc HangDoiUuTienMotKhoa
            }
            // Khóa (nutHienTai->khoa) đã được strdup, sẽ được giải phóng bởi giaiPhongBangBam
            nutHienTai = nutHienTai->tiepTheo;
        }
    }
    giaiPhongBangBam(bbHangDoiTheoKhoa); // Giải phóng cấu trúc bảng băm (bao gồm các NutBam và key của chúng)
}