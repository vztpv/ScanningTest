



#ifndef PARSER2_H
#define PARSER2_H

#include <iostream>
#include <vector>
#include <set>
#include <queue>
#include <stack>
#include <string>
#include <cstring>
#include <fstream>

#include <algorithm>
#include <utility>
#include <thread>


#include <intrin.h> // windows, todo : linux - x86intrin

namespace clau_test {

	class PreTokenUtil {
	public:
		static uint16_t upper_part(uint16_t x) {
			return x >> 8;
		}
		static uint16_t lower_part(uint16_t x) {
			return x & 0xFF; // 0b1111 1111
		}
		static uint16_t make_upper(uint16_t x, uint16_t val) {
			x = lower_part(x);
			val = lower_part(val);

			return x + (val << 8);
		}
		static uint16_t make_lower(uint16_t x, uint16_t val) {
			x = upper_part(x);
			val = lower_part(val);

			return x + val;
		}

	};

	using Token = uint64_t;

	inline static uint64_t GetIdx(uint64_t x) {
		return (x >> 32) & 0x00000000FFFFFFFF;
	}
	inline static uint64_t GetLength(uint64_t x) {
		return (x & 0x00000000FFFFFFF0) >> 4;
	}
	inline static uint64_t GetType(uint64_t x) { //to enum or enum class?
		return (x & 0xF);
	}


	enum TokenType {
		NONE = 0, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET, COLON, COMMA,
		STRING, DATA
	};

	inline uint16_t char_to_token_type[256]; // char to pretoken type.
class Utility {
	private:
		class BomInfo
		{
		public:
			size_t bom_size;
			char seq[5];
		};

		const static size_t BOM_COUNT = 1;


		static const BomInfo bomInfo[1];

	public:
		enum class BomType { UTF_8, ANSI };

		static BomType ReadBom(FILE* file) {
			char btBom[5] = { 0, };
			size_t readSize = fread(btBom, sizeof(char), 5, file);


			if (0 == readSize) {
				clearerr(file);
				fseek(file, 0, SEEK_SET);

				return BomType::ANSI;
			}

			BomInfo stBom = { 0, };
			BomType type = ReadBom(btBom, readSize, stBom);

			if (type == BomType::ANSI) { // ansi
				clearerr(file);
				fseek(file, 0, SEEK_SET);
				return BomType::ANSI;
			}

			clearerr(file);
			fseek(file, static_cast<long>(stBom.bom_size * sizeof(char)), SEEK_SET);
			return type;
		}

		static BomType ReadBom(const char* contents, size_t length, BomInfo& outInfo) {
			char btBom[5] = { 0, };
			size_t testLength = length < 5 ? length : 5;
			memcpy(btBom, contents, testLength);

			size_t i, j;
			for (i = 0; i < BOM_COUNT; ++i) {
				const BomInfo& bom = bomInfo[i];

				if (bom.bom_size > testLength) {
					continue;
				}

				bool matched = true;

				for (j = 0; j < bom.bom_size; ++j) {
					if (bom.seq[j] == btBom[j]) {
						continue;
					}

					matched = false;
					break;
				}

				if (!matched) {
					continue;
				}

				outInfo = bom;

				return (BomType)i;
			}

			return BomType::ANSI;
		}


	public:
		static inline bool isWhitespace(const char ch)
		{
			switch (ch)
			{
			case ' ':
			case '\t':
			case '\r':
			case '\n':
			case '\v':
			case '\f':
				return true;
				break;
			}
			return false;
		}


		static inline int Equal(const int64_t x, const int64_t y)
		{
			if (x == y) {
				return 0;
			}
			return -1;
		}

	public:
		static void PrintToken(const char* buffer, Token token) {
			//	std::ofstream outfile("output.txt", std::ios::app);
			//std::cout << TokenUtil::len(token) << " ";
			//std::cout << std::string_view(buffer + TokenUtil::start(token), TokenUtil::len(token)) << "\n";
			//outfile << std::string_view(buffer + token.start, token.len) << "\n";
				//std::cout << Utility::GetIdx(token) << " " << Utility::GetLength(token) << "\n";
				//std::cout << std::string_view(buffer + Utility::GetIdx(token), Utility::GetLength(token));
			//	outfile.close();
		}
	};

