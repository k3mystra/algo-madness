#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <string>

// Generate one random 5-letter lowercase stringx.
static std::string generateRandomLabel(std::mt19937_64 &rng)
{
	static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
	std::uniform_int_distribution<int> letterDist(0, 25);

	std::string label;
	label.reserve(5);
	for (int index = 0; index < 5; ++index)
	{
		label.push_back(alphabet[letterDist(rng)]);
	}
	return label;
}

// Convert a student-ID style seed token into digits using the assignment mapping.
static std::uint64_t parseSeedToken(const std::string &seedToken)
{
	std::string mappedDigits;
	mappedDigits.reserve(seedToken.size());

	for (char character : seedToken)
	{
		if (character >= '0' && character <= '9')
		{
			mappedDigits.push_back(character);
		}
		else if (character >= 'A' && character <= 'Z')
		{
			mappedDigits.push_back(static_cast<char>('1' + ((character - 'A') % 10)));
		}
		else if (character >= 'a' && character <= 'z')
		{
			mappedDigits.push_back(static_cast<char>('1' + ((character - 'a') % 10)));
		}
		else
		{
			throw std::invalid_argument("Seed token must contain only letters and digits.");
		}
	}

	return std::stoull(mappedDigits);
}

// Keep the step coprime to 9,000,000,000 so the generated integers stay unique.
static std::uint64_t chooseCoprimeStep(std::mt19937_64 &rng)
{
	const std::uint64_t rangeSize = 9000000000ULL;
	std::uniform_int_distribution<std::uint64_t> stepDist(1ULL, rangeSize - 1ULL);

	while (true)
	{
		std::uint64_t step = stepDist(rng) | 1ULL; // Force odd.
		while (step % 3ULL == 0ULL || step % 5ULL == 0ULL)
		{
			step += 2ULL;
			if (step >= rangeSize)
			{
				step = 1ULL;
			}
		}

		if (std::gcd(step, rangeSize) == 1ULL)
		{
			return step;
		}
	}
}

int main(int argc, char *argv[])
{
	std::uint64_t datasetSize = 0;
	std::uint64_t seed = 0;
	std::string datasetSizeInput;
	std::string seedToken;

	// Allow first-run input in the terminal when arguments are missing.
	if (argc >= 2)
	{
		datasetSizeInput = argv[1];
	}
	else
	{
		std::cout << "Enter dataset size: ";
		if (!std::getline(std::cin, datasetSizeInput))
		{
			std::cerr << "Failed to read dataset size.\n";
			return 1;
		}
	}

	if (argc >= 3)
	{
		seedToken = argv[2];
	}
	else
	{
		std::cout << "Enter seed token: ";
		if (!std::getline(std::cin, seedToken))
		{
			std::cerr << "Failed to read seed token.\n";
			return 1;
		}
	}

	try
	{
		datasetSize = std::stoull(datasetSizeInput);
		seed = parseSeedToken(seedToken);
	}
	catch (const std::exception &)
	{
		std::cerr << "Invalid argument. Use only letters and digits for the seed token.\n";
		return 1;
	}

	const std::uint64_t minValue = 1000000000ULL;
	const std::uint64_t maxValue = 9999999999ULL;
	const std::uint64_t rangeSize = maxValue - minValue + 1ULL;

	if (datasetSize == 0ULL || datasetSize > rangeSize)
	{
		std::cerr << "Dataset size must be between 1 and 9000000000.\n";
		return 1;
	}

	std::mt19937_64 rng(seed);

	// This affine generator produces a deterministic permutation over the 10-digit range.
	const std::uint64_t step = chooseCoprimeStep(rng);
	std::uint64_t currentOffset = std::uniform_int_distribution<std::uint64_t>(0ULL, rangeSize - 1ULL)(rng);

	const std::string outputFileName = "dataset_" + std::to_string(datasetSize) + ".csv";
	std::ofstream output(outputFileName, std::ios::out | std::ios::trunc);
	if (!output.is_open())
	{
		std::cerr << "Unable to open output file: " << outputFileName << '\n';
		return 1;
	}

	// Stream the CSV rows directly to disk so large datasets do not need extra memory.
	for (std::uint64_t index = 0; index < datasetSize; ++index)
	{
		const std::uint64_t value = minValue + currentOffset;
		const std::string label = generateRandomLabel(rng);

		output << value << ',' << label;
		if (index + 1ULL < datasetSize)
		{
			output << '\n';
		}

		currentOffset += step;
		if (currentOffset >= rangeSize)
		{
			currentOffset -= rangeSize;
		}
	}

	output.close();
	std::cout << "Generated " << datasetSize << " rows into " << outputFileName << '\n';
	return 0;
}
