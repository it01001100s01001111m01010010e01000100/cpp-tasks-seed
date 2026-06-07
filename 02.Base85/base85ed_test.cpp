#include <gtest/gtest.h>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include "base85ed.h"

using namespace std;


// переводим обычную строку в вектор байтов
static vector<uint8_t> str_to_vec(string const &s)
{
    return vector<uint8_t>(s.begin(), s.end());
}

// переводим вектор байтов обратно в строку
// это удобно, когда надо руками посмотреть результат в отладке
[[maybe_unused]] static string vec_to_str(vector<uint8_t> const &v)
{
    return string(v.begin(), v.end());
}

// делаем вектор со значениями  идущеми подряд
// нужен для бинарных тестов, где проверяются не только текстовые данные
static vector<uint8_t> make_range_vector(int from, int to)
{
    vector<uint8_t> result;

    for (int i = from; i <= to; i++)
    {
        result.push_back((uint8_t)i);
    }

    return result;
}


// проверяем только кодирование строки
static void expect_encode_string(string const &decoded, string const &encoded)
{
    EXPECT_EQ(base85::encode(str_to_vec(decoded)), str_to_vec(encoded));
}


// проверяем только декодирование строки
static void expect_decode_string(string const &encoded, string const &decoded)
{
    EXPECT_EQ(base85::decode(str_to_vec(encoded)), str_to_vec(decoded));
}


// проверяем кодирование и декодирование
static void expect_pair_string(string const &decoded, string const &encoded)
{
    expect_encode_string(decoded, encoded);
    expect_decode_string(encoded, decoded);
}

TEST(Base85Encode, EmptyString)
{
    vector<uint8_t> empty;

    EXPECT_TRUE(base85::encode(empty).empty());
}


TEST(Base85Decode, EmptyString)
{
    vector<uint8_t> empty;

    EXPECT_TRUE(base85::decode(empty).empty());
}

TEST(Base85ShortCases, EncodeSmallStrings)
{
    expect_encode_string("", "");
    expect_encode_string("1", "F#");
    expect_encode_string("12", "F){");
    expect_encode_string("123", "F)}j");
    expect_encode_string("1234", "F)}kW");
}

TEST(Base85ShortCases, DecodeSmallStrings)
{
    expect_decode_string("", "");
    expect_decode_string("F#", "1");
    expect_decode_string("F){", "12");
    expect_decode_string("F)}j", "123");
    expect_decode_string("F)}kW", "1234");
}

TEST(Base85ShortCases, EncodeAndDecodeSmallStringsTogether)
{
    expect_pair_string("", "");
    expect_pair_string("1", "F#");
    expect_pair_string("12", "F){");
    expect_pair_string("123", "F)}j");
    expect_pair_string("1234", "F)}kW");
    expect_pair_string("12345", "F)}kWH2");
}

TEST(Base85TextCases, RussianText)
{
    expect_pair_string("Привет", "(4WzO(74dD(6!Nm");
    expect_pair_string("Привет, мир!", "(4WzO(74dD(6!NmEFjRl(74fnAp");
    expect_pair_string("Тест", "(4x?_(Sgx|");
    expect_pair_string("студенческий тест", "(Sgx|(Sy*m(6!LL(TC8r(Sgvq(74dKAkl)*wb6mmf&");
}

TEST(Base85TextCases, LongRussianText)
{
    expect_pair_string(
        "Съешь ещё этих мягких французских булок, да выпей чаю.",
        "(4o<a(6!Ns(TpI_wb6;uks#5H(Sp#p(S;z;ywQ))v(UQGxY30m(S*@}(6G?G(T353(6`Zn(7MpL(S;z;vC)IjyU@PSx-1~jw9v31(6Z5s(7({N(77PdhtROmjxG"
    );
}

TEST(Base85ZeroBytes, EncodeZeroBytes)
{
    vector<uint8_t> one_zero = {0x00};
    vector<uint8_t> two_zeroes = {0x00, 0x00};
    vector<uint8_t> three_zeroes = {0x00, 0x00, 0x00};
    vector<uint8_t> four_zeroes = {0x00, 0x00, 0x00, 0x00};

    EXPECT_EQ(base85::encode(one_zero), str_to_vec("00"));
    EXPECT_EQ(base85::encode(two_zeroes), str_to_vec("000"));
    EXPECT_EQ(base85::encode(three_zeroes), str_to_vec("0000"));
    EXPECT_EQ(base85::encode(four_zeroes), str_to_vec("00000"));
}

TEST(Base85ZeroBytes, DecodeZeroBytes)
{
    vector<uint8_t> one_zero = {0x00};
    vector<uint8_t> two_zeroes = {0x00, 0x00};
    vector<uint8_t> three_zeroes = {0x00, 0x00, 0x00};
    vector<uint8_t> four_zeroes = {0x00, 0x00, 0x00, 0x00};

    EXPECT_EQ(base85::decode(str_to_vec("00")), one_zero);
    EXPECT_EQ(base85::decode(str_to_vec("000")), two_zeroes);
    EXPECT_EQ(base85::decode(str_to_vec("0000")), three_zeroes);
    EXPECT_EQ(base85::decode(str_to_vec("00000")), four_zeroes);
}

