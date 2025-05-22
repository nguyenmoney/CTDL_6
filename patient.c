    #include "patient.h"
    #include "csv_handler.h" // Cho docDongAnToan
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>   
    #include <ctype.h>  
    #include "hash_table.h"
    #define PATIENT_HO_TEN_COL_WIDTH 30
    #define PATIENT_DIA_CHI_COL_WIDTH 30
    #define PATIENT_TIEU_SU_COL_WIDTH 20
    // static void removeNewline(char* str) { /* ... */ } // Giu nguyen neu co
    // static void readStringSafe(char* buffer, int size) { /* ... */ } // Co the thay bang docDongAnToan
    
    const char* gioiTinhToString(GioiTinh gt) {
        switch (gt) {
            case GT_NAM: return "Nam";
            case GT_NU: return "Nu";
            case GT_KHAC: return "Khac";
            default: return "KhongXacDinh";
        }
    }
    
    GioiTinh stringToGioiTinh(const char* str) {
        if (str == NULL || strlen(str) == 0) return GT_KHAC;    
        char lowerStr[100];
        strncpy(lowerStr, str, 99);
        lowerStr[99] = '\0';
        for(int i = 0; lowerStr[i]; i++){
            lowerStr[i] = tolower(lowerStr[i]);
        }
    
        if (strcmp(lowerStr, "nam") == 0) return GT_NAM;
        if (strcmp(lowerStr, "nu") == 0) return GT_NU;
        if (strcmp(lowerStr, "khac") == 0) return GT_KHAC;
        return GT_KHAC; 
    }
    
    
    BenhNhan* docDanhSachBenhNhan(const char* tenFile, int* soLuongBenhNhan) {
        // ... (Ham nay giu nguyen logic doc tu file)
        // ... (Copy lai toan bo ham docDanhSachBenhNhan tu phien ban truoc)
        FILE* f = fopen(tenFile, "r");
        if (f == NULL) {
            *soLuongBenhNhan = 0;
            return NULL;
        }

        BenhNhan* ds = NULL;
        int capacity = 0;
        *soLuongBenhNhan = 0;
        char line[1500]; 

        while (fgets(line, sizeof(line), f) != NULL) {
            xoaKyTuXuongDong(line); // Su dung ham tu csv_handler.h
            if (strlen(line) < 10) continue; 

            if (*soLuongBenhNhan >= capacity) {
                capacity = (capacity == 0) ? 10 : capacity * 2; 
                BenhNhan* temp = realloc(ds, capacity * sizeof(BenhNhan));
                if (temp == NULL) {
                    printf("Loi cap phat bo nho (realloc docDanhSachBenhNhan)");
                    free(ds); 
                    fclose(f);
                    *soLuongBenhNhan = 0; 
                    return NULL;
                }
                ds = temp;
            }

            BenhNhan bn;
            char gioiTinhStr[20];
            long ngayTaoTimestamp;
            char* token;
            
            token = strtok(line, ","); 
            if (token) {
                strncpy(bn.maDinhDanh, token, sizeof(bn.maDinhDanh)-1);
            bn.maDinhDanh[sizeof(bn.maDinhDanh)-1] = '\0';
            } else {
                // Ma dinh danh la bat buoc, bo qua dong neu khong co
                fprintf(stderr, "Loi doc CSV Benh Nhan: Dong khong co Ma Dinh Danh.\n");
            continue; 
            }
            token = strtok(NULL, ","); if (token) strncpy(bn.bhyt, token, sizeof(bn.bhyt)-1); else strcpy(bn.bhyt, ""); bn.bhyt[sizeof(bn.bhyt)-1] = '\0';
            token = strtok(NULL, ","); if (token) strncpy(bn.soDienThoai, token, sizeof(bn.soDienThoai)-1); else strcpy(bn.soDienThoai, ""); bn.soDienThoai[sizeof(bn.soDienThoai)-1] = '\0';
            token = strtok(NULL, ","); if (token) strncpy(bn.ten, token, sizeof(bn.ten)-1); else strcpy(bn.ten, ""); bn.ten[sizeof(bn.ten)-1] = '\0';
            token = strtok(NULL, ","); if (token) bn.ngaySinh.ngay = atoi(token); else bn.ngaySinh.ngay = 0;
            token = strtok(NULL, ","); if (token) bn.ngaySinh.thang = atoi(token); else bn.ngaySinh.thang = 0;
            token = strtok(NULL, ","); if (token) bn.ngaySinh.nam =  atoi(token); else bn.ngaySinh.nam = 0;
            token = strtok(NULL, ","); if (token) bn.tuoi = atoi(token); else bn.tuoi = 0;
            token = strtok(NULL, ","); if (token) strncpy(gioiTinhStr, token, sizeof(gioiTinhStr)-1); else strcpy(gioiTinhStr, ""); gioiTinhStr[sizeof(gioiTinhStr)-1] = '\0';
                                        bn.gioiTinh = stringToGioiTinh(gioiTinhStr);
            token = strtok(NULL, ","); if (token) strncpy(bn.diaChi, token, sizeof(bn.diaChi)-1); else strcpy(bn.diaChi, ""); bn.diaChi[sizeof(bn.diaChi)-1] = '\0';
            token = strtok(NULL, ","); if (token) strncpy(bn.tieuSuBenhLy, token, sizeof(bn.tieuSuBenhLy)-1); else strcpy(bn.tieuSuBenhLy, ""); bn.tieuSuBenhLy[sizeof(bn.tieuSuBenhLy)-1] = '\0';
            token = strtok(NULL, "\n"); if (token) ngayTaoTimestamp = atol(token); else ngayTaoTimestamp = 0; // Doc den het dong
                                        bn.ngayTaoHoSo = (time_t)ngayTaoTimestamp;

            ds[*soLuongBenhNhan] = bn;
            (*soLuongBenhNhan)++;
        }
        fclose(f);
        return ds;
    }
    
    void ghiDanhSachBenhNhanRaFile(const char* tenFile, const BenhNhan dsBenhNhan[], int soLuongBenhNhan) {
        // ... (Ham nay giu nguyen logic ghi ra file)
        // ... (Copy lai toan bo ham ghiDanhSachBenhNhanRaFile tu phien ban truoc)
        FILE* f = fopen(tenFile, "w"); 
        if (f == NULL) {
            perror("Loi mo file de ghi (ghiDanhSachBenhNhanRaFile)");
            return;
        }
        for (int i = 0; i < soLuongBenhNhan; ++i) {
            fprintf(f, "%s,%s,%s,%s,%d,%d,%d,%d,%s,%s,%s,%ld\n", 
                    dsBenhNhan[i].maDinhDanh,
                    dsBenhNhan[i].bhyt,
                    dsBenhNhan[i].soDienThoai,
                    dsBenhNhan[i].ten,
                    dsBenhNhan[i].ngaySinh.ngay,
                    dsBenhNhan[i].ngaySinh.thang,
                    dsBenhNhan[i].ngaySinh.nam,
                    dsBenhNhan[i].tuoi,
                    gioiTinhToString(dsBenhNhan[i].gioiTinh),
                    dsBenhNhan[i].diaChi,
                    dsBenhNhan[i].tieuSuBenhLy,
                    (long)dsBenhNhan[i].ngayTaoHoSo);
        }
        fclose(f);
    }
    
    // int taoMaDinhDanhBenhNhanMoi(const BenhNhan dsBenhNhan[], int soLuongBenhNhan) { // <<<< XOA HAM NAY
    //     // ...
    // }
    
    // Ham tim benh nhan theo ma dinh danh (noi bo, de kiem tra trung lap)
    static const BenhNhan* timBenhNhanTheoMaNoiBo(const BenhNhan dsBenhNhan[], int soLuong, int maDD) {
        for (int i = 0; i < soLuong; ++i) {
            if (dsBenhNhan[i].maDinhDanh == maDD) {
                return &dsBenhNhan[i];
            }
        }
        return NULL;
    }

    // Ham tim benh nhan theo ma dinh danh (chuoi) - co the la static neu chi dung noi bo
