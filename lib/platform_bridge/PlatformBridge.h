#ifndef PLATFORM_BRIDGE_H
#define PLATFORM_BRIDGE_H

#if defined(TARGET_TEENY41)
#include <Arduino.h>
#include <SD.h>
#elif defined(TARGET_NATIVE)
#include <iostream>
#include <chrono>
#include <thread>
#include <istream>
#include <fstream>
#include <filesystem>
#include <cstring>
#endif

#if defined(TARGET_TEENY41) || defined(TARGET_NATIVE)
#include <vector>
#include <string>
#include <functional>
#endif

/*
 * PlatformBridge.h
 *
 * Created on: 2025-06-30 by Ethan Chen
 * Maintained by Ethan Chen
 * Description: This file acts as an abstraction layer for the portions of the API used across multiple platforms
 */

// add csv and implement for tadpole sensors
// Make everything use CStrings if possible, but otherwise compensate with std::string derived class (yes ik it's bad practice)
// Move to multithreading (complete data no pausing unless explicitly required otherwise use fbo like buffer switching system and deprioritizing recentness of data)

#if defined(TARGET_TEENY41)

// Not required, but list the portions of the APIs used here so we can keep track of things and move it to another namespace if needed
using ::File;
using ::Serial;
using ::delay;
using ::SD;
using ::String;

#define PLATFORM_INIT()

#elif defined(TARGET_NATIVE)

class String : public std::string {
public:
	String() : std::string() {}
	String(const std::string& str) : std::string(str) {}
	String(const char* s) : std::string(s) {}
	String(const std::string& str, size_t pos, size_t len = std::string::npos) : std::string(str, pos, len) {}

	void trim() {
		size_t start = 0;
		for (;start < this->size() && std::isspace((*this)[start]); ++start);
		size_t end = this->size();
		for (;end > start && std::isspace((*this)[end - 1]); --end);

		this->erase(end);
		this->erase(0, start);
	}
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

	size_t write(const char* msg, size_t len) {
		std::cout.write(msg, len);
		return len;
	}

	void flush() {
		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	size_t readBytes(char* buffer, size_t length) {
		size_t count = 0;
		for (char c; count < length && std::cin.get(c); buffer[count++] = c);
		buffer[count] = '\0';

		return count;
	}

	String readString(size_t length = 120) {
		char* buffer = new char[length];
		this->readBytes(buffer, length);

		String str = String(buffer);
		delete buffer;

		return str;
	}

	size_t readBytesUntil(char terminator, char* buffer, size_t length) {
		size_t count = 0;
		for (char c; count < length && std::cin.get(c) && c != terminator; buffer[count++] = c);
		buffer[count] = '\0';

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
		this->path = std::filesystem::path(BUILTIN_SDCARD_DIR);
		this->path /= path_str + (path_str[0] == '/' ? 1 : 0);
		this->name_str = this->path.filename().string();
		this->mode = mode;

		std::ios_base::openmode openMode = std::ios::binary;
		if (mode == FILE_READ)
				openMode |= std::ios::in;
		else if (mode == FILE_WRITE)
				openMode |= std::ios::in | std::ios::out;

		if (mode == FILE_WRITE) {
			std::ofstream createFile(this->path);
			createFile.close();
		}

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

	template <typename T>
	size_t print(T in) {
		this->stream.seekp(0, std::ios::end);
		std::string str = in;
		this->stream << in;
		return str.size();
	}

	template <typename T>
	size_t println(T in) {
		this->stream.seekp(0, std::ios::end);
		std::string str = in;
		this->stream << in << std::endl;
		return str.size();
	}

	void flush() {
		this->stream.flush();
	}

	size_t readBytes(char* buffer, size_t length) {
		size_t count = 0;
		for (char c; count < length && this->stream.get(c); buffer[count++] = c);
		buffer[count] = '\0';

		return count;
	}

	String readString(size_t length = 120) {
		char* buffer = new char[length];
		buffer[this->readBytes(buffer, length)] = '\0';

		String str = String(buffer);
		delete buffer;

		return str;
	}

	size_t readBytesUntil(char terminator, char* buffer, size_t length, std::istream& stream = std::cin) {
		size_t count = 0;
		for (char c; count < length && this->stream.get(c) && c != terminator; buffer[count++] = c);
		buffer[count] = '\0';

		return count;
	}

	String readStringUntil(char terminator, size_t length = 120) {
		char* buffer = new char[length];
		buffer[this->readBytesUntil(terminator, buffer, length)] = '\0';

		String str = String(buffer);
		delete buffer;

		return str;
	}
};

class SDClass {
public:
	SDClass() {

	}

	bool begin(uint8_t csPin = 0) {
		return true;
	}

	File open(const char* path_str, char mode = FILE_WRITE) {
		return File(path_str, mode);
	}

	bool exists(const char* file_path_str) {
		std::filesystem::path file_path = std::filesystem::path(BUILTIN_SDCARD_DIR);
		file_path /= file_path_str + (file_path_str[0] == '/' ? 1 : 0);
		return std::filesystem::exists(file_path);
	}

	bool remove(const char* file_path_str) {
		std::filesystem::path file_path = std::filesystem::path(BUILTIN_SDCARD_DIR);
		file_path /= file_path_str + (file_path_str[0] == '/' ? 1 : 0);
		if (!std::filesystem::is_regular_file(file_path))
			return false;

		std::filesystem::remove(file_path);
		return true;
	}
};

inline SDClass SD = SDClass();

#define PLATFORM_INIT() \
	int main() { \
		if (!std::filesystem::is_directory(BUILTIN_SDCARD_DIR)) { \
			try { \
				std::filesystem::create_directory(BUILTIN_SDCARD_DIR); \
			} \
			catch (const std::exception& exc) { \
				std::cout << "Failed to initialize BUILTIN_SDCARD_DIR and start the program becuase of the following error:" << exc.what() << std::endl; \
				return 1; \
			}\
		} \
		setup(); \
		while (true) \
			loop(); \
		return 0; \
	}

#endif

#endif