#ifndef PRESCRIPTION_H
#define PRESCRIPTION_H

#include "structs.h" // De su dung DonThuocChiTiet, Thuoc (cho kiem tra)
#include <stdio.h>
#include "hash_table.h"

#define FILE_DON_THUOC_CHI_TIET "data/donthuoc.csv" 

// --- Khai bao cac ham ---

/**
 * @brief Doc danh sach chi tiet cac dong don thuoc tu file CSV vao mang dong.
 * Các trường maThuoc, soLuong, lieuDung, cachDung phải có giá trị hợp lệ.
 * @param tenFile Ten file CSV.
 * @param soLuongDongConTro Con tro de luu tru so luong dong don thuoc da doc.
 * @return Con tro toi mang DonThuocChiTiet, hoac NULL neu loi. Nguoi goi giai phong bo nho.
 */
DonThuocChiTiet* docDanhSachDonThuocChiTiet(const char* tenFile, int* soLuongDongConTro);

/**
 * @brief Ghi danh sach chi tiet cac dong don thuoc (tu mang) vao file CSV (ghi de).
 * Đảm bảo các trường không rỗng khi ghi.
 * @param tenFile Ten file CSV.
 * @param dsDonThuocChiTiet Mang chua danh sach cac dong don thuoc.
 * @param soLuongDong So luong dong don thuoc trong mang.
 */
void ghiDanhSachDonThuocChiTietRaFile(const char* tenFile, const DonThuocChiTiet dsDonThuocChiTiet[], int soLuongDong);

/**
 * @brief Them mot dong thuoc moi vao danh sach chi tiet don thuoc.
 * Hàm này không kiểm tra logic nghiệp vụ phức tạp, chỉ thêm vào danh sách và file.
 * Các trường trong dtctMoi phải hợp lệ (không rỗng, maThuoc tồn tại - kiểm tra ở lớp gọi).
 * @param dsDTCTConTro Con tro toi con tro mang DonThuocChiTiet (de realloc).
 * @param soLuongDongConTro Con tro toi bien so luong dong.
 * @param dtctMoi Dong don thuoc chi tiet moi can them.
 * @param tenFile Ten file CSV.
 * @return 1 neu them thanh cong, 0 neu that bai.
 */
int themDongDonThuocChiTiet(DonThuocChiTiet** dsDTCTConTro, int* soLuongDongConTro, DonThuocChiTiet dtctMoi, const char* tenFile);

/**
 * @brief Xoa mot dong thuoc cu the khoi mot don thuoc.
 *
 * @param dsDTCTConTro Con tro toi con tro mang DonThuocChiTiet.
 * @param soLuongDongConTro Con tro toi bien so luong dong.
 * @param maDonThuocCanTim Ma don thuoc (tuong ung voi LanKham.maDonThuoc).
 * @param maThuocCanXoa Ma thuoc can xoa khoi don do.
 * @param tenFile Ten file CSV.
 * @return 1 neu xoa thanh cong, 0 neu khong tim thay hoac loi.
 */
int xoaDongThuocTrongDon(DonThuocChiTiet** dsDTCTConTro, int* soLuongDongConTro, int maDonThuocCanTim, const char* maThuocCanXoa, const char* tenFile);

/**
 * @brief Sua thong tin mot dong thuoc cu the trong mot don thuoc.
 * Các trường trong thongTinMoi phải hợp lệ (không rỗng).
 * @param dsDonThuocChiTiet Mang cac dong don thuoc chi tiet.
 * @param soLuongDong So luong dong trong mang.
 * @param maDonThuocCanTim Ma don thuoc.
 * @param maThuocCanSua Ma thuoc can sua.
 * @param thongTinMoi Thong tin moi cho dong thuoc (so luong, lieu dung, cach dung). Ma thuoc khong doi.
 * @param tenFile Ten file CSV.
 * @return 1 neu sua thanh cong, 0 neu khong tim thay hoac loi.
 */
int suaDongThuocTrongDon(DonThuocChiTiet dsDonThuocChiTiet[], int soLuongDong, int maDonThuocCanTim, const char* maThuocCanSua, DonThuocChiTiet thongTinMoi, const char* tenFile);

/**
 * @brief Hien thi chi tiet mot don thuoc (tat ca cac thuoc trong don do).
 */
void inChiTietMotDonThuocConsole(int maDonThuoc,
    const DonThuocChiTiet dsDonThuocChiTiet[], int soLuongDongTongCong,
    const Thuoc dsThuocToanBo[], int soLuongThuocTB);

/**
 * @brief In tat ca cac dong trong file donthuoc.csv ra console (chu yeu de debug hoac tong quan).
 */
void inTatCaDonThuocChiTietConsole(const DonThuocChiTiet dsDonThuocChiTiet[], int soLuongDong,
    const Thuoc dsThuocToanBo[], int soLuongThuocTB);

/**
 * @brief Ham chinh de cap nhat (them/sua/xoa cac dong thuoc) cho mot don thuoc da co.
 * Nguoi dung se nhap Ma Don Thuoc (tu Lan Kham), sau do co the them thuoc, sua thuoc, xoa thuoc.
 * Sẽ sử dụng timThuocTheoMaBangBam (thông qua dsThuocToanBo và bảng băm tương ứng) để xác thực thuốc.
 * Tất cả các thông tin nhập vào (maThuoc, soLuong, lieuDung, cachDung) phải không rỗng.
 * @param maDonThuocDeCapNhat Ma don thuoc can thao tac.
 * @param dsDTCTConTro Con tro toi con tro mang DonThuocChiTiet.
 * @param soLuongDongConTro Con tro toi bien so luong dong.
 * @param dsThuocToanBo Mang tat ca cac loai thuoc (de chon thuoc và xác thực).
 * @param soLuongThuocTB So luong thuoc.
 * @param bbThuoc Con trỏ tới bảng băm Thuốc (để tìm kiếm thuốc hiệu quả).
 * @param tenFileCSV Ten file CSV de luu.
 */
void quanLyCapNhatChiTietDonThuoc(int maDonThuocDeCapNhat,
    DonThuocChiTiet** dsDTCTConTro, int* soLuongDongConTro,
    const Thuoc dsThuocToanBo[], int soLuongThuocTB,
    BangBam* bbThuoc, const char* tenFileCSV);

#endif // PRESCRIPTION_H