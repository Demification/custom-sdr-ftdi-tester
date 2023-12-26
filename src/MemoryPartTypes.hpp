#pragma once
#include <stdint.h>

union __attribute__((__packed__, aligned(1))) 
Packet {
	unsigned char data[3];
	struct __attribute__((packed)) {
		uint8_t  rw_bit  : 1;
		uint16_t address : 15;
        uint8_t value    : 8;
	};
};

union __attribute__((__packed__, aligned(1))) 
HeaderStreamingPacket {
	unsigned char data[6];
	struct __attribute__((packed)) {
		uint8_t  rw_bit  : 1;
		uint16_t address : 15;
        uint32_t value   : 32;
	};
};

namespace RegistersConfig {
    
    struct __attribute__((__packed__, aligned(4))) 
    Sequence {
        uint32_t ethBurstType; //2byte Ethernet frame type
        uint64_t macDev;       //48-bit device MAC address
        uint64_t macHost;      //48-bit host MAC address
    };

    static const int Count = 5;
    static const int Offset = 1;
};

namespace RegistersEnaMask {
    union __attribute__((__packed__, aligned(4))) 
    Sequence {
		uint32_t config;
		struct __attribute__((packed))
		{
			uint32_t txEna : 1; //enable receiving packets from host
			uint32_t rxEna : 1; //enable Sheduling to host			
		};
    };

    static const int Count = 1;
    static const int Offset = 0;
};

namespace RegistersStatus {
    union __attribute__((__packed__, aligned(4))) 
    Sequence {
	    uint32_t status;
		struct __attribute__((packed))
		{
			uint32_t txOverrun : 1;
			uint32_t rxUnderrun : 1;
			uint32_t rxOverrun : 1;	
		};
    };

    static const int Count = 1;
    static const int Offset = 8;
};

namespace RegistersPriority {
    struct __attribute__((__packed__, aligned(4))) 
    Sequence {
        uint32_t basePriority; 	//8bit
    };

    static const int Count = 1;
    static const int Offset = 6;
};

namespace RegistersPause {
    
    struct __attribute__((__packed__, aligned(4))) 
    Sequence {
        uint32_t pauseFrameLen;	//16-bit
    };

    static const int Count = 1;
    static const int Offset = 7;
};