#include "AoBRandomiser.h"

CAoBRandomiser* AoBRandomiser;
DWORD dOption = 3;
DWORD dAmount = 0;
DWORD dSeed = 0;

DWORD pOffsetList[0x5000] = { 0 };

int main()
{
	AoBRandomiser = new CAoBRandomiser();
	AoBRandomiser->Generate();
	system("PAUSE");
	delete AoBRandomiser;

	return 0;
};

VOID CAoBRandomiser::Generate() {

	printf_s("(-- Dark Souls III Item Randomiser Seed Generator --)\n\n"
		"Author: LukeYui\n");

	printf_s("Generate new seed or use existing? (0 = New, 1 = Existing)\n");
	std::cin >> dOption;

	while (dOption > 2) {
		dOption = 0;
		printf_s("Invalid input: %i\n", dOption);
		printf_s("Generate new seed or use existing? (0 = New, 1 = Existing\n");
		std::cin >> dOption;
	}

	if (dOption) {
		printf_s("Input seed...\n");
		std::cin >> std::hex >> dSeed;
	}
	else dSeed = GetSeed();

	dOption = 0;

	printf_s("Create custom list? (0 = No, 1 = Yes)\n");
	std::cin >> dOption;

	if (dOption) {
		printf_s("How many items? (Default is 1600, max is 20000\n");
		std::cin >> dAmount;
	}
	else dAmount = 1600;

	if (dAmount > 0x5000) {
		dAmount = 0x5000;
	};

	ShuffleList();

	OutputList();

	printf_s("Done!\n");

	return;
};

DWORD CAoBRandomiser::GetSeed() {

	DWORD dNewSeed = 0;

	unsigned uSeed1 = (unsigned)std::chrono::system_clock::now().time_since_epoch().count();
	unsigned uSeed2 = (unsigned)__rdtsc();

	std::minstd_rand0 g1(uSeed1);
	std::minstd_rand0 g2(uSeed2);

	dNewSeed = (DWORD)g1();

	printf_s("Seed = %X\n", dNewSeed);

	return dNewSeed;
};

VOID CAoBRandomiser::ShuffleList() {

	DWORD i = 1;

	pOffsetList[0] = dAmount;

	while (i <= dAmount) {
		pOffsetList[i] = i;
		i++;
	};

	std::shuffle(&pOffsetList[1], &pOffsetList[dAmount+1], std::default_random_engine(dSeed));

	return;
};

VOID CAoBRandomiser::OutputList() {

	DWORD i = 0;

	std::ofstream outfile("DS3RandomAoB.txt");

	if (outfile.is_open()) {

		while (i <= dAmount) {
			outfile << pOffsetList[i] << std::endl;
			i++;
		};
		outfile.close();
		return;
	};

	printf_s("Failed to save list...\n");
	system("PAUSE");
	exit(0xDEADBA);

	return;
};