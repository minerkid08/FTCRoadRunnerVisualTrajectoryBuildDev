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
    start = "path.chain()",
    build = ":build()",
    curve = {
      {
        str = ":add(path.curve3(%.2f, %.2f, %.2f, %.2f, %.2f, %.2f))",
        args = { "x1", "y1", "x2", "y2", "x3", "y3" }
      },
      {
        str = ":add(path.curve4(%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f))",
        args = { "x1", "y1", "x2", "y2", "x3", "y3", "x4", "y4"}
      }
    },
    line = {
      str = ":add(path.line(%.2f, %.2f, %.2f, %.2f))",
      args = { "x1", "y1", "x2", "y2" }
    },
    constantHeading = {
      str = ":constantHeading(%.2f)",
      args = { "h" }
    },
    linearHeading = {
      str = ":linearHeading(%.2f, %.2f)",
      args = { "h1", "h2" }
    }
  },
  parallel = "ParallelAction.new",
  sequential = "SeqAction.new",
  trajectory = "PathAction.new",
  custom = function(name) return name .. ".new" end
};

exportAction(tree, cfg);