const BenhNhan* timBenhNhanTheoMaChuoi(const BenhNhan dsBenhNhan[], int soLuong, const char* maDD_str) {
    if (maDD_str == NULL) return NULL;
    for (int i = 0; i < soLuong; ++i) {
        if (strcmp(dsBenhNhan[i].maDinhDanh, maDD_str) == 0) {
            return &dsBenhNhan[i];
        }
    }
    return NULL;
}
BenhNhan* timBenhNhanTheoMaBangBam(BangBam* bb, const char* maDD_str) {
    if (bb == NULL || maDD_str == NULL) {
        return NULL;
    }
    return (BenhNhan*)timKiemTrongBangBam(bb, maDD_str);
}
int nhapThongTinBenhNhanTuBanPhim(BenhNhan* bn, const BenhNhan dsBenhNhanHienCo[], int soLuongHienCo, BangBam* bbBenhNhan) {
    char buffer[1000];
    char maDD_tam_str[MAX_LEN_MA_DINH_DANH_BN];

    printf("Nhap Ma Dinh Danh Benh Nhan (toi da %d ky tu, VD: BN001): ", MAX_LEN_MA_DINH_DANH_BN - 1);
    if (docDongAnToan(maDD_tam_str, sizeof(maDD_tam_str), stdin) == NULL || strlen(maDD_tam_str) == 0) {
        // Sử dụng hàm inThongDiepLoi từ ui.h (nếu patient.c có thể include ui.h hoặc có hàm tương tự)
        // Hoặc in trực tiếp:
        printf("LOI: Ma Dinh Danh khong duoc de trong. Huy them benh nhan.\n");
        return 0;
    }
    // Chuyển mã sang chữ hoa
    for (int k = 0; maDD_tam_str[k]; k++) maDD_tam_str[k] = toupper(maDD_tam_str[k]);

    // Kiểm tra trùng mã CHỈ bằng bảng băm
    if (bbBenhNhan != NULL) {
        if (timKiemTrongBangBam(bbBenhNhan, maDD_tam_str) != NULL) {
            printf("LOI: Ma Dinh Danh '%s' da ton tai (kiem tra bang bam). Huy them benh nhan.\n", maDD_tam_str);
            return 0;
        }
    }
    else {
        // Nếu bbBenhNhan là NULL, báo lỗi và không cho phép tiếp tục.
        // Việc kiểm tra trùng lặp MaDinhDanh là bắt buộc.
        printf("LOI HE THONG: Bang bam benh nhan khong kha dung. Khong the kiem tra trung lap Ma Dinh Danh. Huy them benh nhan.\n");
        return 0; // Quay lại, không cho phép thêm nếu không kiểm tra được
    }
    strncpy(bn->maDinhDanh, maDD_tam_str, sizeof(bn->maDinhDanh) - 1);
    bn->maDinhDanh[sizeof(bn->maDinhDanh) - 1] = '\0';

    // Nhap va kiem tra BHYT (Sử dụng tìm kiếm tuyến tính)
    printf("Nhap BHYT (toi da %d ky tu, bo trong neu khong co): ", (int)(sizeof(bn->bhyt) - 1));
    docDongAnToan(bn->bhyt, sizeof(bn->bhyt), stdin);
    if (strlen(bn->bhyt) > 0) {
        for (int i = 0; i < soLuongHienCo; ++i) {
            if (strlen(dsBenhNhanHienCo[i].bhyt) > 0 && strcmp(dsBenhNhanHienCo[i].bhyt, bn->bhyt) == 0) {
                printf("LOI: BHYT '%s' da ton tai (kiem tra tuyen tinh). Huy them benh nhan.\n", bn->bhyt);
                return 0;
            }
        }
    }

    // Nhap va kiem tra So Dien Thoai (Sử dụng tìm kiếm tuyến tính)
    printf("Nhap So Dien Thoai (toi da %d ky tu, bo trong neu khong co): ", (int)(sizeof(bn->soDienThoai) - 1));
    docDongAnToan(bn->soDienThoai, sizeof(bn->soDienThoai), stdin);
    if (strlen(bn->soDienThoai) > 0) {
        for (int i = 0; i < soLuongHienCo; ++i) {
            if (strlen(dsBenhNhanHienCo[i].soDienThoai) > 0 && strcmp(dsBenhNhanHienCo[i].soDienThoai, bn->soDienThoai) == 0) {
                printf("LOI: So Dien Thoai '%s' da ton tai (kiem tra tuyen tinh). Huy them benh nhan.\n", bn->soDienThoai);
                return 0;
            }
        }
    }

    // Nhap Ho va Ten (BẮT BUỘC)
    printf("Nhap Ho va Ten (toi da %d ky tu, KHONG DUOC BO TRONG): ", (int)(sizeof(bn->ten) - 1));
    if (docDongAnToan(bn->ten, sizeof(bn->ten), stdin) == NULL || strlen(bn->ten) == 0) {
        printf("LOI: Ho va Ten khong duoc de trong. Huy them benh nhan.\n");
        return 0;
    }

    // Nhap Ngay Sinh (BẮT BUỘC)
    printf("Nhap Ngay Sinh (ngay thang nam, vd: 15 01 1990, KHONG DUOC BO TRONG): ");
    if (docDongAnToan(buffer, sizeof(buffer), stdin) == NULL || strlen(buffer) == 0) {
        printf("LOI: Ngay Sinh khong duoc de trong. Huy them benh nhan.\n");
        return 0;
    }
    if (sscanf(buffer, "%d %d %d", &bn->ngaySinh.ngay, &bn->ngaySinh.thang, &bn->ngaySinh.nam) != 3) {
        printf("LOI: Dinh dang Ngay Sinh khong hop le. Huy them benh nhan.\n");
        return 0;
    }
    // TODO: Thêm kiểm tra tính hợp lệ chi tiết của ngày, tháng, năm (ví dụ: ngày 1-31, tháng 1-12, năm hợp lý)
    // Ví dụ kiểm tra đơn giản:
    if (bn->ngaySinh.ngay < 1 || bn->ngaySinh.ngay > 31 || bn->ngaySinh.thang < 1 || bn->ngaySinh.thang > 12 || bn->ngaySinh.nam < 1900 || bn->ngaySinh.nam > 2100) {
        printf("LOI: Ngay Sinh khong hop le (ngay/thang/nam khong dung). Huy them benh nhan.\n");
        return 0;
    }


    // Nhap Tuoi (BẮT BUỘC)
    printf("Nhap Tuoi (KHONG DUOC BO TRONG): ");
    if (docDongAnToan(buffer, sizeof(buffer), stdin) == NULL || strlen(buffer) == 0) {
        printf("LOI: Tuoi khong duoc de trong. Huy them benh nhan.\n");
        return 0;
    }
    if (sscanf(buffer, "%d", &bn->tuoi) != 1 || bn->tuoi < 0) {
        printf("LOI: Tuoi khong hop le (phai la so nguyen khong am). Huy them benh nhan.\n");
        return 0;
    }

    // Nhap Gioi Tinh
    printf("Nhap Gioi Tinh (Nam, Nu, Khac): ");
    docDongAnToan(buffer, sizeof(buffer), stdin);
    bn->gioiTinh = stringToGioiTinh(buffer); // stringToGioiTinh đã có xử lý mặc định nếu nhập không khớp

    // Nhap Dia Chi
    printf("Nhap Dia Chi (toi da %d ky tu, bo trong neu khong co): ", (int)(sizeof(bn->diaChi) - 1));
    docDongAnToan(bn->diaChi, sizeof(bn->diaChi), stdin);
    if (strlen(bn->diaChi) == 0) { // Nếu địa chỉ không bắt buộc, có thể đặt giá trị mặc định
        strcpy(bn->diaChi, "Khong co");
    }

    // Nhap Tieu Su Benh Ly
    printf("Nhap Tieu Su Benh Ly (neu co, toi da %d ky tu, bo trong neu khong co): ", (int)(sizeof(bn->tieuSuBenhLy) - 1));
    docDongAnToan(bn->tieuSuBenhLy, sizeof(bn->tieuSuBenhLy), stdin);
    if (strlen(bn->tieuSuBenhLy) == 0) {
        strcpy(bn->tieuSuBenhLy, "Khong co");
    }

    bn->ngayTaoHoSo = time(NULL);
    return 1; // Nhap thanh cong neu tat ca kiem tra deu qua
}


    
// Trong file patient.c

