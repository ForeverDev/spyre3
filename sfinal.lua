-- this program converts a Spyre assembly file into a Spyre bytecode file

local compile = {}

compile.registers = {
	["ip"] = 0x00;
	["sp"] = 0x01;
	["bp"] = 0x02;
	["r0"] = 0x03;
	["r1"] = 0x04;
	["r2"] = 0x05;
	["r3"] = 0x06;
	["r4"] = 0x07;
	["r5"] = 0x08;
	["r6"] = 0x09;
	["r7"] = 0x0a;
	["r8"] = 0x0b;
}

compile.modes = {
	[0] = {};
	[1] = {"reg"};
	[2] = {"reg", "f64"};
	[3] = {"reg", "reg"};
	[4] = {"reg", "reg", "u64"};
	[5] = {"reg", "u64", "f64"};
	[6] = {"reg", "u64", "reg"};
}

compile.lookup = {
	["null"]	= 0x00;

	["mov"]		= 0x20;
	["add"]		= 0x21;
	["sub"]		= 0x22;
	["mul"]		= 0x23;
	["div"]		= 0x24;
	["or"]		= 0x25;
	["and"]		= 0x26;
	["xor"]		= 0x27;
	["shl"]		= 0x28;
	["shr"]		= 0x29;
	["cmp"]		= 0x2a;
	["gt"]		= 0x2b;
	["ge"]		= 0x2c;
	["lt"]		= 0x2d;
	["le"]		= 0x2e;

	["push"]	= 0x40;
	["pop"]		= 0x41;
}

function compile.new(tokens)

	local self 			= setmetatable({}, {__index = compile})
	self.tokens			= tokens
	self.bytecode		= {}

	return self

end

function compile:doesMatch(tokens, syntax)
	if #tokens ~= #syntax then
		return false
	end
	for i, v in ipairs(tokens) do
		local s = syntax[i]
		if s == "reg" and not (v.word:match("r%d") or v.word == "ip" or v.word == "sp" or v.word == "bp") then
			return false
		elseif (s == "u64" or s == "f64") and v.typeof ~= "NUMBER" then
			return false
		end
	end	
	return true
end

function compile:writeFormatted(format, val)
	local f = string.pack(format, val)
	for i = 1, f:len() do
		table.insert(self.bytecode, string.byte(f:sub(i, i)))
	end
end

function compile:convert()
	local i = 1
	while i <= #self.tokens do
		local t = self.tokens[i]
		-- we found an opcode
		if t.typeof == "IDENTIFIER" and compile.lookup[t.word] then
			local inrow = {}
			do
				local j = i + 1
				while j <= #self.tokens and not compile.lookup[self.tokens[j].word] do
					table.insert(inrow, self.tokens[j])
					j = j + 1
					i = i + 1
				end
			end
			local add = 1
			for j = #inrow, 1, -1 do
				if inrow[j].typeof ~= "NUMBER" and inrow[j].typeof ~= "IDENTIFIER" then
					if inrow[j].typeof == "MINUS" then
						add = -1
					end
					table.remove(inrow, j)
				end
			end
			table.insert(self.bytecode, compile.lookup[t.word])
			if self:doesMatch(inrow, compile.modes[0]) then
				table.insert(self.bytecode, 0x00)
			elseif self:doesMatch(inrow, compile.modes[1]) then
				table.insert(self.bytecode, 0x01)
				table.insert(self.bytecode, compile.registers[inrow[1].word])
			elseif self:doesMatch(inrow, compile.modes[2]) then
				table.insert(self.bytecode, 0x02)
				table.insert(self.bytecode, compile.registers[inrow[1].word])
				self:writeFormatted("d", tonumber(inrow[2].word))
			elseif self:doesMatch(inrow, compile.modes[3]) then
				table.insert(self.bytecode, 0x03)
				table.insert(self.bytecode, compile.registers[inrow[1].word])
				table.insert(self.bytecode, compile.registers[inrow[2].word])
			elseif self:doesMatch(inrow, compile.modes[4]) then
				table.insert(self.bytecode, 0x04)
				table.insert(self.bytecode, compile.registers[inrow[1].word])
				table.insert(self.bytecode, compile.registers[inrow[2].word])
				self:writeFormatted("d", tonumber(inrow[3].word) * add)
			elseif self:doesMatch(inrow, compile.modes[5]) then
				table.insert(self.bytecode, 0x05)
				table.insert(self.bytecode, compile.registers[inrow[1].word])
				self:writeFormatted("d", tonumber(inrow[2].word) * add)
				self:writeFormatted("d", tonumber(inrow[3].word))
			elseif self:doesMatch(inrow, compile.modes[6]) then
				table.insert(self.bytecode, 0x06)
				table.insert(self.bytecode, compile.registers[inrow[1].word])
				self:writeFormatted("d", tonumber(inrow[2].word) * add)
				table.insert(self.bytecode, compile.registers[inrow[3].word])	
			end	
		end
		i = i + 1
	end
end

function compile:getBytecode()
	local b = ""
	for i, v in ipairs(self.bytecode) do
		b = b .. string.char(v)
	end
	return b
end

function sfinal_main(inputfn)

	local input = io.open(inputfn, "rb")
	local contents = input:read("*all")
	input:close()

	local tokens = dofile("slex.lua")(contents)
	local compile_state = compile.new(tokens)
	compile_state:convert()

	local output = io.open(inputfn:match("(.+)%..-$") .. ".spyb", "w")
	output:write(compile_state:getBytecode())
	output:close()

end
