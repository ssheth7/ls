Problems I faced

Problems #1: Order in which to parse args
My initial approach was to simply do a for loop through the command line arguments and parse each file or directory. However, I realized this will not work because the non-directory entries had to be parsed first. To fix this issues, I had to create seperate data structures for the non-directory and directory arguments. For this approach, I had to learn dynamic memory allocation and ran into a lot of segmentation faults and beginner mistakes. A limitation of this approach is that users can provided arguments that can cause the program to use a lot of memory. 