	__forceinline static uint64_t Get(uint8_t* text, uint64_t position, uint64_t length, uint8_t type) {
		int64_t i = 0, j = 0;

		// remove whitespace.. , trim(str);
		for (i = position; i < position + length; ++i) {
			if (!Utility::isWhitespace(text[i])) {
				break;
			}
		}
		for (j = position + length - 1; i >= position; --i) {
			if (!Utility::isWhitespace(text[i])) {
				break;
			}
		}

		if (i > j) {
			return 0; // error
		}
		
		position = i;
		length = j - i + 1;

		uint64_t x = (position << 32) + (length << 4) + 0;

		if (length != 1) {
			return x;
		}

		x += type;

		return x;
	}

	
	class InFileReserver
	{
	private:
		static size_t _Scanning(char* text, size_t length, uint16_t* tokens) {
			int a = clock();
			int64_t count = 0;

			char_to_token_type['\"'] = STRING;
			char_to_token_type[','] = COMMA;
			char_to_token_type['{'] = LEFT_BRACE;
			char_to_token_type['['] = LEFT_BRACKET;
			char_to_token_type['}'] = RIGHT_BRACE;
			char_to_token_type[']'] = RIGHT_BRACKET;
			char_to_token_type[':'] = COLON;

			int64_t _i = 0;

			__m256i temp;
			__m256i _1st, _2nd, _3rd, _4th, _5th, _6th, _7th, _8th, _9th, _10th, _11th, _12th, _13th, _14th, _15th, _16th, _17th, _18th, _19th, _20th, _21th, _22th;

			char ch1 = '\"';
			_1st = _mm256_set_epi8(ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1,
				ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1);
			char ch2 = '\\';
			_2nd = _mm256_set_epi8(ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2,
				ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2);
			char ch3 = '{';
			_3rd = _mm256_set_epi8(ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3,
				ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3);
			char ch4 = '[';
			_4th = _mm256_set_epi8(ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4,
				ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4);
			char ch5 = '}';
			_5th = _mm256_set_epi8(ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5,
				ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5);
			char ch6 = ']';
			_6th = _mm256_set_epi8(ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6,
				ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6);
			/*
			char ch7 = 't';
			_7th = _mm256_set_epi8(ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7,
				ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7, ch7);
			char ch8 = 'f';
			_8th = _mm256_set_epi8(ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8,
				ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8, ch8);
			char ch9 = 'n';
			_9th = _mm256_set_epi8(ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9,
				ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9, ch9);
			char ch10 = '-';
			_10th = _mm256_set_epi8(ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10,
				ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10, ch10);

			char ch11 = '0';
			_11th = _mm256_set_epi8(ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11,
				ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11, ch11);

			char ch12 = '1';
			_12th = _mm256_set_epi8(ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12,
				ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12, ch12);

			char ch13 = '2';
			_13th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);

			ch13 = '3';
			_14th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);
			ch13 = '4';
			_15th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);
			ch13 = '5';
			_16th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);
			ch13 = '6';
			_17th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);
			ch13 = '7';
			_18th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);
			ch13 = '8';
			_19th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);
			ch13 = '9';
			_20th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);
			*/
			char ch13 = ':';
			_21th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);
			ch13 = ',';
			_22th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);


			size_t before_idx = 0;
			bool is_first = true;

			// todo check.. _i + 32 >= length... 
			
