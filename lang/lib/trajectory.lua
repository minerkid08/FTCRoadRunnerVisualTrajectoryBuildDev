---@param trajectory Trajectory
---@return Segment[]
function processTrajectory(trajectory)
  local nodeUsage = {};
  for k, v in ipairs(trajectory.segments) do
    nodeUsage[v.startNode] = nodeUsage[v.startNode] or {};
    nodeUsage[v.endNode] = nodeUsage[v.endNode] or {};
    nodeUsage[v.startNode].startv = true;
    nodeUsage[v.endNode].endv = true;
  end
  local startInd = -1;
  local emptyNodes = false;
  for k, v in ipairs(trajectory.nodes) do
    if (nodeUsage[k] == nil) then
      emptyNodes = true;
      goto continue
    end
    if (nodeUsage[k].startv == true and nodeUsage[k].endv == nil) then
      if (startInd == -1) then
        startInd = k;
      else
        log.error("export failed: path has multiple start nodes");
        return
      end
    end
    ::continue::
  end

  if (emptyNodes) then
    log.warn("warning: path has unused nodes");
  end
  if (startInd == -1) then
    log.error("export failed: no start node found");
    return;
  end
  local segments = {};
  local targetInd = startInd;
  local foundNode = true;
  while (foundNode) do
    foundNode = false;
    local foundInd = 0;
    for k, seg in ipairs(trajectory.segments) do
      if (seg.startNode == targetInd) then
        if (foundNode) then
          log.error("export failed: fork found at node " .. tostring(k));
          return;
        end
        foundNode = true;
        foundInd = seg.endNode;
        table.insert(segments, seg);
      end
    end
    targetInd = foundInd;
  end
  return segments;
end

---@param angle number
---@return number
function rot(angle)
  return -angle - 90;
end

---@enum HeadingMode
HeadingMode = {
  None = 0,
  Linear = 1,
  Constant = 2
}
