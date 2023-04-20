# co_event
a c++ coroutine asynchronous event library, no IO or network related, just logic

## usage
in old days, if you want do something after some event happens, like after a period of time, you may use callback. like this
```c++
void timer_callback() {
	std::cout << "hello timer!";
}
void some_func() {
	create_timer(100ms)->set_callback(timer_callback);
}
```
even with lambda, you may write
```c++
void some_func() {
	create_timer(100ms)->set_callback([](){
		std::cout << "hello timer!";
	});
}
```
a little better, but still use asynchronous callback function, not enough good.
but now with c++20 coroutine, every thing changed, now you can write like this with co_event.
```c++
co_event::task some_func() {
	co_await std::make_shared<co_event::timer>(100);
	std::cout << "hello timer!";
}
```
thanks to the coroutine, you can write asynchronous logic just like the normal logic!

## how to use?
### timer
for timer event, you should do the following
1. create a event_manager, and save the manger
```c++
auto event_mgr = co_event::event_manager::create();
```
1. update the event manager
```c++
event_mgr->update_timer(elapse);
```
1. call start coroutine when use timer
```c++
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
void some_func() {
	event_mgr->start_coroutine(timer_print());
}
```
### event
not just timer event, you can define your own event

# to do, you can find example in main.cpp
