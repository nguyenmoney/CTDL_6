#include "ui.h"
#include "patient.h"        
#include "examination.h"    
#include "department.h"   
#include "doctor.h"       
#include "medicine.h"     
#include "prescription.h" 
#include "priority_queue.h"
#include "csv_handler.h"  
#include "structs.h"      
#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h>  
#include <time.h>   
#include <math.h>   

// --- Dinh nghia cac ham tien ich giao dien (giữ nguyên) ---
// xoaManHinhConsole, dungManHinhCho, nhapSoNguyenCoGioiHan, 
// nhapSoNguyenKhongAm, inThongDiepLoi, inThongDiepThanhCong

void xoaManHinhConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear"); // Hoặc vòng lặp in newline
#endif
}

void dungManHinhCho() {
    printf("\nNhan Enter de tiep tuc...");
    // Xử lý bộ đệm đầu vào an toàn
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Đọc và loại bỏ ký tự còn lại
    if (c == EOF && feof(stdin)) { // Nếu gặp EOF thực sự trên stdin
        clearerr(stdin); // Xóa trạng thái lỗi/EOF để getchar tiếp theo có thể hoạt động (nếu cần)
    }
    else if (c != '\n') { // Nếu không phải là newline (ví dụ sau khi đọc số không kèm getchar())
        // Không cần làm gì thêm nếu trước đó là docDongAnToan, vì nó đã xử lý newline
    }
    // Không cần getchar() thêm ở đây nếu các hàm nhập trước đó đã xử lý newline (như docDongAnToan)
}


int nhapSoNguyenCoGioiHan(const char* prompt, int min, int max) {
    int value;
    char buffer[100];
    while (1) {
        printf("%s (%d-%d): ", prompt, min, max);
        if (docDongAnToan(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &value) == 1 && value >= min && value <= max) {
                // Kiểm tra xem có ký tự thừa sau số không
                char* endptr;
                strtol(buffer, &endptr, 10);
                if (*endptr == '\0' || isspace((unsigned char)*endptr)) {
                    return value;
                }
            }
        }
        else { // Lỗi đọc hoặc EOF
            printf("\nLoi nhap lieu hoac ket thuc input. Quay lai.\n");
            return min - 1; // Giá trị báo lỗi
        }
        inThongDiepLoi("Lua chon khong hop le. Vui long nhap lai mot so nguyen.");
    }
}

void inThongDiepLoi(const char* message) {
    printf("LOI: %s\n", message);
}

void inThongDiepThanhCong(const char* message) {
    printf("THANH CONG: %s\n", message);
}


// --- Dinh nghia cac ham menu con (CẬP NHẬT CHỮ KÝ VÀ LOGIC) ---

// menuQuanLyBenhNhan (giữ nguyên logic bên trong, chữ ký đã đúng)
// Ham menuQuanLyBenhNhan (da bao gom chuc nang sua co chon loc)
void menuQuanLyBenhNhan(BenhNhan** dsBenhNhan, int* soLuongBenhNhan, BangBam** bbBenhNhan_ptr, const char* tenFileCSV) {
    BangBam* bbBenhNhan = *bbBenhNhan_ptr; //
    int choice;
    do {
        xoaManHinhConsole();
        printf("--- QUAN LY BENH NHAN ---\n");
        printf("1. Them benh nhan moi\n");
        printf("2. Sua thong tin benh nhan\n");
        printf("3. Xoa benh nhan\n");
        printf("4. Xem danh sach benh nhan\n");
        printf("5. Tim kiem benh nhan\n");
        printf("0. Quay lai menu chinh\n");
        choice = nhapSoNguyenCoGioiHan("Lua chon cua ban", 0, 5); //
        if (choice < 0) { choice = 0; } // Xử lý nếu nhapSoNguyenCoGioiHan trả về lỗi

        switch (choice) {
        case 1: { // Them benh nhan moi
            BenhNhan bnMoi;
            printf("\n--- THEM BENH NHAN MOI ---\n");
            // Khi gọi nhapThongTin..., chúng ta cần BangBam* (giá trị của con trỏ)
            if (nhapThongTinBenhNhanTuBanPhim(&bnMoi, *dsBenhNhan, *soLuongBenhNhan, *bbBenhNhan_ptr)) { //
                // Khi gọi themBenhNhan, chúng ta truyền con trỏ cấp hai BangBam**
                if (themBenhNhan(dsBenhNhan, soLuongBenhNhan, bbBenhNhan_ptr, bnMoi, tenFileCSV)) { //
                    inThongDiepThanhCong("Them benh nhan moi thanh cong!"); //
                }
                else {
                    inThongDiepLoi("Them benh nhan moi that bai (co the do loi dong bo voi bang bam hoac ma da ton tai)."); //
                }
            }
            else {
                inThongDiepLoi("Huy them benh nhan moi hoac thong tin nhap khong hop le/bi trung."); //
            }
            dungManHinhCho(); //
            break;
        }
        case 2: { // Sua thong tin benh nhan
            char maSua_str_input[MAX_LEN_MA_DINH_DANH_BN]; // Buffer cho input
            char maSua_str_upper[MAX_LEN_MA_DINH_DANH_BN]; // Buffer cho mã đã uppercase

            printf("Nhap Ma Dinh Danh cua benh nhan can sua: ");
            if (docDongAnToan(maSua_str_input, sizeof(maSua_str_input), stdin) == NULL || strlen(maSua_str_input) == 0) { //
                inThongDiepLoi("Ma dinh danh khong duoc de trong."); //
                dungManHinhCho(); //
                break;
            }
            // Chuyển mã nhập vào thành chữ hoa để tìm kiếm và so sánh
            strncpy(maSua_str_upper, maSua_str_input, sizeof(maSua_str_upper) - 1); //
            maSua_str_upper[sizeof(maSua_str_upper) - 1] = '\0'; //
            for (int k = 0; maSua_str_upper[k]; k++) maSua_str_upper[k] = toupper(maSua_str_upper[k]); //

            BenhNhan* bnCanSuaTrucTiep = NULL; //
            int benhNhanIndex = -1; // Vẫn cần để kiểm tra sự đồng bộ

            if (bbBenhNhan != NULL) { //
                bnCanSuaTrucTiep = (BenhNhan*)timKiemTrongBangBam(bbBenhNhan, maSua_str_upper); //
            }
            else {
                inThongDiepLoi("Bang bam benh nhan khong kha dung. Khong the tim benh nhan de sua."); //
                dungManHinhCho(); //
                break; // Thoát khỏi case 2
            }

            if (bnCanSuaTrucTiep != NULL) { //
                // Tìm index của bnCanSuaTrucTiep trong mảng dsBenhNhan để kiểm tra đồng bộ
                for (int i = 0; i < *soLuongBenhNhan; ++i) { //
                    if (&((*dsBenhNhan)[i]) == bnCanSuaTrucTiep) { //
                        benhNhanIndex = i; //
                        break; //
                    }
                }
                // Kiểm tra nếu không tìm thấy index tương ứng (lỗi đồng bộ nghiêm trọng)
                if (benhNhanIndex == -1) { //
                    inThongDiepLoi("Loi dong bo: Tim thay BN trong bang bam nhung khong tim thay trong mang chinh."); //
                    dungManHinhCho(); //
                    break; // Thoát khỏi case 2
                }

                BenhNhan bnTamSua = *bnCanSuaTrucTiep; // Tạo bản sao để sửa đổi tạm thời
                int suaLuaChonNoiBo; //
                char bufferNhapLieu[1000]; // Buffer chung cho các kiểu nhập liệu khác nhau

                do {
                    xoaManHinhConsole(); //
                    printf("--- SUA THONG TIN BENH NHAN (Ma DD: %s) ---\n", bnTamSua.maDinhDanh); //
                    printf("Thong tin hien tai:\n");
                    printf("  1. Ten           : %s\n", bnTamSua.ten); //
                    printf("  2. So Dien Thoai : %s\n", bnTamSua.soDienThoai); //
                    printf("  3. BHYT          : %s\n", bnTamSua.bhyt); //
                    char ngaySinhStr_ui[12]; //
                    snprintf(ngaySinhStr_ui, sizeof(ngaySinhStr_ui), "%02d/%02d/%04d", bnTamSua.ngaySinh.ngay, bnTamSua.ngaySinh.thang, bnTamSua.ngaySinh.nam); //
                    printf("  4. Ngay Sinh     : %s\n", ngaySinhStr_ui); //
                    printf("  5. Tuoi          : %d\n", bnTamSua.tuoi); //
                    printf("  6. Gioi Tinh     : %s\n", gioiTinhToString(bnTamSua.gioiTinh)); //
                    printf("  7. Dia Chi       : %s\n", bnTamSua.diaChi); //
                    printf("  8. Tieu Su Benh Ly: %s\n", bnTamSua.tieuSuBenhLy); //
                    printf("-------------------------------------------------\n");
                    printf("Chon thong tin muon sua:\n");
                    printf("  1. Sua Ten\n  2. Sua So Dien Thoai\n  3. Sua BHYT\n  4. Sua Ngay Sinh\n  5. Sua Tuoi\n  6. Sua Gioi Tinh\n  7. Sua Dia Chi\n  8. Sua Tieu Su Benh Ly\n"); //
                    printf("-------------------------------------------------\n");
                    printf("  0. HOAN TAT SUA DOI VA LUU\n");
                    printf("  9. Huy bo tat ca thay doi va quay lai\n");

                    suaLuaChonNoiBo = nhapSoNguyenCoGioiHan("Lua chon cua ban", 0, 9); //
                    if (suaLuaChonNoiBo < 0) { suaLuaChonNoiBo = 9; } //

                    switch (suaLuaChonNoiBo) {
                    case 1: // Sua Ten
                        printf("Nhap Ten moi (hien tai: %s, KHONG DUOC BO TRONG): ", bnTamSua.ten); //
                        // Lưu lại tên cũ phòng trường hợp nhập rỗng
                        char tenCu[sizeof(bnTamSua.ten)]; //
                        strcpy(tenCu, bnTamSua.ten); //
                        docDongAnToan(bnTamSua.ten, sizeof(bnTamSua.ten), stdin); //
                        if (strlen(bnTamSua.ten) == 0) { //
                            inThongDiepLoi("Ten khong duoc de trong. Gia tri chua duoc thay doi."); //
                            strcpy(bnTamSua.ten, tenCu); // Khôi phục tên cũ
                        }
                        break;
                    case 2: printf("Nhap So Dien Thoai moi (hien tai: %s): ", bnTamSua.soDienThoai); docDongAnToan(bnTamSua.soDienThoai, sizeof(bnTamSua.soDienThoai), stdin); break; //
                    case 3: printf("Nhap BHYT moi (hien tai: %s): ", bnTamSua.bhyt); docDongAnToan(bnTamSua.bhyt, sizeof(bnTamSua.bhyt), stdin); break; //
                    case 4: { // Sua Ngay Sinh
                        printf("Nhap Ngay Sinh moi (ngay thang nam, vd: 15 01 1990, KHONG DUOC BO TRONG) (hien tai: %02d/%02d/%04d): ", //
                            bnTamSua.ngaySinh.ngay, bnTamSua.ngaySinh.thang, bnTamSua.ngaySinh.nam); //
                        Date ngaySinhCu = bnTamSua.ngaySinh; // Lưu ngày sinh cũ
                        if (docDongAnToan(bufferNhapLieu, sizeof(bufferNhapLieu), stdin) != NULL && strlen(bufferNhapLieu) > 0) { //
                            if (sscanf(bufferNhapLieu, "%d %d %d", &bnTamSua.ngaySinh.ngay, &bnTamSua.ngaySinh.thang, &bnTamSua.ngaySinh.nam) == 3) { //
                                // TODO: Thêm kiểm tra tính hợp lệ chi tiết của ngày, tháng, năm
                                if (bnTamSua.ngaySinh.ngay < 1 || bnTamSua.ngaySinh.ngay > 31 || bnTamSua.ngaySinh.thang < 1 || bnTamSua.ngaySinh.thang > 12 || bnTamSua.ngaySinh.nam < 1900 || bnTamSua.ngaySinh.nam > 2100) { //
                                    inThongDiepLoi("Ngay Sinh khong hop le (ngay/thang/nam khong dung). Gia tri chua duoc thay doi."); //
                                    bnTamSua.ngaySinh = ngaySinhCu; // Khôi phục
                                }
                                // Nếu hợp lệ thì không làm gì, giá trị mới đã được gán
                            }
                            else {
                                inThongDiepLoi("Dinh dang Ngay Sinh khong hop le. Gia tri chua duoc thay doi."); //
                                bnTamSua.ngaySinh = ngaySinhCu; // Khôi phục
                            }
                        }
                        else { // Người dùng nhập rỗng
                            inThongDiepLoi("Ngay Sinh khong duoc de trong. Gia tri chua duoc thay doi."); //
                            bnTamSua.ngaySinh = ngaySinhCu; // Khôi phục
                        }
                        break;
                    }
                    case 5: { // Sua Tuoi
                        printf("Nhap Tuoi moi (KHONG DUOC BO TRONG) (hien tai: %d): ", bnTamSua.tuoi); //
                        int tuoiCu = bnTamSua.tuoi; // Lưu tuổi cũ
                        if (docDongAnToan(bufferNhapLieu, sizeof(bufferNhapLieu), stdin) != NULL && strlen(bufferNhapLieu) > 0) { //
                            if (sscanf(bufferNhapLieu, "%d", &bnTamSua.tuoi) == 1 && bnTamSua.tuoi >= 0) { //
                                // Tuổi hợp lệ, không làm gì, giá trị mới đã được gán
                            }
                            else {
                                inThongDiepLoi("Tuoi khong hop le (phai la so nguyen khong am). Gia tri chua duoc thay doi."); //
                                bnTamSua.tuoi = tuoiCu; // Khôi phục
                            }
                        }
                        else { // Người dùng nhập rỗng
                            inThongDiepLoi("Tuoi khong duoc de trong. Gia tri chua duoc thay doi."); //
                            bnTamSua.tuoi = tuoiCu; // Khôi phục
                        }
                        break;
                    }
                    case 6:
                        printf("Nhap Gioi Tinh moi (Nam, Nu, Khac) (hien tai: %s): ", gioiTinhToString(bnTamSua.gioiTinh)); //
                        docDongAnToan(bufferNhapLieu, sizeof(bufferNhapLieu), stdin); // Đọc giới tính mới vào buffer
                        bnTamSua.gioiTinh = stringToGioiTinh(bufferNhapLieu); // Chuyển đổi và gán
                        break;
                    case 7: printf("Nhap Dia Chi moi (hien tai: %s): ", bnTamSua.diaChi); docDongAnToan(bnTamSua.diaChi, sizeof(bnTamSua.diaChi), stdin); break; //
                    case 8: printf("Nhap Tieu Su Benh Ly moi (hien tai: %s): ", bnTamSua.tieuSuBenhLy); docDongAnToan(bnTamSua.tieuSuBenhLy, sizeof(bnTamSua.tieuSuBenhLy), stdin); break; //
                    case 0: { // HOAN TAT SUA DOI VA LUU
                        // Trước khi lưu, kiểm tra lại lần cuối các trường bắt buộc trong bnTamSua
                        if (strlen(bnTamSua.ten) == 0) { //
                            inThongDiepLoi("LOI KHI LUU: Ten khong duoc de trong. Vui long sua lai."); //
                            suaLuaChonNoiBo = -1; // Đặt để vòng lặp tiếp tục, không thoát và không lưu
                            dungManHinhCho(); //
                            break; // Thoát khỏi switch, quay lại vòng lặp do-while
                        }
                        if (bnTamSua.ngaySinh.ngay < 1 || bnTamSua.ngaySinh.ngay > 31 || bnTamSua.ngaySinh.thang < 1 || bnTamSua.ngaySinh.thang > 12 || bnTamSua.ngaySinh.nam < 1900 || bnTamSua.ngaySinh.nam > 2100) { //
                            inThongDiepLoi("LOI KHI LUU: Ngay Sinh khong hop le. Vui long sua lai."); //
                            suaLuaChonNoiBo = -1; //
                            dungManHinhCho(); //
                            break; //
                        }
                        if (bnTamSua.tuoi < 0) { // Tuổi đã được kiểm tra >= 0 khi nhập, nhưng kiểm tra lại cho chắc
                            inThongDiepLoi("LOI KHI LUU: Tuoi khong hop le. Vui long sua lai."); //
                            suaLuaChonNoiBo = -1; //
                            dungManHinhCho(); //
                            break; //
                        }

                        // Nếu tất cả các kiểm tra cuối cùng đều ổn
                        char tempMaDinhDanh[MAX_LEN_MA_DINH_DANH_BN]; //
                        time_t tempNgayTaoHoSo; //

                        strcpy(tempMaDinhDanh, bnCanSuaTrucTiep->maDinhDanh); //
                        tempNgayTaoHoSo = bnCanSuaTrucTiep->ngayTaoHoSo; //

                        *bnCanSuaTrucTiep = bnTamSua; // Cập nhật toàn bộ thông tin từ bản tạm

                        strcpy(bnCanSuaTrucTiep->maDinhDanh, tempMaDinhDanh); // Khôi phục mã gốc
                        bnCanSuaTrucTiep->ngayTaoHoSo = tempNgayTaoHoSo;     // Khôi phục ngày tạo gốc

                        ghiDanhSachBenhNhanRaFile(tenFileCSV, *dsBenhNhan, *soLuongBenhNhan); //
                        inThongDiepThanhCong("Da luu cac thay doi cho benh nhan!"); //
                        // suaLuaChonNoiBo đã là 0, vòng lặp sẽ tự thoát
                        break; //
                    }
                    case 9:
                        inThongDiepLoi("Da huy bo sua doi."); //
                        suaLuaChonNoiBo = 0; //
                        break;
                    default:
                        inThongDiepLoi("Lua chon khong hop le."); //
                        dungManHinhCho(); //
                        break;
                    }
                    if (suaLuaChonNoiBo >= 1 && suaLuaChonNoiBo <= 8) { //
                        inThongDiepThanhCong("Da cap nhat truong. Chon truong khac hoac Luu/Huy."); //
                        dungManHinhCho(); //
                    }
                } while (suaLuaChonNoiBo != 0); //
            }
            else { // Không tìm thấy bệnh nhân trong bảng băm
                inThongDiepLoi("Khong tim thay benh nhan voi ma dinh danh da nhap trong bang bam."); //
            }
            dungManHinhCho(); //
            break;
        }
        case 3: { // Xoa benh nhan
            char maXoa_str_input[MAX_LEN_MA_DINH_DANH_BN]; //
            char maXoa_str_upper[MAX_LEN_MA_DINH_DANH_BN]; //
            printf("Nhap Ma Dinh Danh cua benh nhan can xoa: ");
            if (docDongAnToan(maXoa_str_input, sizeof(maXoa_str_input), stdin) == NULL || strlen(maXoa_str_input) == 0) { //
                inThongDiepLoi("Ma dinh danh khong duoc de trong."); //
                dungManHinhCho(); //
                break;
            }
            strncpy(maXoa_str_upper, maXoa_str_input, sizeof(maXoa_str_upper) - 1); //
            maXoa_str_upper[sizeof(maXoa_str_upper) - 1] = '\0'; //
            for (int k = 0; maXoa_str_upper[k]; k++) maXoa_str_upper[k] = toupper(maXoa_str_upper[k]); //

            // Xác nhận trước khi xóa (có thể hiển thị tên BN nếu tìm thấy bằng bảng băm)
            BenhNhan* bnCanXoa = NULL; //
            if (bbBenhNhan != NULL) { //
                bnCanXoa = (BenhNhan*)timKiemTrongBangBam(bbBenhNhan, maXoa_str_upper); //
            }
            if (bnCanXoa != NULL) { //
                printf("Ban co chac chan muon xoa benh nhan: %s (Ma DD: %s)? (c/k): ", bnCanXoa->ten, bnCanXoa->maDinhDanh); //
            }
            else {
                printf("Ban co chac chan muon xoa benh nhan co Ma DD: %s (Khong tim thay ten nhanh)? (c/k): ", maXoa_str_upper); //
            }

            char confirm[10]; //
            docDongAnToan(confirm, sizeof(confirm), stdin); //
            if (strlen(confirm) > 0 && tolower(confirm[0]) == 'c') { //
                // Gọi hàm xóa đã được cập nhật để xóa khỏi cả mảng và bảng băm
                if (xoaBenhNhan(dsBenhNhan, soLuongBenhNhan, bbBenhNhan, maXoa_str_upper, tenFileCSV)) { //
                    inThongDiepThanhCong("Xoa benh nhan thanh cong!"); //
                }
                else {
                    inThongDiepLoi("Xoa benh nhan that bai (co the khong tim thay hoac loi dong bo bang bam)."); //
                }
            }
            else {
                printf("Huy xoa benh nhan.\n");
            }
            dungManHinhCho(); //
            break;
        }
        case 4: // Xem danh sach benh nhan
            xoaManHinhConsole(); //
            inDanhSachBenhNhanConsole(*dsBenhNhan, *soLuongBenhNhan); //
            dungManHinhCho(); //
            break;
        case 5: { // Tim kiem benh nhan
            char tieuChi[50], giaTri[100]; //
            BenhNhan* bnTimThayQuaBangBam = NULL;       // Cho tìm kiếm bằng bảng băm (1 kết quả)
            BenhNhan* dsKetQuaTimKiemTuyenTinh = NULL; // Cho tìm kiếm tuyến tính (mảng kết quả)
            int soLuongKQTuyenTinh = 0; //

            printf("Tim kiem theo (MaDinhDanh, BHYT, SoDienThoai, Ten): ");
            docDongAnToan(tieuChi, sizeof(tieuChi), stdin); //
            printf("Nhap gia tri can tim: ");
            docDongAnToan(giaTri, sizeof(giaTri), stdin); //

            char tieuChiLower[50]; //
            strncpy(tieuChiLower, tieuChi, sizeof(tieuChiLower) - 1); //
            tieuChiLower[sizeof(tieuChiLower) - 1] = '\0'; //
            for (int i = 0; tieuChiLower[i]; ++i) tieuChiLower[i] = tolower(tieuChiLower[i]); //

            if (strcmp(tieuChiLower, "madinhdanh") == 0 && bbBenhNhan != NULL) { //
                char giaTriMaUpper[100]; //
                strncpy(giaTriMaUpper, giaTri, sizeof(giaTriMaUpper) - 1); //
                giaTriMaUpper[sizeof(giaTriMaUpper) - 1] = '\0'; //
                for (int k = 0; giaTriMaUpper[k]; ++k) giaTriMaUpper[k] = toupper(giaTriMaUpper[k]); //

                bnTimThayQuaBangBam = timBenhNhanTheoMaBangBam(bbBenhNhan, giaTriMaUpper); //
                if (bnTimThayQuaBangBam != NULL) { //
                    // Để nhất quán với cách hiển thị, ta coi đây là một mảng có 1 phần tử
                    dsKetQuaTimKiemTuyenTinh = bnTimThayQuaBangBam; //
                    soLuongKQTuyenTinh = 1; //
                }
                else {
                    soLuongKQTuyenTinh = 0; // Không tìm thấy
                }
            }
            else {
                // Tìm kiếm tuyến tính cho các tiêu chí khác hoặc nếu bbBenhNhan là NULL
                // Hàm timKiemBenhNhan sẽ cấp phát động dsKetQuaTimKiemTuyenTinh nếu có kết quả
                timKiemBenhNhan(*dsBenhNhan, *soLuongBenhNhan, tieuChi, giaTri, &dsKetQuaTimKiemTuyenTinh, &soLuongKQTuyenTinh); //
            }

            xoaManHinhConsole(); //
            if (soLuongKQTuyenTinh > 0) { //
                printf("--- KET QUA TIM KIEM (%d benh nhan) ---\n", soLuongKQTuyenTinh); //
                inDanhSachBenhNhanConsole(dsKetQuaTimKiemTuyenTinh, soLuongKQTuyenTinh); //

                // Chỉ giải phóng bộ nhớ nếu nó được cấp phát bởi timKiemBenhNhan (tìm tuyến tính)
                // và không phải là trường hợp tìm thấy bằng bảng băm (khi đó dsKetQuaTimKiemTuyenTinh
                // chỉ là con trỏ tới phần tử trong mảng dsBenhNhan).
                if (bnTimThayQuaBangBam == NULL && dsKetQuaTimKiemTuyenTinh != NULL) { //
                    free(dsKetQuaTimKiemTuyenTinh); //
                }
            }
            else {
                printf("Khong tim thay benh nhan nao phu hop.\n");
            }
            dungManHinhCho(); //
            break;
        }
        case 0:
            printf("Quay lai menu chinh...\n");
            break;
        default:
            inThongDiepLoi("Lua chon khong hop le."); //
            dungManHinhCho(); //
            break;
        }
    } while (choice != 0); //
}


