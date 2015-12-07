/*
 FLIF encoder - Free Lossless Image Format
 Copyright (C) 2010-2015  Jon Sneyers & Pieter Wuille, GPL v3+

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

template <class Config, typename IO> class RacOutput {
public:
    typedef typename Config::data_t rac_t;
protected:
    IO& io;
private:
    rac_t range;
    rac_t low;
    int delayed_byte;
    int delayed_count;

    void inline output() {
        while (range <= Config::MIN_RANGE) {
            int byte = low >> Config::MIN_RANGE_BITS;
            if (delayed_byte < 0) { // first generated byte
                delayed_byte = byte;
            } else if (((low + range) >> 8) < Config::MIN_RANGE) { // definitely no overflow
                io.fputc(delayed_byte);
                while (delayed_count) {
                    io.fputc(0xFF);
                    delayed_count--;
                }
                delayed_byte = byte;
            } else if ((low >> 8) >= Config::MIN_RANGE) { // definitely overflow
                io.fputc(delayed_byte + 1);
                while (delayed_count) {
                    io.fputc(0);
                    delayed_count--;
                }
                delayed_byte = byte & 0xFF;
            } else {
                delayed_count++;
            }
            low = (low & (Config::MIN_RANGE - 1)) << 8;
            range <<= 8;
        }
    }
    void inline put(rac_t chance, bool bit) {
        assert(chance > 0);
        assert(chance < range);
        if (bit) {
            low += range - chance;
            range = chance;
        } else {
            range -= chance;
        }
        output();
    }
public:
    RacOutput(IO& ioin) : io(ioin), range(Config::BASE_RANGE), low(0), delayed_byte(-1), delayed_count(0) { }

    void inline write_12bit_chance(uint16_t b12, bool bit) {
        put(Config::chance_12bit_chance(b12, range), bit);
    }

    void inline write_bit(bool bit) {
        put(range >> 1, bit);
    }

    void inline flush() {
        low += (Config::MIN_RANGE - 1);
        // is this the correct way to reliably flush?
          range = Config::MIN_RANGE - 1;
          output();
          range = Config::MIN_RANGE - 1;
          output();
          range = Config::MIN_RANGE - 1;
          output();
          range = Config::MIN_RANGE - 1;
          output();
        io.flush();
    }
};


class RacDummy {
public:
    static void inline write_12bit_chance(uint16_t b12, bool) { }
    static void inline write_bit(bool) { }
    static void inline flush() { }
};


template <typename IO> class RacOutput24 : public RacOutput<RacConfig24, IO> {
public:
    RacOutput24(IO& io) : RacOutput<RacConfig24, IO>(io) { }
};