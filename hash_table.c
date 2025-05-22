#include "hash_table.h" // Giữ nguyên tên file include
#include <stdio.h>     // Cần cho perror
#include <stdlib.h>    // Cần cho malloc, calloc, free
#include <string.h>    // Cần cho strcmp, và _strdup (nếu dùng)

// --- Trien khai cac ham cho bang bam ---

BangBam* taoBangBam(int dungLuongKLT) {
    if (dungLuongKLT <= 0) {
        dungLuongKLT = BANG_BAM_DUNG_LUONG_MAC_DINH;
    }

    BangBam* bb = (BangBam*)malloc(sizeof(BangBam));
    if (bb == NULL) {
        perror("Loi cap phat bo nho cho BangBam");
        return NULL;
    }

    bb->dungLuong = dungLuongKLT;
    bb->soLuongPhanTu = 0;
    bb->cacBucket = (NutBam**)calloc(bb->dungLuong, sizeof(NutBam*));
    if (bb->cacBucket == NULL) {
        perror("Loi cap phat bo nho cho cac bucket cua BangBam");
        free(bb);
        return NULL;
    }
    return bb;
}

void giaiPhongBangBam(BangBam* bb) {
    if (bb == NULL) {
        return;
    }

    for (int i = 0; i < bb->dungLuong; ++i) {
        NutBam* nut = bb->cacBucket[i];
        while (nut != NULL) {
            NutBam* tam = nut;
            nut = nut->tiepTheo;
            free(tam->khoa);
            free(tam);
        }
    }
    free(bb->cacBucket);
    free(bb);
}

unsigned int hamBamChuoi(const char* khoa, int dungLuongBang) {
    unsigned long giaTriBam = 5381;
    int c;
    while ((c = *khoa++)) {
        giaTriBam = ((giaTriBam << 5) + giaTriBam) + c;
    }
    return giaTriBam % dungLuongBang;
}



void* timKiemTrongBangBam(BangBam* bb, const char* khoa) {
    if (bb == NULL || khoa == NULL) {
        return NULL;
    }

    unsigned int chiSo = hamBamChuoi(khoa, bb->dungLuong);
    NutBam* nut = bb->cacBucket[chiSo];

    while (nut != NULL) {
        if (strcmp(nut->khoa, khoa) == 0) {
            return nut->giaTri;
        }
        nut = nut->tiepTheo;
    }
    return NULL;
}

int xoaKhoiBangBam(BangBam* bb, const char* khoa) {
    if (bb == NULL || khoa == NULL) {
        return 0;
    }

    unsigned int chiSo = hamBamChuoi(khoa, bb->dungLuong);
    NutBam* nut = bb->cacBucket[chiSo];
    NutBam* truoc = NULL;

    while (nut != NULL) {
        if (strcmp(nut->khoa, khoa) == 0) {
            if (truoc == NULL) {
                bb->cacBucket[chiSo] = nut->tiepTheo;
            }
            else {
                truoc->tiepTheo = nut->tiepTheo;
            }
            free(nut->khoa);
            free(nut);
            bb->soLuongPhanTu--;
            return 1;
        }
        truoc = nut;
        nut = nut->tiepTheo;
    }
    return 0;
}
int rehashBangBam(BangBam** bb_ptr) {
    if (bb_ptr == NULL || *bb_ptr == NULL) {
        fprintf(stderr, "LOI REHASH: Con tro BangBam khong hop le.\n");
        return 0;
    }
    BangBam* bbCu = *bb_ptr;

    int kichThuocMoi = bbCu->dungLuong * BANG_BAM_HE_SO_TANG_KICH_THUOC;
    if (kichThuocMoi <= bbCu->dungLuong) {
        kichThuocMoi = bbCu->dungLuong + BANG_BAM_DUNG_LUONG_MAC_DINH;
    }

    BangBam* bbMoi = taoBangBam(kichThuocMoi);
    if (bbMoi == NULL) {
        fprintf(stderr, "LOI REHASH: Khong the tao bang bam moi voi kich thuoc %d.\n", kichThuocMoi);
        return 0; // Rehash thất bại, bảng cũ vẫn còn nguyên
    }

    printf("Thong bao: Thuc hien rehash. Kich thuoc cu: %d, Kich thuoc moi: %d, So luong phan tu: %d\n",
        bbCu->dungLuong, bbMoi->dungLuong, bbCu->soLuongPhanTu);

    for (int i = 0; i < bbCu->dungLuong; ++i) {
        NutBam* nutHienTai = bbCu->cacBucket[i];
        while (nutHienTai != NULL) {
            if (!chenVaoBangBam(bbMoi, nutHienTai->khoa, nutHienTai->giaTri)) {
                fprintf(stderr, "LOI REHASH: Khong the chen khoa '%s' vao bang bam moi.\n", nutHienTai->khoa);
                giaiPhongBangBam(bbMoi);
                return 0; // Rehash thất bại, bảng cũ vẫn còn nguyên
            }
            nutHienTai = nutHienTai->tiepTheo;
        }
    }

    giaiPhongBangBam(bbCu); // Giải phóng bảng cũ

    *bb_ptr = bbMoi; // Cập nhật con trỏ ở nơi gọi
    printf("Thong bao: Rehash hoan tat. So luong phan tu trong bang moi: %d\n", (*bb_ptr)->soLuongPhanTu);
    return 1;
}