// CẬP NHẬT: menuQuanLyLanKham
// ui.c

// ... (Các include và hàm tiện ích khác như xoaManHinhConsole, dungManHinhCho, ...)

// CẬP NHẬT: menuQuanLyLanKham
void menuQuanLyLanKham(
    LanKham** dsLanKham, int* soLuongLanKham, BangBam** bbLanKham_ptr,
    BenhNhan* dsBenhNhan, int soLuongBenhNhan, BangBam* bbBenhNhan,
    BacSi* dsBacSi, int soLuongBacSi, BangBam* bbBacSi,
    KhoaPhong* dsKhoaPhong, int soLuongKhoaPhong, BangBam* bbKhoaPhong,
    // Thêm tham số bbHangDoiTheoKhoa để có thể xóa khỏi hàng đợi khi cần
    BangBam* bbHangDoiTheoKhoa,
    const char* fileLanKhamCSV) {

    int choice;

    do {
        xoaManHinhConsole();
        printf("--- QUAN LY THONG TIN CAC LAN KHAM ---\n");
        printf("1. Xem danh sach tat ca lan kham\n");
        printf("2. Tim kiem lan kham (theo MaLK, MaBN, TrangThai, MaBS, MaKP)\n");
        printf("3. Sua chi tiet mot lan kham (nhap MaLK)\n");
        printf("4. Xoa mot lan kham (nhap MaLK)\n");
        printf("0. Quay lai menu chinh\n");

        choice = nhapSoNguyenCoGioiHan("Lua chon cua ban", 0, 4);
        if (choice < 0) { choice = 0; }

        switch (choice) {
        case 1: // Xem danh sách tất cả lần khám
            xoaManHinhConsole();
            inDanhSachLanKhamConsole(*dsLanKham, *soLuongLanKham, dsBenhNhan, soLuongBenhNhan, bbBenhNhan);
            dungManHinhCho();
            break;
        case 2: { // Tìm kiếm lần khám
            char tieuChi[50], giaTri[100];
            LanKham* ketQuaTimKiem = NULL;
            int soLuongKQ = 0;

            printf("Chon tieu chi tim kiem:\n");
            printf("  1. Ma Lan Kham (tim chinh xac bang Bang Bam neu co)\n");
            printf("  2. Ma Dinh Danh Benh Nhan\n");
            printf("  3. Trang Thai (DangCho, DangKham, DaHoanThanh, DaHuy, DaLo)\n");
            printf("  4. Ma Bac Si\n");
            printf("  5. Ma Khoa Phong\n");
            int luaChonTieuChi = nhapSoNguyenCoGioiHan("Lua chon tieu chi", 1, 5);

            if (luaChonTieuChi >= 1 && luaChonTieuChi <= 5) {
                int maLKTimInt;
                LanKham* lkTimThayBangBam = NULL;

                if (luaChonTieuChi == 1) {
                    strcpy(tieuChi, "MaLanKham");
                    printf("Nhap gia tri Ma Lan Kham can tim: ");
                    docDongAnToan(giaTri, sizeof(giaTri), stdin);
                    if (strlen(giaTri) == 0) {
                        inThongDiepLoi("Ma Lan Kham khong duoc de trong.");
                    }
                    else if (sscanf(giaTri, "%d", &maLKTimInt) == 1 && maLKTimInt > 0) {
                        if (*bbLanKham_ptr != NULL) {
                            lkTimThayBangBam = timLanKhamTheoMaBangBam(*bbLanKham_ptr, maLKTimInt);
                            if (lkTimThayBangBam) {
                                ketQuaTimKiem = (LanKham*)malloc(sizeof(LanKham));
                                if (ketQuaTimKiem) {
                                    *ketQuaTimKiem = *lkTimThayBangBam;
                                    soLuongKQ = 1;
                                }
                                else { perror("Loi malloc tim kiem LK (bang bam)"); }
                            }
                        }
                        else {
                            inThongDiepLoi("Bang bam Lan Kham khong kha dung. Su dung tim kiem tuyen tinh.");
                            timKiemLanKham(*dsLanKham, *soLuongLanKham, tieuChi, giaTri, &ketQuaTimKiem, &soLuongKQ);
                        }
                    }
                    else {
                        inThongDiepLoi("Ma Lan Kham nhap vao khong hop le.");
                    }
                }
                else {
                    switch (luaChonTieuChi) {
                    case 2: strcpy(tieuChi, "MaDinhDanhBN"); break;
                    case 3: strcpy(tieuChi, "TrangThai"); break;
                    case 4: strcpy(tieuChi, "BacSi"); break;
                    case 5: strcpy(tieuChi, "MaKhoaPhong"); break;
                    }
                    if (strcmp(tieuChi, "TrangThai") == 0) {
                        printf("Nhap gia tri Trang Thai can tim (vd: DangCho, DaHoanThanh...): ");
                    }
                    else {
                        printf("Nhap gia tri can tim cho '%s': ", tieuChi);
                    }
                    docDongAnToan(giaTri, sizeof(giaTri), stdin);
                    if (strlen(giaTri) > 0) {
                        timKiemLanKham(*dsLanKham, *soLuongLanKham, tieuChi, giaTri, &ketQuaTimKiem, &soLuongKQ);
                    }
                    else {
                        inThongDiepLoi("Gia tri tim kiem khong duoc de trong.");
                    }
                }

                xoaManHinhConsole();
                if (soLuongKQ > 0) {
                    printf("--- KET QUA TIM KIEM (%d lan kham) ---\n", soLuongKQ);
                    inDanhSachLanKhamConsole(ketQuaTimKiem, soLuongKQ, dsBenhNhan, soLuongBenhNhan, bbBenhNhan);
                    if (ketQuaTimKiem) free(ketQuaTimKiem);
                }
                else {
                    printf("Khong tim thay lan kham nao phu hop.\n");
                }
            }
            else {
                inThongDiepLoi("Lua chon tieu chi khong hop le.");
            }
            dungManHinhCho();
            break;
        }
        case 3: { // Sửa chi tiết một lần khám (Nhập MaLK)
            xoaManHinhConsole();
            printf("--- SUA CHI TIET LAN KHAM ---\n");
            if (*soLuongLanKham == 0) {
                printf("Chua co lan kham nao trong he thong de sua.\n");
                dungManHinhCho();
                break;
            }
            int maLKCanSua = nhapSoNguyenCoGioiHan("Nhap Ma Lan Kham can sua", 1, 999999);

            if (maLKCanSua > 0) {
                LanKham* lkSua = NULL;
                if (*bbLanKham_ptr != NULL) {
                    lkSua = timLanKhamTheoMaBangBam(*bbLanKham_ptr, maLKCanSua);
                }
                else {
                    inThongDiepLoi("Bang bam Lan Kham khong kha dung. Khong the tim nhanh.");
                    // Không fallback theo yêu cầu
                }

                if (lkSua != NULL) {
                    // Gọi hàm suaChiTietLanKham, hàm này cần đảm bảo ChanDoan, PPDieuTri không NULL nếu hoàn thành
                    if (suaChiTietLanKham(*dsLanKham, *soLuongLanKham, maLKCanSua,
                        dsBacSi, soLuongBacSi,
                        dsKhoaPhong, soLuongKhoaPhong,
                        bbBacSi, bbKhoaPhong,
                        fileLanKhamCSV)) {
                        // Thông báo thành công đã có trong suaChiTietLanKham
                    }
                    else {
                        // Thông báo lỗi/hủy đã có trong suaChiTietLanKham
                    }
                }
                else {
                    printf("Khong tim thay Lan Kham voi Ma: %d (kiem tra bang bam).\n", maLKCanSua);
                }
            }
            else if (maLKCanSua != (1 - 1)) { // Tránh thông báo lỗi nếu người dùng hủy ở nhapSoNguyenCoGioiHan
                inThongDiepLoi("Ma lan kham nhap vao khong hop le.");
            }
            dungManHinhCho();
            break;
        }
        case 4: { // Xóa Lần Khám (Nhập MaLK)
            xoaManHinhConsole();
            printf("--- XOA LAN KHAM ---\n");
            if (*soLuongLanKham == 0) {
                printf("Chua co lan kham nao de xoa.\n");
                dungManHinhCho();
                break;
            }
            int maLKXoa = nhapSoNguyenCoGioiHan("Nhap Ma Lan Kham can xoa", 1, 999999);
            if (maLKXoa > 0) {
                LanKham* lkTimThay = NULL;
                char maKhoaPhongCuaLKBiXoa[MAX_LEN_MA_KHOA_PHONG] = "";
                TrangThaiLanKham trangThaiLKBiXoa = -1; // Giá trị không hợp lệ ban đầu

                if (*bbLanKham_ptr) {
                    lkTimThay = timLanKhamTheoMaBangBam(*bbLanKham_ptr, maLKXoa);
                    if (lkTimThay) {
                        strncpy(maKhoaPhongCuaLKBiXoa, lkTimThay->maKhoaPhong, sizeof(maKhoaPhongCuaLKBiXoa) - 1);
                        maKhoaPhongCuaLKBiXoa[sizeof(maKhoaPhongCuaLKBiXoa) - 1] = '\0';
                        trangThaiLKBiXoa = lkTimThay->trangThai;
                    }
                }
                else {
                    inThongDiepLoi("Bang bam Lan Kham khong kha dung. Khong the tim nhanh de xoa.");
                    dungManHinhCho();
                    break;
                }

                if (lkTimThay) {
                    printf("Ban co chac chan muon xoa Lan Kham %d (BN: %s, Khoa: %s, TrangThai: %s)? (c/k): ",
                        lkTimThay->maLanKham, lkTimThay->tenBenhNhan, lkTimThay->maKhoaPhong, trangThaiLanKhamToString(lkTimThay->trangThai));
                    char confirm[10];
                    docDongAnToan(confirm, sizeof(confirm), stdin);
                    if (strlen(confirm) > 0 && tolower(confirm[0]) == 'c') {
                        if (xoaLanKham(dsLanKham, soLuongLanKham, *bbLanKham_ptr, maLKXoa, fileLanKhamCSV)) {
                            inThongDiepThanhCong("Xoa Lan Kham khoi danh sach chinh thanh cong!");

                            // Nếu lần khám ở trạng thái đang chờ và có thông tin khoa phòng, xóa khỏi hàng đợi
                            if (trangThaiLKBiXoa == LK_DANG_CHO && strlen(maKhoaPhongCuaLKBiXoa) > 0 && bbHangDoiTheoKhoa != NULL) {
                                // Giả sử bạn có hàm: int xoaMotLanKhamKhoiHangDoiKhoa(BangBam* bbHangDoi, const char* maKP, int maLK);
                                // if (xoaMotLanKhamKhoiHangDoiKhoa(bbHangDoiTheoKhoa, maKhoaPhongCuaLKBiXoa, maLKXoa)) {
                                //     printf("Da xoa Lan Kham %d khoi hang doi khoa %s.\n", maLKXoa, maKhoaPhongCuaLKBiXoa);
                                // } else {
                                //     printf("CANH BAO: Khong tim thay/xoa duoc Lan Kham %d khoi hang doi khoa %s (co the da duoc goi).\n", maLKXoa, maKhoaPhongCuaLKBiXoa);
                                // }
                                // Tạm thời, chúng ta không có hàm xóa một phần tử cụ thể khỏi HangDoiUuTienMotKhoa
                                // mà không phải lấy ra từ đầu. Hàng đợi sẽ tự điều chỉnh khi gọi bệnh nhân tiếp theo
                                // hoặc có thể cần rebuild hàng đợi của khoa đó nếu việc xóa giữa chừng là cần thiết.
                                // Cách đơn giản là không làm gì ở đây, lần gọi tiếp theo sẽ không thấy LK này.
                                // Hoặc, nếu muốn "dọn dẹp" hàng đợi, bạn cần một hàm xóa chuyên biệt.
                                printf("Luu y: Lan Kham %d (trang thai DangCho, khoa %s) da duoc xoa khoi danh sach chinh.\n", maLKXoa, maKhoaPhongCuaLKBiXoa);
                                printf("Hang doi cua khoa se tu dong bo qua lan kham nay khi goi benh nhan.\n");
                            }
                        }
                        else {
                            inThongDiepLoi("Xoa Lan Kham that bai.");
                        }
                    }
                    else {
                        printf("Huy xoa Lan Kham.\n");
                    }
                }
                else {
                    printf("Khong tim thay Lan Kham voi Ma: %d (kiem tra bang bam) de xoa.\n", maLKXoa);
                }
            }
            else if (maLKXoa != (1 - 1)) {
                inThongDiepLoi("Ma lan kham nhap vao khong hop le.");
            }
            dungManHinhCho();
            break;
        }
        case 0:
            printf("Quay lai menu chinh...\n");
            break;
        default:
            inThongDiepLoi("Lua chon khong hop le.");
            dungManHinhCho();
            break;
        }
    } while (choice != 0);
}
// CẬP NHẬT: menuQuanLyBacSi
void menuQuanLyBacSi(
    BacSi** dsBacSi, int* soLuongBacSi,
    BangBam** bbBacSi_ptr,
    const char* tenFileCSV,
    KhoaPhong* dsKhoaPhong, int soLuongKhoaPhong, BangBam* bbKhoaPhong) { // Thêm bbKhoaPhong để xác thực

    BangBam* bbBacSi = *bbBacSi_ptr; // Sử dụng trực tiếp con trỏ BangBam
    int choice;
    char bufferNhap[512]; // Buffer chung cho các thao tác nhập liệu

    do {
        xoaManHinhConsole();
        printf("--- QUAN LY BAC SI ---\n");
        printf("1. Them bac si moi\n");
        printf("2. Sua thong tin bac si \n");
        printf("3. Xoa bac si \n");
        printf("4. Xem danh sach bac si\n");
        printf("5. Tim kiem bac si theo ma bac sic\n");
        printf("6. Tim kiem bac si theo ten, chuyen Khoa, SDT, Email\n");
        printf("0. Quay lai menu chinh\n");
        choice = nhapSoNguyenCoGioiHan("Lua chon cua ban", 0, 6);
        if (choice < 0) { choice = 0; }

        switch (choice) {
        case 1: { // Them bac si moi
            BacSi bsMoi;
            printf("\n--- THEM BAC SI MOI ---\n");
            if (soLuongKhoaPhong == 0) {
                inThongDiepLoi("Chua co khoa phong nao trong he thong. Vui long them khoa phong truoc.");
            }
            else {
                // nhapThongTinBacSiTuBanPhim đã được cập nhật để dùng bbBacSi kiểm tra trùng MaBS
                // và dsBacSiHienCo (chính là *dsBacSi) để kiểm tra trùng SĐT, Email
                if (nhapThongTinBacSiTuBanPhim(&bsMoi, bbBacSi, *dsBacSi, *soLuongBacSi, dsKhoaPhong, soLuongKhoaPhong)) { //
                    // themBacSi nhận BangBam** bbBacSi_ptr
                    if (themBacSi(dsBacSi, soLuongBacSi, bbBacSi_ptr, bsMoi, tenFileCSV)) { //
                        inThongDiepThanhCong("Them bac si moi thanh cong!");
                    }
                    else {
                        inThongDiepLoi("Them bac si moi that bai (co the do loi dong bo hoac he thong).");
                    }
                }
                else {
                    inThongDiepLoi("Thong tin nhap khong hop le (MaBS/Email/SDT trung, hoac truong bat buoc rong, hoac MaKP khong ton tai).");
                }
            }
            dungManHinhCho();
            break;
        }
        case 2: { // Sua thong tin bac si
            char maSua_input[MAX_LEN_MA_BAC_SI];
            printf("Nhap Ma Bac Si can sua: ");
            if (docDongAnToan(maSua_input, sizeof(maSua_input), stdin) == NULL || strlen(maSua_input) == 0) {
                inThongDiepLoi("Ma Bac Si khong duoc de trong.");
                dungManHinhCho();
                break;
            }
            for (int k = 0; maSua_input[k]; ++k) maSua_input[k] = toupper(maSua_input[k]); // Chuẩn hóa mã

            BacSi* bsCanSua = NULL;
            if (bbBacSi != NULL) {
                bsCanSua = timBacSiTheoMaBangBam(bbBacSi, maSua_input); //
            }
            else {
                inThongDiepLoi("Bang bam Bac Si khong kha dung.");
                dungManHinhCho();
                break;
            }
            // KHÔNG fallback sang tìm kiếm tuyến tính theo yêu cầu

            if (bsCanSua != NULL) {
                printf("Tim thay Bac Si: %s (MaBS: %s)\n", bsCanSua->tenBacSi, bsCanSua->maBacSi);
                BacSi bsTamSua = *bsCanSua; // Tạo bản sao để sửa đổi
                int suaLuaChonNoiBo;

                do {
                    xoaManHinhConsole();
                    printf("--- SUA THONG TIN BAC SI (Ma BS: %s) ---\n", bsTamSua.maBacSi);
                    printf("Thong tin hien tai:\n");
                    printf("  1. Ten Bac Si    : %s\n", bsTamSua.tenBacSi);
                    printf("  2. Chuyen Khoa   : %s\n", bsTamSua.chuyenKhoa);
                    printf("  3. So Dien Thoai : %s\n", bsTamSua.soDienThoai);
                    printf("  4. Email         : %s\n", bsTamSua.email);
                    printf("  5. Ma Khoa Phong : %s\n", bsTamSua.maKhoaPhong);
                    printf("-------------------------------------------------\n");
                    printf("Chon thong tin muon sua:\n");
                    printf("  1. Sua Ten Bac Si (KHONG DUOC BO TRONG)\n");
                    printf("  2. Sua Chuyen Khoa (KHONG DUOC BO TRONG)\n");
                    printf("  3. Sua So Dien Thoai (KHONG DUOC BO TRONG, KHONG TRUNG)\n");
                    printf("  4. Sua Email (KHONG DUOC BO TRONG, KHONG TRUNG, dung dinh dang)\n");
                    printf("  5. Sua Ma Khoa Phong (KHONG DUOC BO TRONG, phai ton tai)\n");
                    printf("-------------------------------------------------\n");
                    printf("  0. HOAN TAT SUA DOI VA LUU\n");
                    printf("  9. Huy bo tat ca thay doi va quay lai\n");

                    suaLuaChonNoiBo = nhapSoNguyenCoGioiHan("Lua chon cua ban", 0, 9);
                    if (suaLuaChonNoiBo < 0) { suaLuaChonNoiBo = 9; }

                    switch (suaLuaChonNoiBo) {
                    case 1: // Sửa Tên Bác Sĩ
                        printf("Nhap Ten Bac Si moi (hien tai: %s, KHONG DUOC BO TRONG): ", bsTamSua.tenBacSi);
                        docDongAnToan(bufferNhap, sizeof(bufferNhap), stdin);
                        if (strlen(bufferNhap) > 0) {
                            strncpy(bsTamSua.tenBacSi, bufferNhap, sizeof(bsTamSua.tenBacSi) - 1);
                            bsTamSua.tenBacSi[sizeof(bsTamSua.tenBacSi) - 1] = '\0';
                        }
                        else {
                            inThongDiepLoi("Ten Bac Si khong duoc de trong. Gia tri khong thay doi.");
                        }
                        break;
                    case 2: // Sửa Chuyên Khoa
                        printf("Nhap Chuyen Khoa moi (hien tai: %s, KHONG DUOC BO TRONG): ", bsTamSua.chuyenKhoa);
                        docDongAnToan(bufferNhap, sizeof(bufferNhap), stdin);
                        if (strlen(bufferNhap) > 0) {
                            strncpy(bsTamSua.chuyenKhoa, bufferNhap, sizeof(bsTamSua.chuyenKhoa) - 1);
                            bsTamSua.chuyenKhoa[sizeof(bsTamSua.chuyenKhoa) - 1] = '\0';
                        }
                        else {
                            inThongDiepLoi("Chuyen Khoa khong duoc de trong. Gia tri khong thay doi.");
                        }
                        break;
                    case 3: // Sửa Số Điện Thoại
                        printf("Nhap So Dien Thoai moi (hien tai: %s, KHONG DUOC BO TRONG, chi chua so, KHONG TRUNG): ", bsTamSua.soDienThoai);
                        docDongAnToan(bufferNhap, sizeof(bufferNhap), stdin);
                        if (strlen(bufferNhap) > 0) {
                            // Kiểm tra chỉ chứa số
                            int chiChuaSo = 1;
                            for (int i = 0; bufferNhap[i]; ++i) if (!isdigit(bufferNhap[i])) { chiChuaSo = 0; break; }
                            if (!chiChuaSo) {
                                inThongDiepLoi("So Dien Thoai moi chi duoc chua so. Gia tri khong thay doi.");
                                break;
                            }
                            // Kiểm tra trùng với bác sĩ KHÁC
                            BacSi* bsTrungSDT = timBacSiTheoSDT(*dsBacSi, *soLuongBacSi, bufferNhap); //
                            if (bsTrungSDT != NULL && strcmp(bsTrungSDT->maBacSi, bsTamSua.maBacSi) != 0) {
                                inThongDiepLoi("So Dien Thoai moi da ton tai cho bac si khac. Gia tri khong thay doi.");
                            }
                            else {
                                strncpy(bsTamSua.soDienThoai, bufferNhap, sizeof(bsTamSua.soDienThoai) - 1);
                                bsTamSua.soDienThoai[sizeof(bsTamSua.soDienThoai) - 1] = '\0';
                            }
                        }
                        else {
                            inThongDiepLoi("So Dien Thoai khong duoc de trong. Gia tri khong thay doi.");
                        }
                        break;
                    case 4: // Sửa Email
                        printf("Nhap Email moi (hien tai: %s, KHONG DUOC BO TRONG, dung dinh dang, KHONG TRUNG): ", bsTamSua.email);
                        docDongAnToan(bufferNhap, sizeof(bufferNhap), stdin);
                        if (strlen(bufferNhap) > 0) {
                            if (strchr(bufferNhap, '@') == NULL || strchr(bufferNhap, '.') == NULL) {
                                inThongDiepLoi("Dinh dang Email moi khong hop le. Gia tri khong thay doi.");
                                break;
                            }
                            BacSi* bsTrungEmail = timBacSiTheoEmail(*dsBacSi, *soLuongBacSi, bufferNhap); //
                            if (bsTrungEmail != NULL && strcmp(bsTrungEmail->maBacSi, bsTamSua.maBacSi) != 0) {
                                inThongDiepLoi("Email moi da ton tai cho bac si khac. Gia tri khong thay doi.");
                            }
                            else {
                                strncpy(bsTamSua.email, bufferNhap, sizeof(bsTamSua.email) - 1);
                                bsTamSua.email[sizeof(bsTamSua.email) - 1] = '\0';
                            }
                        }
                        else {
                            inThongDiepLoi("Email khong duoc de trong. Gia tri khong thay doi.");
                        }
                        break;
                    case 5: // Sửa Mã Khoa Phòng
                        printf("Nhap Ma Khoa Phong moi (hien tai: %s, KHONG DUOC BO TRONG, phai ton tai, IN HOA): ", bsTamSua.maKhoaPhong);
                        if (soLuongKhoaPhong > 0 && dsKhoaPhong != NULL) {
                            printf("Cac ma khoa phong hien co:\n");
                            inDanhSachKhoaPhongConsole(dsKhoaPhong, soLuongKhoaPhong); //
                        }
                        else {
                            printf("Khong co du lieu khoa phong de tham khao.\n");
                        }
                        docDongAnToan(bufferNhap, sizeof(bufferNhap), stdin);
                        if (strlen(bufferNhap) > 0) {
                            for (int k = 0; bufferNhap[k]; ++k) bufferNhap[k] = toupper(bufferNhap[k]);
                            KhoaPhong* kpTim = bbKhoaPhong ? timKhoaPhongTheoMaBangBam(bbKhoaPhong, bufferNhap) : timKhoaPhongTheoMa(dsKhoaPhong, soLuongKhoaPhong, bufferNhap); //
                            if (kpTim != NULL) {
                                strncpy(bsTamSua.maKhoaPhong, kpTim->maKhoaPhong, sizeof(bsTamSua.maKhoaPhong) - 1);
                                bsTamSua.maKhoaPhong[sizeof(bsTamSua.maKhoaPhong) - 1] = '\0';
                            }
                            else {
                                inThongDiepLoi("Ma Khoa Phong moi khong ton tai. Gia tri khong thay doi.");
                            }
                        }
                        else {
                            inThongDiepLoi("Ma Khoa Phong khong duoc de trong. Gia tri khong thay doi.");
                        }
                        break;
                    case 0: { // HOAN TAT SUA DOI VA LUU
                        // Kiểm tra lại lần cuối các trường bắt buộc
                        if (strlen(bsTamSua.tenBacSi) == 0 || strlen(bsTamSua.chuyenKhoa) == 0 ||
                            strlen(bsTamSua.soDienThoai) == 0 || strlen(bsTamSua.email) == 0 ||
                            strlen(bsTamSua.maKhoaPhong) == 0) {
                            inThongDiepLoi("LOI KHI LUU: Mot hoac nhieu truong thong tin bat buoc bi de trong. Vui long sua lai.");
                            suaLuaChonNoiBo = -1; // Để vòng lặp tiếp tục
                            dungManHinhCho();
                            break;
                        }
                        // MaBS của bsTamSua đã được giữ nguyên từ bsCanSua
                        if (suaThongTinBacSi(*dsBacSi, *soLuongBacSi, bsCanSua->maBacSi, bsTamSua, dsKhoaPhong, soLuongKhoaPhong, tenFileCSV)) { //
                            // Vì bsCanSua là con trỏ trỏ tới phần tử trong mảng *dsBacSi,
                            // hàm suaThongTinBacSi sẽ cập nhật trực tiếp.
                            inThongDiepThanhCong("Da luu cac thay doi cho bac si!");
                        }
                        else {
                            inThongDiepLoi("Loi khi luu thay doi cho bac si (VD: SDT/Email moi bi trung voi BS khac).");
                            // Không thoát vòng lặp, cho người dùng sửa lại
                            suaLuaChonNoiBo = -1;
                        }
                        // Nếu thành công, suaLuaChonNoiBo đã là 0, vòng lặp sẽ tự thoát.
                        break;
                    }
                    case 9:
                        inThongDiepLoi("Da huy bo sua doi.");
                        suaLuaChonNoiBo = 0; // Để thoát vòng lặp
                        break;
                    default:
                        inThongDiepLoi("Lua chon khong hop le.");
                        break;
                    }
                    if (suaLuaChonNoiBo >= 1 && suaLuaChonNoiBo <= 5) {
                        inThongDiepThanhCong("Da cap nhat truong. Chon truong khac hoac Luu/Huy.");
                        dungManHinhCho();
                    }
                } while (suaLuaChonNoiBo != 0);
            }
            else {
                inThongDiepLoi("Khong tim thay bac si voi ma da nhap trong bang bam.");
            }
            dungManHinhCho();
            break;
        }
        case 3: { // Xoa bac si
            char maXoa_input[MAX_LEN_MA_BAC_SI];
            printf("Nhap Ma Bac Si can xoa: ");
            if (docDongAnToan(maXoa_input, sizeof(maXoa_input), stdin) == NULL || strlen(maXoa_input) == 0) {
                inThongDiepLoi("Ma Bac Si khong duoc de trong.");
                dungManHinhCho();
                break;
            }
            for (int k = 0; maXoa_input[k]; ++k) maXoa_input[k] = toupper(maXoa_input[k]);

            BacSi* bsCanXoa = NULL;
            if (bbBacSi) {
                bsCanXoa = timBacSiTheoMaBangBam(bbBacSi, maXoa_input); //
            }

            if (bsCanXoa != NULL) {
                printf("Ban co chac chan muon xoa bac si: %s (Ma BS: %s)? (c/k): ", bsCanXoa->tenBacSi, bsCanXoa->maBacSi);
                char confirm[10];
                docDongAnToan(confirm, sizeof(confirm), stdin);
                if (strlen(confirm) > 0 && tolower(confirm[0]) == 'c') {
                    // xoaBacSi nhận BangBam* (không phải con trỏ kép)
                    if (xoaBacSi(dsBacSi, soLuongBacSi, bbBacSi, maXoa_input, tenFileCSV)) { //
                        inThongDiepThanhCong("Xoa bac si thanh cong!");
                    }
                    else {
                        inThongDiepLoi("Xoa bac si that bai (co the do loi dong bo hoac khong tim thay trong mang).");
                    }
                }
                else {
                    printf("Huy xoa bac si.\n");
                }
            }
            else {
                inThongDiepLoi("Khong tim thay bac si voi Ma BS da nhap trong bang bam.");
            }
            dungManHinhCho();
            break;
        }
        case 4: // Xem danh sách bác sĩ
            xoaManHinhConsole();
            inDanhSachBacSiConsole(*dsBacSi, *soLuongBacSi); //
            dungManHinhCho();
            break;
        case 5: { // Tìm kiếm bác sĩ theo Mã BS (chỉ dùng bảng băm)
            char maTim_input[MAX_LEN_MA_BAC_SI];
            printf("Nhap Ma Bac Si can tim: ");
            if (docDongAnToan(maTim_input, sizeof(maTim_input), stdin) == NULL || strlen(maTim_input) == 0) {
                inThongDiepLoi("Ma Bac Si khong duoc de trong.");
                dungManHinhCho();
                break;
            }
            for (int k = 0; maTim_input[k]; ++k) maTim_input[k] = toupper(maTim_input[k]);

            BacSi* bsTim = NULL;
            if (bbBacSi) {
                bsTim = timBacSiTheoMaBangBam(bbBacSi, maTim_input); //
            }
            else {
                inThongDiepLoi("Bang bam Bac Si khong kha dung.");
                dungManHinhCho();
                break;
            }

            xoaManHinhConsole();
            if (bsTim) {
                printf("--- THONG TIN BAC SI TIM THAY ---\n");
                printf("Ma BS         : %s\n", bsTim->maBacSi);
                printf("Ten Bac Si    : %s\n", bsTim->tenBacSi);
                printf("Chuyen Khoa   : %s\n", bsTim->chuyenKhoa);
                printf("So Dien Thoai : %s\n", bsTim->soDienThoai);
                printf("Email         : %s\n", bsTim->email);
                printf("Ma Khoa Phong : %s\n", bsTim->maKhoaPhong);
            }
            else {
                printf("Khong tim thay bac si voi ma '%s' trong bang bam.\n", maTim_input);
            }
            dungManHinhCho();
            break;
        }
        case 6: { // TÌM KIẾM TUYẾN TÍNH MỚI
            char tieuChiTimKiem[50];
            char giaTriTimKiem[100];
            BacSi* ketQua = NULL;
            int soLuongKQ = 0;

            printf("--- TIM KIEM BAC SI ---\n");
            printf("Chon tieu chi tim kiem:\n");
            printf("  1. Ten Bac Si (tim kiem chua)\n");
            printf("  2. Chuyen Khoa (tim kiem chua)\n");
            printf("  3. So Dien Thoai (khop chinh xac)\n");
            printf("  4. Email (khop chinh xac, khong phan biet hoa thuong)\n");
            int luaChonTieuChi = nhapSoNguyenCoGioiHan("Lua chon tieu chi", 1, 4);

            if (luaChonTieuChi < 1 || luaChonTieuChi > 4) {
                inThongDiepLoi("Lua chon tieu chi khong hop le.");
                dungManHinhCho();
                break;
            }

            switch (luaChonTieuChi) {
            case 1: strcpy(tieuChiTimKiem, "Ten"); break;
            case 2: strcpy(tieuChiTimKiem, "ChuyenKhoa"); break;
            case 3: strcpy(tieuChiTimKiem, "SoDienThoai"); break;
            case 4: strcpy(tieuChiTimKiem, "Email"); break;
            }

            printf("Nhap gia tri can tim cho '%s': ", tieuChiTimKiem);
            if (docDongAnToan(giaTriTimKiem, sizeof(giaTriTimKiem), stdin) == NULL || strlen(giaTriTimKiem) == 0) {
                inThongDiepLoi("Gia tri tim kiem khong duoc de trong.");
            }
            else {
                timKiemBacSiTuyenTinh(*dsBacSi, *soLuongBacSi, tieuChiTimKiem, giaTriTimKiem, &ketQua, &soLuongKQ);
                xoaManHinhConsole();
                if (soLuongKQ > 0) {
                    printf("--- KET QUA TIM KIEM (%d bac si) ---\n", soLuongKQ);
                    inDanhSachBacSiConsole(ketQua, soLuongKQ);
                    free(ketQua); // Giải phóng bộ nhớ được cấp phát bởi timKiemBacSiTuyenTinh
                }
                else {
                    printf("Khong tim thay bac si nao phu hop voi tieu chi '%s' va gia tri '%s'.\n", tieuChiTimKiem, giaTriTimKiem);
                }
            }
            dungManHinhCho();
            break;
        }
        case 0:
            printf("Quay lai menu chinh...\n");
            break;
        default:
            inThongDiepLoi("Lua chon khong hop le.");
            dungManHinhCho();
            break;
        }
    } while (choice != 0);
}

