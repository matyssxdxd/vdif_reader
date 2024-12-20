#ifndef ISBI_VDIF_STREAM_H
#define ISBI_VDIF_STREAM_H

#include <cstdint>
#include <fstream>
#include <array>
#include <vector>

struct Header {
	uint32_t sec_from_epoch : 30;
	uint32_t legacy_mode : 1;
	uint32_t invalid : 1;

	uint32_t dataframe_in_second : 24;
	uint32_t ref_epoch : 6;
	uint32_t unassigned : 2;

	uint32_t dataframe_length : 24;
	uint32_t log2_nchan : 5;
	uint32_t version : 3;

	uint32_t station_id : 16;
	uint32_t thread_id : 10;
	uint32_t bits_per_sample : 5;
	uint32_t data_type : 1;

	uint32_t user_data_1 : 24;
	uint32_t edv : 8;
	
	uint32_t user_data_2;
	uint32_t user_data_3;
	uint32_t user_data_4;

	Header() : sec_from_epoch(0), legacy_mode(0), invalid(0),
			   dataframe_in_second(0), ref_epoch(0), unassigned(0),
			   dataframe_length(0), log2_nchan(0), version(0), station_id(0),
			   thread_id(0), bits_per_sample(0), data_type(0), user_data_1(0),
			   edv(0), user_data_2(0), user_data_3(0), user_data_4(0) {};
};

class VDIFStream {
private:
	Header header0;
	std::ifstream file;
	uint32_t number_of_headers;
	uint32_t number_of_frames;

	// 2 bit decoding stuff, should probably move to another file
	// Optimal high value for 2-bit reconstruction, taken from baseband vdif source code
	static constexpr float OPTIMAL_2BIT_HIGH = 3.316505f;

	// Look-up table for 2-bit decoding
	static constexpr std::array<float, 4> DECODER_LEVEL = {
		-OPTIMAL_2BIT_HIGH,
		-1.0f,
		1.0f,
		OPTIMAL_2BIT_HIGH
	};

	static std::vector<float> decode_word(uint32_t word);
	static std::vector<float> decode(const std::vector<uint32_t> &words);
public:
	VDIFStream(const std::string &input_file);
	~VDIFStream();
	void read_frame(off_t offset);
	uint32_t bits_per_sample();
	uint32_t number_of_channels();
	uint32_t values_per_word();
	uint32_t payload_nbytes();
	uint32_t samples_per_frame();
	static void print_header(const Header &header);
};

#endif
