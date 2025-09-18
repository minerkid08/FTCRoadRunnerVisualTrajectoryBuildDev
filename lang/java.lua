require("lib.action");
require("lib.trajectory");

---@class file
---@field write fun(string)
file = file or {}

---@type Action[]
actions = actions or {}

table.insert = function(t, v)
  for k = 1, #t do
    if (t[k] == nil) then
      t[k] = v;
      return k;
    end
  end
  t[#t + 1] = v;
  return #t;
end

---@type Action
local tree = actions[1];

processAction(tree);

---@param trajectory Trajectory
---@param indent string
local function printTrajectoryPedro(trajectory, indent)
  file.write(indent .. "follower.pathBuilder()\n");
  local segments = processTrajectory(trajectory);
  for _, seg in ipairs(segments) do
    local startNode = trajectory.nodes[seg.startNode];
    local endNode = trajectory.nodes[seg.endNode];
    if (#seg.controlPoints == 2) then
      local a = seg.controlPoints[1];
      local b = seg.controlPoints[2];
      file.write(indent .. (".addPath(new BezierLine(new Pose(%.2f, %.2f), new Pose(%.2f, %.2f)))\n"):format(a.x, a.y, b.x, b.y));
    else
      file.write(indent .. ".addPath(new BezierCurve(\n");
      for _, p in ipairs(seg.controlPoints) do
        file.write(indent .. ("  new Pose(%.2f, %.2f)\n"):format(p.x, p.y));
      end
      file.write(indent .. "))\n");
    end
    if (seg.heading == HeadingMode.Linear) then
      file.write(indent ..
        (".setLinearHeadingInterpolation(Math.toRadians(%.2f), Math.toRadians(%.2f))\n"):format(rot(startNode.heading),
          rot(endNode.heading)));
    end
    if (seg.heading == HeadingMode.Constant) then
      file.write(indent ..
        (".setConstantHeadingInterpolation(Math.toRadians(%.2f))\n"):format(rot(startNode.heading)));
    end
  end
  file.write(indent .. ".build()\n");
end

---@param trajectory Trajectory
---@param indent string
local function printTrajectoryRR(trajectory, indent)
  local segments = processTrajectory(trajectory);
  local startNode = trajectory.nodes[segments[1].startNode];
  file.write(indent ..
  ("drive.actionBuilder(new Pose2d(%.2f, %.2f, Math.toRadians(%.2f)))\n"):format(startNode.x, startNode.y, rot(startNode.heading)));
  local prevTangent = 0.0;
  for _, seg in ipairs(segments) do
    local endNode = trajectory.nodes[seg.endNode];
    if (prevTangent ~= seg.startTan) then
      file.write(indent .. (".setTangent(%d)\n"):format(rot(seg.startTan)));
    end
    if (seg.heading == HeadingMode.None) then
      file.write(indent ..
        (".splineTo(new Vector2d(%.2f, %.2f), Math.toRadians(%.2f))\n"):format(endNode.x, endNode.y, rot(seg.endTan + 180)));
    end
    if (seg.heading == HeadingMode.None) then
      file.write(indent ..
        (".splineLinearHeadingTo(new Pose2d(%.2f, %.2f, Math.toRadians(%.2f)), Math.toRadians(%.2f))\n"):format(endNode.x,
          endNode.y, rot(endNode.heading), rot(seg.endTan + 180)));
    end
    if (seg.heading == HeadingMode.None) then
      file.write(indent ..
        (".splineToConstantHeading(new Vector2d(%.2f, %.2f), Math.toRadians(%.2f))\n"):format(endNode.x, endNode.y,
          rot(seg.endTan + 180)));
    end
    prevTangent = seg.endTan;
  end
  file.write(indent .. ".build()\n");
end

---@param action Action
---@param indent string
---@param trailingComma boolean
local function printAction(action, indent, trailingComma)
  if (action.name == "sequential") then
    file.write(indent .. "new SequentialAction(\n");
  elseif (action.name == "parallel") then
    file.write(indent .. "new ParallelAction(\n");
  elseif (action.name == "trajectory") then
    file.write(indent .. "new PathAction(\n");
    if (rr == nil) then
      printTrajectoryPedro(action.trajectory, indent .. "  ");
    else
      printTrajectoryRR(action.trajectory, indent .. "  ");
    end
  end
  if (action.tree ~= nil) then
    local i = #action.tree;
    for k, v in ipairs(action.tree) do
      printAction(v, indent .. "  ", i > k);
    end
  end
  if (action.name == "sequential" or action.name == "parallel" or action.name == "trajectory") then
    if (trailingComma) then
      file.write(indent .. "),\n");
    else
      file.write(indent .. ")\n");
    end
  else
    file.write(indent .. "new " .. action.name .. "(");
    local i = #action.fields;
    for k, v in ipairs(action.fields) do
      file.write(tostring(v.value));
      if (i > k) then
        file.write(", ");
      end
    end
    if (trailingComma) then
      file.write("),\n");
    else
      file.write(")\n");
    end
  end
end

printAction(tree, "", false);
