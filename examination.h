#ifndef EXAMINATION_H
#define EXAMINATION_H

#include "structs.h"
#include "patient.h" 
#include "hash_table.h" // Cho BangBam
#include "priority_queue.h" // Cho BangBam* bbHangDoiTheoKhoa (nếu khai báo ở đây)

#define FILE_LAN_KHAM "data/lankham.csv"

// --- Khai báo các hàm ---

/**
 * @brief Đọc danh sách các lần khám từ file CSV.
 * Các trường bắt buộc như maLanKham, maDinhDanh, maKhoaPhong phải hợp lệ.
 * ChanDoan, PhuongPhapDieuTri không được rỗng nếu trangThai là DaHoanThanh.
 */
LanKham* docDanhSachLanKham(const char* tenFile, int* soLuong);

/**
 * @brief Ghi danh sách lần khám vào file CSV.
 * Đảm bảo các trường bắt buộc không rỗng.
 */
void ghiDanhSachLanKhamRaFile(const char* tenFile, const LanKham dsLanKham[], int soLuong);

/**
 * @brief Tạo mã lần khám mới, duy nhất (dạng số nguyên).
 */
int taoMaLanKhamMoi(const LanKham dsLanKham[], int soLuong); // Giữ nguyên trả về int

/**
 * @brief Đăng ký một lượt khám mới cho bệnh nhân.
 * SỬA ĐỔI: Thêm bbLanKham_ptr, bbHangDoiTheoKhoa.
 * Yêu cầu nhập maKhoaPhong.
 * @return Mã lần khám mới (int) nếu thành công, -1 nếu thất bại.
 */
int dangKyKhamMoi(LanKham** dsLanKhamConTro, int* soLuongLanKhamConTro,
    BenhNhan** dsBenhNhanConTro, int* soLuongBenhNhanConTro,
    BangBam* bbBenhNhan,
    KhoaPhong* dsKhoaPhong, int soLuongKhoaPhong,
    BangBam* bbKhoaPhong, // Để xác thực khoa phòng bằng bảng băm nếu có
    BangBam** bbLanKham_ptr, // Bảng băm cho Lần Khám (key là maLanKham dạng chuỗi)
    BangBam* bbHangDoiTheoKhoa, // Bảng băm các hàng đợi theo khoa
    const char* fileLanKhamCSV, const char* fileBenhNhanCSV);

/**
 * @brief Cập nhật thông tin một lần khám.
 * ChanDoan, PhuongPhapDieuTri không được rỗng nếu cập nhật trạng thái thành DaHoanThanh.
 */
int capNhatThongTinLanKham(LanKham dsLanKham[], int soLuongLanKham, int maLanKhamCanCapNhat,
    const BacSi dsBacSi[], int soLuongBacSi,
    const KhoaPhong dsKhoaPhong[], int soLuongKhoaPhong,
    BangBam* bbBacSi, // Để tìm bác sĩ bằng bảng băm
    BangBam* bbKhoaPhong, // Để tìm khoa phòng bằng bảng băm
    const char* fileLanKhamCSV);


/**
 * @brief Thêm một lần khám vào danh sách và bảng băm.
 * SỬA ĐỔI: Thêm bbLanKham_ptr.
 * @return 1 nếu thành công, 0 nếu thất bại.
 */
int themLanKham(LanKham** dsLanKhamConTro, int* soLuongConTro,
    BangBam** bbLanKham_ptr,
    LanKham lanKhamMoi, const char* tenFile);

/**
 * @brief Xóa một lần khám khỏi danh sách và bảng băm.
 * SỬA ĐỔI: Thêm bbLanKham.
 * @return 1 nếu thành công, 0 nếu thất bại.
 */
int xoaLanKham(LanKham** dsLanKhamConTro, int* soLuongConTro,
    BangBam* bbLanKham,
    int maLanKhamCanXoa, const char* tenFile);

/**
 * @brief In danh sách lần khám ra màn hình console.
 */
void inDanhSachLanKhamConsole(const LanKham dsLanKham[], int soLuongLanKham,
    const BenhNhan dsBenhNhan[], int soLuongBenhNhan,
    BangBam* bbBenhNhan); // Thêm bbBenhNhan để lấy tên BN hiệu quả

/**
 * @brief Tìm kiếm lần khám theo các tiêu chí (tìm kiếm tuyến tính).
 */
void timKiemLanKham(const LanKham dsLanKham[], int soLuongLanKham,
    const char* tieuChi, const char* giaTriTimKiem,
    LanKham** ketQuaTimKiem, int* soLuongKetQua);

/**
 * @brief HÀM MỚI: Tìm lần khám theo mã sử dụng bảng băm.
 * @param bbLanKham Bảng băm Lần Khám.
 * @param maLK Mã lần khám (int) cần tìm.
 * @return Con trỏ tới LanKham nếu tìm thấy, NULL nếu không.
 */
LanKham* timLanKhamTheoMaBangBam(BangBam* bbLanKham, int maLK);


/**
 * @brief Sửa chi tiết một lần khám.
 * ChanDoan, PhuongPhapDieuTri không được rỗng nếu trạng thái là DaHoanThanh.
 */
int suaChiTietLanKham(LanKham dsLanKham[], int soLuongLanKham, int maLanKhamCanSua,
    const BacSi dsBacSi[], int soLuongBacSi,
    const KhoaPhong dsKhoaPhong[], int soLuongKhoaPhong,
    BangBam* bbBacSi,   // Để tìm bác sĩ bằng bảng băm
    BangBam* bbKhoaPhong, // Để tìm khoa phòng bằng bảng băm
    const char* fileLanKhamCSV);

// Hàm tiện ích chuyển đổi enum sang chuỗi và ngược lại
const char* trangThaiLanKhamToString(TrangThaiLanKham tt);
TrangThaiLanKham stringToTrangThaiLanKham(const char* str);
const char* mucDoUuTienToString(MucDoUuTien md);
MucDoUuTien stringToMucDoUuTien(const char* str);

#endif // EXAMINATION_H