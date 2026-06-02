# Milestone 23.5: COW-Optimized Shared Object Cache

[TOC]

*   [ ] REFRESH: Review all documents in @perl/doc/guidelines/**
*   [ ] Design file-backed shared object cache structure. (Difficulty: 5/10)
*   [ ] Implement cache population logic (e.g., in a master process). (Difficulty: 6/10)
*   [ ] Implement Copy-On-Write access using `mmap` (`MAP_PRIVATE`). (Difficulty: 7/10)
*   [ ] Develop locking/serialization for initial cache population. (Difficulty: 5/10)
*   [ ] Create tests for shared cache access from multiple processes. (Difficulty: 6/10)
*   [ ] Benchmark read scaling with multiple worker processes. (Difficulty: 5/10)
*   [ ] Address cache invalidation/update strategy. (Difficulty: 8/10)
*   [ ] Review and update perl/doc/architecture/** documents.
