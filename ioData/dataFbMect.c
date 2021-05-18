/*
 * Copyright 2011 Mect s.r.l
 *
 * This file is part of FarosPLC.
 *
 * FarosPLC is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * FarosPLC is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * FarosPLC. If not, see http://www.gnu.org/licenses/.
*/

/*
 * Filename: dataFbMect.c
 */

#include "dataImpl.h"

#define __4CFILE__	"dataFbMect.c"

/* ---------------------------------------------------------------------------- */

static void mect_printbuf(char *msg, char *buf, unsigned len);
static int mect_bcc(char *buf, unsigned len);

/* ---------------------------------------------------------------------------- */

#ifdef __XENO__
#include <rtdm/rtserial.h>

#else

#include <termios.h>
#include <unistd.h>

static ssize_t rt_dev_read(int fd, void *buf, size_t nbyte) {
    return read(fd, buf, nbyte);
}

static ssize_t rt_dev_write(int fd, const void *buf, size_t nbyte) {
    return write(fd, buf, nbyte);
}

static int rt_dev_close(int fd)
{
    return close(fd);
}

#endif

int mect_connect(unsigned devnum, unsigned baudrate, char parity, unsigned databits, unsigned stopbits, unsigned timeout_ms)
{
    int fd = -1;
    char devname[VMM_MAX_PATH] = "";
    int err = 0;

    snprintf(devname, VMM_MAX_PATH, SERIAL_DEVNAME, devnum);
#ifdef __XENO__
    struct rtser_config rt_serial_config;

    fd = rt_dev_open(devname, 0);
    if (fd < 0) {
        goto exit_open_failure;
    }
    rt_serial_config.config_mask =
        RTSER_SET_BAUD
        | RTSER_SET_DATA_BITS
        | RTSER_SET_STOP_BITS
        | RTSER_SET_PARITY
        | RTSER_SET_HANDSHAKE
        | RTSER_SET_TIMEOUT_RX
        ;
    /* Set Baud rate */
    rt_serial_config.baud_rate = baudrate;

    /* Set data bits (5, 6, 7, 8 bits) */
    switch (databits) {
        case 5:  rt_serial_config.data_bits = RTSER_5_BITS; break;
        case 6:  rt_serial_config.data_bits = RTSER_6_BITS; break;
        case 7:  rt_serial_config.data_bits = RTSER_7_BITS; break;
        case 8:  rt_serial_config.data_bits = RTSER_8_BITS; break;
        default: rt_serial_config.data_bits = RTSER_8_BITS; break;
    }

    /* Stop bit (1 or 2) */
    if (stopbits == 1)
        rt_serial_config.stop_bits = RTSER_1_STOPB;
    else /* 2 */
        rt_serial_config.stop_bits = RTSER_2_STOPB;

    /* Parity bit */
    if (parity == 'N')
        rt_serial_config.parity = RTSER_NO_PARITY;		/* None */
    else if (parity == 'E')
        rt_serial_config.parity = RTSER_EVEN_PARITY;	/* Even */
    else
        rt_serial_config.parity = RTSER_ODD_PARITY;		/* Odd */

    /* Disable software flow control */
    rt_serial_config.handshake = RTSER_NO_HAND;

    /* Response timeout in ns */
    rt_serial_config.rx_timeout = timeout_ms * 1E6;

    err = rt_dev_ioctl(fd, RTSER_RTIOC_SET_CONFIG, &rt_serial_config);
    if (err) {
        rt_dev_close(fd);
        fd = -1;
        goto exit_ioctl_failure;
    }
#else
    /* from libmodbus */
    (void)timeout_ms;
    struct termios serial_config;
    speed_t speed;
    int flags;

    flags = O_RDWR | O_NOCTTY | O_NDELAY | O_EXCL;
#ifdef O_CLOEXEC
    flags |= O_CLOEXEC;
#endif
    fd = open(devname, flags);
    if (fd < 0) {
        goto exit_open_failure;
    }
    memset(&serial_config, 0, sizeof(serial_config));

    /* Set Baud rate */
    switch (baudrate) {
    case 2400:
        speed = B2400;
        break;
    case 4800:
        speed = B4800;
        break;
    case 9600:
        speed = B9600;
        break;
    case 19200:
        speed = B19200;
        break;
    case 38400:
        speed = B38400;
        break;
    case 57600:
        speed = B57600;
        break;
    case 115200:
        speed = B115200;
        break;
    case 230400:
        speed = B230400;
        break;
    default:
        speed = B9600;
        fprintf(stderr,"%s(%d=uart%u) wrong baudate %u\n", __func__, fd, devnum, baudrate);
    }
    if (cfsetspeed(&serial_config, speed)) {
        err = errno;
        close(fd);
        goto exit_ioctl_failure;
    }

    /* C_CFLAG      Control options
       CLOCAL       Local line - do not change "owner" of port
       CREAD        Enable receiver
     */
    serial_config.c_cflag |= (CREAD | CLOCAL);
    /* CSIZE, HUPCL, CRTSCTS (hardware flow control) */

    /* Set data bits (5, 6, 7, 8 bits)
       CSIZE        Bit mask for data bits
     */
    serial_config.c_cflag &= ~CSIZE;
    switch (databits) {
        case 5:
            serial_config.c_cflag |= CS5;
            break;
        case 6:
            serial_config.c_cflag |= CS6;
            break;
        case 7:
            serial_config.c_cflag |= CS7;
            break;
        case 8:
        default:
            serial_config.c_cflag |= CS8;
            break;
    }

    /* Stop bit (1 or 2) */
    if (stopbits == 1)
        serial_config.c_cflag &=~ CSTOPB;
    else /* 2 */
        serial_config.c_cflag |= CSTOPB;

    /* Parity bit */
    if (parity == 'N') {
        /* None */
        serial_config.c_cflag &=~ PARENB;
    } else if (parity == 'E') {
        /* Even */
        serial_config.c_cflag |= PARENB;
        serial_config.c_cflag &=~ PARODD;
    } else {
        /* Odd */
        serial_config.c_cflag |= PARENB;
        serial_config.c_cflag |= PARODD;
    }

    /* Raw input */
    serial_config.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    if (parity == 'N') {
        /* None */
        serial_config.c_iflag &= ~INPCK;
    } else {
        serial_config.c_iflag |= INPCK;
    }

    /* Disable software flow control */
    serial_config.c_iflag &= ~(IXON | IXOFF | IXANY);

    /* Raw ouput */
    serial_config.c_oflag &=~ OPOST;

    /* Unused because we use open with the NDELAY option */
    serial_config.c_cc[VMIN] = 0;
    serial_config.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &serial_config) < 0) {
        err = errno;
        close(fd);
        fd = -1;
        goto exit_ioctl_failure;
    }
