#include <iostream>
#include "VDIFStream.h"

int main(int argc, char *argv[]) {
	VDIFStream vdif_stream{argv[1]};
	
	std::cout << "bits_per_sample: " << vdif_stream.bits_per_sample() << "\n";
	std::cout << "number_of_channels: " << vdif_stream.number_of_channels() << "\n";
	std::cout << "values_per_word: " << vdif_stream.values_per_word() << "\n";
	std::cout << "payload_nbytes: " << vdif_stream.payload_nbytes() << "\n";
	std::cout << "samples_per_frame: " << vdif_stream.samples_per_frame() << "\n"; 	
	return 0;
}
