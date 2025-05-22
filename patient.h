#ifndef PATIENT_H
#define PATIENT_H

#include "structs.h"
#include "hash_table.h" // <<< THEM INCLUDE BANG BAM
#include <stdio.h>

#define FILE_BENH_NHAN "data/benhnhan.csv"

BenhNhan* docDanhSachBenhNhan(const char* tenFile, int* soLuongBenhNhan);
void ghiDanhSachBenhNhanRaFile(const char* tenFile, const BenhNhan dsBenhNhan[], int soLuongBenhNhan);

// <<< CAP NHAT CHU KY HAM >>>
int themBenhNhan(BenhNhan** dsBenhNhanConTro, int* soLuongBenhNhan,
    BangBam** bbBenhNhan_ptr, // <<<< THAY ĐỔI Ở ĐÂY
    BenhNhan benhNhanMoi, const char* tenFile);

// Sua lai tham so maDinhDanhCanSua va maDinhDanhCanXoa thanh const char*
int suaThongTinBenhNhan(BenhNhan dsBenhNhan[], int soLuongBenhNhan, char* maDinhDanhCanSua, BenhNhan thongTinMoi, const char* tenFile);

// <<< CAP NHAT CHU KY HAM >>>
int xoaBenhNhan(BenhNhan** dsBenhNhanConTro, int* soLuongBenhNhan, BangBam* bbBenhNhan, char* maDinhDanhCanXoa, const char* tenFile);

void inDanhSachBenhNhanConsole(const BenhNhan dsBenhNhan[], int soLuongBenhNhan);

// giaTriTimKiem cho MaDinhDanh se la chuoi
// <<< CÂN NHẮC: Hàm này có thể được tối ưu hoặc thay thế một phần bởi tìm kiếm bảng băm >>>
// <<< Hiện tại giữ nguyên, nhưng khi gọi từ UI, nếu tìm theo mã, sẽ ưu tiên bảng băm >>>
void timKiemBenhNhan(const BenhNhan dsBenhNhan[], int soLuongBenhNhan,
    const char* tieuChi, const char* giaTriTimKiem,
    BenhNhan** ketQuaTimKiem, int* soLuongKetQua);

// <<< CAP NHAT CHU KY HAM (NẾU MUỐN KIỂM TRA TRÙNG TRONG BẢNG BĂM) >>>
// Hiện tại, dsBenhNhanHienCo được dùng để kiểm tra trùng.
// Nếu muốn dùng bảng băm để kiểm tra trùng khi nhập, cần truyền bbBenhNhan vào đây.
int nhapThongTinBacSiTuBanPhim(BacSi* bs,
    BangBam* bbBacSiHienCo, // Đây chính là nơi bbBacSiHienCo được định nghĩa như một tham số
    const BacSi dsBacSiHienCo[], int soLuongBSHienCo,
    const KhoaPhong dsKhoaPhong[], int soLuongKhoaPhong);

// Hàm này sẽ được thay thế bằng timBenhNhanTheoMaBangBam khi tìm theo mã từ UI
// Giữ lại để các module khác (nếu có) chưa cập nhật vẫn dùng được, hoặc để so sánh.
const BenhNhan* timBenhNhanTheoMaChuoi(const BenhNhan dsBenhNhan[], int soLuong, const char* maDD_str);

// <<< KHAI BAO HAM MOI >>>
BenhNhan* timBenhNhanTheoMaBangBam(BangBam* bb, const char* maDD_str);


const char* gioiTinhToString(GioiTinh gt);
GioiTinh stringToGioiTinh(const char* str);

#endif // PATIENT_H