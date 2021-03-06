#include <iostream>
#include "HackableCode.h"

NO_OPTIMIZE
int hackableRoutineTakeDamage(int health, int damage)
{
	// This is the code we want to be hackable by the user
	HACKABLE_CODE_BEGIN();
		health -= damage;
	HACKABLE_CODE_END();

	HACKABLES_STOP_SEARCH();

	if (health < 0)
	{
		health = 0;
	}

	return health;
}
END_NO_OPTIMIZE

void runExampleHealth()
{
	int health = 100;

	// Print original health
	std::cout << "Original health: " << health << std::endl;

	health = hackableRoutineTakeDamage(health, 25);
	
	// Print health after taking damage
	std::cout << "Health after taking damage: " << health << std::endl;

	// Parse hackable section(s)
	auto funcPtr = &hackableRoutineTakeDamage;
	std::vector<HackableCode*> hackables = HackableCode::create((void*&)funcPtr);

	if (hackables.empty())
	{
		std::cout << "Error finding hackable sections!" << std::endl;
		return;
	}

	HackableCode* hackableCode = hackables[0];

	// Print original code
	std::cout << std::endl << "ORIGINAL CODE:" << std::endl << hackableCode->getAssemblyString() << std::endl;

	if (hackableCode->applyCustomCode("nop"))
	{
		std::cout << std::endl << "NEW CODE:" << std::endl << hackableCode->getAssemblyString() << std::endl << std::endl;

		int newHealth = hackableRoutineTakeDamage(health, 25);

		std::cout << "Health after taking damage (hacked): " << newHealth << std::endl;

		if (newHealth == health)
		{
			std::cout << "Health unchanged! NOP injection successful." << std::endl;
		}
	}
	else
	{
		std::cout << "Error compiling assembly!" << std::endl;
	}
}

// This example uses a significant amount of inline assembly to manipulate variables
NO_OPTIMIZE
int hackableRoutineGainMana(int mana, int manaGain)
{
	// Variables loaded into assembly must be local, static, and volatile to survive compiling with optimizations (tested on Clang/MSVC/GCC)
	static volatile int manaLocal;
	static volatile int manaGainLocal;

	manaLocal = mana;
	manaGainLocal = manaGain;

	// Load variables into registers
	ASM_MOV_REG_VAR(ZAX, manaLocal);
	ASM_MOV_REG_VAR(ZBX, manaGainLocal);

	// This is the code we want to be hackable by the user
	HACKABLE_CODE_BEGIN()
	ASM(add ZAX, ZBX);
	ASM_NOP4(); // Leaving extra space for custom assembly code!
	HACKABLE_CODE_END();

	// Copy register back to variable
	ASM_MOV_VAR_REG(manaLocal, ZAX);

	HACKABLES_STOP_SEARCH();

	return manaLocal;
}
END_NO_OPTIMIZE

void runExampleMana()
{
	int mana = 25;

	// Print original mana
	std::cout << "Original mana: " << mana << std::endl;

	mana = hackableRoutineGainMana(mana, 25);

	// Print mana after gaining some
	std::cout << "Mana after drinking potion: " << mana << std::endl;

	// Parse hackable section(s)
	auto funcPtr = &hackableRoutineGainMana;
	std::vector<HackableCode*> hackables = HackableCode::create((void*&)funcPtr);

	if (hackables.empty())
	{
		std::cout << "Error finding hackable sections!" << std::endl;
		return;
	}

	HackableCode* hackableCode = hackables[0];

	// Print original code
	std::cout << std::endl << "ORIGINAL CODE:" << std::endl << hackableCode->getAssemblyString() << std::endl;

	// Apply 32/64 bit code depending on platform
	if (hackableCode->applyCustomCode(sizeof(int*) == 4 ? "imul eax, ebx" : "imul rax, rbx"))
	{
		std::cout << std::endl << "NEW CODE:" << std::endl << hackableCode->getAssemblyString() << std::endl << std::endl;

		int newMana = hackableRoutineGainMana(mana, 25);

		std::cout << "Mana after drinking potion (hacked): " << newMana << std::endl;
	}
	else
	{
		std::cout << "Error compiling assembly!" << std::endl;
	}
}

int main()
{
	std::cout << "==== EXAMPLE 1 =====" << std::endl << std::endl;
	runExampleHealth();
	std::cout << std::endl << "==== EXAMPLE 2 =====" << std::endl << std::endl;
	runExampleMana();

	std::string stall;
	std::getline(std::cin, stall);
}