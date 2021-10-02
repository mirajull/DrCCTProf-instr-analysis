# DrCCTProf-instr-analysis
Project 0 : CSC 512 Compiler Construction (A new client for DrCCTProf)

The purpose of the project is to install and get familiar with DrCCTProf, a state-of-the-art fine-grained analysis tool to understand compiler-generated binary codes. The total points of this project is 100.

DrCCTProf
An open-source project available via GitHub
DrCCTProf is a fine-grained analyzer for ARM architectures. Unlike existing fine-grained tools, DrCCTProf has the following features: 
Rich insights: DrCCTProf provides both code- and data-centric analysis. The code-centric analysis associates the captured calling contexts with source code if debugging information exists. Moreover, if the monitored instruction is a memory access, DrCCTProf attributes this memory access to the data object allocated either in the static data section or dynamically with malloc family functions. The combination of code- and data-centric provides rich insights for performance analysis. 

Low overhead: As a fine-grained tool, DrCCTProf is efficient both in runtime and memory. DrCCTProf employs a dynamically built CCTs to store every call path resulting in an execution. A CCT merges common prefixes across all call paths, which reduces memory overhead dramatically. DrCCTProf employs efficient data structures and inlined binary instrumentation to keep runtime and memory overheads in an acceptable range. 

Parallel scaling: As our main target is HPC applications, DrCCTProf handles large, parallel programs. DrCCTProf monitors processes and threads efficiently with minimum synchronization overhead, yielding good scalability as shown in Section F.2. Furthermore, DrCCTProf aggregates the profiles from different threads and processes and presents them in a compact view. 

Data visualization: The fine-grained analysis can produce voluminous data, making data exploration difficult. DrCCTProf employs a GUI to help summarize and interpret this data in a compact, navigable, and hierarchical manner. With the GUI, DrCCTProf can show any correlated contexts (e.g., pairs of data use and reuse contexts) ranked with user-defined metrics. 

Extensibility: As a framework, DrCCTProf is well modularized. One can easily enable or disable part of features (e.g., code- or data-centric attribution) to fulfill the need. Moreover, DrCCTProf exposes simple APIs to support client tools. Incorporating code-centric or data-centric features into a fine-grained tool involves minimal efforts. 

Applicability: DrCCTProf is broadly applicable to both ARM and x86 architectures, thanks to the support of DynamoRIO. Moreover, DrCCTProf works on binary, which is language, compiler, or programming model independent.
Specially, you must:

Download DrCCTProf and install it in a Linux machine. We suggest you use VCL system at NCSU. We have created an image named "csc412-512", where we installed all the dependences and HPCToolkit. Once you download DrCCTProf, you just run "sh build.sh" to compile DrCCTProf.
Learn how to write a client tool atop DrCCTProf by reading the source code of client tools (https://github.com/Xuhpclab/DrCCTProf/tree/master/src/clients). Pay particular attention to drcctlib_all_instr_cct clients.
CSC 412 students: Write your own client tool --- drcctlib_instr_analysis --- with the following requirement: (1) The client tool analyzes every instruction. (2) The client tool categorizes every instruction: memory loads, memory stores, conditional branches, and unconditional branches. (3) You print out the total instruction counts for each category for the whole program. Example output: csc412_p0_output.txt
CSC 512 students: Besides the above work, you print out the top 10 contexts in their full calling contexts (and source codes) with the most number of instructions and their instruction categories. Example output: csc512_p0_output.txt
For students in Both CSC 412 and 512, you need to test your client tools with the testing programs released by the TA.

Requirements
You need to use C/C++ to develop the clients.
Besides using DrCCTProf APIs, you will also need to use DynamoRIO APIs to identify different instruction categories. Check The DynamoRIO webpage for more information. 
We will use VCL to test your program. We will release an image for you to work on. Please make sure to store your file into /afs/unity.ncsu.edu/users/..., which will not be deleted when your reservation expires.
Deliverables
    The tarball (or zip file) of the entire directory of your client (i.e., the directory of drcctlib_instr_analysis), which includes a .cpp file and a CMakeLists.txt file.


Questions
Q: Why I cannot install DrCCTProf? 
You need to follow the installation instruction in DrCCTProf README. If you are using your own computer, make sure you install the necessary dependencies (the ones in apt-get install in README) and get DynamoRIO successfully installed. 
Q: The documentation of DrCCTProf is inadequate. 
True, but in this project, you don't need to understand DrCCTProf internals. You just need to know how to use DrCCTProf to develop client tools.
