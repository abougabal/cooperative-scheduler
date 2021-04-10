# cooperative-scheduler

## By
### Marwan Abbas 900153642
### Omar Abu Gabal 900162146

# project explenation

For this project we developed a cooperative scheduler
We developed the tasks as void functions that don't take any paramteres
For the task to be treated and manipulated as an object we developed a struct called taks that stores the function name, priority and delay. (this is initialized in the main.h)
We developed 2 queues, the ready queue, which takes the ready to execute functions and sort them by their priority
We used bubble sort to sort the queues based on the priority
Then we developed a QueTask function that gets called from the main, ISR, other tasks or the task itself
QueTask adds the task to the ready queue
Dispatch is used to get the highest priority in the ready queue and run it

We then developed a ReRunMe function that takes an integer, this integer specifies if the task should get called again imdiatly or after how many ticks
The Tick is changed from the systick by changing the function HAL_InitTick
inside the function we made this change: HAL_SYSTICK_Config(SystemCoreClock / (1000U / 200))
That means that the tick interrupt is made every 50ms rather than every 1ms

# Unit Tests

For the unit tests, we developed a function called Test
This function tests the bubble sort, size function, and shifting function
These screenshots shows the test function running on teraterm

This screenshot shows the ready queue before sorting and shifting

![alt text](https://github.com/marwaneltoukhy/cooperative-scheduler/blob/main/beforeShifting.png)

This screenshot shows the ready queue after shifting

![alt text](https://github.com/marwaneltoukhy/cooperative-scheduler/blob/main/afterShifting.png)

This screenshot shows the ready queue after sorting

![alt text](https://github.com/marwaneltoukhy/cooperative-scheduler/blob/main/afterSorting.png)


## First Test

This screenshot is showing one task running with the ReRunMe(0) function

![alt text](https://github.com/marwaneltoukhy/cooperative-scheduler/blob/main/test1.png)

## Second Test

This screenshot is showign one task running with the ReRunMe(5) function
There is a function inside the systick handler that counts ticks and displays it
This screenshot shows that the task runs every 5 ticks

![alt text](https://github.com/marwaneltoukhy/cooperative-scheduler/blob/main/test2.png)

## Third Test

This test shows 2 tasks running with 2 different priorities, task 1 is priority 1 and task 2 is priority 5

![alt text](https://github.com/marwaneltoukhy/cooperative-scheduler/blob/main/2TasksDifferentPrioritySameDelay.png)

## Fourth Test

This test shows 2 tasks running with 2 tasks running the same priorities

![alt text](https://github.com/marwaneltoukhy/cooperative-scheduler/blob/main/samePriority.png)


# Applications

## Parking sensor

For the parking sensor application we used two tasks one to start the interrupt of TIM of channel 1, this task has the high priority and doesn't have
the ReRunMe function as we need it once.

The second task is periodic with 1 tick, this is not optimal as it should run every 1ms, but the handout specified that the tick should be 50ms
The second task is used to pull the trigger pin high and low with a delay of 10us inbetween as specified in the datasheet

We then triggered a set and reset flag to a GPIO output pin, that sets the flag when the TIM edge is going from low to high
and resets when going from high to low, that pin delivers the voltage to the positive side of the buzzer, and the other side is connected to the gnd

The video for the parking sensor:

![alt text](https://github.com/marwaneltoukhy/cooperative-scheduler/blob/main/parkingSensor.mp4)
