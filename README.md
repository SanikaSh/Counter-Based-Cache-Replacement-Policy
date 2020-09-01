# Counter-Based-Cache-Replacement-Policy
Settings used to run the simulation:
Common settings:
a. L1 cache - 32 KB for instruction, 32 KB for data 
b. L2 cache - 16-way set associative
c. Replacement Policy RefCount - CBAIPRP() (counter-based AIP RP)

I]
build/X86/gem5.opt --outdir=bodytrack_RefCount_1MB configs/example/se.py --cpu-type=AtomicSimpleCPU -n 6 --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=1MB --l2_rpp="CBAIPRP()"  --mem-size=8192MB -c benchmarks/bodytrack/bodytrack -o "benchmarks/bodytrack/sequenceB_1 4 1 100 5 2 4"

Settings:
a. 6 CPU cores for 4-thread bodytrack 
b. L2 cache - 1MB

II]
build/X86/gem5.opt --outdir=bodytrack_RefCount_4MB configs/example/se.py --cpu-type=AtomicSimpleCPU -n 6 --caches --l1i_size=32kB --l1d_size=32kB --l2cache --l2_size=4MB --l2_rpp="CBAIPRP()"  --mem-size=8192MB -c benchmarks/bodytrack/bodytrack -o "benchmarks/bodytrack/sequenceB_1 4 1 100 5 2 4"

Settings:
a. 6 CPU cores for 4-thread bodytrack 
b. L2 cache - 4MB



