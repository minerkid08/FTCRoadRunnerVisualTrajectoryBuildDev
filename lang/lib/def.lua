---@class Action
---@field name string
---@field next integer
---@field prev integer
---@field actions integer
---@field parent integer
---@field tree Action[]

---@class Trajectory
---@field nodes Node[]
---@field segments Segment[]

---@class Node
---@field x number
---@field y number
---@field heading number

---@class Segment
---@field startNode integer
---@field endNode integer
---@field heading HeadingMode
---@field startTan number 
---@field endTan number 
---@field controlPoints Vec2[]

---@class Vec2
---@field x number
---@field y number
