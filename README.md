# cooperative-scheduler

## By Marwan Abbas 900153642
##    Omar Abu Gabal 

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

![alt text]

# First Test

This screenshot is showing one task running with the ReRunMe(0) function

![alt text]

# Second Test

This screenshot is showign one task running with the ReRunMe(5) function
There is a function inside the systick handler that counts ticks and displays it
This screenshot shows that the task runs every 5 ticks

![alt text]
