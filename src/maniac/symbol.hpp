#pragma once

#include <vector>
#include <assert.h>
#include "util.hpp"
#include "chance.hpp"

template <typename RAC> class UniformSymbolCoder
{
private:
    RAC &rac;

public:
    UniformSymbolCoder(RAC &racIn) : rac(racIn) { }

    void write_int(int min, int max, int val) {
        assert(max >= min);
        if (min != 0) {
            max -= min;
            val -= min;
        }
        if (max == 0) return;

        // split in [0..med] [med+1..max]
        int med = max/2;
        if (val > med) {
            rac.write_bit(true);
            write_int(med+1, max, val);
        } else {
            rac.write_bit(false);
            write_int(0, med, val);
        }
        return;
    }

    int read_int(int min, int max) {
        assert(max >= min);
        if (min != 0) {
            max -= min;
        }
        if (max == 0) return min;

        // split in [0..med] [med+1..max]
        int med = max/2;
        bool bit = rac.read_bit();
        if (bit) {
            return read_int(min+med+1, min+max);
        } else {
            return read_int(min, min+med);
        }
    }
};

typedef enum {
    BIT_ZERO,
    BIT_SIGN,
    BIT_EXP,
    BIT_MANT,
//    BIT_EXTRA
} SymbolChanceBitType;

//static const char *SymbolChanceBitName[] = {"zero", "sign", "expo", "mant"};

static const uint16_t EXP_CHANCES[] = {3200, 2800, 2600, 2400, 2000, 1500, 800, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300, 300};
static const uint16_t MANT_CHANCES[] = {1800, 1800, 1800, 1700, 1600, 1200, 1000, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800, 800};
static const uint16_t ZERO_CHANCE = 1500;
static const uint16_t SIGN_CHANCE = 2048;

#ifdef STATS
struct SymbolChanceStats
{
    BitChanceStats stats_zero;
    BitChanceStats stats_sign;
    BitChanceStats stats_exp[31];
    BitChanceStats stats_mant[32];

    std::string format() const;
    ~SymbolChanceStats();
};

extern SymbolChanceStats global_symbol_stats;
#endif

template <typename BitChance, int bits> class SymbolChance
{
    BitChance bit_zero;
    BitChance bit_sign;
    BitChance bit_exp[bits-1];
    BitChance bit_mant[bits];

public:

    BitChance inline &bitZero()      {
        return bit_zero;
    }

    BitChance inline &bitSign()      {
        return bit_sign;
    }

    // all exp bits 0         -> int(log2(val)) == 0  [ val == 1 ]
    // exp bits up to i are 1 -> int(log2(val)) == i+1
    BitChance inline &bitExp(int i)  {
        assert(i >= 0 && i < bits-1);
        return bit_exp[i];
    }
    BitChance inline &bitMant(int i) {
        assert(i >= 0 && i < bits);
        return bit_mant[i];
    }

    BitChance inline &bit(SymbolChanceBitType typ, int i = 0) {
        switch (typ) {
        default:
        case BIT_ZERO:
            return bitZero();
        case BIT_SIGN:
            return bitSign();
        case BIT_EXP:
            return bitExp(i);
        case BIT_MANT:
            return bitMant(i);
        }
    }
    SymbolChance() { // : bit_exp(bitsin-1), bit_mant(bitsin) {
        bitZero().set_12bit(ZERO_CHANCE);
        bitSign().set_12bit(SIGN_CHANCE);
//        printf("bits: %i\n",bits);
        for (int i=0; i<bits-1; i++) {
            bitExp(i).set_12bit(EXP_CHANCES[i]);
        }
        for (int i=0; i<bits; i++) {
            bitMant(i).set_12bit(MANT_CHANCES[i]);
        }
    }

    int scale() const {
        return bitZero().scale();
    }

#ifdef STATS
    ~SymbolChance() {
        global_symbol_stats.stats_zero += bit_zero.stats();
        global_symbol_stats.stats_sign += bit_sign.stats();
        for (int i = 0; i < bits - 1 && i < 31; i++) {
            global_symbol_stats.stats_exp[i] += bit_exp[i].stats();
        }
        for (int i = 0; i < bits && i < 32; i++) {
            global_symbol_stats.stats_mant[i] += bit_mant[i].stats();
        }
    }
#endif
};