// ... (các include và hàm khác giữ nguyên) ...

// Trong patient.c
// Nhớ include hash_table.h nếu chưa có (để gọi rehashBangBam)

int themBenhNhan(BenhNhan** dsBenhNhanConTro, int* soLuongBenhNhan,
    BangBam** bbBenhNhan_ptr, // Sử dụng con trỏ cấp hai
    BenhNhan benhNhanMoi, const char* tenFile) {
    BangBam* bbHienTai = *bbBenhNhan_ptr; // Lấy con trỏ BangBam hiện tại để làm việc

    // Kiểm tra trùng lặp MaDinhDanh bằng bảng băm trước khi thêm vào mảng
    if (bbHienTai != NULL) {
        if (timKiemTrongBangBam(bbHienTai, benhNhanMoi.maDinhDanh) != NULL) {
            fprintf(stderr, "LOI NGHIEM TRONG: Ma Dinh Danh '%s' da ton tai trong bang bam ngay truoc khi them vao mang!\n", benhNhanMoi.maDinhDanh);
            return 0;
        }
    }
    else {
        fprintf(stderr, "LOI HE THONG: Bang bam benh nhan khong kha dung. Khong the them benh nhan '%s' an toan.\n", benhNhanMoi.maDinhDanh);
        return 0;
    }

    // Cấp phát bộ nhớ cho mảng để thêm bệnh nhân mới
    BenhNhan* temp = realloc(*dsBenhNhanConTro, (*soLuongBenhNhan + 1) * sizeof(BenhNhan));
    if (temp == NULL) {
        perror("Loi cap phat bo nho (realloc themBenhNhan)");
        return 0;
    }
    *dsBenhNhanConTro = temp;

    // Thêm bệnh nhân mới vào cuối mảng
    (*dsBenhNhanConTro)[*soLuongBenhNhan] = benhNhanMoi;
    BenhNhan* conTroToiBnTrongMang = &((*dsBenhNhanConTro)[*soLuongBenhNhan]);

    // Chèn vào bảng băm (bbHienTai là con trỏ BangBam* hiện tại)
    if (!chenVaoBangBam(bbHienTai, benhNhanMoi.maDinhDanh, conTroToiBnTrongMang)) {
        fprintf(stderr, "LOI NGHIEM TRONG: Khong the chen Benh Nhan '%s' vao bang bam.\n", benhNhanMoi.maDinhDanh);
        // Không tăng *soLuongBenhNhan, không ghi file, không rollback mảng vì chưa tăng số lượng
        return 0;
    }

    // Chèn thành công vào cả mảng (logic) và bảng băm
    (*soLuongBenhNhan)++; // Bây giờ mới chính thức tăng số lượng bệnh nhân

    // Kiểm tra hệ số tải và thực hiện rehash NẾU CẦN
    // Sau khi chèn, bbHienTai->soLuongPhanTu đã được cập nhật bên trong chenVaoBangBam
    // Cần kiểm tra lại bbHienTai vì nó có thể đã thay đổi sau rehash nếu rehash được gọi từ chenVaoBangBam
    // Tuy nhiên, với thiết kế hiện tại, rehash được gọi TỪ themBenhNhan, nên bbHienTai vẫn là bảng cũ trước khi rehash.
    // Sau khi rehashBangBam(bbBenhNhan_ptr) thành công, *bbBenhNhan_ptr sẽ trỏ tới bảng mới.
    if ((*bbBenhNhan_ptr)->soLuongPhanTu / (double)(*bbBenhNhan_ptr)->dungLuong > BANG_BAM_NGUONG_TAI_TOI_DA) {
        printf("Thong bao: He so tai cua bang bam benh nhan vuot nguong (%d/%d). Dang chuan bi rehash...\n",
            (*bbBenhNhan_ptr)->soLuongPhanTu, (*bbBenhNhan_ptr)->dungLuong);
        if (!rehashBangBam(bbBenhNhan_ptr)) { // Truyền con trỏ cấp hai
            fprintf(stderr, "LOI: Rehash bang bam benh nhan that bai. Chuong trinh co the tiep tuc nhung hieu suat se giam.\n");
            // Nếu rehash thất bại, bbBenhNhan_ptr vẫn trỏ đến bảng cũ, chưa được tối ưu
        }
        // Nếu rehash thành công, bbBenhNhan_ptr đã được cập nhật để trỏ đến bảng mới bên trong rehashBangBam
    }

    ghiDanhSachBenhNhanRaFile(tenFile, *dsBenhNhanConTro, *soLuongBenhNhan);
    return 1; // Thêm thành công
}
int suaThongTinBenhNhan(BenhNhan dsBenhNhan[], int soLuongBenhNhan, char* maDinhDanhCanSua, BenhNhan thongTinMoi, const char* tenFile) {
    int timThayIndex = -1;
    for (int i = 0; i < soLuongBenhNhan; ++i) {
        if (strcmp(dsBenhNhan[i].maDinhDanh, maDinhDanhCanSua) == 0) {
            timThayIndex = i;
            break;
        }
    }

    if (timThayIndex != -1) {
        // Ma Dinh Danh không được sửa. Sao chep ma cu vao thong tin moi.
        strcpy(thongTinMoi.maDinhDanh, dsBenhNhan[timThayIndex].maDinhDanh);
        thongTinMoi.ngayTaoHoSo = dsBenhNhan[timThayIndex].ngayTaoHoSo;
        dsBenhNhan[timThayIndex] = thongTinMoi;

        ghiDanhSachBenhNhanRaFile(tenFile, dsBenhNhan, soLuongBenhNhan);
        return 1;
    }
    return 0; // Khong tim thay
}


