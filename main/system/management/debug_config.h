#pragma once

// Cấu hình debug logging cho các module khác nhau
// Set các giá trị này thành 1 để bật debug logging chi tiết, 0 để tắt

// MQTT Protocol debug
#define CONFIG_MQTT_DEBUG_LOG 0

// AFE (Audio Front End) debug
#define CONFIG_AFE_DEBUG_LOG 0

// Audio Service debug
#define CONFIG_AUDIO_SERVICE_DEBUG_LOG 0

// Network debug
#define CONFIG_NETWORK_DEBUG_LOG 0

// System info debug
#define CONFIG_SYSTEM_INFO_DEBUG_LOG 0

// Memory management debug
#define CONFIG_MEMORY_DEBUG_LOG 0

// Tổng hợp các cấu hình debug
#ifdef CONFIG_MQTT_DEBUG_LOG
#define MQTT_DEBUG_ENABLED 1
#else
#define MQTT_DEBUG_ENABLED 0
#endif

#ifdef CONFIG_AFE_DEBUG_LOG
#define AFE_DEBUG_ENABLED 1
#else
#define AFE_DEBUG_ENABLED 0
#endif

#ifdef CONFIG_AUDIO_SERVICE_DEBUG_LOG
#define AUDIO_SERVICE_DEBUG_ENABLED 1
#else
#define AUDIO_SERVICE_DEBUG_ENABLED 0
#endif

#ifdef CONFIG_NETWORK_DEBUG_LOG
#define NETWORK_DEBUG_ENABLED 1
#else
#define NETWORK_DEBUG_ENABLED 0
#endif

#ifdef CONFIG_SYSTEM_INFO_DEBUG_LOG
#define SYSTEM_INFO_DEBUG_ENABLED 1
#else
#define SYSTEM_INFO_DEBUG_ENABLED 0
#endif

#ifdef CONFIG_MEMORY_DEBUG_LOG
#define MEMORY_DEBUG_ENABLED 1
#else
#define MEMORY_DEBUG_ENABLED 0
#endif