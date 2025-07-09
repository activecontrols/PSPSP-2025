#ifndef PLATFORM_BRIDGE_TEMPLATE
#define PLATFORM_BRIDGE_TEMPLATE

#if defined(TARGET_TEENY41)


#elif defined(TARGET_NATIVE)

template <typename T>
size_t usb_serial_class::print(T in) {
	std::string str = in;
	std::cout << in;
	return str.size();
}
template <typename T>
size_t usb_serial_class::println(T in) {
	std::string str = in;
	std::cout << in << std::endl;
	return str.size();
}


template <typename T>
size_t File::print(T in) {
	this->stream.seekp(0, std::ios::end);
	std::string str = in;
	this->stream << in;
	return str.size();
}

template <typename T>
size_t File::println(T in) {
	this->stream.seekp(0, std::ios::end);
	std::string str = in;
	this->stream << in << std::endl;
	return str.size();
}

#endif

#endif