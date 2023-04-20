#ifndef _CO_EVENT_H_
#define _CO_EVENT_H_

#include <coroutine>
#include <concepts>
#include <exception>
#include <memory>
#include <iostream>
#include <stdio.h>
#include <list>

namespace co_event {

enum class task_type: int {
	Timer,
	Event,
	Max,
};

struct event_base {
	virtual bool update_timer(int) {return false;}
	virtual task_type type() const noexcept {return task_type::Event;}
	template <typename EventType>
		requires std::derived_from<EventType, event_base>
	bool notice(std::shared_ptr<EventType>) const noexcept {return false;}
};

struct task {
	struct promise_type;
	using handle_type = std::coroutine_handle<promise_type>;

	struct promise_type {
		task get_return_object() {
			return task(handle_type::from_promise(*this));
		}
		std::suspend_never initial_suspend() noexcept {
			return {};
		}
		std::suspend_always final_suspend() noexcept {
			return {};
		}
		void return_void() {}
		void unhandled_exception() {}

		template <typename EventType>
			requires std::derived_from<EventType, event_base>
		std::suspend_always await_transform(std::shared_ptr<EventType> event) {
			event_ = event;
			//printf("await_transform %d %d\n", event_->type(), event->type());
			return {};
		}

		std::shared_ptr<event_base> event_;
	};

	task(handle_type h):h_(h), counter_(std::make_shared<int>(1)) {}
	task(const task& rhs):h_(rhs.h_), counter_(rhs.counter_) {
		++(*counter_);
	}
	task& operator=(const task& rhs) {
		if (this != &rhs) {
			h_ = rhs.h_;
			counter_ = rhs.counter_;
			++(*counter_);
		}
		return *this;
	}
	~task() {
		if (--(*counter_) == 0) {
			h_.destroy();
			printf("h_ destroy\n");
		}
		h_ = 0;
	}

	void operator() () {
		h_();
	}
	bool done() {
		return  h_.done();
	}

	handle_type h_;
	std::shared_ptr<int> counter_;
};

struct timer: public event_base {
	virtual task_type type() const noexcept override {return task_type::Timer;}
	bool update_timer(int elapse) noexcept {
		//printf("update_timer\n");
		if (time_ > 0) {
			time_ -= elapse;
			if (time_ <= 0) {
				return true;
			}
		}
		return false;
	}
	timer(int time): time_(time) {}
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
		task_list_.push_back(t);
	}

	void update_timer(int elapse) {
		for (auto it = task_list_.begin(); it != task_list_.end();) {
			auto& task = *it;
			auto& event = task.h_.promise().event_;
			if (event && event->type() == task_type::Timer && event->update_timer(elapse)) {
				task();
			}
			if (task.done()) {
				it = task_list_.erase(it);
			} else {
				++it;
			}
		}
	}

	template <typename EventType>
		requires std::derived_from<EventType, event_base>
	void notice_event(std::shared_ptr<EventType> event) {
		for (auto it = task_list_.begin(); it != task_list_.end();) {
			auto& task = *it;
			auto event_ = std::dynamic_pointer_cast<EventType>(task.h_.promise().event_);
			if (event_ && event_->type() == task_type::Event && event_->notice(event)) {
				task();
			}
			if (task.done()) {
				it = task_list_.erase(it);
			} else {
				++it;
			}
		}
	}

private:
	std::list<task> task_list_;
};


}
#endif

