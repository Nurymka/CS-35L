*** Issues I ran into

Looking at main.c initially, I saw the correct general approach of dividing work into multiple threads
by looping through the right intervals of 0 < x < width. When I created void *pth_srt (the function
that each thread executes), I stumbled upon a problem of figuring out what should be included
in the function and what should be left out in main. I decided to copy over the initial calculations
of pixel_dx/pixel_dy/subsample_dx/subsample_dy in the thread function itself, so there are less things
to pass in the struct argument. The other issue was dividing the width into equal pixels for each
thread, as width of 131 doesn't divide to 2/4/8 without remainders. That problem was solved
by making sure the remainder is also looped through by the last thread. Another issue was the fact
that we have no control over the order of printing the color values for each pixel, so leaving printf
statements in the thread function will lead into a messy .ppm file with chaotic pixel values. I solved
that by introducing a global variable 'float image[width][height][3]', where each thread assigns color
values in thread's own pixel region, so there is not going to be any writing race conditions.
After each thread terminates, the main() funciton later loops through each pixel one more time and
prints the output correctly.

*** Conclusions about my implementation of SRT

If to look at the tests of each ./srt command, we see that 2 threads give a result of 25.031s
(52% of the single threaded performance), 4 threads give 16.748s (35% of the single threaded performance),
8 threads give 10.758s (22% of the single threaded performance). Multithreading yielded an increase in
performance by almost a factor of 5, but there is a certain limit the amount of context switches between
threads would become too expensive to be shorten execution time.


