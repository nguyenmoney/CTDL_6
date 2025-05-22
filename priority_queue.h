#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "structs.h"    
#include "hash_table.h" 
#include <stdio.h>

// Cấu trúc cho một hàng đợi ưu tiên cụ thể (cho một khoa)
typedef struct {
    LanKham* dsChoKham;
    int soLuongHienTai;
    int dungLuong;
} HangDoiUuTienMotKhoa;

// --- Khai báo các hàm ---

/**
 * @brief Khởi tạo một cấu trúc HangDoiUuTienMotKhoa mới.
 * @param hqmk Con trỏ tới HangDoiUuTienMotKhoa cần khởi tạo.
 */
void khoiTaoHangDoiMotKhoa(HangDoiUuTienMotKhoa* hqmk);

/**
 * @brief Thêm một LanKham vào hàng đợi của một khoa phòng cụ thể.
 * Hàng đợi của khoa sẽ được sắp xếp lại (bằng Merge Sort) sau khi thêm.
 * Nếu khoa phòng chưa có hàng đợi, một hàng đợi mới sẽ được tạo và thêm vào bảng băm bbHangDoiTheoKhoa.
 *
 * @param bbHangDoiTheoKhoa Bảng băm chứa các hàng đợi, key là maKhoaPhong (chuỗi đã chuẩn hóa), value là HangDoiUuTienMotKhoa*.
 * @param maKhoaPhong Mã khoa phòng của hàng đợi cần thêm vào (sẽ được chuẩn hóa bên trong).
 * @param lanKhamMoi LanKham cần thêm.
 * @return 1 nếu thêm thành công, 0 nếu thất bại (lỗi cấp phát bộ nhớ, không thể thêm vào bảng băm, v.v.).
 */
int themVaoHangDoiTheoKhoa(BangBam* bbHangDoiTheoKhoa, const char* maKhoaPhong, LanKham lanKhamMoi);

/**
 * @brief Lấy và xóa LanKham có ưu tiên cao nhất từ hàng đợi của một khoa phòng cụ thể.
 * Hàng đợi được sắp xếp bằng Merge Sort trước khi lấy (nếu cần thiết, mặc dù thường đã sắp xếp khi thêm).
 *
 * @param bbHangDoiTheoKhoa Bảng băm chứa các hàng đợi.
 * @param maKhoaPhong Mã khoa phòng của hàng đợi (sẽ được chuẩn hóa bên trong).
 * @param lanKhamLayRa Con trỏ để lưu trữ LanKham được lấy ra.
 * @return 1 nếu lấy thành công, 0 nếu hàng đợi của khoa rỗng hoặc khoa không có hàng đợi.
 */
int layBenhNhanTiepTheoTheoKhoa(BangBam* bbHangDoiTheoKhoa, const char* maKhoaPhong, LanKham* lanKhamLayRa);

/**
 * @brief Kiểm tra xem hàng đợi của một khoa phòng cụ thể có rỗng hay không.
 *
 * @param bbHangDoiTheoKhoa Bảng băm chứa các hàng đợi.
 * @param maKhoaPhong Mã khoa phòng (sẽ được chuẩn hóa bên trong).
 * @return 1 nếu rỗng hoặc không tồn tại, 0 nếu không rỗng.
 */
int laHangDoiTheoKhoaRong(BangBam* bbHangDoiTheoKhoa, const char* maKhoaPhong);

/**
 * @brief In ra danh sách bệnh nhân đang chờ trong hàng đợi của một khoa phòng cụ thể.
 *
 * @param bbHangDoiTheoKhoa Bảng băm chứa các hàng đợi.
 * @param maKhoaPhong Mã khoa phòng (sẽ được chuẩn hóa bên trong).
 * @param bbBenhNhan Bảng băm bệnh nhân để lấy tên (ưu tiên hơn dsBenhNhanToanBo).
 * @param dsBenhNhanToanBo Mảng tất cả bệnh nhân (dùng nếu bbBenhNhan là NULL).
 * @param soLuongBNTB Số lượng bệnh nhân trong dsBenhNhanToanBo.
 */
void inHangDoiMotKhoaConsole(BangBam* bbHangDoiTheoKhoa, const char* maKhoaPhong,
    BangBam* bbBenhNhan,
    const BenhNhan dsBenhNhanToanBo[], int soLuongBNTB);

/**
 * @brief Giải phóng bộ nhớ cho một HangDoiUuTienMotKhoa (chỉ giải phóng dsChoKham).
 * @param hqmk Con trỏ tới HangDoiUuTienMotKhoa.
 */
void giaiPhongDuLieuHangDoiMotKhoa(HangDoiUuTienMotKhoa* hqmk);


/**
 * @brief Giải phóng toàn bộ bộ nhớ được sử dụng bởi tất cả các hàng đợi trong bbHangDoiTheoKhoa.
 * Bao gồm việc giải phóng từng HangDoiUuTienMotKhoa* và các khóa maKhoaPhong đã strdup.
 * Cuối cùng giải phóng chính bbHangDoiTheoKhoa.
 *
 * @param bbHangDoiTheoKhoa Bảng băm chứa các hàng đợi cần giải phóng.
 */
void giaiPhongTatCaHangDoiTheoKhoa(BangBam* bbHangDoiTheoKhoa);

#endif // PRIORITY_QUEUE_H