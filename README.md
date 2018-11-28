# A stm32duino event loop
This is an event loop written for Arduino, it is mainly targetted at 
[ Arduino_STM32 ]( https://github.com/rogerclarkmelbourne/Arduino_STM32 ) the platform on which it is developed.

This may possibly work on other Arduinos but it hasn't been tested on those

#### try the demo:
In bin there is a pre-compiled flash image for maple mini 
[ bin/STM32duinoEventloop.bin ]( https://github.com/ag88/stm32duino-eventloop/tree/master/bin )
##### prerequisites
You need a maple mini (or compatible board)
[ maple mini ]( https://www.leaflabs.com/maple/ )
[ link to ebay  ]( https://www.ebay.com/sch/i.html?_from=R40&_nkw=maple+mini&_sacat=0 )

The flash image is targetted for the maple mini (and clones) (i.e. the led pin on pin 33 PB1) and should use the [ stm32duino bootloader ]( https://github.com/rogerclarkmelbourne/STM32duino-bootloader ) (i.e. image installed at 0x8002000)
You can install it to maple mini using [ dfu-util ]( http://dfu-util.sourceforge.net/ )

` dfu-util -a 2 -RD STM32duinoEventloop.bin `

The full details is beyond the scope here, you may find more details on 
[ Arduino STM32 wiki ](https://github.com/rogerclarkmelbourne/Arduino_STM32/wiki/Installation)
[wiki](http://wiki.stm32duino.com/index.php?title=Installation)

##### running the demo
* You need a serial console, you can use the Arduino IDE's serial console or apps like putty etc.
* Connect to maple mini over usb serial with the serial console, type '?' or 'h' and you should see the prompt:
`f - faster, s - slower, m - memory`
* You can change the blink rate of the led by keying f - faster or s - slower

### how it works:

mainloop.cpp is the normal arduino/wiring setup() and loop(), all the codes there (which is the sketch itself) is needed
[mainloop.cpp](https://github.com/ag88/stm32duino-eventloop/blob/master/src/mainloop.cpp)

In setup(), it registers the event handlers
[mainloop.cpp](https://github.com/ag88/stm32duino-eventloop/blob/master/src/mainloop.cpp#L38)

In loop(), it calls the actual eventloop() codes, i.e. dispatches events. 
There is a particular catch about this which sort of makes it Arduino STM32 Libmaple core specific, Libmaple sets up a 1 ms systick interrupt which is used in the delay() timings. This implementation simply wait for this systick interrupt. It also hook the systick interrupt so that it increment a variable systick_fired to indicate that systick interrupt has occurred. [mainloop.cpp](https://github.com/ag88/stm32duino-eventloop/blob/master/src/mainloop.cpp#L78)

This makes the loop() run every milliseconds, in effect making it a scheduler, i.e. loop() dispatches events every millisecond or so (it may take longer than a single millisecond when other interrupts occur / etc)

In this demo there are 2 event handler classes:
* key sender  CKeySenderTask.cpp  
In key sender, it handles the systick event, poll usbserial for inputs and handles key inputs
[src](https://github.com/ag88/stm32duino-eventloop/blob/master/src/tasks/CKeySenderTask.cpp#L18)  
the interesting parts is where it post key input events to the event queue:
[src](https://github.com/ag88/stm32duino-eventloop/blob/master/src/tasks/CKeySenderTask.cpp#L58)
[src](https://github.com/ag88/stm32duino-eventloop/blob/master/src/tasks/CKeySenderTask.cpp#L97)

* led task CLedTask.cpp  
The event loop then calls the handleEvent method of the CLedTask (this is the main event handler callback for all the event handler classes)
[src](https://github.com/ag88/stm32duino-eventloop/blob/master/src/tasks/CLedTask.cpp#L29)  
this is where the led task (or your apps handle the events)

#### how did the led blink?
There is a utility class AsyncWait, AsyncWait basically 'sleeps' for the specified miliseconds (non-blocking) before it post an event in the event queue
[src](https://github.com/ag88/stm32duino-eventloop/blob/a80b09a7551616377ce280455e5279dba21da116/src/tasks/CLedTask.cpp#L67)
[src](https://github.com/ag88/stm32duino-eventloop/blob/master/src/eventloop/ASyncWait.h#L34)

Hence CLedTask handles 2 events ledon() which calls AsyncWait to enqueue a ledoff() evenf after the specified delay and vice versa, so the led blinks  

#### how to use this, or how to write my own event handler:
* TL;DR: inherit a class from CEventHandler and implement the handleEvent() method

The examples are in [ Tasks ](https://github.com/ag88/stm32duino-eventloop/tree/master/src/tasks)

The main steps are:
1. In the arduino/wiring setup(), register the event handler object. Note that it is an object not simply a class, hence you need to instantiate that as a global variable 
[src](https://github.com/ag88/stm32duino-eventloop/blob/master/src/mainloop.cpp#L38)
2. Implement the handleEvent() in the class, the eventloop() calls this. 
[src](https://github.com/ag88/stm32duino-eventloop/blob/master/src/tasks/CLedTask.cpp#L29)  
When the sketch runs setup() post a EventID::AppStart event to all the event handlers
[src](https://github.com/ag88/stm32duino-eventloop/blob/a80b09a7551616377ce280455e5279dba21da116/src/mainloop.cpp#L49)  
This is so that the event handlers can initialize after Arduino setup() has run but just before arduino loop() runs
3. Events:  
You need to enumerate your events in events.h
[src](https://github.com/ag88/stm32duino-eventloop/blob/a80b09a7551616377ce280455e5279dba21da116/src/eventloop/Event.h#L11)  
The event handlers need to be enumerated as well in events.h 
4. Passing parameters in events  
Events has 2 parameters which are declared as int
[src](https://github.com/ag88/stm32duino-eventloop/blob/master/src/eventloop/Event.h#L47)  
the key sender task post keystrokes to the event queue as follows:
[src](https://github.com/ag88/stm32duino-eventloop/blob/master/src/tasks/CKeySenderTask.cpp#L49)
```
event.handle_id = EHandleID::BroadCast;
event.event = EventID::KeyEvent;
event.param1 = c;
EventLoop.post(event);
```
hence in an event handler you could handle the key like
```
case EventID::KeyEvent:
  int key = event.param1;
  //do something when this key is entered
```
you can cast pointers in the parameters e.g.
in a calling code:
```
char *message = "hello world";
Event event;
event.handle_id = EHandleID::LCDTask;
event.event = EventID::ShowText;
event.param1 = reinterpret_cast<int>(message);
EventLoop.post(event);
```
then in the event handler
```
case EventID::ShowText:
  char *string = reinterpret_cast<char *>(event.param1);
  // codes to show the text 
```
5. Limits:  
The event queue is a 128 entry ring buffer, if it is full, CEventLoop::post(event) returns -1 event not added ! hence be careful about posting too many events if they aren't handled)
20 event handler (classes/objects))
[src](https://github.com/ag88/stm32duino-eventloop/blob/a80b09a7551616377ce280455e5279dba21da116/src/eventloop/EventLoop.h#L12)
6. Async wait:  
Async wait works by posting an event after a delay, there are 32 wait slots (evaluated every systick i.e. every 1 ms, so that is multiplied if you want more slots)
[src](https://github.com/ag88/stm32duino-eventloop/blob/master/src/eventloop/ASyncWait.h)  
To use them first get a handle to a wait slot
[src](https://github.com/ag88/stm32duino-eventloop/blob/a80b09a7551616377ce280455e5279dba21da116/src/tasks/CLedTask.cpp#L60)  
When you want to do an async wait:
* TL;DR: once off wait WaitMgr.await(int duration, Event &event, bool imm)
duration in msec
e.g. 
```
Event event;
event.handle_id = EHandleID::LedTask;
event.event = EventID::LedTaskLedOn;
WaitMgr.await(1000, event, false); 
```
old way:
   1. create an event
   2. call the wait manager with the handle obtained earlier to get a pointer to one of the slots (think take the alarm clock)
   3. call CAsyncWait.setwait(uint32_t duration, bool repeat, Event& event, bool imm);
If you set repeat the event would be posted repeatedly every same duration. Event is the event you've created, imm (immediate) does not mean that it blocks, rather when the wait manager sees that the time is up, it can call the event handler immediately bypassing the event queue. This may make some things more prompt, nevertheless it is *async, not real time*. Duration strictly speaking is the number of systicks on Libmaple, i.e. 1ms each if nothing changed. or tl;dr: milliseconds.
   4. after calling setwait() note that the wait does not immediately take effect, you need to call CAsyncWait.await(); to start that wait timer. This design is necessary due to the repeat option in setting the wait slot, i.e. the same wait slot can keep firing events periodically
[src](https://github.com/ag88/stm32duino-eventloop/blob/a80b09a7551616377ce280455e5279dba21da116/src/tasks/CLedTask.cpp#L72)


### License
Copyright(C) 2018 Andrew Goh  
portions derived other works   
RingBuffer (Copyright (c) 2014 Anders Kalør)  
FreeStack (Copyright (c) 2011-2018 Bill Greiman)  

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.