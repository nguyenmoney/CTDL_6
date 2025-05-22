#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include "structs.h"
#include "hash_table.h" // <<<< THÊM INCLUDE NÀY
#include <stdio.h>

#define FILE_KHOA_PHONG "data/khoaphong.csv"

// --- Khai bao cac ham ---

/**
 * @brief Doc danh sach khoa phong tu file CSV vao mot mang dong.
 *
 * @param tenFile Ten file CSV.
 * @param soLuong Con tro de luu tru so luong khoa phong da doc.
 * @return Con tro toi mang KhoaPhong, hoac NULL neu loi. Nguoi goi giai phong bo nho.
 */
KhoaPhong* docDanhSachKhoaPhong(const char* tenFile, int* soLuong);

/**
 * @brief Ghi danh sach khoa phong (tu mang) vao file CSV (ghi de).
 *
 * @param tenFile Ten file CSV.
 * @param dsKhoaPhong Mang chua danh sach khoa phong.
 * @param soLuong So luong khoa phong trong mang.
 */
void ghiDanhSachKhoaPhongRaFile(const char* tenFile, const KhoaPhong dsKhoaPhong[], int soLuong);

/**
 * @brief Them mot khoa phong moi vao danh sach (trong bo nho va file CSV).
 * SỬA ĐỔI: Thêm BangBam** bbKhoaPhong_ptr
 * @param dsKhoaPhongConTro Con tro toi con tro mang KhoaPhong (de realloc).
 * @param soLuongConTro Con tro toi bien so luong khoa phong.
 * @param bbKhoaPhong_ptr Con tro toi con tro BangBam Khoa Phong.
 * @param khoaPhongMoi Thong tin khoa phong moi can them.
 * @param tenFile Ten file CSV de cap nhat.
 * @return 1 neu them thanh cong, 0 neu that bai (vi du: trung ma).
 */
int themKhoaPhong(KhoaPhong** dsKhoaPhongConTro, int* soLuongConTro,
    BangBam** bbKhoaPhong_ptr,
    KhoaPhong khoaPhongMoi, const char* tenFile);

/**
 * @brief Sua thong tin mot khoa phong trong danh sach.
 * Ma Khoa Phong (khóa) là bất biến.
 * @param dsKhoaPhong Mang khoa phong.
 * @param soLuong So luong khoa phong.
 * @param maKhoaPhongCanSua Ma khoa phong can sua.
 * @param thongTinMoi Thong tin moi de cap nhat.
 * @param tenFile Ten file CSV.
 * @return 1 neu sua thanh cong, 0 neu khong tim thay hoac loi.
 */
int suaThongTinKhoaPhong(KhoaPhong dsKhoaPhong[], int soLuong, const char* maKhoaPhongCanSua, KhoaPhong thongTinMoi, const char* tenFile);

/**
 * @brief Xoa mot khoa phong khoi danh sach.
 * SỬA ĐỔI: Thêm BangBam* bbKhoaPhong
 * @param dsKhoaPhongConTro Con tro toi con tro mang KhoaPhong.
 * @param soLuongConTro Con tro toi bien so luong khoa phong.
 * @param bbKhoaPhong Con tro toi BangBam Khoa Phong.
 * @param maKhoaPhongCanXoa Ma khoa phong can xoa.
 * @param tenFile Ten file CSV.
 * @return 1 neu xoa thanh cong, 0 neu khong tim thay hoac loi.
 */
int xoaKhoaPhong(KhoaPhong** dsKhoaPhongConTro, int* soLuongConTro,
    BangBam* bbKhoaPhong,
    const char* maKhoaPhongCanXoa, const char* tenFile);

/**
 * @brief In danh sach khoa phong ra man hinh console.
 *
 * @param dsKhoaPhong Mang khoa phong.
 * @param soLuong So luong khoa phong.
 */
void inDanhSachKhoaPhongConsole(const KhoaPhong dsKhoaPhong[], int soLuong);

/**
 * @brief Tim kiem khoa phong theo ma (tìm kiếm tuyến tính trên mảng).
 *
 * @param dsKhoaPhong Mang khoa phong.
 * @param soLuong So luong khoa phong.
 * @param maKhoaPhong Ma khoa phong can tim.
 * @return Con tro toi KhoaPhong tim thay, hoac NULL neu khong tim thay.
 */
KhoaPhong* timKhoaPhongTheoMa(const KhoaPhong dsKhoaPhong[], int soLuong, const char* maKhoaPhong);

/**
 * @brief HÀM MỚI: Tim kiem khoa phong theo ma su dung bang bam.
 *
 * @param bbKhoaPhong Con tro toi BangBam Khoa Phong.
 * @param maKhoaPhong Ma khoa phong can tim (sẽ được chuẩn hóa bên trong hàm).
 * @return Con tro toi KhoaPhong tim thay, hoac NULL neu khong tim thay.
 */
KhoaPhong* timKhoaPhongTheoMaBangBam(BangBam* bbKhoaPhong, const char* maKhoaPhong);


/**
 * @brief Nhap thong tin cho mot khoa phong tu ban phim.
 * SỬA ĐỔI: Thêm BangBam* bbKhoaPhongHienCo
 * Ma Khoa Phong phai la duy nhat.
 * @param kp Con tro toi struct KhoaPhong de dien thong tin.
 * @param bbKhoaPhongHienCo Con tro toi BangBam Khoa Phong hien co (de kiem tra trung ma).
 * @param dsKhoaPhongHienCo Mang cac khoa phong hien co (có thể không cần nếu chỉ kiểm tra bằng bảng băm).
 * @param soLuongHienCo So luong khoa phong hien co.
 * @return 1 neu nhap thanh cong va ma la duy nhat, 0 neu ma bi trung hoac nhap khong hop le.
 */
int nhapThongTinKhoaPhongTuBanPhim(KhoaPhong* kp,
    BangBam* bbKhoaPhongHienCo,
    const KhoaPhong dsKhoaPhongHienCo[], int soLuongHienCo); // Giữ dsKhoaPhongHienCo nếu có logic kiểm tra khác ngoài mã
void timKiemKhoaPhongTuyenTinh(const KhoaPhong dsKhoaPhong[], int soLuongKhoaPhong,
    const char* tieuChi, const char* giaTriTimKiem,
    KhoaPhong** ketQuaTimKiem, int* soLuongKetQua);

#endif // DEPARTMENT_H