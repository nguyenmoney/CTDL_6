#ifndef UI_H
#define UI_H

#include "structs.h"
#include "patient.h"
#include "examination.h"
#include "department.h"
#include "doctor.h"
#include "medicine.h"
#include "prescription.h"
#include "priority_queue.h" // Cho HangDoiUuTienMotKhoa (nếu cần) và các hàm hàng đợi
#include "csv_handler.h" 
#include "hash_table.h"  // Cho BangBam

// --- Khai bao cac ham giao dien ---

/**
 * @brief Hien thi menu chinh cua chuong trinh.
 */
void hienThiMenuChinh();

/**
 * @brief Xu ly lua chon cua nguoi dung tu menu chinh.
 * Ham nay se dieu phoi viec goi cac ham chuc nang tuong ung.
 * Tat ca cac con tro toi mang du lieu va so luong deu la con tro cap hai
 * de cac ham trong UI co the thay doi truc tiep du lieu goc.
 * Con trỏ tới BangBam cũng là con trỏ cấp hai để cho phép rehash thay đổi con trỏ gốc.
 */
void xuLyLuaChonMenuChinh(
    int choice,
    BenhNhan** dsBenhNhan, int* soLuongBenhNhan, BangBam** bbBenhNhan_ptr, const char* fileBN,
    LanKham** dsLanKham, int* soLuongLanKham, BangBam** bbLanKham_ptr, const char* fileLK, // THÊM BangBam** bbLanKham_ptr
    KhoaPhong** dsKhoaPhong, int* soLuongKhoaPhong, BangBam** bbKhoaPhong_ptr, const char* fileKP, // THÊM BangBam** bbKhoaPhong_ptr
    BacSi** dsBacSi, int* soLuongBacSi, BangBam** bbBacSi_ptr, const char* fileBS,
    Thuoc** dsThuoc, int* soLuongThuoc, BangBam** bbThuoc_ptr, const char* fileT, // THÊM BangBam** bbThuoc_ptr
    DonThuocChiTiet** dsDonThuocChiTiet, int* soLuongDTCT, const char* fileDTCT,
    BangBam* bbHangDoiTheoKhoa // Bảng băm các hàng đợi theo khoa, không cần con trỏ kép ở đây vì UI không rehash nó
);


// --- Cac ham menu con ---

// Cập nhật chữ ký để nhận BangBam*
void menuQuanLyBenhNhan(BenhNhan** dsBenhNhan, int* soLuongBenhNhan, BangBam** bbBenhNhan_ptr, const char* tenFileCSV);

void menuQuanLyLanKham(
    LanKham** dsLanKham, int* soLuongLanKham, BangBam** bbLanKham_ptr, // Thêm bbLanKham_ptr
    BenhNhan* dsBenhNhan, int soLuongBenhNhan, BangBam* bbBenhNhan, // Thêm bbBenhNhan
    BacSi* dsBacSi, int soLuongBacSi, BangBam* bbBacSi,       // Thêm bbBacSi
    KhoaPhong* dsKhoaPhong, int soLuongKhoaPhong, BangBam* bbKhoaPhong, // Thêm bbKhoaPhong
    const char* fileLanKhamCSV);

void menuQuanLyBacSi(
    BacSi** dsBacSi, int* soLuongBacSi,
    BangBam** bbBacSi_ptr, 
    const char* tenFileCSV,
    KhoaPhong* dsKhoaPhong, int soLuongKhoaPhong, BangBam* bbKhoaPhong // Thêm bbKhoaPhong để xác thực
);

void menuQuanLyThuoc(Thuoc** dsThuoc, int* soLuongThuoc, BangBam** bbThuoc_ptr, const char* tenFileCSV); // Thêm bbThuoc_ptr

void menuQuanLyKhoaPhong(KhoaPhong** dsKhoaPhong, int* soLuongKhoaPhong, BangBam** bbKhoaPhong_ptr, const char* tenFileCSV); // Thêm bbKhoaPhong_ptr


// --- Cac ham tien ich giao dien ---
void xoaManHinhConsole();
void dungManHinhCho();
int nhapSoNguyenCoGioiHan(const char* prompt, int min, int max);
int nhapSoNguyenKhongAm(const char* prompt); // Có thể không cần nếu nhapSoNguyenCoGioiHan đủ dùng
void inThongDiepLoi(const char* message);
void inThongDiepThanhCong(const char* message);

// Hàm hiển thị thông tin bảng băm (có thể mở rộng cho các bảng băm khác)
void hienThiThongTinBangBam(BangBam* bb, const char* tenBangBam);
void menuXemThongTinBangBam(
    BangBam* bbBenhNhan,
    BangBam* bbBacSi,
    BangBam* bbKhoaPhong,
    BangBam* bbThuoc,
    BangBam* bbLanKham
);


#endif // UI_H