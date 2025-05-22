#ifndef MEDICINE_H
#define MEDICINE_H

#include "structs.h"
#include "hash_table.h" // <<<< THÊM INCLUDE NÀY
#include <stdio.h>

#define FILE_THUOC "data/thuoc.csv"

// --- Khai bao cac ham ---

/**
 * @brief Doc danh sach thuoc tu file CSV vao mot mang dong.
 * Các trường maThuoc, tenThuoc phải có giá trị.
 * @param tenFile Ten file CSV.
 * @param soLuong Con tro de luu tru so luong thuoc da doc.
 * @return Con tro toi mang Thuoc, hoac NULL neu loi. Nguoi goi giai phong bo nho.
 */
Thuoc* docDanhSachThuoc(const char* tenFile, int* soLuong);

/**
 * @brief Ghi danh sach thuoc (tu mang) vao file CSV (ghi de).
 * Đảm bảo các trường không rỗng khi ghi.
 * @param tenFile Ten file CSV.
 * @param dsThuoc Mang chua danh sach thuoc.
 * @param soLuong So luong thuoc trong mang.
 */
void ghiDanhSachThuocRaFile(const char* tenFile, const Thuoc dsThuoc[], int soLuong);

/**
 * @brief Them mot loai thuoc moi vao danh sach (trong bo nho, bảng băm và file CSV).
 * SỬA ĐỔI: Thêm BangBam** bbThuoc_ptr
 * @param dsThuocConTro Con tro toi con tro mang Thuoc (de realloc).
 * @param soLuongConTro Con tro toi bien so luong thuoc.
 * @param bbThuoc_ptr Con tro toi con tro BangBam Thuoc.
 * @param thuocMoi Thong tin thuoc moi can them. Ma thuoc phai duy nhat, các trường không được rỗng.
 * @param tenFile Ten file CSV de cap nhat.
 * @return 1 neu them thanh cong, 0 neu that bai (vi du: trung ma, thông tin rỗng).
 */
int themThuoc(Thuoc** dsThuocConTro, int* soLuongConTro,
    BangBam** bbThuoc_ptr,
    Thuoc thuocMoi, const char* tenFile);

/**
 * @brief Sua thong tin mot loai thuoc trong danh sach.
 * Chi sua ten thuoc, ma thuoc khong doi. Ten thuốc mới không được rỗng.
 * @param dsThuoc Mang thuoc.
 * @param soLuong So luong thuoc.
 * @param maThuocCanSua Ma thuoc can sua thong tin.
 * @param thongTinMoi Thong tin ten thuoc moi. Ma thuoc trong thongTinMoi se bi bo qua.
 * @param tenFile Ten file CSV.
 * @return 1 neu sua thanh cong, 0 neu khong tim thay hoac loi.
 */
int suaThongTinThuoc(Thuoc dsThuoc[], int soLuong, const char* maThuocCanSua, Thuoc thongTinMoi, const char* tenFile);

/**
 * @brief Xoa mot loai thuoc khoi danh sach và bảng băm.
 * SỬA ĐỔI: Thêm BangBam* bbThuoc
 * @param dsThuocConTro Con tro toi con tro mang Thuoc.
 * @param soLuongConTro Con tro toi bien so luong thuoc.
 * @param bbThuoc Con tro toi BangBam Thuoc.
 * @param maThuocCanXoa Ma thuoc can xoa.
 * @param tenFile Ten file CSV.
 * @return 1 neu xoa thanh cong, 0 neu khong tim thay hoac loi.
 */
int xoaThuoc(Thuoc** dsThuocConTro, int* soLuongConTro,
    BangBam* bbThuoc,
    const char* maThuocCanXoa, const char* tenFile);

/**
 * @brief In danh sach thuoc ra man hinh console.
 */
void inDanhSachThuocConsole(const Thuoc dsThuoc[], int soLuong);

/**
 * @brief Tim kiem thuoc theo ma (tìm kiếm tuyến tính).
 */
Thuoc* timThuocTheoMa(const Thuoc dsThuoc[], int soLuong, const char* maThuoc);


/**
 * @brief HÀM MỚI: Tim kiem thuoc theo ma su dung bang bam.
 * @param bbThuoc Con tro toi BangBam Thuoc.
 * @param maThuoc Ma thuoc can tim (sẽ được chuẩn hóa bên trong hàm).
 * @return Con tro toi Thuoc tim thay, hoac NULL neu khong tim thay.
 */
Thuoc* timThuocTheoMaBangBam(BangBam* bbThuoc, const char* maThuoc);

/**
 * @brief Nhap thong tin cho mot loai thuoc moi tu ban phim.
 * SỬA ĐỔI: Thêm BangBam* bbThuocHienCo
 * Ma Thuoc, Ten Thuoc phai được nhập và không rỗng. Ma Thuoc duy nhất.
 * @param tConTro Con tro toi struct Thuoc de dien thong tin.
 * @param bbThuocHienCo Con tro toi BangBam Thuoc hien co (de kiem tra trung ma).
 * @param dsThuocHienCo Mang cac loai thuoc hien co (de kiem tra trung ten).
 * @param soLuongThuocHienCo So luong thuoc hien co.
 * @return 1 neu nhap thanh cong, 0 neu ma/ten bi trung hoac nhap khong hop le.
 */
int nhapThongTinThuocTuBanPhim(Thuoc* tConTro,
    BangBam* bbThuocHienCo,
    const Thuoc dsThuocHienCo[], int soLuongThuocHienCo);
void timKiemThuocTheoTenTuyenTinh(const Thuoc dsThuoc[], int soLuongThuoc,
    const char* tenThuocTimKiem,
    Thuoc** ketQuaTimKiem, int* soLuongKetQua);
#endif // MEDICINE_H