// CẬP NHẬT: menuQuanLyThuoc
void menuQuanLyThuoc(Thuoc** dsThuoc, int* soLuongThuoc, BangBam** bbThuoc_ptr, const char* tenFileCSV) {
    BangBam* bbThuoc = *bbThuoc_ptr; // Để sử dụng trực tiếp con trỏ BangBam
    int choice;
    char bufferNhap[512]; // Buffer chung cho các thao tác nhập liệu

    do {
        xoaManHinhConsole();
        printf("--- QUAN LY DANH MUC THUOC ---\n");
        printf("1. Them thuoc moi\n");
        printf("2. Sua thong tin thuoc \n");
        printf("3. Xoa thuoc \n");
        printf("4. Xem danh sach thuoc\n");
        printf("5. Tim kiem thuoc theo ma thuoc\n");
        printf("6. Tim kiem thuoc theo Ten Thuoc\n"); // MỤC MỚI

        printf("0. Quay lai menu chinh\n");
        choice = nhapSoNguyenCoGioiHan("Lua chon cua ban", 0, 6);
        if (choice < 0) choice = 0; // Xử lý lỗi trả về từ nhapSoNguyenCoGioiHan

        switch (choice) {
        case 1: { // Them thuoc moi
            Thuoc thuocMoi;
            printf("\n--- THEM THUOC MOI ---\n");
            // nhapThongTinThuocTuBanPhim đã được cập nhật để:
            // - Yêu cầu MaThuoc, TenThuoc không rỗng.
            // - Kiểm tra trùng MaThuoc bằng bbThuoc.
            // - Kiểm tra trùng TenThuoc bằng cách duyệt mảng *dsThuoc.
            if (nhapThongTinThuocTuBanPhim(&thuocMoi, bbThuoc, *dsThuoc, *soLuongThuoc)) { //
                // themThuoc nhận BangBam** bbThuoc_ptr
                if (themThuoc(dsThuoc, soLuongThuoc, bbThuoc_ptr, thuocMoi, tenFileCSV)) { //
                    inThongDiepThanhCong("Them thuoc moi thanh cong!");
                }
                else {
                    inThongDiepLoi("Them thuoc moi that bai (co the do loi he thong).");
                }
            }
            else {
                inThongDiepLoi("Thong tin nhap khong hop le (MaThuoc/TenThuoc trung, hoac truong bat buoc rong).");
            }
            dungManHinhCho();
            break;
        }
        case 2: { // Sua thong tin thuoc
            char maSua_input[MAX_LEN_MA_THUOC];
            printf("Nhap Ma Thuoc can sua: ");
            if (docDongAnToan(maSua_input, sizeof(maSua_input), stdin) == NULL || strlen(maSua_input) == 0) {
                inThongDiepLoi("Ma Thuoc khong duoc de trong.");
                dungManHinhCho();
                break;
            }
            for (int k = 0; maSua_input[k]; ++k) maSua_input[k] = toupper(maSua_input[k]); // Chuẩn hóa mã

            Thuoc* thuocCanSua = NULL;
            if (bbThuoc != NULL) {
                thuocCanSua = timThuocTheoMaBangBam(bbThuoc, maSua_input); //
            }
            else {
                inThongDiepLoi("Bang bam Thuoc khong kha dung.");
                dungManHinhCho();
                break;
            }
            // KHÔNG fallback sang tìm kiếm tuyến tính theo yêu cầu

            if (thuocCanSua) {
                printf("Tim thay Thuoc: %s (Ma: %s)\n", thuocCanSua->tenThuoc, thuocCanSua->maThuoc);
                Thuoc thongTinMoi;
                // Mã thuốc không đổi, chỉ cho sửa tên thuốc
                strcpy(thongTinMoi.maThuoc, thuocCanSua->maThuoc);

                printf("Ten thuoc hien tai: %s\n", thuocCanSua->tenThuoc);
                printf("Nhap Ten Thuoc moi (KHONG DUOC BO TRONG, KHONG TRUNG voi thuoc khac): ");
                if (docDongAnToan(thongTinMoi.tenThuoc, sizeof(thongTinMoi.tenThuoc), stdin) == NULL || strlen(thongTinMoi.tenThuoc) == 0) {
                    inThongDiepLoi("Ten thuoc moi khong duoc de trong. Huy sua.");
                }
                else {
                    // Hàm suaThongTinThuoc (trong medicine.c) sẽ kiểm tra trùng tên mới
                    if (suaThongTinThuoc(*dsThuoc, *soLuongThuoc, maSua_input, thongTinMoi, tenFileCSV)) { //
                        inThongDiepThanhCong("Sua thong tin thuoc thanh cong!");
                    }
                    else {
                        inThongDiepLoi("Sua thong tin thuoc that bai (VD: Ten thuoc moi da ton tai).");
                    }
                }
            }
            else {
                inThongDiepLoi("Khong tim thay thuoc voi ma da nhap trong bang bam.");
            }
            dungManHinhCho();
            break;
        }
        case 3: { // Xoa Thuoc
            char maXoa_input[MAX_LEN_MA_THUOC];
            printf("Nhap Ma Thuoc can xoa: ");
            if (docDongAnToan(maXoa_input, sizeof(maXoa_input), stdin) == NULL || strlen(maXoa_input) == 0) {
                inThongDiepLoi("Ma Thuoc khong duoc de trong.");
                dungManHinhCho();
                break;
            }
            for (int k = 0; maXoa_input[k]; ++k) maXoa_input[k] = toupper(maXoa_input[k]);

            Thuoc* thuocCanXoa = NULL;
            if (bbThuoc) {
                thuocCanXoa = timThuocTheoMaBangBam(bbThuoc, maXoa_input); //
            }
            else {
                inThongDiepLoi("Bang bam Thuoc khong kha dung.");
                dungManHinhCho();
                break;
            }
            // KHÔNG fallback

            if (thuocCanXoa) {
                printf("Ban co chac muon xoa thuoc: %s (Ma: %s)? (c/k): ", thuocCanXoa->tenThuoc, thuocCanXoa->maThuoc);
                char confirm[10];
                docDongAnToan(confirm, sizeof(confirm), stdin);
                if (strlen(confirm) > 0 && tolower(confirm[0]) == 'c') {
                    // xoaThuoc nhận BangBam* (không phải con trỏ kép)
                    if (xoaThuoc(dsThuoc, soLuongThuoc, bbThuoc, maXoa_input, tenFileCSV)) { //
                        inThongDiepThanhCong("Xoa thuoc thanh cong!");
                        // TODO: Cảnh báo nếu thuốc này đang được sử dụng trong đơn thuốc chi tiết.
                        // Việc này cần duyệt qua dsDonThuocChiTiet.
                    }
                    else {
                        inThongDiepLoi("Xoa thuoc that bai (co the do loi dong bo hoac khong tim thay trong mang).");
                    }
                }
                else {
                    printf("Huy xoa thuoc.\n");
                }
            }
            else {
                inThongDiepLoi("Khong tim thay thuoc voi ma da nhap trong bang bam.");
            }
            dungManHinhCho();
            break;
        }
        case 4: // Xem danh sách thuốc
            xoaManHinhConsole();
            inDanhSachThuocConsole(*dsThuoc, *soLuongThuoc); //
            dungManHinhCho();
            break;
        case 5: { // Tìm kiếm thuốc theo Mã Thuốc (chỉ dùng bảng băm)
            char maTim_input[MAX_LEN_MA_THUOC];
            printf("Nhap Ma Thuoc can tim: ");
            if (docDongAnToan(maTim_input, sizeof(maTim_input), stdin) == NULL || strlen(maTim_input) == 0) {
                inThongDiepLoi("Ma Thuoc khong duoc de trong.");
                dungManHinhCho();
                break;
            }
            for (int k = 0; maTim_input[k]; ++k) maTim_input[k] = toupper(maTim_input[k]);

            Thuoc* thuocTimThay = NULL;
            if (bbThuoc) {
                thuocTimThay = timThuocTheoMaBangBam(bbThuoc, maTim_input); //
            }
            else {
                inThongDiepLoi("Bang bam Thuoc khong kha dung.");
                dungManHinhCho();
                break;
            }
            // KHÔNG fallback

            xoaManHinhConsole();
            if (thuocTimThay) {
                printf("--- THONG TIN THUOC TIM THAY ---\n");
                printf("Ma Thuoc: %s\n", thuocTimThay->maThuoc);
                printf("Ten Thuoc: %s\n", thuocTimThay->tenThuoc);
            }
            else {
                printf("Khong tim thay thuoc voi ma '%s' trong bang bam.\n", maTim_input);
            }
            dungManHinhCho();
            break;
        }
        case 6: { // TÌM KIẾM THEO TÊN THUỐC (TUYẾN TÍNH)
            char tenThuocTim[sizeof(((Thuoc*)0)->tenThuoc)]; // Lấy kích thước từ struct
            Thuoc* ketQua = NULL;
            int soLuongKQ = 0;

            printf("--- TIM KIEM THUOC THEO TEN (TUYEN TINH, CHUA) ---\n");
            printf("Nhap Ten Thuoc can tim: ");
            if (docDongAnToan(tenThuocTim, sizeof(tenThuocTim), stdin) == NULL || strlen(tenThuocTim) == 0) {
                inThongDiepLoi("Ten thuoc tim kiem khong duoc de trong.");
            }
            else {
                timKiemThuocTheoTenTuyenTinh(*dsThuoc, *soLuongThuoc, tenThuocTim, &ketQua, &soLuongKQ);
                xoaManHinhConsole();
                if (soLuongKQ > 0) {
                    printf("--- KET QUA TIM KIEM (%d thuoc) ---\n", soLuongKQ);
                    inDanhSachThuocConsole(ketQua, soLuongKQ);
                    free(ketQua); // Giải phóng bộ nhớ
                }
                else {
                    printf("Khong tim thay thuoc nao co ten chua '%s'.\n", tenThuocTim);
                }
            }
            dungManHinhCho();
            break;
        }
        case 0:
            printf("Quay lai menu chinh...\n");
            break;
        default:
            inThongDiepLoi("Lua chon khong hop le.");
            dungManHinhCho();
            break;
        }
    } while (choice != 0);
}


