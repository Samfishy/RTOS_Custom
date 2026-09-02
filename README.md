# Custom Preemptive ARM Cortex-M RTOS Kernel

A lightweight, deterministic, preemptive Real-Time Operating System (RTOS) engineered from scratch in bare-metal C and ARM Assembly for ARM Cortex-M microcontrollers. 

Drawing architectural inspiration from MiROS by Quantum Leaps, this kernel was developed to deeply explore low-level CPU mechanics, exception handling, and concurrent task scheduling. The kernel features strict priority-based task execution, asynchronous context switching, and custom-built resource synchronization primitives.

---

## ⚙️ Core Architecture

* **O(1) Priority Scheduler:** Implements a priority bitmap scheduler utilizing hardware leading-zero detection (`LOG2` / `__CLZ`) to achieve constant-time thread resolution and instant ready-state evaluation.
* **Asynchronous Context Switching:** Configures dual-stack execution (MSP/PSP) and employs a naked `PendSV_Handler` to directly manage hardware exception frames and software-stacked registers (R4-R11).
* **Non-Blocking Timing Subsystem:** Replaces busy-wait loops with a `SysTick`-driven delta timeout queue to handle delayed or blocked tasks, eliminating CPU busy-waiting and allowing seamless fallback to a background idle task.
* **Custom Synchronization Primitives:** Counting semaphores and mutexes were engineered entirely from scratch to manage shared resource contention, complete with dynamic Priority Inheritance logic.

---

## 🔬 Hardware Verification & Concurrency Stress Test

Real-time guarantees, strict preemptive scheduling, bounded delays, mutual exclusion, and priority inheritance behaviors were rigorously stress-tested and verified using precise GPIO pin toggling captured on a hardware logic analyzer.

### 1️⃣ Baseline Preemptive Scheduling
<img width="1139" height="236" alt="Scheduled Messages" src="https://github.com/user-attachments/assets/44ff35aa-cbfa-4228-a9e1-2566fc4fde20" />


*Zooming in on the preemptive scheduler:* The highest-priority Task 1 executes and then yields via `OS_Delay`, allowing the lower-priority Task 4 to run. Strict preemption is perfectly demonstrated as Tasks 2 and 1 sequentially wake up, their bits are set in `OS_ReadySet`, and they instantly claim the CPU. Execution is then cleanly handed back down the priority hierarchy as they complete their periodic workloads.

### 2️⃣ Bounded Delay & Contention
<img width="1244" height="212" alt="Scheduled Messages(1)" src="https://github.com/user-attachments/assets/ded212d8-3342-4fcf-963c-3b743a84bdeb" />


*Showcasing bounded delay during resource contention:* A lower-priority task acquires a shared resource, clearing its bit in the ready set and temporarily blocking contending threads. The RTOS scheduler safely sustains this critical section until the lock is cleanly released, ensuring deterministic handover and preventing unbounded task starvation.

### 3️⃣ The Semaphore Flaw: Unbounded Priority Inversion
<img width="1247" height="212" alt="Scheduled Messages(3)" src="https://github.com/user-attachments/assets/f8c7cfd2-e158-48a5-81be-fe249dab88e2" />


*Demonstrating the fatal flaw of standard semaphores:* High-priority Task 2 acquires the shared binary semaphore, executes its critical section, and releases it. Later, low-priority Task 4 acquires the semaphore but is immediately preempted by medium-priority Task 3. Because the standard semaphore implementation only tracks counts without tracking resource ownership, Task 3 aggressively monopolizes the CPU. When Task 2 attempts to take the semaphore again, it gets blocked by Task 4—effectively starving the highest-priority task and creating a deadlock.

### 4️⃣ The Solution: Mutex Priority Inheritance
<img width="1247" height="214" alt="Scheduled Messages(4)" src="https://github.com/user-attachments/assets/89aa2253-2bd1-401e-902a-d6337dd713b2" />


*Solving Priority Inversion with Mutex Priority Inheritance:* Following the dashed blue line, high-priority Task 2 blocks on a Mutex held by low-priority Task 4. The kernel instantly elevates Task 4 to Task 2's priority, allowing it to finish its critical section without being preempted by the rogue Task 3. Upon unlocking, Task 4's base priority is restored, and resource ownership seamlessly snaps back to Task 2.

---

## 🎥 Hardware Demonstration

Below is a synchronized demonstration showing the physical hardware executing the RTOS workloads. The breadboard LEDs indicate active task execution in real-time, perfectly synced with the logic analyzer trace capturing the context switches and priority inversions at the microsecond level.

https://github.com/user-attachments/assets/a8b3bcc1-79c8-4c67-ba0e-98d44831f498