			for (; _i + 32 < length; _i = _i + 32) {
				temp = _mm256_setr_epi8(text[_i], text[_i + 1], text[_i + 2], text[_i + 3], text[_i + 4], text[_i + 5], text[_i + 6], text[_i + 7],
					text[_i + 8], text[_i + 9], text[_i + 10], text[_i + 11], text[_i + 12], text[_i + 13], text[_i + 14], text[_i + 15], text[_i + 16],
					text[_i + 17], text[_i + 18], text[_i + 19], text[_i + 20], text[_i + 21], text[_i + 22], text[_i + 23], text[_i + 24], text[_i + 25],
					text[_i + 26], text[_i + 27], text[_i + 28], text[_i + 29], text[_i + 30], text[_i + 31]);

				__m256i x1, x2, x3, x4, x5, x6, x7, x8;

				x1 = _mm256_cmpeq_epi8(temp, _1st);
				x2 = _mm256_cmpeq_epi8(temp, _2nd);
				x3 = _mm256_cmpeq_epi8(temp, _3rd);
				x4 = _mm256_cmpeq_epi8(temp, _4th);
				x5 = _mm256_cmpeq_epi8(temp, _5th);
				x6 = _mm256_cmpeq_epi8(temp, _6th);
				x7 = _mm256_cmpeq_epi8(temp, _21th);
				x8 = _mm256_cmpeq_epi8(temp, _22th);

				x1 = _mm256_add_epi8(x1, x2);
				x3 = _mm256_add_epi8(x3, x4);
				x5 = _mm256_add_epi8(x5, x6);
				x7 = _mm256_add_epi8(x7, x8);

				x1 = _mm256_add_epi8(x1, x3);
				x5 = _mm256_add_epi8(x5, x7);
				

				x1 = _mm256_add_epi8(x1, x5);

				int start = 0;
				int r = _mm256_movemask_epi8(x1);

				while (r != 0) {
					{
						int a = _tzcnt_u32(r); //

						r = r & (r - 1);

						start = a;

						const int64_t i = _i + start;

						if (!is_first) {
							size_t diff = i - before_idx;

							
							if (diff > 254) {
								diff = 255; // infinite? - find using simd..
							}

							tokens[count] = PreTokenUtil::make_upper(tokens[count], char_to_token_type[text[i]]);
							tokens[count] = PreTokenUtil::make_lower(tokens[count], 0); // represent end?

							tokens[count] = PreTokenUtil::make_lower(tokens[before_idx], diff);

							before_idx = i;
						}
						else {
							// count == 0
							// first -> using simd to find first token. - text is changed...
							tokens[count] = PreTokenUtil::make_upper(tokens[count], char_to_token_type[text[i]]);
							tokens[count] = PreTokenUtil::make_lower(tokens[count], 0); // represent end?

							before_idx = i;
							is_first = false;
						}

						++count;
					}
				}
			}
			
			int b = clock();
			std::cout << b - a << "ms\n";

