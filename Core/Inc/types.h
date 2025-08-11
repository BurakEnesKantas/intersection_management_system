#ifndef TYPES_H
#define TYPES_H

// Enum tanımları
typedef enum {
    MODE_IDLE,      // 0: Boşta
    MODE_SEQUENCE,  // 1: Trafik ışığı sequence modu
    MODE_FLASH,     // 2: Yanıp sönme modu
    MODE_MANUAL,    // 3: Manuel kontrol modu
    MODE_RESET,     // 5: Sistem yeniden başlatılıyor
    MODE_OFF,       // 4: Bütün ışıklar kapalı
    MODE_PHASE
} SystemMode_t;

#endif // TYPES_H
