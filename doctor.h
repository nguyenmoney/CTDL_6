#ifndef DOCTOR_H
#define DOCTOR_H

#include "structs.h"
#include "hash_table.h" // <<<< THÊM INCLUDE NÀY
#include <stdio.h>

#define FILE_BAC_SI "data/bacsi.csv"

// Đọc danh sách bác sĩ từ file CSV
BacSi* docDanhSachBacSi(const char* tenFile, int* soLuong);

// Ghi danh sách bác sĩ ra file CSV
void ghiDanhSachBacSiRaFile(const char* tenFile, const BacSi dsBacSi[], int soLuong);

// Thêm bác sĩ mới
// SỬA ĐỔI: Thêm BangBam** bbBacSi_ptr để thao tác với bảng băm
int themBacSi(BacSi** dsBacSiConTro, int* soLuongConTro,
    BangBam** bbBacSi_ptr,
    BacSi bacSiMoi, const char* tenFile);

// Sửa thông tin bác sĩ hiện có
// Giao diện người dùng (UI) nên sử dụng timBacSiTheoMaBangBam để tìm bác sĩ trước nếu ID được nhập trực tiếp.
// Hàm này hoạt động trên mảng chính sau khi bác sĩ được xác định.
// Nếu maBacSi (khóa) là bất biến, không cần thay đổi trực tiếp bảng băm cho chính khóa đó.
// Con trỏ trong bảng băm sẽ vẫn trỏ đến struct đã sửa đổi trong mảng.
int suaThongTinBacSi(BacSi dsBacSi[], int soLuong, const char* maBacSiCanSua, BacSi thongTinMoi,
    const KhoaPhong dsKhoaPhong[], int soLuongKhoaPhong, // Để xác thực maKhoaPhong mới
    const char* tenFile);

// Xóa bác sĩ
// SỬA ĐỔI: Thêm BangBam* bbBacSi để thao tác với bảng băm
int xoaBacSi(BacSi** dsBacSiConTro, int* soLuongConTro,
    BangBam* bbBacSi,
    const char* maBacSiCanXoa, const char* tenFile);

// In danh sách bác sĩ ra console
void inDanhSachBacSiConsole(const BacSi dsBacSi[], int soLuong);

// Nhập thông tin bác sĩ từ bàn phím
// SỬA ĐỔI: Thêm BangBam* bbBacSiHienCo để kiểm tra trùng lặp maBacSi bằng bảng băm
int nhapThongTinBacSiTuBanPhim(BacSi* bs,
    BangBam* bbBacSiHienCo,
    const BacSi dsBacSiHienCo[], int soLuongBSHienCo, // Vẫn cần để kiểm tra trùng lặp các trường không phải khóa (tên, sđt, email)
    const KhoaPhong dsKhoaPhong[], int soLuongKhoaPhong); // Để xác thực maKhoaPhong

// HÀM MỚI: Tìm bác sĩ theo ID sử dụng bảng băm
BacSi* timBacSiTheoMaBangBam(BangBam* bbBacSi, const char* maBacSi);
void timKiemBacSiTuyenTinh(const BacSi dsBacSi[], int soLuongBacSi,
    const char* tieuChi, const char* giaTriTimKiem,
    BacSi** ketQuaTimKiem, int* soLuongKetQua);

// Các hàm tìm kiếm cụ thể (không static để có thể gọi từ UI nếu cần kiểm tra đơn lẻ)
// Những hàm này bạn đã bỏ static đi rồi nên có thể sử dụng ở UI, nhưng hàm timKiemBacSiTuyenTinh ở trên sẽ tổng quát hơn.
BacSi* timBacSiTheoSDT(const BacSi dsBacSi[], int soLuong, const char* soDienThoai);
BacSi* timBacSiTheoEmail(const BacSi dsBacSi[], int soLuong, const char* email);
#endif // DOCTOR_H