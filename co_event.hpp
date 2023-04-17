#ifndef _CO_EVENT_H_
#define _CO_EVENT_H_

#include <coroutine>
#include <exception>
#include <memory>
#include <iostream>
#include <stdio.h>
#include <list>

namespace co_event {

enum class task_type: int {
	Timer,
	Max,
};

struct task {
	struct promise_type;
	using handle_type = std::coroutine_handle<promise_type>;

	struct promise_type {
		task get_return_object() {
			return{
				.h_ = handle_type::from_promise(*this)
			};
		}
		std::suspend_never initial_suspend() noexcept {
			return {};
		}
		std::suspend_always final_suspend() noexcept {
			return {};
		}
		void return_void() {}
		void unhandled_exception() {}

		task_type t_;
		int param_;
	};

	bool operator () () {
		h_();
		return h_.done();
	}
	bool done() {
		return  h_.done();
	}

	task_type type() const noexcept {
		return h_.promise().t_;
	}
	int param() const noexcept {
		return h_.promise().param_;
	}

	void update_time_elapse(int elapse) {
		if (h_.promise().t_ == task_type::Timer && h_.promise().param_ > 0) {
			h_.promise().param_ -= elapse;
			if (h_.promise().param_ <= 0) {
				h_.resume();
			}
		}
	}

	handle_type h_;
};

struct timer {
	timer(int time): time_(time) {}
	bool await_ready() const noexcept {
		return false;
	}
	void await_suspend(task::handle_type h) {
		h.promise().t_ = task_type::Timer;
		h.promise().param_ = time_;
	}
	void await_resume() {}
	int time_ = 0;
};

class event_manager {
public:
	~event_manager() {}

private:
	event_manager() {}
	event_manager(const event_manager&) = delete;
	event_manager& operator=(const event_manager&) = delete;

public:
static std::shared_ptr<event_manager> create() {
	return std::shared_ptr<event_manager>(new event_manager());
}

public:
	void start_coroutine(task t) {
		task_list.push_back(t);
	}

private:
	std::list<task> task_list;
};


}
#endif