template <typename SymbolCoder> int reader(SymbolCoder& coder, int bits)
{
  int pos=0;
  int value=0;
  int b=1;
  while (pos++ < bits) {
    if (coder.read(BIT_MANT, pos)) value += b;
    b *= 2;
  }
  return value;
}

template <int bits, typename SymbolCoder> int reader(SymbolCoder& coder, int min, int max)
{
    assert(min<=max);
    if (min == max) return min;

    bool sign;
    if (max >= 0 && min <= 0) {
      if (coder.read(BIT_ZERO)) return 0;
      if (min < 0) {
        if (max > 0) {
                sign = coder.read(BIT_SIGN);
                if (sign) min = 1; else max = -1;
        } else {sign = false; max=-1;}
      } else {sign = true; min=1;}
//      if (min == max) return min; // nodig?
    } else {
        if (min<0) sign = false;
        else sign = true;
    }


//    if (sign && min <= 0) min = 1;
//    if (!sign && max >= 0) max = -1;

    int amin = (sign? min : -max);
    int amax = (sign? max : -min);

    int emax = maniac::util::ilog2(amax);
    int i = maniac::util::ilog2(amin);

    for (; i < emax; i++) {
        // if exponent >i is impossible, we are done
        if ((1 << (i+1)) > amax) break;
        if (coder.read(BIT_EXP,i)) break;
    }
    int e = i;
    int have = (1 << e);
    int left = have-1;
    for (int pos = e; pos>0;) {
        int bit = 1;
        left ^= (1 << (--pos));
        int minabs1 = have | (1<<pos);
        int maxabs0 = have | left;
        if (minabs1 > amax) { // 1-bit is impossible
            bit = 0;
        } else if (maxabs0 >= amin) { // 0-bit and 1-bit are both possible
            bit = coder.read(BIT_MANT,pos);
        }
        have |= (bit << pos);
    }
    return (sign ? have : -have);
}
/*
template <typename SymbolCoder> int reader_p(SymbolCoder& coder, int min, int max, int *p)
{
  int r = reader(coder, min, max);
  *p = coder.read(BIT_EXTRA);
//  printf("read parity %i",p);
  return r;
}
*/

#ifdef HAS_ENCODER
template <typename SymbolCoder> void writer(SymbolCoder& coder, int bits, int value)
{
  int pos=0;
  while (pos++ < bits) {
    coder.write(value&1, BIT_MANT, pos);
    value >>= 1;
  }
}

template <int bits, typename SymbolCoder> void writer(SymbolCoder& coder, int min, int max, int value)
{
    assert(min<=max);
    assert(value>=min);
    assert(value<=max);

    // avoid doing anything if the value is already known
    if (min == max) return;

    if (value == 0) { // value is zero
        coder.write(true, BIT_ZERO);
        return;
    }

    // only output zero bit if value could also have been zero
    if (max >= 0 && min <= 0) coder.write(false,BIT_ZERO);
    int sign = (value > 0 ? 1 : 0);
    if (max > 0 && min < 0) {
        // only output sign bit if value can be both pos and neg
        if (min < 0 && max > 0) coder.write(sign,BIT_SIGN);
    }
    if (sign && min <= 0) min = 1;
    if (!sign && max >= 0) max = -1;
    const int a = abs(value);
    const int e = maniac::util::ilog2(a);
    int amin = sign ? abs(min) : abs(max);
    int amax = sign ? abs(max) : abs(min);

    int emax = maniac::util::ilog2(amax);
    int i = maniac::util::ilog2(amin);

    while (i < emax) {
        // if exponent >i is impossible, we are done
        if ((1 << (i+1)) > amax) break;
        // if exponent i is possible, output the exponent bit
        coder.write(i==e, BIT_EXP, i);
        if (i==e) break;
        i++;
    }
//  e_printf("exp=%i\n",e);
    int have = (1 << e);
    int left = have-1;
    for (int pos = e; pos>0;) {
        int bit = 1;
        left ^= (1 << (--pos));
        int minabs1 = have | (1<<pos);
        // int maxabs1 = have | left | (1<<pos);
        // int minabs0 = have;
        int maxabs0 = have | left;
        if (minabs1 > amax) { // 1-bit is impossible
            bit = 0;
        } else if (maxabs0 >= amin) { // 0-bit and 1-bit are both possible
            bit = (a >> pos) & 1;
            coder.write(bit, BIT_MANT, pos);
        }
        have |= (bit << pos);
    }
}
#endif

