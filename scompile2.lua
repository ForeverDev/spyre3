-- INPUT: Spyre ASM file name
-- OUTPUT: Spyre bytecode file


local compile = {}

compile.registers = {
	["rip"] = 0x00;
	["rsp"] = 0x01;
	["rbp"] = 0x02;
	["rax"] = 0x03;
	["rbx"] = 0x04;
	["rcx"] = 0x05;
	["rdx"] = 0x06;
	["rex"] = 0x07;
	["rfx"] = 0x08;
	["rgx"] = 0x09;
	["rhx"] = 0x0a;
	["rix"] = 0x0b;
	["rjx"]	= 0x0c;
}

compile.modes = {
	[0] = {};
	[1] = {"reg"};
	[2] = {"reg", "f64"};
	[3] = {"reg", "reg"};
	[4] = {"reg", "reg", "u64"};
	[5] = {"reg", "u64", "f64"};
	[6] = {"reg", "u64", "reg"};
	[7] = {"u64"};
}

compile.lookup = {
	["null"]	= 0x00;
	["exit"]	= 0x01;
	["ret"]		= 0x02;

	["mov"]		= 0x20;
	["add"]		= 0x21;
	["sub"]		= 0x22;
	["mul"]		= 0x23;
	["div"]		= 0x24;
	["neg"]		= 0x25;
	["or"]		= 0x26;
	["and"]		= 0x27;
	["xor"]		= 0x28;
	["not"]		= 0x29;
	["shl"]		= 0x2a;
	["shr"]		= 0x2b;
	["lt"]		= 0x2c;
	["le"]		= 0x2d;
	["gt"]		= 0x2e;
	["ge"]		= 0x2f;
	["cmp"]		= 0x30;
	["land"]	= 0x31;
	["lor"]		= 0x32;
	["lnot"]	= 0x33;

	["push"]	= 0x40;
	["pop"]		= 0x41;

	["call"]	= 0x60;
	["ccall"]	= 0x61;
	["jmp"]		= 0x62;
	["jit"]		= 0x63;
	["jif"]		= 0x64;
}

function compile.new(tokens)

	local self 			= setmetatable({}, {__index = compile})
	self.tokens			= tokens
	self.bytecode		= {}
	self.labels			= {}

	return self

end

function compile:doesMatch(tokens, syntax)
	if #syntax == 0 and #tokens == 0 then
		return true
	end
	if #tokens ~= #syntax then
		return false
	end
	for i, v in ipairs(tokens) do
		local s = syntax[i]
		if s == "reg" and not compile.registers[v.word] then
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
	-- parse the ROM section
	local rom = {}
	local i = 1
	while i <= #self.tokens and self.tokens[i].word ~= "__CODE__" do
		i = i + 1
	end
	for i, v in ipairs(self.tokens) do
		if v.word == "let" then
			local val = self.tokens[i + 2]
			local ptr = #rom 
			if val.typeof == "STRING" then
				-- TODO write chars not doubles
				for j = 1, val.word:len() do
					table.insert(rom, string.byte(val.word:sub(j, j)))
				end
				table.insert(rom, 0)
			end
			for j = i + 3, #self.tokens do
				if self.tokens[j].word == self.tokens[i + 1].word then
					self.tokens[j] = {
						typeof = "NUMBER";
						word = tostring(ptr);
					}
				end
			end
		end
	end
	for j = i, 1, -1 do
		table.remove(self.tokens, j)
	end
	-- preprocess the lables
	local c = 0
	local torem = {}
	for j, v in ipairs(self.tokens) do
		-- TODO fix placement
		if v.typeof == "IDENTIFIER" and self.tokens[j + 1] and self.tokens[j + 1].typeof == "COLON" then
			self.labels[v.word] = c
			table.insert(torem, j)
			table.insert(torem, j + 1)
		else
			if tonumber(v.word) then
				c = c + 8
			elseif compile.lookup[v.word] then
				c = c + 2
			elseif compile.registers[v.word] then
				c = c + 1
			elseif v.typeof == "IDENTIFIER" then
				c = c + 8
			end
		end
	end
	for j, v in ipairs(self.tokens) do
		if v.typeof == "IDENTIFIER" and not compile.lookup[v.word] and not compile.registers[v.word] then
			if not self.tokens[j + 1] or (self.tokens[j + 1] and self.tokens[j + 1].typeof ~= "COLON") then
				self.tokens[j] = {
					typeof = "NUMBER";
					word = tostring(self.labels[v.word]);
				}
				if not self.labels[v.word] then
					print("unknown label", v.word)
					os.exit(1)
				end
			end
		end
	end
	table.sort(torem, function(a, b) return a > b end)
	for i, v in ipairs(torem) do
		table.remove(self.tokens, v)
	end
	-- write the file headers into the self.bytecode table
	-- data start header
	self:writeFormatted("<I4", 8)
	-- code start header
	self:writeFormatted("<I4", 8 + #rom)
	-- write ROM
	for i, v in ipairs(rom) do
		table.insert(self.bytecode, v)
	end
	-- now handle the instructions
	local i = 1
	while i <= #self.tokens do
		local t = self.tokens[i]
		-- we found an opcode
		if t.typeof == "IDENTIFIER" and compile.lookup[t.word] then
			local inrow = {}
			do
				i = i + 1
				while i <= #self.tokens and not compile.lookup[self.tokens[i].word] do
					table.insert(inrow, self.tokens[i])
					i = i + 1
				end
				i = i - 1
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
			elseif self:doesMatch(inrow, compile.modes[7]) then
				table.insert(self.bytecode, 0x07)
				self:writeFormatted("d", tonumber(inrow[1].word))
			else
				print("not matched")
			end
		end
		i = i + 1
	end
end

function compile:dump()
	local i = 1
	while i <= #self.tokens do
		if compile.lookup[self.tokens[i].word] then
			i = i + 1
			io.write(self.tokens[i - 1].word .. " ")
			while not compile.lookup[self.tokens[i].word] do
				if self.tokens[i].word:match("[%[,%d]+") then
					io.write(string.char(8))
				end
				io.write(self.tokens[i].word .. " ")
				i = i + 1
				if i > #self.tokens then
					break
				end
			end
			print()
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

function scompile2_main(inputfn)

	local input = io.open(inputfn, "rb")
	local contents = input:read("*all")
	input:close()

	local tokens = dofile("slex2.lua")(contents)
	local compile_state = compile.new(tokens)
	compile_state:convert()

	local output = io.open(inputfn:match("(.+)%..-$") .. ".spyb", "w")
	output:write(compile_state:getBytecode())
	output:close()

end