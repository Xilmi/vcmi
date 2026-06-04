local Base = require("unitEffect")
local Script = setmetatable({}, {__index = Base})
Script.__index = Script

function Script:deepCopyBonus(b)
	local copy = {}
	for k, v in pairs(b) do
		copy[k] = v
	end
	return copy
end

function Script:convertBonuses(mechanics)
	local duration = mechanics:getEffectDuration()
	local spellKey = mechanics:getSpell():getJsonKey()
	local converted = {}

	for name, b in pairs(self.bonus or {}) do
		local nb = self:deepCopyBonus(b)

		if not nb.turns or nb.turns == 0 then
			nb.turns = duration
		end

		nb.sourceType = "SPELL_EFFECT"
		nb.sourceID = spellKey

		-- Shield/AirShield: val in config is the reduction percentage; stored inverted
		if (spellKey == "core:shield" or spellKey == "core:airShield")
			and nb.type == "GENERAL_DAMAGE_REDUCTION" then
			nb.val = 100 - (nb.val or 0)
		end

		-- Bind: store caster unit ID when cast by a unit (not a hero)
		if spellKey == "core:bind" and nb.type == "BIND_EFFECT" then
			if not mechanics:getHeroCaster() then
				nb.addInfo = mechanics:getUnitCaster():unitId()
			end
		end

		converted[name] = nb
	end

	return converted
end

function Script:applyHeroSpecialty(mechanics, buffer, unit)
	local hero = mechanics:getHeroCaster()
	if not hero then return end

	local spellKey = mechanics:getSpell():getJsonKey()
	local tier = math.max(unit:creatureLevel(), 1)

	local peculiar = hero:getBonuses(function(b)
		return b:getType() == "SPECIAL_PECULIAR_ENCHANT" and b:getSubtype() == spellKey
	end)
	if peculiar:size() > 0 then
		local mode = peculiar:getBonus(1):getParametersAsNumber()
		local power = 0
		if mode == 0 then
			if tier <= 2 then power = 3
			elseif tier <= 4 then power = 2
			elseif tier <= 6 then power = 1
			end
		end
		if mechanics:isNegative() then power = -power end
		if power ~= 0 then
			for _, nb in pairs(buffer) do
				nb.val = (nb.val or 0) + power
			end
		end
	end

	local addVal = hero:getBonuses(function(b)
		return b:getType() == "SPECIAL_ADD_VALUE_ENCHANT" and b:getSubtype() == spellKey
	end)
	if addVal:size() > 0 then
		local addAmount = addVal:getBonus(1):getParametersAsNumber()
		for _, nb in pairs(buffer) do
			nb.val = (nb.val or 0) + addAmount
		end
	end

	local fixedVal = hero:getBonuses(function(b)
		return b:getType() == "SPECIAL_FIXED_VALUE_ENCHANT" and b:getSubtype() == spellKey
	end)
	if fixedVal:size() > 0 then
		local fixedAmount = fixedVal:getBonus(1):getParametersAsNumber()
		for _, nb in pairs(buffer) do
			nb.val = fixedAmount
		end
	end
end

function Script:describeEffect(server, battleID, unit, bonuses, singular, plural)
	-- Age spell: STACK_HEALTH bonus with negative val gets a custom message
	for _, nb in pairs(bonuses) do
		if nb.type == "STACK_HEALTH" and (nb.val or 0) < 0 then
			local healthBonuses = unit:getBonuses(function(b)
				return b:getType() == "STACK_HEALTH"
			end)
			local oldHealth = 0
			for i = 1, healthBonuses:size() do
				oldHealth = oldHealth + healthBonuses:getBonus(i):getVal()
			end
			local lost = oldHealth - (oldHealth + nb.val)
			local ageTextID = unit:getCount() == 1
				and "core.genrltxt.551"
				or  "core.genrltxt.552"
			server:appendLog(battleID, {
				append  = { ageTextID },
				replace = { unit:getCreature():getNameTextID(unit:getCount()), lost }
			})
			return
		end
	end

	if not plural or plural == "" then return end

	local textID = (singular and singular ~= "" and unit:getCount() == 1) and singular or plural
	local nameTextID = unit:getCreature():getNameTextID(unit:getCount())
	server:appendLog(battleID, {
		append  = { textID },
		replace = { nameTextID }
	})
end

function Script:apply(mechanics, server, target)
	local battleID = mechanics:getBattleID()
	local describe = server:describeChanges()
	local converted = self:convertBonuses(mechanics)

	local singular, plural
	if self.battleLogMessage then
		local s = self.battleLogMessage.singular
		local p = self.battleLogMessage.plural
		singular = (s and #s > 1 and s:sub(1, 1) == "@") and s:sub(2) or nil
		plural   = (p and #p > 1 and p:sub(1, 1) == "@") and p:sub(2) or nil
	end

	for _, dest in ipairs(target) do
		local unit = dest.unit
		if not unit or not unit:isAlive() then goto continue end

		local buffer = {}
		for name, nb in pairs(converted) do
			buffer[name] = self:deepCopyBonus(nb)
		end

		self:applyHeroSpecialty(mechanics, buffer, unit)

		if describe then
			self:describeEffect(server, battleID, unit, buffer, singular, plural)
		end

		server:applyUnitBonuses(battleID, unit, buffer, self.cumulative or false)

		::continue::
	end
end

return Script
