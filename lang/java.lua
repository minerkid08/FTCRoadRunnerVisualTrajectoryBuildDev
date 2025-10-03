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

---@type Config
local cfg = {
  pedro = {
    start = "follower.pathBuilder()",
    build = ".build()",
    curveFun = function(points, indent)
      file.write(indent .. ".addPath(new BezierCurve(\n");
      for _, p in ipairs(points) do
        file.write(indent .. ("  new Pose(%.2f, %.2f)\n"):format(p.x + 72, p.y + 72));
      end
      file.write(indent .. "))\n");
    end,
    line = {
      str = ".addPath(new BezierLine(new Pose(%.2f, %.2f), new Pose(%.2f, %.2f)))",
      args = { "x1", "y1", "x2", "y2" }
    },
    constantHeading = {
      str = ".setConstantHeadingInterpolation(Math.toRadians(%.2f))",
      args = { "h" }
    },
    linearHeading = {
      str = ".setLinearHeadingInterpolation(Math.toRadians(%.2f), Math.toRadians(%.2f))",
      args = { "h1", "h2" }
    }
  },
  rr = {
    start = {
      str = "drive.actionBuilder(new Pose2d(%.2f, %.2f, Math.toRadians(%.2f)))",
      args = { "x", "y", "h" },
    },
    build = ".build()",
    setTan = {
      str = ".setTangent(Math.toRadians(%.2f))",
      args = { "t" }
    },
    splineConstant = {
      str = ".splineToConstantHeading(new Vector2d(%.2f, %.2f), Math.toRadians(%.2f))",
      args = { "x", "y", "t" }
    },
    splineLinear = {
      str = ".splineLinearHeadingTo(new Pose2d(%.2f, %.2f, Math.toRadians(%.2f)), Math.toRadians(%.2f))",
      args = { "x", "y", "h", "y" }
    },
    splineTo = {
      str = ".splineTo(new Vector2d(%.2f, %.2f), Math.toRadians(%.2f))",
      args = { "x", "y", "t" }
    }
  },
  parallel = "new ParallelAction",
  sequential = "new SeqAction",
  trajectory = "new PathAction",
  custom = function(name) return "new " .. name end
};

exportAction(tree, cfg);