#endif
    fprintf(stderr, "%s(%d=uart%u) ok\n", __func__, fd, devnum);
    return fd;

exit_open_failure:
    fprintf(stderr, "%s(uart%u) open error\n", __func__, devnum);
    return -1;

exit_ioctl_failure:
    fprintf(stderr, "%s(uart%u) error: %s\n", __func__, devnum, strerror(-err));
    return -1;

}

int mect_read_ascii(int fd, unsigned node, unsigned command, float *value)
{
    char nn[2+1];
    char cc[2+1];
    char buf[13+1];
    int retval = 0;
    char *p;

    if (fd < 0 || value == NULL) {
        return -1;
    }

    // requesting value from node "12" and obtaining "1234.678"

    // question: EOT '1' '1' '2' '2' 'R' 'O' ENQ
    snprintf(nn, 2+1, "%02u", node);
    snprintf(cc, 2+1, "%c%c", (command & 0x7F00) >> 8, (command & 0x007F));
    snprintf(buf, 8+1, "\004%c%c%c%c%s\005", nn[0], nn[0], nn[1], nn[1], cc);
    rt_dev_write(fd, buf, 8);
    if (verbose_print_enabled)
        mect_printbuf("mect_read_ascii: wrote", buf, 8);

    // answer: STX 'R' 'O' '1' '2' '3' '4' '.' '6' '7' '8' ETX BCC
    //         [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10][11][12]
    // answer: STX 'R' 'O' '1' '2' '3' '.' '5' '6' ETX BCC
    //         [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10]
    memset(buf, 0, 13+1);
    retval = rt_dev_read(fd, buf, 13);

    if (verbose_print_enabled)
        mect_printbuf("mect_read_ascii: read", buf, (retval > 0) ? retval : 0);

    if ((retval != 11 && retval != 13) || buf[0] != '\002' || buf[1] != cc[0] || buf[2] != cc[1]
        || buf[retval - 2] != '\003' || (buf[retval - 1] != mect_bcc(&buf[1], retval - 2))) {
        if (verbose_print_enabled)
            fprintf(stderr, "mect_read_ascii: error retval=%d 0:%02x 1:%02x 2:%02x %d:%02x %d:%02x\n",
            retval, buf[0], buf[1], buf[2], retval - 2, buf[retval - 2], retval - 1, buf[retval - 1]);
        if (retval > 0) {
            return -1;
        } else {
            return -2;
        }
    }
    buf[11] = '\0';
    *value = strtof(&buf[3], &p);
    if (p == &buf[3]) {
        if (verbose_print_enabled)
            fprintf(stderr, "mect_read_ascii: error float\n");
        return -1;
    }
#ifdef VERBOSE_DEBUG
    fprintf(stderr, "mect_read_ascii: value=%f\n", *value);
#endif
    return 0;
}

