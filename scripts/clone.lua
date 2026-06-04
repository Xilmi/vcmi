local Base = require("unitEffect")
local Script = setmetatable({}, {__index = Base})
Script.__index = Script

function Script:isReceptive(mechanics, unit)
	if unit:getCreature():getLevel() > self.maxTier then return false end
	return Base.isReceptive(self, mechanics, unit)
end

function Script:isValidTarget(mechanics, unit)
	if unit:isClone()  then return false end
	if unit:hasClone() then return false end
	return unit:isValidTarget(false)
end

function Script:apply(mechanics, server, target)
	local battleID = mechanics:getBattleID()
	for _, dest in ipairs(target) do
		local unit = dest.unit
		if unit == nil then goto continue end
		if unit:getCount() < 1 then goto continue end

		local creature = unit:getCreature()
		local hex = mechanics:getBattle():getAvailableHex(
			creature,
			mechanics:getCasterSide(),
			unit:getPosition():toInteger()
		)
		if not hex:isValid() then break end

		local cloneId = mechanics:getBattle():getNextUnitId()
		server:createUnit(battleID, cloneId, {
			count    = unit:getCount(),
			type     = creature:getJsonKey(),
			side     = mechanics:getCasterSide(),
			position = hex:toInteger(),
			summoned = true
		})

		local cloneUnit = mechanics:getBattle():getUnitById(cloneId)
		if cloneUnit == nil then break end

		local cloneState = cloneUnit:copy()
		cloneState:setCloned(true)
		server:changeUnit(battleID, cloneState)

		local originalState = unit:copy()
		originalState:setCloneID(cloneId)
		server:changeUnit(battleID, originalState)

		server:addUnitBonus(battleID, cloneId, {
			duration   = ENUM.BonusDuration.nTurns,
			type       = "NONE",
			sourceType = ENUM.BonusSource.spellEffect,
			val        = 0,
			sourceID   = mechanics:getSpell():getJsonKey(),
			turns      = mechanics:getEffectDuration()
		})

		::continue::
	end
end

return Script
