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

---@param action Action
---@param config Config
---@param indent string?
---@param trailingComma boolean?
function exportAction(action, config, indent, trailingComma)
	indent = indent or "";
	if (action.name == "sequential") then
		if (action.label == "" or config.sequentialLabel == nil) then
			file.write(indent .. config.sequential .. "(\n");
		else
			file.write(indent .. config.sequentialLabel .. "(\n");
			file.write(indent .. "  \"" .. action.label .. "\",\n");
		end
	elseif (action.name == "parallel") then
		file.write(indent .. config.parallel .. "(\n");
	elseif (action.name == "trajectory") then
		file.write(indent .. config.trajectory .. "(\n");
		if (rr == nil) then
			if (config.pedro == nil) then
				log.error("export failed: exporting to pedro pating is not supported by this language");
			end
			exportTrajectoryPedro(action.trajectory, indent .. "    ", config.pedro);
		else
			if (config.rr == nil) then
				log.error("export failed: exporting to roadrunner is not supported by this language");
			end
			printTrajectoryRR(action.trajectory, indent .. "    ");
		end
	end
	if (action.tree ~= nil) then
		local i = #action.tree;
		for k, v in ipairs(action.tree) do
			exportAction(v, config, indent .. "  ", i > k);
		end
	end
	if (action.name == "sequential" or action.name == "parallel" or action.name == "trajectory") then
		if (trailingComma) then
			file.write(indent .. "),\n");
		else
			file.write(indent .. ")\n");
		end
	else
		file.write(indent .. config.custom(action.name) .. "(");
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
	if (indent == "") then
		log.info("export sucessful");
	end
end