// CẬP NHẬT: menuQuanLyKhoaPhong
// ui.c

// ... (Các include và hàm tiện ích khác như xoaManHinhConsole, dungManHinhCho, ...)

// CẬP NHẬT: menuQuanLyKhoaPhong
void menuQuanLyKhoaPhong(KhoaPhong** dsKhoaPhong, int* soLuongKhoaPhong, BangBam** bbKhoaPhong_ptr, const char* tenFileCSV) {
    BangBam* bbKhoaPhong = *bbKhoaPhong_ptr; // Để sử dụng trực tiếp con trỏ BangBam
    int choice;
    char bufferNhap[512]; // Buffer chung

    do {
        xoaManHinhConsole();
        printf("--- QUAN LY KHOA PHONG ---\n");
        printf("1. Them khoa phong moi\n");
        printf("2. Sua thong tin khoa phong \n");
        printf("3. Xoa khoa phong bang ma khoa phong \n");
        printf("4. Xem danh sach khoa phong\n");
        printf("5. Tim kiem khoa phong theo ma khoa phong\n");
        printf("6. Tim kiem khoa phong theo ten khoa phong, mo ta, vi tri\n"); // MỤC MỚI

        printf("0. Quay lai menu chinh\n");
        choice = nhapSoNguyenCoGioiHan("Lua chon cua ban", 0, 6);
        if (choice < 0) choice = 0; // Xử lý lỗi trả về từ nhapSoNguyenCoGioiHan

        switch (choice) {
        case 1: { // Them khoa phong moi
            KhoaPhong kpMoi;
            printf("\n--- THEM KHOA PHONG MOI ---\n");
            // nhapThongTinKhoaPhongTuBanPhim đã được cập nhật để:
            // - Yêu cầu MaKP, TenKP, ViTri không rỗng.
            // - Kiểm tra trùng MaKP bằng bbKhoaPhong.
            // - Kiểm tra trùng ViTri (nếu MaKP khác) bằng cách duyệt mảng *dsKhoaPhong.
            // - TenKP được phép trùng.
            if (nhapThongTinKhoaPhongTuBanPhim(&kpMoi, bbKhoaPhong, *dsKhoaPhong, *soLuongKhoaPhong)) { //
                // themKhoaPhong nhận BangBam** bbKhoaPhong_ptr
                if (themKhoaPhong(dsKhoaPhong, soLuongKhoaPhong, bbKhoaPhong_ptr, kpMoi, tenFileCSV)) { //
                    inThongDiepThanhCong("Them khoa phong moi thanh cong!");
                }
                else {
                    inThongDiepLoi("Them khoa phong moi that bai (co the do loi he thong).");
                }
            }
            else {
                inThongDiepLoi("Thong tin nhap khong hop le (MaKP trung, ViTri trung voi MaKP khac, hoac truong bat buoc rong).");
            }
            dungManHinhCho();
            break;
        }
        case 2: { // Sua thong tin khoa phong
            char maSua_input[MAX_LEN_MA_KHOA_PHONG];
            printf("Nhap Ma Khoa Phong can sua: ");
            if (docDongAnToan(maSua_input, sizeof(maSua_input), stdin) == NULL || strlen(maSua_input) == 0) {
                inThongDiepLoi("Ma Khoa Phong khong duoc de trong.");
                dungManHinhCho();
                break;
            }
            for (int k = 0; maSua_input[k]; ++k) maSua_input[k] = toupper(maSua_input[k]); // Chuẩn hóa mã

            KhoaPhong* kpCanSua = NULL;
            if (bbKhoaPhong != NULL) {
                kpCanSua = timKhoaPhongTheoMaBangBam(bbKhoaPhong, maSua_input); //
            }
            else {
                inThongDiepLoi("Bang bam Khoa Phong khong kha dung.");
                dungManHinhCho();
                break;
            }
            // KHÔNG fallback sang tìm kiếm tuyến tính theo yêu cầu

            if (kpCanSua) {
                printf("Tim thay Khoa Phong: %s (Ma: %s)\n", kpCanSua->tenKhoaPhong, kpCanSua->maKhoaPhong);
                KhoaPhong thongTinMoi = *kpCanSua; // Tạo bản sao để sửa, giữ lại mã gốc
                int suaLuaChonNoiBo;

                do {
                    xoaManHinhConsole();
                    printf("--- SUA THONG TIN KHOA PHONG (Ma KP: %s) ---\n", thongTinMoi.maKhoaPhong);
                    printf("Thong tin hien tai:\n");
                    printf("  1. Ten Khoa Phong: %s\n", thongTinMoi.tenKhoaPhong);
                    printf("  2. Vi Tri        : %s\n", thongTinMoi.viTri);
                    printf("  3. Mo Ta         : %s\n", thongTinMoi.moTa);
                    printf("-------------------------------------------------\n");
                    printf("Chon thong tin muon sua:\n");
                    printf("  1. Sua Ten Khoa Phong (KHONG DUOC BO TRONG)\n");
                    printf("  2. Sua Vi Tri (KHONG DUOC BO TRONG, KHONG TRUNG voi KP khac)\n");
                    printf("  3. Sua Mo Ta (neu bo trong se la 'Khong co mo ta')\n");
                    printf("-------------------------------------------------\n");
                    printf("  0. HOAN TAT SUA DOI VA LUU\n");
                    printf("  9. Huy bo tat ca thay doi va quay lai\n");

                    suaLuaChonNoiBo = nhapSoNguyenCoGioiHan("Lua chon cua ban", 0, 9);
                    if (suaLuaChonNoiBo < 0) { suaLuaChonNoiBo = 9; }

                    switch (suaLuaChonNoiBo) {
                    case 1: // Sửa Tên Khoa Phòng
                        printf("Nhap Ten Khoa Phong moi (hien tai: %s, KHONG DUOC BO TRONG): ", thongTinMoi.tenKhoaPhong);
                        docDongAnToan(bufferNhap, sizeof(bufferNhap), stdin);
                        if (strlen(bufferNhap) > 0) {
                            // Tên được phép trùng, không cần kiểm tra trùng ở đây
                            strncpy(thongTinMoi.tenKhoaPhong, bufferNhap, sizeof(thongTinMoi.tenKhoaPhong) - 1);
                            thongTinMoi.tenKhoaPhong[sizeof(thongTinMoi.tenKhoaPhong) - 1] = '\0';
                        }
                        else {
                            inThongDiepLoi("Ten Khoa Phong khong duoc de trong. Gia tri khong thay doi.");
                        }
                        break;
                    case 2: // Sửa Vị Trí
                        printf("Nhap Vi Tri moi (hien tai: %s, KHONG DUOC BO TRONG, KHONG TRUNG voi KP khac): ", thongTinMoi.viTri);
                        docDongAnToan(bufferNhap, sizeof(bufferNhap), stdin);
                        if (strlen(bufferNhap) > 0) {
                            // Kiểm tra trùng vị trí với các khoa phòng KHÁC
                            int viTriTrung = 0;
                            for (int i = 0; i < *soLuongKhoaPhong; ++i) {
                                // So sánh với các khoa phòng khác (không phải khoa đang sửa)
                                if (strcmp((*dsKhoaPhong)[i].maKhoaPhong, thongTinMoi.maKhoaPhong) != 0 &&
                                    strcmp((*dsKhoaPhong)[i].viTri, bufferNhap) == 0) {
                                    viTriTrung = 1;
                                    inThongDiepLoi("Vi Tri moi da ton tai cho Khoa Phong khac. Gia tri khong thay doi.");
                                    break;
                                }
                            }
                            if (!viTriTrung) {
                                strncpy(thongTinMoi.viTri, bufferNhap, sizeof(thongTinMoi.viTri) - 1);
                                thongTinMoi.viTri[sizeof(thongTinMoi.viTri) - 1] = '\0';
                            }
                        }
                        else {
                            inThongDiepLoi("Vi Tri khong duoc de trong. Gia tri khong thay doi.");
                        }
                        break;
                    case 3: // Sửa Mô Tả
                        printf("Nhap Mo Ta moi (hien tai: %s, neu bo trong se la 'Khong co mo ta'): ", thongTinMoi.moTa);
                        docDongAnToan(bufferNhap, sizeof(bufferNhap), stdin);
                        if (strlen(bufferNhap) > 0) {
                            strncpy(thongTinMoi.moTa, bufferNhap, sizeof(thongTinMoi.moTa) - 1);
                            thongTinMoi.moTa[sizeof(thongTinMoi.moTa) - 1] = '\0';
                        }
                        else {
                            strcpy(thongTinMoi.moTa, "Khong co mo ta");
                        }
                        break;
                    case 0: { // HOAN TAT SUA DOI VA LUU
                        // Kiểm tra lại các trường bắt buộc
                        if (strlen(thongTinMoi.tenKhoaPhong) == 0 || strlen(thongTinMoi.viTri) == 0) {
                            inThongDiepLoi("LOI KHI LUU: Ten Khoa Phong hoac Vi Tri khong duoc de trong. Vui long sua lai.");
                            suaLuaChonNoiBo = -1; // Để vòng lặp tiếp tục
                            dungManHinhCho();
                            break;
                        }
                        // MaKhoaPhong của thongTinMoi đã được giữ nguyên từ kpCanSua
                        if (suaThongTinKhoaPhong(*dsKhoaPhong, *soLuongKhoaPhong, kpCanSua->maKhoaPhong, thongTinMoi, tenFileCSV)) { //
                            inThongDiepThanhCong("Da luu cac thay doi cho khoa phong!");
                        }
                        else {
                            inThongDiepLoi("Loi khi luu thay doi cho khoa phong (VD: Vi Tri moi bi trung).");
                            // Cho phép người dùng sửa lại
                            suaLuaChonNoiBo = -1;
                        }
                        break;
                    }
                    case 9:
                        inThongDiepLoi("Da huy bo sua doi.");
                        suaLuaChonNoiBo = 0; // Để thoát vòng lặp
                        break;
                    default:
                        inThongDiepLoi("Lua chon khong hop le.");
                        break;
                    }
                    if (suaLuaChonNoiBo >= 1 && suaLuaChonNoiBo <= 3) {
                        inThongDiepThanhCong("Da cap nhat truong. Chon truong khac hoac Luu/Huy.");
                        dungManHinhCho();
                    }
                } while (suaLuaChonNoiBo != 0);
            }
            else {
                inThongDiepLoi("Khong tim thay khoa phong voi ma da nhap trong bang bam.");
            }
            dungManHinhCho();
            break;
        }
        case 3: { // Xoa Khoa Phong
            char maXoa_input[MAX_LEN_MA_KHOA_PHONG];
            printf("Nhap Ma Khoa Phong can xoa: ");
            if (docDongAnToan(maXoa_input, sizeof(maXoa_input), stdin) == NULL || strlen(maXoa_input) == 0) {
                inThongDiepLoi("Ma Khoa Phong khong duoc de trong.");
                dungManHinhCho();
                break;
            }
            for (int k = 0; maXoa_input[k]; ++k) maXoa_input[k] = toupper(maXoa_input[k]);

            KhoaPhong* kpCanXoa = NULL;
            if (bbKhoaPhong) {
                kpCanXoa = timKhoaPhongTheoMaBangBam(bbKhoaPhong, maXoa_input); //
            }
            else {
                inThongDiepLoi("Bang bam Khoa Phong khong kha dung.");
                dungManHinhCho();
                break;
            }
            // KHÔNG fallback

            if (kpCanXoa) {
                printf("Ban co chac muon xoa Khoa Phong: %s (Ma: %s, Vi Tri: %s)? (c/k): ", kpCanXoa->tenKhoaPhong, kpCanXoa->maKhoaPhong, kpCanXoa->viTri);
                char confirm[10];
                docDongAnToan(confirm, sizeof(confirm), stdin);
                if (strlen(confirm) > 0 && tolower(confirm[0]) == 'c') {
                    // xoaKhoaPhong nhận BangBam* (không phải con trỏ kép)
                    if (xoaKhoaPhong(dsKhoaPhong, soLuongKhoaPhong, bbKhoaPhong, maXoa_input, tenFileCSV)) { //
                        inThongDiepThanhCong("Xoa khoa phong thanh cong!");
                        // TODO: Cảnh báo/Ngăn chặn nếu khoa phòng này đang được sử dụng (bởi bác sĩ, lần khám).
                        // Điều này cần logic kiểm tra tham chiếu chéo phức tạp hơn.
                    }
                    else {
                        inThongDiepLoi("Xoa khoa phong that bai (co the do loi dong bo hoac khong tim thay trong mang).");
                    }
                }
                else {
                    printf("Huy xoa khoa phong.\n");
                }
            }
            else {
                inThongDiepLoi("Khong tim thay khoa phong voi ma da nhap trong bang bam.");
            }
            dungManHinhCho();
            break;
        }
        case 4: // Xem danh sách khoa phòng
            xoaManHinhConsole();
            inDanhSachKhoaPhongConsole(*dsKhoaPhong, *soLuongKhoaPhong); //
            dungManHinhCho();
            break;
        case 5: { // Tìm kiếm khoa phòng theo Mã KP (chỉ dùng bảng băm)
            char maTim_input[MAX_LEN_MA_KHOA_PHONG];
            printf("Nhap Ma Khoa Phong can tim: ");
            if (docDongAnToan(maTim_input, sizeof(maTim_input), stdin) == NULL || strlen(maTim_input) == 0) {
                inThongDiepLoi("Ma Khoa Phong khong duoc de trong.");
                dungManHinhCho();
                break;
            }
            for (int k = 0; maTim_input[k]; ++k) maTim_input[k] = toupper(maTim_input[k]);

            KhoaPhong* kpTimThay = NULL;
            if (bbKhoaPhong) {
                kpTimThay = timKhoaPhongTheoMaBangBam(bbKhoaPhong, maTim_input); //
            }
            else {
                inThongDiepLoi("Bang bam Khoa Phong khong kha dung.");
                dungManHinhCho();
                break;
            }
            // KHÔNG fallback

            xoaManHinhConsole();
            if (kpTimThay) {
                printf("--- THONG TIN KHOA PHONG TIM THAY ---\n");
                printf("Ma Khoa Phong: %s\n", kpTimThay->maKhoaPhong);
                printf("Ten Khoa Phong: %s\n", kpTimThay->tenKhoaPhong);
                printf("Vi Tri: %s\n", kpTimThay->viTri);
                printf("Mo Ta: %s\n", kpTimThay->moTa);
            }
            else {
                printf("Khong tim thay khoa phong voi ma '%s' trong bang bam.\n", maTim_input);
            }
            dungManHinhCho();
            break;
        }
        case 6: { // TÌM KIẾM TUYẾN TÍNH MỚI
            char tieuChiTimKiem[50];
            char giaTriTimKiem[200]; // Kích thước đủ cho Mô tả
            KhoaPhong* ketQua = NULL;
            int soLuongKQ = 0;

            printf("--- TIM KIEM KHOA PHONG TUYEN TINH ---\n");
            printf("Chon tieu chi tim kiem:\n");
            printf("  1. Ten Khoa Phong (tim kiem chua)\n");
            printf("  2. Mo Ta (tim kiem chua)\n");
            printf("  3. Vi Tri (tim kiem chua)\n");
            int luaChonTieuChi = nhapSoNguyenCoGioiHan("Lua chon tieu chi", 1, 3);

            if (luaChonTieuChi < 1 || luaChonTieuChi > 3) {
                inThongDiepLoi("Lua chon tieu chi khong hop le.");
                dungManHinhCho();
                break;
            }

            switch (luaChonTieuChi) {
            case 1: strcpy(tieuChiTimKiem, "TenKhoaPhong"); break;
            case 2: strcpy(tieuChiTimKiem, "MoTa"); break;
            case 3: strcpy(tieuChiTimKiem, "ViTri"); break;
            }

            printf("Nhap gia tri can tim cho '%s': ", tieuChiTimKiem);
            if (docDongAnToan(giaTriTimKiem, sizeof(giaTriTimKiem), stdin) == NULL || strlen(giaTriTimKiem) == 0) {
                inThongDiepLoi("Gia tri tim kiem khong duoc de trong.");
            }
            else {
                timKiemKhoaPhongTuyenTinh(*dsKhoaPhong, *soLuongKhoaPhong, tieuChiTimKiem, giaTriTimKiem, &ketQua, &soLuongKQ);
                xoaManHinhConsole();
                if (soLuongKQ > 0) {
                    printf("--- KET QUA TIM KIEM (%d khoa phong) ---\n", soLuongKQ);
                    inDanhSachKhoaPhongConsole(ketQua, soLuongKQ);
                    free(ketQua); // Giải phóng bộ nhớ
                }
                else {
                    printf("Khong tim thay khoa phong nao phu hop voi tieu chi '%s' va gia tri '%s'.\n", tieuChiTimKiem, giaTriTimKiem);
                }
            }
            dungManHinhCho();
            break;
        }
        case 0:
            printf("Quay lai menu chinh...\n");
            break;
        default:
            inThongDiepLoi("Lua chon khong hop le.");
            dungManHinhCho();
            break;
        }
    } while (choice != 0);
}

