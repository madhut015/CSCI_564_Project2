1. Describe how your prefetcher works.
   The fixed stride prefetcher works by prefetching cache lines based on a fixed stride. It initializes the prefetcher with a prefetch_amount and a stride value. I chose prefetching amount to be 3 and stride value to be 4 so the prefetcher will fetch three cache lines ahead of the currently accessed cache line, skipping 4 cache lines between each prefetched cache line. So in general for each memory access, it prefetches the next prefetch_amount cache lines, each separated by the stride value. The idea behind this prefetcher is to exploit the spatial locality in memory accesses with a constant stride between consecutive accesses.

   
2. Explain how you chose that prefetch strategy.
   I chose the fixed stride prefetch strategy because it is relatively simple to implement, and it can capture spatial locality in memory accesses with a constant stride. While it may not be as effective as more advanced prefetching strategies that adapt to memory access patterns, it can still provide some performance benefits for regular memory access patterns with constant strides. For example, it had OUTPUT HIT RATIO of ~ 93.5% for trace 4 and for trace 1 it had a OUTPUT HIT RATIO ~99.3%.


3. Discuss the pros and cons of your prefetch strategy.
   Pros: 
      - Can capture spatial locality for regular memory access patterns with constant strides
      - Easy to implement
      - Potentially improves cache hit rates for specific access patterns, especially if the application has a regular memory access pattern where data elements are spaced by a stride of 4 cache lines
   Cons: 
      - May not be effective for irregular memory access patterns or patterns with varying strides that are not 4 cache lines
      - Can lead to unnecessary prefetches and cache pollution if the chosen stride does not match the memory access pattern
      - Limited performance benefits as it does not adapt to different memory access patterns


4. Demonstrate that the prefetcher could be implemented in hardware (this can be
   as simple as pointing to an existing hardware prefetcher using the strategy
   or a paper describing a hypothetical hardware prefetcher which implements
   your strategy).

   The fixed stride prefetcher could be implemented in hardware. An example of an existing hardware prefetcher using a fixed stride strategy is the Stride Prefetcher found in the IBM POWER4 processor. The Stride Prefetcher in the POWER4 detects constant stride memory access patterns and prefetches cache lines based on the detected stride, which is similar to the fixed stride prefetch strategy implemented. According to the book, "Suppose successive data elements are being processed that are regularly spaced in memory (that is with a constant stride). With the POWER4 cache, the worst case is when the stride is exactly 16 KB or a multiple of 16 KB...". This mentions that the worst case for the POWER4 cache is when the stride is exactly 16 KB or a multiple of 16 KB.


5. Cite any additional sources that you used to develop your prefetcher.

   1. "The IBM POWER4 Processor", IBM Journal of Research and Development, Volume 46, Issue 1, 2002, https://www.redbooks.ibm.com/redbooks/pdfs/sg247041.pdf
   “IBM redbooks.” [Online]. Available: https://www.redbooks.ibm.com/redbooks/pdfs/sg247041.pdf. [Accessed: 06-Apr-2023]. 

   2. J. A. E. K. Y. U.  LEE, H. Y. E. S. O. O. N.  KIM, and R. I. C. H. A. R. D.  VUDUC, “Gatech.edu,” When Prefetching Works, When It Doesn’t, and Why. [Online]. Available: https://faculty.cc.gatech.edu/~hyesoon/lee_taco12.pdf. [Accessed: 06-Apr-2023]. 