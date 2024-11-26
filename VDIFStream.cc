#include "VDIFStream.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <iomanip>

VDIFStream::VDIFStream(const std::string &input_file)
	: file(input_file, std::ios::binary), number_of_headers(0), number_of_frames(0) {
	if (!file.is_open()) {
		throw std::runtime_error("File is not open");
	}

	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char *>(&header0), sizeof(Header));

	print_header(header0);
}

VDIFStream::~VDIFStream() {
	file.close();
}

void VDIFStream::read_frame(off_t offset) {
	std::vector<uint32_t> buffer(payload_nbytes() / sizeof(uint32_t));

	file.seekg(offset, std::ios::beg);
	file.read(reinterpret_cast<char *>(buffer.data()), payload_nbytes());

	std::vector<float> decoded_samples = decode(buffer);

	std::cout << std::fixed << std::setprecision(6);

	for (size_t i = 0; i < decoded_samples.size(); ++i) {
		std::cout << decoded_samples[i] << " ";
		if ((i + 1) % 16 == 0 && i < decoded_samples.size() - 1) {
			std::cout << std::endl;
		}
	}

	std::cout << std::endl;
}

/**
 * Decode a single 32 bit word into 16 samples
 * The input uint32_t word to decode
 * The output vector of 16 floating point samples
 */

std::vector<float> VDIFStream::decode_word(uint32_t word) {
	std::vector<float> samples(16);

	for (int i = 0; i < 16; i++) {
		samples[i] = DECODER_LEVEL[(word >> (i * 2)) & 0b11];
	}

	return samples;
}

/**
 * Decode multiple 32 bit words into samples
 * Vector of uint32_t words to decode
 * Vector of decoded floating point samples
 */

std::vector<float> VDIFStream::decode(const std::vector<uint32_t> &words) {
	std::vector<float> decoded_samples;
	decoded_samples.reserve(words.size() * 16);

	for (uint32_t word : words) {
		auto word_samples = decode_word(word);
		decoded_samples.insert(
				decoded_samples.end(),
				word_samples.begin(),
				word_samples.end()
		);
	}

	return decoded_samples;
}

uint32_t VDIFStream::bits_per_sample() {
	return header0.bits_per_sample + 1;
}

uint32_t VDIFStream::number_of_channels() {
	return 1 << header0.log2_nchan;
}

uint32_t VDIFStream::values_per_word() {
	return 32 / bits_per_sample() / 1;
}

uint32_t VDIFStream::payload_nbytes() {
	return header0.dataframe_length * 8 - 32;
}

uint32_t VDIFStream::samples_per_frame() {
	return payload_nbytes() / 4 * values_per_word() / number_of_channels();
}


void VDIFStream::print_header(const Header &header) {
	std::cout << "sec_from_epoch: "		 << header.sec_from_epoch 		<< "\n";	
	std::cout << "legacy_mode: " 		 << header.legacy_mode 		    << "\n";
    std::cout << "invalid: "			 << header.invalid 			    << "\n";
    std::cout << "dataframe_in_second: " << header.dataframe_in_second  << "\n";
    std::cout << "ref_epoch: " 			 << header.ref_epoch 			<< "\n";
    std::cout << "unassigned: " 		 << header.unassigned 			<< "\n";
    std::cout << "dataframe_length: " 	 << header.dataframe_length 	<< "\n";
    std::cout << "log2_nchan: " 		 << header.log2_nchan 			<< "\n";
    std::cout << "version: " 			 << header.version 				<< "\n";
    std::cout << "station_id: " 		 << header.station_id 			<< "\n";
    std::cout << "thread_id: " 			 << header.thread_id 			<< "\n";
    std::cout << "bits_per_sample: " 	 << header.bits_per_sample 		<< "\n";
    std::cout << "data_type: " 			 << header.data_type 			<< "\n";
    std::cout << "user_data_1: " 		 << header.user_data_1 			<< "\n";
    std::cout << "edv: " 				 << header.edv 					<< "\n";
    std::cout << "user_data_2: " 		 << header.user_data_2 			<< "\n";
    std::cout << "user_data_3: " 		 << header.user_data_3 			<< "\n";
    std::cout << "user_data_4: " 		 << header.user_data_4 			<< "\n";
}    
