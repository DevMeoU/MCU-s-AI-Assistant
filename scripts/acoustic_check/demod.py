"""
Real-time AFSK Demodulator - Based on Goertzel Algorithm
"""
# Real-time AFSK Demodulator - Based on Goertzel Algorithm

import numpy as np
from collections import deque


class TraceGoertzel:
    """Real-time Goertzel Algorithm Implementation"""
    # Real-time Goertzel Algorithm Implementation
    
    def __init__(self, freq: float, n: int):
        """
        Initialize Goertzel Algorithm
        
        Args:
            freq: Normalized frequency (target frequency/sampling frequency)
            n: Window size
        """
        # Initialize Goertzel Algorithm
        self.freq = freq
        self.n = n
        
        # Pre-calculate coefficients - Consistent with reference code
        self.k = int(freq * n)
        self.w = 2.0 * np.pi * freq
        self.cw = np.cos(self.w)
        self.sw = np.sin(self.w)
        self.c = 2.0 * self.cw
        
        # Initialize state variables - Use deque to store the last two values
        self.zs = deque([0.0, 0.0], maxlen=2)
    
    def reset(self):
        """Reset Algorithm State"""
        # Đặt lại trạng thái thuật toán (Vietnamese)
        self.zs.clear()
        self.zs.extend([0.0, 0.0])
    
    def __call__(self, xs):
        """
        Process a Group of Sample Points - Interface Consistent with Reference Code
        
        Args:
            xs: Sequence of Sample Points
            
        Returns:
            Calculated Amplitude
        """
        # Process a Group of Sample Points - Interface Consistent with Reference Code
        self.reset()
        for x in xs:
            z1, z2 = self.zs[-1], self.zs[-2]  # Z[-1], Z[-2]
            z0 = x + self.c * z1 - z2  # S[n] = x[n] + C * S[n-1] - S[n-2]
            self.zs.append(float(z0))  # Update sequence
        return self.amp
    
    @property
    def amp(self) -> float:
        """Calculate Current Amplitude - Consistent with Reference Code"""
        # Calculate Current Amplitude - Consistent with Reference Code
        z1, z2 = self.zs[-1], self.zs[-2]
        ip = self.cw * z1 - z2
        qp = self.sw * z1
        return np.sqrt(ip**2 + qp**2) / (self.n / 2.0)