// Trong hash_table.c, hàm chenVaoBangBam

int chenVaoBangBam(BangBam* bb, const char* khoa, void* giaTri) { // Chữ ký gốc
    // int chenVaoBangBam(BangBam** bb_ptr, const char* khoa, void* giaTri) { // Chữ ký mới nếu rehash thay đổi con trỏ
        // Để dùng chữ ký mới, mọi nơi gọi chenVaoBangBam cần truyền &bbBenhNhan
        // Hiện tại, giữ chữ ký gốc và rehash sẽ phức tạp hơn chút để cập nhật con trỏ bbBenhNhan ở nơi gọi.
        // Cách đơn giản hơn là hàm chenVaoBangBam trả về một con trỏ BangBam mới nếu rehash xảy ra,
        // và nơi gọi sẽ cập nhật con trỏ của nó.

        // *** GIẢ SỬ RẰNG `rehashBangBam` được gọi từ nơi quản lý `BangBam*` chính (ví dụ: patient.c hoặc main.c) ***
        // *** Nếu muốn `chenVaoBangBam` tự rehash và thay đổi con trỏ `bb` được truyền vào, chữ ký hàm `chenVaoBangBam` phải là `BangBam* chenVaoBangBam(BangBam* bb, ...)` và trả về con trỏ mới, hoặc `int chenVaoBangBam(BangBam** bb_ptr, ...)` ***

        // Phiên bản đơn giản: chenVaoBangBam không tự rehash, việc rehash sẽ do hàm cha quản lý.
        // Hoặc, chúng ta sẽ sửa đổi hàm cha (ví dụ themBenhNhan) để gọi rehash.

    if (bb == NULL || khoa == NULL) {
        return 0;
    }

    // --- Phần kiểm tra trùng lặp và tạo nút mới giữ nguyên ---
    unsigned int chiSo = hamBamChuoi(khoa, bb->dungLuong);
    NutBam* nutHienTai = bb->cacBucket[chiSo];

    while (nutHienTai != NULL) {
        if (strcmp(nutHienTai->khoa, khoa) == 0) {
            return 0; // Khoa da ton tai
        }
        nutHienTai = nutHienTai->tiepTheo;
    }

    NutBam* nutMoi = (NutBam*)malloc(sizeof(NutBam));
    if (nutMoi == NULL) {
        perror("Loi cap phat bo nho cho NutBam (chen)");
        return 0;
    }

    nutMoi->khoa = _strdup(khoa); // Hoặc strdup nếu dùng C chuẩn hơn và tự định nghĩa
    if (nutMoi->khoa == NULL) {
        perror("Loi sao chep chuoi khoa (chen)");
        free(nutMoi);
        return 0;
    }
    nutMoi->giaTri = giaTri;
    nutMoi->tiepTheo = bb->cacBucket[chiSo];
    bb->cacBucket[chiSo] = nutMoi;
    bb->soLuongPhanTu++;

    // Kiểm tra hệ số tải SAU KHI CHÈN THÀNH CÔNG
    // Việc gọi rehash từ đây sẽ phức tạp nếu `bb` không phải là con trỏ cấp hai.
    // Để đơn giản, chúng ta sẽ di chuyển logic kiểm tra và gọi rehash ra hàm `themBenhNhan`.

    return 1; // Chèn thành công
}