int mect_write_ascii(int fd, unsigned node, unsigned command, float value)
{
    char nn[2+1];
    char cc[2+1];
    char buf[18+1];
    int retval;

    if (fd < 0) {
        return -1;
    }

    // command: EOT '1' '1' '2' '2' STX 'I' 'U' '1' '2' '3' '.' '5' '6' '7' '8' ETX BCC
    //          [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10][11][12][13][14][15][16][17]
    snprintf(nn, 2+1, "%02u", node);
    snprintf(cc, 2+1, "%c%c", (command & 0x7F00) >> 8, (command & 0x007F));
    snprintf(buf, 18+1, "\004%c%c%c%c\002%s%8.2f\003%c",
        nn[0], nn[0], nn[1], nn[1], cc, value, 0x00);
    buf[17] = mect_bcc(&buf[6], 11);
    rt_dev_write(fd, buf, 18);
    if (verbose_print_enabled) {
        mect_printbuf("mect_write_ascii: wrote", buf, 18);
    }

    // reply: ACK / NAK
    //        [0]
    retval = rt_dev_read(fd, buf, 1);
    if (verbose_print_enabled) {
        mect_printbuf("mect_write_ascii: read", buf, 1);
    }
    if (retval < 0 || buf[0] != '\006') {
        if (verbose_print_enabled) {
            fprintf(stderr, "mect_write_ascii: error retval=%d 0:%02x\n",
                retval, buf[0]);
        }
        return -1;
    }
    return 0;
}

