#include "Core/System/Random.h"

#include <cstdlib>
#include <ctime>

std::atomic_flag Core::System::Random::m_lockRandom = ATOMIC_FLAG_INIT;

void Core::System::Random::Initialization()
{
	while (m_lockRandom.test_and_set()) {}

	srand(static_cast<unsigned>(time(NULL)));

	m_lockRandom.clear();
}

int Core::System::Random::GetRandomInt(const int p_min, const int p_max)
{
	while (m_lockRandom.test_and_set()) {}

	int to_return = p_min + (rand() % (p_max - p_min));

	m_lockRandom.clear();

	return to_return;
}

float Core::System::Random::GetRandomFloat(const float p_min, const float p_max)
{
	while (m_lockRandom.test_and_set()) {}

	float to_return = p_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX) / (p_max - p_min));

	m_lockRandom.clear();

	return to_return;
}