// CẬP NHẬT: hienThiThongTinBangBam (tên cũ: hienThiMaDinhDanhVaHash)
// Hàm này có thể được làm tổng quát hơn để hiển thị thông tin cho bất kỳ bảng băm nào
void hienThiThongTinBangBam(BangBam* bb, const char* tenBangBam) {
    xoaManHinhConsole();
    printf("--- THONG TIN BANG BAM: %s ---\n", tenBangBam);

    if (bb == NULL) {
        printf("Bang bam '%s' chua duoc khoi tao (NULL).\n", tenBangBam);
        dungManHinhCho();
        return;
    }
    printf("Dung luong (so bucket): %d\n", bb->dungLuong);
    printf("So luong phan tu hien tai: %d\n", bb->soLuongPhanTu);
    printf("He so tai: %.2f\n", bb->soLuongPhanTu / (double)bb->dungLuong);
    printf("--------------------------------------------------\n");

    if (bb->soLuongPhanTu == 0) {
        printf("Bang bam '%s' rong.\n", tenBangBam);
    }
    else {
        printf("| %-25s | %-15s |\n", "Khoa (Key)", "Chi So Bucket");
        printf("--------------------------------------------------\n");
        int count = 0;
        for (int i = 0; i < bb->dungLuong; ++i) {
            NutBam* nutHienTai = bb->cacBucket[i]; //
            while (nutHienTai != NULL) {
                printf("| %-25s | %-15d |\n", nutHienTai->khoa, i); //
                nutHienTai = nutHienTai->tiepTheo; //
                count++;
            }
        }
        printf("--------------------------------------------------\n");
        if (count != bb->soLuongPhanTu) {
            printf("CANH BAO: So luong dem duoc (%d) khac so luong luu tru (%d)!\n", count, bb->soLuongPhanTu);
        }
    }
    dungManHinhCho();
}
void menuXemThongTinBangBam(
    BangBam* bbBenhNhan,
    BangBam* bbBacSi,
    BangBam* bbKhoaPhong,
    BangBam* bbThuoc,
    BangBam* bbLanKham
) {
    int choice;
    do {
        xoaManHinhConsole();
        printf("--- XEM THONG TIN BANG BAM (CHI SO BUCKET) ---\n");
        printf("1. Bang Bam Benh Nhan\n");
        printf("2. Bang Bam Bac Si\n");
        printf("3. Bang Bam Khoa Phong\n");
        printf("4. Bang Bam Thuoc\n");
        printf("5. Bang Bam Lan Kham\n");
        printf("0. Quay lai menu chinh\n");

        choice = nhapSoNguyenCoGioiHan("Lua chon cua ban", 0, 5);
        if (choice < 0) { choice = 0; } // Xử lý nếu nhapSoNguyenCoGioiHan trả về lỗi

        switch (choice) {
        case 1:
            if (bbBenhNhan) hienThiThongTinBangBam(bbBenhNhan, "BENH NHAN");
            else inThongDiepLoi("Bang bam Benh Nhan chua khoi tao.");
           
            break;
        case 2:
            if (bbBacSi) hienThiThongTinBangBam(bbBacSi, "BAC SI");
            else inThongDiepLoi("Bang bam Bac Si chua khoi tao.");
     
            break;
        case 3:
            if (bbKhoaPhong) hienThiThongTinBangBam(bbKhoaPhong, "KHOA PHONG");
            else inThongDiepLoi("Bang bam Khoa Phong chua khoi tao.");
    
            break;
        case 4:
            if (bbThuoc) hienThiThongTinBangBam(bbThuoc, "THUOC");
            else inThongDiepLoi("Bang bam Thuoc chua khoi tao.");
    
            break;
        case 5:
            if (bbLanKham) hienThiThongTinBangBam(bbLanKham, "LAN KHAM");
            else inThongDiepLoi("Bang bam Lan Kham chua khoi tao.");
      
            break;
        case 0:
            printf("Quay lai menu chinh...\n");
            break;
        default:
            inThongDiepLoi("Lua chon khong hop le.");
          
            break;
        }
    } while (choice != 0);
}


