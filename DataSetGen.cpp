

#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

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

// Choose the first unused CSV filename for the requested dataset size.
static std::string chooseOutputFileName(std::uint64_t datasetSize)
{
	const std::string baseName = "dataset_" + std::to_string(datasetSize);
	const std::string baseFileName = baseName + ".csv";

	if (!std::ifstream(baseFileName).good())
	{
		return baseFileName;
	}

	for (std::uint64_t suffix = 1ULL; suffix != 0ULL; ++suffix)
	{
		const std::string candidateFileName = baseName + "_" + std::to_string(suffix) + ".csv";
		if (!std::ifstream(candidateFileName).good())
		{
			return candidateFileName;
		}
	}

	return baseFileName;
}

// Write one dataset file using the provided size and seed.
static bool generateDatasetFile(std::uint64_t datasetSize, std::uint64_t seed)
{
	const std::uint64_t minValue = 1000000000ULL;
	const std::uint64_t maxValue = 9999999999ULL;
	const std::uint64_t rangeSize = maxValue - minValue + 1ULL;

	if (datasetSize == 0ULL || datasetSize > rangeSize)
	{
		std::cerr << "Dataset size must be between 1 and 9000000000.\n";
		return false;
	}

	std::mt19937_64 rng(seed);

	// This affine generator produces a deterministic permutation over the 10-digit range.
	const std::uint64_t step = chooseCoprimeStep(rng);
	std::uint64_t currentOffset = std::uniform_int_distribution<std::uint64_t>(0ULL, rangeSize - 1ULL)(rng);

	const std::string outputFileName = chooseOutputFileName(datasetSize);
	std::ofstream output(outputFileName, std::ios::out | std::ios::trunc);
	if (!output.is_open())
	{
		std::cerr << "Unable to open output file: " << outputFileName << '\n';
		return false;
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
	return true;
}

int main(int argc, char *argv[])
{
	if (argc == 3)
	{
		try
		{
			const std::uint64_t datasetSize = std::stoull(argv[1]);
			const std::uint64_t seed = parseSeedToken(argv[2]);
			return generateDatasetFile(datasetSize, seed) ? 0 : 1;
		}
		catch (const std::exception &)
		{
			std::cerr << "Invalid argument. Use only letters and digits for the seed token.\n";
			return 1;
		}
	}

	auto readLine = [](const std::string &prompt) -> std::string
	{
		std::cout << prompt;
		std::string value;
		if (!std::getline(std::cin, value))
		{
			throw std::runtime_error("Failed to read input.");
		}
		return value;
	};

	auto readYesNo = [&](const std::string &prompt) -> bool
	{
		while (true)
		{
			const std::string response = readLine(prompt);
			if (response == "y" || response == "Y" || response == "yes" || response == "YES")
			{
				return true;
			}
			if (response == "n" || response == "N" || response == "no" || response == "NO")
			{
				return false;
			}
			std::cout << "Please answer y or n.\n";
		}
	};

	auto readDatasetSize = [&](const std::string &prompt) -> std::uint64_t
	{
		while (true)
		{
			try
			{
				const std::uint64_t datasetSize = std::stoull(readLine(prompt));
				if (datasetSize == 0ULL || datasetSize > 9000000000ULL)
				{
					std::cout << "Dataset size must be between 1 and 9000000000.\n";
					continue;
				}
				return datasetSize;
			}
			catch (const std::exception &)
			{
				std::cout << "Please enter a valid whole number.\n";
			}
		}
	};

	auto readSeedToken = [&](const std::string &prompt) -> std::uint64_t
	{
		while (true)
		{
			try
			{
				return parseSeedToken(readLine(prompt));
			}
			catch (const std::exception &)
			{
				std::cout << "Seed token must contain only letters and digits.\n";
			}
		}
	};

	const std::uint64_t fileCount = readDatasetSize("How many files do you want to generate? ");
	const bool sameSizeForAll = readYesNo("Use the same size for all files? (y/n): ");
	std::uint64_t sharedSize = 0;
	if (sameSizeForAll)
	{
		sharedSize = readDatasetSize("Enter the size for all files: ");
	}

	const bool sameSeedForAll = readYesNo("Use the same seed for all files? (y/n): ");
	std::uint64_t sharedSeed = 0;
	if (sameSeedForAll)
	{
		sharedSeed = readSeedToken("Enter the seed token for all files: ");
	}

	// Stream the CSV rows directly to disk so large datasets do not need extra memory.
	for (std::uint64_t fileIndex = 0; fileIndex < fileCount; ++fileIndex)
	{
		const std::uint64_t currentSize = sameSizeForAll ? sharedSize : readDatasetSize("Enter the size for file " + std::to_string(fileIndex + 1ULL) + ": ");
		const std::uint64_t currentSeed = sameSeedForAll ? sharedSeed : readSeedToken("Enter the seed token for file " + std::to_string(fileIndex + 1ULL) + ": ");

		if (!generateDatasetFile(currentSize, currentSeed))
		{
			return 1;
		}
	}

	return 0;
}
