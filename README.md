Problems I faced

Problem #1: Order in which to parse args
My initial approach was to simply do a for loop through the command line arguments and parse each file or directory. However, I realized this will not work because the non-directory entries had to be parsed first. To fix this issues, I had to create seperate data structures for the non-directory and directory arguments. For this approach, I had to learn dynamic memory allocation and ran into a lot of segmentation faults and beginner mistakes. A limitation of this approach is that users can provided arguments that can cause the program to use a lot of memory.

Problem #2: Printing sizes
I thought this option would be simple as I would only need to print stblocks. The problem compilcated as I had to adjust the print out for both the -k, -h, and the BLOCKSIZE environment variable. I had to use the default ls command to determine the correct output and math. I also spent a significant time using my own rules for the humanize option, but I realized that humanize_number(3) was available and it made the option a lot easier to implement. Another difficult part of these options were printing the total blocks of a given directory. It was easier to add the blocks as I was traversing the directory, but doing ahead of time required more thought. I eventually used the fts_children function to count the block sizes before printing the directory entry. This option was also complicated by the -k, -h, and BLOCKSIZE arguments. 

Problem #3: Long format

Problem #4: Recursive option

Problem #5: Evaluating arguments
Once the argument flags were set, it was difficult to take all the arguments into account as they created a lot of branches in my code. My first idea was to assign each argument a number and then do a logical or with all those numbers. I would then get a unique number and be able to go evaluate my arguments quicker, similar to the flags in read(2) and open(2). However, I realized that it was easier to evaluate the arguments in different stages. First, I would evaluate the sorting arguments, and then I would move on to the printing arguments. 
