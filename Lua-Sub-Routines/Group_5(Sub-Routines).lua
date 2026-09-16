-- Kericho Tea Factory Production Pipeline

-- 1. Batch Coroutine Workflow
local function processTeaBatch(batchID)
    local stages = {"receiving", "weighing", "withering", "drying"}
    
    -- Progress through initial stages
    for i = 1, #stages do
        coroutine.yield(batchID, stages[i])
    end
    
    -- 2. Quality Control and Reprocessing
    local qualityPassed = false
    while not qualityPassed do
        -- Fictional check: 60% chance to pass inspection
        if math.random() > 0.4 then
            qualityPassed = true
            coroutine.yield(batchID, "quality inspection passed")
        else
            coroutine.yield(batchID, "quality failed - redirecting")
            coroutine.yield(batchID, "withering (reprocessing)")
            coroutine.yield(batchID, "drying (reprocessing)")
        end
    end
    
    -- Final processing stages
    coroutine.yield(batchID, "grading")
    coroutine.yield(batchID, "packaging")
    
    return batchID, "completed"
end

-- Initialize the fictional data environment
math.randomseed(os.time())

-- 3. Construct the Cooperative Scheduler
local batches = {
    {co = coroutine.create(processTeaBatch), id = "Batch-001"},
    {co = coroutine.create(processTeaBatch), id = "Batch-002"},
    {co = coroutine.create(processTeaBatch), id = "Batch-003"}
}

-- 4. Safe State Management Loop
local allCompleted = false
while not allCompleted do
    allCompleted = true
    
    for _, batch in ipairs(batches) do
        -- Check that coroutine is not dead before resuming
        if coroutine.status(batch.co) ~= "dead" then
            allCompleted = false
            
            -- Resume passes the ID on the first call; subsequent calls ignore it
            local success, returnedID, currentStage = coroutine.resume(batch.co, batch.id)
            
            -- Error handling based on resume return values
            if success then
                print(string.format("[%s] Current Stage: %s", returnedID, currentStage))
            else
                print(string.format("[%s] Pipeline Error: %s", batch.id, returnedID))
            end
        end
    end
end

print("All factory batches processed successfully.")
