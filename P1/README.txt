name: Bryan Shangguan
netID: bys8

name: Christopher Chan
netID: cwc85

Test plan: 
There are 8 test cases. The first five are performance tests and the last three are for testing errors. The first performance test (task1) allocates one byte and the immediately frees it, 120 
times. The second test (task2) allocates 120 1-byte objects, and stores the pointers into an array. Then, it uses free() to deallocate the chunks. The third test (task 3) creates an array of 
120 pointers. It then repeatedly makes a random choice between allocating a 1-byte object and adding a pointer to the array and deallocating a previously allocated object, until you have 
allocated 120 times. It then deallocates any remaining objects. To test the last three, you would need to comment out the ones above it (to test the second one you would comment out the first, 
and so on) to determine the error that would appear. The fourth performance test (task4) allocates memory for 10 pointers, that each have 100 bytes per chunk, then it deallocates the allocated 
chunks. The fifth performance test (task 5) runs NUM_ALLOC times, and will allocate memory the size (i % 10) + 1 bytes. It will then free the allocated memory. The first error 
(test_invalid_free_non_malloc()) is what will happen when we call free() with an address that is unobtainable from malloc(). The second error (test_invalid_free_not_chunk_start()) is what 
happens when we call free(), but we are not calling an address at the start of a chunk. The third error (test_double_free()) is when we call free() twice on the same pointer. 

Program Description: 
The program has a region of memory called the heap. The size of the heap we are using is 4096 bytes. We will be allocating space for "chunks" that contain a header and a payload. The header 
contains the size of the "chunk" and if it has been allocated or not. The payload contains the data that we will be allocating memory for. There are two functions that are called malloc() and
free(), allocating and deallocating memory respectively. The malloc() function saves space for the "chunk". The free() method removes the designated allocated "chunk". In the program, we have 
four "helper" functions: align_size(size_t size), leakdetector(), initialize_heap(). The function align_size(size_t size) makes sure the size being malloced is only multiples of 8. The function 
leakdetector() keeps track of the amount of chunks and bytes being leaked. The function initialize_heap() makes sure it has enough memory for the "chunk" being saved. The function 
coalesce_chunks() merges free chunks that are next to each other in memory. 

Design Notes: To run the last three errors, you will need to comment out the ones above it to see the errors. 
