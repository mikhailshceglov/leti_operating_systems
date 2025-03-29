#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("suid_test_file", O_CREAT | O_WRONLY, 0664);
    close(fd);
    return 0;
}