class PairGoertzel:
    """Dual-Frequency Goertzel Demodulator"""
    # Dual-Frequency Goertzel Demodulator
    
    def __init__(self, f_sample: int, f_space: int, f_mark: int, 
                 bit_rate: int, win_size: int):
        """
        Initialize Dual-Frequency Demodulator
        
        Args:
            f_sample: Sampling Frequency
            f_space: Space Frequency (usually corresponds to 0)
            f_mark: Mark Frequency (usually corresponds to 1)
            bit_rate: Bit Rate
            win_size: Goertzel Window Size
        """
        # Initialize Dual-Frequency Demodulator
        assert f_sample % bit_rate == 0, "Sampling frequency must be an integer multiple of the bit rate"
        
        self.Fs = f_sample
        self.F0 = f_space
        self.F1 = f_mark
        self.bit_rate = bit_rate
        self.n_per_bit = int(f_sample // bit_rate)  # Số lượng điểm mẫu cho mỗi bit
        
        # Tính toán tần số chuẩn hóa
        f1 = f_mark / f_sample
        f0 = f_space / f_sample
        
        # Khởi tạo thuật toán Goertzel
        self.g0 = TraceGoertzel(freq=f0, n=win_size)
        self.g1 = TraceGoertzel(freq=f1, n=win_size)
        
        # 输入缓冲区
        self.in_buffer = deque(maxlen=win_size)
        self.out_count = 0
        
        print(f"PairGoertzel initialized: f0={f0:.6f}, f1={f1:.6f}, win_size={win_size}, n_per_bit={self.n_per_bit}") # PairGoertzel initialized
    
    def __call__(self, s: float):
        """
        Process a Single Sample Point - Interface Consistent with Reference Code
        
        Args:
            s: Sample Point Value
            
        Returns:
            (amp0, amp1, p1_prob) - Space Frequency Amplitude, Mark Frequency Amplitude, Mark Probability
        """
        # Process a Single Sample Point - Interface Consistent with Reference Code
        self.in_buffer.append(s)
        self.out_count += 1
        
        amp0, amp1, p1_prob = 0, 0, None
        
        # Xuất kết quả một lần cho mỗi chu kỳ bit
        if self.out_count >= self.n_per_bit:
            amp0 = self.g0(self.in_buffer)  # Calculate space frequency amplitude
            amp1 = self.g1(self.in_buffer)  # Calculate mark frequency amplitude
            p1_prob = amp1 / (amp0 + amp1 + 1e-8)  # Tính xác suất mark
            self.out_count = 0
            
        return amp0, amp1, p1_prob


class RealTimeAFSKDecoder:
    """Real-time AFSK Decoder - Based on Start Frame Triggering"""
    # Real-time AFSK Decoder - Based on Start Frame Triggering
    
    def __init__(self, f_sample: int = 16000, mark_freq: int = 1800, 
                 space_freq: int = 1500, bitrate: int = 100, 
                 s_goertzel: int = 9, threshold: float = 0.5):
        """
        Initialize Real-time AFSK Decoder
        
        Args:
            f_sample: Sampling Frequency
            mark_freq: Mark Frequency
            space_freq: Space Frequency 
            bitrate: Bit Rate
            s_goertzel: Goertzel Window Size Coefficient (win_size = f_sample // mark_freq * s_goertzel)
            threshold: Decision Threshold
        """
        # Initialize Real-time AFSK Decoder
        self.f_sample = f_sample
        self.mark_freq = mark_freq
        self.space_freq = space_freq
        self.bitrate = bitrate
        self.threshold = threshold
        
        # Calculate window size - Consistent with reference code
        win_size = int(f_sample / mark_freq * s_goertzel)
        
        # Khởi tạo bộ giải điều chế
        self.demodulator = PairGoertzel(f_sample, space_freq, mark_freq, 
                                       bitrate, win_size)
        
        # Định nghĩa khung - phù hợp với mã tham khảo
        self.start_bytes = b'\x01\x02'
        self.end_bytes = b'\x03\x04'
        self.start_bits = "".join(format(int(x), '08b') for x in self.start_bytes)
        self.end_bits = "".join(format(int(x), '08b') for x in self.end_bytes)

        # Máy trạng thái
        self.state = "idle" # idle / entering
        
        # Lưu trữ kết quả giải điều chế
        self.buffer_prelude:deque = deque(maxlen=len(self.start_bits)) # Kiểm tra xem có khởi động hay không
        self.indicators = []  # Lưu trữ chuỗi xác suất
        self.signal_bits = ""  # Lưu trữ chuỗi bit
        self.text_cache = ""
        
        # Kết quả giải mã
        self.decoded_messages = []
        self.total_bits_received = 0
        
        print(f"Decoder initialized: win_size={win_size}") # Decoder initialized
        print(f"Start frame: {self.start_bits} (from {self.start_bytes.hex()})") # Start frame
        print(f"End frame: {self.end_bits} (from {self.end_bytes.hex()})") # End frame
    
    def process_audio(self, samples: np.array) -> str:
        """
        Process Audio Data and Return Decoded Text
        
        Args:
            audio_data: Audio Byte Data (16-bit PCM)
            
        Returns:
            Newly Decoded Text
        """       
        # Process Audio Data and Return Decoded Text
        new_text = "" 
        # Process each sample point
        # If there is probability output, record and make decisions
        for sample in samples:
            amp0, amp1, p1_prob = self.demodulator(sample)
            if p1_prob is not None:
                bit = '1' if p1_prob > self.threshold else '0'
                match self.state:
                    case "idle":
                        self.buffer_prelude.append(bit)
                        pass
                    case "entering":
                        self.buffer_prelude.append(bit)
                        self.signal_bits += bit
                        self.total_bits_received += 1
                    case _:
                        pass
                self.indicators.append(p1_prob)

                # Kiểm tra máy trạng thái
                if self.state == "idle" and "".join(self.buffer_prelude) == self.start_bits:
                    self.state = "entering"
                    self.text_cache = ""
                    self.signal_bits = ""  # Xóa chuỗi bit
                    self.buffer_prelude.clear()
                elif self.state == "entering" and ("".join(self.buffer_prelude) == self.end_bits or len(self.signal_bits) >= 256):
                    self.state = "idle"
                    self.buffer_prelude.clear()

                # Thử giải mã sau khi thu thập đủ số lượng bit nhất định
                if len(self.signal_bits) >= 8:
                    text = self._decode_bits_to_text(self.signal_bits)
                    if len(text) > len(self.text_cache):
                        new_text = text[len(self.text_cache) - len(text):]
                        self.text_cache = text
        return new_text
    
    def _decode_bits_to_text(self, bits: str) -> str:
        """
        Decode Bit String to Text
        
        Args:
            bits: Bit String
            
        Returns:
            Decoded Text
        """
        # Decode Bit String to Text
        if len(bits) < 8:
            return ""
        
        decoded_text = ""
        byte_count = len(bits) // 8
        
        for i in range(byte_count):
            # 提取8位
            byte_bits = bits[i*8:(i+1)*8]
            
            # 位转字节
            byte_val = int(byte_bits, 2)
            
            # 尝试解码为ASCII字符
            if 32 <= byte_val <= 126:  # 可打印ASCII字符
                decoded_text += chr(byte_val)
            elif byte_val == 0:  # NULL字符，忽略
                continue
            else:
                # 非可打印字符pass，以十六进制显示
                pass
                # decoded_text += f"\\x{byte_val:02X}"
        
        return decoded_text
    
    def clear(self):
        """Clear Decoding State"""
        # Clear Decoding State
        self.indicators = []
        self.signal_bits = ""
        self.decoded_messages = []
        self.total_bits_received = 0
        print("Decoder state cleared") # Decoder state cleared
    
    def get_stats(self) -> dict:
        """Get Decoding Statistics"""
        # Get Decoding Statistics
        return {
            'prelude_bits': "".join(self.buffer_prelude),
            "state": self.state,
            'total_chars': sum(len(msg) for msg in self.text_cache),
            'buffer_bits': len(self.signal_bits),
            'mark_freq': self.mark_freq,
            'space_freq': self.space_freq,
            'bitrate': self.bitrate,
            'threshold': self.threshold,
        }
