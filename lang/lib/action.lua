---@param parent Action
function processAction(parent)
  if (parent.actions == -1) then
    return
  end

  parent.tree = {};
  local c = actions[parent.actions];
  while (c ~= nil) do
    table.insert(parent.tree, c);
    processAction(c);
    c = actions[c.next];
  end
end

