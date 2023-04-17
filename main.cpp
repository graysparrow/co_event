#include <iostream>
#include <chrono>
#include <thread>
#include <stdio.h>

#include "co_event.hpp"

void init() {
}

void update(int elapse) {
	auto event_mgr = co_event::event_manager::create();
}

int main() {
	init();
	using namespace std::chrono_literals;

	while (true) {
		const auto start = std::chrono::high_resolution_clock::now();
		std::this_thread::sleep_for(200ms);
		const auto end = std::chrono::high_resolution_clock::now();
		const std::chrono::duration<double, std::milli> elapsed = end - start;

		//std::cout << "Waited " << elapsed << '\n';
		update(elapsed.count());
	};
	return 0;
}
