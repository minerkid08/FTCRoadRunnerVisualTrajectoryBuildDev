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

---@param trajectory Trajectory
---@param indent string
---@param config TrajConfigPedro
function exportTrajectoryPedro(trajectory, indent, config)
  file.write(indent .. config.start .. "\n");
  local segments = processTrajectory(trajectory);
  for _, seg in ipairs(segments) do
    local startNode = trajectory.nodes[seg.startNode];
    local endNode = trajectory.nodes[seg.endNode];
    if (#seg.controlPoints == 2) then
      local a = seg.controlPoints[1];
      local b = seg.controlPoints[2];
      local ids = {
        x1 = a.x + 72,
        y1 = a.y + 72,
        x2 = b.x + 72,
        y2 = b.y + 72
      };
      local args = {};
      for k, v in ipairs(config.line.args) do
        args[k] = ids[v];
      end
      file.write(indent .. config.line.str:format(table.unpack(args)) .. "\n");
    else
      if (config.curveFun ~= nil) then
        config.curveFun(seg.controlPoints, indent)
      else
        local i = #seg.controlPoints - 2;
        if (config.curve[i] == nil) then
          log.error("export failed: curves with " .. tostring(i) .. " control points are not supported");
        end
        local ids = {}
        for k, v in ipairs(seg.controlPoints) do
          ids["x" .. tostring(k)] = v.x;
          ids["y" .. tostring(k)] = v.y;
        end
        local args = {};
        for k, v in ipairs(config.curve[i].args) do
          args[k] = ids[v];
        end
        file.write(indent .. config.curve[i].str:format(table.unpack(args)) .. "\n");
      end
    end
    if (seg.heading == HeadingMode.Linear) then
      local ids = {
        h1 = -startNode.heading + 90,
        h2 = -endNode.heading + 90
      };
      local args = {};
      for k, v in ipairs(config.linearHeading.args) do
        args[k] = ids[v];
      end
      file.write(indent .. config.linearHeading.str:format(table.unpack(args)) .. "\n");
    end
    if (seg.heading == HeadingMode.Constant) then
      local ids = {
        h = -startNode.heading + 90,
      };
      local args = {};
      for k, v in ipairs(config.constantHeading.args) do
        args[k] = ids[v];
      end
      file.write(indent .. config.constantHeading.str:format(table.unpack(args)) .. "\n");
    end
  end
  if (config.build ~= nil) then
    file.write(indent .. config.build .. "\n");
  end
end

---@param trajectory Trajectory
---@param indent string
---@param config TrajConfigRR
function exportTrajectoryRR(trajectory, indent, config)
  local segments = processTrajectory(trajectory);
  local startNode = trajectory.nodes[segments[1].startNode];

  local ids = {
    x = startNode.x,
    y = startNode.y,
    h = rot(startNode.heading)
  };
  local args = {};
  for k, v in ipairs(config.start.args) do
    args[k] = ids[v];
  end
  file.write(indent .. config.start.str:format(table.unpack(args)) .. "\n");

  local prevTangent = 0.0;
  for _, seg in ipairs(segments) do
    local endNode = trajectory.nodes[seg.endNode];
    if (prevTangent ~= seg.startTan) then
      local ids = {
        t = rot(seg.startTan + 180)
      };
      local args = {};
      for k, v in ipairs(config.setTan.args) do
        args[k] = ids[v];
      end
      file.write(indent .. config.setTan.str:format(table.unpack(args)) .. "\n");
    end
    if (seg.heading == HeadingMode.None) then
      local ids = {
        x = endNode.x,
        y = endNode.y,
        h = rot(endNode.heading),
        t = rot(seg.endTan + 180)
      };
      local args = {};
      for k, v in ipairs(config.splineTo.args) do
        args[k] = ids[v];
      end
      file.write(indent .. config.splineTo.str:format(table.unpack(args)) .. "\n");
    end
    if (seg.heading == HeadingMode.None) then
      local ids = {
        x = endNode.x,
        y = endNode.y,
        h = rot(endNode.heading),
        t = rot(seg.endTan + 180)
      };
      local args = {};
      for k, v in ipairs(config.splineLinear.args) do
        args[k] = ids[v];
      end
      file.write(indent .. config.splineLinear.str:format(table.unpack(args)) .. "\n");
    end
    if (seg.heading == HeadingMode.None) then
      local ids = {
        x = endNode.x,
        y = endNode.y,
        t = rot(seg.endTan + 180)
      };
      local args = {};
      for k, v in ipairs(config.splineConstant.args) do
        args[k] = ids[v];
      end
      file.write(indent .. config.splineConstant.str:format(table.unpack(args)) .. "\n");
    end
    prevTangent = seg.endTan;
  end
  file.write(indent .. config.build .. "\n");
end
