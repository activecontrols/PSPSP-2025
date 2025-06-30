#ifndef PLATFORM_BRIDGE_H
#define PLATFORM_BRIDGE_H

#if defined(TARGET_TEENY41)
#include <Arduino.h>
#include <SD.h>
#elif defined(TARGET_NATIVE)
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <istream>
#include <fstream>
#endif

namespace PlatformBridge {
	#if defined(TARGET_TEENY41)

	using ::File;
	using ::Serial;
	using ::delay;
	using ::SD;

	#elif defined(TARGET_NATIVE)

	#define BUILTIN_SDCARD 254

	inline void delay(unsigned long ms) {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}

	class usb_serial_class {
	public:
		usb_serial_class() {

		}

		void begin(long bitsPerSecond) {

		}

		void setTimeout(long msTime) {

		}

		template <typename T>
		size_t print(T in) {
			std::string str = in;
			std::cout << in;
			return str.size();
		}

		template <typename T>
		size_t println(T in) {
			std::string str = in;
			std::cout << in << std::endl;
			return str.size();
		}

		size_t readBytes(char* buffer, size_t length, std::istream& stream = std::cin) {
			size_t count = 0;
			for (char c; count < length && stream.get(c); buffer[count++] = c);

			return count;
		}

		size_t readBytesUntil(char terminator, char* buffer, size_t length, std::istream& stream = std::cin) {
			size_t count = 0;
			for (char c; count < length && stream.get(c) && c != terminator; buffer[count++] = c);

			return count;
		}
	};

	inline usb_serial_class Serial = PlatformBridge::usb_serial_class();

	#define FILE_READ 0
	#define FILE_WRITE 1
	#define FILE_APPEND 2

	class File {
	public:
		std::fstream stream;
		char mode;

		File() = default;

		File(const char* filename, char mode) {
			this->mode = mode;

			std::ios_base::openmode openMode = std::ios::binary;
			if (mode == FILE_READ)
					openMode |= std::ios::in;
			else if (mode == FILE_WRITE)
					openMode |= std::ios::in | std::ios::out | std::ios::trunc;
			else if (mode == FILE_APPEND)
					openMode |= std::ios::out | std::ios::app;

			this->stream.open(filename, openMode);
		}
	};

	class SDClass {
	public:
		SDClass() {

		}

		bool begin(uint8_t csPin = 0) {
			return true;
		}

		File open(const char* filename, char mode) {
			return File(filename, mode);
		}
	};

	inline SDClass SD = SDClass();

	#endif
};

#endif