// --- Dinh nghia ham menu chinh ---
void hienThiMenuChinh() {
    xoaManHinhConsole();
    printf("=============================================\n");
    printf("   HE THONG QUAN LY KHAM BENH BENH VIEN XYZ  \n");
    printf("=============================================\n");
    printf("1. Quan ly Benh Nhan\n");
    printf("2. Quan ly Thong Tin Cac Lan Kham\n");
    printf("3. Dang Ky Kham Moi\n");
    printf("4. Goi Benh Nhan Tiep Theo Vao Kham (Theo Khoa)\n");
    printf("5. Cap Nhat Thong Tin Lan Kham Da Goi\n");
    printf("6. Quan Ly/Cap Nhat Don Thuoc (Sau Kham)\n");
    printf("7. Quan ly Khoa Phong\n");
    printf("8. Quan ly Bac Si\n");
    printf("9. Quan ly Danh Muc Thuoc\n");
    printf("10. Xem Hang Doi Kham Benh (Theo Khoa)\n");
    printf("11. Xem Thong Tin Bang Bam (Chi So Bucket)\n"); // <-- THAY ĐỔI Ở ĐÂY
    printf("0. Thoat Chuong Trinh\n");
    printf("---------------------------------------------\n");
}

// CẬP NHẬT: xuLyLuaChonMenuChinh
void xuLyLuaChonMenuChinh(
    int choice,
    BenhNhan** dsBenhNhan, int* soLuongBenhNhan, BangBam** bbBenhNhan_ptr, const char* fileBN,
    LanKham** dsLanKham, int* soLuongLanKham, BangBam** bbLanKham_ptr, const char* fileLK,
    KhoaPhong** dsKhoaPhong, int* soLuongKhoaPhong, BangBam** bbKhoaPhong_ptr, const char* fileKP,
    BacSi** dsBacSi, int* soLuongBacSi, BangBam** bbBacSi_ptr, const char* fileBS,
    Thuoc** dsThuoc, int* soLuongThuoc, BangBam** bbThuoc_ptr, const char* fileT,
    DonThuocChiTiet** dsDonThuocChiTiet, int* soLuongDTCT, const char* fileDTCT,
    BangBam* bbHangDoiTheoKhoa
) {
    switch (choice) {
    case 1:
        if (bbBenhNhan_ptr == NULL || *bbBenhNhan_ptr == NULL) {
            inThongDiepLoi("He thong du lieu Benh Nhan chua san sang (Bang Bam NULL).");
            dungManHinhCho();
            break;
        }
        menuQuanLyBenhNhan(dsBenhNhan, soLuongBenhNhan, bbBenhNhan_ptr, fileBN);
        break;
    case 2:
        if (bbLanKham_ptr == NULL || *bbLanKham_ptr == NULL ||
            bbBenhNhan_ptr == NULL || *bbBenhNhan_ptr == NULL ||
            bbBacSi_ptr == NULL || *bbBacSi_ptr == NULL ||
            bbKhoaPhong_ptr == NULL || *bbKhoaPhong_ptr == NULL) {
            inThongDiepLoi("Mot hoac nhieu he thong du lieu can thiet (LanKham, BenhNhan, BacSi, KhoaPhong) chua san sang.");
            dungManHinhCho();
            break;
        }
        // Chữ ký của menuQuanLyLanKham cần được cập nhật để nhận bbHangDoiTheoKhoa
        menuQuanLyLanKham(dsLanKham, soLuongLanKham, bbLanKham_ptr,
            *dsBenhNhan, *soLuongBenhNhan, *bbBenhNhan_ptr,
            *dsBacSi, *soLuongBacSi, *bbBacSi_ptr,
            *dsKhoaPhong, *soLuongKhoaPhong, *bbKhoaPhong_ptr,
            bbHangDoiTheoKhoa,
            fileLK);
        break;
    case 3: {
        xoaManHinhConsole();
        printf("--- DANG KY KHAM MOI ---\n");
        if (soLuongKhoaPhong == NULL || *soLuongKhoaPhong == 0) {
            inThongDiepLoi("Chua co khoa phong nao trong he thong. Vui long them khoa phong truoc khi dang ky.");
        }
        else if (bbBenhNhan_ptr == NULL || *bbBenhNhan_ptr == NULL ||
            bbKhoaPhong_ptr == NULL || *bbKhoaPhong_ptr == NULL ||
            bbLanKham_ptr == NULL || *bbLanKham_ptr == NULL ||
            bbHangDoiTheoKhoa == NULL) {
            inThongDiepLoi("Mot hoac nhieu he thong du lieu/bang bam can thiet chua duoc khoi tao de dang ky kham moi!");
        }
        else {
            // Chữ ký của dangKyKhamMoi cần được cập nhật để nhận tất cả các BangBam cần thiết
            dangKyKhamMoi(dsLanKham, soLuongLanKham,
                dsBenhNhan, soLuongBenhNhan, *bbBenhNhan_ptr,
                *dsKhoaPhong, *soLuongKhoaPhong, *bbKhoaPhong_ptr,
                bbLanKham_ptr,
                bbHangDoiTheoKhoa,
                fileLK, fileBN);
        }
        dungManHinhCho();
        break;
    }
    case 4: {
        xoaManHinhConsole();
        printf("--- GOI BENH NHAN TIEP THEO (THEO KHOA) ---\n");
        if (bbHangDoiTheoKhoa == NULL) {
            inThongDiepLoi("He thong hang doi theo khoa chua san sang (NULL).");
        }
        else if (soLuongKhoaPhong == NULL || *soLuongKhoaPhong == 0) {
            printf("Chua co khoa phong nao trong he thong de co hang doi.\n");
        }
        else {
            int coHangDoiNaoKhongRong = 0;
            if (bbHangDoiTheoKhoa->cacBucket) {
                for (int i = 0; i < bbHangDoiTheoKhoa->dungLuong; ++i) {
                    NutBam* nut = bbHangDoiTheoKhoa->cacBucket[i];
                    while (nut) {
                        HangDoiUuTienMotKhoa* hqmk = (HangDoiUuTienMotKhoa*)nut->giaTri;
                        if (hqmk && hqmk->soLuongHienTai > 0) {
                            coHangDoiNaoKhongRong = 1;
                            break;
                        }
                        nut = nut->tiepTheo;
                    }
                    if (coHangDoiNaoKhongRong) break;
                }
            }
            if (!coHangDoiNaoKhongRong && bbHangDoiTheoKhoa->soLuongPhanTu > 0) { // Kiểm tra thêm bbHangDoiTheoKhoa->soLuongPhanTu > 0
                printf("Tat ca cac khoa phong hien khong co benh nhan nao trong hang doi.\n");
            }
            else if (bbHangDoiTheoKhoa->soLuongPhanTu == 0 && *soLuongKhoaPhong > 0) { // Trường hợp có khoa nhưng chưa khoa nào có hàng đợi
                printf("Chua co khoa phong nao co benh nhan trong hang doi.\n");
            }
            else {
                char maKP_goi[MAX_LEN_MA_KHOA_PHONG];
                printf("Danh sach cac khoa phong:\n");

                if (dsKhoaPhong != NULL && *dsKhoaPhong != NULL && *soLuongKhoaPhong > 0) {
                    inDanhSachKhoaPhongConsole(*dsKhoaPhong, *soLuongKhoaPhong);
                }
                else {
                    printf("Khong co du lieu Khoa Phong de hien thi.\n");
                    dungManHinhCho();
                    break;
                }

                printf("Nhap Ma Khoa Phong de goi benh nhan (DE TRONG DE HUY): ");
                if (docDongAnToan(maKP_goi, sizeof(maKP_goi), stdin) == NULL || strlen(maKP_goi) == 0) {
                    inThongDiepLoi("Huy goi benh nhan.");
                }
                else {
                    for (int k = 0; maKP_goi[k]; ++k) maKP_goi[k] = toupper(maKP_goi[k]);

                    KhoaPhong* kpKiemTra = NULL;
                    // Ưu tiên tìm bằng bảng băm nếu có và được khởi tạo
                    if (bbKhoaPhong_ptr && *bbKhoaPhong_ptr) kpKiemTra = timKhoaPhongTheoMaBangBam(*bbKhoaPhong_ptr, maKP_goi);
                    else if (dsKhoaPhong && *dsKhoaPhong) kpKiemTra = timKhoaPhongTheoMa(*dsKhoaPhong, *soLuongKhoaPhong, maKP_goi);

                    if (!kpKiemTra) {
                        inThongDiepLoi("Ma Khoa Phong khong ton tai.");
                    }
                    else if (laHangDoiTheoKhoaRong(bbHangDoiTheoKhoa, maKP_goi)) {
                        printf("Hang doi cua khoa %s rong, khong co benh nhan nao de goi.\n", maKP_goi);
                    }
                    else {
                        LanKham lkDuocGoi;
                        if (layBenhNhanTiepTheoTheoKhoa(bbHangDoiTheoKhoa, maKP_goi, &lkDuocGoi)) {
                            int timThayLKTrongDS = 0;
                            for (int i = 0; i < *soLuongLanKham; ++i) {
                                if ((*dsLanKham)[i].maLanKham == lkDuocGoi.maLanKham) {
                                    (*dsLanKham)[i].trangThai = LK_DANG_KHAM;
                                    (*dsLanKham)[i].ngayGioKham = time(NULL);
                                    ghiDanhSachLanKhamRaFile(fileLK, *dsLanKham, *soLuongLanKham);
                                    timThayLKTrongDS = 1;

                                    char tenBNHienThi[101] = "Khong ro";
                                    if (bbBenhNhan_ptr && *bbBenhNhan_ptr) {
                                        BenhNhan* bn = timBenhNhanTheoMaBangBam(*bbBenhNhan_ptr, lkDuocGoi.maDinhDanh);
                                        if (bn) strncpy(tenBNHienThi, bn->ten, sizeof(tenBNHienThi) - 1);
                                        tenBNHienThi[sizeof(tenBNHienThi) - 1] = '\0';
                                    }

                                    printf("Moi benh nhan (MaLK: %d, Ten: %s, MaBN: %s) vao kham tai khoa %s.\n",
                                        lkDuocGoi.maLanKham, tenBNHienThi, lkDuocGoi.maDinhDanh, maKP_goi);
                                    printf("  Muc Do Uu Tien: %s\n", mucDoUuTienToString(lkDuocGoi.mucDoUuTien));

                                    // Hiển thị thời gian đăng ký và thời gian chờ (lấy từ file gốc của bạn)
                                    char gioDangKyStr[30];
                                    struct tm* tm_info_dangky = localtime(&lkDuocGoi.gioDangKyThanhCong);
                                    if (tm_info_dangky != NULL) {
                                        strftime(gioDangKyStr, sizeof(gioDangKyStr), "%H:%M:%S %d/%m/%Y", tm_info_dangky);
                                        printf("  Thoi gian dang ky: %s\n", gioDangKyStr);
                                    }
                                    else {
                                        printf("  Thoi gian dang ky: Khong xac dinh\n");
                                    }
                                    double thoiGianChoGiay = difftime(time(NULL), lkDuocGoi.gioDangKyThanhCong); //
                                    // ... (logic tính và in thời gian chờ như file gốc)
                                    int tongGiayCho = (int)thoiGianChoGiay; //
                                    // ...
                                    printf("  Trang thai da cap nhat thanh: Dang Kham.\n");
                                    break;
                                }
                            }
                            if (!timThayLKTrongDS) {
                                inThongDiepLoi("Khong tim thay lan kham duoc goi trong danh sach chinh de cap nhat trang thai.");
                            }
                        }
                        else {
                            inThongDiepLoi("Khong the lay benh nhan tu hang doi khoa (co the do khoa khong co hang doi hoac loi logic).");
                        }
                    }
                }
            }
        }
        dungManHinhCho();
        break;
    }
    case 5: {
        xoaManHinhConsole();
        printf("--- CAP NHAT THONG TIN LAN KHAM DA GOI ---\n");
        if (bbBenhNhan_ptr == NULL || *bbBenhNhan_ptr == NULL ||
            bbBacSi_ptr == NULL || *bbBacSi_ptr == NULL ||
            bbKhoaPhong_ptr == NULL || *bbKhoaPhong_ptr == NULL) {
            inThongDiepLoi("Mot hoac nhieu bang bam (BenhNhan, BacSi, KhoaPhong) chua khoi tao de cap nhat lan kham!");
            dungManHinhCho();
            break;
        }

        printf("Cac lan kham co the cap nhat (Dang Cho hoac Dang Kham):\n");
        // Code hiển thị danh sách lần khám (tương tự file gốc, sử dụng bbBenhNhan để lấy tên)
        int countLKPhuHop = 0;
        if (dsLanKham && *dsLanKham) { // Kiểm tra con trỏ trước khi dùng
            for (int i = 0; i < *soLuongLanKham; ++i) {
                if ((*dsLanKham)[i].trangThai == LK_DANG_CHO || (*dsLanKham)[i].trangThai == LK_DANG_KHAM) {
                    char tenBNHienThi[101] = "N/A";
                    if (bbBenhNhan_ptr && *bbBenhNhan_ptr && strlen((*dsLanKham)[i].maDinhDanh) > 0) {
                        BenhNhan* bn = timBenhNhanTheoMaBangBam(*bbBenhNhan_ptr, (*dsLanKham)[i].maDinhDanh);
                        if (bn) strncpy(tenBNHienThi, bn->ten, sizeof(tenBNHienThi) - 1);
                        tenBNHienThi[sizeof(tenBNHienThi) - 1] = '\0';
                    }
                    printf(" - MaLK: %d, MaBN: %s, TenBN: %s, TrangThai: %s\n",
                        (*dsLanKham)[i].maLanKham,
                        (*dsLanKham)[i].maDinhDanh,
                        tenBNHienThi,
                        trangThaiLanKhamToString((*dsLanKham)[i].trangThai));
                    countLKPhuHop++;
                }
            }
        }

        if (countLKPhuHop > 0) {
            int maLKCapNhat = nhapSoNguyenCoGioiHan("Nhap Ma Lan Kham can cap nhat", 1, 999999);
            if (maLKCapNhat > 0) {
                capNhatThongTinLanKham(*dsLanKham, *soLuongLanKham, maLKCapNhat,
                    *dsBacSi, *soLuongBacSi,
                    *dsKhoaPhong, *soLuongKhoaPhong,
                    *bbBacSi_ptr, *bbKhoaPhong_ptr,
                    fileLK);
            }
        }
        else {
            printf("Khong co lan kham nao o trang thai 'Dang Cho' hoac 'Dang Kham' de cap nhat.\n");
        }
        dungManHinhCho();
        break;
    }
    case 6: {
        xoaManHinhConsole();
        printf("--- QUAN LY / CAP NHAT DON THUOC ---\n");
        if (bbThuoc_ptr == NULL || *bbThuoc_ptr == NULL ||
            bbBenhNhan_ptr == NULL || *bbBenhNhan_ptr == NULL) {
            inThongDiepLoi("Bang bam Thuoc hoac Benh Nhan chua duoc khoi tao!");
            dungManHinhCho();
            break;
        }

        printf("Cac lan kham da hoan thanh va co chi dinh don thuoc:\n");
        int countLKCoDon = 0;
        if (dsLanKham && *dsLanKham) { // Kiểm tra con trỏ
            for (int i = 0; i < *soLuongLanKham; ++i) {
                if ((*dsLanKham)[i].trangThai == LK_DA_HOAN_THANH && (*dsLanKham)[i].coDonThuoc == 1) {
                    char tenBNHienThi[101] = "N/A";
                    if (bbBenhNhan_ptr && *bbBenhNhan_ptr && strlen((*dsLanKham)[i].maDinhDanh) > 0) {
                        BenhNhan* bn = timBenhNhanTheoMaBangBam(*bbBenhNhan_ptr, (*dsLanKham)[i].maDinhDanh);
                        if (bn) strncpy(tenBNHienThi, bn->ten, sizeof(tenBNHienThi) - 1);
                        tenBNHienThi[sizeof(tenBNHienThi) - 1] = '\0';
                    }
                    printf(" - MaLK/MaDon: %d, MaBN: %s, TenBN: %s\n",
                        (*dsLanKham)[i].maLanKham,
                        (*dsLanKham)[i].maDinhDanh,
                        tenBNHienThi);
                    countLKCoDon++;
                }
            }
        }

        if (countLKCoDon > 0) {
            int maDonThuoc = nhapSoNguyenCoGioiHan("Nhap Ma Don Thuoc (Ma Lan Kham) de cap nhat chi tiet", 1, 999999);
            if (maDonThuoc > 0) {
                int hopLeDeCapNhatDT = 0;
                int lkIndex = -1;
                if (dsLanKham && *dsLanKham) {
                    for (int i = 0; i < *soLuongLanKham; ++i) {
                        if ((*dsLanKham)[i].maLanKham == maDonThuoc) {
                            lkIndex = i;
                            if ((*dsLanKham)[i].coDonThuoc == 1 && (*dsLanKham)[i].trangThai == LK_DA_HOAN_THANH) {
                                hopLeDeCapNhatDT = 1;
                            }
                            else if ((*dsLanKham)[i].coDonThuoc == 0) {
                                printf("Lan kham %d khong duoc chi dinh don thuoc.\n", maDonThuoc);
                            }
                            else if ((*dsLanKham)[i].trangThai != LK_DA_HOAN_THANH) {
                                printf("Lan kham %d chua hoan thanh, khong the cap nhat don thuoc.\n", maDonThuoc);
                            }
                            break;
                        }
                    }
                }
                if (lkIndex == -1 && maDonThuoc > 0) {
                    printf("Khong tim thay Lan Kham/Don Thuoc voi Ma %d.\n", maDonThuoc);
                }

                if (hopLeDeCapNhatDT) {
                    quanLyCapNhatChiTietDonThuoc(maDonThuoc, dsDonThuocChiTiet, soLuongDTCT,
                        *dsThuoc, *soLuongThuoc,
                        *bbThuoc_ptr,
                        fileDTCT);
                }
            }
        }
        else {
            printf("Khong co lan kham nao da hoan thanh va co don thuoc de cap nhat.\n");
        }
        dungManHinhCho();
        break;
    }
    case 7:
        if (bbKhoaPhong_ptr == NULL || *bbKhoaPhong_ptr == NULL) {
            inThongDiepLoi("He thong du lieu Khoa Phong chua san sang (Bang Bam NULL).");
            dungManHinhCho();
            break;
        }
        menuQuanLyKhoaPhong(dsKhoaPhong, soLuongKhoaPhong, bbKhoaPhong_ptr, fileKP);
        break;
    case 8:
        if (bbBacSi_ptr == NULL || *bbBacSi_ptr == NULL) {
            inThongDiepLoi("He thong du lieu Bac Si chua san sang (Bang Bam NULL).");
            dungManHinhCho();
            break;
        }
        if (bbKhoaPhong_ptr == NULL || *bbKhoaPhong_ptr == NULL) {
            inThongDiepLoi("He thong du lieu Khoa Phong chua san sang (can cho Quan Ly Bac Si).");
            dungManHinhCho();
            break;
        }
        menuQuanLyBacSi(dsBacSi, soLuongBacSi, bbBacSi_ptr, fileBS, *dsKhoaPhong, *soLuongKhoaPhong, *bbKhoaPhong_ptr);
        break;
    case 9:
        if (bbThuoc_ptr == NULL || *bbThuoc_ptr == NULL) {
            inThongDiepLoi("He thong du lieu Thuoc chua san sang (Bang Bam NULL).");
            dungManHinhCho();
            break;
        }
        menuQuanLyThuoc(dsThuoc, soLuongThuoc, bbThuoc_ptr, fileT);
        break;
    case 10: {
        xoaManHinhConsole();
        printf("--- XEM HANG DOI KHAM BENH (THEO KHOA) ---\n");
        if (bbHangDoiTheoKhoa == NULL) {
            inThongDiepLoi("He thong hang doi theo khoa chua san sang (NULL).");
        }
        else if (soLuongKhoaPhong == NULL || *soLuongKhoaPhong == 0) {
            printf("Chua co khoa phong nao trong he thong.\n");
        }
        else if (bbHangDoiTheoKhoa->soLuongPhanTu == 0 && *soLuongKhoaPhong > 0) {
            printf("Chua co khoa phong nao co benh nhan trong hang doi.\n");
        }
        else {
            char maKP_xem[MAX_LEN_MA_KHOA_PHONG];
            if (dsKhoaPhong != NULL && *dsKhoaPhong != NULL && *soLuongKhoaPhong > 0) {
                inDanhSachKhoaPhongConsole(*dsKhoaPhong, *soLuongKhoaPhong);
            }
            else {
                printf("Khong co du lieu Khoa Phong de hien thi.\n");
                dungManHinhCho();
                break;
            }
            printf("Nhap Ma Khoa Phong de xem hang doi (DE TRONG DE HUY): ");
            if (docDongAnToan(maKP_xem, sizeof(maKP_xem), stdin) != NULL && strlen(maKP_xem) > 0) {
                for (int k = 0; maKP_xem[k]; ++k) maKP_xem[k] = toupper(maKP_xem[k]);

                KhoaPhong* kpKiemTra = NULL;
                if (bbKhoaPhong_ptr && *bbKhoaPhong_ptr) kpKiemTra = timKhoaPhongTheoMaBangBam(*bbKhoaPhong_ptr, maKP_xem);
                else if (dsKhoaPhong && *dsKhoaPhong) kpKiemTra = timKhoaPhongTheoMa(*dsKhoaPhong, *soLuongKhoaPhong, maKP_xem);

                if (!kpKiemTra) {
                    inThongDiepLoi("Ma Khoa Phong khong ton tai.");
                }
                else {
                    inHangDoiMotKhoaConsole(bbHangDoiTheoKhoa, maKP_xem,
                        (bbBenhNhan_ptr ? *bbBenhNhan_ptr : NULL),
                        (dsBenhNhan ? *dsBenhNhan : NULL),
                        (soLuongBenhNhan ? *soLuongBenhNhan : 0));
                }
            }
            else {
                inThongDiepLoi("Huy xem hang doi.");
            }
        }
        dungManHinhCho();
        break;
    }
    case 11: // <-- THAY ĐỔI Ở ĐÂY: Gọi menu con mới
        menuXemThongTinBangBam(
            (bbBenhNhan_ptr ? *bbBenhNhan_ptr : NULL),
            (bbBacSi_ptr ? *bbBacSi_ptr : NULL),
            (bbKhoaPhong_ptr ? *bbKhoaPhong_ptr : NULL),
            (bbThuoc_ptr ? *bbThuoc_ptr : NULL),
            (bbLanKham_ptr ? *bbLanKham_ptr : NULL)
        );
        // Không cần dungManHinhCho() ở đây vì menuXemThongTinBangBam đã có
        break;
    case 0:
        printf("Dang thoat chuong trinh...\n");
        break;
    default:
        inThongDiepLoi("Lua chon khong hop le.");
        dungManHinhCho();
        break;
    }
}