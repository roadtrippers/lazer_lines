/*
 * Copyright (C) 2019 HERE Europe B.V.
 * Licensed under MIT, see full license in LICENSE
 * SPDX-License-Identifier: MIT
 * License-Filename: LICENSE
 */
#include <string>
#include <cmath>
#include <array>
#include <stdexcept>
#include <tuple>

namespace hf {
    const uint8_t FORMAT_VERSION=1;

    enum class ThirdDim: int {
        ABSENT = 0,
        LEVEL = 1,
        ALTITUDE = 2,
        ELEVATION = 3,
        /* Reserved values should not be selectable */
        CUSTOM1 = 6,
        CUSTOM2 = 7
    };

    template<typename Fn>
    bool polyline_decode(const std::string &encoded, Fn&& push);
} // namespace hf

namespace encoder {
    const char* ENCODING_TABLE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
    const int8_t DECODING_TABLE[] = {
        62, -1, -1, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, -1,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21,
        22, 23, 24, 25, -1, -1, -1, -1, 63, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
    };

    int8_t decode_char(char char_value) {
        /* Decode a single char to the corresponding value */
        int8_t pos = char_value - 45;
        if (pos < 0 or pos > 77) {
            return -1;
        }
        return DECODING_TABLE[pos];
    }

    class Converter {
        private:
            int64_t m_multiplier = 0;
            int64_t m_last_value = 0;

        public:
            Converter() {}

            Converter(int precision) {
                set_precision(precision);
            }

            void set_precision(int precision) {
                m_multiplier = std::pow(10, precision);
            }

            void inline static encode_unsigned_varint(int64_t value, std::string& result) {
                // var-length encode the number in chunks of 5 bits starting with the least significant
                // to the most significant
                while (value > 0x1F) {
                    uint8_t pos = (value & 0x1F) | 0x20;
                    result.push_back(ENCODING_TABLE[pos]);
                    value >>= 5;
                }
                result.push_back(ENCODING_TABLE[value]);
            }

            void encode_value(double value, std::string& result) {
                int64_t scaled_value = std::llrint(value * m_multiplier);
                int64_t delta = scaled_value - m_last_value;
                bool negative = delta < 0;

                m_last_value = scaled_value;

                // make room on lowest bit
                delta <<= 1;

                // invert bits if the value is negative
                if (negative) {
                    delta = ~delta;
                }

                encode_unsigned_varint(delta, result);
            }

            bool inline static decode_unsigned_varint(const std::string& encoded, uint32_t& index, uint32_t length, int64_t& result) {
                int64_t delta = 0;
                short shift = 0;
                int8_t value = 0;

                while (index < length) {
                    value = decode_char(encoded[index]);
                    if (value < 0) {
                        return false;
                    }

                    index++;

                    delta |= (static_cast<int64_t>(value) & 0x1F) << shift;
                    if ((value & 0x20) == 0) {
                        result = delta;
                        return true;
                    } else {
                        shift += 5;
                    }
                }

                if (shift > 0) {
                    return false;
                }
                return true;
            }

            bool decode_value(const std::string& encoded, uint32_t& index, uint32_t length, double& result) {
                int64_t delta = 0;
                if (!decode_unsigned_varint(encoded, index, length, delta)) {
                    return false;
                }
                if (delta & 1) {
                    delta = ~delta;
                }
                delta >>= 1;
                m_last_value += delta;
                result = static_cast<double>(m_last_value) / m_multiplier;
                return true;
            }
    };

    class Decoder {
        private:
            const std::string m_encoded;
            uint32_t m_index;
            unsigned long m_length;
            int m_precision;
            int m_third_dim_precision;
            hf::ThirdDim m_third_dim;
            Converter m_lat_conv;
            Converter m_lng_conv;
            Converter m_z_conv;

        public:
            Decoder(std::string encoded):
                m_encoded{std::move(encoded)}, m_index{0}, m_length{m_encoded.length()}
            {
                if (m_length == 0) {
                    return;
                }
                decode_header();
                m_lat_conv.set_precision(m_precision);
                m_lng_conv.set_precision(m_precision);
                if (has_third_dimension()) {
                    m_z_conv.set_precision(m_third_dim_precision);
                }
            }

            bool inline has_third_dimension() {
                return m_third_dim != hf::ThirdDim::ABSENT;
            }

            void inline static decode_header_from_string(const std::string& encoded, uint32_t& index, uint32_t length, uint16_t& header) {
                int64_t value = 0;
                /* Decode the header version */
                if(!Converter::decode_unsigned_varint(encoded, index, length, value)) throw std::invalid_argument("Invalid encoding");
                if (value != hf::FORMAT_VERSION) throw std::invalid_argument("Invalid format version");

                /* Decode the polyline header */
                if(!Converter::decode_unsigned_varint(encoded, index, length, value)) throw std::invalid_argument("Invalid encoding");
                header = value;
            }

            void decode_header() {
                uint16_t header = 0;
                decode_header_from_string(m_encoded, m_index, m_length, header);
                m_precision = header & 15;  // we pick the first 3 bits only
                header >>= 4;
                m_third_dim = static_cast<hf::ThirdDim>(header & 7); // we pick the first 4 bits only
                m_third_dim_precision = (header >> 3) & 15;
            }

            bool decode_one(double& lat, double& lng, double& z) {
                if (m_index == m_length) return false;
                if (!m_lat_conv.decode_value(m_encoded, m_index, m_length, lat)) throw std::invalid_argument("Invalid encoding");
                if (!m_lng_conv.decode_value(m_encoded, m_index, m_length, lng)) throw std::invalid_argument("Invalid encoding");
                if (has_third_dimension()) {
                    if (!m_z_conv.decode_value(m_encoded, m_index, m_length, z)) throw std::invalid_argument("Invalid encoding");
                }
                return true;
            }
    };


} // namespace encoder

namespace hf {
    // Implementations

    template<typename Fn>
    bool polyline_decode(const std::string& encoded, Fn&& push) {
        auto dec = encoder::Decoder(encoded);
        bool res;
        double lat=0, lng=0, z=0;
        try {
            while (1) {
                res = dec.decode_one(lat, lng, z);
                if (!res) return true;
                push(lat, lng, z);
            }
        } catch (...) {}
        return false;
    }
} // namespace hf