TEST(Base85BinaryCases, FourMaxBytes)
{
    vector<uint8_t> data = {0xFF, 0xFF, 0xFF, 0xFF};

    EXPECT_EQ(base85::encode(data), str_to_vec("|NsC0"));
    EXPECT_EQ(base85::decode(str_to_vec("|NsC0")), data);
}

TEST(Base85BinaryCases, FirstSixteenBytes)
{
    vector<uint8_t> data = make_range_vector(0, 15);

    string encoded = "009C61O)~M2nh-c3=Iws";

    EXPECT_EQ(base85::encode(data), str_to_vec(encoded));
    EXPECT_EQ(base85::decode(str_to_vec(encoded)), data);
}

TEST(Base85BinaryCases, FirstThirtyTwoBytes)
{
    vector<uint8_t> data = make_range_vector(0, 31);

    string encoded = "009C61O)~M2nh-c3=Iws5D^j+6crX17#SKH9337X";

    EXPECT_EQ(base85::encode(data), str_to_vec(encoded));
    EXPECT_EQ(base85::decode(str_to_vec(encoded)), data);
}

TEST(Base85RoundTrip, DifferentLengths)
{
    for (int len = 0; len < 100; len++)
    {
        vector<uint8_t> data;

        for (int i = 0; i < len; i++)
        {
            data.push_back((uint8_t)((i * 37 + len * 13) % 256));
        }

        vector<uint8_t> encoded = base85::encode(data);
        vector<uint8_t> decoded = base85::decode(encoded);

        EXPECT_EQ(decoded, data) << "Сломалось на длине = " << len;
    }
}

TEST(Base85RoundTrip, AllByteValues)
{
    vector<uint8_t> data;

    for (int i = 0; i <= 255; i++)
    {
        data.push_back((uint8_t)i);
    }

    vector<uint8_t> encoded = base85::encode(data);
    vector<uint8_t> decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, data);
}

TEST(Base85RoundTrip, BiggerVector)
{
    vector<uint8_t> data;

    for (int i = 0; i < 4096; i++)
    {
        uint8_t x = (uint8_t)((i * 17 + 123) % 256);
        data.push_back(x);
    }

    vector<uint8_t> encoded = base85::encode(data);
    vector<uint8_t> decoded = base85::decode(encoded);

    EXPECT_EQ(decoded, data);
}

TEST(Base85Decode, OneSymbolCanDecodeToNothing)
{
    vector<uint8_t> decoded = base85::decode(str_to_vec("0"));

    EXPECT_TRUE(decoded.empty());
}

TEST(Base85Decode, PartialGroups)
{
    EXPECT_EQ(base85::decode(str_to_vec("00")), vector<uint8_t>({0x00}));
    EXPECT_EQ(base85::decode(str_to_vec("000")), vector<uint8_t>({0x00, 0x00}));
    EXPECT_EQ(base85::decode(str_to_vec("0000")), vector<uint8_t>({0x00, 0x00, 0x00}));
}

TEST(Base85Decode, BadCharacterThrows)
{
    EXPECT_THROW(base85::decode(str_to_vec("\"")), invalid_argument);
    EXPECT_THROW(base85::decode(str_to_vec(" ")), invalid_argument);
    EXPECT_THROW(base85::decode(str_to_vec("\n")), invalid_argument);
    EXPECT_THROW(base85::decode(str_to_vec("]")), invalid_argument);
}

TEST(Base85Decode, OverflowThrows)
{
    EXPECT_THROW(base85::decode(str_to_vec("~~~~~")), invalid_argument);
    EXPECT_THROW(base85::decode(str_to_vec("}~~~~")), invalid_argument);
}

TEST(Base85Encode, ResultSizeForDifferentInputLengths)
{
    EXPECT_EQ(base85::encode(vector<uint8_t>()).size(), 0u);
    EXPECT_EQ(base85::encode(vector<uint8_t>({1})).size(), 2u);
    EXPECT_EQ(base85::encode(vector<uint8_t>({1, 2})).size(), 3u);
    EXPECT_EQ(base85::encode(vector<uint8_t>({1, 2, 3})).size(), 4u);
    EXPECT_EQ(base85::encode(vector<uint8_t>({1, 2, 3, 4})).size(), 5u);
    EXPECT_EQ(base85::encode(vector<uint8_t>({1, 2, 3, 4, 5})).size(), 7u);
}

TEST(Base85Decode, ResultSizeForDifferentInputLengths)
{
    EXPECT_EQ(base85::decode(str_to_vec("")).size(), 0u);
    EXPECT_EQ(base85::decode(str_to_vec("0")).size(), 0u);
    EXPECT_EQ(base85::decode(str_to_vec("00")).size(), 1u);
    EXPECT_EQ(base85::decode(str_to_vec("000")).size(), 2u);
    EXPECT_EQ(base85::decode(str_to_vec("0000")).size(), 3u);
    EXPECT_EQ(base85::decode(str_to_vec("00000")).size(), 4u);
    EXPECT_EQ(base85::decode(str_to_vec("0000000")).size(), 5u);
} //фух
