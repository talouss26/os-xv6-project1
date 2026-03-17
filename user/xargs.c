#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    // Nếu không có lệnh nào được truyền vào
    if (argc < 2) {
        printf("Usage: xargs [command]\n");
        exit(1);
    }

    char *xargs_argv[MAXARG]; // Mảng chứa các tham số cho lệnh exec
    int xargs_argc = 0;

    // 1. Copy câu lệnh gốc và các tham số ban đầu của nó
    // Ví dụ: xargs echo bye -> xargs_argv[0] = "echo", xargs_argv[1] = "bye"
    for (int i = 1; i < argc; i++) {
        xargs_argv[xargs_argc++] = argv[i];
    }

    char buf[512]; // Bộ đệm chứa từng dòng đọc được
    char ch;
    int buf_idx = 0;

    // 2. Đọc đầu vào chuẩn (file descriptor 0) từng ký tự một
    while (read(0, &ch, 1) == 1) {
        if (ch == '\n') {
            buf[buf_idx] = '\0'; // Kết thúc chuỗi khi gặp dấu xuống dòng

            // 3. Gắn dòng vừa đọc vào mảng tham số
            xargs_argv[xargs_argc] = buf;
            xargs_argv[xargs_argc + 1] = 0; // exec yêu cầu phần tử cuối cùng phải là 0 (NULL)

            // 4. Tạo tiến trình con để chạy lệnh
            int pid = fork();
            if (pid == 0) {
                // Trong tiến trình con: chạy lệnh bằng exec
                exec(xargs_argv[0], xargs_argv);
                // Nếu exec thất bại (không tìm thấy lệnh), in ra lỗi và thoát
                printf("xargs: exec %s failed\n", xargs_argv[0]);
                exit(1);
            } else if (pid > 0) {
                // Trong tiến trình cha: chờ tiến trình con chạy xong
                wait(0);
            } else {
                printf("xargs: fork failed\n");
                exit(1);
            }

            // 5. Reset lại bộ đệm để đọc dòng tiếp theo
            buf_idx = 0;
        } else {
            // Nếu chưa hết dòng, tiếp tục lưu ký tự vào bộ đệm
            buf[buf_idx++] = ch;
        }
    }

    exit(0);
}