template <typename BitChance, typename RAC> class SimpleBitCoder
{
    typedef typename BitChance::Table Table;

private:
    const Table table;
    BitChance ctx;
    RAC &rac;

public:
    SimpleBitCoder(RAC &racIn, int cut = 2, int alpha = 0xFFFFFFFF / 19) : rac(racIn), table(cut,alpha) {}

    void set(uint16_t chance) {
        ctx.set(chance);
    }

    void write(bool bit) {
        rac.write(ctx.get(), bit);
        ctx.put(bit, table);
    }

    bool read() {
        bool bit = rac.read(ctx.get());
        ctx.put(bit, table);
        return bit;
    }
};

template <typename BitChance, typename RAC, int bits> class SimpleSymbolBitCoder
{
    typedef typename BitChance::Table Table;

private:
    const Table &table;
    SymbolChance<BitChance,bits> &ctx;
    RAC &rac;

public:
    SimpleSymbolBitCoder(const Table &tableIn, SymbolChance<BitChance,bits> &ctxIn, RAC &racIn) : table(tableIn), ctx(ctxIn), rac(racIn) {}

    void write(bool bit, SymbolChanceBitType typ, int i = 0) {
        BitChance& bch = ctx.bit(typ, i);
        rac.write_12bit_chance(bch.get_12bit(), bit);
        bch.put(bit, table);
//    e_printf("bit %s%i = %s\n", SymbolChanceBitName[typ], i, bit ? "true" : "false");
    }

    bool read(SymbolChanceBitType typ, int i = 0) {
        BitChance& bch = ctx.bit(typ, i);
        bool bit = rac.read_12bit_chance(bch.get_12bit());
        bch.put(bit, table);
//    e_printf("bit %s%i = %s\n", SymbolChanceBitName[typ], i, bit ? "true" : "false");
        return bit;
    }
};

template <typename BitChance, typename RAC, int bits> class SimpleSymbolCoder
{
    typedef typename BitChance::Table Table;

private:
    SymbolChance<BitChance,bits> ctx;
    const Table table;
    RAC &rac;

public:
    SimpleSymbolCoder(RAC& racIn, int cut = 2, int alpha = 0xFFFFFFFF / 19) :  table(cut,alpha), rac(racIn) {
    }

#ifdef HAS_ENCODER
    void write_int(int min, int max, int value) {
        SimpleSymbolBitCoder<BitChance, RAC, bits> bitCoder(table, ctx, rac);
        writer<bits, SimpleSymbolBitCoder<BitChance, RAC, bits> >(bitCoder, min, max, value);
    }
#endif

    int read_int(int min, int max) {
        SimpleSymbolBitCoder<BitChance, RAC, bits> bitCoder(table, ctx, rac);
        return reader<bits, SimpleSymbolBitCoder<BitChance, RAC, bits>>(bitCoder, min, max);
    }

#ifdef HAS_ENCODER
    void write_int(int nbits, int value) {
        assert (nbits <= bits);
        SimpleSymbolBitCoder<BitChance, RAC, bits> bitCoder(table, ctx, rac);
        writer(bitCoder, nbits, value);
    }
#endif

    int read_int(int nbits) {
        assert (nbits <= bits);
        SimpleSymbolBitCoder<BitChance, RAC, bits> bitCoder(table, ctx, rac);
        return reader(bitCoder, nbits);
    }
};