			return 0;
		}
		
		static char f(char* ch) {
			return *ch;
		}

		static size_t FindUsingSimd(char* text, const size_t length) {

			char_to_token_type['\"'] = STRING;
			char_to_token_type[','] = COMMA;
			char_to_token_type['{'] = LEFT_BRACE;
			char_to_token_type['['] = LEFT_BRACKET;
			char_to_token_type['}'] = RIGHT_BRACE;
			char_to_token_type[']'] = RIGHT_BRACKET;
			char_to_token_type[':'] = COLON;
		
			int64_t _i = 0;

			__m256i temp;
			__m256i _1st, _2nd, _3rd, _4th, _5th, _6th, _7th, _8th, _9th, _10th, _11th, _12th, _13th, _14th, _15th, _16th, _17th, _18th, _19th, _20th, _21th, _22th;

			char ch1 = '\"';
			_1st = _mm256_set_epi8(ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1,
				ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1, ch1);
			char ch2 = '\\';
			_2nd = _mm256_set_epi8(ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2,
				ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2, ch2);
			char ch3 = '{';
			_3rd = _mm256_set_epi8(ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3,
				ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3, ch3);
			char ch4 = '[';
			_4th = _mm256_set_epi8(ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4,
				ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4, ch4);
			char ch5 = '}';
			_5th = _mm256_set_epi8(ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5,
				ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5, ch5);
			char ch6 = ']';
			_6th = _mm256_set_epi8(ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6,
				ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6, ch6);

			char ch13 = ':';
			_21th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);
			
			ch13 = ',';
			_22th = _mm256_set_epi8(ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13,
				ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13, ch13);

			size_t before_idx = 0;
			bool is_first = true;

			for (; _i + 32 < length; _i = _i + 32) {
				temp = _mm256_setr_epi8(text[_i], text[_i + 1], text[_i + 2], text[_i + 3], text[_i + 4], text[_i + 5], text[_i + 6], text[_i + 7],
					text[_i + 8], text[_i + 9], text[_i + 10], text[_i + 11], text[_i + 12], text[_i + 13], text[_i + 14], text[_i + 15], text[_i + 16],
					text[_i + 17], text[_i + 18], text[_i + 19], text[_i + 20], text[_i + 21], text[_i + 22], text[_i + 23], text[_i + 24], text[_i + 25],
					text[_i + 26], text[_i + 27], text[_i + 28], text[_i + 29], text[_i + 30], text[_i + 31]);

				__m256i x1, x2, x3, x4, x5, x6, x7, x8;

				x1 = _mm256_cmpeq_epi8(temp, _1st);
				x2 = _mm256_cmpeq_epi8(temp, _2nd);
				x3 = _mm256_cmpeq_epi8(temp, _3rd);
				x4 = _mm256_cmpeq_epi8(temp, _4th);
				x5 = _mm256_cmpeq_epi8(temp, _5th);
				x6 = _mm256_cmpeq_epi8(temp, _6th);
				x7 = _mm256_cmpeq_epi8(temp, _21th);
				x8 = _mm256_cmpeq_epi8(temp, _22th);

				x1 = _mm256_add_epi8(x1, x2);
				x3 = _mm256_add_epi8(x3, x4);
				x5 = _mm256_add_epi8(x5, x6);
				x7 = _mm256_add_epi8(x7, x8);

				x1 = _mm256_add_epi8(x1, x3);
				x5 = _mm256_add_epi8(x5, x7);

				x1 = _mm256_add_epi8(x1, x5);

				int start = 0;
				int r = _mm256_movemask_epi8(x1);

				if (r != 0) {
					{
						int a = _tzcnt_u32(r); //

						r = r & (r - 1);

						start = a;

						const int64_t i = _i + start;
						
						return i;
					}
				}
			}

			return 0; // ERROR_CODE ?
		}

		static void ScanningNew(char* text, const size_t length, const int thr_num,
			Token*& _token_arr, size_t& _token_arr_size, bool use_simd)
		{
			std::vector<std::thread> thr(thr_num);
			std::vector<size_t> start(thr_num);
			std::vector<size_t> last(thr_num);

			{
				start[0] = 0;

				for (int i = 1; i < thr_num; ++i) {
					start[i] = length / thr_num * i;

					for (size_t x = start[i]; x <= length; ++x) {
						if (Utility::isWhitespace(text[x]) || '\0' == text[x] ||
							LEFT_BRACKET == text[x] || RIGHT_BRACKET == text[x] ||
							LEFT_BRACE == text[x] || RIGHT_BRACE == text[x] || COLON == text[x]) {
							start[i] = x;
							break;
						}
					}
				}
				for (int i = 0; i < thr_num - 1; ++i) {
					last[i] = start[i + 1];
				}

				last[thr_num - 1] = length + 1;
			}
			size_t real_token_arr_count = 0;

			size_t tokens_max = (32 + (length + 1) / thr_num) * thr_num;

			uint16_t* tokens = (uint16_t*)calloc(length, sizeof(uint16_t));

			int64_t token_count = 0;
			int64_t pre_token_count = 0; // todo? - std::vector<int64_t> ?

			std::vector<size_t> token_arr_size(thr_num);
			auto a = std::chrono::steady_clock::now();
			
			for (int i = 0; i < thr_num; ++i) {
				thr[i] = std::thread(_Scanning, text + start[i], last[i] - start[i], tokens + start[i]);
			}

			for (int i = 0; i < thr_num; ++i) {
				thr[i].join();
			}

			auto b = std::chrono::steady_clock::now();

			Token* real_tokens = (Token*)calloc(length, sizeof(Token));

			token_count = 0;
			
			for (int i = 0; i < thr_num; ++i) { 
				
				size_t a = start[i];

				int j = 0;
				
				// first find using simd.
				size_t pre_token_start = start[i] + FindUsingSimd(text + start[i], last[i] - start[i]);
				

				if (pre_token_start > start[i]) {
					real_tokens[token_count] = Get((uint8_t*)text, start[i], pre_token_start - start[i], DATA);
					token_count++;
				}
				
				a = pre_token_start; // first token start idx!
				
				//std::cout << "a : " << a << "\t";
				//std::cout << "start : " << start[i] << " " << "last : " << last[i] << "\n";
				while (a < last[i]) {
					int64_t diff = PreTokenUtil::lower_part(tokens[start[i] + j]);

					//std::cout << "diff " << (int)diff << "\n";

					if (diff == 255) { // infinity?
						std::cout << "diff is over 255" << " " << a + 1 << "\t";

						diff = 1 + FindUsingSimd(text + a + 1, last[i] - a - 1);
						
						std::cout << (int)diff << "\n";
					}

					
					//std::cout << (int)diff << " , a : " << a << "\n";

					if (diff == 0) {
						break;
					}

					real_tokens[token_count] = Get((uint8_t*)text, a, 1, PreTokenUtil::upper_part(tokens[start[i] + j]));
					token_count++;
					
					a = a + diff; // next token start idx!
					
					++j;
				}

				//std::cout << "test " << a << "\n";
			
			//std::cout << "token count " << token_count << "\n";
			}
			free(tokens);

			auto c = std::chrono::steady_clock::now();
			auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(b - a);
			auto dur2 = std::chrono::duration_cast<std::chrono::milliseconds>(c - b);

			std::cout << "pre " << dur.count() << "ms\n";
			std::cout << "scan " << dur2.count() << "ms\n";

			//for (int i = 0; i < real_token_arr_count; ++i) {
			//	Utility::PrintToken(text, tokens[i]);
			//}

			{
				_token_arr = real_tokens;
				_token_arr_size = real_token_arr_count;
			}
		}

		static std::pair<bool, int> Scan(FILE* inFile, int thr_num,
			char*& _buffer, size_t* _buffer_len, Token*& _token_arr, size_t* _token_arr_len, bool use_simd)
		{
			if (inFile == nullptr) {
				return { false, 0 };
			}

			int64_t* arr_count = nullptr; //
			size_t arr_count_size = 0;

			//std::string temp;
			char* buffer = nullptr;
			size_t file_length;

			{
				fseek(inFile, 0, SEEK_END);
				size_t length = ftell(inFile);
				fseek(inFile, 0, SEEK_SET);

				Utility::BomType x = Utility::ReadBom(inFile);

				//	clau_parser11::Out << "length " << length << "\n";
				if (x == Utility::BomType::UTF_8) {
					length = length - 3;
				}

				file_length = length;
				buffer = new char[file_length + 1]; // 

				int a = clock();
				// read data as a block:
				fread(buffer, sizeof(char), file_length, inFile);
				int b = clock();
				std::cout << "load file " << b - a << "ms " << file_length << "\n";

				buffer[file_length] = '\0';

				{
					Token* token_arr;
					size_t token_arr_size;

					{
						ScanningNew(buffer, file_length, thr_num, token_arr, token_arr_size, use_simd);
					}

					_buffer = buffer;
					_token_arr = token_arr;
					*_token_arr_len = token_arr_size;
					*_buffer_len = file_length;
				}
			}

			return{ true, 1 };
		}

	private:
		FILE* pInFile;
		bool use_simd;
	public:
		explicit InFileReserver(FILE* inFile, bool use_simd)
		{
			pInFile = inFile;
			this->use_simd = use_simd;
		}
	public:
		bool operator() (int thr_num, char*& buffer, size_t* buffer_len, Token*& token_arr, size_t* token_arr_len)
		{
			bool x = Scan(pInFile, thr_num, buffer, buffer_len, token_arr, token_arr_len, use_simd).second > 0;

			return x;
		}
	};
	class LoadData
	{

	public:
		static bool LoadDataFromFile(const std::string& fileName, int lex_thr_num = 1, int parse_thr_num = 1, bool use_simd = false) /// global should be empty
		{
			if (lex_thr_num <= 0) {
				lex_thr_num = std::thread::hardware_concurrency();
			}
			if (lex_thr_num <= 0) {
				lex_thr_num = 1;
			}

			if (parse_thr_num <= 0) {
				parse_thr_num = std::thread::hardware_concurrency();
			}
			if (parse_thr_num <= 0) {
				parse_thr_num = 1;
			}

			int a = clock();

			bool success = true;
			FILE* inFile;

#ifdef _WIN32 
			fopen_s(&inFile, fileName.c_str(), "rb");
#else
			inFile = fopen(fileName.c_str(), "rb");
#endif

			if (!inFile)
			{
				return false;
			}

			try {

				InFileReserver ifReserver(inFile, use_simd);
				char* buffer = nullptr;
				size_t buffer_len, token_arr_len;
				Token* token_arr;

				ifReserver(lex_thr_num, buffer, &buffer_len, token_arr, &token_arr_len);


				int b = clock();

				std::cout << "total " <<  b - a << "ms\n";

				fclose(inFile);

				delete[] buffer;
				free(token_arr);


			}
			catch (const char* err) { std::cout << err << "\n"; fclose(inFile); return false; }
			catch (const std::string& e) { std::cout << e << "\n"; fclose(inFile); return false; }
			catch (const std::exception& e) { std::cout << e.what() << "\n"; fclose(inFile); return false; }
			catch (...) { std::cout << "not expected error" << "\n"; fclose(inFile); return false; }

			return true;
		}

	};
}



#endif



