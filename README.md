# Online-Food-Order-Sysytem
This Repository contains Multi-Process Embedded Linux System.
🎯 1. Scenario Selection & Problem Understanding 

Modern food platforms like Swiggy and Zomato must:

Handle multiple incoming orders concurrently
Process them asynchronously
Maintain logs and system reliability
This scenario requires:
Naturally requires concurrency (threads)
Requires multi-process separation
Needs IPC for communication
Needs signals for control (shutdown/emergency)


🏗️ 2. Process-Level Design 
Processes:
🧠 Parent Controller
Creates all child processes
Handles shutdown via signals
Uses waitpid() → prevents zombies
🧾 Order Generator Process
Multi-threaded order creation
Sends orders via pipe
👨‍🍳 Kitchen Process
Processes incoming orders
Adds delay 
Sends results via message queue
📄 Logger Process
Writes structured logs
Maintains log metadata using lseek()


🧵 3. Thread-Level Design

Inside Order Generator:

Thread	Purpose
Pizza Thread	Generates pizza orders
Burger Thread	Generates burger orders
Fries Thread	Generates fries orders
✅ Synchronization:
pthread_mutex protects shared order ID
Prevents race conditions
🔗 4. IPC Design 
✔ Pipe
Fast, unstructured data transfer
Order Generator → Kitchen
✔ Message Queue
Structured async messaging
Kitchen → Logger
✅ Justification:
Pipe = stream data
Message Queue = structured processing


🔄 5. Data Flow Diagram 
   <img width="1536" height="1024" alt="Online food order flowchart diagram" src="https://github.com/user-attachments/assets/caf25369-fc6e-4b4d-8c34-741641304bfb" />


🚨 6. Signals & Lifecycle Management 
Signal	Action
SIGINT	Graceful shutdown
SIGTERM	Force cleanup
SIGUSR1	Print system snapshot

So All processes terminate cleanly

📁 7. File I/O 
Features:
Structured logs
File header updated using lseek()

Example:

Total Orders: 25
-----------------------
OrderID=1 Item=Pizza Status=READY
