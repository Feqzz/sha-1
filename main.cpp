#include <iostream>
#include <bitset>
#include <vector>
#include <math.h>
#include <sstream>

void constructBlocks(std::vector<std::vector<std::bitset<32>>> &vec, const std::string binaryPlainText,
	const int numberOfBlocks)
{
	for (int i = 0; i < numberOfBlocks; i++)
	{
		std::vector<std::bitset<32>> tmp;
		for (int j = 0; j < 16; j++)
		{
		 	tmp.push_back(std::bitset<32>(binaryPlainText.substr(((32 * j) + 512 * i), 32)));
		}
		vec.push_back(tmp);
	}
}

std::string addPadding(const std::string input, const int numberOfBlocks)
{
	std::string retStr;
	for (std::size_t i = 0; i < input.size(); ++i)
	{
		retStr += std::bitset<8>(input[i]).to_string();
	}

	int retStrSize = retStr.size();

	for (std::size_t i = retStr.size(); i < ((512 * numberOfBlocks) - 64); i++)
	{
		if (i == retStrSize)
		{
			retStr += "1";
		}
		else
		{
			retStr += "0";
		}
	}
	retStr += std::bitset<64>(retStrSize).to_string();
	return retStr;
}

std::bitset<32> rotl(const std::bitset<32> x, const unsigned int moves)
{
	return (x << moves) | (x >> (32 - moves));
}

void compress(std::vector<std::bitset<32>> &vec)
{
	for (int i = 16; i < 80; i++)
	{
		vec.push_back(rotl((vec.at(i - 3) ^ vec.at(i - 8) ^ vec.at(i - 14) ^ vec.at(i - 16)), 1));
	}
}

std::bitset<32> plus(const std::bitset<32> a, const std::bitset<32> b)
{
	const uint32_t left = static_cast<uint32_t>(a.to_ulong());
	const uint32_t right = static_cast<uint32_t>(b.to_ulong());
	return std::bitset<32>(left + right);
}

std::string fun(std::bitset<32> &a, std::bitset<32> &b, std::bitset<32> &c, 
	std::bitset<32> &d, std::bitset<32> &e, const std::vector<std::bitset<32>> w)
{
	std::bitset<32> originalA = a;
	std::bitset<32> originalB = b;
	std::bitset<32> originalC = c;
	std::bitset<32> originalD = d;
	std::bitset<32> originalE = e;

	for (int i = 0; i < 80; i++)
	{
		std::bitset<32> function;
		std::bitset<32> k;
		if (i < 20)
		{
			function = (b & c) | ((~b) & d);
			k = 0x5A827999;
		}
		else if (i < 40)
		{
			function = b ^ c ^ d;
			k = 0x6ED9EBA1;
		}
		else if (i < 60)
		{
			function = (b & c) | (b & d) | (c & d);
			k = 0x8F1BBCDC;
		}
		else if (i < 80)
		{
			function = b ^ c ^ d;
			k = 0xCA62C1D6;
		}

		std::bitset<32> temp = plus(rotl(a, 5), plus(function, plus(e, plus(w.at(i), k))));

		e = d;
		d = c;
		c = rotl(b, 30);
		b = a;
		a = temp;
	}

	a = plus(originalA, a);
	b = plus(originalB, b);
	c = plus(originalC, c);
	d = plus(originalD, d);
	e = plus(originalE, e);

	std::cout << std::hex << a.to_ulong() << std::endl;

	std::stringstream stream;
	stream << std::hex << a.to_ulong() << b.to_ulong() << c.to_ulong() << d.to_ulong() << e.to_ulong();
	return stream.str();
}

void printUsage()
{
	std::cout << "Usage: ./sha-1 <plaintext>" << std::endl;
}

int main(int args, char *argv[])
{
	if (args < 2)
	{
		printUsage();
		return 0;
	}

	std::string input = argv[1];

	if (input == "--help" || input == "-h")
	{
		printUsage();
		return 0;
	}

	const int inputBitLength = input.length() * 8;
	const int numberOfBlocks = ceil(inputBitLength / 448.0);
	const int paddingAmount = ((numberOfBlocks * 512) - 64) - inputBitLength;

	std::string binaryPlainText = addPadding(input, numberOfBlocks);
	
	std::vector<std::vector<std::bitset<32>>> blocksVector;
	constructBlocks(blocksVector, binaryPlainText, numberOfBlocks);
	std::string hashedText;

	std::bitset<32> h0 = 0x67452301;
	std::bitset<32> h1 = 0xEFCDAB89;
	std::bitset<32> h2 = 0x98BADCFE;
	std::bitset<32> h3 = 0x10325476;
	std::bitset<32> h4 = 0xC3D2E1F0;

	std::bitset<32> a = h0;
	std::bitset<32> b = h1;
	std::bitset<32> c = h2;
	std::bitset<32> d = h3;
	std::bitset<32> e = h4;

	for (int i = 0; i < numberOfBlocks; i++)
	{
		compress(blocksVector[i]);
		hashedText = fun(a, b, c, d, e, blocksVector[i]);
	}

	std::cout << "plaintext: " << input << std::endl;
	std::cout << "hashed: " << hashedText << std::endl;
}