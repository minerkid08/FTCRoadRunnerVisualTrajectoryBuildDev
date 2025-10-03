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

---@class FmtStr
---@field str string
---@field args string[]

---@class TrajConfigPedro
---@field start string
---@field line FmtStr
---@field curve FmtStr[]?
---@field curveFun fun(points: Vec2[], indent: string)?
---@field linearHeading FmtStr
---@field constantHeading FmtStr
---@field build string?

---@class TrajConfigRR
---@field start FmtStr
---@field setTan FmtStr
---@field splineTo FmtStr
---@field splineLinear FmtStr
---@field splineConstant FmtStr
---@field build string?

---@class Config
---@field pedro TrajConfigPedro?
---@field rr TrajConfigRR?
---@field trajectory string
---@field sequential string
---@field parallel string
---@field custom fun(name: string): string
