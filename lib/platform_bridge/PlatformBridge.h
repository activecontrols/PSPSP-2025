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
#include <filesystem>
#include <cstring>
#endif

// Move BUILTIN_SDCARD to be a local filename next to the executable
// Remove the namespace and have it be a drop in replacement
// csv and implement for tadpole
// Make everything use CStrings if possible, but otherwise compensate with std::string
// Move to multithreading (complete data no pausing unless explicitly required otherwise use fbo like buffer switching system and deprioritizing recentness of data)
// Share to everyone else to test on their machines

#if defined(TARGET_TEENY41)

// Not required, but list the portions of the APIs used here so we can keep track of things and move it to another namespace if needed
using ::File;
using ::Serial;
using ::delay;
using ::SD;
using ::String;

#elif defined(TARGET_NATIVE)

class String : public std::string {
public:
    String() : std::string() {}
    String(const std::string& str) : std::string(str) {}
    String(const char* s) : std::string(s) {}
    String(const std::string& str, size_t pos, size_t len = std::string::npos) : std::string(str, pos, len) {}
};

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

	String readString(size_t length = 120) {
		char* buffer = new char[length];
		this->readBytes(buffer, length);

		String str = String(buffer);
		delete buffer;

		return str;
	}

	size_t readBytesUntil(char terminator, char* buffer, size_t length, std::istream& stream = std::cin) {
		size_t count = 0;
		for (char c; count < length && stream.get(c) && c != terminator; buffer[count++] = c);

		return count;
	}

	String readStringUntil(char terminator, size_t length = 120) {
		char* buffer = new char[length];
		this->readBytesUntil(terminator, buffer, length);

		String str = String(buffer);
		delete buffer;

		return str;
	}
};

inline usb_serial_class Serial = usb_serial_class();

#define FILE_READ 0
#define FILE_WRITE 1
#define FILE_APPEND 2

class File {
public:
	std::string name_str;
	std::filesystem::path path;
	char mode;
	std::fstream stream;
	std::filesystem::directory_iterator dir_cursor;
	inline const static std::filesystem::directory_iterator dir_end;

	File() = default;

	File(const char* path_str, char mode = FILE_WRITE) {
		this->path = std::filesystem::path(path_str);
		this->name_str = this->path.filename().string();
		this->mode = mode;

		std::ios_base::openmode openMode = std::ios::binary;
		if (mode == FILE_READ)
				openMode |= std::ios::in;
		else if (mode == FILE_WRITE)
				openMode |= std::ios::in | std::ios::out | std::ios::trunc;
		else if (mode == FILE_APPEND)
				openMode |= std::ios::out | std::ios::app;

		this->stream.open(this->path, openMode);

		if (std::filesystem::is_directory(this->path))
			this->dir_cursor = std::filesystem::directory_iterator(this->path);
	}

	operator bool() const {
		return !this->path.empty();
	}

	File openNextFile(uint8_t mode = 0) {
		if (this->dir_cursor == this->dir_end)
			return File();

		const std::filesystem::directory_entry& entry = *this->dir_cursor;
		const std::string entry_str = entry.path().string();
		++this->dir_cursor;

		return File(entry_str.c_str());
	}

	const char* name() {
		return this->name_str.c_str();
	}

	void close() {
		this->stream.close();
	}

	bool available() {
		return this->stream.peek() != EOF;
	}

	size_t readBytes(char* buffer, size_t length) {
		size_t count = 0;
		for (char c; count < length && this->stream.get(c); buffer[count++] = c);

		return count;
	}

	String readString(size_t length = 120) {
		char* buffer = new char[length];
		this->readBytes(buffer, length);

		String str = String(buffer);
		delete buffer;

		return str;
	}

	size_t readBytesUntil(char terminator, char* buffer, size_t length, std::istream& stream = std::cin) {
		size_t count = 0;
		for (char c; count < length && this->stream.get(c) && c != terminator; buffer[count++] = c);

		return count;
	}

	String readStringUntil(char terminator, size_t length = 120) {
		char* buffer = new char[length];
		this->readBytesUntil(terminator, buffer, length);

		String str = String(buffer);
		delete buffer;

		return str;
	}
};
//const std::filesystem::directory_iterator File::dir_end = std::filesystem::directory_iterator();

class SDClass {
public:
	SDClass() {

	}

	bool begin(uint8_t csPin = 0) {
		return true;
	}

	File open(const char* path, char mode = FILE_WRITE) {
		return File(path, mode);
	}

	bool exists(const char* file_path) {
		return std::filesystem::exists(file_path);
	}

	bool remove(const char* file_path) {
		if (!std::filesystem::is_regular_file(file_path))
			return false;

		std::filesystem::remove(file_path);
		return true;
	}
};

inline SDClass SD = SDClass();

#endif

#endif