// <<< SUA HAM: xoaBenhNhan >>>
// Thêm tham số BangBam* bbBenhNhan
int xoaBenhNhan(BenhNhan** dsBenhNhanConTro, int* soLuongBenhNhan, BangBam* bbBenhNhan, char* maDinhDanhCanXoa, const char* tenFile) {
    int viTriCanXoa = -1;
    for (int i = 0; i < *soLuongBenhNhan; ++i) {
        if (strcmp((*dsBenhNhanConTro)[i].maDinhDanh, maDinhDanhCanXoa) == 0) {
            viTriCanXoa = i;
            break;
        }
    }

    if (viTriCanXoa != -1) {
        // Xóa khỏi bảng băm trước
        if (bbBenhNhan != NULL) {
            if (!xoaKhoiBangBam(bbBenhNhan, maDinhDanhCanXoa)) {
                fprintf(stderr, "CANH BAO: Khong tim thay Benh Nhan '%s' trong bang bam de xoa (nhung co trong mang).\n", maDinhDanhCanXoa);
                // Điều này có thể xảy ra nếu có lỗi trong việc đồng bộ giữa mảng và bảng băm
            }
        }

        // Dịch chuyển các phần tử trong mảng động
        for (int i = viTriCanXoa; i < (*soLuongBenhNhan - 1); ++i) {
            (*dsBenhNhanConTro)[i] = (*dsBenhNhanConTro)[i + 1];
        }
        (*soLuongBenhNhan)--;

        if (*soLuongBenhNhan == 0) {
            free(*dsBenhNhanConTro);
            *dsBenhNhanConTro = NULL;
        }
        else {
            BenhNhan* temp = realloc(*dsBenhNhanConTro, (*soLuongBenhNhan) * sizeof(BenhNhan));
            if (temp == NULL && *soLuongBenhNhan > 0) {
                perror("Loi realloc khi xoa benh nhan (thu nho mang)");
                // Dữ liệu cũ vẫn còn trong *dsBenhNhanConTro, nhưng có thể không phải là kích thước mong muốn
                // Đây là trường hợp khó xử lý, có thể gây rò rỉ hoặc lỗi sau này.
                // Để an toàn, nếu realloc thất bại, có thể không nên thay đổi *dsBenhNhanConTro
                // Tuy nhiên, file CSV vẫn sẽ được ghi với *soLuongBenhNhan đã giảm.
            }
            else if (*soLuongBenhNhan > 0) {
                *dsBenhNhanConTro = temp;
            }
            else { // soLuongBenhNhan == 0
                free(*dsBenhNhanConTro); // Sicherstellen, dass der alte Speicher freigegeben wird, falls realloc NULL zurückgibt
                *dsBenhNhanConTro = NULL;
            }
        }
        ghiDanhSachBenhNhanRaFile(tenFile, *dsBenhNhanConTro, *soLuongBenhNhan);
        return 1;
    }
    return 0;
}

    
    void inDanhSachBenhNhanConsole(const BenhNhan dsBenhNhan[], int soLuongBenhNhan) {
        printf("\n--- DANH SACH BENH NHAN ---\n");
        if (soLuongBenhNhan == 0) {
            printf("Khong co benh nhan nao trong danh sach.\n");
            return;
        }

        // Dinh dang cho printf, su dung * de truyen do rong cot mot cach dong
        printf("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        printf("| %-5s | %-18s | %-15s | %-15s | %-*s | %-10s | %-5s | %-8s | %-*s | %-*s |\n",
            "STT", "Ma DD", "BHYT", "SDT",
            PATIENT_HO_TEN_COL_WIDTH, "Ho Ten",        // Su dung define
            "Ngay Sinh", "Tuoi", "GioiTinh",
            PATIENT_DIA_CHI_COL_WIDTH, "Dia Chi",      // Su dung define
            PATIENT_TIEU_SU_COL_WIDTH, "Tieu Su Benh Ly"); // Su dung define
        printf("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

        for (int i = 0; i < soLuongBenhNhan; ++i) {
            char ngaySinhStr[11];
            snprintf(ngaySinhStr, sizeof(ngaySinhStr), "%02d/%02d/%04d",
                dsBenhNhan[i].ngaySinh.ngay, dsBenhNhan[i].ngaySinh.thang, dsBenhNhan[i].ngaySinh.nam);

            // Buffer phai du lon de chua chuoi da rut gon (VD: doRong + 1 cho '\0')
            // >>> SUA: Su dung #define de khai bao kich thuoc mang <<<
            char hoTenDisplay[PATIENT_HO_TEN_COL_WIDTH + 1];
            rutGonChuoiHienThi(hoTenDisplay, sizeof(hoTenDisplay), dsBenhNhan[i].ten, PATIENT_HO_TEN_COL_WIDTH);

            char diaChiDisplay[PATIENT_DIA_CHI_COL_WIDTH + 1];
            rutGonChuoiHienThi(diaChiDisplay, sizeof(diaChiDisplay), dsBenhNhan[i].diaChi, PATIENT_DIA_CHI_COL_WIDTH);

            char tieuSuDisplay[PATIENT_TIEU_SU_COL_WIDTH + 1];
            rutGonChuoiHienThi(tieuSuDisplay, sizeof(tieuSuDisplay), dsBenhNhan[i].tieuSuBenhLy, PATIENT_TIEU_SU_COL_WIDTH);

            printf("| %-5d | %-18s | %-15s | %-15s | %-*s | %-10s | %-5d | %-8s | %-*s | %-*s |\n",
                i + 1,
                dsBenhNhan[i].maDinhDanh,
                dsBenhNhan[i].bhyt,
                dsBenhNhan[i].soDienThoai,
                PATIENT_HO_TEN_COL_WIDTH, hoTenDisplay, // Truyen define cho do rong
                ngaySinhStr,
                dsBenhNhan[i].tuoi,
                gioiTinhToString(dsBenhNhan[i].gioiTinh),
                PATIENT_DIA_CHI_COL_WIDTH, diaChiDisplay,    // Truyen define cho do rong
                PATIENT_TIEU_SU_COL_WIDTH, tieuSuDisplay); // Truyen define cho do rong
        }
        printf("-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    }
    void timKiemBenhNhan(const BenhNhan dsBenhNhan[], int soLuongBenhNhan,
                         const char* tieuChi, const char* giaTriTimKiem,
                         BenhNhan** ketQuaTimKiem, int* soLuongKetQua) {
        // ... (Ham nay giu nguyen)
        // ... (Copy lai toan bo ham timKiemBenhNhan tu phien ban truoc)
        *ketQuaTimKiem = NULL;
        *soLuongKetQua = 0;
        if (soLuongBenhNhan == 0 || giaTriTimKiem == NULL || tieuChi == NULL) {
            return;
        }

        BenhNhan* dsKetQuaTamThoi = malloc(soLuongBenhNhan * sizeof(BenhNhan)); 
        if (dsKetQuaTamThoi == NULL) {
            perror("Loi cap phat bo nho (timKiemBenhNhan)");
            return;
        }
        int count = 0;

        char giaTriTimKiemLower[100];
        strncpy(giaTriTimKiemLower, giaTriTimKiem, sizeof(giaTriTimKiemLower)-1);
        giaTriTimKiemLower[sizeof(giaTriTimKiemLower)-1] = '\0';
        for(int i = 0; giaTriTimKiemLower[i]; i++) giaTriTimKiemLower[i] = tolower(giaTriTimKiemLower[i]);

        char tieuChiLower[50];
        strncpy(tieuChiLower, tieuChi, sizeof(tieuChiLower)-1);
        tieuChiLower[sizeof(tieuChiLower)-1] = '\0';
        for(int i = 0; tieuChiLower[i]; i++) tieuChiLower[i] = tolower(tieuChiLower[i]);


        for (int i = 0; i < soLuongBenhNhan; ++i) {
            int timThayTrongVongLapNay = 0;
            if (strcmp(tieuChiLower, "madinhdanh") == 0) {
                if (strcmp(dsBenhNhan[i].maDinhDanh, giaTriTimKiem) == 0) { // atoi van an toan neu giaTriTimKiem khong phai so
                    timThayTrongVongLapNay = 1;
                }
            } else if (strcmp(tieuChiLower, "bhyt") == 0) {
                if (strcmp(dsBenhNhan[i].bhyt, giaTriTimKiem) == 0) {
                    timThayTrongVongLapNay = 1;
                }
            } else if (strcmp(tieuChiLower, "sodienthoai") == 0) {
                if (strcmp(dsBenhNhan[i].soDienThoai, giaTriTimKiem) == 0) {
                    timThayTrongVongLapNay = 1;
                }
            } else if (strcmp(tieuChiLower, "ten") == 0) {
                char tenBenhNhanLower[100];
                strncpy(tenBenhNhanLower, dsBenhNhan[i].ten, sizeof(tenBenhNhanLower)-1);
                tenBenhNhanLower[sizeof(tenBenhNhanLower)-1] = '\0';
                for(int k=0; tenBenhNhanLower[k]; k++) tenBenhNhanLower[k] = tolower(tenBenhNhanLower[k]);

                if (strstr(tenBenhNhanLower, giaTriTimKiemLower) != NULL) {
                    timThayTrongVongLapNay = 1;
                }
            }

            if (timThayTrongVongLapNay) {
                dsKetQuaTamThoi[count++] = dsBenhNhan[i];
            }
        }

        if (count > 0) {
            *ketQuaTimKiem = malloc(count * sizeof(BenhNhan));
            if (*ketQuaTimKiem == NULL) {
                perror("Loi cap phat bo nho cho ket qua tim kiem");
                free(dsKetQuaTamThoi);
                return;
            }
            memcpy(*ketQuaTimKiem, dsKetQuaTamThoi, count * sizeof(BenhNhan));
            *soLuongKetQua = count;
        }
        free(dsKetQuaTamThoi);
    }
    