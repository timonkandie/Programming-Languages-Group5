# Kericho Tea Factory Production Pipeline

### CCS 2105 — Programming Languages Lab: Sub-Routines & Coroutines

---

## Group 5 Members

| # | Name | Registration Number | Contribution |
|---|------|---------------------|-------------|
| 1 | **Pheneas Lazarus** | C026-01-0986/2025 | Coroutine workflow design, quality control logic, documentation |
| 2 | **Timon Kandie** | C026-01-0956/2025 | Cooperative scheduler implementation, state management, testing |
| 3 | **Victor Rutto** | C026-01-0960/2025 | Error handling, output formatting, code review |

---

## System Documentation

### 1. Problem Statement

In a real-world tea processing factory, multiple batches of tea must be processed simultaneously through a series of stages — from receiving raw leaves to final packaging. Each batch may fail quality inspection and require reprocessing, while other batches continue independently. Traditional sequential programming cannot elegantly model this interleaved, cooperative execution.

This project uses **Lua coroutines** to simulate the **Kericho Tea Factory Production Pipeline**, demonstrating how cooperative multitasking can model concurrent-like workflows without true multithreading.

### 2. Objectives

- Demonstrate the creation and lifecycle of Lua coroutines (`coroutine.create`, `coroutine.yield`, `coroutine.resume`, `coroutine.status`)
- Implement a **round-robin cooperative scheduler** that fairly distributes processing time across multiple batches
- Show how coroutine **state persistence** simplifies complex workflow management compared to normal functions
- Implement a **quality control reprocessing loop** where failed batches re-enter earlier pipeline stages
- Demonstrate robust **error handling** using coroutine resume return values

### 3. System Architecture

```
+------------------------------------------------------------------+
|                   MAIN SCHEDULER (while loop)                    |
|   Iterates through all batches in round-robin fashion            |
|   Checks coroutine.status() before each resume                  |
+----------+---------------------+---------------------+----------+
           |                     |                     |
     +-----+-----+         +----+------+         +----+------+
     | Batch-001 |         | Batch-002 |         | Batch-003 |
     +-----+-----+         +-----+-----+         +-----+-----+
           |                     |                     |
           +-----------+---------+---------+-----------+
                       |
              +--------+--------+
              |  PIPELINE STAGES  |
              +-----------------+
              | 1. Receiving     |
              | 2. Weighing      |
              | 3. Withering     |
              | 4. Drying        |
              | 5. Quality Check |--- FAIL ---+
              | 6. Grading       |            |
              | 7. Packaging     |    Reprocess (3->4)
              +-----------------+
```

### 4. Key Concepts Demonstrated

#### 4.1 Coroutine Creation & Lifecycle

Each tea batch is wrapped in a coroutine using `coroutine.create(processTeaBatch)`. The coroutine begins in a **suspended** state and transitions through:
- **suspended** → `coroutine.resume()` → **running**
- **running** → `coroutine.yield()` → **suspended**
- **running** → `return` → **dead**

```lua
local batches = {
    {co = coroutine.create(processTeaBatch), id = "Batch-001"},
    {co = coroutine.create(processTeaBatch), id = "Batch-002"},
    {co = coroutine.create(processTeaBatch), id = "Batch-003"}
}
```

#### 4.2 Yielding & Resuming (Cooperative Multitasking)

Each batch **voluntarily** pauses execution after completing a stage by calling `coroutine.yield()`. The scheduler cannot forcibly interrupt a batch:

```lua
for i = 1, #stages do
    coroutine.yield(batchID, stages[i])  -- Pause and report current stage
end
```

#### 4.3 Round-Robin Cooperative Scheduler

The main `while` loop acts as a scheduler, iterating through all batches in order. Before resuming a batch, it checks `coroutine.status()` to skip completed ("dead") coroutines:

```lua
for _, batch in ipairs(batches) do
    if coroutine.status(batch.co) ~= "dead" then
        local success, returnedID, currentStage = coroutine.resume(batch.co, batch.id)
    end
end
```

#### 4.4 Quality Control & Reprocessing Loop

Batches that fail quality inspection (60% pass rate) are redirected back through the withering and drying stages before re-inspection. This loop naturally leverages coroutine state persistence:

```lua
local qualityPassed = false
while not qualityPassed do
    if math.random() > 0.4 then
        qualityPassed = true
        coroutine.yield(batchID, "quality inspection passed")
    else
        coroutine.yield(batchID, "quality failed - redirecting")
        coroutine.yield(batchID, "withering (reprocessing)")
        coroutine.yield(batchID, "drying (reprocessing)")
    end
end
```

#### 4.5 State Persistence vs. Normal Functions

When a normal Lua function returns, its execution stack is **completely destroyed**. All local variables and progress are lost. If we used normal functions for this pipeline, we would need a complex external state machine (global variables or database tables) to track each batch's position.

Coroutines **persist their state** across yields. When a coroutine yields, its execution stack, local variables (like `batchID` and `qualityPassed`), and exact line of execution are preserved in memory. When `coroutine.resume()` is called, the batch wakes up **exactly** where it left off.

#### 4.6 Error Handling

The scheduler uses the boolean return value of `coroutine.resume()` to detect and handle runtime errors gracefully:

```lua
if success then
    print(string.format("[%s] Current Stage: %s", returnedID, currentStage))
else
    print(string.format("[%s] Pipeline Error: %s", batch.id, returnedID))
end
```

### 5. How to Run

**Prerequisites**: Lua 5.3+ interpreter installed.

```bash
# Run the simulation
lua Group_5\(Sub-Routines\).lua
```

**Expected behavior**: The program processes three tea batches cooperatively. Output shows interleaved stage progression across all batches. Batches that fail quality inspection are automatically reprocessed. The program terminates when all batches reach the "completed" state.

### 6. Sample Output

```
[Batch-001] Current Stage: receiving
[Batch-002] Current Stage: receiving
[Batch-003] Current Stage: receiving
[Batch-001] Current Stage: weighing
[Batch-002] Current Stage: weighing
[Batch-003] Current Stage: weighing
[Batch-001] Current Stage: withering
[Batch-002] Current Stage: withering
[Batch-003] Current Stage: withering
[Batch-001] Current Stage: drying
[Batch-002] Current Stage: drying
[Batch-003] Current Stage: drying
[Batch-001] Current Stage: quality inspection passed
[Batch-002] Current Stage: quality failed - redirecting
[Batch-003] Current Stage: quality inspection passed
[Batch-001] Current Stage: grading
[Batch-002] Current Stage: withering (reprocessing)
[Batch-003] Current Stage: grading
[Batch-001] Current Stage: packaging
[Batch-002] Current Stage: drying (reprocessing)
[Batch-003] Current Stage: packaging
[Batch-002] Current Stage: quality inspection passed
[Batch-002] Current Stage: grading
[Batch-002] Current Stage: packaging
All factory batches processed successfully.
```

**Note**: Output varies between runs due to the randomized quality inspection (60% pass rate).

### 7. Conclusion

This project demonstrates that Lua coroutines provide a powerful, lightweight mechanism for modeling cooperative concurrent workflows. By using `coroutine.yield()` and `coroutine.resume()`, we achieved quasi-concurrent execution of multiple tea batches without the complexity of OS-level threads or external state machines. The coroutine's built-in state persistence eliminates the need for manual progress tracking, resulting in clean, readable, and maintainable code.

---

<p align="center"><sub>© 2026 Group 5 — CCS 2105 Programming Languages. University of Kabianga.</sub></p>
