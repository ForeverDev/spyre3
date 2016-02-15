-- INPUT: Spyre ASM file contents
-- OUTPUT: Tokens

local lex = {}

lex.operators = {
    -- operators
    ["+"]           = "PLUS";
    ["-"]           = "MINUS";
    ["["]           = "OPENSQ";
    ["]"]           = "CLOSESQ";
    [":"]           = "COLON";
    [","]           = "COMMA";
}

function lex.new(contents)

	local self 		= setmetatable({}, {__index = lex})
	self.contents 	= contents
	self.tokens 	= {}
	self.index		= 1
	
	return self

end

function lex:getchar()
	return self.contents:sub(self.index, self.index)
end

function lex:inc(i)
	self.index = self.index + (i or 1)
end

function lex:dec(i)
	self.index = self.index - (i or 1)
end	

function lex:space()
	return self.index <= #self.contents
end

function lex:peek(i)
	return self.contents:sub(self.index + i, self.index + i)
end

function lex:pushtoken(t)
	local typeof, word;
	word = t
	if t:sub(1, 1) == "\"" and t:sub(-1, -1) == "\"" then
		typeof = "STRING"
		word = t:sub(2, -2)
	elseif tonumber(t) then
		typeof = "NUMBER"
	elseif lex.operators[t] then
		typeof = lex.operators[t]
	else
		typeof = "IDENTIFIER"
	end
	table.insert(self.tokens, {
		typeof = typeof;
		word = word;
	})
end

function lex:convert()
    while self:space() do
        local c = self:getchar()
		local n = self:peek(1)
        if c ~= " " then
            if c == ";" then
                self:inc()
                while self:space() and self:getchar() ~= "\n" do
                    self:inc()
                end
                self:dec()
            -- string
            elseif c == "\"" then
                local str = c
                self:inc()
                c = self:getchar()
				if c == "\"" then
					self:inc()
				else
					while c ~= "\"" do
						c = self:getchar()
						if c == "\\" then
							self:inc()
							local n = self:getchar()
							if n == "n" then
								c = "\n"
							elseif n == "t" then
								c = "\t"
							end
						end
						str = str .. c
						self:inc()
					end
				end
                self:pushtoken(str)
                self:dec()
            -- number
            elseif c:match("%d") then
                local num = ""
                while c:match("%d") or c == "." or c == "e" do
                    num = num .. c
                    self:inc()
                    c = self:getchar()
                end
				if self.tokens[#self.tokens - 1] then
					local t0 = self.tokens[#self.tokens].typeof
					local t1 = self.tokens[#self.tokens - 1].typeof
					if t0 == "MINUS" and t1 ~= "IDENTIFIER" and t1 ~= "NUMBER" then
						num = "-" .. num
						table.remove(self.tokens, #self.tokens)
					end
				end
                self:pushtoken(num)
                self:dec()
            -- operator
            elseif c:match("%p") and c ~= "_" then
                local nxt = self:peek(1)
                if lex.operators[c .. nxt] then
                    -- found double operator
                    self:pushtoken(c .. nxt)
                    self:inc()
                else
                    self:pushtoken(c)
                end
            -- keyword or identifier
            else
                local id = ""
                while (c:match("%a") or c == "_" or c:match("%d")) and c ~= " " do
                    id = id .. c
                    self:inc()
                    c = self:getchar()
                end
                if id ~= "" then
                    self:pushtoken(id)
                    self:dec()
                end
            end
        end
        self:inc()
	end
end

return function(contents)

	local lex_state = lex.new(contents)
	lex_state:convert()

	return lex_state.tokens

end