int mect_read_hexad(int fd, unsigned node, unsigned command, unsigned *value)
{
    char nn[2+1];
    char cc[2+1];
    char buf[13+1];
    int retval;
    char *p;

    if (fd < 0 || value == NULL) {
        return -1;
    }

    // requesting value from node "12" and obtaining "1234.678"

    // question: EOT '1' '1' '2' '2' 'R' 'O' ENQ
    snprintf(nn, 2+1, "%02u", node);
    snprintf(cc, 2+1, "%c%c", (command & 0x7F00) >> 8, (command & 0x007F));
    snprintf(buf, 8+1, "\004%c%c%c%c%s\005", nn[0], nn[0], nn[1], nn[1], cc);
    rt_dev_write(fd, buf, 8);
    if (verbose_print_enabled)
        mect_printbuf("mect_read_hexad: wrote", buf, 8);


    // answer: STX 'R' 'O' ' ' ' ' ' ' '>' '0' '0' 'F' 'F' ETX BCC
    //         [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10][11][12]
    // answer: STX 'R' 'O' ' ' '>' '0' '0' 'F' 'F' ETX BCC
    //         [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10]
    retval = rt_dev_read(fd, buf, 13);
    if (verbose_print_enabled)
        mect_printbuf("mect_read_hexad: read", buf, (retval > 0) ? retval : 0);

    if ((retval != 11 && retval != 13) || buf[0] != '\002' || buf[1] != cc[0] || buf[2] != cc[1] || buf[3] != ' '
            || ((retval == 13) && (buf[4] != ' ' || buf[5] != ' '  || buf[6] != '>'))
            || ((retval == 11) && (buf[4] != '>'))
            || buf[retval - 2] != '\003' || buf[retval - 1] != mect_bcc(&buf[1], retval - 2)) {
        if (verbose_print_enabled)
            fprintf(stderr, "mect_read_hexad: error retval=%d 0:%02x 1:%02x 2:%02x 11:%02x 12:%02x\n",
                retval, buf[0], buf[1], buf[2], buf[11], buf[12]);
        if (retval > 0) {
            return -1;
        } else {
            return -2;
        }
    }
    buf[11] = '\0';
    *value = strtoul(&buf[retval - 6], &p, 16);
    if (p == &buf[retval - 6]) {
        if (verbose_print_enabled)
            fprintf(stderr, "mect_read_hexad: error hexadecimal\n");
        return -1;
    }
#ifdef VERBOSE_DEBUG
    fprintf(stderr, "mect_read_hexad: value=0x%04xn", *value);
#endif
    return 0;
}

int mect_write_hexad(int fd, unsigned node, unsigned command, unsigned value)
{
    char nn[2+1];
    char cc[2+1];
    char buf[18+1];
    int retval;

    if (fd < 0) {
        return -1;
    }

    // command: EOT '1' '1' '2' '2' STX 'P' 'T' ' ' ' ' ' ' '>' '0' '0' 'F' 'F' ETX BCC
    //          [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10][11][12][13][14][15][16][17]
    snprintf(nn, 2+1, "%02u", node);
    snprintf(cc, 2+1, "%c%c", (command & 0x7F00) >> 8, (command & 0x007F));
    snprintf(buf, 18+1, "\004%c%c%c%c\002%s   >%04X\003%c",
        nn[0], nn[0], nn[1], nn[1], cc, (value & 0xFFFF), 0x00);
    buf[17] = mect_bcc(&buf[6], 11);
    rt_dev_write(fd, buf, 18);
#ifdef VERBOSE_DEBUG
    mect_printbuf("mect_write_hexad: wrote", buf, 18);
#endif

    // reply: ACK / NAK
    //        [0]
    retval = rt_dev_read(fd, buf, 1);
#ifdef VERBOSE_DEBUG
    mect_printbuf("mect_write_hexad: read", buf, 1);
#endif
    if (retval < 0 || buf[0] != '\006') {
#ifdef VERBOSE_DEBUG
        fprintf(stderr, "mect_write_hexad: error retval=%d 0:%02x\n",
            retval, buf[0]);
#endif
        return -1;
    }
    return 0;
}

void mect_close(int fd)
{
    if (fd >= 0) {
        rt_dev_close(fd);
    }
    fprintf(stderr, "%s(%d)\n", __func__, fd);
}

/* ---------------------------------------------------------------------------- */

static int mect_bcc(char *buf, unsigned len)
{
    unsigned i;
    int bcc = buf[0];

    for (i = 1; i < len; ++i) {
        bcc ^= buf[i];
    }

    return bcc;
}

static void mect_printbuf(char *msg, char *buf, unsigned len)
{
    unsigned i;

    fputs(msg, stderr);
    for (i = 0; i < len; ++i) {
        fprintf(stderr, " %02x", buf[i]);
    }
    fprintf(stderr, "\n");
}

/* ---------------------------------------------------------------------------- */
