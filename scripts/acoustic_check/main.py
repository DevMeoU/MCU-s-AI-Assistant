#!/usr/bin/env python3
""""
Main program for real-time audio monitoring and plotting system
Based on Qt GUI + Matplotlib + UDP receiving + AFSK decoding strings
--# Translated from: Real-time audio monitoring and plotting system main program based on Qt GUI + Matplotlib + UDP receiving + AFSK decoding strings
"""

import sys
import asyncio
from graphic import main

if __name__ == '__main__':
    try:
        asyncio.run(main())
    except KeyboardInterrupt:
        print("Program interrupted by user")
        # -- Dịch từ: Chương trình bị người dùng gián đoạn
    except Exception as e:
        print(f"Program execution error: {e}")
        # -- Dịch từ: Chương trình thực thi bị lỗi
        sys.exit(1)
