#include "NativeSpiBackend.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "Debug.hpp"

#define CHECK_RESULT(X,Y) if(X==-1) {__DEBUG_ERROR__(Y); return; }

NativeSpiBackend::NativeSpiBackend(): AbstractSpiBackend() {
    int result = -1;
    
    m_fd = open(m_device.c_str(), O_RDWR);
    CHECK_RESULT(m_fd, "Can't open device.");

	result = ioctl(m_fd, SPI_IOC_WR_MODE32, &m_mode);
    CHECK_RESULT(result, "Can't set spi mode.");

	result = ioctl(m_fd, SPI_IOC_RD_MODE32, &m_mode);
    CHECK_RESULT(result, "Can't get spi mode.");

	result = ioctl(m_fd, SPI_IOC_WR_BITS_PER_WORD, &m_bits);
    CHECK_RESULT(result, "Can't set bits per word");

    result = ioctl(m_fd, SPI_IOC_RD_BITS_PER_WORD, &m_bits);
    CHECK_RESULT(result, "Can't get bits per word");

    result = ioctl(m_fd, SPI_IOC_WR_MAX_SPEED_HZ, &m_speed);
    CHECK_RESULT(result, "Can't set max speed hz");

    result = ioctl(m_fd, SPI_IOC_RD_MAX_SPEED_HZ, &m_speed);
    CHECK_RESULT(result, "Can't get max speed hz");

    m_isInited = result == -1 ? false : true;
}

NativeSpiBackend::~NativeSpiBackend() {
    if((m_fd != -1) && ::close(m_fd) == -1) {
        __DEBUG_ERROR__("Can`t close file: " + m_device);
    }
}

bool NativeSpiBackend::isInited() {
    return m_isInited;
}

SpiBackendType NativeSpiBackend::type() const {
    return SpiBackendType::Native;
}

bool NativeSpiBackend::write(uint8_t *data, unsigned length) {
    return writeAndRead(data, nullptr, length);
}

bool NativeSpiBackend::writeAndRead(uint8_t *data, uint8_t *result, unsigned length) {
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long) data,
		.rx_buf = (unsigned long) result,
		.len = length,
		.delay_usecs = 0,
		.bits_per_word = 0,
	};

    if(ioctl(m_fd, SPI_IOC_MESSAGE(1), &tr) == -1) {
        return false;
    }

    return true;
}
