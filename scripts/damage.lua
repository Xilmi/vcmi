local Base = require("unitEffect")
local Script = setmetatable({}, {__index = Base})
Script.__index = Script

function Script:sumBonusVal(unit, filterFn)
	local total = 0
	local list = unit:getBonuses(filterFn)
	for i = 1, list:size() do
		total = total + list:getBonus(i):getVal()
	end
	return total
end

function Script:isReceptive(mechanics, unit)
	local spell = mechanics:getSpell()
	if spell:isMagical() then
		if self:sumBonusVal(unit, function(b)
			return b:getType() == "SPELL_DAMAGE_REDUCTION" and b:getSubtype() == "any"
		end) >= 100 then
			return false
		end
	end
	for _, school in ipairs(spell:getSchools()) do
		if self:sumBonusVal(unit, function(b)
			return b:getType() == "SPELL_DAMAGE_REDUCTION" and b:getSubtype() == school
		end) >= 100 then
			return false
		end
	end
	return Base.isReceptive(self, mechanics, unit)
end

function Script:damageForTarget(targetIndex, mechanics, unit)
	local base
	if self.killByPercentage then
		local toKill = math.floor(unit:getCount() * mechanics:getEffectValue() / 100)
		base = toKill * unit:getMaxHealth()
	elseif self.killByCount then
		base = mechanics:getEffectValue() * unit:getMaxHealth()
	else
		base = mechanics:adjustEffectValue(unit)
	end
	local chainLength = self.chainLength or 0
	if chainLength > 1 and targetIndex > 0 then
		base = math.floor((self.chainFactor ^ targetIndex) * base)
	end
	return base
end

function Script:getHealthChange(mechanics, spellTarget)
	local result = { hpDelta = 0, unitsDelta = 0, unitType = -1 }
	for i, dest in ipairs(spellTarget) do
		local unit = dest.unit
		if unit and unit:isAlive() then
			local amount = self:damageForTarget(i - 1, mechanics, unit)
			local copy = unit:copy()
			local hpBefore    = copy:getAvailableHealth()
			local countBefore = copy:getCount()
			copy:damage(amount)
			result.hpDelta    = result.hpDelta    - (hpBefore    - copy:getAvailableHealth())
			result.unitsDelta = result.unitsDelta - (countBefore - copy:getCount())
		end
	end
	return result
end

function Script:apply(mechanics, server, target)
	local battleID = mechanics:getBattleID()
	local describe = server:describeChanges()
	local firstUnit, totalDamage, totalKilled, multiple = nil, 0, 0, false

	for i, dest in ipairs(target) do
		local unit = dest.unit
		if unit and unit:isAlive() then
			local amount = self:damageForTarget(i - 1, mechanics, unit)
			local dmg, killed = server:damageUnit(battleID, unit, amount)
			if describe then
				if firstUnit then multiple = true else firstUnit = unit end
				totalDamage = totalDamage + dmg
				totalKilled = totalKilled + killed
			end
		end
	end

	if describe and firstUnit and totalDamage > 0 then
		self:describeEffect(server, battleID, mechanics, firstUnit, totalKilled, totalDamage, multiple)
	end
end

function Script:describeEffect(server, battleID, mechanics, firstUnit, kills, damage, multiple)
	local spell    = mechanics:getSpell()
	local spellKey = spell:getJsonKey()

	if spellKey:find("deathStare") and not multiple then
		local casterNameID = mechanics:getCasterNameTextID()
		if kills > 1 then
			server:appendLog(battleID, {
				append  = { "core.genrltxt.119" },
				replace = { kills, firstUnit:getCreature():getNamePluralTextID(), casterNameID }
			})
		else
			server:appendLog(battleID, {
				append  = { "core.genrltxt.118" },
				replace = { firstUnit:getCreature():getNameSingularTextID(), casterNameID }
			})
		end

	elseif spellKey:find("accurateShot") and not multiple then
		local textID = mechanics:getPluralFormTextID(
			"vcmi.battleWindow.accurateShot.resultDescription", kills)
		server:appendLog(battleID, {
			append  = { textID },
			replace = { kills, firstUnit:getCreature():getNameTextID(kills) }
		})

	elseif spellKey:find("thunderbolt") and not multiple then
		server:appendLog(battleID, {
			append  = { "core.genrltxt.367" },
			replace = { firstUnit:getCreature():getNamePluralTextID() }
		})
		server:appendLog(battleID, {
			append  = { "core.genrltxt.343" },
			replace = { damage }
		})

	else
		server:appendLog(battleID, {
			append  = { "core.genrltxt.376" },
			replace = { spell:getNameTextID(), damage }
		})
		if kills > 0 then
			if kills > 1 then
				server:appendLog(battleID, {
					append  = { "core.genrltxt.379" },
					replace = { kills, multiple and "core.genrltxt.43"
					                            or firstUnit:getCreature():getNamePluralTextID() }
				})
			else
				server:appendLog(battleID, {
					append  = { "core.genrltxt.378" },
					replace = { multiple and "core.genrltxt.42"
					                      or firstUnit:getCreature():getNameSingularTextID() }
				})
			end
		end
	end
end

return Script
