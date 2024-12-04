#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

speed_t get_speed_mask(int speed) {
    switch (speed) {
        case 0: return B0;
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        case 200: return B200;
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        default: abort();
    }
}

tcflag_t get_data_bits_mask(int data_bits) {
    switch (data_bits) {
        case 5: return CS5;
        case 6: return CS6;
        case 7: return CS7;
        case 8: return CS8;
        default: abort();
    }
}

void set_input_speed(struct termios *options, int speed) {
    cfsetispeed(options, get_speed_mask(speed));
}

void set_output_speed(struct termios *options, int speed) {
    cfsetospeed(options, get_speed_mask(speed));
}

void set_data_bits(struct termios *options, int data_bits) {
    options->c_cflag &= ~CSIZE;
    options->c_cflag |= get_data_bits_mask(data_bits);
}

void set_parity(struct termios *options, char parity) {
    switch (parity) {
    case 'n':
        options->c_cflag &= ~PARENB;
        options->c_iflag &= ~INPCK;
        break;
    case 'o':
        options->c_cflag |= PARENB;
        options->c_cflag |= PARODD;
        options->c_iflag |= INPCK;
        break;
    case 'e':
        options->c_cflag |= PARENB;
        options->c_cflag &= ~PARODD;
        options->c_iflag |= INPCK;
        break;
    }
}

void set_stop_bits(struct termios *options, int stop_bits) {
    switch (stop_bits) {
    case 1:
        options->c_cflag &= ~CSTOPB;
        break;
    case 2:
        options->c_cflag |= CSTOPB;
        break;
    default:
        abort();
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <device>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDWR | O_NOCTTY | O_SYNC);
    if (fd == -1) {
        error(1, errno, "open");
    }

    struct termios options;
    if (tcgetattr(fd, &options) == -1) {
        error(1, errno, "tcgetattr");
    }

    set_input_speed(&options, 115200);
    set_output_speed(&options, 115200);
    set_data_bits(&options, 8);
    set_parity(&options, 'n');
    set_stop_bits(&options, 1);

    if (tcsetattr(fd, TCSANOW, &options) == -1) {
        error(1, errno, "tcsetattr");
    }

    char buf[256];
    for (;;) {
        ssize_t n_read = read(fd, buf, 256);
        if (n_read == -1) {
            error(1, errno, "read");
        }
        if (n_read > 0) {
            ssize_t n_written = write(fd, buf, n_read);
            if (n_written == -1) {
                error(1, errno, "write");
            }
        }
    }
    
    close(fd);
    return 0;
}
