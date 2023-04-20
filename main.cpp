#include <iostream>
#include <chrono>
#include <thread>
#include <stdio.h>

#include "co_event.hpp"

struct recharge_event:public co_event::event_base {
	recharge_event(int id):id_(id){}
	bool notice(std::shared_ptr<recharge_event> event) {
		printf("recharge_event notice\n");
		if (event->id_ == id_) {
			return true;
		}
		return false;
	}
	int id_;
};

auto event_mgr = co_event::event_manager::create();

co_event::task timer_print() {
	printf("begin timer_print\n");
	for (int i = 0; i < 3; ++i) {
		printf("timer print %d\n", i);
		co_await std::make_shared<co_event::timer>(600);
	}
	co_await std::make_shared<recharge_event>(99);
	printf("wait recharge ok\n");
	printf("end timer_print\n");
}

co_event::task recharge_print() {
	printf("begin recharge_print\n");
	co_await std::make_shared<recharge_event>(99);
	printf("end recharge_print\n");
}

void init() {
	event_mgr->start_coroutine(timer_print());
	//event_mgr->start_coroutine(recharge_print());
}

void update(int elapse) {
	event_mgr->update_timer(elapse);
	event_mgr->notice_event(std::make_shared<recharge_event>(99));
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
