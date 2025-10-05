// image_to_jpeg.h - Efficient encoding interface for image to JPEG conversion
// JPEG encoding implementation that saves about 8KB SRAM
// (Gốc: image_to_jpeg.h - Giao diện mã hóa hiệu quả từ hình ảnh đến chuyển đổi JPEG
// Tiết kiệm khoảng 8KB SRAM cho việc thực hiện mã hóa JPEG)

#ifndef IMAGE_TO_JPEG_H
#define IMAGE_TO_JPEG_H

#include <stdint.h>
#include <stddef.h>
#include <esp_camera.h>  // Bao gồm định nghĩa của trình điều khiển camera ESP32, tránh định nghĩa lại pixformat_t và camera_fb_t

#ifdef __cplusplus
extern "C" {
#endif

// JPEG output callback function type
// arg: User-defined parameter, index: Current data index, data: JPEG data block, len: Data block length
// Returns: Number of bytes actually processed
// (Gốc: Loại hàm callback đầu ra JPEG
// arg: Tham số tùy chỉnh người dùng, index: Chỉ số dữ liệu hiện tại, data: Khối dữ liệu JPEG, len: Độ dài khối dữ liệu
// Trả về: Số byte thực tế được xử lý)
typedef size_t (*jpg_out_cb)(void *arg, size_t index, const void *data, size_t len);

/**
 * @brief Efficiently convert image format to JPEG
 * 
 * This function uses an optimized JPEG encoder for encoding, main features:
 * - Saves about 8KB SRAM usage (static variables changed to heap allocation)
 * - Supports multiple image format inputs
 * - High-quality JPEG output
 * 
 * @param src       Source image data
 * @param src_len   Source image data length
 * @param width     Image width
 * @param height    Image height  
 * @param format    Image format (PIXFORMAT_RGB565, PIXFORMAT_RGB888, etc.)
 * @param quality   JPEG quality (1-100)
 * @param out       Output JPEG data pointer (caller needs to free)
 * @param out_len   Output JPEG data length
 * 
 * @return true success, false failure
 */
// (Gốc: Chuyển đổi định dạng hình ảnh hiệu quả sang JPEG
// Hàm này sử dụng bộ mã hóa JPEG tối ưu hóa để mã hóa, các đặc điểm chính:
// - Tiết kiệm khoảng 8KB sử dụng SRAM (biến tĩnh thay đổi thành phân bổ heap)
// - Hỗ trợ nhiều định dạng hình ảnh đầu vào
// - Đầu ra JPEG chất lượng cao
// @param src       Dữ liệu hình ảnh nguồn
// @param src_len   Độ dài dữ liệu hình ảnh nguồn
// @param width     Chiều rộng hình ảnh
// @param height    Chiều cao hình ảnh
// @param format    Định dạng hình ảnh (PIXFORMAT_RGB565, PIXFORMAT_RGB888, v.v.)
// @param quality   Chất lượng JPEG (1-100)
// @param out       Con trỏ dữ liệu JPEG đầu ra (người gọi cần giải phóng)
// @param out_len   Độ dài dữ liệu JPEG đầu ra
// @return true thành công, false thất bại)
bool image_to_jpeg(uint8_t *src, size_t src_len, uint16_t width, uint16_t height, 
                   pixformat_t format, uint8_t quality, uint8_t **out, size_t *out_len);

/**
 * @brief Convert image format to JPEG (callback version)
 * 
 * Uses callback function to handle JPEG output data, suitable for streaming or block processing:
 * - Saves about 8KB SRAM usage (static variables changed to heap allocation)
 * - Supports streaming output, no need for pre-allocating large buffers
 * - Processes JPEG data block by block via callback function
 * 
 * @param src       Source image data
 * @param src_len   Source image data length
 * @param width     Image width
 * @param height    Image height
 * @param format    Image format
 * @param quality   JPEG quality (1-100)
 * @param cb        Output callback function
 * @param arg       User parameter passed to callback function
 * 
 * @return true success, false failure
 */
// (Gốc: Chuyển đổi định dạng hình ảnh sang JPEG (phiên bản callback)
// Sử dụng hàm callback để xử lý dữ liệu đầu ra JPEG, phù hợp cho truyền phát hoặc xử lý khối:
// - Tiết kiệm khoảng 8KB sử dụng SRAM (biến tĩnh thay đổi thành phân bổ heap)
// - Hỗ trợ đầu ra truyền phát, không cần phân bổ trước bộ đệm lớn
// - Xử lý dữ liệu JPEG theo khối qua hàm callback
// @param src       Dữ liệu hình ảnh nguồn
// @param src_len   Độ dài dữ liệu hình ảnh nguồn
// @param width     Chiều rộng hình ảnh
// @param height    Chiều cao hình ảnh
// @param format    Định dạng hình ảnh
// @param quality   Chất lượng JPEG (1-100)
// @param cb        Hàm callback đầu ra
// @param arg       Tham số người dùng truyền cho hàm callback
// @return true thành công, false thất bại)
bool image_to_jpeg_cb(uint8_t *src, size_t src_len, uint16_t width, uint16_t height, 
                      pixformat_t format, uint8_t quality, jpg_out_cb cb, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* IMAGE_TO_JPEG_H */
