#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BANG_BAM_DUNG_LUONG_MAC_DINH 101
#define BANG_BAM_NGUONG_TAI_TOI_DA 0.75
#define BANG_BAM_HE_SO_TANG_KICH_THUOC 2
// Dung luong (so bucket) mac dinh cho bang bam neu khong duoc chi dinh
#define BANG_BAM_DUNG_LUONG_MAC_DINH 101 // Giữ lại define này với tên tiếng Việt

// Cau truc cho mot nut trong danh sach lien ket tai moi bucket
typedef struct NutBam {
    char* khoa;             // Khoa (phai duoc cap phat dong va sao chep)
    void* giaTri;           // Con tro toi gia tri/du lieu thuc te
    struct NutBam* tiepTheo; // Con tro toi nut tiep theo trong chuoi (neu co xung dot)
} NutBam;

// Cau truc cho bang bam
typedef struct BangBam {
    NutBam** cacBucket;     // Mang cac con tro toi NutBam (cac bucket)
    int dungLuong;          // So luong bucket toi da trong bang
    int soLuongPhanTu;      // So luong phan tu (cap khoa-giaTri) hien co trong bang bam
} BangBam;

// --- Khai bao cac ham cho bang bam ---

/**
 * @brief Tao mot bang bam moi.
 *
 * @param dungLuongKLT Kich thuoc mong muon cua bang bam (so luong bucket).
 * Neu <= 0, se dung BANG_BAM_DUNG_LUONG_MAC_DINH.
 * @return Con tro toi BangBam moi duoc tao, hoac NULL neu loi cap phat bo nho.
 */
BangBam* taoBangBam(int dungLuongKLT); // Đổi tên hàm

/**
 * @brief Giai phong toan bo bo nho duoc su dung boi bang bam.
 * Bao gom cac NutBam, cac khoa (key) da duoc sao chep, va mang buckets.
 * @warning Ham nay KHONG giai phong bo nho cua `giaTri` ma cac NutBam tro toi.
 *
 * @param bb Con tro toi BangBam can giai phong.
 */
void giaiPhongBangBam(BangBam* bb); // Đổi tên hàm

/**
 * @brief Ham bam cho khoa kieu chuoi (djb2 algorithm).
 *
 * @param khoa Chuoi khoa.
 * @param dungLuongBang Kich thuoc (so bucket) cua bang bam.
 * @return Chi so (index) cua bucket.
 */
unsigned int hamBamChuoi(const char* khoa, int dungLuongBang); // Đổi tên hàm

/**
 * @brief Chen mot cap (khoa, giaTri) vao bang bam.
 * Ham nay se tao mot ban sao cua `khoa`.
 * Neu `khoa` da ton tai trong bang, ham se khong chen va tra ve 0.
 *
 * @param bb Con tro toi BangBam.
 * @param khoa Chuoi khoa. Chuoi nay se duoc sao chep.
 * @param giaTri Con tro void toi gia tri/du lieu can luu.
 * @return 1 neu chen thanh cong.
 * 0 neu loi (vi du: het bo nho, khoa bi NULL, hoac khoa da ton tai).
 */
int chenVaoBangBam(BangBam* bb, const char* khoa, void* giaTri); // Đổi tên hàm

/**
 * @brief Tim kiem mot phan tu trong bang bam dua tren khoa.
 *
 * @param bb Con tro toi BangBam.
 * @param khoa Chuoi khoa can tim.
 * @return Con tro void toi gia tri/du lieu neu tim thay, NULL neu khong tim thay.
 */
void* timKiemTrongBangBam(BangBam* bb, const char* khoa); // Đổi tên hàm

/**
 * @brief Xoa mot phan tu khoi bang bam dua tren khoa.
 * Ham nay se giai phong bo nho cua khoa da sao chep va NutBam tuong ung.
 * @warning Ham nay KHONG giai phong bo nho cua `giaTri` ma nut bi xoa tro toi.
 *
 * @param bb Con tro toi BangBam.
 * @param khoa Chuoi khoa cua phan tu can xoa.
 * @return 1 neu xoa thanh cong.
 * 0 neu khong tim thay khoa.
 */
int xoaKhoiBangBam(BangBam* bb, const char* khoa); // Đổi tên hàm

int rehashBangBam(BangBam** bb_ptr); // Đổi tên hàm


#endif // HASH_TABLE_H